#pragma once

#include <string>
#include <memory>

namespace CHTLJS {

/// CHTL JS词法分析器Token类型枚举
enum class TokenType {
    // 基础符号
    IDENTIFIER,         // 标识符
    STRING_LITERAL,     // 字符串字面量  
    UNQUOTED_LITERAL,   // 无修饰字面量
    NUMBER,             // 数字
    
    // CHTL JS专用关键字
    VIR,                // vir (虚对象)
    LISTEN,             // listen
    DELEGATE,           // delegate
    ANIMATE,            // animate
    TARGET,             // target
    DURATION,           // duration
    EASING,             // easing
    BEGIN,              // begin
    WHEN,               // when
    END,                // end
    LOOP,               // loop
    DIRECTION,          // direction
    DELAY,              // delay
    CALLBACK,           // callback
    AT,                 // at (动画关键字)
    CLICK,              // click
    MOUSEENTER,         // mouseenter
    MOUSELEAVE,         // mouseleave
    MOUSEMOVE,          // mousemove
    
    // 增强选择器
    SELECTOR_START,     // {{
    SELECTOR_END,       // }}
    SELECTOR_CLASS,     // .class格式
    SELECTOR_ID,        // #id格式
    SELECTOR_TAG,       // 标签格式
    SELECTOR_INDEX,     // [index]格式
    
    // CHTL JS操作符
    ARROW,              // ->
    EVENT_BIND,         // &->
    
    // 符号
    LEFT_BRACE,         // {
    RIGHT_BRACE,        // }
    LEFT_BRACKET,       // [
    RIGHT_BRACKET,      // ]
    LEFT_PAREN,         // (
    RIGHT_PAREN,        // )
    SEMICOLON,          // ;
    COLON,              // :
    EQUALS,             // =
    COMMA,              // ,
    DOT,                // .
    HASH,               // #
    AMPERSAND,          // &
    PLUS,               // +
    MINUS,              // -
    STAR,               // *
    SLASH,              // /
    QUESTION,           // ?
    EXCLAMATION,        // !
    
    // 注释
    COMMENT_LINE,       // //
    COMMENT_BLOCK,      // /* */
    COMMENT_GENERATOR,  // --
    
    // 结束符
    EOF_TOKEN,          // 文件结束
    NEWLINE,            // 换行
    WHITESPACE,         // 空白
    
    // 错误
    UNKNOWN             // 未知token
};

/// CHTL JS Token位置信息
struct TokenPosition {
    size_t line;        // 行号，从1开始
    size_t column;      // 列号，从1开始
    size_t offset;      // 文件偏移量
    
    TokenPosition(size_t l = 1, size_t c = 1, size_t o = 0) 
        : line(l), column(c), offset(o) {}
};

/// CHTL JS Token类
class Token {
public:
    TokenType type;
    std::string value;
    TokenPosition position;
    
    Token(TokenType t, const std::string& v, const TokenPosition& pos)
        : type(t), value(v), position(pos) {}
    
    Token(TokenType t, const std::string& v)
        : type(t), value(v), position() {}
    
    Token(TokenType t)
        : type(t), value(), position() {}
    
    // 获取Token类型名称
    std::string GetTypeName() const;
    
    // 判断是否为关键字
    bool IsKeyword() const;
    
    // 判断是否为符号
    bool IsSymbol() const;
    
    // 判断是否为字面量
    bool IsLiteral() const;
    
    // 判断是否为注释
    bool IsComment() const;
    
    // 判断是否为空白字符
    bool IsWhitespace() const;
    
    // 判断是否为选择器相关
    bool IsSelector() const;
    
    // 判断是否为CHTL JS操作符
    bool IsCHTLJSOperator() const;
    
    // 转换为字符串表示
    std::string ToString() const;
    
    // 复制Token
    std::unique_ptr<Token> Clone() const;
};

} // namespace CHTLJS