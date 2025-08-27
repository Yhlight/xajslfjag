#pragma once

#include <string>
#include <iostream>
#include <unordered_map>

namespace CHTL {

// CHTL的Token类型定义
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
    AT,                // @
    HASH,              // #
    AMPERSAND,         // &
    
    // 字面量
    STRING_LITERAL,    // "string" 或 'string'
    UNQUOTED_LITERAL,  // 无引号字符串
    NUMBER,            // 数字
    
    // 标识符
    IDENTIFIER,        // 标识符
    
    // 注释
    SINGLE_COMMENT,    // // 注释
    MULTI_COMMENT,     // /* */ 注释
    GEN_COMMENT,       // -- 生成器注释
    
    // 关键字
    TEXT,              // text
    STYLE,             // style
    SCRIPT,            // script
    INHERIT,           // inherit
    DELETE,            // delete
    INSERT,            // insert
    AFTER,             // after
    BEFORE,            // before
    REPLACE,           // replace
    AT_TOP,            // at top
    AT_BOTTOM,         // at bottom
    FROM,              // from
    AS,                // as
    EXCEPT,            // except
    USE,               // use
    
    // 前缀关键字
    CUSTOM,            // [Custom]
    TEMPLATE,          // [Template]
    ORIGIN,            // [Origin]
    IMPORT,            // [Import]
    NAMESPACE,         // [Namespace]
    CONFIGURATION,     // [Configuration]
    INFO,              // [Info]
    EXPORT,            // [Export]
    NAME,              // [Name]
    ORIGIN_TYPE,       // [OriginType]
    
    // 类型标识符
    TYPE_STYLE,        // @Style
    TYPE_ELEMENT,      // @Element
    TYPE_VAR,          // @Var
    TYPE_HTML,         // @Html
    TYPE_JAVASCRIPT,   // @JavaScript
    TYPE_CONFIG,       // @Config
    TYPE_CHTL,         // @Chtl
    TYPE_CJMOD,        // @CJmod
    
    // HTML元素标签
    HTML_TAG,          // div, span等HTML标签
    
    // 特殊标记
    INVALID,           // 无效token
};

// Token结构体
struct Token {
    TokenType type;
    std::string value;
    int line;
    int column;
    int position;  // 在源文件中的位置
    
    Token() : type(TokenType::INVALID), line(1), column(1), position(0) {}
    
    Token(TokenType t, const std::string& v, int l, int c, int pos)
        : type(t), value(v), line(l), column(c), position(pos) {}
    
    bool IsKeyword() const;
    bool IsTypeIdentifier() const;
    bool IsPrefixKeyword() const;
    bool IsHTMLTag() const;
    
    std::string ToString() const;
};

// Token工具类
class TokenUtils {
public:
    static std::string TokenTypeToString(TokenType type);
    static TokenType StringToTokenType(const std::string& str);
    static bool IsKeyword(const std::string& str);
    static bool IsHTMLTag(const std::string& str);
    static bool IsPrefixKeyword(const std::string& str);
    static bool IsTypeIdentifier(const std::string& str);
    
private:
    static std::unordered_map<std::string, TokenType> keywordMap;
    static std::unordered_map<std::string, TokenType> prefixKeywordMap;
    static std::unordered_map<std::string, TokenType> typeIdentifierMap;
    static std::unordered_map<TokenType, std::string> tokenTypeNameMap;
    
    static void InitializeMaps();
    static bool mapsInitialized;
};

} // namespace CHTL