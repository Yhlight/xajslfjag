#include "Generator.h"
#include <algorithm>
#include <unordered_set>
#include "../../Error/ErrorReport.h"

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
    
    // 遍历所有顶层节点
    for (const auto& node : program->getTopLevelNodes()) {
        if (node) {
            // Use dynamic dispatch based on node type
            switch (node->getType()) {
                case NodeType::ELEMENT:
                    visitElementNode(static_cast<ElementNode*>(node.get()));
                    break;
                case NodeType::TEXT:
                    visitTextNode(static_cast<TextNode*>(node.get()));
                    break;
                case NodeType::COMMENT:
                    visitCommentNode(static_cast<CommentNode*>(node.get()));
                    break;
                case NodeType::TEMPLATE:
                    visitTemplateNode(static_cast<TemplateNode*>(node.get()));
                    break;
                case NodeType::CUSTOM:
                    visitCustomNode(static_cast<CustomNode*>(node.get()));
                    break;
                case NodeType::STYLE_BLOCK:
                    visitStyleNode(static_cast<StyleNode*>(node.get()));
                    break;
                case NodeType::SCRIPT_BLOCK:
                    visitScriptNode(static_cast<ScriptNode*>(node.get()));
                    break;
                case NodeType::ORIGIN:
                    visitOriginNode(static_cast<OriginNode*>(node.get()));
                    break;
                case NodeType::IMPORT:
                    visitImportNode(static_cast<ImportNode*>(node.get()));
                    break;
                case NodeType::CONFIGURATION:
                    visitConfigNode(static_cast<ConfigNode*>(node.get()));
                    break;
                case NodeType::NAMESPACE:
                    visitNamespaceNode(static_cast<NamespaceNode*>(node.get()));
                    break;
                default:
                    // Skip unknown node types
                    break;
            }
        }
    }
    
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
        std::string styleBlock = "<style>\n" + globalStyles_.str() + "</style>\n";
        std::string content = finalOutput.str();
        size_t headPos = content.find("</head>");
        if (headPos != std::string::npos) {
            content.insert(headPos, styleBlock);
        } else {
            // 如果没有head标签，在开头插入
            content = styleBlock + content;
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
    // TODO: 实现节点遍历
    (void)node->getTopLevelNodes();
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
    
    // 保存当前元素节点引用
    pushState();
    currentState_.currentElementNode = node;
    currentState_.currentElement = tagName;
    
    // 先扫描子节点，收集局部样式块的信息
    for (const auto& child : node->getChildNodes()) {
        if (child && child->getType() == NodeType::STYLE_BLOCK) {
            auto styleNode = static_cast<StyleNode*>(child.get());
            if (styleNode->getBlockType() == StyleBlockType::LOCAL) {
                // 处理自动选择器和收集内联样式
                processAutoSelectors(styleNode);
                
                // 收集内联样式
                for (const auto& rule : styleNode->getRules()) {
                    if (rule->getType() == NodeType::PROPERTY) {
                        auto prop = static_cast<PropertyNode*>(rule.get());
                        if (!currentState_.pendingInlineStyles.empty()) {
                            currentState_.pendingInlineStyles += " ";
                        }
                        currentState_.pendingInlineStyles += prop->getName() + ": " + prop->getValue() + ";";
                    }
                }
            }
        }
    }
    
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
        
        // 遍历子节点
        for (const auto& child : node->getChildNodes()) {
            if (child) {
                switch (child->getType()) {
                    case NodeType::ELEMENT:
                        visitElementNode(static_cast<ElementNode*>(child.get()));
                        break;
                    case NodeType::TEXT:
                        visitTextNode(static_cast<TextNode*>(child.get()));
                        break;
                    case NodeType::COMMENT:
                        visitCommentNode(static_cast<CommentNode*>(child.get()));
                        break;
                    case NodeType::STYLE_BLOCK:
                        visitStyleNode(static_cast<StyleNode*>(child.get()));
                        break;
                    case NodeType::SCRIPT_BLOCK:
                        visitScriptNode(static_cast<ScriptNode*>(child.get()));
                        break;
                    default:
                        // Skip other node types inside elements
                        break;
                }
            }
        }
        
        if (hasChildren && !config_.minify) {
            dedent();
        }
        
        // 结束标签
        writeLine("</" + tagName + ">");
    }
    
    popState();
}

