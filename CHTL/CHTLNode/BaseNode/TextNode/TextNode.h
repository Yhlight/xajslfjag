#pragma once
#include "../BaseNode.h"

namespace CHTL {

/**
 * 文本节点
 * 表示CHTL中的text {}块或纯文本内容
 */
class TextNode : public BaseNode {
public:
    explicit TextNode(const std::string& text = "", const Position& pos = Position{});
    ~TextNode() override = default;
    
    // 文本内容管理
    void setText(const std::string& text) { text_ = text; setContent(text); }
    const std::string& getText() const { return text_; }
    
    // 文本类型
    enum class TextType {
        QUOTED_STRING,      // 带引号的字符串 "text"
        UNQUOTED_LITERAL,   // 无修饰字面量 text
        INLINE_TEXT,        // 内联文本（元素内部的直接文本）
        TEXT_BLOCK          // text {} 块
    };
    
    void setTextType(TextType type) { textType_ = type; }
    TextType getTextType() const { return textType_; }
    
    // 文本处理
    std::string getProcessedText() const;
    std::string getEscapedText() const;
    std::string getTrimmedText() const;
    
    // 空白处理
    void setPreserveWhitespace(bool preserve) { preserveWhitespace_ = preserve; }
    bool isWhitespacePreserved() const { return preserveWhitespace_; }
    
    // 是否为空文本
    bool isEmpty() const;
    bool isWhitespaceOnly() const;
    
    // 文本格式化
    void normalize(); // 规范化空白字符
    void trim();      // 去除首尾空白
    
    // 验证
    bool validate() const override;
    std::vector<std::string> getValidationErrors() const override;
    
    // BaseNode接口实现
    std::string toString() const override;
    std::shared_ptr<BaseNode> clone() const override;
    void accept(CHTLGenerator* generator) override;
    
private:
    std::string text_;
    TextType textType_ = TextType::TEXT_BLOCK;
    bool preserveWhitespace_ = false;
    
    // 辅助方法
    std::string escapeHtml(const std::string& text) const;
    std::string normalizeWhitespace(const std::string& text) const;
};

} // namespace CHTL