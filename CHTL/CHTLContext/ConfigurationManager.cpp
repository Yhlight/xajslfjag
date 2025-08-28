#include "ConfigurationManager.h"
#include <algorithm>
#include <sstream>
#include <stdexcept>

namespace CHTL {

ConfigurationManager::ConfigurationManager() {
    initializeDefaultConfiguration();
}

ConfigurationManager::~ConfigurationManager() {
}

void ConfigurationManager::initializeDefaultConfiguration() {
    // 创建默认配置组
    defaultConfiguration_ = std::make_shared<ConfigurationGroup>();
    createStandardConfigs(defaultConfiguration_);
    
    // 创建无名配置组
    auto unnamedGroup = std::make_shared<ConfigurationGroup>();
    createStandardConfigs(unnamedGroup);
    configurationGroups_[""] = unnamedGroup;
    
    // 设置默认激活组为无名配置组
    activeGroupName_ = "";
    unnamedGroup->isActive = true;
}

void ConfigurationManager::createStandardConfigs(std::shared_ptr<ConfigurationGroup> group) {
    // 基本配置项
    group->configs["INDEX_INITIAL_COUNT"] = ConfigValue(0);
    group->configs["DISABLE_NAME_GROUP"] = ConfigValue(false);
    group->configs["DISABLE_CUSTOM_ORIGIN_TYPE"] = ConfigValue(false);
    group->configs["DEBUG_MODE"] = ConfigValue(false);
    group->configs["DISABLE_DEFAULT_NAMESPACE"] = ConfigValue(false);
    
    // 选择器自动化配置
    group->configs["DISABLE_STYLE_AUTO_ADD_CLASS"] = ConfigValue(false);
    group->configs["DISABLE_STYLE_AUTO_ADD_ID"] = ConfigValue(false);
    group->configs["DISABLE_SCRIPT_AUTO_ADD_CLASS"] = ConfigValue(true);
    group->configs["DISABLE_SCRIPT_AUTO_ADD_ID"] = ConfigValue(true);
    
    // 关键字配置
    group->configs["CUSTOM_STYLE"] = ConfigValue(std::vector<std::string>{"@Style", "@style", "@CSS", "@Css", "@css"});
    group->configs["CUSTOM_ELEMENT"] = ConfigValue("@Element");
    group->configs["CUSTOM_VAR"] = ConfigValue("@Var");
    group->configs["TEMPLATE_STYLE"] = ConfigValue("@Style");
    group->configs["TEMPLATE_ELEMENT"] = ConfigValue("@Element");
    group->configs["TEMPLATE_VAR"] = ConfigValue("@Var");
    group->configs["ORIGIN_HTML"] = ConfigValue("@Html");
    group->configs["ORIGIN_STYLE"] = ConfigValue("@Style");
    group->configs["ORIGIN_JAVASCRIPT"] = ConfigValue("@JavaScript");
    group->configs["CONFIGURATION_CONFIG"] = ConfigValue("@Config");
    group->configs["IMPORT_HTML"] = ConfigValue("@Html");
    group->configs["IMPORT_STYLE"] = ConfigValue("@Style");
    group->configs["IMPORT_JAVASCRIPT"] = ConfigValue("@JavaScript");
    group->configs["IMPORT_CHTL"] = ConfigValue("@Chtl");
    group->configs["IMPORT_CRMOD"] = ConfigValue("@CJmod");
    group->configs["IMPORT_CONFIG"] = ConfigValue("@Config");
    group->configs["KEYWORD_INHERIT"] = ConfigValue("inherit");
    group->configs["KEYWORD_DELETE"] = ConfigValue("delete");
    group->configs["KEYWORD_INSERT"] = ConfigValue("insert");
    group->configs["KEYWORD_AFTER"] = ConfigValue("after");
    group->configs["KEYWORD_BEFORE"] = ConfigValue("before");
    group->configs["KEYWORD_REPLACE"] = ConfigValue("replace");
    group->configs["KEYWORD_ATTOP"] = ConfigValue("at top");
    group->configs["KEYWORD_ATBOTTOM"] = ConfigValue("at bottom");
    group->configs["KEYWORD_FROM"] = ConfigValue("from");
    group->configs["KEYWORD_AS"] = ConfigValue("as");
    group->configs["KEYWORD_EXCEPT"] = ConfigValue("except");
    group->configs["KEYWORD_TEXT"] = ConfigValue("text");
    group->configs["KEYWORD_STYLE"] = ConfigValue("style");
    group->configs["KEYWORD_SCRIPT"] = ConfigValue("script");
    group->configs["KEYWORD_CUSTOM"] = ConfigValue("[Custom]");
    group->configs["KEYWORD_TEMPLATE"] = ConfigValue("[Template]");
    group->configs["KEYWORD_ORIGIN"] = ConfigValue("[Origin]");
    group->configs["KEYWORD_IMPORT"] = ConfigValue("[Import]");
    group->configs["KEYWORD_NAMESPACE"] = ConfigValue("[Namespace]");
    group->configs["OPTION_COUNT"] = ConfigValue(3);
    
    // 原始嵌入类型
    group->originTypes["ORIGINTYPE_VUE"] = {"@Vue"};
}

bool ConfigurationManager::createConfigurationGroup(const std::string& name) {
    if (hasConfigurationGroup(name)) {
        return false; // 配置组已存在
    }
    
    auto newGroup = std::make_shared<ConfigurationGroup>(name);
    createStandardConfigs(newGroup);
    configurationGroups_[name] = newGroup;
    
    return true;
}

std::shared_ptr<ConfigurationGroup> ConfigurationManager::getConfigurationGroup(const std::string& name) const {
    auto it = configurationGroups_.find(name);
    if (it != configurationGroups_.end()) {
        return it->second;
    }
    return nullptr;
}

const std::map<std::string, std::shared_ptr<ConfigurationGroup>>& ConfigurationManager::getAllConfigurationGroups() const {
    return configurationGroups_;
}

std::shared_ptr<ConfigurationGroup> ConfigurationManager::getUnnamedConfigurationGroup() {
    return getConfigurationGroup("");
}

bool ConfigurationManager::setConfig(const std::string& groupName, const std::string& key, const ConfigValue& value) {
    auto group = getConfigurationGroup(groupName);
    if (!group) {
        return false;
    }
    
    if (!validateConfigValue(key, value)) {
        return false;
    }
    
    group->configs[key] = value;
    return true;
}

ConfigValue ConfigurationManager::getConfig(const std::string& groupName, const std::string& key) const {
    auto group = getConfigurationGroup(groupName);
    if (!group) {
        // 如果指定的配置组不存在，尝试从默认配置获取
        return defaultConfiguration_->configs.count(key) ? defaultConfiguration_->configs.at(key) : ConfigValue();
    }
    
    auto it = group->configs.find(key);
    if (it != group->configs.end()) {
        return it->second;
    }
    
    // 如果配置组中没有该配置项，从默认配置获取
    return defaultConfiguration_->configs.count(key) ? defaultConfiguration_->configs.at(key) : ConfigValue();
}

bool ConfigurationManager::setOriginType(const std::string& groupName, const std::string& typeName, const std::string& typeValue) {
    auto group = getConfigurationGroup(groupName);
    if (!group) {
        return false;
    }
    
    // 验证类型名称格式
    if (typeName.find("ORIGINTYPE_") != 0) {
        return false;
    }
    
    group->originTypes[typeName].push_back(typeValue);
    return true;
}

std::vector<std::string> ConfigurationManager::getOriginType(const std::string& groupName, const std::string& typeName) const {
    auto group = getConfigurationGroup(groupName);
    if (!group) {
        return {};
    }
    
    auto it = group->originTypes.find(typeName);
    if (it != group->originTypes.end()) {
        return it->second;
    }
    
    return {};
}

bool ConfigurationManager::activateConfigurationGroup(const std::string& name) {
    if (!hasConfigurationGroup(name)) {
        return false;
    }
    
    // 停用当前激活的配置组
    if (!activeGroupName_.empty()) {
        auto currentActive = getConfigurationGroup(activeGroupName_);
        if (currentActive) {
            currentActive->isActive = false;
        }
    }
    
    // 激活新的配置组
    auto newActive = getConfigurationGroup(name);
    if (newActive) {
        newActive->isActive = true;
        activeGroupName_ = name;
        return true;
    }
    
    return false;
}

std::string ConfigurationManager::getActiveConfigurationGroup() const {
    return activeGroupName_;
}

bool ConfigurationManager::hasConfig(const std::string& groupName, const std::string& key) const {
    auto group = getConfigurationGroup(groupName);
    if (!group) {
        return false;
    }
    
    return group->configs.count(key) > 0;
}

bool ConfigurationManager::hasConfigurationGroup(const std::string& name) const {
    return configurationGroups_.count(name) > 0;
}

bool ConfigurationManager::deleteConfigurationGroup(const std::string& name) {
    if (name.empty()) {
        return false; // 不能删除无名配置组
    }
    
    auto it = configurationGroups_.find(name);
    if (it == configurationGroups_.end()) {
        return false;
    }
    
    // 如果删除的是当前激活的配置组，需要重新激活无名配置组
    if (name == activeGroupName_) {
        activateConfigurationGroup("");
    }
    
    configurationGroups_.erase(it);
    return true;
}

bool ConfigurationManager::mergeConfigurationGroups(const std::string& targetName, const std::string& sourceName) {
    auto targetGroup = getConfigurationGroup(targetName);
    auto sourceGroup = getConfigurationGroup(sourceName);
    
    if (!targetGroup || !sourceGroup) {
        return false;
    }
    
    // 合并配置项
    for (const auto& config : sourceGroup->configs) {
        targetGroup->configs[config.first] = config.second;
    }
    
    // 合并原始嵌入类型
    for (const auto& originType : sourceGroup->originTypes) {
        auto& targetTypes = targetGroup->originTypes[originType.first];
        targetTypes.insert(targetTypes.end(), originType.second.begin(), originType.second.end());
    }
    
    return true;
}

std::shared_ptr<ConfigurationGroup> ConfigurationManager::getDefaultConfiguration() {
    return defaultConfiguration_;
}

void ConfigurationManager::resetToDefault() {
    // 重置所有配置组为标准配置
    for (auto& pair : configurationGroups_) {
        pair.second->configs.clear();
        pair.second->originTypes.clear();
        createStandardConfigs(pair.second);
    }
    
    // 重新激活无名配置组
    activateConfigurationGroup("");
}

bool ConfigurationManager::validateConfigurationGroup(const std::string& groupName) const {
    auto group = getConfigurationGroup(groupName);
    if (!group) {
        return false;
    }
    
    // 检查必要的配置项是否存在
    std::vector<std::string> requiredConfigs = {
        "INDEX_INITIAL_COUNT", "DISABLE_NAME_GROUP", "DISABLE_CUSTOM_ORIGIN_TYPE", "DEBUG_MODE"
    };
    
    for (const auto& required : requiredConfigs) {
        if (!hasConfig(groupName, required)) {
            return false;
        }
    }
    
    return true;
}

std::string ConfigurationManager::getConfigurationGroupInfo(const std::string& groupName) const {
    auto group = getConfigurationGroup(groupName);
    if (!group) {
        return "Configuration group not found: " + groupName;
    }
    
    std::ostringstream oss;
    oss << "Configuration Group: " << (group->name.empty() ? "(unnamed)" : group->name) << "\n";
    oss << "Active: " << (group->isActive ? "Yes" : "No") << "\n";
    oss << "Configs: " << group->configs.size() << "\n";
    oss << "Origin Types: " << group->originTypes.size() << "\n";
    
    return oss.str();
}

bool ConfigurationManager::validateConfigValue(const std::string& key, const ConfigValue& value) const {
    // 根据配置键验证值的类型
    if (key == "INDEX_INITIAL_COUNT") {
        return value.type == ConfigValueType::INTEGER;
    } else if (key == "DISABLE_NAME_GROUP" || key == "DISABLE_CUSTOM_ORIGIN_TYPE" || 
               key == "DEBUG_MODE" || key == "DISABLE_DEFAULT_NAMESPACE" ||
               key == "DISABLE_STYLE_AUTO_ADD_CLASS" || key == "DISABLE_STYLE_AUTO_ADD_ID" ||
               key == "DISABLE_SCRIPT_AUTO_ADD_CLASS" || key == "DISABLE_SCRIPT_AUTO_ADD_ID") {
        return value.type == ConfigValueType::BOOLEAN;
    } else if (key == "OPTION_COUNT") {
        return value.type == ConfigValueType::INTEGER && value.intValue > 0;
    } else if (key.find("KEYWORD_") == 0) {
        return value.type == ConfigValueType::STRING;
    } else if (key.find("CUSTOM_") == 0 || key.find("TEMPLATE_") == 0 || 
               key.find("ORIGIN_") == 0 || key.find("IMPORT_") == 0) {
        return value.type == ConfigValueType::STRING || value.type == ConfigValueType::STRING_ARRAY;
    }
    
    return true; // 其他配置项允许任意类型
}

ConfigValue ConfigurationManager::parseConfigValue(const std::string& value) const {
    // 尝试解析为布尔值
    if (value == "true" || value == "false") {
        return ConfigValue(value == "true");
    }
    
    // 尝试解析为整数
    try {
        int intValue = std::stoi(value);
        return ConfigValue(intValue);
    } catch (...) {
        // 不是整数，继续尝试其他类型
    }
    
    // 尝试解析为字符串数组（用逗号分隔）
    if (value.find(',') != std::string::npos) {
        std::vector<std::string> array;
        std::istringstream iss(value);
        std::string item;
        while (std::getline(iss, item, ',')) {
            // 去除首尾空格
            item.erase(0, item.find_first_not_of(" \t"));
            item.erase(item.find_last_not_of(" \t") + 1);
            if (!item.empty()) {
                array.push_back(item);
            }
        }
        if (!array.empty()) {
            return ConfigValue(array);
        }
    }
    
    // 默认为字符串
    return ConfigValue(value);
}

} // namespace CHTL