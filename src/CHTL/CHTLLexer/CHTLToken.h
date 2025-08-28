#pragma once

#include <string>
#include <vector>
#include <memory>

namespace CHTL {

enum class CHTLTokenType {
    // 基础字面量和标识符
    IDENTIFIER = 0,
    STRING_LITERAL,
    NUMBER_LITERAL,
    UNQUOTED_LITERAL,
    
    // 分隔符和运算符
    LBRACE,          // {
    RBRACE,          // }
    LBRACKET,        // [
    RBRACKET,        // ]
    LPAREN,          // (
    RPAREN,          // )
    SEMICOLON,       // ;
    COMMA,           // ,
    COLON,           // :
    EQUALS,          // =
    DOT,             // .
    ARROW,           // ->
    AMPERSAND,       // &
    AMPERSAND_ARROW, // &->
    
    // 注释
    SINGLE_LINE_COMMENT,    // //
    MULTI_LINE_COMMENT,     // /* */
    GENERATOR_COMMENT,      // --
    
    // 关键字
    TEXT,
    STYLE,
    SCRIPT,
    INHERIT,
    DELETE,
    INSERT,
    AFTER,
    BEFORE,
    REPLACE,
    AT_TOP,
    AT_BOTTOM,
    FROM,
    AS,
    EXCEPT,
    USE,
    
    // 标记类型
    TEMPLATE,       // [Template]
    CUSTOM,         // [Custom]
    ORIGIN,         // [Origin]
    IMPORT,         // [Import]
    NAMESPACE,      // [Namespace]
    CONFIGURATION,  // [Configuration]
    INFO,           // [Info]
    EXPORT,         // [Export]
    
    // 类型标记
    AT_STYLE,       // @Style
    AT_ELEMENT,     // @Element
    AT_VAR,         // @Var
    AT_HTML,        // @Html
    AT_JAVASCRIPT,  // @JavaScript
    AT_CHTL,        // @Chtl
    AT_CJMOD,       // @CJmod
    AT_CONFIG,      // @Config
    AT_VUE,         // @Vue (自定义类型示例)
    
    // 选择器引用
    SELECTOR_REF,   // {{selector}}
    CLASS_REF,      // {{.class}}
    ID_REF,         // {{#id}}
    REF_AMPERSAND,  // {{&}}
    
    // HTML5声明
    HTML5,          // html5
    
    // 文件结束
    END_OF_FILE,
    
    // 错误标记
    ERROR_TOKEN,
    
    // 空白符
    WHITESPACE,
    NEWLINE
};

class CHTLToken {
public:
    CHTLTokenType type;
    std::string text;
    size_t line;
    size_t column;
    size_t position;
    
    CHTLToken() = default;
    CHTLToken(CHTLTokenType type, const std::string& text, size_t line, size_t column, size_t position);
    
    // 获取Token类型的字符串表示
    std::string getTypeName() const;
    
    // 判断是否为关键字
    bool isKeyword() const;
    
    // 判断是否为操作符
    bool isOperator() const;
    
    // 判断是否为字面量
    bool isLiteral() const;
    
    // 判断是否为标记
    bool isMarker() const;
    
    // 判断是否为类型标记
    bool isTypeMarker() const;
    
    // 判断是否为选择器引用
    bool isSelectorRef() const;
    
    // 判断是否为注释
    bool isComment() const;
    
    // 判断是否为空白符
    bool isWhitespace() const;
    
    // 调试输出
    std::string toString() const;
};

using CHTLTokenPtr = std::shared_ptr<CHTLToken>;
using CHTLTokenList = std::vector<CHTLTokenPtr>;

} // namespace CHTL