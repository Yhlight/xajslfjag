#pragma once

#include "CHTLBaseNode.h"

namespace CHTL {

// 注释类型枚举
enum class CHTLCommentType {
    SINGLE_LINE,    // //
    MULTI_LINE,     // /* */
    GENERATOR       // --
};

// 注释节点
class CHTLCommentNode : public CHTLBaseNode {
public:
    explicit CHTLCommentNode(const std::string& content = "", CHTLCommentType type = CHTLCommentType::SINGLE_LINE);
    virtual ~CHTLCommentNode() = default;
    
    // 注释内容管理
    void setContent(const std::string& content) { this->content = content; }
    const std::string& getContent() const { return content; }
    
    // 注释类型管理
    void setCommentType(CHTLCommentType type) { this->commentType = type; }
    CHTLCommentType getCommentType() const { return commentType; }
    std::string getCommentTypeName() const;
    
    // 注释处理
    std::string getProcessedContent() const;
    void appendContent(const std::string& content);
    void prependContent(const std::string& content);
    
    // 注释属性
    bool isEmpty() const { return content.empty(); }
    size_t getLength() const { return content.length(); }
    
    // 空白符处理
    bool isWhitespaceOnly() const;
    std::string getTrimmedContent() const;
    void trimContent();
    
    // 输出格式化
    std::string toHTML() const;
    std::string toCSS() const;
    std::string toJavaScript() const;
    std::string toOriginalFormat() const;
    
    // 生成器注释特殊处理
    bool isGeneratorComment() const { return commentType == CHTLCommentType::GENERATOR; }
    std::string generateContextualComment(const std::string& context) const;
    
    // 访问者模式
    void accept(CHTLNodeVisitor* visitor) override;
    
    // 深度复制
    std::shared_ptr<CHTLBaseNode> clone() const override;
    
    // 调试输出
    std::string toString() const override;

private:
    std::string content;
    CHTLCommentType commentType;
    
    // 辅助方法
    std::string escapeComment(const std::string& input, const std::string& format) const;
    std::vector<std::string> splitLines(const std::string& input) const;
};

} // namespace CHTL