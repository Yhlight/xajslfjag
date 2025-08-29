#include "BaseNode.h"
#include <sstream>

namespace CHTLJS {

// IdentifierNode implementation
void IdentifierNode::accept(Visitor* visitor) {
    visitor->visitIdentifierNode(this);
}

std::string IdentifierNode::toString() const {
    return "IdentifierNode(" + name_ + ")";
}

// LiteralNode implementation
void LiteralNode::accept(Visitor* visitor) {
    visitor->visitLiteralNode(this);
}

std::string LiteralNode::toString() const {
    std::stringstream ss;
    ss << "LiteralNode(";
    
    switch (literalType_) {
        case LiteralType::STRING:
            ss << "STRING, \"" << std::get<std::string>(value_) << "\"";
            break;
        case LiteralType::NUMBER:
            if (std::holds_alternative<int64_t>(value_)) {
                ss << "NUMBER, " << std::get<int64_t>(value_);
            } else if (std::holds_alternative<double>(value_)) {
                ss << "NUMBER, " << std::get<double>(value_);
            }
            break;
        case LiteralType::BOOLEAN:
            ss << "BOOLEAN, " << (std::get<bool>(value_) ? "true" : "false");
            break;
        case LiteralType::NULL_VALUE:
            ss << "NULL";
            break;
        case LiteralType::UNQUOTED:
            ss << "UNQUOTED, " << std::get<std::string>(value_);
            break;
    }
    
    ss << ")";
    return ss.str();
}

} // namespace CHTLJS