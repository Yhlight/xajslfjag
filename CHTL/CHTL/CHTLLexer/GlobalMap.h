#ifndef CHTL_GLOBAL_MAP_H
#define CHTL_GLOBAL_MAP_H

#include <unordered_map>
#include <string>
#include <vector>
#include "Token.h"

namespace CHTL {

// 全局映射类，管理关键字和配置
class GlobalMap {
private:
    // 关键字映射表
    std::unordered_map<std::string, TokenType> keywordMap;
    
    // 可配置的关键字映射（支持Name配置组）
    std::unordered_map<std::string, std::vector<std::string>> configurableKeywords;
    
    // HTML元素集合
    std::unordered_map<std::string, bool> htmlElements;
    
    // 单例模式
    static GlobalMap* instance;
    
    GlobalMap();

public:
    // 获取单例实例
    static GlobalMap* getInstance();
    
    // 销毁单例实例
    static void destroyInstance();
    
    // 检查是否为关键字
    TokenType getKeywordType(const std::string& word) const;
    
    // 检查是否为HTML元素
    bool isHtmlElement(const std::string& element) const;
    
    // 更新可配置关键字（从Configuration读取）
    void updateConfigurableKeyword(const std::string& key, const std::vector<std::string>& values);
    
    // 初始化默认关键字映射
    void initializeDefaultKeywords();
    
    // 初始化HTML元素集合
    void initializeHtmlElements();
    
    // 重置为默认配置
    void reset();
};

} // namespace CHTL

#endif // CHTL_GLOBAL_MAP_H