#pragma once
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <memory>

namespace CHTL {

/**
 * GlobalMap 全局静态映射表
 * 极简设计，仅提供基础的静态信息查询
 * 不负责任何管理逻辑，所有管理功能由相应的管理器负责
 */
class GlobalMap {
public:
    // 删除拷贝构造和赋值操作（单例模式）
    GlobalMap(const GlobalMap&) = delete;
    GlobalMap& operator=(const GlobalMap&) = delete;
    
    // 获取全局实例
    static GlobalMap& getInstance();
    
    // ========== 静态映射查询 ==========
    
    /**
     * 检查是否为CHTL关键字
     */
    bool isKeyword(const std::string& word) const;
    
    /**
     * 检查是否为HTML元素
     */
    bool isHtmlElement(const std::string& element) const;
    
    /**
     * 检查是否为CSS属性
     */
    bool isCssProperty(const std::string& property) const;

private:
    // 私有构造函数（单例模式）
    GlobalMap();
    ~GlobalMap() = default;
    
    // 初始化静态数据
    void initializeStaticData();
    
    // ========== 成员变量 ==========
    
    // 静态映射表（只读）
    std::unordered_set<std::string> m_keywords;
    std::unordered_set<std::string> m_htmlElements;
    std::unordered_set<std::string> m_cssProperties;
    
    // 初始化标志
    bool m_initialized;
};

/**
 * GlobalMap的便捷访问宏
 */
#define GLOBAL_MAP GlobalMap::getInstance()

} // namespace CHTL