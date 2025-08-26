#ifndef CHTL_TOKEN_H
#define CHTL_TOKEN_H

#include <string>

namespace CHTL {

enum class TokenType {
    // 基本符号
    LBRACE, RBRACE, LPAREN, RPAREN, LBRACKET, RBRACKET,
    SEMICOLON, COLON, EQUAL, COMMA, DOT, SLASH, STAR, AMPERSAND,
    
    // 字面量
    STRING, IDENTIFIER, NUMBER,
    
    // 注释
    LINE_COMMENT, BLOCK_COMMENT, GENERATOR_COMMENT,
    
    // 关键字
    TEXT, STYLE, SCRIPT, INHERIT, DELETE, INSERT, FROM, AS, USE,
    
    // 前缀
    TEMPLATE, CUSTOM, ORIGIN, IMPORT, NAMESPACE, CONFIGURATION,
    
    // 类型
    AT_STYLE, AT_ELEMENT, AT_VAR, AT_HTML, AT_JAVASCRIPT, AT_CHTL,
    
    // 选择器
    CLASS_SELECTOR, ID_SELECTOR,
    
    // 特殊
    EOF_TOKEN, INVALID
};

struct Token {
    TokenType type;
    std::string value;
    size_t line;
    size_t column;
    
    Token(TokenType t = TokenType::INVALID, const std::string& v = "", size_t l = 0, size_t c = 0)
        : type(t), value(v), line(l), column(c) {}
};

} // namespace CHTL

#endif