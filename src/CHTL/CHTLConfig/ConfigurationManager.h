#pragma once

#include "../Util/Common.h"
#include "../CHTLNode/BaseNode.h"
#include "ConfigNode.h"
#include <unordered_map>
#include <memory>

namespace CHTL {

// 配置类型枚举
enum class ConfigType {
    GLOBAL,         // 全局配置
    NAMED,          // 命名配置组
    NAME_BLOCK,     // Name配置块
    ORIGIN_TYPE     // OriginType配置块
};

// 关键字配置项
struct KeywordConfig {
    String keyword;             // 关键字
    StringVector alternatives;  // 可选形式 (如[@Style, @style, @CSS])
    bool isGroupOption;        // 是否为组选项
    size_t optionCount;        // 组选项数量限制
    
    KeywordConfig(const String& kw = "", const StringVector& alts = {}, bool isGroup = false, size_t count = 1)
        : keyword(kw), alternatives(alts), isGroupOption(isGroup), optionCount(count) {}
};

// 原始类型配置项
struct OriginTypeConfig {
    String typeName;           // 类型名称 (如Vue)
    String fullName;           // 全写名称 (如@Vue)
    String implementation;     // 实现内容
    bool isCustom;            // 是否为自定义类型
    
    OriginTypeConfig(const String& name = "", const String& full = "", const String& impl = "", bool custom = true)
        : typeName(name), fullName(full), implementation(impl), isCustom(custom) {}
};

// 配置管理器
class ConfigurationManager {
private:
    // 配置存储
    std::unordered_map<String, std::unique_ptr<ConfigNode>> namedConfigs;
    std::unique_ptr<ConfigNode> globalConfig;
    
    // 关键字配置
    std::unordered_map<String, KeywordConfig> keywordConfigs;
    
    // 原始类型配置
    std::unordered_map<String, OriginTypeConfig> originTypeConfigs;
    
    // 配置状态
    bool isEnabled;
    String activeConfigName;
    
public:
    ConfigurationManager();
    ~ConfigurationManager() = default;
    
    // 配置节点管理
    void addNamedConfig(const String& name, std::unique_ptr<ConfigNode> config);
    void setGlobalConfig(std::unique_ptr<ConfigNode> config);
    
    ConfigNode* getNamedConfig(const String& name) const;
    ConfigNode* getGlobalConfig() const;
    ConfigNode* getActiveConfig() const;
    
    // 配置激活
    void activateConfig(const String& configName);
    void activateGlobalConfig();
    String getActiveConfigName() const { return activeConfigName; }
    bool hasActiveConfig() const { return !activeConfigName.empty(); }
    
    // 关键字配置管理
    void addKeywordConfig(const String& configKey, const KeywordConfig& config);
    void setKeywordAlternatives(const String& configKey, const StringVector& alternatives);
    
    KeywordConfig getKeywordConfig(const String& configKey) const;
    StringVector getKeywordAlternatives(const String& configKey) const;
    bool hasKeywordConfig(const String& configKey) const;
    
    // 原始类型配置管理
    void addOriginTypeConfig(const String& typeName, const OriginTypeConfig& config);
    void registerCustomOriginType(const String& typeName, const String& fullName = "");
    
    OriginTypeConfig getOriginTypeConfig(const String& typeName) const;
    bool hasOriginTypeConfig(const String& typeName) const;
    bool isCustomOriginType(const String& typeName) const;
    StringVector getAllOriginTypes() const;
    
    // 配置值获取 (统一接口)
    String getConfigValue(const String& key, const String& defaultValue = "") const;
    bool getBoolConfigValue(const String& key, bool defaultValue = false) const;
    int getIntConfigValue(const String& key, int defaultValue = 0) const;
    
    // 特定配置项的便捷方法
    size_t getIndexInitialCount() const;
    bool isNameGroupDisabled() const;
    bool isCustomOriginTypeDisabled() const;
    bool isDebugMode() const;
    bool isStyleAutoAddClassDisabled() const;
    bool isStyleAutoAddIdDisabled() const;
    bool isScriptAutoAddClassDisabled() const;
    bool isScriptAutoAddIdDisabled() const;
    bool isDefaultNamespaceDisabled() const;
    size_t getOptionCount() const;
    
    // 动态关键字映射
    String mapKeyword(const String& keyword) const;
    StringVector mapKeywordAlternatives(const String& keyword) const;
    bool isValidKeyword(const String& keyword, const String& configKey) const;
    
    // 配置验证
    bool validateConfiguration() const;
    bool validateNameConfig() const;
    bool validateOriginTypeConfig() const;
    StringVector getValidationErrors() const;
    
    // 配置导入/导出
    bool importConfiguration(const ConfigNode* configNode);
    std::unique_ptr<ConfigNode> exportConfiguration() const;
    
    // 配置重置
    void resetToDefaults();
    void clearNamedConfigs();
    void clearKeywordConfigs();
    void clearOriginTypeConfigs();
    
    // 调试和状态
    void printConfiguration() const;
    size_t getConfigCount() const;
    StringVector getConfigNames() const;
    
    // 静态默认配置
    static ConfigurationManager createDefaultManager();
    static KeywordConfig createDefaultKeywordConfig(const String& configKey);
    static StringVector getDefaultKeywords();
    
private:
    // 初始化方法
    void initializeDefaults();
    void setupDefaultKeywords();
    void setupDefaultOriginTypes();
    
    // 辅助方法
    bool isValidConfigKey(const String& key) const;
    String normalizeConfigKey(const String& key) const;
    void applyNameConfiguration(const StringUnorderedMap& nameConfig);
    void applyOriginTypeConfiguration(const StringUnorderedMap& originTypeConfig);
};

// 配置解析器
class ConfigurationParser {
public:
    // 解析Name配置块
    static std::unordered_map<String, KeywordConfig> parseNameConfiguration(const StringUnorderedMap& nameConfig);
    
    // 解析OriginType配置块
    static std::unordered_map<String, OriginTypeConfig> parseOriginTypeConfiguration(const StringUnorderedMap& originTypeConfig);
    
    // 解析关键字组选项
    static StringVector parseGroupOptions(const String& optionString);
    
    // 解析配置值
    static bool parseBoolean(const String& value);
    static int parseInt(const String& value);
    static StringVector parseStringList(const String& value);
    
    // 验证配置格式
    static bool validateConfigFormat(const String& key, const String& value);
    static bool validateKeywordConfig(const String& configKey, const String& configValue);
    static bool validateOriginTypeConfig(const String& configKey, const String& configValue);
    
    // 配置错误处理
    static String generateConfigError(const String& key, const String& value, const String& reason);
    
private:
    // 解析辅助方法
    static bool isGroupOptionSyntax(const String& value);
    static String extractGroupOptionName(const String& value);
    static StringVector extractGroupOptionValues(const String& value);
    static bool isValidOriginTypeName(const String& typeName);
};

// 全局配置实例
extern ConfigurationManager* g_configManager;

// 配置初始化函数
void initializeGlobalConfiguration();
void shutdownGlobalConfiguration();

// 便捷访问函数
ConfigurationManager& getGlobalConfigManager();
String getGlobalConfigValue(const String& key, const String& defaultValue = "");
bool getGlobalBoolConfig(const String& key, bool defaultValue = false);

} // namespace CHTL