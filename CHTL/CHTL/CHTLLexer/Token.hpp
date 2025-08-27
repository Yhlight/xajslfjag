#pragma once
#include <string>
#include <unordered_map>

namespace CHTL {

/**
 * CHTL词法单元类型枚举
 * 严格按照CHTL语法文档定义
 */
enum class TokenType {
    // 基础类型
    IDENTIFIER,           // 标识符
    STRING_LITERAL,       // 字符串字面量 "" ''
    UNQUOTED_LITERAL,     // 无修饰字面量 
    NUMBER,               // 数字
    
    // 符号
    LBRACE,               // {
    RBRACE,               // }
    LBRACKET,             // [
    RBRACKET,             // ]
    LPAREN,               // (
    RPAREN,               // )
    SEMICOLON,            // ;
    COLON,                // :
    EQUALS,               // = (CE对等式，与:等价)
    COMMA,                // ,
    DOT,                  // .
    SHARP,                // #
    AMPERSAND,            // &
    AT,                   // @
    
    // 注释
    SINGLE_LINE_COMMENT,  // //
    MULTI_LINE_COMMENT,   // /**/
    GENERATOR_COMMENT,    // --
    
    // 关键字
    TEXT,                 // text
    STYLE,                // style (局部样式块)
    SCRIPT,               // script (局部脚本块)
    INHERIT,              // inherit
    DELETE,               // delete
    INSERT,               // insert
    AFTER,                // after
    BEFORE,               // before
    REPLACE,              // replace
    AT_TOP,               // at top
    AT_BOTTOM,            // at bottom
    FROM,                 // from
    AS,                   // as
    EXCEPT,               // except
    USE,                  // use
    HTML5,                // html5
    
    // 块类型标识符
    TEMPLATE,             // [Template]
    CUSTOM,               // [Custom]
    ORIGIN,               // [Origin]
    IMPORT,               // [Import]
    CONFIGURATION,        // [Configuration]
    NAMESPACE,            // [Namespace]
    NAME,                 // [Name]
    ORIGIN_TYPE,          // [OriginType]
    INFO,                 // [Info]
    EXPORT,               // [Export]
    
    // 类型前缀
    AT_STYLE,             // @Style
    AT_ELEMENT,           // @Element
    AT_VAR,               // @Var
    AT_HTML,              // @Html
    AT_JAVASCRIPT,        // @JavaScript
    AT_CHTL,              // @Chtl
    AT_CJMOD,             // @CJmod
    AT_CONFIG,            // @Config
    
    // HTML元素（动态识别）
    HTML_ELEMENT,         // html, head, body, div, span等所有HTML元素
    
    // 特殊符号
    CSS_SELECTOR_PREFIX,  // . 或 # 用于CSS选择器
    
    // 文件结束
    END_OF_FILE,
    
    // 错误
    UNKNOWN
};

/**
 * CHTL词法单元
 */
struct Token {
    TokenType type;
    std::string value;
    size_t line;
    size_t column;
    size_t position;
    
    Token() : type(TokenType::UNKNOWN), line(0), column(0), position(0) {}
    
    Token(TokenType t, const std::string& v, size_t l, size_t c, size_t p)
        : type(t), value(v), line(l), column(c), position(p) {}
        
    bool isKeyword() const;
    bool isOperator() const;
    bool isLiteral() const;
    bool isBlockType() const;
    bool isHTMLElement() const;
    std::string toString() const;
};

/**
 * 关键字映射表
 */
class KeywordMap {
public:
    static const std::unordered_map<std::string, TokenType> keywords;
    static const std::unordered_map<std::string, TokenType> blockTypes;
    static const std::unordered_map<std::string, TokenType> typePrefix;
    
    static TokenType getKeywordType(const std::string& word);
    static TokenType getBlockType(const std::string& word);
    static TokenType getTypePrefix(const std::string& word);
    static bool isValidHTMLElement(const std::string& element);
};

}