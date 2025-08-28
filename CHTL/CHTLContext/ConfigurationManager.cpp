#include "ConfigurationManager.h"
#include <algorithm>
#include <sstream>
#include <stdexcept>

namespace CHTL {

ConfigurationManager::ConfigurationManager() {
    initializeDefaultConfiguration();
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
    unnamedGroup->isDefault = true;
}

void ConfigurationManager::createStandardConfigs(std::shared_ptr<ConfigurationGroup> group) {
    // 基本配置项
    group->configItems["INDEX_INITIAL_COUNT"] = ConfigValue(ConfigItemType::INTEGER, 0);
    group->configItems["DISABLE_NAME_GROUP"] = ConfigValue(ConfigItemType::BOOLEAN, false);
    group->configItems["DISABLE_CUSTOM_ORIGIN_TYPE"] = ConfigValue(ConfigItemType::BOOLEAN, false);
    group->configItems["DEBUG_MODE"] = ConfigValue(ConfigItemType::BOOLEAN, false);
    group->configItems["DISABLE_DEFAULT_NAMESPACE"] = ConfigValue(ConfigItemType::BOOLEAN, false);
    
    // 选择器自动化配置
    group->configItems["DISABLE_STYLE_AUTO_ADD_CLASS"] = ConfigValue(ConfigItemType::BOOLEAN, false);
    group->configItems["DISABLE_STYLE_AUTO_ADD_ID"] = ConfigValue(ConfigItemType::BOOLEAN, false);
    group->configItems["DISABLE_SCRIPT_AUTO_ADD_CLASS"] = ConfigValue(ConfigItemType::BOOLEAN, true);
    group->configItems["DISABLE_SCRIPT_AUTO_ADD_ID"] = ConfigValue(ConfigItemType::BOOLEAN, true);
    
    // 关键字配置
    group->configItems["CUSTOM_STYLE"] = ConfigValue(ConfigItemType::STRING_ARRAY, 
        std::vector<std::string>{"@Style", "@style", "@CSS", "@Css", "@css"});
    group->configItems["CUSTOM_ELEMENT"] = ConfigValue(ConfigItemType::STRING, "@Element");
    group->configItems["CUSTOM_VAR"] = ConfigValue(ConfigItemType::STRING, "@Var");
    group->configItems["TEMPLATE_STYLE"] = ConfigValue(ConfigItemType::STRING, "@Style");
    group->configItems["TEMPLATE_ELEMENT"] = ConfigValue(ConfigItemType::STRING, "@Element");
    group->configItems["TEMPLATE_VAR"] = ConfigValue(ConfigItemType::STRING, "@Var");
    group->configItems["ORIGIN_HTML"] = ConfigValue(ConfigItemType::STRING, "@Html");
    group->configItems["ORIGIN_STYLE"] = ConfigValue(ConfigItemType::STRING, "@Style");
    group->configItems["ORIGIN_JAVASCRIPT"] = ConfigValue(ConfigItemType::STRING, "@JavaScript");
    group->configItems["CONFIGURATION_CONFIG"] = ConfigValue(ConfigItemType::STRING, "@Config");
    group->configItems["IMPORT_HTML"] = ConfigValue(ConfigItemType::STRING, "@Html");
    group->configItems["IMPORT_STYLE"] = ConfigValue(ConfigItemType::STRING, "@Style");
    group->configItems["IMPORT_JAVASCRIPT"] = ConfigValue(ConfigItemType::STRING, "@JavaScript");
    group->configItems["IMPORT_CHTL"] = ConfigValue(ConfigItemType::STRING, "@Chtl");
    group->configItems["IMPORT_CJMOD"] = ConfigValue(ConfigItemType::STRING, "@CJmod");
    group->configItems["IMPORT_CONFIG"] = ConfigValue(ConfigItemType::STRING, "@Config");
    group->configItems["KEYWORD_INHERIT"] = ConfigValue(ConfigItemType::STRING, "inherit");
    group->configItems["KEYWORD_DELETE"] = ConfigValue(ConfigItemType::STRING, "delete");
    group->configItems["KEYWORD_INSERT"] = ConfigValue(ConfigItemType::STRING, "insert");
    group->configItems["KEYWORD_AFTER"] = ConfigValue(ConfigItemType::STRING, "after");
    group->configItems["KEYWORD_BEFORE"] = ConfigValue(ConfigItemType::STRING, "before");
    group->configItems["KEYWORD_REPLACE"] = ConfigValue(ConfigItemType::STRING, "replace");
    group->configItems["KEYWORD_ATTOP"] = ConfigValue(ConfigItemType::STRING, "at top");
    group->configItems["KEYWORD_ATBOTTOM"] = ConfigValue(ConfigItemType::STRING, "at bottom");
    group->configItems["KEYWORD_FROM"] = ConfigValue(ConfigItemType::STRING, "from");
    group->configItems["KEYWORD_AS"] = ConfigValue(ConfigItemType::STRING, "as");
    group->configItems["KEYWORD_EXCEPT"] = ConfigValue(ConfigItemType::STRING, "except");
    group->configItems["KEYWORD_TEXT"] = ConfigValue(ConfigItemType::STRING, "text");
    group->configItems["KEYWORD_STYLE"] = ConfigValue(ConfigItemType::STRING, "style");
    group->configItems["KEYWORD_SCRIPT"] = ConfigValue(ConfigItemType::STRING, "script");
    group->configItems["KEYWORD_CUSTOM"] = ConfigValue(ConfigItemType::STRING, "[Custom]");
    group->configItems["KEYWORD_TEMPLATE"] = ConfigValue(ConfigItemType::STRING, "[Template]");
    group->configItems["KEYWORD_ORIGIN"] = ConfigValue(ConfigItemType::STRING, "[Origin]");
    group->configItems["KEYWORD_IMPORT"] = ConfigValue(ConfigItemType::STRING, "[Import]");
    group->configItems["KEYWORD_NAMESPACE"] = ConfigValue(ConfigItemType::STRING, "[Namespace]");
    group->configItems["OPTION_COUNT"] = ConfigValue(ConfigItemType::INTEGER, 3);
    
    // 原始嵌入类型
    group->originTypes["ORIGINTYPE_VUE"] = "@Vue";
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

bool ConfigurationManager::setConfig(const std::string& groupName, const std::string& key, const ConfigValue& value) {
    auto group = getConfigurationGroup(groupName);
    if (!group) {
        return false;
    }
    
    if (!validateConfigValue(key, value)) {
        return false;
    }
    
    group->configItems[key] = value;
    return true;
}

ConfigValue ConfigurationManager::getConfig(const std::string& groupName, const std::string& key) const {
    auto group = getConfigurationGroup(groupName);
    if (!group) {
        // 如果指定的配置组不存在，尝试从默认配置获取
        return defaultConfiguration_->configItems.count(key) ? defaultConfiguration_->configItems.at(key) : ConfigValue();
    }
    
    auto it = group->configItems.find(key);
    if (it != group->configItems.end()) {
        return it->second;
    }
    
    // 如果配置组中没有该配置项，从默认配置获取
    return defaultConfiguration_->configItems.count(key) ? defaultConfiguration_->configItems.at(key) : ConfigValue();
}

bool ConfigurationManager::hasConfig(const std::string& groupName, const std::string& key) const {
    auto group = getConfigurationGroup(groupName);
    if (!group) {
        return false;
    }
    
    return group->configItems.count(key) > 0;
}

bool ConfigurationManager::hasConfigurationGroup(const std::string& name) const {
    return configurationGroups_.count(name) > 0;
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
    
    group->originTypes[typeName] = typeValue;
    return true;
}

std::vector<std::string> ConfigurationManager::getOriginType(const std::string& groupName, const std::string& typeName) const {
    auto group = getConfigurationGroup(groupName);
    if (!group) {
        return {};
    }
    
    auto it = group->originTypes.find(typeName);
    if (it != group->originTypes.end()) {
        return {it->second};
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
            currentActive->isDefault = false;
        }
    }
    
    // 激活新的配置组
    auto newActive = getConfigurationGroup(name);
    if (newActive) {
        newActive->isDefault = true;
        activeGroupName_ = name;
        return true;
    }
    
    return false;
}

std::string ConfigurationManager::getActiveConfigurationGroup() const {
    return activeGroupName_;
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
    for (const auto& config : sourceGroup->configItems) {
        targetGroup->configItems[config.first] = config.second;
    }
    
    // 合并原始嵌入类型
    for (const auto& originType : sourceGroup->originTypes) {
        targetGroup->originTypes[originType.first] = originType.second;
    }
    
    return true;
}

std::shared_ptr<ConfigurationGroup> ConfigurationManager::getDefaultConfiguration() const {
    return defaultConfiguration_;
}

void ConfigurationManager::resetToDefault() {
    // 重置所有配置组为标准配置
    for (auto& pair : configurationGroups_) {
        pair.second->configItems.clear();
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
    oss << "Active: " << (group->isDefault ? "Yes" : "No") << "\n";
    oss << "Configs: " << group->configItems.size() << "\n";
    oss << "Origin Types: " << group->originTypes.size() << "\n";
    
    return oss.str();
}

bool ConfigurationManager::validateConfigValue(const std::string& key, const ConfigValue& value) const {
    // 根据配置键验证值的类型
    if (key == "INDEX_INITIAL_COUNT") {
        return value.type == ConfigItemType::INTEGER;
    } else if (key == "DISABLE_NAME_GROUP" || key == "DISABLE_CUSTOM_ORIGIN_TYPE" || 
               key == "DEBUG_MODE" || key == "DISABLE_DEFAULT_NAMESPACE" ||
               key == "DISABLE_STYLE_AUTO_ADD_CLASS" || key == "DISABLE_STYLE_AUTO_ADD_ID" ||
               key == "DISABLE_SCRIPT_AUTO_ADD_CLASS" || key == "DISABLE_SCRIPT_AUTO_ADD_ID") {
        return value.type == ConfigItemType::BOOLEAN;
    } else if (key == "OPTION_COUNT") {
        return value.type == ConfigItemType::INTEGER && value.get<int>() > 0;
    } else if (key.find("KEYWORD_") == 0) {
        return value.type == ConfigItemType::STRING;
    } else if (key.find("CUSTOM_") == 0 || key.find("TEMPLATE_") == 0 || 
               key.find("ORIGIN_") == 0 || key.find("IMPORT_") == 0) {
        return value.type == ConfigItemType::STRING || value.type == ConfigItemType::STRING_ARRAY;
    }
    
    return true; // 其他配置项允许任意类型
}

ConfigValue ConfigurationManager::parseConfigValue(const std::string& value) const {
    // 尝试解析为布尔值
    if (value == "true" || value == "false") {
        return ConfigValue(ConfigItemType::BOOLEAN, value == "true");
    }
    
    // 尝试解析为整数
    try {
        int intValue = std::stoi(value);
        return ConfigValue(ConfigItemType::INTEGER, intValue);
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
            return ConfigValue(ConfigItemType::STRING_ARRAY, array);
        }
    }
    
    // 默认为字符串
    return ConfigValue(ConfigItemType::STRING, value);
}

} // namespace CHTL