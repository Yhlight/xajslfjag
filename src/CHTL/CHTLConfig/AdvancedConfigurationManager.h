#pragma once

#include "ConfigurationManager.h"
#include "../../Util/Common.h"
#include <unordered_map>
#include <vector>
#include <variant>

namespace CHTL {

// 配置选项值类型
using ConfigValue = std::variant<String, int, double, bool, StringVector>;

// 配置选项类型
enum class ConfigOptionType {
    STRING,         // 字符串选项
    INTEGER,        // 整数选项
    DOUBLE,         // 浮点数选项
    BOOLEAN,        // 布尔选项
    STRING_ARRAY,   // 字符串数组选项
    GROUP_OPTION,   // 组选项 (多个值的选择)
    KEYWORD_MAP,    // 关键字映射
    ENUM_OPTION     // 枚举选项
};

// 配置选项元数据
struct ConfigOptionMetadata {
    String name;                    // 选项名称
    String description;             // 选项描述
    ConfigOptionType type;          // 选项类型
    ConfigValue defaultValue;       // 默认值
    ConfigValue minValue;           // 最小值 (数值类型)
    ConfigValue maxValue;           // 最大值 (数值类型)
    StringVector allowedValues;     // 允许的值列表
    bool isRequired;                // 是否必需
    bool isDeprecated;              // 是否已弃用
    String deprecationMessage;      // 弃用消息
    StringVector dependencies;      // 依赖的其他选项
    StringVector conflicts;         // 冲突的选项
    
    ConfigOptionMetadata() : type(ConfigOptionType::STRING), isRequired(false), isDeprecated(false) {}
};

// 组选项配置
struct GroupOptionConfig {
    String groupName;               // 组名称
    StringVector options;           // 组内选项列表
    size_t maxCount;               // 最大选项数量
    size_t minCount;               // 最小选项数量
    bool allowDuplicates;          // 是否允许重复
    bool isCaseSensitive;          // 是否大小写敏感
    String separator;              // 分隔符
    
    GroupOptionConfig() : maxCount(10), minCount(1), allowDuplicates(false), isCaseSensitive(true), separator(",") {}
};

// 关键字映射配置
struct KeywordMappingConfig {
    String keyword;                 // 关键字
    StringVector aliases;           // 别名列表
    String category;                // 类别
    bool isCore;                   // 是否核心关键字
    bool isExtensible;             // 是否可扩展
    String namespace_;             // 命名空间
    
    KeywordMappingConfig() : isCore(true), isExtensible(false) {}
};

// 命名配置组
class NamedConfigurationGroup {
public:
    String groupName;               // 配置组名称
    String description;             // 配置组描述
    String version;                 // 配置组版本
    String author;                  // 作者
    bool isDefault;                 // 是否默认配置组
    std::unordered_map<String, ConfigValue> options; // 配置选项
    std::unordered_map<String, GroupOptionConfig> groupOptions; // 组选项
    std::unordered_map<String, KeywordMappingConfig> keywordMappings; // 关键字映射
    
    explicit NamedConfigurationGroup(const String& name);
    ~NamedConfigurationGroup() = default;
    
    // 选项管理
    void setOption(const String& name, const ConfigValue& value);
    ConfigValue getOption(const String& name) const;
    bool hasOption(const String& name) const;
    void removeOption(const String& name);
    
    // 组选项管理
    void setGroupOption(const String& name, const GroupOptionConfig& config);
    GroupOptionConfig getGroupOption(const String& name) const;
    bool hasGroupOption(const String& name) const;
    void addGroupOptionValue(const String& groupName, const String& value);
    StringVector getGroupOptionValues(const String& groupName) const;
    
    // 关键字映射管理
    void setKeywordMapping(const String& keyword, const KeywordMappingConfig& config);
    KeywordMappingConfig getKeywordMapping(const String& keyword) const;
    bool hasKeywordMapping(const String& keyword) const;
    void addKeywordAlias(const String& keyword, const String& alias);
    StringVector getKeywordAliases(const String& keyword) const;
    String resolveKeyword(const String& input) const;
    
    // 验证
    bool validateConfiguration() const;
    StringVector getValidationErrors() const;
    bool isCompatibleWith(const NamedConfigurationGroup& other) const;
    
    // 序列化
    String toString() const;
    String toJSON() const;
    bool fromString(const String& configStr);
    bool fromJSON(const String& jsonStr);
    
