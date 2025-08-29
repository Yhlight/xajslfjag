#include "ConfigurationManager.h"
#include "../CHTLLexer/GlobalMap.h"
#include <iostream>
#include <sstream>
#include <algorithm>

namespace CHTL {

// 全局配置实例
ConfigurationManager* g_configManager = nullptr;

// ========== ConfigurationManager 实现 ==========

ConfigurationManager::ConfigurationManager() : isEnabled(true) {
    initializeDefaults();
}

void ConfigurationManager::addNamedConfig(const String& name, std::unique_ptr<ConfigNode> config) {
    namedConfigs[name] = std::move(config);
}

void ConfigurationManager::setGlobalConfig(std::unique_ptr<ConfigNode> config) {
    globalConfig = std::move(config);
    if (activeConfigName.empty()) {
        activeConfigName = ""; // 表示使用全局配置
    }
}

ConfigNode* ConfigurationManager::getNamedConfig(const String& name) const {
    auto it = namedConfigs.find(name);
    return (it != namedConfigs.end()) ? it->second.get() : nullptr;
}

ConfigNode* ConfigurationManager::getGlobalConfig() const {
    return globalConfig.get();
}

ConfigNode* ConfigurationManager::getActiveConfig() const {
    if (activeConfigName.empty()) {
        return getGlobalConfig();
    }
    return getNamedConfig(activeConfigName);
}

void ConfigurationManager::activateConfig(const String& configName) {
    activeConfigName = configName;
}

void ConfigurationManager::activateGlobalConfig() {
    activeConfigName = "";
}

void ConfigurationManager::addKeywordConfig(const String& configKey, const KeywordConfig& config) {
    keywordConfigs[configKey] = config;
}

void ConfigurationManager::setKeywordAlternatives(const String& configKey, const StringVector& alternatives) {
    auto it = keywordConfigs.find(configKey);
    if (it != keywordConfigs.end()) {
        it->second.alternatives = alternatives;
        it->second.isGroupOption = alternatives.size() > 1;
    } else {
        // 创建新的关键字配置
        KeywordConfig config;
        config.keyword = configKey;
        config.alternatives = alternatives;
        config.isGroupOption = alternatives.size() > 1;
        config.optionCount = alternatives.size();
        keywordConfigs[configKey] = config;
    }
}

KeywordConfig ConfigurationManager::getKeywordConfig(const String& configKey) const {
    auto it = keywordConfigs.find(configKey);
    return (it != keywordConfigs.end()) ? it->second : KeywordConfig();
}

StringVector ConfigurationManager::getKeywordAlternatives(const String& configKey) const {
    auto config = getKeywordConfig(configKey);
    return config.alternatives;
}

bool ConfigurationManager::hasKeywordConfig(const String& configKey) const {
    return keywordConfigs.find(configKey) != keywordConfigs.end();
}

void ConfigurationManager::addOriginTypeConfig(const String& typeName, const OriginTypeConfig& config) {
    originTypeConfigs[typeName] = config;
}

void ConfigurationManager::registerCustomOriginType(const String& typeName, const String& fullName) {
    String resolvedFullName = fullName.empty() ? ("@" + typeName) : fullName;
    OriginTypeConfig config(typeName, resolvedFullName, "", true);
    addOriginTypeConfig(typeName, config);
}

OriginTypeConfig ConfigurationManager::getOriginTypeConfig(const String& typeName) const {
    auto it = originTypeConfigs.find(typeName);
    return (it != originTypeConfigs.end()) ? it->second : OriginTypeConfig();
}

bool ConfigurationManager::hasOriginTypeConfig(const String& typeName) const {
    return originTypeConfigs.find(typeName) != originTypeConfigs.end();
}

bool ConfigurationManager::isCustomOriginType(const String& typeName) const {
    auto config = getOriginTypeConfig(typeName);
    return config.isCustom;
}

StringVector ConfigurationManager::getAllOriginTypes() const {
    StringVector types;
    for (const auto& pair : originTypeConfigs) {
        types.push_back(pair.first);
    }
    return types;
}

String ConfigurationManager::getConfigValue(const String& key, const String& defaultValue) const {
    auto* activeConfig = getActiveConfig();
    if (activeConfig) {
        String value = activeConfig->getConfigValue(key, defaultValue);
        if (!value.empty() && value != defaultValue) {
            return value;
        }
    }
    
    // 如果没有找到，查找全局配置
    if (activeConfig != getGlobalConfig() && getGlobalConfig()) {
        return getGlobalConfig()->getConfigValue(key, defaultValue);
    }
    
    return defaultValue;
}

bool ConfigurationManager::getBoolConfigValue(const String& key, bool defaultValue) const {
    String value = getConfigValue(key, defaultValue ? "true" : "false");
    return ConfigurationParser::parseBoolean(value);
}

int ConfigurationManager::getIntConfigValue(const String& key, int defaultValue) const {
    String value = getConfigValue(key, std::to_string(defaultValue));
    return ConfigurationParser::parseInt(value);
}

size_t ConfigurationManager::getIndexInitialCount() const {
    return static_cast<size_t>(getIntConfigValue("INDEX_INITIAL_COUNT", 0));
}

bool ConfigurationManager::isNameGroupDisabled() const {
    return getBoolConfigValue("DISABLE_NAME_GROUP", true);
}

bool ConfigurationManager::isCustomOriginTypeDisabled() const {
    return getBoolConfigValue("DISABLE_CUSTOM_ORIGIN_TYPE", false);
}

bool ConfigurationManager::isDebugMode() const {
    return getBoolConfigValue("DEBUG_MODE", false);
}

bool ConfigurationManager::isStyleAutoAddClassDisabled() const {
    return getBoolConfigValue("DISABLE_STYLE_AUTO_ADD_CLASS", false);
}

bool ConfigurationManager::isStyleAutoAddIdDisabled() const {
    return getBoolConfigValue("DISABLE_STYLE_AUTO_ADD_ID", false);
}

bool ConfigurationManager::isScriptAutoAddClassDisabled() const {
    return getBoolConfigValue("DISABLE_SCRIPT_AUTO_ADD_CLASS", true);
}

bool ConfigurationManager::isScriptAutoAddIdDisabled() const {
    return getBoolConfigValue("DISABLE_SCRIPT_AUTO_ADD_ID", true);
}

bool ConfigurationManager::isDefaultNamespaceDisabled() const {
    return getBoolConfigValue("DISABLE_DEFAULT_NAMESPACE", false);
}

size_t ConfigurationManager::getOptionCount() const {
    return static_cast<size_t>(getIntConfigValue("OPTION_COUNT", 3));
}

String ConfigurationManager::mapKeyword(const String& keyword) const {
    // 在Name配置中查找关键字映射
    auto* activeConfig = getActiveConfig();
    if (activeConfig) {
        String mapped = activeConfig->getNameConfig(keyword);
        if (!mapped.empty()) {
            return mapped;
        }
    }
    
    // 在关键字配置中查找
    for (const auto& pair : keywordConfigs) {
        const auto& config = pair.second;
        if (config.keyword == keyword && !config.alternatives.empty()) {
            return config.alternatives[0]; // 返回主要形式
        }
        
        // 检查是否是某个关键字的替代形式
        for (const auto& alt : config.alternatives) {
            if (alt == keyword) {
                return config.keyword; // 返回标准形式
            }
        }
    }
    
    return keyword; // 未找到映射，返回原值
}

StringVector ConfigurationManager::mapKeywordAlternatives(const String& keyword) const {
    String mappedKeyword = mapKeyword(keyword);
    return getKeywordAlternatives(mappedKeyword);
}

bool ConfigurationManager::isValidKeyword(const String& keyword, const String& configKey) const {
    auto alternatives = getKeywordAlternatives(configKey);
    return std::find(alternatives.begin(), alternatives.end(), keyword) != alternatives.end();
}

bool ConfigurationManager::validateConfiguration() const {
    return validateNameConfig() && validateOriginTypeConfig();
}

bool ConfigurationManager::validateNameConfig() const {
    // 验证Name配置的有效性
    for (const auto& pair : keywordConfigs) {
        const auto& config = pair.second;
        
        // 验证组选项数量限制
        if (config.isGroupOption && config.alternatives.size() > getOptionCount()) {
            return false;
        }
        
        // 验证关键字格式
        if (config.keyword.empty()) {
            return false;
        }
    }
    
    return true;
}

bool ConfigurationManager::validateOriginTypeConfig() const {
    // 验证OriginType配置的有效性
    for (const auto& pair : originTypeConfigs) {
        const auto& config = pair.second;
        
        // 验证类型名称格式
        if (config.typeName.empty() || config.fullName.empty()) {
            return false;
        }
        
        // 验证全名格式 (应该以@开头)
        if (config.fullName.front() != '@') {
            return false;
        }
    }
    
    return true;
}

StringVector ConfigurationManager::getValidationErrors() const {
    StringVector errors;
    
    if (!validateNameConfig()) {
        errors.push_back("Name配置验证失败");
    }
    
    if (!validateOriginTypeConfig()) {
        errors.push_back("OriginType配置验证失败");
    }
    
    return errors;
}

bool ConfigurationManager::importConfiguration(const ConfigNode* configNode) {
    if (!configNode) return false;
    
    try {
        // 导入基本配置值
        for (const auto& pair : configNode->configValues) {
            // 根据配置类型处理不同的配置项
            // 这里可以添加更复杂的配置导入逻辑
        }
        
        // 导入Name配置
        applyNameConfiguration(configNode->nameConfig);
        
        // 导入OriginType配置
        applyOriginTypeConfiguration(configNode->originTypeConfig);
        
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

std::unique_ptr<ConfigNode> ConfigurationManager::exportConfiguration() const {
    auto configNode = std::make_unique<ConfigNode>();
    
    // 导出关键字配置为Name配置
    for (const auto& pair : keywordConfigs) {
        const auto& config = pair.second;
        if (!config.alternatives.empty()) {
            // 将替代形式组合为配置值
            std::ostringstream oss;
            if (config.isGroupOption) {
                oss << "[";
                for (size_t i = 0; i < config.alternatives.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << config.alternatives[i];
                }
                oss << "]";
            } else {
                oss << config.alternatives[0];
            }
            configNode->setNameConfig(pair.first, oss.str());
        }
    }
    
    // 导出原始类型配置
    for (const auto& pair : originTypeConfigs) {
        const auto& config = pair.second;
        configNode->setOriginTypeConfig("ORIGINTYPE_" + config.typeName, config.fullName);
    }
    
    return configNode;
}

void ConfigurationManager::resetToDefaults() {
    clearNamedConfigs();
    clearKeywordConfigs();
    clearOriginTypeConfigs();
    initializeDefaults();
}

void ConfigurationManager::clearNamedConfigs() {
    namedConfigs.clear();
    activeConfigName = "";
}

void ConfigurationManager::clearKeywordConfigs() {
    keywordConfigs.clear();
}

void ConfigurationManager::clearOriginTypeConfigs() {
    originTypeConfigs.clear();
}

void ConfigurationManager::printConfiguration() const {
    std::cout << "=== CHTL Configuration Manager ===" << std::endl;
    std::cout << "Active Config: " << (activeConfigName.empty() ? "Global" : activeConfigName) << std::endl;
    
    std::cout << "\nNamed Configurations (" << namedConfigs.size() << "):" << std::endl;
    for (const auto& pair : namedConfigs) {
        std::cout << "  " << pair.first << std::endl;
    }
    
    std::cout << "\nKeyword Configurations (" << keywordConfigs.size() << "):" << std::endl;
    for (const auto& pair : keywordConfigs) {
        const auto& config = pair.second;
        std::cout << "  " << pair.first << " -> ";
        if (config.isGroupOption) {
            std::cout << "[";
            for (size_t i = 0; i < config.alternatives.size(); ++i) {
                if (i > 0) std::cout << ", ";
                std::cout << config.alternatives[i];
            }
            std::cout << "]";
        } else if (!config.alternatives.empty()) {
            std::cout << config.alternatives[0];
        }
        std::cout << std::endl;
    }
    
    std::cout << "\nOrigin Type Configurations (" << originTypeConfigs.size() << "):" << std::endl;
    for (const auto& pair : originTypeConfigs) {
        const auto& config = pair.second;
        std::cout << "  " << pair.first << " -> " << config.fullName 
                  << (config.isCustom ? " (custom)" : " (built-in)") << std::endl;
    }
}

size_t ConfigurationManager::getConfigCount() const {
    return namedConfigs.size() + (globalConfig ? 1 : 0);
}

StringVector ConfigurationManager::getConfigNames() const {
    StringVector names;
    for (const auto& pair : namedConfigs) {
        names.push_back(pair.first);
    }
    return names;
}

ConfigurationManager ConfigurationManager::createDefaultManager() {
    ConfigurationManager manager;
    return manager;
}

KeywordConfig ConfigurationManager::createDefaultKeywordConfig(const String& configKey) {
    // 为不同的配置键创建默认配置
    if (configKey == "CUSTOM_STYLE") {
        return KeywordConfig("@Style", {"@Style", "@style", "@CSS", "@Css", "@css"}, true, 5);
    }
    else if (configKey == "CUSTOM_ELEMENT") {
        return KeywordConfig("@Element", {"@Element"}, false, 1);
    }
    else if (configKey == "TEMPLATE_STYLE") {
        return KeywordConfig("@Style", {"@Style"}, false, 1);
    }
    // ... 可以添加更多默认配置
    
    return KeywordConfig(configKey, {configKey}, false, 1);
}

StringVector ConfigurationManager::getDefaultKeywords() {
    return {
        "CUSTOM_STYLE", "CUSTOM_ELEMENT", "CUSTOM_VAR",
        "TEMPLATE_STYLE", "TEMPLATE_ELEMENT", "TEMPLATE_VAR",
        "ORIGIN_HTML", "ORIGIN_STYLE", "ORIGIN_JAVASCRIPT",
        "IMPORT_HTML", "IMPORT_STYLE", "IMPORT_JAVASCRIPT",
        "IMPORT_CHTL", "IMPORT_CRMOD", "KEYWORD_INHERIT",
        "KEYWORD_DELETE", "KEYWORD_INSERT", "KEYWORD_FROM",
        "KEYWORD_AS", "KEYWORD_EXCEPT", "KEYWORD_USE"
    };
}

void ConfigurationManager::initializeDefaults() {
    setupDefaultKeywords();
    setupDefaultOriginTypes();
}

void ConfigurationManager::setupDefaultKeywords() {
    // 设置默认关键字配置 (按照CHTL语法文档)
    auto defaultKeywords = getDefaultKeywords();
    for (const auto& keyword : defaultKeywords) {
        auto config = createDefaultKeywordConfig(keyword);
        addKeywordConfig(keyword, config);
    }
}

void ConfigurationManager::setupDefaultOriginTypes() {
    // 设置默认原始类型配置
    addOriginTypeConfig("Html", OriginTypeConfig("Html", "@Html", "", false));
    addOriginTypeConfig("Style", OriginTypeConfig("Style", "@Style", "", false));
    addOriginTypeConfig("JavaScript", OriginTypeConfig("JavaScript", "@JavaScript", "", false));
    
    // 示例自定义类型
    addOriginTypeConfig("Vue", OriginTypeConfig("Vue", "@Vue", "", true));
    addOriginTypeConfig("Markdown", OriginTypeConfig("Markdown", "@Markdown", "", true));
}

bool ConfigurationManager::isValidConfigKey(const String& key) const {
    // 验证配置键的有效性
    return !key.empty() && key.find_first_of(" \t\r\n") == String::npos;
}

String ConfigurationManager::normalizeConfigKey(const String& key) const {
    String normalized = key;
    std::transform(normalized.begin(), normalized.end(), normalized.begin(), ::toupper);
    return normalized;
}

void ConfigurationManager::applyNameConfiguration(const StringUnorderedMap& nameConfig) {
    for (const auto& pair : nameConfig) {
        // 解析配置值
        StringVector alternatives = ConfigurationParser::parseGroupOptions(pair.second);
        setKeywordAlternatives(pair.first, alternatives);
    }
}

void ConfigurationManager::applyOriginTypeConfiguration(const StringUnorderedMap& originTypeConfig) {
    for (const auto& pair : originTypeConfig) {
        // 解析原始类型配置
        if (pair.first.find("ORIGINTYPE_") == 0) {
            String typeName = pair.first.substr(11); // 移除 "ORIGINTYPE_" 前缀
            OriginTypeConfig config(typeName, pair.second, "", true);
            addOriginTypeConfig(typeName, config);
        }
    }
}

// ========== ConfigurationParser 实现 ==========

std::unordered_map<String, KeywordConfig> ConfigurationParser::parseNameConfiguration(const StringUnorderedMap& nameConfig) {
    std::unordered_map<String, KeywordConfig> configs;
    
    for (const auto& pair : nameConfig) {
        KeywordConfig config;
        config.keyword = pair.first;
        config.alternatives = parseGroupOptions(pair.second);
        config.isGroupOption = config.alternatives.size() > 1;
        config.optionCount = config.alternatives.size();
        
        configs[pair.first] = config;
    }
    
    return configs;
}

std::unordered_map<String, OriginTypeConfig> ConfigurationParser::parseOriginTypeConfiguration(const StringUnorderedMap& originTypeConfig) {
    std::unordered_map<String, OriginTypeConfig> configs;
    
    for (const auto& pair : originTypeConfig) {
        if (pair.first.find("ORIGINTYPE_") == 0) {
            String typeName = pair.first.substr(11);
            OriginTypeConfig config(typeName, pair.second, "", true);
            configs[typeName] = config;
        }
    }
    
    return configs;
}

StringVector ConfigurationParser::parseGroupOptions(const String& optionString) {
    StringVector options;
    
    if (isGroupOptionSyntax(optionString)) {
        // 解析 [option1, option2, option3] 格式
        options = extractGroupOptionValues(optionString);
    } else {
        // 单一选项
        options.push_back(optionString);
    }
    
    return options;
}

bool ConfigurationParser::parseBoolean(const String& value) {
    String lowered = value;
    std::transform(lowered.begin(), lowered.end(), lowered.begin(), ::tolower);
    return lowered == "true" || lowered == "1" || lowered == "yes" || lowered == "on";
}

int ConfigurationParser::parseInt(const String& value) {
    try {
        return std::stoi(value);
    } catch (const std::exception&) {
        return 0;
    }
}

StringVector ConfigurationParser::parseStringList(const String& value) {
    StringVector result;
    std::istringstream iss(value);
    String token;
    
    while (std::getline(iss, token, ',')) {
        // 移除首尾空白
        token.erase(0, token.find_first_not_of(" \t\r\n"));
        token.erase(token.find_last_not_of(" \t\r\n") + 1);
        
        if (!token.empty()) {
            result.push_back(token);
        }
    }
    
    return result;
}

bool ConfigurationParser::validateConfigFormat(const String& key, const String& value) {
    // 基础格式验证
    return !key.empty() && !value.empty();
}

bool ConfigurationParser::validateKeywordConfig(const String& configKey, const String& configValue) {
    // 验证关键字配置的格式
    return validateConfigFormat(configKey, configValue);
}

bool ConfigurationParser::validateOriginTypeConfig(const String& configKey, const String& configValue) {
    // 验证原始类型配置的格式
    return validateConfigFormat(configKey, configValue) && 
           configKey.find("ORIGINTYPE_") == 0 &&
           configValue.front() == '@';
}

String ConfigurationParser::generateConfigError(const String& key, const String& value, const String& reason) {
    return "配置错误 [" + key + " = " + value + "]: " + reason;
}

bool ConfigurationParser::isGroupOptionSyntax(const String& value) {
    return value.front() == '[' && value.back() == ']';
}

String ConfigurationParser::extractGroupOptionName(const String& value) {
    // 从组选项中提取名称 (这里简化处理)
    if (isGroupOptionSyntax(value)) {
        size_t start = value.find('[') + 1;
        size_t end = value.find(',');
        if (end == String::npos) {
            end = value.find(']');
        }
        return value.substr(start, end - start);
    }
    return value;
}

StringVector ConfigurationParser::extractGroupOptionValues(const String& value) {
    if (!isGroupOptionSyntax(value)) {
        return {value};
    }
    
    // 提取 [...] 中的内容
    String content = value.substr(1, value.length() - 2);
    return parseStringList(content);
}

bool ConfigurationParser::isValidOriginTypeName(const String& typeName) {
    return !typeName.empty() && 
           typeName.find_first_of(" \t\r\n[]{}()") == String::npos;
}

// ========== 全局函数实现 ==========

void initializeGlobalConfiguration() {
    if (!g_configManager) {
        g_configManager = new ConfigurationManager();
    }
}

void shutdownGlobalConfiguration() {
    delete g_configManager;
    g_configManager = nullptr;
}

ConfigurationManager& getGlobalConfigManager() {
    if (!g_configManager) {
        initializeGlobalConfiguration();
    }
    return *g_configManager;
}

String getGlobalConfigValue(const String& key, const String& defaultValue) {
    return getGlobalConfigManager().getConfigValue(key, defaultValue);
}

bool getGlobalBoolConfig(const String& key, bool defaultValue) {
    return getGlobalConfigManager().getBoolConfigValue(key, defaultValue);
}

} // namespace CHTL