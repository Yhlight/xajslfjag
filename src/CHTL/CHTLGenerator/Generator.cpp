#include "Generator.h"
#include <algorithm>
#include <unordered_set>

namespace CHTL {

// 自闭合标签列表
static const std::unordered_set<std::string> voidElements = {
    "area", "base", "br", "col", "embed", "hr", "img", "input",
    "link", "meta", "param", "source", "track", "wbr"
};

Generator::Generator(std::shared_ptr<CompileContext> context,
                     const GeneratorConfig& config)
    : context_(context), config_(config) {}

std::string Generator::generate(std::shared_ptr<ProgramNode> program) {
    output_.str("");
    globalStyles_.str("");
    globalScripts_.str("");
    indentLevel_ = 0;
    
    // 访问AST
    program->accept(this);
    
    // 构建最终输出
    std::stringstream finalOutput;
    
    // 检查是否需要HTML5声明
    auto useStmt = program->getUseStatement();
    if (useStmt && useStmt->getType() == NodeType::USE_OP) {
        auto useNode = static_cast<UseNode*>(useStmt.get());
        if (useNode->getTarget() == "html5") {
            generateHtml5Doctype();
        }
    }
    
    // 输出主内容
    finalOutput << output_.str();
    
    // 插入全局样式（如果有）
    if (!globalStyles_.str().empty()) {
        std::string content = finalOutput.str();
        size_t headPos = content.find("</head>");
        if (headPos != std::string::npos) {
            content.insert(headPos, globalStyles_.str());
        } else {
            // 如果没有head标签，在开头插入
            content = globalStyles_.str() + content;
        }
        finalOutput.str(content);
    }
    
    // 插入全局脚本（如果有）
    if (!globalScripts_.str().empty()) {
        std::string content = finalOutput.str();
        size_t bodyPos = content.find("</body>");
        if (bodyPos != std::string::npos) {
            content.insert(bodyPos, globalScripts_.str());
        } else {
            // 如果没有body标签，在末尾插入
            content += globalScripts_.str();
        }
        finalOutput.str(content);
    }
    
    return finalOutput.str();
}

void Generator::visitProgramNode(ProgramNode* node) {
    for (const auto& child : node->getTopLevelNodes()) {
        child->accept(this);
    }
}

void Generator::visitElementNode(ElementNode* node) {
    pushState();
    currentState_.currentElement = node->getTagName();
    
    // 处理选择器自动化
    applyAutoClasses(node);
    applyAutoIds(node);
    
    // 生成HTML元素
    generateHtmlElement(node);
    
    popState();
}

void Generator::generateHtmlElement(ElementNode* node) {
    const std::string& tagName = node->getTagName();
    
    // 开始标签
    write("<" + tagName);
    generateAttributes(node);
    
    if (isVoidElement(tagName)) {
        writeLine(config_.minify ? "/>" : " />");
    } else {
        write(">");
        
        // 子节点
        bool hasChildren = !node->getChildNodes().empty();
        if (hasChildren && !config_.minify) {
            writeLine();
            indent();
        }
        
        for (const auto& child : node->getChildNodes()) {
            child->accept(this);
        }
        
        if (hasChildren && !config_.minify) {
            dedent();
        }
        
        // 结束标签
        writeLine("</" + tagName + ">");
    }
}

void Generator::generateAttributes(ElementNode* node) {
    for (const auto& [name, value] : node->getAttributes()) {
        write(" " + name + "=\"" + escapeHtml(value) + "\"");
    }
}

void Generator::visitTextNode(TextNode* node) {
    const std::string& content = node->getContent();
    if (!content.empty()) {
        writeLine(escapeHtml(content));
    }
}

void Generator::visitAttributeNode(AttributeNode* node) {
    // 属性节点在ElementNode中处理
}

void Generator::visitCommentNode(CommentNode* node) {
    if (!config_.generateComments && 
        node->getCommentType() != CommentType::GENERATOR) {
        return;
    }
    
    const std::string& content = node->getContent();
    
    if (currentState_.inStyleBlock) {
        generateCssComment(content);
    } else if (currentState_.inScriptBlock) {
        generateJsComment(content);
    } else {
        generateHtmlComment(content);
    }
}

void Generator::visitStyleNode(StyleNode* node) {
    pushState();
    currentState_.inStyleBlock = true;
    currentState_.inLocalStyle = (node->getBlockType() == StyleBlockType::LOCAL);
    
    if (node->getBlockType() == StyleBlockType::LOCAL) {
        // 局部样式块处理
        processAutoSelectors(node);
        generateLocalStyles(node);
    } else {
        // 全局样式块
        writeLine("<style>");
        indent();
        
        for (const auto& rule : node->getRules()) {
            rule->accept(this);
        }
        
        dedent();
        writeLine("</style>");
    }
    
    popState();
}

void Generator::generateLocalStyles(StyleNode* node) {
    // 收集内联样式和选择器样式
    std::stringstream inlineStyles;
    std::stringstream selectorStyles;
    
    for (const auto& rule : node->getRules()) {
        if (rule->getType() == NodeType::PROPERTY) {
            // 内联样式
            auto prop = static_cast<PropertyNode*>(rule.get());
            inlineStyles << prop->getName() << ": " << prop->getValue() << "; ";
        } else if (rule->getType() == NodeType::SELECTOR) {
            // 选择器样式（添加到全局样式块）
            auto selector = static_cast<SelectorNode*>(rule.get());
            std::stringstream temp;
            std::swap(output_, temp);
            output_ = selectorStyles;
            
            generateSelector(selector);
            
            std::swap(output_, temp);
            globalStyles_ << selectorStyles.str();
        }
    }
    
    // 如果有内联样式，添加到当前元素
    if (!inlineStyles.str().empty()) {
        // 这里需要通过某种机制将内联样式添加到父元素
        // 暂时简化处理
    }
}

void Generator::visitScriptNode(ScriptNode* node) {
    pushState();
    currentState_.inScriptBlock = true;
    currentState_.inLocalScript = (node->getBlockType() == ScriptBlockType::LOCAL);
    
    if (node->getBlockType() == ScriptBlockType::LOCAL) {
        // 局部脚本添加到全局脚本
        globalScripts_ << node->getContent() << "\n";
    } else {
        // 全局脚本
        writeLine("<script>");
        write(node->getContent());
        writeLine("</script>");
    }
    
    popState();
}

void Generator::visitSelectorNode(SelectorNode* node) {
    std::string selector = node->getSelector();
    
    // 处理引用选择器 &
    if (node->getSelectorType() == SelectorNode::SelectorType::REFERENCE) {
        selector = processSelectorReference(selector);
    }
    
    writeLine(selector + " {");
    indent();
    
    if (node->getContent()) {
        node->getContent()->accept(this);
    }
    
    dedent();
    writeLine("}");
}

void Generator::visitPropertyNode(PropertyNode* node) {
    std::string value = node->getValue();
    
    // 处理变量组替换
    if (node->getVariableGroup().has_value()) {
        value = replaceVariables(value);
    }
    
    writeLine(node->getName() + ": " + value + ";");
}

void Generator::generateHtml5Doctype() {
    writeLine("<!DOCTYPE html>");
}

void Generator::generateHtmlComment(const std::string& comment) {
    writeLine("<!-- " + comment + " -->");
}

void Generator::generateCssComment(const std::string& comment) {
    writeLine("/* " + comment + " */");
}

void Generator::generateJsComment(const std::string& comment) {
    if (comment.find('\n') == std::string::npos) {
        writeLine("// " + comment);
    } else {
        writeLine("/* " + comment + " */");
    }
}

std::string Generator::processSelectorReference(const std::string& selector) {
    // 处理 & 引用选择器
    if (selector == "&") {
        // 优先使用类选择器
        if (!currentState_.autoAddedClasses.empty()) {
            return "." + currentState_.autoAddedClasses.front();
        } else if (!currentState_.autoAddedIds.empty()) {
            return "#" + currentState_.autoAddedIds.front();
        }
    }
    return selector;
}

void Generator::processAutoSelectors(StyleNode* node) {
    // 扫描样式规则，查找类和ID选择器
    for (const auto& rule : node->getRules()) {
        if (rule->getType() == NodeType::SELECTOR) {
            auto selector = static_cast<SelectorNode*>(rule.get());
            const std::string& sel = selector->getSelector();
            
            if (selector->getSelectorType() == SelectorNode::SelectorType::CLASS) {
                std::string className = sel.substr(1); // 去掉 .
                currentState_.autoAddedClasses.push_back(className);
            } else if (selector->getSelectorType() == SelectorNode::SelectorType::ID) {
                std::string idName = sel.substr(1); // 去掉 #
                currentState_.autoAddedIds.push_back(idName);
            }
        }
    }
}

void Generator::applyAutoClasses(ElementNode* element) {
    if (!currentState_.autoAddedClasses.empty() && 
        !element->getAttribute("class").has_value()) {
        // 自动添加第一个类选择器
        element->addAttribute("class", currentState_.autoAddedClasses.front());
    }
}

void Generator::applyAutoIds(ElementNode* element) {
    if (!currentState_.autoAddedIds.empty() && 
        !element->getAttribute("id").has_value()) {
        // 自动添加第一个ID选择器
        element->addAttribute("id", currentState_.autoAddedIds.front());
    }
}

std::string Generator::replaceVariables(const std::string& value) {
    // TODO: 实现变量替换逻辑
    return value;
}

void Generator::write(const std::string& text) {
    output_ << text;
}

void Generator::writeLine(const std::string& text) {
    if (!config_.minify) {
        output_ << getIndent();
    }
    output_ << text;
    if (!config_.minify) {
        output_ << config_.lineEnding;
    }
}

void Generator::indent() {
    indentLevel_++;
}

void Generator::dedent() {
    if (indentLevel_ > 0) {
        indentLevel_--;
    }
}

std::string Generator::getIndent() const {
    if (config_.minify) {
        return "";
    }
    return std::string(indentLevel_ * config_.indentSize, ' ');
}

void Generator::pushState() {
    stateStack_.push(currentState_);
    currentState_ = GeneratorState();
}

void Generator::popState() {
    if (!stateStack_.empty()) {
        currentState_ = stateStack_.top();
        stateStack_.pop();
    }
}

bool Generator::isVoidElement(const std::string& tagName) {
    return voidElements.find(tagName) != voidElements.end();
}

std::string Generator::escapeHtml(const std::string& text) {
    std::string result;
    for (char c : text) {
        switch (c) {
            case '<': result += "&lt;"; break;
            case '>': result += "&gt;"; break;
            case '&': result += "&amp;"; break;
            case '"': result += "&quot;"; break;
            case '\'': result += "&#39;"; break;
            default: result += c; break;
        }
    }
    return result;
}

std::string Generator::escapeCss(const std::string& text) {
    // CSS转义实现
    return text;
}

std::string Generator::escapeJs(const std::string& text) {
    // JavaScript转义实现
    return text;
}

// 其他访问者方法的空实现
void Generator::visitTemplateNode(TemplateNode* node) {}
void Generator::visitTemplateUseNode(TemplateUseNode* node) {}
void Generator::visitCustomNode(CustomNode* node) {}
void Generator::visitCustomUseNode(CustomUseNode* node) {}
void Generator::visitOriginNode(OriginNode* node) {}
void Generator::visitOriginUseNode(OriginUseNode* node) {}
void Generator::visitImportNode(ImportNode* node) {}
void Generator::visitConfigNode(ConfigNode* node) {}
void Generator::visitInfoNode(InfoNode* node) {}
void Generator::visitExportNode(ExportNode* node) {}
void Generator::visitNamespaceNode(NamespaceNode* node) {}
void Generator::visitFromNode(FromNode* node) {}
void Generator::visitDeleteNode(DeleteNode* node) {}
void Generator::visitInsertNode(InsertNode* node) {}
void Generator::visitInheritNode(InheritNode* node) {}
void Generator::visitExceptNode(ExceptNode* node) {}
void Generator::visitUseNode(UseNode* node) {}

} // namespace CHTL