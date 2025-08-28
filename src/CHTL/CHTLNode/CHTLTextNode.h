#pragma once

#include "CHTLBaseNode.h"

namespace CHTL {

// 文本节点
class CHTLTextNode : public CHTLBaseNode {
public:
    explicit CHTLTextNode(const std::string& text = "");
    virtual ~CHTLTextNode() = default;
    
    // 文本内容管理
    void setText(const std::string& text) { this->text = text; }
    const std::string& getText() const { return text; }
    
    // 文本处理
    std::string getProcessedText() const; // 处理转义字符等
    void appendText(const std::string& text);
    void prependText(const std::string& text);
    
    // 文本属性
    bool isEmpty() const { return text.empty(); }
    size_t getLength() const { return text.length(); }
    
    // 空白符处理
    bool isWhitespaceOnly() const;
    std::string getTrimmedText() const;
    void trimText();
    
    // 文本转换
    std::string toHTML() const;
    std::string toPlainText() const;
    
    // 访问者模式
    void accept(CHTLNodeVisitor* visitor) override;
    
    // 深度复制
    std::shared_ptr<CHTLBaseNode> clone() const override;
    
    // 调试输出
    std::string toString() const override;

private:
    std::string text;
    
    // 辅助方法
    std::string escapeHTML(const std::string& input) const;
    std::string unescapeHTML(const std::string& input) const;
};

} // namespace CHTL