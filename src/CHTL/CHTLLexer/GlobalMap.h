#pragma once
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <memory>

namespace CHTL {

/**
 * GlobalMap 全局静态表
 * 轻量级设计，仅提供基础的全局信息存储和查询
 * 不负责复杂的管理逻辑，由相关管理器负责具体功能
 */
class GlobalMap {
public:
    // 删除拷贝构造和赋值操作（单例模式）
    GlobalMap(const GlobalMap&) = delete;
    GlobalMap& operator=(const GlobalMap&) = delete;
    
    // 获取全局实例
    static GlobalMap& getInstance();
    
    // ========== 关键字映射 ==========
    
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
    
    // ========== 配置信息 ==========
    
    /**
     * 获取配置值
     */
    bool getConfigBool(const std::string& key, bool defaultValue = false) const;
    void setConfigBool(const std::string& key, bool value);
    
    std::string getConfigString(const std::string& key, const std::string& defaultValue = "") const;
    void setConfigString(const std::string& key, const std::string& value);
    
    int getConfigInt(const std::string& key, int defaultValue = 0) const;
    void setConfigInt(const std::string& key, int value);
    
    // ========== 路径信息 ==========
    
    /**
     * 设置和获取当前编译文件路径
     */
    void setCurrentFilePath(const std::string& path);
    std::string getCurrentFilePath() const;
    
    /**
     * 设置和获取官方模块目录
     */
    void setOfficialModulePath(const std::string& path);
    std::string getOfficialModulePath() const;
    
    /**
     * 设置和获取当前工作目录
     */
    void setWorkingDirectory(const std::string& path);
    std::string getWorkingDirectory() const;
    
    // ========== 状态信息 ==========
    
    /**
     * 调试模式
     */
    bool isDebugMode() const;
    void setDebugMode(bool enabled);
    
    /**
     * 索引起始计数
     */
    int getIndexInitialCount() const;
    void setIndexInitialCount(int count);
    
    // ========== 静态表信息 ==========
    
    /**
     * 注册自定义原始嵌入类型
     */
    void registerCustomOriginType(const std::string& typeName);
    bool isCustomOriginType(const std::string& typeName) const;
    
    /**
     * 获取所有自定义原始嵌入类型
     */
    const std::unordered_set<std::string>& getCustomOriginTypes() const;
    
    // ========== 清理和重置 ==========
    
    /**
     * 清理临时数据
     */
    void clearTempData();
    
    /**
     * 重置为默认状态
     */
    void reset();

private:
    // 私有构造函数（单例模式）
    GlobalMap();
    ~GlobalMap() = default;
    
    // 初始化静态数据
    void initializeStaticData();
    
    // ========== 成员变量 ==========
    
    // 静态映射表
    std::unordered_set<std::string> m_keywords;
    std::unordered_set<std::string> m_htmlElements;
    std::unordered_set<std::string> m_cssProperties;
    
    // 配置信息
    std::unordered_map<std::string, bool> m_boolConfigs;
    std::unordered_map<std::string, std::string> m_stringConfigs;
    std::unordered_map<std::string, int> m_intConfigs;
    
    // 路径信息
    std::string m_currentFilePath;
    std::string m_officialModulePath;
    std::string m_workingDirectory;
    
    // 状态信息
    bool m_debugMode;
    int m_indexInitialCount;
    
    // 自定义类型
    std::unordered_set<std::string> m_customOriginTypes;
    
    // 初始化标志
    bool m_initialized;
};

/**
 * GlobalMap的便捷访问宏
 */
#define GLOBAL_MAP GlobalMap::getInstance()

} // namespace CHTL