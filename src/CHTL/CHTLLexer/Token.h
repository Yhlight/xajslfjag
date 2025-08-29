#pragma once

#include "../../Util/Common.h"

namespace CHTL {

// Token类型枚举，严格按照CHTL语法文档定义
enum class TokenType {
    // 基础标识符和字面量
    IDENTIFIER,           // 标识符
    LITERAL_STRING,       // 字符串字面量 "text" 或 'text'
    LITERAL_UNQUOTED,     // 无修饰字面量 text
    LITERAL_NUMBER,       // 数字字面量
    
    // HTML元素
    HTML_ELEMENT,         // HTML元素名 (div, span, html, body等)
    
    // 特殊关键字
    TEXT,                 // text
    STYLE,                // style
    SCRIPT,               // script
    
    // 模板系统
    TEMPLATE,             // [Template]
    CUSTOM,               // [Custom]
    AT_STYLE,             // @Style
    AT_ELEMENT,           // @Element
    AT_VAR,               // @Var
    
    // 原始嵌入
    ORIGIN,               // [Origin]
    AT_HTML,              // @Html
    AT_JAVASCRIPT,        // @JavaScript
    
    // 导入系统
    IMPORT,               // [Import]
    FROM,                 // from
    AS,                   // as
    EXCEPT,               // except
    CHTL,                 // @Chtl
    CJMOD,                // @CJmod
    CONFIG,               // @Config
    
    // 配置系统
    CONFIGURATION,        // [Configuration]
    NAME,                 // [Name]
    ORIGIN_TYPE,          // [OriginType]
    
    // 命名空间
    NAMESPACE,            // [Namespace]
    
    // 操作符
    INHERIT,              // inherit
    DELETE,               // delete
    INSERT,               // insert
    AFTER,                // after
    BEFORE,               // before
    REPLACE,              // replace
    AT_TOP,               // at top
    AT_BOTTOM,            // at bottom
    USE,                  // use
    HTML5,                // html5
    
    // 符号
    LBRACE,               // {
    RBRACE,               // }
    LBRACKET,             // [
    RBRACKET,             // ]
    LPAREN,               // (
    RPAREN,               // )
    SEMICOLON,            // ;
    COLON,                // :
    EQUALS,               // =
    COMMA,                // ,
    DOT,                  // .
    HASH,                 // #
    AMPERSAND,            // &
    ASTERISK,             // *
    SLASH,                // /
    QUESTION,             // ?
    EXCLAMATION,          // !
    UNDERSCORE,           // _
    ELLIPSIS,             // ...
    DOLLAR,               // $
    
    // 通用分类 (为了向后兼容测试代码)
    OPERATOR,             // 操作符通用类型
    KEYWORD,              // 关键字通用类型  
    PUNCTUATION,          // 标点符号通用类型
    
    // 特殊符号组合
    ARROW,                // ->
    DOUBLE_COLON,         // ::
    PSEUDO_HOVER,         // :hover
    PSEUDO_BEFORE,        // ::before
    PSEUDO_AFTER,         // ::after
    
    // 索引访问
    INDEX_ACCESS,         // [0], [1], etc.
    
    // CSS选择器相关
    CLASS_SELECTOR,       // .class
    ID_SELECTOR,          // #id
    
    // CHTL JS 特殊语法
    DOUBLE_LBRACE,        // {{
    DOUBLE_RBRACE,        // }}
    EVENT_BIND,           // &->
    MODULE,               // module
    LISTEN,               // listen
    DELEGATE,             // delegate
    ANIMATE,              // animate
    VIR,                  // vir
    LOAD,                 // load
    
    // 注释
    COMMENT_LINE,         // //
    COMMENT_BLOCK,        // /* */
    COMMENT_GENERATOR,    // --
    
    // 空白和换行
    WHITESPACE,
    NEWLINE,
    
    // 特殊
    EOF_TOKEN,
    UNKNOWN
};

// 位置信息
struct Position {
    size_t line = 1;
    size_t column = 1;
    size_t offset = 0;
    
    Position() = default;
    Position(size_t l, size_t c, size_t o) : line(l), column(c), offset(o) {}
};

// Token类
class Token {
public:
    TokenType type;
    String value;
    Position position;
    
    Token() : type(TokenType::UNKNOWN) {}
    Token(TokenType t, const String& v, const Position& pos = Position())
        : type(t), value(v), position(pos) {}
    
    // Getter方法
    TokenType getType() const { return type; }
    const String& getValue() const { return value; }
    const Position& getPosition() const { return position; }
    
    // 工具方法
    bool isKeyword() const;
    bool isOperator() const;
    bool isLiteral() const;
    bool isIdentifier() const;
    bool isSymbol() const;
    bool isComment() const;
    bool isWhitespace() const;
    
    String toString() const;
    static String tokenTypeToString(TokenType type);
};

} // namespace CHTL