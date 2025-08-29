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

// 高级配置验证器
class AdvancedConfigValidator {
public:
    virtual ~AdvancedConfigValidator() = default;
    
    // 验证单个配置选项
    virtual bool validateOption(const String& name, const ConfigValue& value, 
                              const ConfigOptionMetadata& metadata) const = 0;
    
    // 验证配置组的完整性
    virtual bool validateGroup(const NamedConfigurationGroup& group) const = 0;
    
    // 验证配置之间的依赖关系
    virtual bool validateDependencies(const NamedConfigurationGroup& group) const = 0;
    
    // 验证配置冲突
    virtual bool validateConflicts(const NamedConfigurationGroup& group) const = 0;
    
    // 获取验证错误
    virtual StringVector getValidationErrors(const NamedConfigurationGroup& group) const = 0;
    
    // 获取验证警告
    virtual StringVector getValidationWarnings(const NamedConfigurationGroup& group) const = 0;
    
    // 检查配置安全性
    virtual bool validateSecurity(const NamedConfigurationGroup& group) const = 0;
    
    // 验证配置性能影响
    virtual bool validatePerformance(const NamedConfigurationGroup& group) const = 0;
};

// 标准配置验证器实现
class StandardConfigValidator : public AdvancedConfigValidator {
public:
    bool validateOption(const String& name, const ConfigValue& value, 
                       const ConfigOptionMetadata& metadata) const override;
    bool validateGroup(const NamedConfigurationGroup& group) const override;
    bool validateDependencies(const NamedConfigurationGroup& group) const override;
    bool validateConflicts(const NamedConfigurationGroup& group) const override;
    StringVector getValidationErrors(const NamedConfigurationGroup& group) const override;
    StringVector getValidationWarnings(const NamedConfigurationGroup& group) const override;
    bool validateSecurity(const NamedConfigurationGroup& group) const override;
    bool validatePerformance(const NamedConfigurationGroup& group) const override;
    
private:
    bool validateValueType(const ConfigValue& value, ConfigOptionType expectedType) const;
    bool validateValueRange(const ConfigValue& value, const ConfigValue& min, const ConfigValue& max) const;
    bool validateAllowedValues(const ConfigValue& value, const StringVector& allowedValues) const;
    bool isSecureOption(const String& name, const ConfigValue& value) const;
    bool isPerformanceCritical(const String& name) const;
};

// 配置迁移器 - 处理版本升级
class ConfigurationMigrator {
public:
    struct MigrationRule {
        String fromVersion;              // 源版本
        String toVersion;               // 目标版本
        String oldOptionName;           // 旧选项名
        String newOptionName;           // 新选项名
        std::function<ConfigValue(const ConfigValue&)> transformer; // 值转换器
        String description;             // 迁移说明
    };
    
private:
    std::vector<MigrationRule> migrationRules;
    String currentVersion;
    
public:
    explicit ConfigurationMigrator(const String& version);
    
    // 添加迁移规则
    void addMigrationRule(const MigrationRule& rule);
    void addMigrationRule(const String& fromVersion, const String& toVersion,
                         const String& oldName, const String& newName,
                         std::function<ConfigValue(const ConfigValue&)> transformer = nullptr);
    
    // 执行迁移
    bool migrateConfiguration(NamedConfigurationGroup& group, const String& targetVersion);
    std::unique_ptr<NamedConfigurationGroup> migrateConfiguration(
        const NamedConfigurationGroup& group, const String& targetVersion) const;
    
    // 检查是否需要迁移
    bool needsMigration(const NamedConfigurationGroup& group, const String& targetVersion) const;
    
    // 获取迁移路径
    StringVector getMigrationPath(const String& fromVersion, const String& toVersion) const;
    
    // 验证迁移
    bool validateMigration(const NamedConfigurationGroup& original, 
                          const NamedConfigurationGroup& migrated) const;
    
    // 获取迁移报告
    String getMigrationReport(const String& fromVersion, const String& toVersion) const;
};

// 配置模板系统
class ConfigurationTemplate {
public:
    String templateName;                // 模板名称
    String description;                 // 模板描述
    String category;                    // 模板类别
    StringVector tags;                  // 标签
    std::unordered_map<String, ConfigOptionMetadata> templateOptions; // 模板选项
    StringVector requiredOptions;       // 必需选项
    StringVector optionalOptions;       // 可选选项
    
    explicit ConfigurationTemplate(const String& name);
    
