#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <optional>
#include <variant>
#include <sstream>

namespace CHTL {

// 配置项类型
enum class ConfigItemType {
    BOOLEAN,
    STRING,
    INTEGER,
    NUMBER,
    STRING_ARRAY,
    ARRAY,
    GROUP,
    UNKNOWN
};

// 配置项值
struct ConfigValue {
    ConfigItemType type;
    std::variant<bool, std::string, int, double, std::vector<std::string>> value;
    
    ConfigValue() : type(ConfigItemType::STRING), value(std::string("")) {}
    ConfigValue(ConfigItemType t) : type(t) {
        switch (t) {
            case ConfigItemType::BOOLEAN: value = false; break;
            case ConfigItemType::STRING: value = std::string(""); break;
            case ConfigItemType::INTEGER: value = 0; break;
            case ConfigItemType::NUMBER: value = 0.0; break;
            case ConfigItemType::STRING_ARRAY: value = std::vector<std::string>(); break;
            case ConfigItemType::ARRAY: value = std::vector<std::string>(); break;
            case ConfigItemType::GROUP: value = std::vector<std::string>(); break;
            case ConfigItemType::UNKNOWN: value = std::string(""); break;
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
    
    std::string toString() const {
        switch (type) {
            case ConfigItemType::BOOLEAN:
                return std::get<bool>(value) ? "true" : "false";
            case ConfigItemType::STRING:
                return std::get<std::string>(value);
            case ConfigItemType::INTEGER:
                return std::to_string(std::get<int>(value));
            case ConfigItemType::NUMBER:
                return std::to_string(std::get<double>(value));
            case ConfigItemType::STRING_ARRAY: {
                const auto& arr = std::get<std::vector<std::string>>(value);
                std::ostringstream oss;
                for (size_t i = 0; i < arr.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << arr[i];
                }
                return oss.str();
            }
            case ConfigItemType::ARRAY:
                return std::get<std::vector<std::string>>(value).empty() ? "[]" : "[...]";
            case ConfigItemType::GROUP:
                return std::get<std::vector<std::string>>(value).empty() ? "{}" : "{...}";
            default:
                return "unknown";
        }
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
    
    // 基本配置管理
    bool setConfig(const std::string& groupName, const std::string& key, const ConfigValue& value);
    ConfigValue getConfig(const std::string& groupName, const std::string& key) const;
    bool hasConfig(const std::string& groupName, const std::string& key) const;
    
    // 配置组管理
    bool createConfigurationGroup(const std::string& name);
    std::shared_ptr<ConfigurationGroup> getConfigurationGroup(const std::string& name) const;
    bool hasConfigurationGroup(const std::string& name) const;
    
    // 原始类型管理
    bool setOriginType(const std::string& groupName, const std::string& typeName, const std::string& typeValue);
    std::vector<std::string> getOriginType(const std::string& groupName, const std::string& typeName) const;
    
    // 配置组激活
    bool activateConfigurationGroup(const std::string& name);
    std::string getActiveConfigurationGroup() const;
    
    // 配置组删除
    bool deleteConfigurationGroup(const std::string& name);
    
    // 配置合并
    bool mergeConfigurationGroups(const std::string& targetName, const std::string& sourceName);
    
    // 默认配置
    std::shared_ptr<ConfigurationGroup> getDefaultConfiguration() const;
    void resetToDefault();
    
    // 配置验证
    bool validateConfigurationGroup(const std::string& groupName) const;
    std::string getConfigurationGroupInfo(const std::string& groupName) const;
    
    // 配置值验证
    bool validateConfigValue(const std::string& key, const ConfigValue& value) const;
    
    // 配置解析
    ConfigValue parseConfigValue(const std::string& value) const;
    
private:
    std::unordered_map<std::string, std::shared_ptr<ConfigurationGroup>> configurationGroups_;
    std::shared_ptr<ConfigurationGroup> defaultConfiguration_;
    std::string activeGroupName_;
    
    // 辅助函数
    void initializeDefaultConfiguration();
    void createStandardConfigs(std::shared_ptr<ConfigurationGroup> group);
};

} // namespace CHTL