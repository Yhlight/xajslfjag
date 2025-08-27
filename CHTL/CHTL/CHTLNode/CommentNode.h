#pragma once

#include "BaseNode.h"

namespace CHTL {

// 注释节点类型
enum class CommentType {
    SINGLE_LINE,    // // 单行注释
    MULTI_LINE,     // /* */ 多行注释  
    GENERATOR       // -- 生成器注释
};

// 注释节点
class CommentNode : public BaseNode {
public:
    CommentNode(CommentType type, const std::string& content);
    
    CommentType GetCommentType() const { return commentType; }
    std::string GetContent() const { return content; }
    void SetContent(const std::string& c) { content = c; }
    
    std::string Generate() const override;
    NodePtr Clone() const override;
    void Accept(NodeVisitor* visitor) override;
    
    // 获取生成的注释格式（根据上下文）
    std::string GenerateForContext(const std::string& context) const;
    
private:
    CommentType commentType;
    std::string content;
};

} // namespace CHTL