    // 合并和继承
    void mergeFrom(const NamedConfigurationGroup& other);
    void inheritFrom(const NamedConfigurationGroup& parent);
    NamedConfigurationGroup createChild(const String& childName) const;
    
private:
    // 内部验证方法
    bool validateOption(const String& name, const ConfigValue& value) const;
    bool validateGroupOption(const String& name, const GroupOptionConfig& config) const;
    bool validateKeywordMapping(const String& keyword, const KeywordMappingConfig& config) const;
    
    // 序列化辅助
    String configValueToString(const ConfigValue& value) const;
    ConfigValue stringToConfigValue(const String& str, ConfigOptionType type) const;
};

// 高级配置管理器
class AdvancedConfigurationManager : public ConfigurationManager {
public:
    AdvancedConfigurationManager();
    ~AdvancedConfigurationManager() override = default;
    
    // 命名配置组管理
    void registerNamedGroup(const String& name, std::shared_ptr<NamedConfigurationGroup> group);
    void unregisterNamedGroup(const String& name);
    std::shared_ptr<NamedConfigurationGroup> getNamedGroup(const String& name) const;
    bool hasNamedGroup(const String& name) const;
    StringVector getAllGroupNames() const;
    
    // 默认配置组设置
    void setDefaultGroup(const String& groupName);
    String getDefaultGroupName() const;
    std::shared_ptr<NamedConfigurationGroup> getDefaultGroup() const;
    
    // 配置选项元数据管理
    void registerOptionMetadata(const String& optionName, const ConfigOptionMetadata& metadata);
    ConfigOptionMetadata getOptionMetadata(const String& optionName) const;
    bool hasOptionMetadata(const String& optionName) const;
    StringVector getAllOptionNames() const;
    
    // 组选项处理
    void registerGroupOptionType(const String& typeName, const GroupOptionConfig& config);
    GroupOptionConfig getGroupOptionType(const String& typeName) const;
    bool parseGroupOption(const String& input, const String& typeName, StringVector& result) const;
    String formatGroupOption(const StringVector& values, const String& typeName) const;
    
    // 关键字映射系统
    void registerKeywordCategory(const String& category, const StringVector& keywords);
    StringVector getKeywordCategory(const String& category) const;
    String resolveKeywordAlias(const String& alias) const;
    void addGlobalKeywordAlias(const String& keyword, const String& alias);
    StringVector getKeywordAliases(const String& keyword) const;
    
    // 配置继承和层次结构
    void setConfigurationHierarchy(const StringVector& hierarchy);
    StringVector getConfigurationHierarchy() const;
    ConfigValue resolveOptionWithInheritance(const String& optionName) const;
    
    // 配置验证和诊断
    bool validateAllConfigurations() const;
    StringVector getAllValidationErrors() const;
    void performCompatibilityCheck() const;
    void generateConfigurationReport() const;
    
    // 动态配置更新
    void enableDynamicUpdates(bool enable) { dynamicUpdatesEnabled = enable; }
    bool isDynamicUpdatesEnabled() const { return dynamicUpdatesEnabled; }
    void subscribeToConfigChanges(const String& optionName, std::function<void(const ConfigValue&)> callback);
    void notifyConfigChange(const String& optionName, const ConfigValue& newValue);
    
    // 配置导入导出
    bool exportConfiguration(const String& filePath, const String& format = "chtl") const;
    bool importConfiguration(const String& filePath, const String& format = "chtl");
    String exportConfigurationToString(const String& format = "chtl") const;
    bool importConfigurationFromString(const String& configStr, const String& format = "chtl");
    
    // 预设配置管理
    void registerPresetConfiguration(const String& presetName, std::shared_ptr<NamedConfigurationGroup> preset);
    void loadPresetConfiguration(const String& presetName);
    StringVector getAvailablePresets() const;
    
    // 配置性能优化
    void enableConfigurationCaching(bool enable) { cachingEnabled = enable; }
    void clearConfigurationCache();
    void optimizeConfigurationAccess();
    
