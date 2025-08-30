#include "OriginNode.h"
#include "../CHTLLexer/GlobalMap.h"
#include <algorithm>
#include <sstream>
#include <iostream>
#include <regex>
#include <fstream>

namespace CHTL {

// OriginNode基类实现
bool OriginNode::validateContent() const {
    if (rawContent.empty()) {
        return false;
    }
    
    // 根据类型进行基础验证
    if (isHtmlOrigin()) {
        return OriginValidator::validateHTML(rawContent);
    } else if (isStyleOrigin()) {
        return OriginValidator::validateCSS(rawContent);
    } else if (isJavaScriptOrigin()) {
        return OriginValidator::validateJavaScript(rawContent);
    } else if (isCustomOrigin()) {
        // 自定义类型验证
        if (OriginManager::hasValidator(originType)) {
            // 使用注册的验证器
            return true; // 简化实现
        }
        return true; // 默认通过
    }
    
    return true;
}

StringVector OriginNode::getContentValidationErrors() const {
    StringVector errors;
    
    if (rawContent.empty()) {
        errors.push_back("原始内容不能为空");
        return errors;
    }
    
    // 根据类型获取详细验证错误
    if (isHtmlOrigin()) {
        auto htmlErrors = OriginValidator::getHTMLValidationErrors(rawContent);
        errors.insert(errors.end(), htmlErrors.begin(), htmlErrors.end());
    } else if (isStyleOrigin()) {
        auto cssErrors = OriginValidator::getCSSValidationErrors(rawContent);
        errors.insert(errors.end(), cssErrors.begin(), cssErrors.end());
    } else if (isJavaScriptOrigin()) {
        auto jsErrors = OriginValidator::getJavaScriptValidationErrors(rawContent);
        errors.insert(errors.end(), jsErrors.begin(), jsErrors.end());
    }
    
    return errors;
}

String OriginNode::generateOutput() const {
    return rawContent;
}

String OriginNode::getOutputType() const {
    if (isHtmlOrigin()) return "text/html";
    if (isStyleOrigin()) return "text/css";
    if (isJavaScriptOrigin()) return "application/javascript";
    return "text/plain";
}

String OriginNode::toString() const {
    std::ostringstream oss;
    oss << "[Origin] @" << originType;
    if (isNamed) {
        oss << " " << originName;
    }
    oss << " (" << rawContent.length() << " characters)";
    return oss.str();
}

std::unique_ptr<BaseNode> OriginNode::clone() const {
    auto cloned = std::make_unique<OriginNode>(type, originType, rawContent, originName, position);
    cloned->isCustomType = isCustomType;
    cloned->metadata = metadata;
    
    // 克隆子节点
    for (const auto& child : children) {
        cloned->addChild(child->clone());
    }
    
    return std::unique_ptr<BaseNode>(cloned.release());
}

bool OriginNode::validate() const {
    if (originType.empty()) {
        return false;
    }
    return validateContent();
}

StringVector OriginNode::getValidationErrors() const {
    StringVector errors = BaseNode::getValidationErrors();
    auto contentErrors = getContentValidationErrors();
    errors.insert(errors.end(), contentErrors.begin(), contentErrors.end());
    return errors;
}

// HtmlOriginNode实现
bool HtmlOriginNode::validateContent() const {
    return OriginValidator::validateHTML(rawContent);
}

StringVector HtmlOriginNode::getContentValidationErrors() const {
    return OriginValidator::getHTMLValidationErrors(rawContent);
}

bool HtmlOriginNode::isWellFormed() const {
    // 简化的格式检查
    std::regex openTag(R"(<[^/>]+>)");
    std::regex closeTag(R"(</[^>]+>)");
    std::regex selfCloseTag(R"(<[^>]+/>)");
    
    std::sregex_iterator openBegin(rawContent.begin(), rawContent.end(), openTag);
    std::sregex_iterator openEnd;
    std::sregex_iterator closeBegin(rawContent.begin(), rawContent.end(), closeTag);
    std::sregex_iterator closeEnd;
    
    size_t openCount = std::distance(openBegin, openEnd);
    size_t closeCount = std::distance(closeBegin, closeEnd);
    
    // 简单检查：开标签数量应该等于闭标签数量（忽略自闭合标签）
    return openCount == closeCount;
}

StringVector HtmlOriginNode::extractTags() const {
    StringVector tags;
    std::regex tagRegex(R"(<([^/>]+)>)");
    std::sregex_iterator begin(rawContent.begin(), rawContent.end(), tagRegex);
    std::sregex_iterator end;
    
    for (std::sregex_iterator i = begin; i != end; ++i) {
        String tag = (*i)[1].str();
        // 提取标签名（忽略属性）
        size_t spacePos = tag.find(' ');
        if (spacePos != String::npos) {
            tag = tag.substr(0, spacePos);
        }
        tags.push_back(tag);
    }
    
    return tags;
}

bool HtmlOriginNode::hasScriptTags() const {
    return rawContent.find("<script") != String::npos;
}

bool HtmlOriginNode::hasStyleTags() const {
    return rawContent.find("<style") != String::npos;
}

String HtmlOriginNode::toString() const {
    std::ostringstream oss;
    oss << "[Origin] @Html";
    if (isNamed) {
        oss << " " << originName;
    }
    oss << " (" << rawContent.length() << " chars";
    if (hasScriptTags()) oss << ", has script";
    if (hasStyleTags()) oss << ", has style";
    oss << ")";
    return oss.str();
}

std::unique_ptr<BaseNode> HtmlOriginNode::clone() const {
    auto cloned = std::make_unique<HtmlOriginNode>(rawContent, originName, position);
    cloned->metadata = metadata;
    
    // 克隆子节点
    for (const auto& child : children) {
        cloned->addChild(child->clone());
    }
    
    return std::unique_ptr<BaseNode>(cloned.release());
}

// StyleOriginNode实现
bool StyleOriginNode::validateContent() const {
    return OriginValidator::validateCSS(rawContent);
}

StringVector StyleOriginNode::getContentValidationErrors() const {
    return OriginValidator::getCSSValidationErrors(rawContent);
}

bool StyleOriginNode::isValidCSS() const {
    // 简化的CSS有效性检查
    return !rawContent.empty() && rawContent.find('{') != String::npos;
}

StringVector StyleOriginNode::extractSelectors() const {
    StringVector selectors;
    std::regex selectorRegex(R"(([^{]+)\s*\{)");
    std::sregex_iterator begin(rawContent.begin(), rawContent.end(), selectorRegex);
    std::sregex_iterator end;
    
    for (std::sregex_iterator i = begin; i != end; ++i) {
        String selector = (*i)[1].str();
        // 移除前后空白
        selector.erase(0, selector.find_first_not_of(" \t\n\r"));
        selector.erase(selector.find_last_not_of(" \t\n\r") + 1);
        selectors.push_back(selector);
    }
    
    return selectors;
}

StringVector StyleOriginNode::extractProperties() const {
    StringVector properties;
    std::regex propertyRegex(R"(([^:{]+):\s*([^;]+);)");
    std::sregex_iterator begin(rawContent.begin(), rawContent.end(), propertyRegex);
    std::sregex_iterator end;
    
    for (std::sregex_iterator i = begin; i != end; ++i) {
        String property = (*i)[1].str();
        // 移除前后空白
        property.erase(0, property.find_first_not_of(" \t\n\r"));
        property.erase(property.find_last_not_of(" \t\n\r") + 1);
        properties.push_back(property);
    }
    
    return properties;
}

bool StyleOriginNode::hasMediaQueries() const {
    return rawContent.find("@media") != String::npos;
}

bool StyleOriginNode::hasKeyframes() const {
    return rawContent.find("@keyframes") != String::npos || 
           rawContent.find("@-webkit-keyframes") != String::npos;
}

String StyleOriginNode::minifyCSS() const {
    String minified = rawContent;
    
    // 移除注释
    std::regex commentRegex(R"(/\*.*?\*/)");
    minified = std::regex_replace(minified, commentRegex, "");
    
    // 移除多余的空白
    std::regex whitespaceRegex(R"(\s+)");
    minified = std::regex_replace(minified, whitespaceRegex, " ");
    
    // 移除分号前的空格
    std::regex semicolonRegex(R"(\s*;\s*)");
    minified = std::regex_replace(minified, semicolonRegex, ";");
    
    // 移除花括号周围的空格
    std::regex braceRegex(R"(\s*{\s*)");
    minified = std::regex_replace(minified, braceRegex, "{");
    std::regex closeBraceRegex(R"(\s*}\s*)");
    minified = std::regex_replace(minified, closeBraceRegex, "}");
    
    return minified;
}

String StyleOriginNode::addVendorPrefixes() const {
    String prefixed = rawContent;
    
    // 简化的供应商前缀添加
    StringUnorderedMap prefixProperties = {
        {"transform", "-webkit-transform,-moz-transform,-ms-transform"},
        {"transition", "-webkit-transition,-moz-transition,-ms-transition"},
        {"animation", "-webkit-animation,-moz-animation,-ms-animation"},
        {"box-shadow", "-webkit-box-shadow,-moz-box-shadow"},
        {"border-radius", "-webkit-border-radius,-moz-border-radius"}
    };
    
    for (const auto& prop : prefixProperties) {
        std::regex propRegex("([^-])" + prop.first + ":");
        String replacement = "$1";
        
        std::istringstream iss(prop.second);
        String prefix;
        while (std::getline(iss, prefix, ',')) {
            replacement += prefix + ":$2;" + prop.first + ":";
        }
        
        prefixed = std::regex_replace(prefixed, propRegex, replacement);
    }
    
    return prefixed;
}

String StyleOriginNode::generateOutput() const {
    String output = rawContent;
    
    // 根据元数据决定是否应用优化
    if (getMetadata("minify") == "true") {
        output = minifyCSS();
    }
    
    if (getMetadata("autoprefixer") == "true") {
        output = addVendorPrefixes();
    }
    
    return output;
}

String StyleOriginNode::toString() const {
    std::ostringstream oss;
    oss << "[Origin] @Style";
    if (isNamed) {
        oss << " " << originName;
    }
    oss << " (" << rawContent.length() << " chars";
    if (hasMediaQueries()) oss << ", media queries";
    if (hasKeyframes()) oss << ", keyframes";
    oss << ")";
    return oss.str();
}

std::unique_ptr<BaseNode> StyleOriginNode::clone() const {
    auto cloned = std::make_unique<StyleOriginNode>(rawContent, originName, position);
    cloned->metadata = metadata;
    
    // 克隆子节点
    for (const auto& child : children) {
        cloned->addChild(child->clone());
    }
    
    return std::unique_ptr<BaseNode>(cloned.release());
}

// JavaScriptOriginNode实现
bool JavaScriptOriginNode::validateContent() const {
    return OriginValidator::validateJavaScript(rawContent);
}

StringVector JavaScriptOriginNode::getContentValidationErrors() const {
    return OriginValidator::getJavaScriptValidationErrors(rawContent);
}

bool JavaScriptOriginNode::isValidJavaScript() const {
    // 简化的JavaScript有效性检查
    return !rawContent.empty();
}

StringVector JavaScriptOriginNode::extractFunctions() const {
    StringVector functions;
    std::regex funcRegex(R"(function\s+([a-zA-Z_$][a-zA-Z0-9_$]*)\s*\()");
    std::sregex_iterator begin(rawContent.begin(), rawContent.end(), funcRegex);
    std::sregex_iterator end;
    
    for (std::sregex_iterator i = begin; i != end; ++i) {
        functions.push_back((*i)[1].str());
    }
    
    // 箭头函数
    std::regex arrowFuncRegex(R"(const\s+([a-zA-Z_$][a-zA-Z0-9_$]*)\s*=\s*\([^)]*\)\s*=>)");
    std::sregex_iterator arrowBegin(rawContent.begin(), rawContent.end(), arrowFuncRegex);
    
    for (std::sregex_iterator i = arrowBegin; i != end; ++i) {
        functions.push_back((*i)[1].str());
    }
    
    return functions;
}

StringVector JavaScriptOriginNode::extractVariables() const {
    StringVector variables;
    std::regex varRegex(R"((var|let|const)\s+([a-zA-Z_$][a-zA-Z0-9_$]*))");
    std::sregex_iterator begin(rawContent.begin(), rawContent.end(), varRegex);
    std::sregex_iterator end;
    
    for (std::sregex_iterator i = begin; i != end; ++i) {
        variables.push_back((*i)[2].str());
    }
    
    return variables;
}

bool JavaScriptOriginNode::hasESModuleSyntax() const {
    return rawContent.find("import ") != String::npos || 
           rawContent.find("export ") != String::npos;
}

bool JavaScriptOriginNode::hasCommonJSSyntax() const {
    return rawContent.find("require(") != String::npos || 
           rawContent.find("module.exports") != String::npos;
}

String JavaScriptOriginNode::minifyJavaScript() const {
    String minified = rawContent;
    
    // 移除单行注释
    std::regex singleCommentRegex(R"(//.*$)", std::regex_constants::multiline);
    minified = std::regex_replace(minified, singleCommentRegex, "");
    
    // 移除多行注释
    std::regex multiCommentRegex(R"(/\*.*?\*/)");
    minified = std::regex_replace(minified, multiCommentRegex, "");
    
    // 移除多余的空白（保留字符串内容）
    std::regex whitespaceRegex(R"(\s+)");
    minified = std::regex_replace(minified, whitespaceRegex, " ");
    
    return minified;
}

String JavaScriptOriginNode::addStrictMode() const {
    if (rawContent.find("'use strict'") == String::npos && 
        rawContent.find("\"use strict\"") == String::npos) {
        return "'use strict';\n" + rawContent;
    }
    return rawContent;
}

String JavaScriptOriginNode::toString() const {
    std::ostringstream oss;
    oss << "[Origin] @JavaScript";
    if (isNamed) {
        oss << " " << originName;
    }
    oss << " (" << rawContent.length() << " chars";
    if (hasESModuleSyntax()) oss << ", ES modules";
    if (hasCommonJSSyntax()) oss << ", CommonJS";
    oss << ")";
    return oss.str();
}

std::unique_ptr<BaseNode> JavaScriptOriginNode::clone() const {
    auto cloned = std::make_unique<JavaScriptOriginNode>(rawContent, originName, position);
    cloned->metadata = metadata;
    
    // 克隆子节点
    for (const auto& child : children) {
        cloned->addChild(child->clone());
    }
    
    return std::unique_ptr<BaseNode>(cloned.release());
}

// CustomOriginNode实现
bool CustomOriginNode::validateContent() const {
    if (OriginManager::hasValidator(originType)) {
        // 使用注册的验证器
        return true; // 简化实现
    }
    return !rawContent.empty();
}

StringVector CustomOriginNode::getContentValidationErrors() const {
    StringVector errors;
    if (rawContent.empty()) {
        errors.push_back("自定义原始内容不能为空");
    }
    return errors;
}

String CustomOriginNode::generateOutput() const {
    if (contentProcessor) {
        return contentProcessor(rawContent);
    }
    return rawContent;
}

String CustomOriginNode::toString() const {
    std::ostringstream oss;
    oss << "[Origin] @" << originType;
    if (isNamed) {
        oss << " " << originName;
    }
    oss << " (" << rawContent.length() << " chars, custom type)";
    return oss.str();
}

std::unique_ptr<BaseNode> CustomOriginNode::clone() const {
    auto cloned = std::make_unique<CustomOriginNode>(originType, rawContent, originName, position);
    cloned->customProperties = customProperties;
    cloned->contentProcessor = contentProcessor;
    cloned->outputMimeType = outputMimeType;
    cloned->metadata = metadata;
    
    // 克隆子节点
    for (const auto& child : children) {
        cloned->addChild(child->clone());
    }
    
    return std::unique_ptr<BaseNode>(cloned.release());
}

// OriginManager静态成员初始化
StringUnorderedMap OriginManager::registeredTypes;
std::unordered_map<String, std::function<String(const String&)>> OriginManager::contentProcessors;
std::unordered_map<String, std::function<bool(const String&)>> OriginManager::validators;

// OriginManager实现
void OriginManager::registerCustomType(const String& typeName, const String& defaultMimeType) {
    registeredTypes[typeName] = defaultMimeType;
}

void OriginManager::unregisterCustomType(const String& typeName) {
    registeredTypes.erase(typeName);
    contentProcessors.erase(typeName);
    validators.erase(typeName);
}

bool OriginManager::isRegisteredCustomType(const String& typeName) {
    return registeredTypes.find(typeName) != registeredTypes.end();
}

StringVector OriginManager::getRegisteredCustomTypes() {
    StringVector types;
    for (const auto& pair : registeredTypes) {
        types.push_back(pair.first);
    }
    return types;
}

void OriginManager::registerContentProcessor(const String& typeName, std::function<String(const String&)> processor) {
    contentProcessors[typeName] = processor;
}

void OriginManager::unregisterContentProcessor(const String& typeName) {
    contentProcessors.erase(typeName);
}

bool OriginManager::hasContentProcessor(const String& typeName) {
    return contentProcessors.find(typeName) != contentProcessors.end();
}

void OriginManager::registerValidator(const String& typeName, std::function<bool(const String&)> validator) {
    validators[typeName] = validator;
}

void OriginManager::unregisterValidator(const String& typeName) {
    validators.erase(typeName);
}

bool OriginManager::hasValidator(const String& typeName) {
    return validators.find(typeName) != validators.end();
}

std::unique_ptr<OriginNode> OriginManager::createOriginNode(const String& type, const String& content, const String& name) {
    if (type == "Html") {
        return std::make_unique<HtmlOriginNode>(content, name);
    } else if (type == "Style") {
        return std::make_unique<StyleOriginNode>(content, name);
    } else if (type == "JavaScript") {
        return std::make_unique<JavaScriptOriginNode>(content, name);
    } else if (isRegisteredCustomType(type)) {
        return std::make_unique<CustomOriginNode>(type, content, name);
    }
    
    // 默认创建自定义类型
    return std::make_unique<CustomOriginNode>(type, content, name);
}

// OriginValidator实现
bool OriginValidator::validateHTML(const String& content) {
    if (content.empty()) return false;
    
    // 基础HTML验证
    return !hasXSSVulnerabilities(content);
}

bool OriginValidator::validateCSS(const String& content) {
    if (content.empty()) return false;
    
    // 基础CSS验证
    return !hasCSSInjection(content);
}

bool OriginValidator::validateJavaScript(const String& content) {
    if (content.empty()) return false;
    
    // 基础JavaScript验证
    return !hasJSInjection(content);
}

StringVector OriginValidator::getHTMLValidationErrors(const String& content) {
    StringVector errors;
    if (content.empty()) {
        errors.push_back("HTML内容不能为空");
    }
    if (hasXSSVulnerabilities(content)) {
        errors.push_back("检测到潜在的XSS漏洞");
    }
    return errors;
}

StringVector OriginValidator::getCSSValidationErrors(const String& content) {
    StringVector errors;
    if (content.empty()) {
        errors.push_back("CSS内容不能为空");
    }
    if (hasCSSInjection(content)) {
        errors.push_back("检测到潜在的CSS注入");
    }
    return errors;
}

StringVector OriginValidator::getJavaScriptValidationErrors(const String& content) {
    StringVector errors;
    if (content.empty()) {
        errors.push_back("JavaScript内容不能为空");
    }
    if (hasJSInjection(content)) {
        errors.push_back("检测到潜在的JavaScript注入");
    }
    return errors;
}

bool OriginValidator::hasXSSVulnerabilities(const String& htmlContent) {
    // 简化的XSS检测
    StringVector dangerousPatterns = {
        "<script", "javascript:", "onload=", "onerror=", "onclick="
    };
    
    for (const auto& pattern : dangerousPatterns) {
        if (htmlContent.find(pattern) != String::npos) {
            return true;
        }
    }
    
    return false;
}

bool OriginValidator::hasCSSInjection(const String& cssContent) {
    // 简化的CSS注入检测
    StringVector dangerousPatterns = {
        "expression(", "javascript:", "behavior:", "@import"
    };
    
    for (const auto& pattern : dangerousPatterns) {
        if (cssContent.find(pattern) != String::npos) {
            return true;
        }
    }
    
    return false;
}

bool OriginValidator::hasJSInjection(const String& jsContent) {
    // 简化的JS注入检测
    StringVector dangerousPatterns = {
        "eval(", "setTimeout(", "setInterval(", "Function("
    };
    
    for (const auto& pattern : dangerousPatterns) {
        if (jsContent.find(pattern) != String::npos) {
            return true;
        }
    }
    
    return false;
}

bool OriginValidator::hasPerformanceIssues(const String& content, const String& type) {
    // 简化的性能问题检测
    if (type == "JavaScript") {
        return content.find("document.write") != String::npos;
    }
    return false;
}

StringVector OriginValidator::getPerformanceWarnings(const String& content, const String& type) {
    StringVector warnings;
    
    if (type == "JavaScript" && content.find("document.write") != String::npos) {
        warnings.push_back("使用document.write可能影响性能");
    }
    
    return warnings;
}

// OriginFactory实现
std::unique_ptr<HtmlOriginNode> OriginFactory::createHtmlOrigin(const String& content, const String& name) {
    return std::make_unique<HtmlOriginNode>(content, name);
}

std::unique_ptr<StyleOriginNode> OriginFactory::createStyleOrigin(const String& content, const String& name) {
    return std::make_unique<StyleOriginNode>(content, name);
}

std::unique_ptr<JavaScriptOriginNode> OriginFactory::createJavaScriptOrigin(const String& content, const String& name) {
    return std::make_unique<JavaScriptOriginNode>(content, name);
}

std::unique_ptr<CustomOriginNode> OriginFactory::createCustomOrigin(const String& type, const String& content, const String& name) {
    return std::make_unique<CustomOriginNode>(type, content, name);
}

String OriginFactory::detectTypeFromExtension(const String& filePath) {
    size_t dotPos = filePath.find_last_of('.');
    if (dotPos == String::npos) return "Html";
    
    String ext = filePath.substr(dotPos + 1);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == "html" || ext == "htm") return "Html";
    if (ext == "css") return "Style";
    if (ext == "js") return "JavaScript";
    
    return "Html"; // 默认
}

String OriginFactory::readFileContent(const String& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        return "";
    }
    
    std::ostringstream oss;
    oss << file.rdbuf();
    return oss.str();
}

} // namespace CHTL