#pragma once

#include "BaseNode.h"
#include <set>

namespace CHTL {

// 命名空间节点
class NamespaceNode : public BaseNode {
public:
    NamespaceNode(const std::string& name);
    
    std::string GetName() const override { return namespaceName; }
    
    std::string Generate() const override;
    NodePtr Clone() const override;
    void Accept(NodeVisitor* visitor) override;
    
    // 约束管理
    void AddExceptItem(const std::string& item);
    void AddExceptType(const std::string& type);
    bool IsItemAllowed(const std::string& item) const;
    bool IsTypeAllowed(const std::string& type) const;
    
    std::set<std::string> GetExceptItems() const { return exceptItems; }
    std::set<std::string> GetExceptTypes() const { return exceptTypes; }
    
    // 嵌套命名空间
    void SetParentNamespace(const std::string& parent) { parentNamespace = parent; }
    std::string GetParentNamespace() const { return parentNamespace; }
    std::string GetFullName() const;
    
    // 合并同名命名空间
    void MergeFrom(const NamespaceNode& other);
    
private:
    std::string namespaceName;
    std::string parentNamespace;  // 父命名空间名称
    
    // 约束
    std::set<std::string> exceptItems;  // 禁用的具体项
    std::set<std::string> exceptTypes;  // 禁用的类型
};

} // namespace CHTL