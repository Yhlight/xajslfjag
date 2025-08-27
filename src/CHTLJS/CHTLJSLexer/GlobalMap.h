#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <vector>
#include "Token.h"

namespace CHTLJS {

/// CHTL JS全局映射表，专门用于CHTL JS编译器的符号管理
class GlobalMap {
public:
    /// 关键字映射表
    std::unordered_map<std::string, TokenType> keywords;
    
    /// 符号映射表
    std::unordered_map<std::string, TokenType> symbols;
    
    /// CHTL JS操作符映射表
    std::unordered_map<std::string, TokenType> operators;
    
    /// JavaScript事件类型集合
    std::unordered_set<std::string> jsEvents;
    
    /// CSS动画缓动函数集合
    std::unordered_set<std::string> easingFunctions;
    
    /// DOM属性集合
    std::unordered_set<std::string> domProperties;
    
    /// DOM方法集合
    std::unordered_set<std::string> domMethods;
    
    /// 虚对象函数映射（函数名 -> 参数列表）
    std::unordered_map<std::string, std::vector<std::string>> virtualObjectFunctions;

public:
    GlobalMap();
    
    /// 初始化默认映射
    void InitializeDefaults();
    
    /// 查找关键字
    TokenType LookupKeyword(const std::string& word) const;
    
    /// 查找符号
    TokenType LookupSymbol(const std::string& symbol) const;
    
    /// 查找操作符
    TokenType LookupOperator(const std::string& op) const;
    
    /// 验证JavaScript事件
    bool IsValidJSEvent(const std::string& event) const;
    
    /// 验证CSS缓动函数
    bool IsValidEasingFunction(const std::string& easing) const;
    
    /// 验证DOM属性
    bool IsValidDOMProperty(const std::string& property) const;
    
    /// 验证DOM方法
    bool IsValidDOMMethod(const std::string& method) const;
    
    /// 注册虚对象函数
    void RegisterVirtualObjectFunction(const std::string& funcName, const std::vector<std::string>& params);
    
    /// 获取虚对象函数参数
    std::vector<std::string> GetVirtualObjectFunctionParams(const std::string& funcName) const;
    
    /// 检查虚对象函数是否存在
    bool HasVirtualObjectFunction(const std::string& funcName) const;
    
    /// 添加用户定义的事件类型
    void AddUserEvent(const std::string& event);
    
    /// 清空所有映射
    void Clear();
    
    /// 重置为默认状态
    void Reset();

private:
    /// 初始化关键字映射
    void InitializeKeywords();
    
    /// 初始化符号映射
    void InitializeSymbols();
    
    /// 初始化操作符映射
    void InitializeOperators();
    
    /// 初始化JavaScript事件集合
    void InitializeJSEvents();
    
    /// 初始化CSS缓动函数集合
    void InitializeEasingFunctions();
    
    /// 初始化DOM属性集合
    void InitializeDOMProperties();
    
    /// 初始化DOM方法集合
    void InitializeDOMMethods();
};

} // namespace CHTLJS