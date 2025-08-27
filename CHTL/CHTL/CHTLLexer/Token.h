#ifndef CHTL_TOKEN_H
#define CHTL_TOKEN_H

#include <string>
#include <utility>

namespace CHTL {

// Token类型枚举
enum class TokenType {
    // 字面量
    TEXT,                  // 文本内容
    STRING_LITERAL,        // 双引号字符串
    SINGLE_STRING_LITERAL, // 单引号字符串
    UNQUOTED_LITERAL,      // 无引号字面量
    NUMBER,                // 数字
    
    // 标识符
    IDENTIFIER,            // 标识符
    
    // 关键字
    KEYWORD_TEXT,          // text
    KEYWORD_STYLE,         // style
    KEYWORD_SCRIPT,        // script
    KEYWORD_CUSTOM,        // [Custom]
    KEYWORD_TEMPLATE,      // [Template]
    KEYWORD_ORIGIN,        // [Origin]
    KEYWORD_IMPORT,        // [Import]
    KEYWORD_NAMESPACE,     // [Namespace]
    KEYWORD_CONFIGURATION, // [Configuration]
    KEYWORD_INFO,          // [Info]
    KEYWORD_EXPORT,        // [Export]
    KEYWORD_USE,           // use
    KEYWORD_DELETE,        // delete
    KEYWORD_INSERT,        // insert
    KEYWORD_INHERIT,       // inherit
    KEYWORD_FROM,          // from
    KEYWORD_AS,            // as
    KEYWORD_EXCEPT,        // except
    KEYWORD_AFTER,         // after
    KEYWORD_BEFORE,        // before
    KEYWORD_REPLACE,       // replace
    KEYWORD_AT,            // at
    KEYWORD_TOP,           // top
    KEYWORD_BOTTOM,        // bottom
    
    // 符号
    AT,                    // @
    DOT,                   // .
    COLON,                 // :
    SEMICOLON,             // ;
    COMMA,                 // ,
    EQUALS,                // =
    LEFT_BRACE,            // {
    RIGHT_BRACE,           // }
    LEFT_BRACKET,          // [
    RIGHT_BRACKET,         // ]
    LEFT_PAREN,            // (
    RIGHT_PAREN,           // )
    AMPERSAND,             // &
    HASH,                  // #
    
    // 注释
    COMMENT_SINGLE,        // //
    COMMENT_MULTI,         // /* */
    COMMENT_GENERATE,      // --
    
    // 特殊
    NEWLINE,              // 换行
    WHITESPACE,           // 空白
    EOF_TOKEN,            // 文件结束
    UNKNOWN               // 未知token
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
    
    // 判断是否为符号
    bool isSymbol() const;
    
    // 判断是否为注释
    bool isComment() const;
};

} // namespace CHTL

#endif // CHTL_TOKEN_H