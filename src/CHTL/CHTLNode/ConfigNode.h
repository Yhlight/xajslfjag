#pragma once
#include "BaseNode.h"
#include <string>
#include <unordered_map>
#include <vector>

namespace CHTL {

/**
 * 配置类型枚举
 */
enum class ConfigType {
    CONFIGURATION,      // [Configuration]
    NAME,              // [Name]
    ORIGIN_TYPE,       // [OriginType]
    INFO,              // [Info]
    EXPORT             // [Export]
};

/**
 * 配置值类型
 */
enum class ConfigValueType {
    BOOLEAN,           // 布尔值
    INTEGER,           // 整数
    STRING,            // 字符串
    ARRAY,             // 数组
    IDENTIFIER         // 标识符
};

/**
 * 配置值结构
 */
struct ConfigValue {
    ConfigValueType type;
    std::string value;
    std::vector<std::string> arrayValues;  // 用于数组类型
    
    ConfigValue() : type(ConfigValueType::STRING) {}
    ConfigValue(const std::string& val) : type(ConfigValueType::STRING), value(val) {}
    ConfigValue(bool val) : type(ConfigValueType::BOOLEAN), value(val ? "true" : "false") {}
    ConfigValue(int val) : type(ConfigValueType::INTEGER), value(std::to_string(val)) {}
    ConfigValue(const std::vector<std::string>& vals) : type(ConfigValueType::ARRAY), arrayValues(vals) {}
    
    // 获取布尔值
    bool getBool() const;
    
    // 获取整数值
    int getInt() const;
    
    // 获取字符串值
    const std::string& getString() const;
    
    // 获取数组值
    const std::vector<std::string>& getArray() const;
    
    // 检查是否为数组
    bool isArray() const { return type == ConfigValueType::ARRAY; }
};

/**
 * 配置节点基类
 */
class ConfigNode : public BaseNode {
public:
    /**
     * 构造函数
     */
    ConfigNode(ConfigType configType, const std::string& name = "");
    
    /**
     * 析构函数
     */
    virtual ~ConfigNode() = default;
    
    /**
     * 获取配置类型
     */
    ConfigType getConfigType() const;
    
    /**
     * 获取配置名称
     */
    const std::string& getConfigName() const;
    
    /**
     * 设置配置名称
     */
    void setConfigName(const std::string& name);
    
    /**
     * 设置配置值
     */
    void setConfigValue(const std::string& key, const ConfigValue& value);
    
    /**
     * 获取配置值
     */
    const ConfigValue* getConfigValue(const std::string& key) const;
    
    /**
     * 检查是否存在配置项
     */
    bool hasConfig(const std::string& key) const;
    
    /**
     * 获取所有配置
     */
    const std::unordered_map<std::string, ConfigValue>& getAllConfigs() const;
    
    /**
     * 检查是否为命名配置
     */
    bool isNamed() const;
    
    // BaseNode 接口实现
    CHTLNodeType getNodeType() const { return CHTLNodeType::CONFIG_NODE; }
    std::string toString() const override;
    NodePtr clone() const override;
    bool validate(ErrorReporter* errorReporter = nullptr) const override;

protected:
    ConfigType m_configType;                                        // 配置类型
    std::string m_configName;                                       // 配置名称（可选）
    std::unordered_map<std::string, ConfigValue> m_configurations;  // 配置项
};

/**
 * 主配置节点
 * [Configuration] 或 [Configuration] @Config Name
 */
class ConfigurationNode : public ConfigNode {
public:
    /**
     * 构造函数
     */
    ConfigurationNode(const std::string& name = "");
    
    /**
     * 设置索引起始计数
     */
    void setIndexInitialCount(int count);
    
    /**
     * 获取索引起始计数
     */
    int getIndexInitialCount() const;
    
    /**
     * 设置是否禁用Name配置组
     */
    void setDisableNameGroup(bool disable);
    
    /**
     * 检查是否禁用Name配置组
     */
    bool isNameGroupDisabled() const;
    
    /**
     * 设置是否禁用自定义原始嵌入类型
     */
    void setDisableCustomOriginType(bool disable);
    
    /**
     * 检查是否禁用自定义原始嵌入类型
     */
    bool isCustomOriginTypeDisabled() const;
    
    /**
     * 设置调试模式
     */
    void setDebugMode(bool debug);
    
    /**
     * 检查是否为调试模式
     */
    bool isDebugMode() const;
    
    /**
     * 设置是否禁用默认命名空间
     */
    void setDisableDefaultNamespace(bool disable);
    
