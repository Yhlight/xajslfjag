#ifndef CHTL_TOKEN_H
#define CHTL_TOKEN_H

#include <string>
#include <memory>

namespace CHTL {

// Token类型枚举，覆盖所有CHTL语法
enum class TokenType {
    // 基本符号
    LBRACE,         // {
    RBRACE,         // }
    LPAREN,         // (
    RPAREN,         // )
    LBRACKET,       // [
    RBRACKET,       // ]
    SEMICOLON,      // ;
    COLON,          // :
    EQUAL,          // =
    COMMA,          // ,
    DOT,            // .
    SLASH,          // /
    STAR,           // *
    AMPERSAND,      // &
    
    // 字符串和字面量
    STRING_LITERAL,     // "string" 或 'string'
    UNQUOTED_LITERAL,   // 无修饰字面量
    NUMBER,             // 数字
    
    // 注释
    LINE_COMMENT,       // //
    BLOCK_COMMENT,      // /* */
    GENERATOR_COMMENT,  // --
    
    // HTML元素
    HTML_TAG,           // html, body, div, span等
    TEXT_KEYWORD,       // text
    STYLE_KEYWORD,      // style
    SCRIPT_KEYWORD,     // script
    
    // 关键字前缀
    TEMPLATE_PREFIX,    // [Template]
    CUSTOM_PREFIX,      // [Custom]
    ORIGIN_PREFIX,      // [Origin]
    IMPORT_PREFIX,      // [Import]
    NAMESPACE_PREFIX,   // [Namespace]
    CONFIGURATION_PREFIX, // [Configuration]
    INFO_PREFIX,        // [Info]
    EXPORT_PREFIX,      // [Export]
    ORIGINTYPE_PREFIX,  // [OriginType]
    NAME_PREFIX,        // [Name]
    
    // 类型标识符
    AT_STYLE,           // @Style
    AT_ELEMENT,         // @Element
    AT_VAR,             // @Var
    AT_HTML,            // @Html
    AT_JAVASCRIPT,      // @JavaScript
    AT_CHTL,            // @Chtl
    AT_CJMOD,           // @CJmod
    AT_CONFIG,          // @Config
    
    // 操作关键字
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
    
    // 选择器
    CLASS_SELECTOR,     // .className
    ID_SELECTOR,        // #idName
    ELEMENT_SELECTOR,   // elementName
    
    // 索引访问
    INDEX_ACCESS,       // [index]
    
    // use关键字
    USE,                // use
    HTML5,              // html5
    
    // 特殊符号
    HASH,               // #
    DOUBLE_COLON,       // ::
    ARROW,              // ->
    WILDCARD,           // *
    
    // 配置相关
    CONFIG_IDENTIFIER,  // 配置项名称
    CONFIG_VALUE,       // 配置项值
    
    // 占位符和特殊
    PLACEHOLDER,        // $ (在某些上下文中)
    
    // 文件结束
    EOF_TOKEN,
    
    // 错误
    INVALID,
    
    // 标识符
    IDENTIFIER,         // 通用标识符
    
    // 路径相关
    PATH_SEPARATOR,     // / 或 .
    FILE_EXTENSION,     // .chtl, .cmod等
    
    // 字符串连接
    DOUBLE_STAR,        // **
    
    // 新行和空白
    NEWLINE,
    WHITESPACE
};

// Token类
class Token {
public:
    TokenType type;
    std::string value;
    size_t line;
    size_t column;
    size_t startPos;
    size_t endPos;
    
    Token(TokenType t = TokenType::INVALID, 
          const std::string& val = "", 
          size_t ln = 0, 
          size_t col = 0,
          size_t start = 0,
          size_t end = 0)
        : type(t), value(val), line(ln), column(col), startPos(start), endPos(end) {}
    
    // 获取Token类型的字符串表示
    std::string getTypeName() const;
    
    // 判断是否为关键字
    bool isKeyword() const;
    
    // 判断是否为类型标识符
    bool isTypeIdentifier() const;
    
    // 判断是否为选择器
    bool isSelector() const;
    
    // 判断是否为前缀关键字
    bool isPrefix() const;
    
    // 判断是否为操作关键字
    bool isOperationKeyword() const;
};

} // namespace CHTL

#endif // CHTL_TOKEN_H