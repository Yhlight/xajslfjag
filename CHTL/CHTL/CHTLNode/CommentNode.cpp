#include "CommentNode.h"
#include "NodeVisitor.h"
#include <sstream>

namespace CHTL {

CommentNode::CommentNode(CommentType type, const std::string& content)
    : BaseNode(NodeType::COMMENT, "comment"), comment_type_(type), 
      content_(cleanContent(content)), preserve_in_output_(type == CommentType::GENERATOR) {
}

void CommentNode::appendContent(const std::string& content) {
    if (!content_.empty()) {
        content_ += "\n";
    }
    content_ += content;
}

NodePtr CommentNode::clone(bool deep) const {
    auto cloned = std::make_shared<CommentNode>(comment_type_, content_);
    cloned->attributes_ = attributes_;
    cloned->start_pos_ = start_pos_;
    cloned->end_pos_ = end_pos_;
    cloned->raw_content_ = raw_content_;
    cloned->preserve_in_output_ = preserve_in_output_;
    
    if (deep) {
        for (const auto& child : children_) {
            cloned->addChild(child->clone(true));
        }
    }
    
    return cloned;
}

void CommentNode::accept(NodeVisitor& visitor) {
    visitor.visitComment(*this);
}

std::string CommentNode::toString(int indent) const {
    std::ostringstream oss;
    oss << getIndent(indent) << "CommentNode{type=" << getCommentTypeString()
        << ", preserve=" << (preserve_in_output_ ? "true" : "false")
        << ", content=\"" << escapeString(content_) << "\"}";
    
    return oss.str();
}

std::string CommentNode::toComment(const std::string& target_language) const {
    if (content_.empty()) {
        return "";
    }

    std::ostringstream oss;
    
    if (target_language == "html") {
        oss << "<!-- " << content_ << " -->";
    } else if (target_language == "css" || target_language == "js") {
        if (comment_type_ == CommentType::SINGLE_LINE) {
            oss << "// " << content_;
        } else {
            oss << "/* " << content_ << " */";
        }
    } else {
        // 默认使用原始格式
        switch (comment_type_) {
            case CommentType::SINGLE_LINE:
                oss << "// " << content_;
                break;
            case CommentType::MULTI_LINE:
                oss << "/* " << content_ << " */";
                break;
            case CommentType::GENERATOR:
                oss << "-- " << content_;
                break;
        }
    }
    
    return oss.str();
}

std::string CommentNode::validate() const {
    // 检查注释内容
    if (comment_type_ == CommentType::MULTI_LINE) {
        // 检查多行注释是否包含结束标记
        if (content_.find("*/") != std::string::npos) {
            return "Multi-line comment content should not contain '*/'";
        }
    }
    
    return BaseNode::validate();
}

std::string CommentNode::getCommentTypeString() const {
    switch (comment_type_) {
        case CommentType::SINGLE_LINE: return "SINGLE_LINE";
        case CommentType::MULTI_LINE: return "MULTI_LINE";
        case CommentType::GENERATOR: return "GENERATOR";
        default: return "UNKNOWN";
    }
}

std::string CommentNode::cleanContent(const std::string& raw_content) const {
    if (raw_content.empty()) {
        return "";
    }
    
    std::string content = raw_content;
    
    // 移除注释标记
    if (content.substr(0, 2) == "//") {
        content = content.substr(2);
    } else if (content.substr(0, 2) == "/*" && content.substr(content.length() - 2) == "*/") {
        content = content.substr(2, content.length() - 4);
    } else if (content.substr(0, 2) == "--") {
        content = content.substr(2);
    }
    
    // 移除前后的空白字符
    size_t start = content.find_first_not_of(" \t\n\r");
    if (start == std::string::npos) {
        return "";
    }
    
    size_t end = content.find_last_not_of(" \t\n\r");
    return content.substr(start, end - start + 1);
}

} // namespace CHTL