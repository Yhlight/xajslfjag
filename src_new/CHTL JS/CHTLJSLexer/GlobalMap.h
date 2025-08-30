#pragma once
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <memory>

namespace CHTLJS {

/**
 * CHTL JS GlobalMap 全局静态映射表
 * 专门用于CHTL JS语法的关键字和语法元素识别
 */
class GlobalMap {
public:
    // 删除拷贝构造和赋值操作（单例模式）
    GlobalMap(const GlobalMap&) = delete;
    GlobalMap& operator=(const GlobalMap&) = delete;
    
    // 获取全局实例
    static GlobalMap& getInstance();
    
    // ========== CHTL JS专用映射查询 ==========
    
    /**
     * 检查是否为CHTL JS关键字
     */
    bool isCHTLJSKeyword(const std::string& word) const;
    
    /**
     * 检查是否为JavaScript事件名
     */
    bool isJSEvent(const std::string& event) const;
    
    /**
     * 检查是否为增强选择器
     */
    bool isEnhancedSelector(const std::string& selector) const;
    
    /**
     * 检查是否为CHTL JS函数语法
     */
    bool isCHTLJSFunction(const std::string& syntax) const;

private:
    // 私有构造函数（单例模式）
    GlobalMap();
    ~GlobalMap() = default;
    
    // 初始化CHTL JS专用数据
    void initializeCHTLJSData();
    
    // ========== 成员变量 ==========
    
    // CHTL JS专用映射表
    std::unordered_set<std::string> m_chtlJSKeywords;
    std::unordered_set<std::string> m_jsEvents;
    std::unordered_set<std::string> m_enhancedSelectorPatterns;
    
    // 初始化标志
    bool m_initialized;
};

/**
 * CHTL JS GlobalMap的便捷访问宏
 */
#define CHTLJS_GLOBAL_MAP GlobalMap::getInstance()

} // namespace CHTLJS