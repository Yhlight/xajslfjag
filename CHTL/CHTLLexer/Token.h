#ifndef CHTL_TOKEN_H
#define CHTL_TOKEN_H

#include <string>
#include <unordered_map>

namespace CHTL {

// Token类型枚举
enum class TokenType {
    // 结束标记
    EOF_TOKEN = 0,
    
    // 字面量
    STRING_LITERAL,       // "string" 或 'string'
    UNQUOTED_LITERAL,    // 无引号字面量
    
    // 标识符
    IDENTIFIER,          // 标识符
    
    // 关键字
    TEXT,               // text
    STYLE,              // style
    SCRIPT,             // script
    TEMPLATE,           // [Template]
    CUSTOM,             // [Custom]
    ORIGIN,             // [Origin]
    IMPORT,             // [Import]
    NAMESPACE,          // [Namespace]
    CONFIGURATION,      // [Configuration]
    INFO,               // [Info]
    EXPORT,             // [Export]
    USE,                // use
    
    // 类型标识符
    AT_STYLE,           // @Style
    AT_ELEMENT,         // @Element
    AT_VAR,             // @Var
    AT_HTML,            // @Html
    AT_JAVASCRIPT,      // @JavaScript
    AT_CHTL,            // @Chtl
    AT_CJMOD,           // @CJmod
    AT_CONFIG,          // @Config
    
    // 操作符
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
    ARROW,              // ->
    DOUBLE_LEFT_BRACE,  // {{
    DOUBLE_RIGHT_BRACE, // }}
    AMPERSAND,          // &
    HASH,               // #
    STAR,               // *
    SLASH,              // /
    
    // 注释
    SINGLE_COMMENT,     // // 注释
    MULTI_COMMENT,      // /* */ 注释
    GENERATOR_COMMENT,  // -- 注释
    
    // 特殊
    NEWLINE,            // 换行
    WHITESPACE,         // 空白
    UNKNOWN             // 未知token
};

// Token结构体
struct Token {
    TokenType type;
    std::string value;
    size_t line;
    size_t column;
    size_t position;  // 在源文件中的位置
    
    Token() : type(TokenType::UNKNOWN), line(1), column(1), position(0) {}
    
    Token(TokenType t, const std::string& v, size_t l, size_t c, size_t p)
        : type(t), value(v), line(l), column(c), position(p) {}
    
    bool isKeyword() const;
    bool isLiteral() const;
    bool isOperator() const;
    bool isSymbol() const;
    bool isComment() const;
    bool isAtType() const;
    
    std::string toString() const;
};

// Token类型名称映射
extern const std::unordered_map<TokenType, std::string> TOKEN_TYPE_NAMES;

// 关键字映射
extern const std::unordered_map<std::string, TokenType> KEYWORDS;

// @类型映射
extern const std::unordered_map<std::string, TokenType> AT_TYPES;

} // namespace CHTL

#endif // CHTL_TOKEN_H