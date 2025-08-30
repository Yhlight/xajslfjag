#pragma once

#include "../CHTLNode/BaseNode.h"

namespace CHTL {

// 配置节点
class ConfigNode : public BaseNode {
public:
    String configName;
    StringUnorderedMap configValues;
    StringUnorderedMap nameConfig;
    StringUnorderedMap originTypeConfig;
    bool isNamed;
    
    explicit ConfigNode(const String& name = "", const Position& pos = Position())
        : BaseNode(NodeType::CONFIGURATION, name, pos), configName(name), isNamed(!name.empty()) {}
    
    // 配置值管理
    void setConfigValue(const String& key, const String& value) { configValues[key] = value; }
    String getConfigValue(const String& key, const String& defaultValue = "") const {
        auto it = configValues.find(key);
        return (it != configValues.end()) ? it->second : defaultValue;
    }
    
    // Name配置
    void setNameConfig(const String& key, const String& value) { nameConfig[key] = value; }
    String getNameConfig(const String& key) const {
        auto it = nameConfig.find(key);
        return (it != nameConfig.end()) ? it->second : "";
    }
    
    // OriginType配置  
    void setOriginTypeConfig(const String& key, const String& value) { originTypeConfig[key] = value; }
    String getOriginTypeConfig(const String& key) const {
        auto it = originTypeConfig.find(key);
        return (it != originTypeConfig.end()) ? it->second : "";
    }
    
    // 重写方法
    String toString() const override {
        return "[Configuration]" + (isNamed ? " @Config " + configName : "");
    }
    
    std::unique_ptr<BaseNode> clone() const override {
        auto cloned = std::make_unique<ConfigNode>(configName, position);
        cloned->configValues = configValues;
        cloned->nameConfig = nameConfig;
        cloned->originTypeConfig = originTypeConfig;
        return std::unique_ptr<BaseNode>(cloned.release());
    }
};

// Use语句节点
class UseNode : public BaseNode {
public:
    String useType;  // html5, @Config等
    String targetName;
    
    UseNode(const String& type, const String& target = "", const Position& pos = Position())
        : BaseNode(NodeType::USE, type + (target.empty() ? "" : " " + target), pos), useType(type), targetName(target) {}
    
    String toString() const override {
        return "use " + useType + (targetName.empty() ? "" : " " + targetName);
    }
    
    std::unique_ptr<BaseNode> clone() const override {
        return std::make_unique<UseNode>(useType, targetName, position);
    }
};

} // namespace CHTL