#include "OriginNode.h"
#include "NodeVisitor.h"
#include <sstream>

namespace CHTL {

OriginNode::OriginNode(const std::string& name)
    : BaseNode(NodeType::ORIGIN, name) {
}

NodePtr OriginNode::clone(bool deep) const {
    auto cloned = std::make_shared<OriginNode>(name_);
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

void OriginNode::accept(NodeVisitor& visitor) {
    visitor.visitOrigin(*this);
}

std::string OriginNode::toString(int indent) const {
    std::ostringstream oss;
    oss << getIndent(indent) << "OriginNode{name=\"" << name_ << "\"}";
    
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