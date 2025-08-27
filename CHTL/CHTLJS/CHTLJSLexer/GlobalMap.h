#ifndef CHTLJS_GLOBAL_MAP_H
#define CHTLJS_GLOBAL_MAP_H

#include <unordered_map>
#include <string>
#include <unordered_set>
#include "Token.h"

namespace CHTLJS {

// CHTL JS 全局映射类
class GlobalMap {
private:
    // 关键字映射表
    std::unordered_map<std::string, TokenType> keywordMap;
    
    // CHTL JS 特殊函数集合
    std::unordered_set<std::string> chtljsFunctions;
    
    // CSS缓动函数集合
    std::unordered_set<std::string> easingFunctions;
    
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
    
    // 检查是否为CHTL JS函数
    bool isCHTLJSFunction(const std::string& func) const;
    
    // 检查是否为缓动函数
    bool isEasingFunction(const std::string& func) const;
    
    // 初始化关键字映射
    void initializeKeywords();
    
    // 初始化CHTL JS函数
    void initializeCHTLJSFunctions();
    
    // 初始化缓动函数
    void initializeEasingFunctions();
    
    // 重置为默认配置
    void reset();
};

} // namespace CHTLJS

#endif // CHTLJS_GLOBAL_MAP_H