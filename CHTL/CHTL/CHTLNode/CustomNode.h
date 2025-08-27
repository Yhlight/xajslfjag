#pragma once

#include "BaseNode.h"
#include <unordered_map>
#include <set>

namespace CHTL {

// 自定义类型（与模板类型相同）
enum class CustomType {
    STYLE,      // @Style 自定义样式组
    ELEMENT,    // @Element 自定义元素
    VAR         // @Var 自定义变量组
};

// 自定义节点 - 支持特例化操作
class CustomNode : public BaseNode {
public:
    CustomNode(CustomType type, const std::string& name);
    
    std::string GetName() const override { return customName; }
    CustomType GetCustomType() const { return customType; }
    
    std::string Generate() const override;
    NodePtr Clone() const override;
    void Accept(NodeVisitor* visitor) override;
    
    // 内容管理
    void SetContent(NodePtr content) { contentNode = content; }
    NodePtr GetContent() const { return contentNode; }
    
    // 样式组特有方法
    void AddStyleProperty(const std::string& name, const std::string& value = "");
    bool HasStyleProperty(const std::string& name) const;
    std::string GetStyleProperty(const std::string& name) const;
    std::unordered_map<std::string, std::string> GetStyleProperties() const { return styleProperties; }
    
    // 删除操作
    void DeleteProperty(const std::string& name);
    void DeleteInheritance(const std::string& type, const std::string& name);
    void AddDeletedProperty(const std::string& name) { deletedProperties.insert(name); }
    void AddDeletedInheritance(const std::string& inheritance) { deletedInheritances.insert(inheritance); }
    
    // 继承处理
    void AddInheritedCustom(const std::string& type, const std::string& name);
    std::vector<std::pair<std::string, std::string>> GetInheritedCustoms() const { return inheritedCustoms; }
    
    // 变量组特有方法
    void AddVariable(const std::string& name, const std::string& value = "");
    std::string GetVariable(const std::string& name) const;
    bool HasVariable(const std::string& name) const;
    
    // 特例化实例化
    NodePtr Instantiate(const std::unordered_map<std::string, std::string>& params,
                       const std::set<std::string>& deletions = {}) const;
    
    // 检查是否是无值样式组
    bool IsValuelessStyleGroup() const;
    
private:
    CustomType customType;
    std::string customName;
    NodePtr contentNode;
    
    // 样式属性（可能无值）
    std::unordered_map<std::string, std::string> styleProperties;
    
    // 变量（可能无值）
    std::unordered_map<std::string, std::string> variables;
    
    // 继承的自定义（type:name对）
    std::vector<std::pair<std::string, std::string>> inheritedCustoms;
    
    // 删除的属性和继承
    std::set<std::string> deletedProperties;
    std::set<std::string> deletedInheritances;
};

} // namespace CHTL