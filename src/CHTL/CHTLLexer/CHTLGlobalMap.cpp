#include "CHTLGlobalMap.h"
#include <algorithm>
#include <fstream>
#include <sstream>

namespace CHTL {

// CHTLKeywordMap 实现
CHTLKeywordMap::CHTLKeywordMap() {
    initializeDefaultKeywords();
}

void CHTLKeywordMap::initializeDefaultKeywords() {
    // 基础关键字
    keywordMap["text"] = CHTLTokenType::TEXT;
    keywordMap["style"] = CHTLTokenType::STYLE;
    keywordMap["script"] = CHTLTokenType::SCRIPT;
    keywordMap["inherit"] = CHTLTokenType::INHERIT;
    keywordMap["delete"] = CHTLTokenType::DELETE;
    keywordMap["insert"] = CHTLTokenType::INSERT;
    keywordMap["after"] = CHTLTokenType::AFTER;
    keywordMap["before"] = CHTLTokenType::BEFORE;
    keywordMap["replace"] = CHTLTokenType::REPLACE;
    keywordMap["from"] = CHTLTokenType::FROM;
    keywordMap["as"] = CHTLTokenType::AS;
    keywordMap["except"] = CHTLTokenType::EXCEPT;
    keywordMap["use"] = CHTLTokenType::USE;
    keywordMap["html5"] = CHTLTokenType::HTML5;
    
    // 复合关键字
    keywordMap["at top"] = CHTLTokenType::AT_TOP;
    keywordMap["at bottom"] = CHTLTokenType::AT_BOTTOM;
    
    // 标记关键字
    keywordMap["[Template]"] = CHTLTokenType::TEMPLATE;
    keywordMap["[Custom]"] = CHTLTokenType::CUSTOM;
    keywordMap["[Origin]"] = CHTLTokenType::ORIGIN;
    keywordMap["[Import]"] = CHTLTokenType::IMPORT;
    keywordMap["[Namespace]"] = CHTLTokenType::NAMESPACE;
    keywordMap["[Configuration]"] = CHTLTokenType::CONFIGURATION;
    keywordMap["[Info]"] = CHTLTokenType::INFO;
    keywordMap["[Export]"] = CHTLTokenType::EXPORT;
    
    // 类型标记
    keywordMap["@Style"] = CHTLTokenType::AT_STYLE;
    keywordMap["@Element"] = CHTLTokenType::AT_ELEMENT;
    keywordMap["@Var"] = CHTLTokenType::AT_VAR;
    keywordMap["@Html"] = CHTLTokenType::AT_HTML;
    keywordMap["@JavaScript"] = CHTLTokenType::AT_JAVASCRIPT;
    keywordMap["@Chtl"] = CHTLTokenType::AT_CHTL;
    keywordMap["@CJmod"] = CHTLTokenType::AT_CJMOD;
    keywordMap["@Config"] = CHTLTokenType::AT_CONFIG;
}

bool CHTLKeywordMap::isKeyword(const std::string& text) const {
    return keywordMap.find(text) != keywordMap.end();
}

CHTLTokenType CHTLKeywordMap::getKeywordType(const std::string& text) const {
    auto it = keywordMap.find(text);
    return it != keywordMap.end() ? it->second : CHTLTokenType::IDENTIFIER;
}

void CHTLKeywordMap::addKeyword(const std::string& keyword, CHTLTokenType type) {
    keywordMap[keyword] = type;
}

void CHTLKeywordMap::removeKeyword(const std::string& keyword) {
    keywordMap.erase(keyword);
}

std::vector<std::string> CHTLKeywordMap::getAllKeywords() const {
    std::vector<std::string> keywords;
    keywords.reserve(keywordMap.size());
    for (const auto& pair : keywordMap) {
        keywords.push_back(pair.first);
    }
    return keywords;
}

// CHTLHtmlTagMap 实现
CHTLHtmlTagMap::CHTLHtmlTagMap() {
    initializeHtmlTags();
    initializeSelfClosingTags();
    initializeBlockElements();
    initializeInlineElements();
}

void CHTLHtmlTagMap::initializeHtmlTags() {
    // HTML5 标准标签
    std::vector<std::string> tags = {
        "html", "head", "title", "meta", "link", "style", "script", "noscript",
        "body", "section", "nav", "article", "aside", "h1", "h2", "h3", "h4", "h5", "h6",
        "header", "footer", "address", "main", "p", "hr", "pre", "blockquote",
        "ol", "ul", "li", "dl", "dt", "dd", "figure", "figcaption", "div",
        "a", "em", "strong", "small", "s", "cite", "q", "dfn", "abbr", "data",
        "time", "code", "var", "samp", "kbd", "sub", "sup", "i", "b", "u",
        "mark", "ruby", "rt", "rp", "bdi", "bdo", "span", "br", "wbr",
        "ins", "del", "img", "iframe", "embed", "object", "param", "video",
        "audio", "source", "track", "canvas", "map", "area", "svg", "math",
        "table", "caption", "colgroup", "col", "tbody", "thead", "tfoot",
        "tr", "td", "th", "form", "fieldset", "legend", "label", "input",
        "button", "select", "datalist", "optgroup", "option", "textarea",
        "keygen", "output", "progress", "meter", "details", "summary",
        "menuitem", "menu"
    };
    
    for (const auto& tag : tags) {
        htmlTags.insert(tag);
    }
}

void CHTLHtmlTagMap::initializeSelfClosingTags() {
    std::vector<std::string> selfClosing = {
        "area", "base", "br", "col", "embed", "hr", "img", "input",
        "keygen", "link", "meta", "param", "source", "track", "wbr"
    };
    
    for (const auto& tag : selfClosing) {
        selfClosingTags.insert(tag);
    }
}

void CHTLHtmlTagMap::initializeBlockElements() {
    std::vector<std::string> blockElems = {
        "address", "article", "aside", "blockquote", "details", "dialog", "dd", "div",
        "dl", "dt", "fieldset", "figcaption", "figure", "footer", "form", "h1", "h2",
        "h3", "h4", "h5", "h6", "header", "hgroup", "hr", "li", "main", "nav", "ol",
        "p", "pre", "section", "table", "ul"
    };
    
    for (const auto& elem : blockElems) {
        blockElements.insert(elem);
    }
}

void CHTLHtmlTagMap::initializeInlineElements() {
    std::vector<std::string> inlineElems = {
        "a", "abbr", "acronym", "b", "bdi", "bdo", "big", "br", "button", "cite",
        "code", "dfn", "em", "i", "img", "input", "kbd", "label", "map", "mark",
        "meter", "noscript", "object", "output", "progress", "q", "ruby", "s", "samp",
        "script", "select", "small", "span", "strong", "sub", "sup", "textarea",
        "time", "tt", "u", "var", "wbr"
    };
    
    for (const auto& elem : inlineElems) {
        inlineElements.insert(elem);
    }
}

bool CHTLHtmlTagMap::isValidHtmlTag(const std::string& tagName) const {
    return htmlTags.find(tagName) != htmlTags.end();
}

bool CHTLHtmlTagMap::isSelfClosingTag(const std::string& tagName) const {
    return selfClosingTags.find(tagName) != selfClosingTags.end();
}

bool CHTLHtmlTagMap::isBlockElement(const std::string& tagName) const {
    return blockElements.find(tagName) != blockElements.end();
}

bool CHTLHtmlTagMap::isInlineElement(const std::string& tagName) const {
    return inlineElements.find(tagName) != inlineElements.end();
}

void CHTLHtmlTagMap::addCustomTag(const std::string& tagName, bool isSelfClosing, bool isBlock) {
    htmlTags.insert(tagName);
    if (isSelfClosing) {
        selfClosingTags.insert(tagName);
    }
    if (isBlock) {
        blockElements.insert(tagName);
    } else {
        inlineElements.insert(tagName);
    }
}

// CHTLTypeMap 实现
CHTLTypeMap::CHTLTypeMap() {
}

std::string CHTLTypeMap::makeKey(const std::string& name, const std::string& namespace_) const {
    return namespace_.empty() ? name : (namespace_ + "::" + name);
}

void CHTLTypeMap::registerTemplate(const std::string& name, CHTLTokenType markerType, const std::string& namespace_) {
    TypeInfo info;
    info.name = name;
    info.namespace_ = namespace_;
    info.isTemplate = true;
    info.isCustom = false;
    info.markerType = markerType;
    
    typeMap[makeKey(name, namespace_)] = info;
}

void CHTLTypeMap::registerCustom(const std::string& name, CHTLTokenType markerType, const std::string& namespace_) {
    TypeInfo info;
    info.name = name;
    info.namespace_ = namespace_;
    info.isTemplate = false;
    info.isCustom = true;
    info.markerType = markerType;
    
    typeMap[makeKey(name, namespace_)] = info;
}

bool CHTLTypeMap::hasType(const std::string& name, const std::string& namespace_) const {
    return typeMap.find(makeKey(name, namespace_)) != typeMap.end();
}

const CHTLTypeMap::TypeInfo* CHTLTypeMap::getTypeInfo(const std::string& name, const std::string& namespace_) const {
    auto it = typeMap.find(makeKey(name, namespace_));
    return it != typeMap.end() ? &it->second : nullptr;
}

void CHTLTypeMap::removeType(const std::string& name, const std::string& namespace_) {
    typeMap.erase(makeKey(name, namespace_));
}

std::vector<CHTLTypeMap::TypeInfo> CHTLTypeMap::getTypesInNamespace(const std::string& namespace_) const {
    std::vector<TypeInfo> result;
    for (const auto& pair : typeMap) {
        if (pair.second.namespace_ == namespace_) {
            result.push_back(pair.second);
        }
    }
    return result;
}

// CHTLConfigMap 实现
CHTLConfigMap::CHTLConfigMap() {
    initializeDefaultConfigs();
}

void CHTLConfigMap::initializeDefaultConfigs() {
    // 默认配置
    setConfig("INDEX_INITIAL_COUNT", 0);
    setConfig("DISABLE_NAME_GROUP", true);
    setConfig("DISABLE_CUSTOM_ORIGIN_TYPE", false);
    setConfig("DEBUG_MODE", false);
    setConfig("DISABLE_STYLE_AUTO_ADD_CLASS", false);
    setConfig("DISABLE_STYLE_AUTO_ADD_ID", false);
    setConfig("DISABLE_SCRIPT_AUTO_ADD_CLASS", true);
    setConfig("DISABLE_SCRIPT_AUTO_ADD_ID", true);
    setConfig("DISABLE_DEFAULT_NAMESPACE", false);
    setConfig("OPTION_COUNT", 3);
}

void CHTLConfigMap::setConfig(const std::string& key, const std::string& value) {
    ConfigValue config;
    config.type = ConfigValue::STRING;
    config.stringValue = value;
    configMap[key] = config;
}

void CHTLConfigMap::setConfig(const std::string& key, bool value) {
    ConfigValue config;
    config.type = ConfigValue::BOOL;
    config.boolValue = value;
    configMap[key] = config;
}

void CHTLConfigMap::setConfig(const std::string& key, int value) {
    ConfigValue config;
    config.type = ConfigValue::INT;
    config.intValue = value;
    configMap[key] = config;
}

void CHTLConfigMap::setConfig(const std::string& key, const std::vector<std::string>& value) {
    ConfigValue config;
    config.type = ConfigValue::ARRAY;
    config.arrayValue = value;
    configMap[key] = config;
}

std::string CHTLConfigMap::getStringConfig(const std::string& key, const std::string& defaultValue) const {
    auto it = configMap.find(key);
    if (it != configMap.end() && it->second.type == ConfigValue::STRING) {
        return it->second.stringValue;
    }
    return defaultValue;
}

bool CHTLConfigMap::getBoolConfig(const std::string& key, bool defaultValue) const {
    auto it = configMap.find(key);
    if (it != configMap.end() && it->second.type == ConfigValue::BOOL) {
        return it->second.boolValue;
    }
    return defaultValue;
}

int CHTLConfigMap::getIntConfig(const std::string& key, int defaultValue) const {
    auto it = configMap.find(key);
    if (it != configMap.end() && it->second.type == ConfigValue::INT) {
        return it->second.intValue;
    }
    return defaultValue;
}

std::vector<std::string> CHTLConfigMap::getArrayConfig(const std::string& key) const {
    auto it = configMap.find(key);
    if (it != configMap.end() && it->second.type == ConfigValue::ARRAY) {
        return it->second.arrayValue;
    }
    return std::vector<std::string>();
}

bool CHTLConfigMap::hasConfig(const std::string& key) const {
    return configMap.find(key) != configMap.end();
}

void CHTLConfigMap::removeConfig(const std::string& key) {
    configMap.erase(key);
}

std::vector<std::string> CHTLConfigMap::getAllKeys() const {
    std::vector<std::string> keys;
    keys.reserve(configMap.size());
    for (const auto& pair : configMap) {
        keys.push_back(pair.first);
    }
    return keys;
}

// CHTLGlobalMap 实现
CHTLGlobalMap::CHTLGlobalMap() {
}

void CHTLGlobalMap::reset() {
    keywordMap = CHTLKeywordMap();
    htmlTagMap = CHTLHtmlTagMap();
    typeMap = CHTLTypeMap();
    configMap = CHTLConfigMap();
}

bool CHTLGlobalMap::loadFromConfigFile(const std::string& configFilePath) {
    // TODO: 实现从配置文件加载
    return false;
}

bool CHTLGlobalMap::saveToConfigFile(const std::string& configFilePath) const {
    // TODO: 实现保存到配置文件
    return false;
}

} // namespace CHTL