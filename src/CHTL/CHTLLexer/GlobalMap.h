/**
 * CHTL全局静态映射表
 * 
 * 管理CHTL语法中的各种全局映射关系
 * 包括关键字映射、操作符映射、配置映射等
 * 
 * @author CHTL Team
 * @version 1.0.0
 * @license MIT
 */

#ifndef CHTL_GLOBAL_MAP_H
#define CHTL_GLOBAL_MAP_H

#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include "Token.h"

namespace CHTL {

/**
 * 全局静态映射表类
 * 管理CHTL语法中的所有全局映射关系
 */
class GlobalMap {
public:
    // 单例模式
    static GlobalMap& getInstance();
    
    // 禁用拷贝和赋值
    GlobalMap(const GlobalMap&) = delete;
    GlobalMap& operator=(const GlobalMap&) = delete;
    
    // 初始化全局映射表
    void initialize();
    
    // 关键字映射相关
    bool isKeyword(const std::string& word) const;
    TokenType getKeywordType(const std::string& word) const;
    std::vector<std::string> getAllKeywords() const;
    
    // 操作符映射相关
    bool isOperator(const std::string& op) const;
    TokenType getOperatorType(const std::string& op) const;
    std::vector<std::string> getAllOperators() const;
    
    // 分隔符映射相关
    bool isDelimiter(const std::string& delimiter) const;
    TokenType getDelimiterType(const std::string& delimiter) const;
    std::vector<std::string> getAllDelimiters() const;
    
    // 配置关键字映射
    bool isConfigKeyword(const std::string& word) const;
    TokenType getConfigKeywordType(const std::string& word) const;
    
    // 模板关键字映射
    bool isTemplateKeyword(const std::string& word) const;
    TokenType getTemplateKeywordType(const std::string& word) const;
    
    // 自定义关键字映射
    bool isCustomKeyword(const std::string& word) const;
    TokenType getCustomKeywordType(const std::string& word) const;
    
    // 原始嵌入关键字映射
    bool isOriginKeyword(const std::string& word) const;
    TokenType getOriginKeywordType(const std::string& word) const;
    
    // 导入关键字映射
    bool isImportKeyword(const std::string& word) const;
    TokenType getImportKeywordType(const std::string& word) const;
    
    // 特殊符号映射
    bool isSpecialSymbol(const std::string& symbol) const;
    TokenType getSpecialSymbolType(const std::string& symbol) const;
    
    // 选择器相关映射
    bool isSelectorSymbol(const std::string& symbol) const;
    TokenType getSelectorSymbolType(const std::string& symbol) const;
    
    // 配置组映射
    bool isConfigurationGroup(const std::string& group) const;
    std::vector<std::string> getConfigurationGroups() const;
    
    // 模块前缀映射
    bool isModulePrefix(const std::string& prefix) const;
    std::string getModulePrefix() const;
    
    // 路径分隔符映射
    bool isPathSeparator(const std::string& separator) const;
    std::vector<std::string> getPathSeparators() const;
    
    // 通配符映射
    bool isWildcard(const std::string& wildcard) const;
    TokenType getWildcardType(const std::string& wildcard) const;
    
    // 验证标识符
    bool isValidIdentifier(const std::string& identifier) const;
    
    // 获取所有映射的统计信息
    size_t getKeywordCount() const;
    size_t getOperatorCount() const;
    size_t getDelimiterCount() const;
    
    // 调试信息
    void printAllMappings() const;
    std::string getMappingInfo() const;

private:
    GlobalMap() = default;
    ~GlobalMap() = default;
    
    // 初始化各种映射表
    void initializeKeywordMap();
    void initializeOperatorMap();
    void initializeDelimiterMap();
    void initializeSpecialSymbolMap();
    void initializeSelectorMap();
    void initializeConfigurationMap();
    void initializePathMap();
    void initializeWildcardMap();
    
    // 映射表存储
    std::unordered_map<std::string, TokenType> m_keywordMap;           // 关键字映射
    std::unordered_map<std::string, TokenType> m_operatorMap;          // 操作符映射
    std::unordered_map<std::string, TokenType> m_delimiterMap;         // 分隔符映射
    std::unordered_map<std::string, TokenType> m_specialSymbolMap;     // 特殊符号映射
    std::unordered_map<std::string, TokenType> m_selectorMap;          // 选择器映射
    std::unordered_map<std::string, TokenType> m_configMap;            // 配置映射
    std::unordered_map<std::string, TokenType> m_pathMap;              // 路径映射
    std::unordered_map<std::string, TokenType> m_wildcardMap;          // 通配符映射
    
    // 配置组信息
    std::vector<std::string> m_configurationGroups;                    // 配置组列表
    std::string m_modulePrefix;                                        // 模块前缀
    std::vector<std::string> m_pathSeparators;                         // 路径分隔符列表
    
    // 标识符验证规则
    std::string m_identifierPattern;                                   // 标识符模式
};

} // namespace CHTL

#endif // CHTL_GLOBAL_MAP_H