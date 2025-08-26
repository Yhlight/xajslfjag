#include "CHTLGenerator.h"
#include <algorithm>
#include <regex>
#include <iomanip>
#include <random>

namespace CHTL {

// ====== CHTLStyleScope 实现 ======

CHTLStyleScope::CHTLStyleScope(const std::string& id, bool global) 
    : scopeId(id), isGlobal(global) {
}

std::string CHTLStyleScope::getScopedSelector(const std::string& selector) {
    if (isGlobal) {
        return selector;
    }
    
    auto it = localSelectors.find(selector);
    if (it != localSelectors.end()) {
        return it->second;
    }
    
    // 生成作用域选择器
    std::string scopedSelector;
    if (selector.empty() || selector[0] == '.') {
        scopedSelector = "." + scopeId + " " + selector;
    } else if (selector[0] == '#') {
        scopedSelector = "#" + scopeId + " " + selector;
    } else if (selector[0] == '&') {
        scopedSelector = "." + scopeId + selector.substr(1);
    } else {
        scopedSelector = "." + scopeId + " " + selector;
    }
    
    localSelectors[selector] = scopedSelector;
    return scopedSelector;
}

void CHTLStyleScope::addLocalSelector(const std::string& original, const std::string& scoped) {
    localSelectors[original] = scoped;
}

// ====== CHTLTemplateManager 实现 ======

void CHTLTemplateManager::registerTemplate(const std::string& name, std::shared_ptr<CHTLTemplateNode> templateNode) {
    templates[name] = templateNode;
}

void CHTLTemplateManager::registerCustom(const std::string& name, std::shared_ptr<CHTLCustomNode> customNode) {
    customs[name] = customNode;
}

void CHTLTemplateManager::registerOrigin(const std::string& name, std::shared_ptr<CHTLNode> originNode) {
    origins[name] = originNode;
}

std::shared_ptr<CHTLTemplateNode> CHTLTemplateManager::getTemplate(const std::string& name) {
    auto it = templates.find(name);
    return it != templates.end() ? it->second : nullptr;
}

std::shared_ptr<CHTLCustomNode> CHTLTemplateManager::getCustom(const std::string& name) {
    auto it = customs.find(name);
    return it != customs.end() ? it->second : nullptr;
}

std::shared_ptr<CHTLNode> CHTLTemplateManager::getOrigin(const std::string& name) {
    auto it = origins.find(name);
    return it != origins.end() ? it->second : nullptr;
}

bool CHTLTemplateManager::hasTemplate(const std::string& name) const {
    return templates.find(name) != templates.end();
}

bool CHTLTemplateManager::hasCustom(const std::string& name) const {
    return customs.find(name) != customs.end();
}

bool CHTLTemplateManager::hasOrigin(const std::string& name) const {
    return origins.find(name) != origins.end();
}

// ====== CHTLVariableResolver 实现 ======

void CHTLVariableResolver::setVariable(const std::string& name, const std::string& value) {
    variables[name] = value;
}

void CHTLVariableResolver::setGlobalVariable(const std::string& name, const std::string& value) {
    globalVariables[name] = value;
}

std::string CHTLVariableResolver::resolveVariable(const std::string& name) const {
    auto it = variables.find(name);
    if (it != variables.end()) {
        return it->second;
    }
    
    auto git = globalVariables.find(name);
    if (git != globalVariables.end()) {
        return git->second;
    }
    
    return "${" + name + "}"; // 保持原样如果找不到
}

bool CHTLVariableResolver::hasVariable(const std::string& name) const {
    return variables.find(name) != variables.end() || 
           globalVariables.find(name) != globalVariables.end();
}

std::string CHTLVariableResolver::interpolateString(const std::string& input) const {
    std::string result = input;
    std::regex varPattern(R"(\$\{([^}]+)\})");
    std::smatch match;
    
    while (std::regex_search(result, match, varPattern)) {
        std::string varName = match[1].str();
        std::string varValue = resolveVariable(varName);
        result.replace(match.position(), match.length(), varValue);
    }
    
    return result;
}

// ====== CHTLGenerator 主类实现 ======

CHTLGenerator::CHTLGenerator(const CHTLGeneratorConfig& cfg) 
    : config(cfg), indentLevel(0), nextScopeId(1) {
}

CHTLGenerationResult CHTLGenerator::generate(std::shared_ptr<CHTLDocumentNode> document) {
    if (!document) {
        addError("文档节点为空");
        return createFailedResult();
    }
    
    // 重置状态
    htmlStream.str("");
    cssStream.str("");
    jsStream.str("");
    errors.clear();
    warnings.clear();
    indentLevel = 0;
    styleScopes.clear();
    generationCache.clear();
    
    // 创建全局样式作用域
    pushStyleScope(std::make_unique<CHTLStyleScope>("global", true));
    
    try {
        // 预处理：注册所有模板和自定义
        preprocessDocument(document);
        
        // 生成文档
        generateDocument(document);
        
        // 生成全局样式和脚本
        generateGlobalStyles();
        generateGlobalScripts();
        
        // 创建结果
        CHTLGenerationResult result;
        result.html = htmlStream.str();
        result.css = cssStream.str();
        result.javascript = jsStream.str();
        result.errors = errors;
        result.warnings = warnings;
        result.success = errors.empty();
        
        // 优化和验证
        if (config.enableOptimization && result.success) {
            result.html = optimizeHTML(result.html);
            result.css = optimizeCSS(result.css);
        }
        
        if (config.minifyOutput && result.success) {
            result.html = minifyHTML(result.html);
            result.css = minifyCSS(result.css);
        }
        
        if (config.validateOutput && result.success) {
            if (!validateHTML(result.html)) {
                addWarning("生成的HTML可能存在问题");
            }
            if (!validateCSS(result.css)) {
                addWarning("生成的CSS可能存在问题");
            }
        }
        
        // 设置元数据
        result.metadata["generator"] = "CHTL Compiler v1.0";
        result.metadata["timestamp"] = std::to_string(std::time(nullptr));
        result.metadata["charset"] = config.outputCharset;
        
        return result;
        
    } catch (const std::exception& e) {
        addError("生成过程中发生异常: " + std::string(e.what()));
        return createFailedResult();
    }
}

void CHTLGenerator::preprocessDocument(std::shared_ptr<CHTLDocumentNode> document) {
    // 遍历文档，注册所有模板、自定义和原始嵌入
    document->traverse([this](std::shared_ptr<CHTLNode> node) {
        switch (node->type) {
            case CHTLNodeType::TEMPLATE_DEFINITION:
                if (auto templateNode = std::dynamic_pointer_cast<CHTLTemplateNode>(node)) {
                    templateManager.registerTemplate(templateNode->name, templateNode);
                }
                break;
                
            case CHTLNodeType::CUSTOM_DEFINITION:
                if (auto customNode = std::dynamic_pointer_cast<CHTLCustomNode>(node)) {
                    templateManager.registerCustom(customNode->name, customNode);
                }
                break;
                
            case CHTLNodeType::ORIGIN_DEFINITION:
                templateManager.registerOrigin(node->name, node);
                break;
                
            case CHTLNodeType::VARIABLE_DEFINITION:
                // 处理变量定义
                processVariableDefinitions(node);
                break;
                
            default:
                break;
        }
    });
}

void CHTLGenerator::generateDocument(std::shared_ptr<CHTLDocumentNode> document) {
    writeHTML("<!DOCTYPE html>");
    writeHTML("<html lang=\"zh-CN\">");
    increaseIndent();
    
    // 生成头部
    generateDocumentHead(document);
    
    // 生成主体
    generateDocumentBody(document);
    
    decreaseIndent();
    writeHTML("</html>");
}

void CHTLGenerator::generateDocumentHead(std::shared_ptr<CHTLDocumentNode> document) {
    writeIndent();
    writeHTML("<head>");
    increaseIndent();
    
    writeIndent();
    writeHTML("<meta charset=\"" + config.outputCharset + "\">");
    writeIndent();
    writeHTML("<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">");
    writeIndent();
    writeHTML("<meta name=\"generator\" content=\"CHTL Compiler\">");
    
    // 生成标题
    writeIndent();
    writeHTML("<title>CHTL Generated Page</title>");
    
    // 如果有CSS，添加样式标签
    if (!cssStream.str().empty()) {
        writeIndent();
        writeHTML("<style>");
        writeHTML(cssStream.str());
        writeIndent();
        writeHTML("</style>");
    }
    
    decreaseIndent();
    writeIndent();
    writeHTML("</head>");
}

void CHTLGenerator::generateDocumentBody(std::shared_ptr<CHTLDocumentNode> document) {
    writeIndent();
    writeHTML("<body>");
    increaseIndent();
    
    // 生成use声明（作为注释）
    auto useDecls = document->getUseDeclarations();
    for (auto& useDecl : useDecls) {
        if (config.includeComments) {
            writeIndent();
            writeHTML("<!-- Use: " + useDecl->value + " -->");
        }
    }
    
    // 生成命名空间声明（作为注释）
    auto namespaceDecls = document->getNamespaceDeclarations();
    for (auto& nsDecl : namespaceDecls) {
        if (config.includeComments) {
            writeIndent();
            writeHTML("<!-- Namespace: " + nsDecl->name + " -->");
        }
    }
    
    // 生成主要内容
    for (auto& child : document->children) {
        if (child->type == CHTLNodeType::HTML_ELEMENT ||
            child->type == CHTLNodeType::TEXT_NODE ||
            child->type == CHTLNodeType::TEMPLATE_USAGE ||
            child->type == CHTLNodeType::CUSTOM_USAGE) {
            generateNode(child);
        }
    }
    
    decreaseIndent();
    writeIndent();
    writeHTML("</body>");
}

void CHTLGenerator::generateNode(std::shared_ptr<CHTLNode> node) {
    if (!node) return;
    
    // 检查缓存
    if (config.enableCaching) {
        std::string cacheKey = getCacheKey(node);
        if (hasCachedResult(cacheKey)) {
            writeHTML(getCachedResult(cacheKey));
            return;
        }
    }
    
    switch (node->type) {
        case CHTLNodeType::HTML_ELEMENT:
            if (auto element = std::dynamic_pointer_cast<CHTLElementNode>(node)) {
                generateElement(element);
            }
            break;
            
        case CHTLNodeType::TEXT_NODE:
            generateTextNode(node);
            break;
            
        case CHTLNodeType::TEMPLATE_DEFINITION:
            // 模板定义不直接生成内容
            break;
            
        case CHTLNodeType::TEMPLATE_USAGE:
            generateTemplateUsage(node);
            break;
            
        case CHTLNodeType::CUSTOM_DEFINITION:
            // 自定义定义不直接生成内容
            break;
            
        case CHTLNodeType::CUSTOM_USAGE:
            generateCustomUsage(node);
            break;
            
        case CHTLNodeType::STYLE_BLOCK:
            if (auto styleNode = std::dynamic_pointer_cast<CHTLStyleNode>(node)) {
                generateStyleBlock(styleNode);
            }
            break;
            
        case CHTLNodeType::SCRIPT_BLOCK:
            if (auto scriptNode = std::dynamic_pointer_cast<CHTLScriptNode>(node)) {
                generateScriptBlock(scriptNode);
            }
            break;
            
        case CHTLNodeType::LINE_COMMENT:
        case CHTLNodeType::BLOCK_COMMENT:
        case CHTLNodeType::GENERATOR_COMMENT:
            generateComment(node);
            break;
            
        default:
            // 递归处理子节点
            for (auto& child : node->children) {
                generateNode(child);
            }
            break;
    }
}

void CHTLGenerator::generateElement(std::shared_ptr<CHTLElementNode> element) {
    if (!element) return;
    
    std::string tagName = element->name;
    bool isVoid = isVoidElement(tagName);
    
    // 生成开始标签
    writeIndent();
    writeHTML("<" + tagName);
    
    // 生成属性
    generateElementAttributes(element);
    
    if (isVoid) {
        writeHTML(" />");
        return;
    } else {
        writeHTML(">");
    }
    
    // 检查是否有本地样式或脚本需要创建作用域
    bool hasLocalStyle = element->getStyleBlock() != nullptr;
    std::string scopeId;
    
    if (hasLocalStyle) {
        scopeId = createStyleScope(false);
        element->addClass("chtl-scope-" + scopeId);
    }
    
    // 生成子内容
    bool hasBlockContent = false;
    increaseIndent();
    
    for (auto& child : element->children) {
        if (child->type == CHTLNodeType::STYLE_BLOCK) {
            // 样式块单独处理
            if (auto styleNode = std::dynamic_pointer_cast<CHTLStyleNode>(child)) {
                generateStyleBlock(styleNode);
            }
        } else if (child->type == CHTLNodeType::SCRIPT_BLOCK) {
            // 脚本块单独处理
            if (auto scriptNode = std::dynamic_pointer_cast<CHTLScriptNode>(child)) {
                generateScriptBlock(scriptNode);
            }
        } else {
            generateNode(child);
            hasBlockContent = true;
        }
    }
    
    decreaseIndent();
    
    // 生成结束标签
    if (hasBlockContent) {
        writeIndent();
    }
    writeHTML("</" + tagName + ">");
    
    if (hasLocalStyle) {
        popStyleScope();
    }
}

void CHTLGenerator::generateElementAttributes(std::shared_ptr<CHTLElementNode> element) {
    for (const auto& attr : element->attributes) {
        if (!attr.second.empty()) {
            writeHTML(" " + attr.first + "=\"" + escapeHTML(attr.second) + "\"");
        } else {
            writeHTML(" " + attr.first);
        }
    }
}

void CHTLGenerator::generateTextNode(std::shared_ptr<CHTLNode> textNode) {
    if (!textNode || textNode->value.empty()) return;
    
    std::string text = variableResolver.interpolateString(textNode->value);
    text = escapeHTML(text);
    
    if (!isEmptyOrWhitespace(text)) {
        writeHTML(text);
    }
}

void CHTLGenerator::generateStyleBlock(std::shared_ptr<CHTLStyleNode> styleNode) {
    if (!styleNode) return;
    
    std::string scopeId = getCurrentStyleScope() ? getCurrentStyleScope()->getScopeId() : "";
    bool isLocal = styleNode->isLocal;
    
    for (auto& child : styleNode->children) {
        if (child->type == CHTLNodeType::STYLE_RULE) {
            generateStyleRule(child, isLocal ? scopeId : "");
        } else if (child->type == CHTLNodeType::STYLE_PROPERTY) {
            generateStyleProperty(child);
        }
    }
}

void CHTLGenerator::generateStyleRule(std::shared_ptr<CHTLNode> styleRule, const std::string& scopeId) {
    if (!styleRule) return;
    
    std::string selector = styleRule->name;
    
    // 处理选择器作用域
    if (!scopeId.empty()) {
        selector = processCHTLSelector(selector, scopeId);
    }
    
    writeCSS(selector + " {\n");
    
    // 生成属性
    for (auto& property : styleRule->children) {
        if (property->type == CHTLNodeType::STYLE_PROPERTY) {
            writeCSS("  " + property->name + ": " + property->value + ";\n");
        }
    }
    
    writeCSS("}\n\n");
}

void CHTLGenerator::generateStyleProperty(std::shared_ptr<CHTLNode> styleProperty) {
    if (!styleProperty) return;
    
    std::string property = normalizeCSSProperty(styleProperty->name);
    std::string value = normalizeCSSValue(styleProperty->value);
    
    if (!property.empty() && !value.empty()) {
        writeCSS("  " + property + ": " + value + ";\n");
    }
}

void CHTLGenerator::generateScriptBlock(std::shared_ptr<CHTLScriptNode> scriptNode) {
    if (!scriptNode) return;
    
    std::string scriptContent = scriptNode->getScriptContent();
    if (!scriptContent.empty()) {
        writeJS(scriptContent + "\n");
    }
}

void CHTLGenerator::generateComment(std::shared_ptr<CHTLNode> commentNode) {
    if (!commentNode || !config.includeComments) return;
    
    std::string comment = commentNode->value;
    switch (commentNode->type) {
        case CHTLNodeType::LINE_COMMENT:
            writeIndent();
            writeHTML("<!-- " + escapeHTML(comment) + " -->");
            break;
            
        case CHTLNodeType::BLOCK_COMMENT:
            writeIndent();
            writeHTML("<!--\n" + escapeHTML(comment) + "\n-->");
            break;
            
        case CHTLNodeType::GENERATOR_COMMENT:
            // 生成器注释不输出到最终HTML
            break;
            
        default:
            break;
    }
}

// === 模板处理 ===

void CHTLGenerator::generateTemplateUsage(std::shared_ptr<CHTLNode> templateUsage) {
    // TODO: 实现模板使用生成
    addWarning("模板使用功能尚未实现");
}

void CHTLGenerator::generateCustomUsage(std::shared_ptr<CHTLNode> customUsage) {
    // TODO: 实现自定义使用生成
    addWarning("自定义使用功能尚未实现");
}

// === 辅助方法实现 ===

std::string CHTLGenerator::createStyleScope(bool isGlobal) {
    std::string scopeId = "scope" + std::to_string(nextScopeId++);
    return scopeId;
}

CHTLStyleScope* CHTLGenerator::getCurrentStyleScope() {
    return styleScopes.empty() ? nullptr : styleScopes.back().get();
}

void CHTLGenerator::pushStyleScope(std::unique_ptr<CHTLStyleScope> scope) {
    styleScopes.push_back(std::move(scope));
}

void CHTLGenerator::popStyleScope() {
    if (!styleScopes.empty()) {
        styleScopes.pop_back();
    }
}

std::string CHTLGenerator::processCHTLSelector(const std::string& selector, const std::string& scopeId) {
    if (selector.empty()) return selector;
    
    // 处理&选择器（父选择器）
    if (selector[0] == '&') {
        return ".chtl-scope-" + scopeId + selector.substr(1);
    }
    
    // 处理类选择器
    if (selector[0] == '.') {
        return ".chtl-scope-" + scopeId + " " + selector;
    }
    
    // 处理ID选择器
    if (selector[0] == '#') {
        return ".chtl-scope-" + scopeId + " " + selector;
    }
    
    // 普通元素选择器
    return ".chtl-scope-" + scopeId + " " + selector;
}

void CHTLGenerator::processVariableDefinitions(std::shared_ptr<CHTLNode> node) {
    // 简单的变量处理
    if (node->type == CHTLNodeType::VARIABLE_DEFINITION) {
        variableResolver.setVariable(node->name, node->value);
    }
}

// === 输出格式化 ===

void CHTLGenerator::writeIndent() {
    if (!config.minifyOutput) {
        htmlStream << std::string(indentLevel * config.indentString.length(), ' ');
    }
}

void CHTLGenerator::increaseIndent() {
    indentLevel++;
}

void CHTLGenerator::decreaseIndent() {
    if (indentLevel > 0) {
        indentLevel--;
    }
}

void CHTLGenerator::writeLine(const std::string& content) {
    writeHTML(content);
    if (!config.minifyOutput) {
        htmlStream << "\n";
    }
}

void CHTLGenerator::writeHTML(const std::string& content) {
    htmlStream << content;
}

void CHTLGenerator::writeCSS(const std::string& content) {
    cssStream << content;
}

void CHTLGenerator::writeJS(const std::string& content) {
    jsStream << content;
}

// === 错误处理 ===

void CHTLGenerator::addError(const std::string& message) {
    errors.push_back("[CHTLGenerator] " + message);
}

void CHTLGenerator::addWarning(const std::string& message) {
    warnings.push_back("[CHTLGenerator] " + message);
}

// === 验证和优化 ===

bool CHTLGenerator::validateHTML(const std::string& html) {
    // 简单的HTML验证
    return !html.empty() && html.find("<!DOCTYPE html>") != std::string::npos;
}

bool CHTLGenerator::validateCSS(const std::string& css) {
    // 简单的CSS验证
    return true; // 暂时返回true
}

std::string CHTLGenerator::optimizeHTML(const std::string& html) {
    std::string result = html;
    
    // 移除多余的空白
    std::regex extraSpaces(R"(\s+)");
    result = std::regex_replace(result, extraSpaces, " ");
    
    // 移除注释中的多余空白
    std::regex commentSpaces(R"(<!--\s+([^>]+)\s+-->)");
    result = std::regex_replace(result, commentSpaces, "<!-- $1 -->");
    
    return result;
}

std::string CHTLGenerator::optimizeCSS(const std::string& css) {
    std::string result = css;
    
    // 移除多余的空白和换行
    std::regex extraSpaces(R"(\s+)");
    result = std::regex_replace(result, extraSpaces, " ");
    
    return result;
}

std::string CHTLGenerator::minifyHTML(const std::string& html) {
    std::string result = html;
    
    // 移除换行和多余空白
    std::regex newlines(R"(\n\s*)");
    result = std::regex_replace(result, newlines, "");
    
    return result;
}

std::string CHTLGenerator::minifyCSS(const std::string& css) {
    std::string result = css;
    
    // 移除换行和空白
    std::regex whitespace(R"(\s+)");
    result = std::regex_replace(result, whitespace, "");
    
    // 在特定字符后添加必要的空格
    std::regex addSpace(R"(([{}:;,]))");
    result = std::regex_replace(result, addSpace, "$1 ");
    
    return result;
}

// === 缓存管理 ===

std::string CHTLGenerator::getCacheKey(std::shared_ptr<CHTLNode> node) {
    return node->getTypeName() + "_" + node->name + "_" + std::to_string(reinterpret_cast<uintptr_t>(node.get()));
}

bool CHTLGenerator::hasCachedResult(const std::string& key) {
    return generationCache.find(key) != generationCache.end();
}

std::string CHTLGenerator::getCachedResult(const std::string& key) {
    auto it = generationCache.find(key);
    return it != generationCache.end() ? it->second : "";
}

void CHTLGenerator::setCachedResult(const std::string& key, const std::string& result) {
    generationCache[key] = result;
}

// === HTML实体处理 ===

std::string CHTLGenerator::escapeHTML(const std::string& text) {
    std::string result = text;
    
    std::regex ampersand("&");
    result = std::regex_replace(result, ampersand, "&amp;");
    
    std::regex lessThan("<");
    result = std::regex_replace(result, lessThan, "&lt;");
    
    std::regex greaterThan(">");
    result = std::regex_replace(result, greaterThan, "&gt;");
    
    std::regex quote("\"");
    result = std::regex_replace(result, quote, "&quot;");
    
    std::regex apostrophe("'");
    result = std::regex_replace(result, apostrophe, "&#x27;");
    
    return result;
}

// === CSS处理 ===

std::string CHTLGenerator::normalizeCSSProperty(const std::string& property) {
    std::string result = trim(property);
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

std::string CHTLGenerator::normalizeCSSValue(const std::string& value) {
    return variableResolver.interpolateString(trim(value));
}

bool CHTLGenerator::isValidCSSProperty(const std::string& property) {
    // 简单验证
    return !property.empty() && property.find(':') == std::string::npos;
}

// === 工具方法 ===

std::string CHTLGenerator::generateUniqueId(const std::string& prefix) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(1000, 9999);
    
    return prefix + "_" + std::to_string(dis(gen));
}

bool CHTLGenerator::isEmptyOrWhitespace(const std::string& str) {
    return str.empty() || std::all_of(str.begin(), str.end(), ::isspace);
}

std::string CHTLGenerator::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\n\r");
    if (start == std::string::npos) return "";
    
    size_t end = str.find_last_not_of(" \t\n\r");
    return str.substr(start, end - start + 1);
}

void CHTLGenerator::generateGlobalStyles() {
    // 生成全局样式
}

void CHTLGenerator::generateGlobalScripts() {
    // 生成全局脚本
}

// === 静态HTML标签验证 ===

const std::vector<std::string>& CHTLGenerator::getVoidElements() {
    static std::vector<std::string> voidElements = {
        "area", "base", "br", "col", "embed", "hr", "img", "input",
        "link", "meta", "param", "source", "track", "wbr"
    };
    return voidElements;
}

bool CHTLGenerator::isVoidElement(const std::string& tagName) {
    const auto& voidElements = getVoidElements();
    return std::find(voidElements.begin(), voidElements.end(), tagName) != voidElements.end();
}

// === 辅助方法 ===

CHTLGenerationResult CHTLGenerator::createFailedResult() {
    CHTLGenerationResult result;
    result.success = false;
    result.errors = errors;
    result.warnings = warnings;
    return result;
}

} // namespace CHTL