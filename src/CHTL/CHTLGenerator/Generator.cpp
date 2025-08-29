#include "Generator.h"
#include "../CHTLNode/TemplateNode.h"
#include "../CHTLNode/CustomNode.h"
#include "../CHTLNode/OriginNode.h"
#include "../CHTLConfig/ConfigNode.h"
#include <sstream>
#include <algorithm>
#include <regex>

namespace CHTL {

// CHTLGenerator实现
CHTLGenerator::CHTLGenerator(const GeneratorConfig& config) 
    : config(config), indentLevel(0), generatedElementIds(0) {
    errors.clear();
    warnings.clear();
}

String CHTLGenerator::generateHTML(BaseNode* ast) {
    if (!ast) {
        reportError("AST is null");
        return "";
    }
    
    std::ostringstream html;
    indentLevel = 0;
    
    try {
        if (config.generateDocumentStructure) {
            html << generateDocumentHeader();
        }
        
        // 生成主体内容
        String bodyContent = generateNodeHTML(ast);
        html << bodyContent;
        
        if (config.generateDocumentStructure) {
            html << generateDocumentFooter();
        }
        
    } catch (const std::exception& e) {
        reportError("HTML生成异常: " + String(e.what()));
    }
    
    return html.str();
}

String CHTLGenerator::generateCSS(BaseNode* ast) {
    if (!ast) {
        reportError("AST is null");
        return "";
    }
    
    std::ostringstream css;
    
    try {
        // 收集所有样式信息
        collectStyleInformation(ast);
        
        // 生成全局样式
        css << generateGlobalStyles();
        
        // 生成组件样式
        css << generateComponentStyles(ast);
        
        // 生成自动化样式
        if (config.enableStyleAutomation) {
            css << generateAutomatedStyles(ast);
        }
        
    } catch (const std::exception& e) {
        reportError("CSS生成异常: " + String(e.what()));
    }
    
    return css.str();
}

String CHTLGenerator::generateJavaScript(BaseNode* ast) {
    if (!ast) {
        reportError("AST is null");
        return "";
    }
    
    std::ostringstream js;
    
    try {
        if (config.generateJSWrapper) {
            js << generateJSHeader();
        }
        
        // 生成脚本内容
        js << generateNodeJS(ast);
        
        // 生成初始化代码
        if (config.enableAutoInitialization) {
            js << generateInitializationCode(ast);
        }
        
        if (config.generateJSWrapper) {
            js << generateJSFooter();
        }
        
    } catch (const std::exception& e) {
        reportError("JavaScript生成异常: " + String(e.what()));
    }
    
    return js.str();
}

String CHTLGenerator::generateNodeHTML(BaseNode* node) {
    if (!node) return "";
    
    std::ostringstream html;
    
    switch (node->type) {
        case NodeType::DOCUMENT:
            html << generateDocumentHTML(node);
            break;
        case NodeType::ELEMENT:
            html << generateElementHTML(static_cast<ElementNode*>(node));
            break;
        case NodeType::TEXT:
            html << generateTextHTML(static_cast<TextNode*>(node));
            break;
        case NodeType::TEMPLATE:
        case NodeType::TEMPLATE_STYLE:
        case NodeType::TEMPLATE_ELEMENT:
        case NodeType::TEMPLATE_VAR:
            html << generateTemplateHTML(node);
            break;
        case NodeType::CUSTOM:
        case NodeType::CUSTOM_STYLE:
        case NodeType::CUSTOM_ELEMENT:
        case NodeType::CUSTOM_VAR:
            html << generateCustomHTML(node);
            break;
        case NodeType::ORIGIN:
        case NodeType::ORIGIN_HTML:
        case NodeType::ORIGIN_STYLE:
        case NodeType::ORIGIN_JAVASCRIPT:
        case NodeType::ORIGIN_CUSTOM:
            html << generateOriginHTML(node);
            break;
        case NodeType::STYLE:
            // 样式节点在CSS生成中处理
            break;
        case NodeType::SCRIPT:
            // 脚本节点在JS生成中处理
            break;
        default:
            // 处理其他节点类型的子节点
            for (const auto& child : node->children) {
                html << generateNodeHTML(child.get());
            }
            break;
    }
    
    return html.str();
}

String CHTLGenerator::generateElementHTML(ElementNode* element) {
    if (!element) return "";
    
    std::ostringstream html;
    
    // 处理自动ID分配
    if (config.enableAutoIdAssignment && !element->hasAttribute("id")) {
        String autoId = generateAutoId();
        element->setAttribute("id", autoId);
    }
    
    // 开始标签
    html << getIndent() << "<" << element->getTagName();
    
    // 生成属性
    for (const auto& attrName : element->getAttributeNames()) {
        String attrValue = element->getAttribute(attrName);
        html << " " << attrName << "=\"" << escapeHTML(attrValue) << "\"";
    }
    
    // 处理自闭合标签
    if (element->isSelfClosing || HTMLElementMap::isSingleTag(element->getTagName())) {
        html << " />";
        if (config.enablePrettyPrint) html << "\n";
        return html.str();
    }
    
    html << ">";
    if (config.enablePrettyPrint) html << "\n";
    
    // 生成子内容
    indentLevel++;
    
    // 首先处理文本内容
    auto textNodes = element->getTextNodes();
    if (!textNodes.empty()) {
        for (auto* textNode : textNodes) {
            html << generateTextHTML(static_cast<TextNode*>(textNode));
        }
    }
    
    // 然后处理其他子元素
    for (const auto& child : element->children) {
        if (child->type != NodeType::TEXT && 
            child->type != NodeType::STYLE && 
            child->type != NodeType::SCRIPT) {
            html << generateNodeHTML(child.get());
        }
    }
    
    indentLevel--;
    
    // 结束标签
    html << getIndent() << "</" << element->getTagName() << ">";
    if (config.enablePrettyPrint) html << "\n";
    
    return html.str();
}

String CHTLGenerator::generateTextHTML(TextNode* textNode) {
    if (!textNode) return "";
    
    String text = textNode->getText();
    if (text.empty()) return "";
    
    String escaped = escapeHTML(text);
    
    if (config.enablePrettyPrint) {
        return getIndent() + escaped + "\n";
    }
    
    return escaped;
}

String CHTLGenerator::generateTemplateHTML(BaseNode* templateNode) {
    // 模板节点不直接生成HTML，而是在使用时实例化
    String templateType = templateNode->getAttribute("template_type");
    String templateName = templateNode->value;
    
    if (templateType == "Element") {
        // 元素模板实例化
        return instantiateElementTemplate(templateNode);
    }
    
    return ""; // 样式和变量模板不生成HTML
}

String CHTLGenerator::generateCustomHTML(BaseNode* customNode) {
    String customType = customNode->getAttribute("custom_type");
    String customName = customNode->value;
    
    if (customType == "Element") {
        // 自定义元素实例化
        return instantiateCustomElement(customNode);
    }
    
    return ""; // 样式和变量自定义不生成HTML
}

String CHTLGenerator::generateOriginHTML(BaseNode* originNode) {
    String originType = originNode->getAttribute("origin_type");
    
    if (originType == "Html") {
        // HTML原始嵌入直接输出
        return originNode->value;
    }
    
    return ""; // 其他原始嵌入类型不生成HTML
}

String CHTLGenerator::generateDocumentHTML(BaseNode* documentNode) {
    std::ostringstream html;
    
    for (const auto& child : documentNode->children) {
        html << generateNodeHTML(child.get());
    }
    
    return html.str();
}

String CHTLGenerator::generateNodeJS(BaseNode* node) {
    if (!node) return "";
    
    std::ostringstream js;
    
    // 处理脚本节点
    if (node->type == NodeType::SCRIPT) {
        js << generateScriptContent(node);
    }
    
    // 递归处理子节点
    for (const auto& child : node->children) {
        js << generateNodeJS(child.get());
    }
    
    return js.str();
}

String CHTLGenerator::generateScriptContent(BaseNode* scriptNode) {
    String content = scriptNode->value;
    
    if (config.enableJSOptimization) {
        content = optimizeJavaScript(content);
    }
    
    return content + "\n";
}

void CHTLGenerator::collectStyleInformation(BaseNode* node) {
    if (!node) return;
    
    // 收集样式节点信息
    if (node->type == NodeType::STYLE) {
        styleNodes.push_back(node);
    }
    
    // 收集模板样式信息
    if (node->type == NodeType::TEMPLATE_STYLE) {
        templateStyles.push_back(node);
    }
    
    // 收集自定义样式信息
    if (node->type == NodeType::CUSTOM_STYLE) {
        customStyles.push_back(node);
    }
    
    // 递归收集
    for (const auto& child : node->children) {
        collectStyleInformation(child.get());
    }
}

String CHTLGenerator::generateGlobalStyles() {
    std::ostringstream css;
    
    // 生成全局样式
    for (BaseNode* styleNode : styleNodes) {
        String content = styleNode->value;
        if (!content.empty()) {
            css << "/* Global Style */\n";
            css << content << "\n";
        }
    }
    
    return css.str();
}

String CHTLGenerator::generateComponentStyles(BaseNode* ast) {
    std::ostringstream css;
    
    // 生成组件相关样式
    generateElementStyles(ast, css);
    
    return css.str();
}

void CHTLGenerator::generateElementStyles(BaseNode* node, std::ostringstream& css) {
    if (!node) return;
    
    if (node->type == NodeType::ELEMENT) {
        ElementNode* element = static_cast<ElementNode*>(node);
        
        // 检查元素是否有局部样式块
        BaseNode* styleBlock = element->getStyleBlock();
        if (styleBlock) {
            generateLocalStyleBlock(element, styleBlock, css);
        }
    }
    
    // 递归处理子节点
    for (const auto& child : node->children) {
        generateElementStyles(child.get(), css);
    }
}

void CHTLGenerator::generateLocalStyleBlock(ElementNode* element, BaseNode* styleBlock, std::ostringstream& css) {
    String styleContent = styleBlock->value;
    String elementId = element->getAttribute("id");
    String elementClass = element->getAttribute("class");
    
    // 处理&引用选择器
    if (styleContent.find("&") != String::npos) {
        String targetSelector;
        
        if (!elementClass.empty()) {
            targetSelector = "." + elementClass;
        } else if (!elementId.empty()) {
            targetSelector = "#" + elementId;
        } else {
            targetSelector = element->getTagName();
        }
        
        // 替换&引用
        std::regex ampersandRegex(R"(&)");
        styleContent = std::regex_replace(styleContent, ampersandRegex, targetSelector);
    }
    
    // 生成样式块注释
    css << "/* Local style for " << element->getTagName();
    if (!elementId.empty()) css << "#" << elementId;
    if (!elementClass.empty()) css << "." << elementClass;
    css << " */\n";
    
    css << styleContent << "\n";
}

String CHTLGenerator::generateAutomatedStyles(BaseNode* ast) {
    std::ostringstream css;
    
    if (config.enableSelectorAutomation) {
        // 生成选择器自动化样式
        css << "/* Automated Styles */\n";
        generateAutomatedSelectors(ast, css);
    }
    
    return css.str();
}

void CHTLGenerator::generateAutomatedSelectors(BaseNode* node, std::ostringstream& css) {
    // 根据配置生成自动化选择器
    // 这里可以实现复杂的选择器自动化逻辑
}

String CHTLGenerator::instantiateElementTemplate(BaseNode* templateNode) {
    // 元素模板实例化逻辑
    return "<div class=\"template-instance\"><!-- Template: " + templateNode->value + " --></div>\n";
}

String CHTLGenerator::instantiateCustomElement(BaseNode* customNode) {
    // 自定义元素实例化逻辑
    return "<div class=\"custom-element\"><!-- Custom: " + customNode->value + " --></div>\n";
}

String CHTLGenerator::generateInitializationCode(BaseNode* ast) {
    std::ostringstream js;
    
    js << "\n// CHTL Initialization Code\n";
    js << "document.addEventListener('DOMContentLoaded', function() {\n";
    js << "    console.log('CHTL page initialized');\n";
    js << "    // Additional initialization code here\n";
    js << "});\n";
    
    return js.str();
}

String CHTLGenerator::generateDocumentHeader() {
    std::ostringstream html;
    
    html << "<!DOCTYPE html>\n";
    html << "<html lang=\"" << config.documentLanguage << "\">\n";
    html << "<head>\n";
    html << "    <meta charset=\"" << config.documentCharset << "\">\n";
    html << "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
    html << "    <title>" << config.documentTitle << "</title>\n";
    
    if (config.includeGeneratedCSS) {
        html << "    <style>\n";
        // CSS将在后续生成并插入
        html << "    </style>\n";
    }
    
    html << "</head>\n";
    html << "<body>\n";
    
    return html.str();
}

String CHTLGenerator::generateDocumentFooter() {
    std::ostringstream html;
    
    if (config.includeGeneratedJS) {
        html << "    <script>\n";
        // JavaScript将在后续生成并插入
        html << "    </script>\n";
    }
    
    html << "</body>\n";
    html << "</html>\n";
    
    return html.str();
}

String CHTLGenerator::generateJSHeader() {
    std::ostringstream js;
    
    if (config.enableStrictMode) {
        js << "'use strict';\n\n";
    }
    
    js << "// CHTL Generated JavaScript\n";
    js << "(function() {\n";
    
    return js.str();
}

String CHTLGenerator::generateJSFooter() {
    return "\n})();\n";
}

String CHTLGenerator::getIndent() {
    if (!config.enablePrettyPrint) return "";
    return String(indentLevel * config.indentSize, ' ');
}

String CHTLGenerator::escapeHTML(const String& text) {
    String escaped = text;
    
    std::regex ampRegex("&");
    escaped = std::regex_replace(escaped, ampRegex, "&amp;");
    
    std::regex ltRegex("<");
    escaped = std::regex_replace(escaped, ltRegex, "&lt;");
    
    std::regex gtRegex(">");
    escaped = std::regex_replace(escaped, gtRegex, "&gt;");
    
    std::regex quotRegex("\"");
    escaped = std::regex_replace(escaped, quotRegex, "&quot;");
    
    std::regex aposRegex("'");
    escaped = std::regex_replace(escaped, aposRegex, "&#39;");
    
    return escaped;
}

String CHTLGenerator::generateAutoId() {
    return "chtl-auto-id-" + std::to_string(++generatedElementIds);
}

String CHTLGenerator::optimizeJavaScript(const String& js) {
    String optimized = js;
    
    if (config.enableJSMinification) {
        // 移除多行注释
        std::regex multiCommentRegex(R"(/\*.*?\*/)");
        optimized = std::regex_replace(optimized, multiCommentRegex, "");
        
        // 移除单行注释
        std::regex singleCommentRegex(R"(//.*$)", std::regex_constants::multiline);
        optimized = std::regex_replace(optimized, singleCommentRegex, "");
        
        // 压缩空白
        std::regex whitespaceRegex(R"(\s+)");
        optimized = std::regex_replace(optimized, whitespaceRegex, " ");
    }
    
    return optimized;
}

void CHTLGenerator::reportError(const String& error) {
    errors.push_back(error);
}

void CHTLGenerator::reportWarning(const String& warning) {
    warnings.push_back(warning);
}

bool CHTLGenerator::hasErrors() const {
    return !errors.empty();
}

bool CHTLGenerator::hasWarnings() const {
    return !warnings.empty();
}

StringVector CHTLGenerator::getErrors() const {
    return errors;
}

StringVector CHTLGenerator::getWarnings() const {
    return warnings;
}

void CHTLGenerator::clearErrors() {
    errors.clear();
}

void CHTLGenerator::clearWarnings() {
    warnings.clear();
}

void CHTLGenerator::setConfig(const GeneratorConfig& newConfig) {
    config = newConfig;
}

GeneratorConfig CHTLGenerator::getConfig() const {
    return config;
}

} // namespace CHTL