    // 调试和诊断
    void enableDebugMode(bool enable) { debugMode = enable; }
    void dumpConfigurationState() const;
    void validateConfigurationIntegrity() const;
    
private:
    std::unordered_map<String, std::shared_ptr<NamedConfigurationGroup>> namedGroups;
    std::unordered_map<String, ConfigOptionMetadata> optionMetadata;
    std::unordered_map<String, GroupOptionConfig> groupOptionTypes;
    std::unordered_map<String, StringVector> keywordCategories;
    std::unordered_map<String, String> globalKeywordAliases;
    std::unordered_map<String, StringVector> keywordHierarchy;
    std::unordered_map<String, std::shared_ptr<NamedConfigurationGroup>> presetConfigurations;
    
    String defaultGroupName;
    StringVector configurationHierarchy;
    bool dynamicUpdatesEnabled;
    bool cachingEnabled;
    bool debugMode;
    
    // 回调系统
    std::unordered_map<String, std::vector<std::function<void(const ConfigValue&)>>> configChangeCallbacks;
    
    // 缓存系统
    mutable std::unordered_map<String, ConfigValue> resolvedOptionCache;
    mutable std::unordered_map<String, StringVector> resolvedGroupOptionCache;
    
    // 内部辅助方法
    void initializeDefaultMetadata();
    void initializeDefaultGroupOptions();
    void initializeDefaultKeywordMappings();
    
    // 配置解析
    ConfigValue parseConfigValue(const String& valueStr, ConfigOptionType expectedType) const;
    String formatConfigValue(const ConfigValue& value) const;
    
    // 验证辅助
    bool validateConfigValue(const ConfigValue& value, const ConfigOptionMetadata& metadata) const;
    bool checkOptionDependencies(const String& optionName) const;
    bool checkOptionConflicts(const String& optionName) const;
    
    // 缓存管理
    void invalidateCache(const String& optionName);
    String generateCacheKey(const String& optionName, const StringVector& context = {}) const;
    
    // 错误处理
    void logConfigurationError(const String& message) const;
    void logConfigurationWarning(const String& message) const;
};

// 配置构建器 - 流式API
class ConfigurationBuilder {
public:
    ConfigurationBuilder(const String& groupName);
    ~ConfigurationBuilder() = default;
    
    // 流式配置方法
    ConfigurationBuilder& description(const String& desc);
    ConfigurationBuilder& version(const String& ver);
    ConfigurationBuilder& author(const String& authorName);
    ConfigurationBuilder& setDefault(bool isDefault = true);
    
    // 选项配置
    ConfigurationBuilder& option(const String& name, const ConfigValue& value);
    ConfigurationBuilder& stringOption(const String& name, const String& value, const String& description = "");
    ConfigurationBuilder& intOption(const String& name, int value, int minVal = 0, int maxVal = 1000);
    ConfigurationBuilder& boolOption(const String& name, bool value, const String& description = "");
    ConfigurationBuilder& arrayOption(const String& name, const StringVector& values);
    
    // 组选项配置
    ConfigurationBuilder& groupOption(const String& name, const StringVector& options, size_t maxCount = 10);
    ConfigurationBuilder& keywordGroup(const String& name, const StringVector& keywords);
    
    // 关键字映射配置
    ConfigurationBuilder& keywordMapping(const String& keyword, const StringVector& aliases);
    ConfigurationBuilder& coreKeyword(const String& keyword, const StringVector& aliases = {});
    ConfigurationBuilder& extensibleKeyword(const String& keyword, const StringVector& aliases = {});
    
    // 构建方法
    std::shared_ptr<NamedConfigurationGroup> build();
    String buildToString();
    bool buildToFile(const String& filePath);
    
private:
    std::shared_ptr<NamedConfigurationGroup> configGroup;
    StringVector buildErrors;
    
    // 验证构建
    bool validateBuild() const;
    void addBuildError(const String& error);
};

// 内置预设配置
namespace ConfigPresets {
    // 基础配置预设
    std::shared_ptr<NamedConfigurationGroup> createBasicPreset();
    
    // 高级配置预设
    std::shared_ptr<NamedConfigurationGroup> createAdvancedPreset();
    
    // 性能优化配置预设
    std::shared_ptr<NamedConfigurationGroup> createPerformancePreset();
    
    // 调试配置预设
    std::shared_ptr<NamedConfigurationGroup> createDebugPreset();
    
    // Web应用配置预设
    std::shared_ptr<NamedConfigurationGroup> createWebAppPreset();
    
    // 移动端配置预设
    std::shared_ptr<NamedConfigurationGroup> createMobilePreset();
}

} // namespace CHTL