#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <vector>
#include "Token.h"

namespace CHTL {

/// CHTL全局映射表，专门用于CHTL编译器的符号管理
class GlobalMap {
public:
    /// 关键字映射表
    std::unordered_map<std::string, TokenType> keywords;
    
    /// 符号映射表
    std::unordered_map<std::string, TokenType> symbols;
    
    /// 类型标识符映射表
    std::unordered_map<std::string, TokenType> typeIdentifiers;
    
    /// 括号类型映射表
    std::unordered_map<std::string, TokenType> bracketTypes;
    
    /// HTML元素集合（用于验证元素合法性）
    std::unordered_set<std::string> htmlElements;
    
    /// CSS属性集合（用于验证CSS属性）
    std::unordered_set<std::string> cssProperties;
    
    /// 配置项映射（用于处理配置块）
    std::unordered_map<std::string, std::string> configurationItems;

public:
    GlobalMap();
    
    /// 初始化默认映射
    void InitializeDefaults();
    
    /// 查找关键字
    TokenType LookupKeyword(const std::string& word) const;
    
    /// 查找符号
    TokenType LookupSymbol(const std::string& symbol) const;
    
    /// 查找类型标识符
    TokenType LookupTypeIdentifier(const std::string& identifier) const;
    
    /// 查找括号类型
    TokenType LookupBracketType(const std::string& bracket) const;
    
    /// 验证HTML元素
    bool IsValidHtmlElement(const std::string& element) const;
    
    /// 验证CSS属性
    bool IsValidCssProperty(const std::string& property) const;
    
    /// 添加用户定义的类型标识符
    void AddUserTypeIdentifier(const std::string& identifier, TokenType type);
    
    /// 添加配置项
    void AddConfigurationItem(const std::string& key, const std::string& value);
    
    /// 获取配置项
    std::string GetConfigurationItem(const std::string& key) const;
    
    /// 清空所有映射
    void Clear();
    
    /// 重置为默认状态
    void Reset();

private:
    /// 初始化关键字映射
    void InitializeKeywords();
    
    /// 初始化符号映射
    void InitializeSymbols();
    
    /// 初始化类型标识符映射
    void InitializeTypeIdentifiers();
    
    /// 初始化括号类型映射
    void InitializeBracketTypes();
    
    /// 初始化HTML元素集合
    void InitializeHtmlElements();
    
    /// 初始化CSS属性集合
    void InitializeCssProperties();
};

} // namespace CHTL