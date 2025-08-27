#ifndef CHTL_CUSTOM_NODE_H
#define CHTL_CUSTOM_NODE_H

#include "BaseNode.h"
#include <unordered_map>
#include <unordered_set>

namespace CHTL {

// 自定义类型枚举（与模板类型相同）
enum class CustomType {
    STYLE,      // @Style 自定义样式组
    ELEMENT,    // @Element 自定义元素
    VAR         // @Var 自定义变量组
};

// 自定义节点类
class CustomNode : public BaseNode {
private:
    std::string name;
    CustomType customType;
    std::unordered_map<std::string, std::string> properties;  // 用于样式组和变量组
    std::unordered_set<std::string> noValueProperties;        // 无值属性（自定义样式组特有）
    std::vector<std::string> inheritFrom;                     // 继承的模板/自定义列表
    std::vector<std::string> deletedProperties;               // 删除的属性列表
    std::vector<std::string> deletedInheritances;            // 删除的继承列表
    
public:
    CustomNode(const std::string& n, CustomType type, int line = 0, int col = 0)
        : BaseNode(NodeType::CUSTOM, line, col), name(n), customType(type) {}
    
    // 名称访问
    const std::string& getName() const { return name; }
    void setName(const std::string& n) { name = n; }
    
    // 自定义类型访问
    CustomType getCustomType() const { return customType; }
    void setCustomType(CustomType type) { customType = type; }
    
    // 属性管理
    void setProperty(const std::string& key, const std::string& value);
    std::string getProperty(const std::string& key) const;
    bool hasProperty(const std::string& key) const;
    void removeProperty(const std::string& key);
    const std::unordered_map<std::string, std::string>& getProperties() const { return properties; }
    
    // 无值属性管理（自定义样式组特有）
    void addNoValueProperty(const std::string& prop);
    bool hasNoValueProperty(const std::string& prop) const;
    const std::unordered_set<std::string>& getNoValueProperties() const { return noValueProperties; }
    
    // 继承管理
    void addInheritance(const std::string& name);
    const std::vector<std::string>& getInheritances() const { return inheritFrom; }
    bool hasInheritance() const { return !inheritFrom.empty(); }
    
    // 删除操作管理
    void addDeletedProperty(const std::string& prop);
    void addDeletedInheritance(const std::string& name);
    const std::vector<std::string>& getDeletedProperties() const { return deletedProperties; }
    const std::vector<std::string>& getDeletedInheritances() const { return deletedInheritances; }
    
    // 重写虚函数
    void accept(NodeVisitor* visitor) override;
    std::shared_ptr<BaseNode> clone() const override;
    std::string toString() const override;
};

// 扩展访问者接口
class CustomNodeVisitor : public NodeVisitor {
public:
    virtual void visitCustomNode(CustomNode* node) = 0;
};

} // namespace CHTL

#endif // CHTL_CUSTOM_NODE_H