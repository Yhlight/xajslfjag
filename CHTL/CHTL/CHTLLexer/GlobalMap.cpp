#include "GlobalMap.h"
#include <iostream>
#include <algorithm>

namespace CHTL {

// 静态成员初始化
std::unique_ptr<GlobalMap> GlobalMap::instance_ = nullptr;

// 私有构造函数
GlobalMap::GlobalMap() {
    initializeKeywords();
    initializeBracketKeywords();
    initializeAtTypes();
    initializeOperators();
}

// 获取单例实例
GlobalMap& GlobalMap::getInstance() {
    if (!instance_) {
        instance_ = std::unique_ptr<GlobalMap>(new GlobalMap());
    }
    return *instance_;
}

// 初始化关键字映射表
void GlobalMap::initializeKeywords() {
    keywords_.clear();
    
    // 基础关键字 - 严格根据CHTL语法文档
    keywords_[nameConfig_.KEYWORD_TEXT] = TokenType::TEXT;
    keywords_[nameConfig_.KEYWORD_STYLE] = TokenType::STYLE;
    keywords_[nameConfig_.KEYWORD_SCRIPT] = TokenType::SCRIPT;
    keywords_["use"] = TokenType::USE;
    keywords_[nameConfig_.KEYWORD_INHERIT] = TokenType::INHERIT;
    keywords_[nameConfig_.KEYWORD_DELETE] = TokenType::DELETE_KW;
    keywords_[nameConfig_.KEYWORD_INSERT] = TokenType::INSERT;
    keywords_[nameConfig_.KEYWORD_AFTER] = TokenType::AFTER;
    keywords_[nameConfig_.KEYWORD_BEFORE] = TokenType::BEFORE;
    keywords_[nameConfig_.KEYWORD_REPLACE] = TokenType::REPLACE;
    keywords_["at"] = TokenType::AT;
    keywords_["top"] = TokenType::TOP;
    keywords_["bottom"] = TokenType::BOTTOM;
    keywords_[nameConfig_.KEYWORD_FROM] = TokenType::FROM;
    keywords_[nameConfig_.KEYWORD_AS] = TokenType::AS;
    keywords_[nameConfig_.KEYWORD_EXCEPT] = TokenType::EXCEPT;
}

// 初始化方括号关键字映射表
void GlobalMap::initializeBracketKeywords() {
    bracketKeywords_.clear();
    
    bracketKeywords_[nameConfig_.KEYWORD_TEMPLATE] = TokenType::TEMPLATE;
    bracketKeywords_[nameConfig_.KEYWORD_CUSTOM] = TokenType::CUSTOM;
    bracketKeywords_[nameConfig_.KEYWORD_ORIGIN] = TokenType::ORIGIN;
    bracketKeywords_[nameConfig_.KEYWORD_IMPORT] = TokenType::IMPORT;
    bracketKeywords_[nameConfig_.KEYWORD_NAMESPACE] = TokenType::NAMESPACE;
    bracketKeywords_["[Configuration]"] = TokenType::CONFIGURATION;
}

// 初始化@前缀类型映射表
void GlobalMap::initializeAtTypes() {
    atTypes_.clear();
    
    // 处理CUSTOM_STYLE的组选项
    if (!config_.DISABLE_NAME_GROUP) {
        int count = std::min(static_cast<int>(nameConfig_.CUSTOM_STYLE.size()), 
                            config_.OPTION_COUNT);
        for (int i = 0; i < count; ++i) {
            atTypes_[nameConfig_.CUSTOM_STYLE[i]] = TokenType::AT_STYLE;
        }
    } else {
        atTypes_[nameConfig_.TEMPLATE_STYLE] = TokenType::AT_STYLE;
    }
    
    atTypes_[nameConfig_.CUSTOM_ELEMENT] = TokenType::AT_ELEMENT;
    atTypes_[nameConfig_.CUSTOM_VAR] = TokenType::AT_VAR;
    atTypes_[nameConfig_.ORIGIN_HTML] = TokenType::AT_HTML;
    atTypes_[nameConfig_.ORIGIN_JAVASCRIPT] = TokenType::AT_JAVASCRIPT;
    atTypes_[nameConfig_.IMPORT_CHTL] = TokenType::AT_CHTL;
    atTypes_[nameConfig_.IMPORT_CJMOD] = TokenType::AT_CJMOD;
    atTypes_[nameConfig_.CONFIGURATION_CONFIG] = TokenType::AT_CONFIG;
}

// 初始化操作符映射表
void GlobalMap::initializeOperators() {
    operators_.clear();
    
    operators_["{"] = TokenType::LEFT_BRACE;
    operators_["}"] = TokenType::RIGHT_BRACE;
    operators_["["] = TokenType::LEFT_BRACKET;
    operators_["]"] = TokenType::RIGHT_BRACKET;
    operators_["("] = TokenType::LEFT_PAREN;
    operators_[")"] = TokenType::RIGHT_PAREN;
    operators_[";"] = TokenType::SEMICOLON;
    operators_[":"] = TokenType::COLON;
    operators_["="] = TokenType::EQUALS;
    operators_[","] = TokenType::COMMA;
    operators_["."] = TokenType::DOT;
    operators_["/"] = TokenType::SLASH;
    operators_["*"] = TokenType::STAR;
    operators_["&"] = TokenType::AMPERSAND;
    operators_["#"] = TokenType::HASH;
}

// 查询是否为关键字
bool GlobalMap::isKeyword(const std::string& word) const {
    return keywords_.find(word) != keywords_.end();
}

// 查询是否为方括号关键字
bool GlobalMap::isBracketKeyword(const std::string& word) const {
    return bracketKeywords_.find(word) != bracketKeywords_.end();
}

// 查询是否为@前缀类型
bool GlobalMap::isAtType(const std::string& word) const {
    return atTypes_.find(word) != atTypes_.end();
}

// 查询是否为操作符
bool GlobalMap::isOperator(const std::string& op) const {
    return operators_.find(op) != operators_.end();
}

// 获取关键字类型
TokenType GlobalMap::getKeywordType(const std::string& word) const {
    auto it = keywords_.find(word);
    if (it != keywords_.end()) {
        return it->second;
    }
    return TokenType::IDENTIFIER;
}

// 获取方括号关键字类型
TokenType GlobalMap::getBracketKeywordType(const std::string& word) const {
    auto it = bracketKeywords_.find(word);
    if (it != bracketKeywords_.end()) {
        return it->second;
    }
    return TokenType::UNKNOWN;
}

// 获取@前缀类型
TokenType GlobalMap::getAtType(const std::string& word) const {
    auto it = atTypes_.find(word);
    if (it != atTypes_.end()) {
        return it->second;
    }
    // 如果不是预定义的@类型，且允许自定义原始嵌入类型
    if (!config_.DISABLE_CUSTOM_ORIGIN_TYPE && word.length() > 1 && word[0] == '@') {
        return TokenType::AT_CUSTOM_TYPE;
    }
    return TokenType::UNKNOWN;
}

// 获取操作符类型
TokenType GlobalMap::getOperatorType(const std::string& op) const {
    auto it = operators_.find(op);
    if (it != operators_.end()) {
        return it->second;
    }
    return TokenType::UNKNOWN;
}

// 更新Name配置
void GlobalMap::updateNameConfiguration(const NameConfiguration& nameConfig) {
    nameConfig_ = nameConfig;
    reinitialize();
}

// 添加自定义原始嵌入类型
void GlobalMap::addCustomOriginType(const std::string& name, const std::string& type) {
    if (!config_.DISABLE_CUSTOM_ORIGIN_TYPE) {
        customOriginTypes_[name] = type;
    }
}

// 检查是否为自定义原始嵌入类型
bool GlobalMap::isCustomOriginType(const std::string& type) const {
    return customOriginTypes_.find(type) != customOriginTypes_.end();
}

// 重新初始化
void GlobalMap::reinitialize() {
    initializeKeywords();
    initializeBracketKeywords();
    initializeAtTypes();
    initializeOperators();
}

// 调试输出
void GlobalMap::printAllMappings() const {
    std::cout << "=== CHTL GlobalMap Debug Output ===" << std::endl;
    
    std::cout << "\n[Keywords]" << std::endl;
    for (const auto& pair : keywords_) {
        std::cout << "  " << pair.first << " -> " << Token::getTypeName(pair.second) << std::endl;
    }
    
    std::cout << "\n[Bracket Keywords]" << std::endl;
    for (const auto& pair : bracketKeywords_) {
        std::cout << "  " << pair.first << " -> " << Token::getTypeName(pair.second) << std::endl;
    }
    
    std::cout << "\n[@ Types]" << std::endl;
    for (const auto& pair : atTypes_) {
        std::cout << "  " << pair.first << " -> " << Token::getTypeName(pair.second) << std::endl;
    }
    
    std::cout << "\n[Operators]" << std::endl;
    for (const auto& pair : operators_) {
        std::cout << "  " << pair.first << " -> " << Token::getTypeName(pair.second) << std::endl;
    }
    
    std::cout << "\n[Custom Origin Types]" << std::endl;
    for (const auto& pair : customOriginTypes_) {
        std::cout << "  " << pair.first << " -> " << pair.second << std::endl;
    }
    
    std::cout << "\n[Configuration]" << std::endl;
    std::cout << "  INDEX_INITIAL_COUNT: " << config_.INDEX_INITIAL_COUNT << std::endl;
    std::cout << "  DISABLE_NAME_GROUP: " << (config_.DISABLE_NAME_GROUP ? "true" : "false") << std::endl;
    std::cout << "  DISABLE_CUSTOM_ORIGIN_TYPE: " << (config_.DISABLE_CUSTOM_ORIGIN_TYPE ? "true" : "false") << std::endl;
    std::cout << "  DEBUG_MODE: " << (config_.DEBUG_MODE ? "true" : "false") << std::endl;
    std::cout << "=================================" << std::endl;
}

} // namespace CHTL