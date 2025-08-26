#ifndef CHTL_CONFIG_MANAGER_H
#define CHTL_CONFIG_MANAGER_H

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

namespace CHTL {

// 配置选项类型
enum class ConfigOptionType {
    BOOLEAN,
    STRING,
    INTEGER,
    STRING_LIST,
    KEY_VALUE_MAP
};

// 配置选项值
struct ConfigOption {
    std::string key;
    ConfigOptionType type;
    std::string stringValue;
    int intValue;
    bool boolValue;
    std::vector<std::string> listValue;
    std::unordered_map<std::string, std::string> mapValue;
    
    ConfigOption() : type(ConfigOptionType::STRING), intValue(0), boolValue(false) {}
};

// 配置信息
struct ConfigurationInfo {
    std::string name;                    // 配置名称
    bool isDefault;                      // 是否为默认配置（无名配置）
    std::string sourcePath;              // 定义所在文件
    size_t line;                         // 定义所在行
    size_t column;                       // 定义所在列
    
    // 配置选项
    std::unordered_map<std::string, ConfigOption> options;
    
    // Name组配置
    std::unordered_map<std::string, std::vector<std::string>> nameGroup;
    
    // OriginType配置
    std::unordered_map<std::string, std::string> originTypes;
    
    ConfigurationInfo() : isDefault(false), line(0), column(0) {}
};

// 配置管理器
class ConfigManager {
private:
    // 配置表：配置名 -> 配置信息
    std::unordered_map<std::string, ConfigurationInfo> configurations;
    
    // 活动配置名
    std::string activeConfigName;
    
    // 默认配置名（如果有多个无名配置会冲突）
    std::string defaultConfigName;
    
    // 预定义的配置选项及其默认值
    std::unordered_map<std::string, ConfigOption> defaultOptions;
    
    // 预定义的Name组选项
    std::unordered_map<std::string, std::vector<std::string>> defaultNameGroup;
    
    // 辅助方法
    void initializeDefaults();
    bool validateOption(const std::string& key, const ConfigOption& option) const;
    ConfigOption parseOptionValue(const std::string& key, const std::string& value) const;
    
public:
    ConfigManager();
    ~ConfigManager() = default;
    
    // 配置管理
    void addConfiguration(const std::string& name, const ConfigurationInfo& config);
    void addConfiguration(const ConfigurationInfo& config);  // 无名配置
    bool configurationExists(const std::string& name) const;
    ConfigurationInfo getConfiguration(const std::string& name) const;
    void removeConfiguration(const std::string& name);
    
    // 活动配置
    void setActiveConfiguration(const std::string& name);
    std::string getActiveConfigurationName() const { return activeConfigName; }
    ConfigurationInfo getActiveConfiguration() const;
    bool hasActiveConfiguration() const;
    
    // 默认配置
    void setDefaultConfiguration(const std::string& name);
    bool hasDefaultConfiguration() const;
    ConfigurationInfo getDefaultConfiguration() const;
    std::vector<std::string> findDefaultConfigurations() const;
    
    // 配置选项访问
    ConfigOption getOption(const std::string& key) const;
    ConfigOption getOption(const std::string& configName, const std::string& key) const;
    std::string getStringOption(const std::string& key, const std::string& defaultValue = "") const;
    int getIntOption(const std::string& key, int defaultValue = 0) const;
    bool getBoolOption(const std::string& key, bool defaultValue = false) const;
    std::vector<std::string> getListOption(const std::string& key) const;
    
    // 配置选项设置
    void setOption(const std::string& key, const std::string& value);
    void setOption(const std::string& key, int value);
    void setOption(const std::string& key, bool value);
    void setOption(const std::string& key, const std::vector<std::string>& value);
    void setOption(const std::string& configName, const std::string& key, const ConfigOption& option);
    
    // Name组访问
    std::vector<std::string> getNameGroupOption(const std::string& key) const;
    std::vector<std::string> getNameGroupOption(const std::string& configName, const std::string& key) const;
    void setNameGroupOption(const std::string& key, const std::vector<std::string>& values);
    void setNameGroupOption(const std::string& configName, const std::string& key, const std::vector<std::string>& values);
    
    // OriginType访问
    std::string getOriginType(const std::string& key) const;
    std::unordered_map<std::string, std::string> getAllOriginTypes() const;
    void setOriginType(const std::string& key, const std::string& value);
    void setOriginType(const std::string& configName, const std::string& key, const std::string& value);
    
    // 特定配置选项的便捷访问
    bool isDebugMode() const;
    bool isNameGroupDisabled() const;
    bool isCustomOriginTypeDisabled() const;
    int getIndexInitialCount() const;
    int getNameGroupOptionCount() const;
    
    // 选择器自动化配置
    bool isStyleAutoAddClassDisabled() const;
    bool isStyleAutoAddIdDisabled() const;
    bool isScriptAutoAddClassDisabled() const;
    bool isScriptAutoAddIdDisabled() const;
    bool isDefaultNamespaceDisabled() const;
    
    // 验证
    bool validateConfiguration(const ConfigurationInfo& config) const;
    std::vector<std::string> getConfigurationErrors(const std::string& name) const;
    bool hasConfigurationConflict() const;
    
    // 合并配置
    void mergeConfiguration(const std::string& targetName, const std::string& sourceName);
    ConfigurationInfo mergeConfigurations(const ConfigurationInfo& base, const ConfigurationInfo& override) const;
    
    // 清理
    void clearConfiguration(const std::string& name);
    void clearAll();
    void resetToDefaults();
    
    // 导入导出
    ConfigurationInfo importConfiguration(const std::string& configData);
    std::string exportConfiguration(const std::string& name) const;
    
    // 调试
    void dumpConfigurations() const;
    void dumpActiveConfiguration() const;
    void dumpDefaults() const;
};

} // namespace CHTL

#endif // CHTL_CONFIG_MANAGER_H