void Generator::generateAttributes(ElementNode* node) {
    // 先输出普通属性
    bool hasStyleAttr = false;
    std::string existingStyle;
    
    for (const auto& [name, value] : node->getAttributes()) {
        if (name == "style") {
            hasStyleAttr = true;
            existingStyle = value;
        } else {
            write(" " + name + "=\"" + escapeHtml(value) + "\"");
        }
    }
    
    // 处理内联样式（来自局部样式块）
    if (!currentState_.pendingInlineStyles.empty() || hasStyleAttr) {
        std::string combinedStyles = existingStyle;
        if (!existingStyle.empty() && !currentState_.pendingInlineStyles.empty()) {
            combinedStyles += " ";
        }
        combinedStyles += currentState_.pendingInlineStyles;
        
        if (!combinedStyles.empty()) {
            write(" style=\"" + escapeHtml(combinedStyles) + "\"");
        }
    }
    
    // 处理自动添加的类
    if (!currentState_.autoAddedClasses.empty()) {
        std::string classes;
        auto it = node->getAttributes().find("class");
        if (it != node->getAttributes().end()) {
            classes = it->second;
        }
        
        for (const auto& cls : currentState_.autoAddedClasses) {
            if (!classes.empty()) classes += " ";
            classes += cls;
        }
        
        if (!classes.empty()) {
            write(" class=\"" + escapeHtml(classes) + "\"");
        }
    }
    
    // 处理自动添加的ID
    if (!currentState_.autoAddedIds.empty() && node->getAttributes().find("id") == node->getAttributes().end()) {
        write(" id=\"" + escapeHtml(currentState_.autoAddedIds.front()) + "\"");
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
    (void)node;
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
        // 局部样式块在父元素处理时已经被处理了，这里不需要生成输出
        // 只处理选择器样式（添加到全局样式）
        for (const auto& rule : node->getRules()) {
            if (rule->getType() == NodeType::SELECTOR) {
                auto selector = static_cast<SelectorNode*>(rule.get());
                // 将选择器样式添加到全局样式
                std::stringstream temp;
                std::swap(output_, temp);
                
                generateCSSRule(selector);
                globalStyles_ << output_.str();
                
                std::swap(output_, temp);
            }
        }
    } else {
        // 全局样式块
        writeLine("<style>");
        indent();
        
        for (const auto& rule : node->getRules()) {
            if (rule->getType() == NodeType::PROPERTY) {
                auto prop = static_cast<PropertyNode*>(rule.get());
                writeLine(prop->getName() + ": " + prop->getValue() + ";");
            } else if (rule->getType() == NodeType::SELECTOR) {
                generateCSSRule(static_cast<SelectorNode*>(rule.get()));
            }
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
            std::stringstream selectorStyles;
            std::stringstream temp;
            std::swap(output_, temp);
            output_.swap(selectorStyles);
            
            generateSelector(selector);
            
            output_.swap(selectorStyles);
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
        // TODO: node->getContent()->accept(this);
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
                currentState_.autoAddedClasses.push_back(sel);
            } else if (selector->getSelectorType() == SelectorNode::SelectorType::ID) {
                currentState_.autoAddedIds.push_back(sel);
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
    // 实现变量替换逻辑
    std::string result = value;
    size_t pos = 0;
    
    while ((pos = result.find("@", pos)) != std::string::npos) {
        // 查找变量名的结束位置
        size_t endPos = pos + 1;
        while (endPos < result.length() && 
               (std::isalnum(result[endPos]) || result[endPos] == '_')) {
            endPos++;
        }
        
        if (endPos > pos + 1) {
            std::string varName = result.substr(pos + 1, endPos - pos - 1);
            
            // 在当前上下文中查找变量
            auto it = templateVars_.find(varName);
            if (it != templateVars_.end()) {
                result.replace(pos, endPos - pos, it->second);
                pos += it->second.length();
            } else {
                // 变量未定义
                ErrorBuilder(ErrorLevel::WARNING, ErrorType::REFERENCE_ERROR)
                    .withMessage("Undefined variable: @" + varName)
                    .report();
                pos = endPos;
            }
        } else {
            pos++;
        }
    }
    
    return result;
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
void Generator::visitTemplateNode(TemplateNode* node) { 
    // Templates are registered during parsing, no HTML output needed
    (void)node;
}
void Generator::visitTemplateUseNode(TemplateUseNode* node) { (void)node; }
void Generator::visitCustomNode(CustomNode* node) { (void)node; }
void Generator::visitCustomUseNode(CustomUseNode* node) { (void)node; }
void Generator::visitOriginNode(OriginNode* node) { (void)node; }
void Generator::visitOriginUseNode(OriginUseNode* node) { (void)node; }
void Generator::visitImportNode(ImportNode* node) { (void)node; }
void Generator::visitConfigNode(ConfigNode* node) { (void)node; }
void Generator::visitInfoNode(InfoNode* node) { (void)node; }
void Generator::visitExportNode(ExportNode* node) { (void)node; }
void Generator::visitNamespaceNode(NamespaceNode* node) { (void)node; }
void Generator::visitFromNode(FromNode* node) { (void)node; }
void Generator::visitDeleteNode(DeleteNode* node) { (void)node; }
void Generator::visitInsertNode(InsertNode* node) { (void)node; }
void Generator::visitInheritNode(InheritNode* node) { (void)node; }
void Generator::visitExceptNode(ExceptNode* node) { (void)node; }
void Generator::visitUseNode(UseNode* node) { (void)node; }

void Generator::generateCSSRule(SelectorNode* node) {
    if (!node) return;
    
    // 生成选择器
    generateSelector(node);
    write(" {");
    writeLine();
    indent();
    
    // 生成属性
    if (node->getContent() && node->getContent()->getType() == NodeType::STYLE_BLOCK) {
        auto styleContent = static_cast<StyleNode*>(node->getContent().get());
        for (const auto& rule : styleContent->getRules()) {
            if (rule && rule->getType() == NodeType::PROPERTY) {
                auto prop = static_cast<PropertyNode*>(rule.get());
                writeLine(prop->getName() + ": " + prop->getValue() + ";");
            }
        }
    }
    
    dedent();
    writeLine("}");
}

void Generator::generateSelector(SelectorNode* node) {
    if (!node) return;
    
    switch (node->getSelectorType()) {
        case SelectorNode::SelectorType::CLASS:
            output_ << "." << node->getSelector();
            break;
        case SelectorNode::SelectorType::ID:
            output_ << "#" << node->getSelector();
            break;
        case SelectorNode::SelectorType::TAG:
            output_ << node->getSelector();
            break;
        case SelectorNode::SelectorType::PSEUDO_CLASS: {
            std::string sel = node->getSelector();
            // 处理&:hover格式
            if (sel.find("&:") == 0) {
                // TODO: 替换&为父选择器
                output_ << sel.substr(1); // 暂时去掉&
            } else {
                output_ << ":" << sel;
            }
            break;
        }
        case SelectorNode::SelectorType::PSEUDO_ELEMENT:
            output_ << "::" << node->getSelector();
            break;
        case SelectorNode::SelectorType::REFERENCE:
            output_ << "&";
            if (!node->getSelector().empty()) {
                output_ << node->getSelector();
            }
            break;
        case SelectorNode::SelectorType::COMPOUND:
            // Handle compound selectors
            for (const auto& child : node->getChildren()) {
                if (auto childSelector = dynamic_cast<SelectorNode*>(child.get())) {
                    generateSelector(childSelector);
                }
            }
            break;
    }
}

} // namespace CHTL