#include "ConfigNode.h"
#include <sstream>
#include <stdexcept>

namespace CHTL {

// ========== ConfigValue 实现 ==========

bool ConfigValue::getBool() const {
    if (type != ConfigValueType::BOOLEAN) {
        throw std::runtime_error("Config value is not a boolean");
    }
    return value == "true";
}

int ConfigValue::getInt() const {
    if (type != ConfigValueType::INTEGER) {
        throw std::runtime_error("Config value is not an integer");
    }
    return std::stoi(value);
}

const std::string& ConfigValue::getString() const {
    if (type != ConfigValueType::STRING && type != ConfigValueType::IDENTIFIER) {
        throw std::runtime_error("Config value is not a string");
    }
    return value;
}

const std::vector<std::string>& ConfigValue::getArray() const {
    if (type != ConfigValueType::ARRAY) {
        throw std::runtime_error("Config value is not an array");
    }
    return arrayValues;
}

// ========== ConfigNode 实现 ==========

ConfigNode::ConfigNode(ConfigType configType, const std::string& name)
    : BaseNode(CHTLNodeType::CONFIG_NODE, name), m_configType(configType), m_configName(name) {
}

ConfigType ConfigNode::getConfigType() const {
    return m_configType;
}

const std::string& ConfigNode::getConfigName() const {
    return m_configName;
}

void ConfigNode::setConfigName(const std::string& name) {
    m_configName = name;
}

void ConfigNode::setConfigValue(const std::string& key, const ConfigValue& value) {
    m_configurations[key] = value;
}

const ConfigValue* ConfigNode::getConfigValue(const std::string& key) const {
    auto it = m_configurations.find(key);
    return (it != m_configurations.end()) ? &it->second : nullptr;
}

bool ConfigNode::hasConfig(const std::string& key) const {
    return m_configurations.find(key) != m_configurations.end();
}

const std::unordered_map<std::string, ConfigValue>& ConfigNode::getAllConfigs() const {
    return m_configurations;
}

bool ConfigNode::isNamed() const {
    return !m_configName.empty();
}

std::string ConfigNode::toString() const {
    std::stringstream ss;
    
    switch (m_configType) {
        case ConfigType::CONFIGURATION:
            ss << "[Configuration]";
            if (isNamed()) {
                ss << " @Config " << m_configName;
            }
            break;
        case ConfigType::NAME:
            ss << "[Name]";
            break;
        case ConfigType::ORIGIN_TYPE:
            ss << "[OriginType]";
            break;
        case ConfigType::INFO:
            ss << "[Info]";
            break;
        case ConfigType::EXPORT:
            ss << "[Export]";
            break;
    }
    
    ss << " {\n";
    
    for (const auto& [key, value] : m_configurations) {
        ss << "    " << key << " = ";
        
        if (value.isArray()) {
            ss << "[";
            const auto& arr = value.getArray();
            for (size_t i = 0; i < arr.size(); ++i) {
                ss << arr[i];
                if (i < arr.size() - 1) ss << ", ";
            }
            ss << "]";
        } else {
            ss << value.getString();
        }
        
        ss << ";\n";
    }
    
    ss << "}";
    
    return ss.str();
}

NodePtr ConfigNode::clone() const {
    auto cloned = std::make_shared<ConfigNode>(m_configType, m_configName);
    cloned->m_configurations = m_configurations;
    return cloned;
}

bool ConfigNode::validate(ErrorReporter* errorReporter) const {
    (void)errorReporter; // 暂时不使用
    return true;
}

// ========== ConfigurationNode 实现 ==========

ConfigurationNode::ConfigurationNode(const std::string& name)
    : ConfigNode(ConfigType::CONFIGURATION, name) {
    // 设置默认配置值
    setConfigValue("INDEX_INITIAL_COUNT", ConfigValue(0));
    setConfigValue("DISABLE_NAME_GROUP", ConfigValue(false));
    setConfigValue("DISABLE_CUSTOM_ORIGIN_TYPE", ConfigValue(false));
    setConfigValue("DEBUG_MODE", ConfigValue(false));
    setConfigValue("DISABLE_DEFAULT_NAMESPACE", ConfigValue(false));
    setConfigValue("DISABLE_STYLE_AUTO_ADD_CLASS", ConfigValue(false));
    setConfigValue("DISABLE_STYLE_AUTO_ADD_ID", ConfigValue(false));
    setConfigValue("DISABLE_SCRIPT_AUTO_ADD_CLASS", ConfigValue(true));
    setConfigValue("DISABLE_SCRIPT_AUTO_ADD_ID", ConfigValue(true));
}

void ConfigurationNode::setIndexInitialCount(int count) {
    setConfigValue("INDEX_INITIAL_COUNT", ConfigValue(count));
}

int ConfigurationNode::getIndexInitialCount() const {
    const auto* value = getConfigValue("INDEX_INITIAL_COUNT");
    return value ? value->getInt() : 0;
}

void ConfigurationNode::setDisableNameGroup(bool disable) {
    setConfigValue("DISABLE_NAME_GROUP", ConfigValue(disable));
}

bool ConfigurationNode::isNameGroupDisabled() const {
    const auto* value = getConfigValue("DISABLE_NAME_GROUP");
    return value ? value->getBool() : false;
}

void ConfigurationNode::setDisableCustomOriginType(bool disable) {
    setConfigValue("DISABLE_CUSTOM_ORIGIN_TYPE", ConfigValue(disable));
}

bool ConfigurationNode::isCustomOriginTypeDisabled() const {
    const auto* value = getConfigValue("DISABLE_CUSTOM_ORIGIN_TYPE");
    return value ? value->getBool() : false;
}

void ConfigurationNode::setDebugMode(bool debug) {
    setConfigValue("DEBUG_MODE", ConfigValue(debug));
}

bool ConfigurationNode::isDebugMode() const {
    const auto* value = getConfigValue("DEBUG_MODE");
    return value ? value->getBool() : false;
}

void ConfigurationNode::setDisableDefaultNamespace(bool disable) {
    setConfigValue("DISABLE_DEFAULT_NAMESPACE", ConfigValue(disable));
}

bool ConfigurationNode::isDefaultNamespaceDisabled() const {
    const auto* value = getConfigValue("DISABLE_DEFAULT_NAMESPACE");
    return value ? value->getBool() : false;
}

void ConfigurationNode::setStyleAutoAddClass(bool enable) {
    setConfigValue("DISABLE_STYLE_AUTO_ADD_CLASS", ConfigValue(!enable));
}

void ConfigurationNode::setStyleAutoAddId(bool enable) {
    setConfigValue("DISABLE_STYLE_AUTO_ADD_ID", ConfigValue(!enable));
}

void ConfigurationNode::setScriptAutoAddClass(bool enable) {
    setConfigValue("DISABLE_SCRIPT_AUTO_ADD_CLASS", ConfigValue(!enable));
}

void ConfigurationNode::setScriptAutoAddId(bool enable) {
    setConfigValue("DISABLE_SCRIPT_AUTO_ADD_ID", ConfigValue(!enable));
}

bool ConfigurationNode::isStyleAutoAddClassEnabled() const {
    const auto* value = getConfigValue("DISABLE_STYLE_AUTO_ADD_CLASS");
    return value ? !value->getBool() : true;
}

bool ConfigurationNode::isStyleAutoAddIdEnabled() const {
    const auto* value = getConfigValue("DISABLE_STYLE_AUTO_ADD_ID");
    return value ? !value->getBool() : true;
}

bool ConfigurationNode::isScriptAutoAddClassEnabled() const {
    const auto* value = getConfigValue("DISABLE_SCRIPT_AUTO_ADD_CLASS");
    return value ? !value->getBool() : false;
}

bool ConfigurationNode::isScriptAutoAddIdEnabled() const {
    const auto* value = getConfigValue("DISABLE_SCRIPT_AUTO_ADD_ID");
    return value ? !value->getBool() : false;
}

// ========== NameConfigNode 实现 ==========

NameConfigNode::NameConfigNode() : ConfigNode(ConfigType::NAME), m_optionCount(3) {
    // 设置默认关键字别名
    setKeywordAlias("CUSTOM_STYLE", {"@Style", "@style", "@CSS", "@Css", "@css"});
    setKeywordAlias("CUSTOM_ELEMENT", {"@Element"});
    setKeywordAlias("CUSTOM_VAR", {"@Var"});
    setKeywordAlias("TEMPLATE_STYLE", {"@Style"});
    setKeywordAlias("TEMPLATE_ELEMENT", {"@Element"});
    setKeywordAlias("TEMPLATE_VAR", {"@Var"});
    setKeywordAlias("ORIGIN_HTML", {"@Html"});
    setKeywordAlias("ORIGIN_STYLE", {"@Style"});
    setKeywordAlias("ORIGIN_JAVASCRIPT", {"@JavaScript"});
    setKeywordAlias("KEYWORD_INHERIT", {"inherit"});
    setKeywordAlias("KEYWORD_DELETE", {"delete"});
    setKeywordAlias("KEYWORD_INSERT", {"insert"});
    setKeywordAlias("KEYWORD_AFTER", {"after"});
    setKeywordAlias("KEYWORD_BEFORE", {"before"});
    setKeywordAlias("KEYWORD_REPLACE", {"replace"});
    setKeywordAlias("KEYWORD_ATTOP", {"at top"});
    setKeywordAlias("KEYWORD_ATBOTTOM", {"at bottom"});
    setKeywordAlias("KEYWORD_FROM", {"from"});
    setKeywordAlias("KEYWORD_AS", {"as"});
    setKeywordAlias("KEYWORD_EXCEPT", {"except"});
    setKeywordAlias("KEYWORD_USE", {"use"});
    setKeywordAlias("KEYWORD_HTML5", {"html5"});
    setKeywordAlias("KEYWORD_TEXT", {"text"});
    setKeywordAlias("KEYWORD_STYLE", {"style"});
    setKeywordAlias("KEYWORD_SCRIPT", {"script"});
    setKeywordAlias("KEYWORD_CUSTOM", {"[Custom]"});
    setKeywordAlias("KEYWORD_TEMPLATE", {"[Template]"});
    setKeywordAlias("KEYWORD_ORIGIN", {"[Origin]"});
    setKeywordAlias("KEYWORD_IMPORT", {"[Import]"});
    setKeywordAlias("KEYWORD_NAMESPACE", {"[Namespace]"});
}

void NameConfigNode::setKeywordAlias(const std::string& keyword, const std::vector<std::string>& aliases) {
    m_keywordAliases[keyword] = aliases;
    
    // 更新别名到关键字的映射
    for (const auto& alias : aliases) {
        m_aliasToKeyword[alias] = keyword;
    }
    
    // 更新配置值
    setConfigValue(keyword, ConfigValue(aliases));
}

const std::vector<std::string>& NameConfigNode::getKeywordAliases(const std::string& keyword) const {
    static const std::vector<std::string> empty;
    auto it = m_keywordAliases.find(keyword);
    return (it != m_keywordAliases.end()) ? it->second : empty;
}

bool NameConfigNode::isKeywordAlias(const std::string& alias) const {
    return m_aliasToKeyword.find(alias) != m_aliasToKeyword.end();
}

std::string NameConfigNode::getOriginalKeyword(const std::string& alias) const {
    auto it = m_aliasToKeyword.find(alias);
    return (it != m_aliasToKeyword.end()) ? it->second : "";
}

void NameConfigNode::setOptionCount(int count) {
    m_optionCount = count;
    setConfigValue("OPTION_COUNT", ConfigValue(count));
}

int NameConfigNode::getOptionCount() const {
    return m_optionCount;
}

// ========== OriginTypeConfigNode 实现 ==========

OriginTypeConfigNode::OriginTypeConfigNode() : ConfigNode(ConfigType::ORIGIN_TYPE) {
}

void OriginTypeConfigNode::addCustomOriginType(const std::string& typeName, const std::string& typeIdentifier) {
    m_customOriginTypes[typeIdentifier] = typeName;
    setConfigValue("ORIGINTYPE_" + typeName, ConfigValue(typeIdentifier));
}

bool OriginTypeConfigNode::isCustomOriginType(const std::string& typeIdentifier) const {
    return m_customOriginTypes.find(typeIdentifier) != m_customOriginTypes.end();
}

std::string OriginTypeConfigNode::getCustomOriginTypeName(const std::string& typeIdentifier) const {
    auto it = m_customOriginTypes.find(typeIdentifier);
    return (it != m_customOriginTypes.end()) ? it->second : "";
}

const std::unordered_map<std::string, std::string>& OriginTypeConfigNode::getAllCustomOriginTypes() const {
    return m_customOriginTypes;
}

// ========== InfoConfigNode 实现 ==========

InfoConfigNode::InfoConfigNode() : ConfigNode(ConfigType::INFO) {
}

void InfoConfigNode::setModuleInfo(const std::string& name, const std::string& version, 
                                  const std::string& description, const std::string& author,
                                  const std::string& license) {
    setConfigValue("name", ConfigValue(name));
    setConfigValue("version", ConfigValue(version));
    setConfigValue("description", ConfigValue(description));
    setConfigValue("author", ConfigValue(author));
    setConfigValue("license", ConfigValue(license));
}

std::string InfoConfigNode::getModuleName() const {
    const auto* value = getConfigValue("name");
    return value ? value->getString() : "";
}

std::string InfoConfigNode::getModuleVersion() const {
    const auto* value = getConfigValue("version");
    return value ? value->getString() : "";
}

std::string InfoConfigNode::getModuleDescription() const {
    const auto* value = getConfigValue("description");
    return value ? value->getString() : "";
}

std::string InfoConfigNode::getModuleAuthor() const {
    const auto* value = getConfigValue("author");
    return value ? value->getString() : "";
}

std::string InfoConfigNode::getModuleLicense() const {
    const auto* value = getConfigValue("license");
    return value ? value->getString() : "";
}

// ========== ExportConfigNode 实现 ==========

ExportConfigNode::ExportConfigNode() : ConfigNode(ConfigType::EXPORT) {
}

void ExportConfigNode::addExport(const std::string& type, const std::vector<std::string>& items) {
    m_exports[type] = items;
    setConfigValue(type, ConfigValue(items));
}

const std::vector<std::string>& ExportConfigNode::getExports(const std::string& type) const {
    static const std::vector<std::string> empty;
    auto it = m_exports.find(type);
    return (it != m_exports.end()) ? it->second : empty;
}

const std::unordered_map<std::string, std::vector<std::string>>& ExportConfigNode::getAllExports() const {
    return m_exports;
}

} // namespace CHTL