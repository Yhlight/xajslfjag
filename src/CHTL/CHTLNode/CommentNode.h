#ifndef CHTL_COMMENT_NODE_H
#define CHTL_COMMENT_NODE_H

#include "BaseNode.h"

namespace CHTL {

// 注释类型
enum class CommentType {
    SINGLE_LINE,    // //
    MULTI_LINE,     // /* */
    GENERATOR       // --
};

// 注释节点
class CommentNode : public ASTNode {
public:
    CommentNode(CommentType commentType, const std::string& content, 
                const TokenLocation& location)
        : ASTNode(NodeType::COMMENT, location), 
          commentType_(commentType), content_(content) {}
    
    CommentType getCommentType() const { return commentType_; }
    const std::string& getContent() const { return content_; }
    
    void accept(Visitor* visitor) override;
    std::string toString() const override;
    
private:
    CommentType commentType_;
    std::string content_;
};

// 扩展访问者接口
class CommentVisitor : public Visitor {
public:
    virtual void visitCommentNode(CommentNode* node) = 0;
};

} // namespace CHTL

#endif // CHTL_COMMENT_NODE_H