#pragma once
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <memory>
#include "Token.h"

namespace CHTLJS {

/**
 * @brief CHTL JS全局标识符映射类
 * 负责管理CHTL JS语言中的关键字、操作符等的映射关系
 */
class GlobalMap {
private:
    // 关键字映射表
    std::unordered_map<std::string, TokenType> keywords_;
    
    // 操作符映射表
    std::unordered_map<std::string, TokenType> operators_;
    
    // 增强语法关键字映射表
    std::unordered_map<std::string, TokenType> enhanced_keywords_;
    
    // DOM选择器函数集合
    std::unordered_set<std::string> dom_functions_;
    
    // CSS属性集合
    std::unordered_set<std::string> css_properties_;

public:
    /**
     * @brief 构造函数
     * 初始化所有映射表
     */
    GlobalMap();

    /**
     * @brief 析构函数
     */
    ~GlobalMap() = default;

    /**
     * @brief 获取关键字对应的Token类型
     * @param keyword 关键字字符串
     * @return Token类型，如果不是关键字返回IDENTIFIER
     */
    TokenType getKeywordType(const std::string& keyword) const;

    /**
     * @brief 获取操作符对应的Token类型
     * @param op 操作符字符串
     * @return Token类型，如果不是操作符返回UNKNOWN
     */
    TokenType getOperatorType(const std::string& op) const;

    /**
     * @brief 获取增强语法关键字对应的Token类型
     * @param keyword 关键字字符串
     * @return Token类型，如果不是增强关键字返回IDENTIFIER
     */
    TokenType getEnhancedKeywordType(const std::string& keyword) const;

    /**
     * @brief 判断是否为DOM函数
     * @param function 函数名称
     * @return 如果是DOM函数返回true，否则false
     */
    bool isDomFunction(const std::string& function) const;

    /**
     * @brief 判断是否为CSS属性
     * @param property 属性名称
     * @return 如果是CSS属性返回true，否则false
     */
    bool isCssProperty(const std::string& property) const;

    /**
     * @brief 添加自定义DOM函数
     * @param function 函数名称
     */
    void addDomFunction(const std::string& function);

    /**
     * @brief 添加自定义CSS属性
     * @param property 属性名称
     */
    void addCssProperty(const std::string& property);

    /**
     * @brief 获取所有关键字
     * @return 关键字列表
     */
    std::vector<std::string> getAllKeywords() const;

    /**
     * @brief 获取所有增强语法关键字
     * @return 增强关键字列表
     */
    std::vector<std::string> getAllEnhancedKeywords() const;

    /**
     * @brief 判断字符串是否为保留字
     * @param str 待检查的字符串
     * @return 如果是保留字返回true，否则false
     */
    bool isReservedWord(const std::string& str) const;

    /**
     * @brief 重置映射表
     */
    void reset();

private:
    /**
     * @brief 初始化基础关键字映射
     */
    void initKeywords();

    /**
     * @brief 初始化操作符映射
     */
    void initOperators();

    /**
     * @brief 初始化增强语法关键字映射
     */
    void initEnhancedKeywords();

    /**
     * @brief 初始化DOM函数集合
     */
    void initDomFunctions();

    /**
     * @brief 初始化CSS属性集合
     */
    void initCssProperties();
};

/**
 * @brief GlobalMap智能指针类型定义
 */
using GlobalMapPtr = std::shared_ptr<GlobalMap>;

} // namespace CHTLJS