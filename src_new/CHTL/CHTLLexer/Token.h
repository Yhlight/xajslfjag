#pragma once

#include <string>

namespace CHTL {

enum class TokenType {
    IDENTIFIER,
    STRING_LITERAL,
    NUMBER_LITERAL,
    
    // 符号
    BRACE_OPEN,        // {
    BRACE_CLOSE,       // }
    BRACKET_OPEN,      // [
    BRACKET_CLOSE,     // ]
    PAREN_OPEN,        // (
    PAREN_CLOSE,       // )
    
    COLON,             // :
    SEMICOLON,         // ;
    COMMA,             // ,
    AT,                // @
    ASSIGN,            // =
    
    // CHTL JS特殊符号
    ENHANCED_SELECTOR, // {{...}}
    EVENT_BINDING,     // &->
    ARROW,             // ->
    
    // 注释
    COMMENT_NORMAL,    // // 或 /**/
    COMMENT_GENERATOR, // --
    
    // 特殊
    WHITESPACE,
    NEWLINE,
    EOF_TOKEN,
    UNKNOWN
};

struct Token {
    TokenType type;
    std::string value;
    size_t line;
    size_t column;
    size_t position;
    
    Token() : type(TokenType::UNKNOWN), line(0), column(0), position(0) {}
    Token(TokenType t, const std::string& v, size_t l = 0, size_t c = 0, size_t p = 0)
        : type(t), value(v), line(l), column(c), position(p) {}
};

}