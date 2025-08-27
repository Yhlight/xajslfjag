#include "TextNode.h"
#include <algorithm>
#include <sstream>
#include <cctype>

namespace CHTL {

TextNode::TextNode(const std::string& text, const Position& pos)
    : BaseNode(NodeType::TEXT_NODE, pos), text_(text) {
    setContent(text);
}

std::string TextNode::getProcessedText() const {
    std::string result = text_;
    
    if (!preserveWhitespace_) {
        result = normalizeWhitespace(result);
    }
    
    return result;
}

std::string TextNode::getEscapedText() const {
    return escapeHtml(getProcessedText());
}

std::string TextNode::getTrimmedText() const {
    std::string result = text_;
    
    // 去除前导空白
    result.erase(result.begin(), std::find_if(result.begin(), result.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
    
    // 去除尾随空白
    result.erase(std::find_if(result.rbegin(), result.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), result.end());
    
    return result;
}

bool TextNode::isEmpty() const {
    return text_.empty();
}

bool TextNode::isWhitespaceOnly() const {
    return std::all_of(text_.begin(), text_.end(), [](unsigned char c) {
        return std::isspace(c);
    });
}

void TextNode::normalize() {
    text_ = normalizeWhitespace(text_);
    setContent(text_);
}

void TextNode::trim() {
    text_ = getTrimmedText();
    setContent(text_);
}

bool TextNode::validate() const {
    // 文本节点一般总是有效的，除非有特殊约束
    return true;
}

std::vector<std::string> TextNode::getValidationErrors() const {
    std::vector<std::string> errors;
    
    // 可以在这里添加文本验证规则，比如：
    // - 检查非法字符
    // - 检查长度限制
    // - 检查编码问题等
    
    return errors;
}

std::string TextNode::toString() const {
    switch (textType_) {
        case TextType::QUOTED_STRING:
            return "\"" + getEscapedText() + "\"";
        case TextType::UNQUOTED_LITERAL:
            return getProcessedText();
        case TextType::INLINE_TEXT:
            return getEscapedText();
        case TextType::TEXT_BLOCK:
            return "text { " + getProcessedText() + " }";
        default:
            return getProcessedText();
    }
}

std::shared_ptr<BaseNode> TextNode::clone() const {
    auto cloned = std::make_shared<TextNode>(text_, position_);
    cloned->textType_ = textType_;
    cloned->preserveWhitespace_ = preserveWhitespace_;
    
    // 复制基础属性
    for (const auto& attr : attributes_) {
        cloned->setAttribute(attr.first, attr.second);
    }
    
    for (const auto& meta : metadata_) {
        cloned->setMetadata(meta.first, meta.second);
    }
    
    return cloned;
}

void TextNode::accept(CHTLGenerator* /*generator*/) {
    // 在实现CHTLGenerator时会完成这个方法
    // generator->visitTextNode(this);
}

std::string TextNode::escapeHtml(const std::string& text) const {
    std::string result;
    result.reserve(text.length() * 1.2); // 预分配空间
    
    for (char c : text) {
        switch (c) {
            case '<':
                result += "&lt;";
                break;
            case '>':
                result += "&gt;";
                break;
            case '&':
                result += "&amp;";
                break;
            case '"':
                result += "&quot;";
                break;
            case '\'':
                result += "&#39;";
                break;
            default:
                result += c;
                break;
        }
    }
    
    return result;
}

std::string TextNode::normalizeWhitespace(const std::string& text) const {
    std::string result;
    bool inWhitespace = false;
    
    for (char c : text) {
        if (std::isspace(c)) {
            if (!inWhitespace) {
                result += ' '; // 将所有空白字符规范化为空格
                inWhitespace = true;
            }
            // 跳过连续的空白字符
        } else {
            result += c;
            inWhitespace = false;
        }
    }
    
    return result;
}

} // namespace CHTL