#ifndef CHTL_GLOBALMAP_H
#define CHTL_GLOBALMAP_H

#include <unordered_map>
#include <string>
#include <memory>
#include "Token.h"

namespace CHTL {

// 全局映射表 - 仅负责管理关键字和符号的映射
class GlobalMap {
private:
    // 单例模式
    static std::unique_ptr<GlobalMap> instance_;
    
    // 关键字映射表
    std::unordered_map<std::string, TokenType> keywords_;
    
    // 方括号关键字映射表
    std::unordered_map<std::string, TokenType> bracketKeywords_;
    
    // @前缀类型映射表
    std::unordered_map<std::string, TokenType> atTypes_;
    
    // 操作符映射表
    std::unordered_map<std::string, TokenType> operators_;
    
    // 自定义原始嵌入类型
    std::unordered_map<std::string, std::string> customOriginTypes_;
    
    // 私有构造函数
    GlobalMap();
    
    // 初始化各种映射表
    void initializeKeywords();
    void initializeBracketKeywords();
    void initializeAtTypes();
    void initializeOperators();
    
public:
    // 获取单例实例
    static GlobalMap& getInstance();
    
    // 禁用拷贝和赋值
    GlobalMap(const GlobalMap&) = delete;
    GlobalMap& operator=(const GlobalMap&) = delete;
    
    // 析构函数
    ~GlobalMap() = default;
    
    // 查询方法
    bool isKeyword(const std::string& word) const;
    bool isBracketKeyword(const std::string& word) const;
    bool isAtType(const std::string& word) const;
    bool isOperator(const std::string& op) const;
    
    // 获取Token类型
    TokenType getKeywordType(const std::string& word) const;
    TokenType getBracketKeywordType(const std::string& word) const;
    TokenType getAtType(const std::string& word) const;
    TokenType getOperatorType(const std::string& op) const;
    
    // 自定义原始嵌入类型相关方法
    void addCustomOriginType(const std::string& name, const std::string& type);
    bool isCustomOriginType(const std::string& type) const;
    
    // 重新初始化（配置更新后需要调用）
    void reinitialize();
    
    // 调试输出
    void printAllMappings() const;
};

} // namespace CHTL

#endif // CHTL_GLOBALMAP_H