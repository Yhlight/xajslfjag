#pragma once

#include "BaseNode.h"
#include <unordered_map>
#include <vector>

namespace CHTL {

// 配置项
struct ConfigItem {
    std::string name;
    std::string value;
    bool isArray;
    std::vector<std::string> arrayValues;
};

// 配置节点
class ConfigNode : public BaseNode {
public:
    ConfigNode(const std::string& name = "");
    
    std::string GetName() const override { return configName; }
    bool IsDefaultConfig() const { return configName.empty(); }
    
    std::string Generate() const override;
    NodePtr Clone() const override;
    void Accept(NodeVisitor* visitor) override;
    
    // 配置项管理
    void SetConfigItem(const std::string& name, const std::string& value);
    void SetConfigItem(const std::string& name, const std::vector<std::string>& values);
    std::string GetConfigValue(const std::string& name) const;
    std::vector<std::string> GetConfigArray(const std::string& name) const;
    bool HasConfig(const std::string& name) const;
    
    // Name配置块
    void AddNameConfig(const std::string& key, const std::string& value);
    void AddNameConfig(const std::string& key, const std::vector<std::string>& values);
    std::string GetNameConfig(const std::string& key) const;
    
    // OriginType配置块
    void AddOriginType(const std::string& name, const std::string& type);
    std::unordered_map<std::string, std::string> GetOriginTypes() const { return originTypes; }
    
    // 获取所有配置
    std::unordered_map<std::string, ConfigItem> GetAllConfigs() const { return configs; }
    std::unordered_map<std::string, ConfigItem> GetAllNameConfigs() const { return nameConfigs; }
    
    // 合并配置
    void MergeFrom(const ConfigNode& other);
    
    // 验证配置
    bool ValidateConfig() const;
    std::vector<std::string> GetValidationErrors() const;
    
private:
    std::string configName;  // 配置名称（空表示默认配置）
    
    // 主配置项
    std::unordered_map<std::string, ConfigItem> configs;
    
    // Name配置块
    std::unordered_map<std::string, ConfigItem> nameConfigs;
    
    // OriginType配置块
    std::unordered_map<std::string, std::string> originTypes;
    
    // 验证特定配置项
    bool ValidateConfigItem(const std::string& name, const ConfigItem& item) const;
};

} // namespace CHTL