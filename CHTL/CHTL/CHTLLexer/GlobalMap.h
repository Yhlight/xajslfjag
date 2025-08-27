#pragma once
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <memory>
#include "Token.h"

namespace CHTL {

/**
 * @brief 全局标识符映射类
 * 负责管理CHTL语言中的关键字、类型标识符等的映射关系
 */
class GlobalMap {
private:
    // 关键字映射表
    std::unordered_map<std::string, TokenType> keywords_;
    
    // 块类型标识符映射表
    std::unordered_map<std::string, TokenType> block_types_;
    
    // 类型标识符映射表
    std::unordered_map<std::string, TokenType> type_identifiers_;
    
    // 操作符映射表
    std::unordered_map<std::string, TokenType> operators_;
    
    // HTML元素集合
    std::unordered_set<std::string> html_elements_;
    
    // 自定义类型映射 (用于原始嵌入的自定义类型)
    std::unordered_map<std::string, std::string> custom_origin_types_;

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
     * @brief 获取块类型标识符对应的Token类型
     * @param block_type 块类型字符串
     * @return Token类型，如果不是块类型返回UNKNOWN
     */
    TokenType getBlockType(const std::string& block_type) const;

    /**
     * @brief 获取类型标识符对应的Token类型
     * @param type_id 类型标识符字符串
     * @return Token类型，如果不是类型标识符返回UNKNOWN
     */
    TokenType getTypeIdentifier(const std::string& type_id) const;

    /**
     * @brief 获取操作符对应的Token类型
     * @param op 操作符字符串
     * @return Token类型，如果不是操作符返回UNKNOWN
     */
    TokenType getOperatorType(const std::string& op) const;

    /**
     * @brief 判断是否为HTML元素
     * @param element 元素名称
     * @return 如果是HTML元素返回true，否则false
     */
    bool isHtmlElement(const std::string& element) const;

    /**
     * @brief 添加自定义原始嵌入类型
     * @param type_name 类型名称 (如@Vue)
     * @param description 类型描述
     */
    void addCustomOriginType(const std::string& type_name, const std::string& description);

    /**
     * @brief 判断是否为自定义原始嵌入类型
     * @param type_name 类型名称
     * @return 如果是自定义类型返回true，否则false
     */
    bool isCustomOriginType(const std::string& type_name) const;

    /**
     * @brief 获取所有关键字
     * @return 关键字列表
     */
    std::vector<std::string> getAllKeywords() const;

    /**
     * @brief 获取所有HTML元素
     * @return HTML元素列表
     */
    std::vector<std::string> getAllHtmlElements() const;

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
     * @brief 初始化关键字映射
     */
    void initKeywords();

    /**
     * @brief 初始化块类型映射
     */
    void initBlockTypes();

    /**
     * @brief 初始化类型标识符映射
     */
    void initTypeIdentifiers();

    /**
     * @brief 初始化操作符映射
     */
    void initOperators();

    /**
     * @brief 初始化HTML元素集合
     */
    void initHtmlElements();
};

/**
 * @brief GlobalMap智能指针类型定义
 */
using GlobalMapPtr = std::shared_ptr<GlobalMap>;

} // namespace CHTL