    // 应用模板到配置组
    bool applyToGroup(NamedConfigurationGroup& group) const;
    std::unique_ptr<NamedConfigurationGroup> createFromTemplate() const;
    
    // 验证配置组是否符合模板
    bool validateGroup(const NamedConfigurationGroup& group) const;
    StringVector getComplianceErrors(const NamedConfigurationGroup& group) const;
    
    // 模板合并
    ConfigurationTemplate merge(const ConfigurationTemplate& other) const;
    
    // 序列化
    String toJSON() const;
    bool fromJSON(const String& json);
};

// 配置预设管理器
class ConfigurationPresetManager {
private:
    std::unordered_map<String, std::unique_ptr<NamedConfigurationGroup>> presets;
    std::unordered_map<String, std::unique_ptr<ConfigurationTemplate>> templates;
    String presetsDirectory;
    
public:
    explicit ConfigurationPresetManager(const String& presetsDir = "");
    
    // 预设管理
    void addPreset(const String& name, std::unique_ptr<NamedConfigurationGroup> preset);
    std::unique_ptr<NamedConfigurationGroup> getPreset(const String& name) const;
    bool hasPreset(const String& name) const;
    void removePreset(const String& name);
    StringVector getPresetNames() const;
    
    // 模板管理
    void addTemplate(const String& name, std::unique_ptr<ConfigurationTemplate> template_);
    std::unique_ptr<ConfigurationTemplate> getTemplate(const String& name) const;
    bool hasTemplate(const String& name) const;
    void removeTemplate(const String& name);
    StringVector getTemplateNames() const;
    
    // 文件操作
    bool loadPresetsFromDirectory();
    bool savePresetToFile(const String& name) const;
    bool loadPresetFromFile(const String& filePath);
    
    // 搜索和过滤
    StringVector searchPresets(const String& query) const;
    StringVector getPresetsByCategory(const String& category) const;
    StringVector getPresetsByTag(const String& tag) const;
    
    // 创建预设
    std::unique_ptr<NamedConfigurationGroup> createPresetFromTemplate(
        const String& templateName, const StringUnorderedMap& customOptions = {}) const;
};

// 动态配置监听器
class ConfigurationListener {
public:
    virtual ~ConfigurationListener() = default;
    
    // 配置变更事件
    virtual void onConfigurationChanged(const String& groupName, 
                                      const String& optionName, 
                                      const ConfigValue& oldValue, 
                                      const ConfigValue& newValue) = 0;
    
    // 配置组创建事件
    virtual void onConfigurationGroupCreated(const String& groupName) = 0;
    
    // 配置组删除事件
    virtual void onConfigurationGroupDeleted(const String& groupName) = 0;
    
    // 配置验证失败事件
    virtual void onValidationFailed(const String& groupName, 
                                  const StringVector& errors) = 0;
};

// 增强配置监控器
class AdvancedConfigurationMonitor {
private:
    std::vector<std::shared_ptr<ConfigurationListener>> listeners;
    std::unordered_map<String, std::chrono::milliseconds> changeTimestamps;
    bool isMonitoring;
    
public:
    AdvancedConfigurationMonitor();
    
    // 监听器管理
    void addListener(std::shared_ptr<ConfigurationListener> listener);
    void removeListener(std::shared_ptr<ConfigurationListener> listener);
    void clearListeners();
    
    // 事件触发
    void notifyConfigurationChanged(const String& groupName, 
                                  const String& optionName, 
                                  const ConfigValue& oldValue, 
                                  const ConfigValue& newValue);
    void notifyConfigurationGroupCreated(const String& groupName);
    void notifyConfigurationGroupDeleted(const String& groupName);
    void notifyValidationFailed(const String& groupName, const StringVector& errors);
    
    // 监控控制
    void startMonitoring();
    void stopMonitoring();
    bool isMonitoringActive() const { return isMonitoring; }
    
    // 变更历史
    std::chrono::milliseconds getLastChangeTime(const String& groupName) const;
    StringVector getRecentChanges(std::chrono::milliseconds since) const;
};

// 超级配置管理器 - 整合所有功能
class SuperAdvancedConfigurationManager : public AdvancedConfigurationManager {
private:
    std::unique_ptr<AdvancedConfigValidator> validator;
    std::unique_ptr<ConfigurationMigrator> migrator;
    std::unique_ptr<ConfigurationPresetManager> presetManager;
    std::unique_ptr<AdvancedConfigurationMonitor> monitor;
    
