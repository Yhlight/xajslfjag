#include "ConfigNode.h"
#include <sstream>
#include <algorithm>

namespace CHTL {

ConfigNode::ConfigNode(const std::string& name)
    : BaseNode(NodeType::CONFIG), configName(name) {
}

std::string ConfigNode::Generate() const {
    // 配置节点不生成输出
    return "";
}

NodePtr ConfigNode::Clone() const {
    auto clone = std::make_shared<ConfigNode>(configName);
    clone->SetPosition(line, column);
    clone->configs = configs;
    clone->nameConfigs = nameConfigs;
    clone->originTypes = originTypes;
    return clone;
}

void ConfigNode::Accept(NodeVisitor* visitor) {
    visitor->VisitConfigNode(this);
}

void ConfigNode::SetConfigItem(const std::string& name, const std::string& value) {
    ConfigItem item;
    item.name = name;
    item.value = value;
    item.isArray = false;
    configs[name] = item;
}

void ConfigNode::SetConfigItem(const std::string& name, const std::vector<std::string>& values) {
    ConfigItem item;
    item.name = name;
    item.isArray = true;
    item.arrayValues = values;
    configs[name] = item;
}

std::string ConfigNode::GetConfigValue(const std::string& name) const {
    auto it = configs.find(name);
    if (it != configs.end() && !it->second.isArray) {
        return it->second.value;
    }
    return "";
}

std::vector<std::string> ConfigNode::GetConfigArray(const std::string& name) const {
    auto it = configs.find(name);
    if (it != configs.end() && it->second.isArray) {
        return it->second.arrayValues;
    }
    return {};
}

bool ConfigNode::HasConfig(const std::string& name) const {
    return configs.find(name) != configs.end();
}

void ConfigNode::AddNameConfig(const std::string& key, const std::string& value) {
    ConfigItem item;
    item.name = key;
    item.value = value;
    item.isArray = false;
    nameConfigs[key] = item;
}

void ConfigNode::AddNameConfig(const std::string& key, const std::vector<std::string>& values) {
    ConfigItem item;
    item.name = key;
    item.isArray = true;
    item.arrayValues = values;
    nameConfigs[key] = item;
}

std::string ConfigNode::GetNameConfig(const std::string& key) const {
    auto it = nameConfigs.find(key);
    if (it != nameConfigs.end()) {
        if (it->second.isArray && !it->second.arrayValues.empty()) {
            return it->second.arrayValues[0];  // 返回第一个值作为默认
        }
        return it->second.value;
    }
    return "";
}

void ConfigNode::AddOriginType(const std::string& name, const std::string& type) {
    originTypes[name] = type;
}

void ConfigNode::MergeFrom(const ConfigNode& other) {
    // 合并主配置
    for (const auto& [key, item] : other.configs) {
        configs[key] = item;
    }
    
    // 合并Name配置
    for (const auto& [key, item] : other.nameConfigs) {
        nameConfigs[key] = item;
    }
    
    // 合并OriginType
    for (const auto& [key, value] : other.originTypes) {
        originTypes[key] = value;
    }
}

bool ConfigNode::ValidateConfig() const {
    // 验证所有配置项
    for (const auto& [name, item] : configs) {
        if (!ValidateConfigItem(name, item)) {
            return false;
        }
    }
    
    return true;
}

std::vector<std::string> ConfigNode::GetValidationErrors() const {
    std::vector<std::string> errors;
    
    // 检查必要的配置项
    if (!HasConfig("INDEX_INITIAL_COUNT")) {
        errors.push_back("缺少必要的配置项: INDEX_INITIAL_COUNT");
    }
    
    // 检查配置值的有效性
    std::string indexCount = GetConfigValue("INDEX_INITIAL_COUNT");
    if (!indexCount.empty()) {
        try {
            int count = std::stoi(indexCount);
            if (count < 0) {
                errors.push_back("INDEX_INITIAL_COUNT 必须是非负整数");
            }
        } catch (...) {
            errors.push_back("INDEX_INITIAL_COUNT 必须是有效的整数");
        }
    }
    
    // 检查布尔值配置
    std::vector<std::string> boolConfigs = {
        "DISABLE_NAME_GROUP",
        "DISABLE_CUSTOM_ORIGIN_TYPE",
        "DEBUG_MODE",
        "DISABLE_STYLE_AUTO_ADD_CLASS",
        "DISABLE_STYLE_AUTO_ADD_ID",
        "DISABLE_SCRIPT_AUTO_ADD_CLASS",
        "DISABLE_SCRIPT_AUTO_ADD_ID",
        "DISABLE_DEFAULT_NAMESPACE"
    };
    
    for (const auto& configName : boolConfigs) {
        std::string value = GetConfigValue(configName);
        if (!value.empty() && value != "true" && value != "false") {
            errors.push_back(configName + " 必须是 true 或 false");
        }
    }
    
    return errors;
}

bool ConfigNode::ValidateConfigItem(const std::string& name, const ConfigItem& item) const {
    // 验证特定配置项的有效性
    // TODO: 实现具体的验证逻辑
    return true;
}

} // namespace CHTL