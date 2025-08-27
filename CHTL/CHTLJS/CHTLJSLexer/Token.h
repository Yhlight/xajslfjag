#ifndef CHTLJS_TOKEN_H
#define CHTLJS_TOKEN_H

#include <string>
#include <utility>

namespace CHTLJS {

// CHTL JS Token类型枚举
enum class TokenType {
    // 字面量
    STRING_LITERAL,        // 字符串字面量
    NUMBER_LITERAL,        // 数字字面量
    BOOLEAN_LITERAL,       // 布尔字面量
    NULL_LITERAL,          // null
    
    // 标识符
    IDENTIFIER,            // 标识符
    
    // 关键字
    KEYWORD_CONST,         // const
    KEYWORD_LET,           // let
    KEYWORD_VAR,           // var
    KEYWORD_FUNCTION,      // function
    KEYWORD_RETURN,        // return
    KEYWORD_IF,            // if
    KEYWORD_ELSE,          // else
    KEYWORD_FOR,           // for
    KEYWORD_WHILE,         // while
    KEYWORD_DO,            // do
    KEYWORD_BREAK,         // break
    KEYWORD_CONTINUE,      // continue
    KEYWORD_TRUE,          // true
    KEYWORD_FALSE,         // false
    KEYWORD_NULL,          // null
    KEYWORD_VIR,           // vir (虚对象)
    
    // CHTL JS 特殊关键字
    KEYWORD_LISTEN,        // listen
    KEYWORD_ANIMATE,       // animate
    KEYWORD_DELEGATE,      // delegate
    KEYWORD_TARGET,        // target
    KEYWORD_DURATION,      // duration
    KEYWORD_EASING,        // easing
    KEYWORD_BEGIN,         // begin
    KEYWORD_WHEN,          // when
    KEYWORD_END,           // end
    KEYWORD_AT,            // at
    KEYWORD_LOOP,          // loop
    KEYWORD_DIRECTION,     // direction
    KEYWORD_DELAY,         // delay
    KEYWORD_CALLBACK,      // callback
    
    // 操作符
    PLUS,                  // +
    MINUS,                 // -
    MULTIPLY,              // *
    DIVIDE,                // /
    MODULO,                // %
    ASSIGN,                // =
    PLUS_ASSIGN,           // +=
    MINUS_ASSIGN,          // -=
    MULTIPLY_ASSIGN,       // *=
    DIVIDE_ASSIGN,         // /=
    MODULO_ASSIGN,         // %=
    EQUALS,                // ==
    NOT_EQUALS,            // !=
    STRICT_EQUALS,         // ===
    STRICT_NOT_EQUALS,     // !==
    LESS_THAN,             // <
    GREATER_THAN,          // >
    LESS_EQUALS,           // <=
    GREATER_EQUALS,        // >=
    LOGICAL_AND,           // &&
    LOGICAL_OR,            // ||
    LOGICAL_NOT,           // !
    BITWISE_AND,           // &
    BITWISE_OR,            // |
    BITWISE_XOR,           // ^
    BITWISE_NOT,           // ~
    LEFT_SHIFT,            // <<
    RIGHT_SHIFT,           // >>
    UNSIGNED_RIGHT_SHIFT,  // >>>
    INCREMENT,             // ++
    DECREMENT,             // --
    QUESTION,              // ?
    
    // CHTL JS 特殊操作符
    ARROW,                 // ->
    DOUBLE_ARROW,          // =>
    BIND_ARROW,            // &->
    
    // 符号
    LEFT_PAREN,            // (
    RIGHT_PAREN,           // )
    LEFT_BRACE,            // {
    RIGHT_BRACE,           // }
    LEFT_BRACKET,          // [
    RIGHT_BRACKET,         // ]
    SEMICOLON,             // ;
    COMMA,                 // ,
    DOT,                   // .
    COLON,                 // :
    
    // CHTL JS 增强选择器
    DOUBLE_LEFT_BRACE,     // {{
    DOUBLE_RIGHT_BRACE,    // }}
    
    // 特殊
    NEWLINE,               // 换行
    WHITESPACE,            // 空白
    COMMENT,               // 注释
    EOF_TOKEN,             // 文件结束
    UNKNOWN                // 未知token
};

// Token结构体
struct Token {
    TokenType type;
    std::string value;
    int line;
    int column;
    int length;
    
    Token() : type(TokenType::UNKNOWN), line(0), column(0), length(0) {}
    
    Token(TokenType t, std::string v, int l, int c, int len)
        : type(t), value(std::move(v)), line(l), column(c), length(len) {}
    
    // 获取Token类型的字符串表示
    std::string getTypeString() const;
    
    // 判断是否为关键字
    bool isKeyword() const;
    
    // 判断是否为字面量
    bool isLiteral() const;
    
    // 判断是否为操作符
    bool isOperator() const;
    
    // 判断是否为赋值操作符
    bool isAssignmentOperator() const;
    
    // 判断是否为比较操作符
    bool isComparisonOperator() const;
    
    // 判断是否为逻辑操作符
    bool isLogicalOperator() const;
    
    // 判断是否为CHTL JS特殊功能
    bool isCHTLJSFeature() const;
};

} // namespace CHTLJS

#endif // CHTLJS_TOKEN_H