#include "OperatorNode.h"

namespace CHTL {

OperatorNode::OperatorNode(OperatorType type)
    : BaseNode(NodeType::DELETE_OP), operatorType(type), 
      insertPosition(InsertPosition::AFTER) {
    
    // 根据操作符类型设置正确的节点类型
    switch (type) {
        case OperatorType::DELETE:
            nodeType = NodeType::DELETE_OP;
            break;
        case OperatorType::INSERT:
            nodeType = NodeType::INSERT_OP;
            break;
        case OperatorType::USE:
            nodeType = NodeType::USE_OP;
            break;
        default:
            break;
    }
}

std::string OperatorNode::Generate() const {
    // 操作符节点不直接生成输出
    // 它们的效果在解析和转换阶段应用
    return "";
}

NodePtr OperatorNode::Clone() const {
    auto clone = std::make_shared<OperatorNode>(operatorType);
    clone->SetPosition(line, column);
    
    // 复制各种属性
    clone->deleteTargets = deleteTargets;
    clone->insertPosition = insertPosition;
    clone->insertSelector = insertSelector;
    if (insertContent) {
        clone->insertContent = insertContent->Clone();
    }
    clone->useTarget = useTarget;
    clone->inheritTargets = inheritTargets;
    clone->exceptItems = exceptItems;
    clone->exceptTypes = exceptTypes;
    
    return clone;
}

void OperatorNode::Accept(NodeVisitor* visitor) {
    visitor->VisitOperatorNode(this);
}

void OperatorNode::AddDeleteTarget(const std::string& target) {
    deleteTargets.push_back(target);
}

void OperatorNode::AddInheritTarget(const std::string& type, const std::string& name) {
    inheritTargets.push_back({type, name});
}

void OperatorNode::AddExceptItem(const std::string& item) {
    exceptItems.push_back(item);
}

void OperatorNode::AddExceptType(const std::string& type) {
    exceptTypes.push_back(type);
}

} // namespace CHTL