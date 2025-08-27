#include "ConfigNode.h"
#include <sstream>

namespace CHTL {

void ConfigNode::setConfig(const std::string& key, const std::string& value) {
    configurations[key] = ConfigValue(value);
}

void ConfigNode::setConfig(const std::string& key, const std::vector<std::string>& values) {
    configurations[key] = ConfigValue(values);
}

ConfigValue ConfigNode::getConfig(const std::string& key) const {
    auto it = configurations.find(key);
    return (it != configurations.end()) ? it->second : ConfigValue();
}

bool ConfigNode::hasConfig(const std::string& key) const {
    return configurations.find(key) != configurations.end();
}

void ConfigNode::addSubGroup(const std::string& groupName) {
    if (subGroups.find(groupName) == subGroups.end()) {
        subGroups[groupName] = std::unordered_map<std::string, ConfigValue>();
    }
}

void ConfigNode::setSubGroupConfig(const std::string& groupName, const std::string& key, const std::string& value) {
    addSubGroup(groupName);
    subGroups[groupName][key] = ConfigValue(value);
}

void ConfigNode::setSubGroupConfig(const std::string& groupName, const std::string& key, const std::vector<std::string>& values) {
    addSubGroup(groupName);
    subGroups[groupName][key] = ConfigValue(values);
}

bool ConfigNode::hasSubGroup(const std::string& groupName) const {
    return subGroups.find(groupName) != subGroups.end();
}

const std::unordered_map<std::string, ConfigValue>& ConfigNode::getSubGroup(const std::string& groupName) const {
    static const std::unordered_map<std::string, ConfigValue> empty;
    auto it = subGroups.find(groupName);
    return (it != subGroups.end()) ? it->second : empty;
}

bool ConfigNode::getDebugMode() const {
    auto config = getConfig("DEBUG_MODE");
    return !config.isArray && config.value == "true";
}

int ConfigNode::getIndexInitialCount() const {
    auto config = getConfig("INDEX_INITIAL_COUNT");
    if (!config.isArray && !config.value.empty()) {
        try {
            return std::stoi(config.value);
        } catch (...) {
            return 0;
        }
    }
    return 0;
}

bool ConfigNode::getDisableNameGroup() const {
    auto config = getConfig("DISABLE_NAME_GROUP");
    return !config.isArray && config.value == "true";
}

bool ConfigNode::getDisableCustomOriginType() const {
    auto config = getConfig("DISABLE_CUSTOM_ORIGIN_TYPE");
    return !config.isArray && config.value == "true";
}

bool ConfigNode::getDisableDefaultNamespace() const {
    auto config = getConfig("DISABLE_DEFAULT_NAMESPACE");
    return !config.isArray && config.value == "true";
}

bool ConfigNode::getDisableStyleAutoAddClass() const {
    auto config = getConfig("DISABLE_STYLE_AUTO_ADD_CLASS");
    return !config.isArray && config.value == "true";
}

bool ConfigNode::getDisableStyleAutoAddId() const {
    auto config = getConfig("DISABLE_STYLE_AUTO_ADD_ID");
    return !config.isArray && config.value == "true";
}

bool ConfigNode::getDisableScriptAutoAddClass() const {
    auto config = getConfig("DISABLE_SCRIPT_AUTO_ADD_CLASS");
    return !config.isArray && config.value == "true";
}

bool ConfigNode::getDisableScriptAutoAddId() const {
    auto config = getConfig("DISABLE_SCRIPT_AUTO_ADD_ID");
    return !config.isArray && config.value == "true";
}

void ConfigNode::accept(NodeVisitor* visitor) {
    if (auto* configVisitor = dynamic_cast<ConfigNodeVisitor*>(visitor)) {
        configVisitor->visitConfigNode(this);
    }
}

std::shared_ptr<BaseNode> ConfigNode::clone() const {
    auto cloned = std::make_shared<ConfigNode>(name, line, column);
    
    // 复制配置
    cloned->configurations = configurations;
    cloned->subGroups = subGroups;
    
    // 递归克隆子节点
    for (const auto& child : children) {
        cloned->addChild(child->clone());
    }
    
    return cloned;
}

std::string ConfigNode::toString() const {
    std::stringstream ss;
    
    ss << "[Configuration]";
    if (!name.empty()) {
        ss << " @Config " << name;
    }
    ss << " {\n";
    
    // 输出主配置
    for (const auto& [key, config] : configurations) {
        ss << "    " << key << " = ";
        if (config.isArray) {
            ss << "[";
            for (size_t i = 0; i < config.arrayValues.size(); ++i) {
                if (i > 0) ss << ", ";
                ss << config.arrayValues[i];
            }
            ss << "]";
        } else {
            ss << config.value;
        }
        ss << ";\n";
    }
    
    // 输出子配置组
    for (const auto& [groupName, group] : subGroups) {
        ss << "\n    [" << groupName << "] {\n";
        for (const auto& [key, config] : group) {
            ss << "        " << key << " = ";
            if (config.isArray) {
                ss << "[";
                for (size_t i = 0; i < config.arrayValues.size(); ++i) {
                    if (i > 0) ss << ", ";
                    ss << config.arrayValues[i];
                }
                ss << "]";
            } else {
                ss << config.value;
            }
            ss << ";\n";
        }
        ss << "    }\n";
    }
    
    ss << "}";
    
    return ss.str();
}

} // namespace CHTL