    // 缓存和性能
    mutable std::unordered_map<String, ConfigValue> valueCache;
    mutable std::chrono::milliseconds lastCacheUpdate;
    bool enableCaching;
    size_t maxCacheSize;
    
    // 安全和权限
    std::unordered_map<String, StringUnorderedSet> permissions; // group -> allowed operations
    bool enableSecurity;
    String currentUser;
    
    // 性能统计
    mutable std::unordered_map<String, size_t> accessCounts;
    mutable std::unordered_map<String, std::chrono::milliseconds> operationTimes;
    
public:
    SuperAdvancedConfigurationManager();
    ~SuperAdvancedConfigurationManager() override = default;
    
    // 组件访问
    AdvancedConfigValidator* getValidator() const { return validator.get(); }
    ConfigurationMigrator* getMigrator() const { return migrator.get(); }
    ConfigurationPresetManager* getPresetManager() const { return presetManager.get(); }
    AdvancedConfigurationMonitor* getMonitor() const { return monitor.get(); }
    
    // 设置组件
    void setValidator(std::unique_ptr<AdvancedConfigValidator> val);
    void setMigrator(std::unique_ptr<ConfigurationMigrator> mig);
    void setPresetManager(std::unique_ptr<ConfigurationPresetManager> preset);
    void setMonitor(std::unique_ptr<AdvancedConfigurationMonitor> mon);
    
    // 增强的配置组管理
    bool createConfigurationGroupFromTemplate(const String& groupName, 
                                            const String& templateName,
                                            const StringUnorderedMap& customOptions = {}) override;
    bool validateConfigurationGroup(const String& groupName) const;
    bool migrateConfigurationGroup(const String& groupName, const String& targetVersion);
    
    // 配置验证和安全
    StringVector getValidationErrors(const String& groupName) const;
    StringVector getValidationWarnings(const String& groupName) const;
    bool isConfigurationSecure(const String& groupName) const;
    
    // 权限管理
    void setUserPermissions(const String& user, const String& groupName, 
                           const StringUnorderedSet& allowedOps);
    bool hasPermission(const String& user, const String& groupName, 
                      const String& operation) const;
    void enableSecurityMode(bool enable = true) { enableSecurity = enable; }
    
    // 缓存管理
    void enableValueCaching(bool enable = true, size_t maxSize = 1000);
    void invalidateCache();
    void invalidateCache(const String& groupName);
    size_t getCacheSize() const { return valueCache.size(); }
    double getCacheHitRatio() const;
    
    // 性能监控
    std::unordered_map<String, size_t> getAccessStatistics() const { return accessCounts; }
    std::unordered_map<String, std::chrono::milliseconds> getPerformanceStatistics() const { return operationTimes; }
    void resetStatistics();
    
    // 配置导入导出
    bool exportConfigurationGroup(const String& groupName, const String& filePath) const;
    bool importConfigurationGroup(const String& filePath);
    String exportAllConfigurations() const;
    bool importAllConfigurations(const String& data);
    
    // 配置比较和合并
    StringVector compareConfigurationGroups(const String& group1, const String& group2) const;
    bool mergeConfigurationGroups(const String& targetGroup, const String& sourceGroup, 
                                 bool overwriteExisting = false);
    
    // 配置搜索
    StringVector searchConfigurations(const String& query) const;
    StringVector getConfigurationsByCategory(const String& category) const;
    StringVector getConfigurationsByTag(const String& tag) const;
    
    // 配置历史和回滚
    bool saveConfigurationSnapshot(const String& groupName, const String& snapshotName);
    bool restoreConfigurationSnapshot(const String& groupName, const String& snapshotName);
    StringVector getConfigurationSnapshots(const String& groupName) const;
    
    // 配置优化
    void optimizeConfigurations();
    void compactConfigurations();
    size_t getConfigurationMemoryUsage() const;
    
    // 诊断和调试
    String generateDiagnosticReport() const;
    String generatePerformanceReport() const;
    String generateSecurityReport() const;
    bool validateAllConfigurations() const;
    
private:
    // 内部方法
    void recordAccess(const String& operation) const;
    void recordOperationTime(const String& operation, std::chrono::milliseconds time) const;
    bool checkPermission(const String& groupName, const String& operation) const;
    void updateCache(const String& key, const ConfigValue& value) const;
    ConfigValue getCachedValue(const String& key) const;
    bool isCached(const String& key) const;
    void cleanupCache();
};

} // namespace CHTL