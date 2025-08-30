#include "CommentNode.h"

namespace CHTL {

CommentNode::CommentNode(CommentType commentType, const std::string& content)
    : BaseNode(CHTLNodeType::COMMENT_NODE, "comment")
    , m_commentType(commentType)
    , m_commentContent(content) {
    setValue(content);
}

CommentType CommentNode::getCommentType() const {
    return m_commentType;
}

const std::string& CommentNode::getCommentContent() const {
    return m_commentContent;
}

void CommentNode::setCommentContent(const std::string& content) {
    m_commentContent = content;
    setValue(content);
}

bool CommentNode::isGeneratorComment() const {
    return m_commentType == CommentType::GENERATOR_COMMENT;
}

std::string CommentNode::getCommentTypeString() const {
    switch (m_commentType) {
        case CommentType::LINE_COMMENT: return "//";
        case CommentType::BLOCK_COMMENT: return "/* */";
        case CommentType::GENERATOR_COMMENT: return "--";
        default: return "unknown";
    }
}

std::shared_ptr<BaseNode> CommentNode::clone() const {
    auto cloned = std::make_shared<CommentNode>(m_commentType, m_commentContent);
    cloned->m_position = m_position;
    cloned->m_access = m_access;
    cloned->m_attributes = m_attributes;
    return cloned;
}

std::string CommentNode::toString() const {
    return "Comment(" + getCommentTypeString() + "): " + m_commentContent;
}

bool CommentNode::validate(ErrorReporter* errorReporter) const {
    return BaseNode::validate(errorReporter);
}

} // namespace CHTL