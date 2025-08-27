#pragma once

#include <string>
#include <iostream>
#include <unordered_map>

namespace CHTLJS {

// CHTL JS的Token类型定义
enum class TokenType {
    // 基本符号
    EOF_TOKEN,
    NEWLINE,
    WHITESPACE,
    
    // 分隔符
    LEFT_BRACE,        // {
    RIGHT_BRACE,       // }
    LEFT_BRACKET,      // [
    RIGHT_BRACKET,     // ]
    LEFT_PAREN,        // (
    RIGHT_PAREN,       // )
    SEMICOLON,         // ;
    COLON,             // :
    EQUALS,            // =
    COMMA,             // ,
    DOT,               // .
    
    // CHTL JS特有操作符
    ARROW,             // ->
    DOUBLE_LEFT_BRACE, // {{
    DOUBLE_RIGHT_BRACE,// }}
    AMPERSAND_ARROW,   // &->
    DOUBLE_STAR,       // **
    
    // 字面量
    STRING_LITERAL,    // "string" 或 'string'
    UNQUOTED_LITERAL,  // 无引号字符串
    NUMBER,            // 数字
    BOOLEAN,           // true/false
    NULL_LITERAL,      // null
    UNDEFINED,         // undefined
    
    // 标识符
    IDENTIFIER,        // 标识符
    
    // 注释
    SINGLE_COMMENT,    // // 注释
    MULTI_COMMENT,     // /* */ 注释
    
    // CHTL JS关键字
    VIR,               // vir
    LISTEN,            // listen
    DELEGATE,          // delegate
    ANIMATE,           // animate
    
    // JS关键字
    CONST,             // const
    LET,               // let
    VAR,               // var
    FUNCTION,          // function
    RETURN,            // return
    IF,                // if
    ELSE,              // else
    FOR,               // for
    WHILE,             // while
    DO,                // do
    BREAK,             // break
    CONTINUE,          // continue
    
    // CHTL JS函数属性
    TARGET,            // target
    DURATION,          // duration
    EASING,            // easing
    BEGIN,             // begin
    WHEN,              // when
    END,               // end
    LOOP,              // loop
    DIRECTION,         // direction
    DELAY,             // delay
    CALLBACK,          // callback
    CLICK,             // click
    AT,                // at
    
    // 其他
    ARROW_FUNCTION,    // =>
    PLUS,              // +
    MINUS,             // -
    STAR,              // *
    SLASH,             // /
    PERCENT,           // %
    
    // 特殊标记
    INVALID,           // 无效token
};

// Token结构体
struct Token {
    TokenType type;
    std::string value;
    int line;
    int column;
    int position;
    
    Token() : type(TokenType::INVALID), line(1), column(1), position(0) {}
    
    Token(TokenType t, const std::string& v, int l, int c, int pos)
        : type(t), value(v), line(l), column(c), position(pos) {}
    
    bool IsCHTLJSKeyword() const;
    bool IsJSKeyword() const;
    bool IsCHTLJSOperator() const;
    
    std::string ToString() const;
};

// Token工具类
class TokenUtils {
public:
    static std::string TokenTypeToString(TokenType type);
    static TokenType StringToTokenType(const std::string& str);
    static bool IsCHTLJSKeyword(const std::string& str);
    static bool IsJSKeyword(const std::string& str);
    static bool IsCHTLJSFunctionProperty(const std::string& str);
    
private:
    static std::unordered_map<std::string, TokenType> chtljsKeywordMap;
    static std::unordered_map<std::string, TokenType> jsKeywordMap;
    static std::unordered_map<std::string, TokenType> functionPropertyMap;
    static std::unordered_map<TokenType, std::string> tokenTypeNameMap;
    
    static void InitializeMaps();
    static bool mapsInitialized;
};

} // namespace CHTLJS