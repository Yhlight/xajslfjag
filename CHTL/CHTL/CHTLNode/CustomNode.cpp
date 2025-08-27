#include "CustomNode.h"
#include "NodeVisitor.h"
#include <sstream>

namespace CHTL {

CustomNode::CustomNode(const std::string& name)
    : BaseNode(NodeType::CUSTOM, name) {
}

NodePtr CustomNode::clone(bool deep) const {
    auto cloned = std::make_shared<CustomNode>(name_);
    cloned->attributes_ = attributes_;
    cloned->start_pos_ = start_pos_;
    cloned->end_pos_ = end_pos_;
    cloned->raw_content_ = raw_content_;
    
    if (deep) {
        for (const auto& child : children_) {
            cloned->addChild(child->clone(true));
        }
    }
    
    return cloned;
}

void CustomNode::accept(NodeVisitor& visitor) {
    visitor.visitCustom(*this);
}

std::string CustomNode::toString(int indent) const {
    std::ostringstream oss;
    oss << getIndent(indent) << "CustomNode{name=\"" << name_ << "\"}";
    
    if (!children_.empty()) {
        oss << " {" << std::endl;
        for (const auto& child : children_) {
            oss << child->toString(indent + 1) << std::endl;
        }
        oss << getIndent(indent) << "}";
    }
    
    return oss.str();
}

} // namespace CHTL