#pragma once

#include <string>
#include <memory>

namespace CHTL {

/// CHTL词法分析器Token类型枚举
enum class TokenType {
    // 基础符号
    IDENTIFIER,         // 标识符
    STRING_LITERAL,     // 字符串字面量  
    UNQUOTED_LITERAL,   // 无修饰字面量
    NUMBER,             // 数字
    COLOR_VALUE,        // 颜色值 (#rrggbb)
    
    // 关键字
    TEXT,               // text
    STYLE,              // style 
    SCRIPT,             // script
    USE,                // use
    HTML5,              // html5
    INHERIT,            // inherit
    DELETE,             // delete
    INSERT,             // insert
    AFTER,              // after
    BEFORE,             // before
    REPLACE,            // replace
    AT_TOP,             // at top
    AT_BOTTOM,          // at bottom
    FROM,               // from
    AS,                 // as
    EXCEPT,             // except
    
    // 括号类型
    TEMPLATE,           // [Template]
    CUSTOM,             // [Custom]
    ORIGIN,             // [Origin]
    IMPORT,             // [Import]
    NAMESPACE,          // [Namespace]
    CONFIGURATION,      // [Configuration]
    
    // 类型标识符
    AT_STYLE,           // @Style
    AT_ELEMENT,         // @Element
    AT_VAR,             // @Var
    AT_HTML,            // @Html
    AT_JAVASCRIPT,      // @JavaScript
    AT_CHTL,            // @Chtl
    AT_CJMOD,           // @CJmod
    AT_CONFIG,          // @Config
    
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
    
    // 特殊
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

/// CHTL Token位置信息
struct TokenPosition {
    size_t line;        // 行号，从1开始
    size_t column;      // 列号，从1开始
    size_t offset;      // 文件偏移量
    
    TokenPosition(size_t l = 1, size_t c = 1, size_t o = 0) 
        : line(l), column(c), offset(o) {}
};

/// CHTL Token类
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
    
    // 转换为字符串表示
    std::string ToString() const;
    
    // 复制Token
    std::unique_ptr<Token> Clone() const;
};

} // namespace CHTL