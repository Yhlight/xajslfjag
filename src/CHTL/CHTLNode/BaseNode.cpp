#include "BaseNode.h"
#include <sstream>

namespace CHTL {

// ElementNode implementation
void ElementNode::accept(Visitor* visitor) {
    visitor->visitElementNode(this);
}

std::string ElementNode::toString() const {
    std::stringstream ss;
    ss << "ElementNode(" << tagName_;
    
    if (index_.has_value()) {
        ss << "[" << index_.value() << "]";
    }
    
    if (!attributes_.empty()) {
        ss << ", attrs={";
        bool first = true;
        for (const auto& [name, value] : attributes_) {
            if (!first) ss << ", ";
            ss << name << "=\"" << value << "\"";
            first = false;
        }
        ss << "}";
    }
    
    ss << ", children=" << children_.size() << ")";
    return ss.str();
}

// TextNode implementation
void TextNode::accept(Visitor* visitor) {
    visitor->visitTextNode(this);
}

std::string TextNode::toString() const {
    std::stringstream ss;
    ss << "TextNode(\"" << content_ << "\")";
    return ss.str();
}

// AttributeNode implementation
void AttributeNode::accept(Visitor* visitor) {
    visitor->visitAttributeNode(this);
}

std::string AttributeNode::toString() const {
    std::stringstream ss;
    ss << "AttributeNode(" << name_ << "=\"" << value_ << "\")";
    return ss.str();
}

} // namespace CHTL