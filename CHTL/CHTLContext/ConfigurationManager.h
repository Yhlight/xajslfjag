#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <optional>
#include <variant> // Added for std::variant

namespace CHTL {

// 配置项类型
enum class ConfigItemType {
    BOOLEAN,
    STRING,
    NUMBER,
    ARRAY,
    GROUP
};

// 配置项值
struct ConfigValue {
    ConfigItemType type;
    std::variant<bool, std::string, double, std::vector<std::string>> value;
    
    ConfigValue() : type(ConfigItemType::STRING), value(std::string("")) {}
    ConfigValue(ConfigItemType t) : type(t) {
        switch (t) {
            case ConfigItemType::BOOLEAN: value = false; break;
            case ConfigItemType::STRING: value = std::string(""); break;
            case ConfigItemType::NUMBER: value = 0.0; break;
            case ConfigItemType::ARRAY: value = std::vector<std::string>(); break;
            case ConfigItemType::GROUP: value = std::vector<std::string>(); break;
        }
    }
    
    template<typename T>
    ConfigValue(ConfigItemType t, const T& v) : type(t), value(v) {}
    
    template<typename T>
    T get() const {
        return std::get<T>(value);
    }
    
    template<typename T>
    void set(const T& v) {
        value = v;
    }
};

// 配置组
struct ConfigurationGroup {
    std::string name;
    std::unordered_map<std::string, ConfigValue> configItems;
    std::unordered_map<std::string, std::vector<std::string>> nameGroups;
    std::unordered_map<std::string, std::string> originTypes;
    std::string sourceFile;
    size_t line;
    bool isDefault;
    
    ConfigurationGroup(const std::string& name = "", const std::string& sourceFile = "", 
                      size_t line = 0, bool isDefault = false)
        : name(name), sourceFile(sourceFile), line(line), isDefault(isDefault) {}
};

// 配置管理器
class ConfigurationManager {
public:
    ConfigurationManager();
    ~ConfigurationManager() = default;
    
    // 配置组管理
    bool addConfigurationGroup(const std::string& name, const ConfigurationGroup& config);
    std::shared_ptr<ConfigurationGroup> getConfigurationGroup(const std::string& name);
    bool removeConfigurationGroup(const std::string& name);
    
    // 配置项管理
    bool setConfigItem(const std::string& groupName, const std::string& key, const ConfigValue& value);
    std::optional<ConfigValue> getConfigItem(const std::string& groupName, const std::string& key);
    
    // 名称组管理
    bool setNameGroup(const std::string& groupName, const std::string& key, 
                     const std::vector<std::string>& values);
    std::optional<std::vector<std::string>> getNameGroup(const std::string& groupName, const std::string& key);
    
    // 原始类型管理
    bool setOriginType(const std::string& groupName, const std::string& key, const std::string& value);
    std::optional<std::string> getOriginType(const std::string& groupName, const std::string& key);
    
    // 默认配置组
    void setDefaultConfigurationGroup(const std::string& name);
    std::string getDefaultConfigurationGroup() const { return defaultGroup_; }
    
    // 配置组查询
    std::vector<std::string> getConfigurationGroupNames() const;
    bool hasConfigurationGroup(const std::string& name) const;
    size_t getConfigurationGroupCount() const { return configurationGroups_.size(); }
    
    // 配置验证
    bool validateConfiguration(const std::string& groupName);
    std::vector<std::string> getValidationErrors(const std::string& groupName);
    
    // 配置合并
    bool mergeConfigurationGroups(const std::string& target, const std::string& source);
    
    // 配置继承
    bool inheritConfiguration(const std::string& child, const std::string& parent);
    
    // 配置导出
    std::string exportConfiguration(const std::string& groupName) const;
    bool importConfiguration(const std::string& configText, const std::string& groupName = "");
    
    // 配置重置
    void resetConfiguration(const std::string& groupName);
    void resetAllConfigurations();
    
    // 配置备份和恢复
    bool backupConfiguration(const std::string& groupName);
    bool restoreConfiguration(const std::string& groupName);
    
    // 调试信息
    std::string getDebugInfo() const;
    std::string getConfigurationInfo(const std::string& groupName) const;
    
private:
    std::unordered_map<std::string, ConfigurationGroup> configurationGroups_;
    std::string defaultGroup_;
    std::unordered_map<std::string, ConfigurationGroup> backupGroups_;
    
    // 辅助函数
    bool isValidConfigKey(const std::string& key) const;
    bool isValidConfigValue(const ConfigValue& value) const;
    std::string generateConfigKey(const std::string& prefix, const std::string& suffix) const;
    
    // 配置解析
    bool parseConfigText(const std::string& configText, ConfigurationGroup& config);
    std::vector<std::string> parseNameGroup(const std::string& text);
    
    // 配置验证
    bool validateConfigItem(const std::string& key, const ConfigValue& value);
    bool validateNameGroup(const std::string& key, const std::vector<std::string>& values);
    bool validateOriginType(const std::string& key, const std::string& value);
    
    // 配置合并
    void mergeConfigItems(std::unordered_map<std::string, ConfigValue>& target, 
                         const std::unordered_map<std::string, ConfigValue>& source);
    void mergeNameGroups(std::unordered_map<std::string, std::vector<std::string>>& target,
                        const std::unordered_map<std::string, std::vector<std::string>>& source);
    void mergeOriginTypes(std::unordered_map<std::string, std::string>& target,
                         const std::unordered_map<std::string, std::string>& source);
};

} // namespace CHTL