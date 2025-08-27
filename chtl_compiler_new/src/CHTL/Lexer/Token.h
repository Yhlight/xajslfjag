#pragma once

#include "TokenType.h"
#include "../../Core/Types.h"
#include <string>

namespace CHTL {
namespace Lexer {

/**
 * CHTL Token
 * 表示词法分析的最小单元
 */
class Token {
private:
    TokenType type_;
    std::string value_;
    SourceRange range_;
    
public:
    Token() : type_(TokenType::UNKNOWN) {}
    
    Token(TokenType type, const std::string& value, const SourceRange& range)
        : type_(type), value_(value), range_(range) {}
    
    // 基本访问器
    TokenType getType() const { return type_; }
    const std::string& getValue() const { return value_; }
    const SourceRange& getRange() const { return range_; }
    
    // 位置信息
    size_t getLine() const { return range_.start.line; }
    size_t getColumn() const { return range_.start.column; }
    const std::string& getFilename() const { return range_.start.filename; }
    
    // 类型检查便利方法
    bool isLiteral() const {
        return type_ == TokenType::UNQUOTED_LITERAL ||
               type_ == TokenType::DOUBLE_QUOTED_STRING ||
               type_ == TokenType::SINGLE_QUOTED_STRING;
    }
    
    bool isKeyword() const {
        return type_ == TokenType::TEXT ||
               type_ == TokenType::STYLE ||
               type_ == TokenType::SCRIPT;
    }
    
    bool isBlockKeyword() const {
        return type_ == TokenType::TEMPLATE ||
               type_ == TokenType::CUSTOM ||
               type_ == TokenType::CONFIGURATION ||
               type_ == TokenType::IMPORT ||
               type_ == TokenType::NAMESPACE ||
               type_ == TokenType::ORIGIN;
    }
    
    bool isTypePrefix() const {
        return type_ == TokenType::AT_ELEMENT ||
               type_ == TokenType::AT_STYLE ||
               type_ == TokenType::AT_VAR ||
               type_ == TokenType::AT_HTML ||
               type_ == TokenType::AT_JAVASCRIPT ||
               type_ == TokenType::AT_CHTL ||
               type_ == TokenType::AT_CJMOD ||
               type_ == TokenType::AT_CONFIG;
    }
    
    bool isComment() const {
        return type_ == TokenType::LINE_COMMENT ||
               type_ == TokenType::BLOCK_COMMENT ||
               type_ == TokenType::GENERATOR_COMMENT;
    }
    
    bool isWhitespace() const {
        return type_ == TokenType::WHITESPACE ||
               type_ == TokenType::NEWLINE;
    }
    
    // CE对等式检查 (: 和 = 等价)
    bool isAssignmentOperator() const {
        return type_ == TokenType::COLON || type_ == TokenType::EQUALS;
    }
    
    // 调试输出
    std::string toString() const;
    
    // 相等比较
    bool operator==(const Token& other) const {
        return type_ == other.type_ && value_ == other.value_;
    }
    
    bool operator!=(const Token& other) const {
        return !(*this == other);
    }
};

} // namespace Lexer
} // namespace CHTL