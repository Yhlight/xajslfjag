#include "OriginNode.h"
#include <sstream>

namespace CHTL {

void OriginNode::accept(NodeVisitor* visitor) {
    if (auto* originVisitor = dynamic_cast<OriginNodeVisitor*>(visitor)) {
        originVisitor->visitOriginNode(this);
    }
}

std::shared_ptr<BaseNode> OriginNode::clone() const {
    auto cloned = std::make_shared<OriginNode>(originType, name, line, column);
    cloned->content = content;
    
    // 递归克隆子节点（虽然原始嵌入通常没有子节点）
    for (const auto& child : children) {
        cloned->addChild(child->clone());
    }
    
    return cloned;
}

std::string OriginNode::toString() const {
    std::stringstream ss;
    
    ss << "[Origin] " << originType;
    
    if (!name.empty()) {
        ss << " " << name;
    }
    
    ss << " {\n";
    ss << content;
    ss << "\n}";
    
    return ss.str();
}

} // namespace CHTL