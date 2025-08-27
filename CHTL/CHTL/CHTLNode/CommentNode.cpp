#include "CommentNode.h"

namespace CHTL {

void CommentNode::accept(NodeVisitor* visitor) {
    if (auto* commentVisitor = dynamic_cast<CommentNodeVisitor*>(visitor)) {
        commentVisitor->visitCommentNode(this);
    }
}

std::shared_ptr<BaseNode> CommentNode::clone() const {
    return std::make_shared<CommentNode>(content, commentType, line, column);
}

std::string CommentNode::toString() const {
    switch (commentType) {
        case CommentType::SINGLE_LINE:
            return "//" + content;
        case CommentType::MULTI_LINE:
            return "/*" + content + "*/";
        case CommentType::GENERATE:
            return "--" + content;
        default:
            return content;
    }
}

} // namespace CHTL