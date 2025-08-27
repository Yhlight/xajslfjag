#include "NamespaceNode.h"
#include <sstream>

namespace CHTL {

NamespaceNode::NamespaceNode(const std::string& name)
    : BaseNode(NodeType::NAMESPACE), namespaceName(name) {
}

std::string NamespaceNode::Generate() const {
    // 命名空间不生成输出
    return "";
}

NodePtr NamespaceNode::Clone() const {
    auto clone = std::make_shared<NamespaceNode>(namespaceName);
    clone->SetPosition(line, column);
    clone->parentNamespace = parentNamespace;
    clone->exceptItems = exceptItems;
    clone->exceptTypes = exceptTypes;
    
    // 克隆子节点
    for (const auto& child : children) {
        clone->AddChild(child->Clone());
    }
    
    return clone;
}

void NamespaceNode::Accept(NodeVisitor* visitor) {
    visitor->VisitNamespaceNode(this);
}

void NamespaceNode::AddExceptItem(const std::string& item) {
    exceptItems.insert(item);
}

void NamespaceNode::AddExceptType(const std::string& type) {
    exceptTypes.insert(type);
}

bool NamespaceNode::IsItemAllowed(const std::string& item) const {
    return exceptItems.find(item) == exceptItems.end();
}

bool NamespaceNode::IsTypeAllowed(const std::string& type) const {
    return exceptTypes.find(type) == exceptTypes.end();
}

std::string NamespaceNode::GetFullName() const {
    if (parentNamespace.empty()) {
        return namespaceName;
    }
    return parentNamespace + "." + namespaceName;
}

void NamespaceNode::MergeFrom(const NamespaceNode& other) {
    // 合并约束
    for (const auto& item : other.exceptItems) {
        exceptItems.insert(item);
    }
    
    for (const auto& type : other.exceptTypes) {
        exceptTypes.insert(type);
    }
    
    // 合并子节点
    for (const auto& child : other.children) {
        AddChild(child->Clone());
    }
}

} // namespace CHTL