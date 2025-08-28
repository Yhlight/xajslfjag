#ifndef CONFIGURATIONMANAGER_H
#define CONFIGURATIONMANAGER_H

#include <string>
#include <map>
#include <vector>
#include <memory>
#include <unordered_set>

namespace CHTL {

/**
 * @brief 配置项类型枚举
 */
enum class ConfigValueType {
    BOOLEAN,        // 布尔值
    INTEGER,        // 整数值
    STRING,         // 字符串值
    STRING_ARRAY,   // 字符串数组
    KEYWORD         // 关键字
};

/**
 * @brief 配置项值结构
 */
struct ConfigValue {
    ConfigValueType type;
    std::string stringValue;
    int intValue;
    bool boolValue;
    std::vector<std::string> stringArrayValue;
    
    ConfigValue() : type(ConfigValueType::STRING), intValue(0), boolValue(false) {}
    
    // 构造函数重载
    ConfigValue(const std::string& value) : type(ConfigValueType::STRING), stringValue(value), intValue(0), boolValue(false) {}
    ConfigValue(int value) : type(ConfigValueType::INTEGER), intValue(value), boolValue(false) {}
    ConfigValue(bool value) : type(ConfigValueType::BOOLEAN), intValue(0), boolValue(value) {}
    ConfigValue(const std::vector<std::string>& value) : type(ConfigValueType::STRING_ARRAY), stringArrayValue(value), intValue(0), boolValue(false) {}
};

/**
 * @brief 配置组结构
 */
struct ConfigurationGroup {
    std::string name;                           // 配置组名称（空字符串表示无名配置组）
    std::map<std::string, ConfigValue> configs; // 配置项映射
    std::map<std::string, std::vector<std::string>> originTypes; // 原始嵌入类型
    bool isActive;                              // 是否激活
    
    ConfigurationGroup() : isActive(false) {}
    ConfigurationGroup(const std::string& n) : name(n), isActive(false) {}
};

/**
 * @brief 配置管理器
 * 管理CHTL项目的所有配置组和配置项
 */
class ConfigurationManager {
public:
    /**
     * @brief 构造函数
     */
    ConfigurationManager();
    
    /**
     * @brief 析构函数
     */
    ~ConfigurationManager();
    
    /**
     * @brief 创建配置组
     * @param name 配置组名称（空字符串表示无名配置组）
     * @return 是否成功创建
     */
    bool createConfigurationGroup(const std::string& name = "");
    
    /**
     * @brief 获取配置组
     * @param name 配置组名称
     * @return 配置组指针，如果不存在返回nullptr
     */
    std::shared_ptr<ConfigurationGroup> getConfigurationGroup(const std::string& name) const;
    
    /**
     * @brief 获取所有配置组
     * @return 配置组映射
     */
    const std::map<std::string, std::shared_ptr<ConfigurationGroup>>& getAllConfigurationGroups() const;
    
    /**
     * @brief 获取无名配置组
     * @return 无名配置组指针
     */
    std::shared_ptr<ConfigurationGroup> getUnnamedConfigurationGroup();
    
    /**
     * @brief 设置配置项
     * @param groupName 配置组名称
     * @param key 配置键
     * @param value 配置值
     * @return 是否成功设置
     */
    bool setConfig(const std::string& groupName, const std::string& key, const ConfigValue& value);
    
    /**
     * @brief 获取配置项
     * @param groupName 配置组名称
     * @param key 配置键
     * @return 配置值，如果不存在返回默认值
     */
    ConfigValue getConfig(const std::string& groupName, const std::string& key) const;
    
    /**
     * @brief 设置原始嵌入类型
     * @param groupName 配置组名称
     * @param typeName 类型名称
     * @param typeValue 类型值
     * @return 是否成功设置
     */
    bool setOriginType(const std::string& groupName, const std::string& typeName, const std::string& typeValue);
    
    /**
     * @brief 获取原始嵌入类型
     * @param groupName 配置组名称
     * @param typeName 类型名称
     * @return 类型值列表
     */
    std::vector<std::string> getOriginType(const std::string& groupName, const std::string& typeName) const;
    
    /**
     * @brief 激活配置组
     * @param name 配置组名称
     * @return 是否成功激活
     */
    bool activateConfigurationGroup(const std::string& name);
    
    /**
     * @brief 获取当前激活的配置组
     * @return 激活的配置组名称
     */
    std::string getActiveConfigurationGroup() const;
    
    /**
     * @brief 检查配置项是否存在
     * @param groupName 配置组名称
     * @param key 配置键
     * @return 是否存在
     */
    bool hasConfig(const std::string& groupName, const std::string& key) const;
    
    /**
     * @brief 检查配置组是否存在
     * @param name 配置组名称
     * @return 是否存在
     */
    bool hasConfigurationGroup(const std::string& name) const;
    
    /**
     * @brief 删除配置组
     * @param name 配置组名称
     * @return 是否成功删除
     */
    bool deleteConfigurationGroup(const std::string& name);
    
    /**
     * @brief 合并配置组
     * @param targetName 目标配置组名称
     * @param sourceName 源配置组名称
     * @return 是否成功合并
     */
    bool mergeConfigurationGroups(const std::string& targetName, const std::string& sourceName);
    
    /**
     * @brief 获取默认配置
     * @return 默认配置组
     */
    std::shared_ptr<ConfigurationGroup> getDefaultConfiguration();
    
    /**
     * @brief 重置为默认配置
     */
    void resetToDefault();
    
    /**
     * @brief 验证配置组
     * @param groupName 配置组名称
     * @return 是否有效
     */
    bool validateConfigurationGroup(const std::string& groupName) const;
    
    /**
     * @brief 获取配置组信息
     * @param groupName 配置组名称
     * @return 配置组信息字符串
     */
    std::string getConfigurationGroupInfo(const std::string& groupName) const;

private:
    std::map<std::string, std::shared_ptr<ConfigurationGroup>> configurationGroups_; // 配置组映射
    std::string activeGroupName_;                                                    // 当前激活的配置组名称
    std::shared_ptr<ConfigurationGroup> defaultConfiguration_;                       // 默认配置
    
    /**
     * @brief 初始化默认配置
     */
    void initializeDefaultConfiguration();
    
    /**
     * @brief 创建标准配置项
     */
    void createStandardConfigs(std::shared_ptr<ConfigurationGroup> group);
    
    /**
     * @brief 验证配置项值
     * @param key 配置键
     * @param value 配置值
     * @return 是否有效
     */
    bool validateConfigValue(const std::string& key, const ConfigValue& value) const;
    
    /**
     * @brief 解析配置值
     * @param value 原始值字符串
     * @return 解析后的配置值
     */
    ConfigValue parseConfigValue(const std::string& value) const;
};

} // namespace CHTL

#endif // CONFIGURATIONMANAGER_H