    /**
     * 检查是否禁用默认命名空间
     */
    bool isDefaultNamespaceDisabled() const;
    
    /**
     * 设置选择器自动化配置
     */
    void setStyleAutoAddClass(bool enable);
    void setStyleAutoAddId(bool enable);
    void setScriptAutoAddClass(bool enable);
    void setScriptAutoAddId(bool enable);
    
    /**
     * 获取选择器自动化配置
     */
    bool isStyleAutoAddClassEnabled() const;
    bool isStyleAutoAddIdEnabled() const;
    bool isScriptAutoAddClassEnabled() const;
    bool isScriptAutoAddIdEnabled() const;
    
    CHTLNodeType getNodeType() const { return CHTLNodeType::CONFIG_NODE; }
};

/**
 * 名称配置节点
 * [Name] { ... }
 */
class NameConfigNode : public ConfigNode {
public:
    /**
     * 构造函数
     */
    NameConfigNode();
    
    /**
     * 设置关键字别名
     */
    void setKeywordAlias(const std::string& keyword, const std::vector<std::string>& aliases);
    
    /**
     * 获取关键字别名
     */
    const std::vector<std::string>& getKeywordAliases(const std::string& keyword) const;
    
    /**
     * 检查是否为关键字别名
     */
    bool isKeywordAlias(const std::string& alias) const;
    
    /**
     * 获取别名对应的原始关键字
     */
    std::string getOriginalKeyword(const std::string& alias) const;
    
    /**
     * 设置选项数量限制
     */
    void setOptionCount(int count);
    
    /**
     * 获取选项数量限制
     */
    int getOptionCount() const;
    
    CHTLNodeType getNodeType() const { return CHTLNodeType::NAME_CONFIG_NODE; }

private:
    std::unordered_map<std::string, std::vector<std::string>> m_keywordAliases;
    std::unordered_map<std::string, std::string> m_aliasToKeyword;
    int m_optionCount;
};

/**
 * 原始类型配置节点
 * [OriginType] { ... }
 */
class OriginTypeConfigNode : public ConfigNode {
public:
    /**
     * 构造函数
     */
    OriginTypeConfigNode();
    
    /**
     * 添加自定义原始类型
     */
    void addCustomOriginType(const std::string& typeName, const std::string& typeIdentifier);
    
    /**
     * 检查是否为自定义原始类型
     */
    bool isCustomOriginType(const std::string& typeIdentifier) const;
    
    /**
     * 获取自定义原始类型名称
     */
    std::string getCustomOriginTypeName(const std::string& typeIdentifier) const;
    
    /**
     * 获取所有自定义原始类型
     */
    const std::unordered_map<std::string, std::string>& getAllCustomOriginTypes() const;
    
    CHTLNodeType getNodeType() const { return CHTLNodeType::ORIGIN_TYPE_CONFIG_NODE; }

private:
    std::unordered_map<std::string, std::string> m_customOriginTypes;  // typeIdentifier -> typeName
};

/**
 * 信息配置节点
 * [Info] { ... }
 */
class InfoConfigNode : public ConfigNode {
public:
    /**
     * 构造函数
     */
    InfoConfigNode();
    
    /**
     * 设置模块信息
     */
    void setModuleInfo(const std::string& name, const std::string& version, 
                      const std::string& description, const std::string& author,
                      const std::string& license);
    
    /**
     * 获取模块信息
     */
    std::string getModuleName() const;
    std::string getModuleVersion() const;
    std::string getModuleDescription() const;
    std::string getModuleAuthor() const;
    std::string getModuleLicense() const;
    
    CHTLNodeType getNodeType() const { return CHTLNodeType::INFO_CONFIG_NODE; }
};

/**
 * 导出配置节点
 * [Export] { ... }
 */
class ExportConfigNode : public ConfigNode {
public:
    /**
     * 构造函数
     */
    ExportConfigNode();
    
    /**
     * 添加导出项
     */
    void addExport(const std::string& type, const std::vector<std::string>& items);
    
    /**
     * 获取导出项
     */
    const std::vector<std::string>& getExports(const std::string& type) const;
    
    /**
     * 获取所有导出项
     */
    const std::unordered_map<std::string, std::vector<std::string>>& getAllExports() const;
    
    CHTLNodeType getNodeType() const { return CHTLNodeType::EXPORT_CONFIG_NODE; }

private:
    std::unordered_map<std::string, std::vector<std::string>> m_exports;
};

} // namespace CHTL