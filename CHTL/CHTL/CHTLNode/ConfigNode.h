#ifndef CHTL_CONFIG_NODE_H
#define CHTL_CONFIG_NODE_H

#include "BaseNode.h"
#include <unordered_map>
#include <vector>

namespace CHTL {

// 配置值类型
struct ConfigValue {
    std::string value;
    std::vector<std::string> arrayValues;  // 用于组选项
    bool isArray;
    
    ConfigValue() : isArray(false) {}
    ConfigValue(const std::string& v) : value(v), isArray(false) {}
    ConfigValue(const std::vector<std::string>& arr) : arrayValues(arr), isArray(true) {}
};

// 配置节点类
class ConfigNode : public BaseNode {
private:
    std::string name;  // 配置组名称（可选）
    std::unordered_map<std::string, ConfigValue> configurations;
    std::unordered_map<std::string, std::unordered_map<std::string, ConfigValue>> subGroups;  // 子配置组
    
public:
    ConfigNode(const std::string& n = "", int line = 0, int col = 0)
        : BaseNode(NodeType::CONFIG, line, col), name(n) {}
    
    // 名称访问
    const std::string& getName() const { return name; }
    void setName(const std::string& n) { name = n; }
    bool hasName() const { return !name.empty(); }
    
    // 配置项管理
    void setConfig(const std::string& key, const std::string& value);
    void setConfig(const std::string& key, const std::vector<std::string>& values);
    ConfigValue getConfig(const std::string& key) const;
    bool hasConfig(const std::string& key) const;
    const std::unordered_map<std::string, ConfigValue>& getConfigurations() const { return configurations; }
    
    // 子配置组管理（如[Name]、[OriginType]）
    void addSubGroup(const std::string& groupName);
    void setSubGroupConfig(const std::string& groupName, const std::string& key, const std::string& value);
    void setSubGroupConfig(const std::string& groupName, const std::string& key, const std::vector<std::string>& values);
    bool hasSubGroup(const std::string& groupName) const;
    const std::unordered_map<std::string, ConfigValue>& getSubGroup(const std::string& groupName) const;
    const std::unordered_map<std::string, std::unordered_map<std::string, ConfigValue>>& getSubGroups() const { return subGroups; }
    
    // 获取特定配置值
    bool getDebugMode() const;
    int getIndexInitialCount() const;
    bool getDisableNameGroup() const;
    bool getDisableCustomOriginType() const;
    bool getDisableDefaultNamespace() const;
    bool getDisableStyleAutoAddClass() const;
    bool getDisableStyleAutoAddId() const;
    bool getDisableScriptAutoAddClass() const;
    bool getDisableScriptAutoAddId() const;
    
    // 重写虚函数
    void accept(NodeVisitor* visitor) override;
    std::shared_ptr<BaseNode> clone() const override;
    std::string toString() const override;
};

// 扩展访问者接口
class ConfigNodeVisitor : public NodeVisitor {
public:
    virtual void visitConfigNode(ConfigNode* node) = 0;
};

} // namespace CHTL

#endif // CHTL_CONFIG_NODE_H