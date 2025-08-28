#pragma once
#include <string>
#include <vector>
#include <memory>

namespace CHTL {

// CHTL词法单元类型
enum class CHTLTokenType {
    // 基础标识符和文字
    IDENTIFIER,
    STRING_LITERAL,
    UNQUOTED_LITERAL,   // 无修饰字面量
    NUMERIC_LITERAL,
    
    // 关键字组
    // HTML元素相关
    HTML_ELEMENT,       // html, div, span等
    TEXT,               // text
    STYLE,              // style (局部样式块)
    SCRIPT,             // script (局部脚本块)
    
    // 模板相关
    TEMPLATE,           // [Template]
    CUSTOM,             // [Custom]
    STYLE_TEMPLATE,     // @Style
    ELEMENT_TEMPLATE,   // @Element  
    VAR_TEMPLATE,       // @Var
    
    // 原始嵌入相关
    ORIGIN,             // [Origin]
    ORIGIN_HTML,        // @Html
    ORIGIN_STYLE,       // @Style
    ORIGIN_JAVASCRIPT,  // @JavaScript
    
    // 配置相关
    CONFIGURATION,      // [Configuration]
    NAME_SECTION,       // [Name]
    ORIGIN_TYPE_SECTION,// [OriginType]
    
    // 导入相关
    IMPORT,             // [Import]
    FROM,               // from
    AS,                 // as
    EXCEPT,             // except
    
    // 命名空间相关
    NAMESPACE,          // [Namespace]
    
    // 操作符
    INHERIT,            // inherit
    DELETE,             // delete
    INSERT,             // insert
    AFTER,              // after
    BEFORE,             // before
    REPLACE,            // replace
    AT_TOP,             // at top
    AT_BOTTOM,          // at bottom
    
    // 选择器相关
    CLASS_SELECTOR,     // .classname
    ID_SELECTOR,        // #idname
    REFERENCE_SELECTOR, // & 
    
    // 符号
    LEFT_BRACE,         // {
    RIGHT_BRACE,        // }
    LEFT_BRACKET,       // [
    RIGHT_BRACKET,      // ]
    LEFT_PAREN,         // (
    RIGHT_PAREN,        // )
    SEMICOLON,          // ;
    COMMA,              // ,
    COLON,              // :
    EQUALS,             // =
    DOT,                // .
    HASH,               // #
    AMPERSAND,          // &
    ASTERISK,           // *
    SLASH,              // /
    DOUBLE_COLON,       // ::
    ARROW,              // ->
    
    // 注释
    SINGLE_LINE_COMMENT,// //
    MULTI_LINE_COMMENT, // /* */
    GENERATOR_COMMENT,  // --
    
    // 特殊
    WHITESPACE,
    NEWLINE,
    EOF_TOKEN,
    UNKNOWN
};

// CHTL词法单元
class CHTLToken {
public:
    CHTLTokenType type;
    std::string value;
    size_t line;
    size_t column;
    size_t position;
    
    CHTLToken(CHTLTokenType type = CHTLTokenType::UNKNOWN, 
              const std::string& value = "",
              size_t line = 0, 
              size_t column = 0, 
              size_t position = 0);
              
    // 判断是否为特定类型
    bool Is(CHTLTokenType type) const;
    bool IsKeyword() const;
    bool IsOperator() const;
    bool IsSelector() const;
    bool IsComment() const;
    
    // 获取字符串表示
    std::string ToString() const;
    static std::string TypeToString(CHTLTokenType type);
};

using CHTLTokenPtr = std::shared_ptr<CHTLToken>;

} // namespace CHTL