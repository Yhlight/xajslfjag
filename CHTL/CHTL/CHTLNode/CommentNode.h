#ifndef CHTL_COMMENT_NODE_H
#define CHTL_COMMENT_NODE_H

#include "BaseNode.h"

namespace CHTL {

// 注释类型枚举
enum class CommentType {
    SINGLE_LINE,    // // 单行注释
    MULTI_LINE,     // /* */ 多行注释
    GENERATE        // -- 生成注释
};

// 注释节点类
class CommentNode : public BaseNode {
private:
    std::string content;
    CommentType commentType;
    
public:
    CommentNode(const std::string& text, CommentType type, int line = 0, int col = 0)
        : BaseNode(NodeType::COMMENT, line, col), content(text), commentType(type) {}
    
    // 内容访问
    const std::string& getContent() const { return content; }
    void setContent(const std::string& text) { content = text; }
    
    // 注释类型访问
    CommentType getCommentType() const { return commentType; }
    void setCommentType(CommentType type) { commentType = type; }
    
    // 判断是否为生成注释
    bool isGenerateComment() const { return commentType == CommentType::GENERATE; }
    
    // 重写虚函数
    void accept(NodeVisitor* visitor) override;
    std::shared_ptr<BaseNode> clone() const override;
    std::string toString() const override;
};

// 扩展访问者接口
class CommentNodeVisitor : public NodeVisitor {
public:
    virtual void visitCommentNode(CommentNode* node) = 0;
};

} // namespace CHTL

#endif // CHTL_COMMENT_NODE_H