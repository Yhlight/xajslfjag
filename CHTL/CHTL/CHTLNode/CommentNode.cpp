#include "CommentNode.h"
#include <sstream>

namespace CHTL {

CommentNode::CommentNode(CommentType type, const std::string& content)
    : BaseNode(NodeType::COMMENT), commentType(type), content(content) {
}

std::string CommentNode::Generate() const {
    switch (commentType) {
        case CommentType::SINGLE_LINE:
        case CommentType::MULTI_LINE:
            // 单行和多行注释不生成到最终HTML
            return "";
            
        case CommentType::GENERATOR:
            // 生成器注释转换为HTML注释
            return "<!-- " + content + " -->";
            
        default:
            return "";
    }
}

NodePtr CommentNode::Clone() const {
    auto clone = std::make_shared<CommentNode>(commentType, content);
    clone->SetPosition(line, column);
    return clone;
}

void CommentNode::Accept(NodeVisitor* visitor) {
    visitor->VisitCommentNode(this);
}

std::string CommentNode::GenerateForContext(const std::string& context) const {
    // 只有生成器注释需要根据上下文生成
    if (commentType != CommentType::GENERATOR) {
        return "";
    }
    
    // 根据上下文决定注释格式
    if (context == "css" || context == "style") {
        return "/* " + content + " */";
    } else if (context == "js" || context == "script") {
        return "// " + content;
    } else {
        // HTML上下文
        return "<!-- " + content + " -->";
    }
}

} // namespace CHTL