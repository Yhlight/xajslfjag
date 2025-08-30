#include "CHTLGenerator.h"
#include "../../Error/ErrorReporter.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <regex>

namespace CHTL {

CHTLGenerator::CHTLGenerator() : m_debugMode(false) {
    // 设置默认选项
    m_options.indentation = "  ";
    m_options.lineEnding = "\n";
    m_options.preserveComments = true;
    
    m_context = std::make_shared<CHTLContext>();
}

CHTLGenerator::~CHTLGenerator() = default;

CHTLGenerator::GenerationResult CHTLGenerator::generateAll(std::shared_ptr<BaseNode> rootNode) {
    GenerationResult result;
    
    try {
        if (m_debugMode) {
            std::cout << "🚀 开始CHTL代码生成" << std::endl;
        }
        
        clearErrors();
        
        // 生成HTML
        pushGenerationContext("html");
        result.html = generateHTML(rootNode);
        popGenerationContext();
        
        // 生成CSS
        pushGenerationContext("css");
        result.css = generateCSS(rootNode);
        popGenerationContext();
        
        // 生成JavaScript
        pushGenerationContext("javascript");
        result.javascript = generateJavaScript(rootNode);
        popGenerationContext();
        
        result.success = m_errors.empty();
        result.errors = m_errors;
        result.warnings = m_warnings;
        
        if (m_debugMode) {
            std::cout << "  ✓ HTML: " << result.html.length() << " 字符" << std::endl;
            std::cout << "  ✓ CSS: " << result.css.length() << " 字符" << std::endl;
            std::cout << "  ✓ JavaScript: " << result.javascript.length() << " 字符" << std::endl;
        }
        
    } catch (const std::exception& e) {
        result.success = false;
        result.errors.push_back("生成过程中发生异常: " + std::string(e.what()));
        reportError(result.errors.back());
    }
    
    return result;
}

std::string CHTLGenerator::generateHTML(std::shared_ptr<BaseNode> rootNode) {
    if (!rootNode) {
        reportError("根节点为空");
        return "";
    }
    
    std::ostringstream html;
    
    // 生成HTML文档结构
    html << "<!DOCTYPE html>" << m_options.lineEnding;
    html << generateHTMLNode(rootNode, 0);
    
    std::string result = html.str();
    
    // 处理变量替换
    result = replaceVariables(result);
    
    // 格式化
    if (!m_options.enableMinification) {
        result = formatHTML(result);
    }
    
    return result;
}

std::string CHTLGenerator::generateCSS(std::shared_ptr<BaseNode> rootNode) {
    if (!rootNode) {
        return "";
    }
    
    std::ostringstream css;
    
    // 收集所有样式节点
    std::vector<std::shared_ptr<BaseNode>> styleNodes;
    collectStyleNodes(rootNode, styleNodes);
    
    // 生成CSS规则
    for (const auto& styleNode : styleNodes) {
        css << generateCSSNode(styleNode, 0);
    }
    
    std::string result = css.str();
    
    // 处理变量替换
    result = replaceVariables(result);
    
    // 格式化
    if (!m_options.enableMinification) {
        result = formatCSS(result);
    }
    
    return result;
}

std::string CHTLGenerator::generateJavaScript(std::shared_ptr<BaseNode> rootNode) {
    if (!rootNode) {
        return "";
    }
    
    std::ostringstream js;
    
    // 收集所有脚本节点
    std::vector<std::shared_ptr<BaseNode>> scriptNodes;
    collectScriptNodes(rootNode, scriptNodes);
    
    // 生成JavaScript代码
    js << "(function() {" << m_options.lineEnding;
    js << m_options.indentation << "// CHTL生成的JavaScript代码" << m_options.lineEnding;
    
    for (const auto& scriptNode : scriptNodes) {
        js << generateJSNode(scriptNode, 1);
    }
    
    js << "})();" << m_options.lineEnding;
    
    std::string result = js.str();
    
    // 处理变量替换
    result = replaceVariables(result);
    
    // 格式化
    if (!m_options.enableMinification) {
        result = formatJavaScript(result);
    }
    
    return result;
}

std::string CHTLGenerator::generateHTMLNode(std::shared_ptr<BaseNode> node, int depth) {
    if (!node) {
        return "";
    }
    
    std::ostringstream html;
    
    switch (node->getType()) {
        case NodeType::DOCUMENT:
            for (const auto& child : node->getChildren()) {
                html << generateHTMLNode(child, depth);
            }
            break;
            
        case NodeType::ELEMENT:
            html << generateHTMLElement(node, depth);
            break;
            
        case NodeType::TEXT:
            html << generateHTMLText(node);
            break;
            
        case NodeType::COMMENT:
            if (m_options.preserveComments) {
                html << generateHTMLComment(node);
            }
            break;
            
        case NodeType::TEMPLATE:
            html << expandTemplate(node);
            break;
            
        case NodeType::CUSTOM:
            html << expandCustom(node);
            break;
            
        case NodeType::ORIGIN:
            html << processOriginEmbedding(node);
            break;
            
        default:
            // 跳过非HTML相关节点
            break;
    }
    
    return html.str();
}

std::string CHTLGenerator::generateHTMLElement(std::shared_ptr<BaseNode> node, int depth) {
    std::ostringstream html;
    
    std::string tagName = node->getAttribute("tag");
    if (tagName.empty()) {
        tagName = node->getValue();
    }
    
    // 开始标签
    html << getIndentation(depth) << "<" << tagName;
    
    // 添加属性
    for (const auto& child : node->getChildren()) {
        if (child->getType() == NodeType::ATTRIBUTE) {
            std::string attrName = child->getAttribute("name");
            std::string attrValue = child->getAttribute("value");
            html << " " << attrName << "=\"" << escapeHTML(attrValue) << "\"";
        }
    }
    
    html << ">";
    
    // 检查是否为自闭合标签
    std::vector<std::string> selfClosingTags = {
        "area", "base", "br", "col", "embed", "hr", "img", "input",
        "link", "meta", "param", "source", "track", "wbr"
    };
    
    bool isSelfClosing = std::find(selfClosingTags.begin(), selfClosingTags.end(), tagName) != selfClosingTags.end();
    
    if (!isSelfClosing) {
        // 处理子元素和内容
        bool hasBlockChildren = false;
        
        for (const auto& child : node->getChildren()) {
            if (child->getType() != NodeType::ATTRIBUTE && 
                child->getType() != NodeType::STYLE && 
                child->getType() != NodeType::SCRIPT) {
                
                if (child->getType() == NodeType::ELEMENT) {
                    if (!hasBlockChildren) {
                        html << m_options.lineEnding;
                        hasBlockChildren = true;
                    }
                    html << generateHTMLNode(child, depth + 1);
                } else {
                    html << generateHTMLNode(child, depth);
                }
            }
        }
        
        // 结束标签
        if (hasBlockChildren) {
            html << getIndentation(depth);
        }
        html << "</" << tagName << ">" << m_options.lineEnding;
    } else {
        html << m_options.lineEnding;
    }
    
    return html.str();
}

std::string CHTLGenerator::generateHTMLText(std::shared_ptr<BaseNode> node) {
    std::string text = node->getValue();
    return escapeHTML(text);
}

std::string CHTLGenerator::generateHTMLComment(std::shared_ptr<BaseNode> node) {
    std::string commentType = node->getAttribute("type");
    std::string content = node->getValue();
    
    if (commentType == "generator") {
        // 生成器注释，根据上下文生成HTML注释
        return "<!-- " + content + " -->" + m_options.lineEnding;
    }
    
    return "";  // 普通注释不输出到HTML
}

std::string CHTLGenerator::generateCSSNode(std::shared_ptr<BaseNode> node, int depth) {
    if (!node) {
        return "";
    }
    
    std::ostringstream css;
    
    switch (node->getType()) {
        case NodeType::STYLE:
            css << generateCSSRule(node, depth);
            break;
            
        case NodeType::TEMPLATE:
            if (node->getAttribute("type") == "Style") {
                css << expandTemplate(node);
            }
            break;
            
        case NodeType::CUSTOM:
            if (node->getAttribute("type") == "Style") {
                css << expandCustom(node);
            }
            break;
            
        default:
            // 递归处理子节点
            for (const auto& child : node->getChildren()) {
                css << generateCSSNode(child, depth);
            }
            break;
    }
    
    return css.str();
}

std::string CHTLGenerator::generateCSSRule(std::shared_ptr<BaseNode> node, int depth) {
    std::ostringstream css;
    
    // 生成选择器
    std::string selector = generateCSSSelector(node);
    if (!selector.empty()) {
        css << getIndentation(depth) << selector << " {" << m_options.lineEnding;
        
        // 生成属性
        for (const auto& attr : node->getAttributes()) {
            if (attr.first != "selector") {
                css << getIndentation(depth + 1) << attr.first << ": " << attr.second << ";" << m_options.lineEnding;
            }
        }
        
        css << getIndentation(depth) << "}" << m_options.lineEnding;
    }
    
    return css.str();
}

std::string CHTLGenerator::generateCSSSelector(std::shared_ptr<BaseNode> node) {
    std::string selector = node->getAttribute("selector");
    
    if (selector.empty()) {
        // 根据上下文推断选择器
        selector = "." + node->getValue();  // 默认类选择器
    }
    
    return selector;
}

std::string CHTLGenerator::generateJSNode(std::shared_ptr<BaseNode> node, int depth) {
    if (!node) {
        return "";
    }
    
    std::ostringstream js;
    
    switch (node->getType()) {
        case NodeType::SCRIPT:
            js << getIndentation(depth) << node->getValue() << m_options.lineEnding;
            break;
            
        default:
            // 递归处理子节点
            for (const auto& child : node->getChildren()) {
                js << generateJSNode(child, depth);
            }
            break;
    }
    
    return js.str();
}

std::string CHTLGenerator::expandTemplate(std::shared_ptr<BaseNode> templateNode) {
    std::string templateType = templateNode->getAttribute("type");
    std::string templateName = templateNode->getAttribute("name");
    
    std::ostringstream expanded;
    
    if (templateType == "Style") {
        // 展开样式模板
        expanded << "/* Template: " << templateName << " */" << m_options.lineEnding;
        
        for (const auto& attr : templateNode->getAttributes()) {
            if (attr.first != "type" && attr.first != "name") {
                expanded << attr.first << ": " << attr.second << ";" << m_options.lineEnding;
            }
        }
    } else if (templateType == "Element") {
        // 展开元素模板
        expanded << "<!-- Template: " << templateName << " -->" << m_options.lineEnding;
        
        for (const auto& child : templateNode->getChildren()) {
            expanded << generateHTMLNode(child, 0);
        }
    } else if (templateType == "Var") {
        // 变量模板在变量替换阶段处理
        expanded << "/* Variables: " << templateName << " */";
    }
    
    return expanded.str();
}

std::string CHTLGenerator::expandCustom(std::shared_ptr<BaseNode> customNode) {
    std::string customType = customNode->getAttribute("type");
    std::string customName = customNode->getAttribute("name");
    std::string inherit = customNode->getAttribute("inherit");
    
    std::ostringstream expanded;
    
    if (customType == "Style") {
        expanded << "/* Custom Style: " << customName;
        if (!inherit.empty()) {
            expanded << " from " << inherit;
        }
        expanded << " */" << m_options.lineEnding;
        
        // 处理继承
        if (!inherit.empty()) {
            expanded << processTemplateInheritance(customNode);
        }
        
        // 处理特例化
        expanded << processCustomSpecialization(customNode);
        
    } else if (customType == "Element") {
        expanded << "<!-- Custom Element: " << customName;
        if (!inherit.empty()) {
            expanded << " from " << inherit;
        }
        expanded << " -->" << m_options.lineEnding;
        
        for (const auto& child : customNode->getChildren()) {
            expanded << generateHTMLNode(child, 0);
        }
    }
    
    return expanded.str();
}

std::string CHTLGenerator::processTemplateInheritance(std::shared_ptr<BaseNode> node) {
    std::string inherit = node->getAttribute("inherit");
    if (inherit.empty()) {
        return "";
    }
    
    // 查找继承的模板
    if (m_context) {
        // 这里应该从上下文中获取模板定义
        // 暂时返回注释
        return "/* Inherit from: " + inherit + " */" + m_options.lineEnding;
    }
    
    return "";
}

std::string CHTLGenerator::processCustomSpecialization(std::shared_ptr<BaseNode> node) {
    std::ostringstream specialized;
    
    // 处理特例化操作：delete, insert, replace等
    for (const auto& child : node->getChildren()) {
        if (child->getType() == NodeType::OPERATION) {
            std::string operation = child->getAttribute("operation");
            
            if (operation == "delete") {
                specialized << "/* Delete: " << child->getValue() << " */" << m_options.lineEnding;
            } else if (operation == "insert") {
                std::string position = child->getAttribute("position");
                specialized << "/* Insert " << position << ": " << child->getValue() << " */" << m_options.lineEnding;
            } else if (operation == "replace") {
                specialized << "/* Replace: " << child->getValue() << " */" << m_options.lineEnding;
            }
        }
    }
    
    return specialized.str();
}

std::string CHTLGenerator::replaceVariables(const std::string& content) {
    std::string result = content;
    
    // 替换变量引用 VarGroup(varName)
    std::regex varRegex(R"((\w+)\((\w+)\))");
    std::sregex_iterator iter(result.begin(), result.end(), varRegex);
    std::sregex_iterator end;
    
    std::vector<std::pair<std::string, std::string>> replacements;
    
    for (; iter != end; ++iter) {
        std::string varGroup = (*iter)[1].str();
        std::string varName = (*iter)[2].str();
        std::string varRef = (*iter).str();
        
        std::string resolvedValue = resolveVariableReference(varRef);
        if (!resolvedValue.empty()) {
            replacements.push_back({varRef, resolvedValue});
        }
    }
    
    // 执行替换
    for (const auto& replacement : replacements) {
        size_t pos = 0;
        while ((pos = result.find(replacement.first, pos)) != std::string::npos) {
            result.replace(pos, replacement.first.length(), replacement.second);
            pos += replacement.second.length();
        }
    }
    
    return result;
}

std::string CHTLGenerator::resolveVariableReference(const std::string& varRef) {
    if (m_context) {
        auto varGroup = m_context->GetVarGroup();
        if (varGroup) {
            // 解析变量引用
            std::regex varRefRegex(R"((\w+)\((\w+)\))");
            std::smatch match;
            
            if (std::regex_match(varRef, match, varRefRegex)) {
                std::string groupName = match[1].str();
                std::string varName = match[2].str();
                
                // 这里应该从对应的变量组中获取值
                // 暂时返回占位符
                return "var(--" + groupName + "-" + varName + ")";
            }
        }
    }
    
    return varRef;  // 无法解析时返回原值
}

std::string CHTLGenerator::processOriginEmbedding(std::shared_ptr<BaseNode> originNode) {
    std::string originType = originNode->getAttribute("type");
    std::string content = originNode->getValue();
    
    return generateOriginContent(originType, content);
}

std::string CHTLGenerator::generateOriginContent(const std::string& type, const std::string& content) {
    if (type == "Html") {
        return content;  // HTML内容直接输出
    } else if (type == "Style") {
        return "<style>" + m_options.lineEnding + content + m_options.lineEnding + "</style>";
    } else if (type == "JavaScript") {
        return "<script>" + m_options.lineEnding + content + m_options.lineEnding + "</script>";
    } else {
        // 自定义类型
        return "<!-- " + type + " Origin -->" + m_options.lineEnding + content;
    }
}

std::string CHTLGenerator::processGeneratorComment(const std::string& comment, const std::string& context) {
    if (context == "html") {
        return "<!-- " + comment + " -->";
    } else if (context == "css") {
        return "/* " + comment + " */";
    } else if (context == "javascript") {
        return "// " + comment;
    }
    
    return "<!-- " + comment + " -->";  // 默认HTML注释
}

void CHTLGenerator::collectStyleNodes(std::shared_ptr<BaseNode> node, std::vector<std::shared_ptr<BaseNode>>& styleNodes) {
    if (!node) return;
    
    if (node->getType() == NodeType::STYLE) {
        styleNodes.push_back(node);
    }
    
    for (const auto& child : node->getChildren()) {
        collectStyleNodes(child, styleNodes);
    }
}

void CHTLGenerator::collectScriptNodes(std::shared_ptr<BaseNode> node, std::vector<std::shared_ptr<BaseNode>>& scriptNodes) {
    if (!node) return;
    
    if (node->getType() == NodeType::SCRIPT) {
        scriptNodes.push_back(node);
    }
    
    for (const auto& child : node->getChildren()) {
        collectScriptNodes(child, scriptNodes);
    }
}

std::string CHTLGenerator::formatHTML(const std::string& html) {
    // 基本HTML格式化
    std::string formatted = html;
    
    // 移除多余的空行
    std::regex multipleNewlines(R"(\n\s*\n\s*\n)");
    formatted = std::regex_replace(formatted, multipleNewlines, "\n\n");
    
    return formatted;
}

std::string CHTLGenerator::formatCSS(const std::string& css) {
    // 基本CSS格式化
    std::string formatted = css;
    
    // 在规则之间添加空行
    std::regex ruleEnd(R"(\}\s*([.#\w]))");
    formatted = std::regex_replace(formatted, ruleEnd, "}\n\n$1");
    
    return formatted;
}

std::string CHTLGenerator::formatJavaScript(const std::string& js) {
    // 基本JavaScript格式化
    std::string formatted = js;
    
    // 在函数之间添加空行
    std::regex functionEnd(R"(\}\s*(function|var|let|const))");
    formatted = std::regex_replace(formatted, functionEnd, "}\n\n$1");
    
    return formatted;
}

std::string CHTLGenerator::getIndentation(int depth) const {
    std::string indentation;
    for (int i = 0; i < depth; ++i) {
        indentation += m_options.indentation;
    }
    return indentation;
}

std::string CHTLGenerator::escapeHTML(const std::string& text) {
    std::string escaped = text;
    
    // HTML实体转义
    std::regex ampRegex("&");
    escaped = std::regex_replace(escaped, ampRegex, "&amp;");
    
    std::regex ltRegex("<");
    escaped = std::regex_replace(escaped, ltRegex, "&lt;");
    
    std::regex gtRegex(">");
    escaped = std::regex_replace(escaped, gtRegex, "&gt;");
    
    std::regex quotRegex("\"");
    escaped = std::regex_replace(escaped, quotRegex, "&quot;");
    
    return escaped;
}

void CHTLGenerator::setContext(std::shared_ptr<CHTLContext> context) {
    m_context = context;
}

void CHTLGenerator::setOptions(const GenerationOptions& options) {
    m_options = options;
}

void CHTLGenerator::clearErrors() {
    m_errors.clear();
    m_warnings.clear();
}

void CHTLGenerator::pushGenerationContext(const std::string& context) {
    m_generationContextStack.push_back(context);
}

void CHTLGenerator::popGenerationContext() {
    if (!m_generationContextStack.empty()) {
        m_generationContextStack.pop_back();
    }
}

std::string CHTLGenerator::getCurrentGenerationContext() const {
    if (!m_generationContextStack.empty()) {
        return m_generationContextStack.back();
    }
    return "html";
}

void CHTLGenerator::reportError(const std::string& message) {
    m_errors.push_back(message);
    
    if (m_debugMode) {
        std::cout << "  ❌ 生成错误: " << message << std::endl;
    }
    
    ErrorReporter reporter;
    reporter.error("CHTLGenerator", message);
}

void CHTLGenerator::reportWarning(const std::string& message) {
    m_warnings.push_back(message);
    
    if (m_debugMode) {
        std::cout << "  ⚠️ 生成警告: " << message << std::endl;
    }
    
    ErrorReporter reporter;
    reporter.warning("CHTLGenerator", message);
}

}