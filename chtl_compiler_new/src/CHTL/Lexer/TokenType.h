#pragma once

namespace CHTL {
namespace Lexer {

/**
 * CHTL Token类型
 * 严格按照CHTL语法文档定义
 */
enum class TokenType {
    // 字面量
    UNQUOTED_LITERAL,     // HelloWorld, red, 100px
    DOUBLE_QUOTED_STRING, // "Hello World"
    SINGLE_QUOTED_STRING, // 'Hello World'
    
    // 标识符
    IDENTIFIER,           // 元素名、属性名等
    
    // 关键字
    TEXT,                 // text
    STYLE,                // style
    SCRIPT,               // script
    
    // 块关键字 (需要[]前缀)
    TEMPLATE,             // [Template]
    CUSTOM,               // [Custom]
    CONFIGURATION,        // [Configuration]
    IMPORT,               // [Import]
    NAMESPACE,            // [Namespace]
    ORIGIN,               // [Origin]
    
    // 类型前缀 (需要@前缀)
    AT_ELEMENT,           // @Element
    AT_STYLE,             // @Style
    AT_VAR,               // @Var
    AT_HTML,              // @Html
    AT_JAVASCRIPT,        // @JavaScript
    AT_CHTL,              // @Chtl
    AT_CJMOD,             // @CJmod
    AT_CONFIG,            // @Config
    
    // 符号
    LEFT_BRACE,           // {
    RIGHT_BRACE,          // }
    LEFT_BRACKET,         // [
    RIGHT_BRACKET,        // ]
    LEFT_PAREN,           // (
    RIGHT_PAREN,          // )
    
    COLON,                // :
    SEMICOLON,            // ;
    EQUALS,               // = (CE对等式)
    DOT,                  // .
    HASH,                 // #
    AMPERSAND,            // & (上下文推导)
    
    // 运算符
    DOUBLE_COLON,         // :: (伪元素选择器)
    ARROW,                // -> (CHTL JS语法)
    
    // 导入相关
    FROM,                 // from
    AS,                   // as
    
    // 注释
    LINE_COMMENT,         // // 注释
    BLOCK_COMMENT,        // /* */ 注释
    GENERATOR_COMMENT,    // -- 生成器注释
    
    // 特殊
    NEWLINE,              // 换行
    WHITESPACE,           // 空白字符
    END_OF_FILE,          // 文件结束
    
    // 错误
    UNKNOWN               // 未知token
};

// Token类型转换为字符串
const char* tokenTypeToString(TokenType type);

} // namespace Lexer
} // namespace CHTL