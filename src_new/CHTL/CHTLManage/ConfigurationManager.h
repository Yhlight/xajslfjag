#pragma once
#include "../CHTLNode/ConfigNode.h"
#include <memory>
#include <unordered_map>
#include <string>

namespace CHTL {

/**
 * 配置管理器
 * 负责管理和应用CHTL配置系统
 */
class ConfigurationManager {
public:
    /**
     * 构造函数
     */
    ConfigurationManager();
    
    /**
     * 析构函数
     */
    ~ConfigurationManager() = default;
    
    /**
     * 注册配置节点
     */
    void registerConfiguration(std::shared_ptr<ConfigurationNode> configNode);
    
    /**
     * 设置活动配置（用于use语句）
     */
    void setActiveConfiguration(const std::string& configName);
    
    /**
     * 获取活动配置
     */
    std::shared_ptr<ConfigurationNode> getActiveConfiguration() const;
    
    /**
     * 获取指定名称的配置
     */
    std::shared_ptr<ConfigurationNode> getConfiguration(const std::string& name) const;
    
    /**
     * 获取默认配置（无名配置或第一个有名配置）
     */
    std::shared_ptr<ConfigurationNode> getDefaultConfiguration() const;
    
    /**
     * 应用配置到全局设置
     */
    void applyConfiguration();
    
    // ========== 配置选项访问器 ==========
    
    /**
     * 获取索引起始计数
     */
    int getIndexInitialCount() const;
    
    /**
     * 检查是否禁用Name配置组
     */
    bool isNameGroupDisabled() const;
    
    /**
     * 检查是否禁用自定义原始嵌入类型
     */
    bool isCustomOriginTypeDisabled() const;
    
    /**
     * 检查是否为调试模式
     */
    bool isDebugMode() const;
    
    /**
     * 检查是否禁用默认命名空间
     */
    bool isDefaultNamespaceDisabled() const;
    
    /**
     * 获取选择器自动化配置
     */
    bool isStyleAutoAddClassEnabled() const;
    bool isStyleAutoAddIdEnabled() const;
    bool isScriptAutoAddClassEnabled() const;
    bool isScriptAutoAddIdEnabled() const;
    
    /**
     * 获取关键字别名
     */
    std::vector<std::string> getKeywordAliases(const std::string& keyword) const;
    
    /**
     * 获取自定义原始嵌入类型
     */
    std::unordered_map<std::string, std::string> getCustomOriginTypes() const;
    
    /**
     * 检查是否存在自定义原始嵌入类型
     */
    bool hasCustomOriginType(const std::string& typeName) const;
    
    /**
     * 获取自定义原始嵌入类型标识符
     */
    std::string getCustomOriginTypeIdentifier(const std::string& typeName) const;

private:
    std::unordered_map<std::string, std::shared_ptr<ConfigurationNode>> m_namedConfigurations;  // 命名配置
    std::shared_ptr<ConfigurationNode> m_unnamedConfiguration;                                  // 无名配置
    std::shared_ptr<ConfigurationNode> m_activeConfiguration;                                   // 当前活动配置
    
    /**
     * 选择活动配置
     */
    void selectActiveConfiguration();
    
    /**
     * 合并配置（如果有多个配置的话）
     */
    void mergeConfigurations();
};

} // namespace CHTL