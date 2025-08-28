/**
 * CHTL全局静态映射表实现
 * 
 * 实现GlobalMap类的所有方法
 * 
 * @author CHTL Team
 * @version 1.0.0
 * @license MIT
 */

#include "GlobalMap.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <regex>

namespace CHTL {

// 单例实例获取
GlobalMap& GlobalMap::getInstance() {
    static GlobalMap instance;
    return instance;
}

// 初始化全局映射表
void GlobalMap::initialize() {
    initializeKeywordMap();
    initializeOperatorMap();
    initializeDelimiterMap();
    initializeSpecialSymbolMap();
    initializeSelectorMap();
    initializeConfigurationMap();
    initializePathMap();
    initializeWildcardMap();
    
    // 设置配置组信息
    m_configurationGroups = {"Name", "OriginType"};
    m_modulePrefix = "chtl::";
    m_pathSeparators = {"/", "."};
    
    // 设置标识符验证模式
    m_identifierPattern = "^[a-zA-Z_][a-zA-Z0-9_]*$";
}

// 初始化关键字映射表
void GlobalMap::initializeKeywordMap() {
    // 模板系统关键字
    m_keywordMap["template"] = TokenType::TEMPLATE;
    m_keywordMap["@style"] = TokenType::TEMPLATE_STYLE;
    m_keywordMap["@element"] = TokenType::TEMPLATE_ELEMENT;
    m_keywordMap["@var"] = TokenType::TEMPLATE_VAR;
    
    // 自定义系统关键字
    m_keywordMap["custom"] = TokenType::CUSTOM;
    m_keywordMap["@style"] = TokenType::CUSTOM_STYLE;  // 在[Custom]中
    m_keywordMap["@element"] = TokenType::CUSTOM_ELEMENT;  // 在[Custom]中
    m_keywordMap["@var"] = TokenType::CUSTOM_VAR;  // 在[Custom]中
    
    // 原始嵌入关键字
    m_keywordMap["origin"] = TokenType::ORIGIN;
    m_keywordMap["@html"] = TokenType::ORIGIN_HTML;
    m_keywordMap["@style"] = TokenType::ORIGIN_STYLE;  // 在[Origin]中
    m_keywordMap["@javascript"] = TokenType::ORIGIN_JAVASCRIPT;
    
    // 导入系统关键字
    m_keywordMap["import"] = TokenType::IMPORT;
    m_keywordMap["@html"] = TokenType::IMPORT_HTML;  // 在[Import]中
    m_keywordMap["@style"] = TokenType::IMPORT_STYLE;  // 在[Import]中
    m_keywordMap["@javascript"] = TokenType::IMPORT_JAVASCRIPT;  // 在[Import]中
    m_keywordMap["@chtl"] = TokenType::IMPORT_CHTL;
    m_keywordMap["@cjmod"] = TokenType::IMPORT_CJMOD;
    m_keywordMap["@config"] = TokenType::IMPORT_CONFIG;
    
    // 配置系统关键字
    m_keywordMap["configuration"] = TokenType::CONFIGURATION;
    m_keywordMap["@config"] = TokenType::CONFIGURATION_CONFIG;  // 在[Configuration]中
    
    // 命名空间关键字
    m_keywordMap["namespace"] = TokenType::NAMESPACE;
    
    // 基础关键字
    m_keywordMap["text"] = TokenType::TEXT;
    m_keywordMap["style"] = TokenType::STYLE;
    m_keywordMap["script"] = TokenType::SCRIPT;
    
    // 操作关键字
    m_keywordMap["inherit"] = TokenType::INHERIT;
    m_keywordMap["delete"] = TokenType::DELETE;
    m_keywordMap["insert"] = TokenType::INSERT;
    m_keywordMap["after"] = TokenType::AFTER;
    m_keywordMap["before"] = TokenType::BEFORE;
    m_keywordMap["replace"] = TokenType::REPLACE;
    m_keywordMap["at top"] = TokenType::AT_TOP;
    m_keywordMap["at bottom"] = TokenType::AT_BOTTOM;
    m_keywordMap["from"] = TokenType::FROM;
    m_keywordMap["as"] = TokenType::AS;
    m_keywordMap["except"] = TokenType::EXCEPT;
    
    // 配置组内部关键字
    m_keywordMap["name"] = TokenType::NAME;
    m_keywordMap["origintype"] = TokenType::ORIGIN_TYPE;
    
    // 使用配置关键字
    m_keywordMap["use"] = TokenType::USE;
    m_keywordMap["html5"] = TokenType::HTML5;
}

// 初始化操作符映射表
void GlobalMap::initializeOperatorMap() {
    m_operatorMap[":"] = TokenType::COLON;
    m_operatorMap["="] = TokenType::EQUALS;
    m_operatorMap["."] = TokenType::DOT;
    m_operatorMap["/"] = TokenType::SLASH;
    m_operatorMap["\\"] = TokenType::BACKSLASH;
    m_operatorMap[","] = TokenType::COMMA;
    m_operatorMap[";"] = TokenType::SEMICOLON;
}

// 初始化分隔符映射表
void GlobalMap::initializeDelimiterMap() {
    m_delimiterMap["{"] = TokenType::LEFT_BRACE;
    m_delimiterMap["}"] = TokenType::RIGHT_BRACE;
    m_delimiterMap["["] = TokenType::LEFT_BRACKET;
    m_delimiterMap["]"] = TokenType::RIGHT_BRACKET;
    m_delimiterMap["("] = TokenType::LEFT_PAREN;
    m_delimiterMap[")"] = TokenType::RIGHT_PAREN;
}

// 初始化特殊符号映射表
void GlobalMap::initializeSpecialSymbolMap() {
    m_specialSymbolMap["&"] = TokenType::AMPERSAND;
    m_specialSymbolMap["@"] = TokenType::AT_SYMBOL;
    m_specialSymbolMap["#"] = TokenType::HASH_SYMBOL;
    m_specialSymbolMap["."] = TokenType::PERIOD;
}

// 初始化选择器映射表
void GlobalMap::initializeSelectorMap() {
    // 选择器符号已经在特殊符号中定义
    // 这里可以添加更复杂的选择器模式
}

// 初始化配置映射表
void GlobalMap::initializeConfigurationMap() {
    // 配置相关的映射
    m_configMap["index_initial_count"] = TokenType::CONFIG_VALUE;
    m_configMap["disable_name_group"] = TokenType::CONFIG_VALUE;
    m_configMap["disable_custom_origin_type"] = TokenType::CONFIG_VALUE;
    m_configMap["debug_mode"] = TokenType::CONFIG_VALUE;
    m_configMap["option_count"] = TokenType::CONFIG_OPTION_COUNT;
}

// 初始化路径映射表
void GlobalMap::initializePathMap() {
    m_pathMap["/"] = TokenType::PATH_SEPARATOR;
    m_pathMap["."] = TokenType::PATH_SEPARATOR;
}

// 初始化通配符映射表
void GlobalMap::initializeWildcardMap() {
    m_wildcardMap["*"] = TokenType::WILDCARD;
    m_wildcardMap["*.cmod"] = TokenType::WILDCARD_EXTENSION;
    m_wildcardMap["*.chtl"] = TokenType::WILDCARD_EXTENSION;
    m_wildcardMap["*.cjmod"] = TokenType::WILDCARD_EXTENSION;
}

// 关键字映射相关方法
bool GlobalMap::isKeyword(const std::string& word) const {
    std::string lowerWord = word;
    std::transform(lowerWord.begin(), lowerWord.end(), lowerWord.begin(), ::tolower);
    return m_keywordMap.find(lowerWord) != m_keywordMap.end();
}

TokenType GlobalMap::getKeywordType(const std::string& word) const {
    std::string lowerWord = word;
    std::transform(lowerWord.begin(), lowerWord.end(), lowerWord.begin(), ::tolower);
    auto it = m_keywordMap.find(lowerWord);
    return (it != m_keywordMap.end()) ? it->second : TokenType::UNKNOWN;
}

std::vector<std::string> GlobalMap::getAllKeywords() const {
    std::vector<std::string> keywords;
    for (const auto& pair : m_keywordMap) {
        keywords.push_back(pair.first);
    }
    return keywords;
}

// 操作符映射相关方法
bool GlobalMap::isOperator(const std::string& op) const {
    return m_operatorMap.find(op) != m_operatorMap.end();
}

TokenType GlobalMap::getOperatorType(const std::string& op) const {
    auto it = m_operatorMap.find(op);
    return (it != m_operatorMap.end()) ? it->second : TokenType::UNKNOWN;
}

std::vector<std::string> GlobalMap::getAllOperators() const {
    std::vector<std::string> operators;
    for (const auto& pair : m_operatorMap) {
        operators.push_back(pair.first);
    }
    return operators;
}

// 分隔符映射相关方法
bool GlobalMap::isDelimiter(const std::string& delimiter) const {
    return m_delimiterMap.find(delimiter) != m_delimiterMap.end();
}

TokenType GlobalMap::getDelimiterType(const std::string& delimiter) const {
    auto it = m_delimiterMap.find(delimiter);
    return (it != m_delimiterMap.end()) ? it->second : TokenType::UNKNOWN;
}

std::vector<std::string> GlobalMap::getAllDelimiters() const {
    std::vector<std::string> delimiters;
    for (const auto& pair : m_delimiterMap) {
        delimiters.push_back(pair.first);
    }
    return delimiters;
}

// 配置关键字映射方法
bool GlobalMap::isConfigKeyword(const std::string& word) const {
    std::string lowerWord = word;
    std::transform(lowerWord.begin(), lowerWord.end(), lowerWord.begin(), ::tolower);
    return m_configMap.find(lowerWord) != m_configMap.end();
}

TokenType GlobalMap::getConfigKeywordType(const std::string& word) const {
    std::string lowerWord = word;
    std::transform(lowerWord.begin(), lowerWord.end(), lowerWord.begin(), ::tolower);
    auto it = m_configMap.find(lowerWord);
    return (it != m_configMap.end()) ? it->second : TokenType::UNKNOWN;
}

// 模板关键字映射方法
bool GlobalMap::isTemplateKeyword(const std::string& word) const {
    std::string lowerWord = word;
    std::transform(lowerWord.begin(), lowerWord.end(), lowerWord.begin(), ::tolower);
    return (lowerWord == "template" || lowerWord == "@style" || 
            lowerWord == "@element" || lowerWord == "@var");
}

TokenType GlobalMap::getTemplateKeywordType(const std::string& word) const {
    std::string lowerWord = word;
    std::transform(lowerWord.begin(), lowerWord.end(), lowerWord.begin(), ::tolower);
    
    if (lowerWord == "template") return TokenType::TEMPLATE;
    if (lowerWord == "@style") return TokenType::TEMPLATE_STYLE;
    if (lowerWord == "@element") return TokenType::TEMPLATE_ELEMENT;
    if (lowerWord == "@var") return TokenType::TEMPLATE_VAR;
    
    return TokenType::UNKNOWN;
}

// 自定义关键字映射方法
bool GlobalMap::isCustomKeyword(const std::string& word) const {
    std::string lowerWord = word;
    std::transform(lowerWord.begin(), lowerWord.end(), lowerWord.begin(), ::tolower);
    return (lowerWord == "custom" || lowerWord == "@style" || 
            lowerWord == "@element" || lowerWord == "@var");
}

TokenType GlobalMap::getCustomKeywordType(const std::string& word) const {
    std::string lowerWord = word;
    std::transform(lowerWord.begin(), lowerWord.end(), lowerWord.begin(), ::tolower);
    
    if (lowerWord == "custom") return TokenType::CUSTOM;
    if (lowerWord == "@style") return TokenType::CUSTOM_STYLE;
    if (lowerWord == "@element") return TokenType::CUSTOM_ELEMENT;
    if (lowerWord == "@var") return TokenType::CUSTOM_VAR;
    
    return TokenType::UNKNOWN;
}

// 原始嵌入关键字映射方法
bool GlobalMap::isOriginKeyword(const std::string& word) const {
    std::string lowerWord = word;
    std::transform(lowerWord.begin(), lowerWord.end(), lowerWord.begin(), ::tolower);
    return (lowerWord == "origin" || lowerWord == "@html" || 
            lowerWord == "@style" || lowerWord == "@javascript");
}

TokenType GlobalMap::getOriginKeywordType(const std::string& word) const {
    std::string lowerWord = word;
    std::transform(lowerWord.begin(), lowerWord.end(), lowerWord.begin(), ::tolower);
    
    if (lowerWord == "origin") return TokenType::ORIGIN;
    if (lowerWord == "@html") return TokenType::ORIGIN_HTML;
    if (lowerWord == "@style") return TokenType::ORIGIN_STYLE;
    if (lowerWord == "@javascript") return TokenType::ORIGIN_JAVASCRIPT;
    
    return TokenType::UNKNOWN;
}

// 导入关键字映射方法
bool GlobalMap::isImportKeyword(const std::string& word) const {
    std::string lowerWord = word;
    std::transform(lowerWord.begin(), lowerWord.end(), lowerWord.begin(), ::tolower);
    return (lowerWord == "import" || lowerWord == "@html" || 
            lowerWord == "@style" || lowerWord == "@javascript" ||
            lowerWord == "@chtl" || lowerWord == "@cjmod" || lowerWord == "@config");
}

TokenType GlobalMap::getImportKeywordType(const std::string& word) const {
    std::string lowerWord = word;
    std::transform(lowerWord.begin(), lowerWord.end(), lowerWord.begin(), ::tolower);
    
    if (lowerWord == "import") return TokenType::IMPORT;
    if (lowerWord == "@html") return TokenType::IMPORT_HTML;
    if (lowerWord == "@style") return TokenType::IMPORT_STYLE;
    if (lowerWord == "@javascript") return TokenType::IMPORT_JAVASCRIPT;
    if (lowerWord == "@chtl") return TokenType::IMPORT_CHTL;
    if (lowerWord == "@cjmod") return TokenType::IMPORT_CJMOD;
    if (lowerWord == "@config") return TokenType::IMPORT_CONFIG;
    
    return TokenType::UNKNOWN;
}

// 特殊符号映射方法
bool GlobalMap::isSpecialSymbol(const std::string& symbol) const {
    return m_specialSymbolMap.find(symbol) != m_specialSymbolMap.end();
}

TokenType GlobalMap::getSpecialSymbolType(const std::string& symbol) const {
    auto it = m_specialSymbolMap.find(symbol);
    return (it != m_specialSymbolMap.end()) ? it->second : TokenType::UNKNOWN;
}

// 选择器相关映射方法
bool GlobalMap::isSelectorSymbol(const std::string& symbol) const {
    return (symbol == "." || symbol == "#" || symbol == "&");
}

TokenType GlobalMap::getSelectorSymbolType(const std::string& symbol) const {
    if (symbol == ".") return TokenType::PERIOD;
    if (symbol == "#") return TokenType::HASH_SYMBOL;
    if (symbol == "&") return TokenType::AMPERSAND;
    return TokenType::UNKNOWN;
}

// 配置组映射方法
bool GlobalMap::isConfigurationGroup(const std::string& group) const {
    std::string lowerGroup = group;
    std::transform(lowerGroup.begin(), lowerGroup.end(), lowerGroup.begin(), ::tolower);
    return std::find(m_configurationGroups.begin(), m_configurationGroups.end(), lowerGroup) != m_configurationGroups.end();
}

std::vector<std::string> GlobalMap::getConfigurationGroups() const {
    return m_configurationGroups;
}

// 模块前缀映射方法
bool GlobalMap::isModulePrefix(const std::string& prefix) const {
    return prefix == m_modulePrefix;
}

std::string GlobalMap::getModulePrefix() const {
    return m_modulePrefix;
}

// 路径分隔符映射方法
bool GlobalMap::isPathSeparator(const std::string& separator) const {
    return std::find(m_pathSeparators.begin(), m_pathSeparators.end(), separator) != m_pathSeparators.end();
}

std::vector<std::string> GlobalMap::getPathSeparators() const {
    return m_pathSeparators;
}

// 通配符映射方法
bool GlobalMap::isWildcard(const std::string& wildcard) const {
    return m_wildcardMap.find(wildcard) != m_wildcardMap.end();
}

TokenType GlobalMap::getWildcardType(const std::string& wildcard) const {
    auto it = m_wildcardMap.find(wildcard);
    return (it != m_wildcardMap.end()) ? it->second : TokenType::UNKNOWN;
}

// 验证标识符
bool GlobalMap::isValidIdentifier(const std::string& identifier) const {
    std::regex pattern(m_identifierPattern);
    return std::regex_match(identifier, pattern);
}

// 获取统计信息
size_t GlobalMap::getKeywordCount() const {
    return m_keywordMap.size();
}

size_t GlobalMap::getOperatorCount() const {
    return m_operatorMap.size();
}

size_t GlobalMap::getDelimiterCount() const {
    return m_delimiterMap.size();
}

// 调试信息
void GlobalMap::printAllMappings() const {
    std::cout << "=== CHTL GlobalMap 统计信息 ===" << std::endl;
    std::cout << "关键字数量: " << getKeywordCount() << std::endl;
    std::cout << "操作符数量: " << getOperatorCount() << std::endl;
    std::cout << "分隔符数量: " << getDelimiterCount() << std::endl;
    std::cout << "模块前缀: " << m_modulePrefix << std::endl;
    std::cout << "路径分隔符: ";
    for (const auto& sep : m_pathSeparators) {
        std::cout << sep << " ";
    }
    std::cout << std::endl;
}

std::string GlobalMap::getMappingInfo() const {
    std::ostringstream oss;
    oss << "CHTL GlobalMap 统计信息:\n";
    oss << "关键字数量: " << getKeywordCount() << "\n";
    oss << "操作符数量: " << getOperatorCount() << "\n";
    oss << "分隔符数量: " << getDelimiterCount() << "\n";
    oss << "模块前缀: " << m_modulePrefix << "\n";
    oss << "路径分隔符: ";
    for (const auto& sep : m_pathSeparators) {
        oss << sep << " ";
    }
    return oss.str();
}

} // namespace CHTL