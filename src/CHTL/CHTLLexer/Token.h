#pragma once
#include <string>
#include <vector>
#include <memory>

namespace CHTL {

/**
 * CHTL Token类型枚举
 * 严格按照CHTL语法文档定义
 */
enum class TokenType {
    // 基础符号
    LEFT_BRACE,         // {
    RIGHT_BRACE,        // }
    LEFT_BRACKET,       // [
    RIGHT_BRACKET,      // ]
    LEFT_PAREN,         // (
    RIGHT_PAREN,        // )
    SEMICOLON,          // ;
    COLON,              // :
    EQUAL,              // = (CE对等式，与COLON等价)
    COMMA,              // ,
    DOT,                // .
    
    // 字符串和字面量
    DOUBLE_QUOTED_STRING,   // "string"
    SINGLE_QUOTED_STRING,   // 'string'
    UNQUOTED_LITERAL,       // 无修饰字面量
    
    // 注释
    LINE_COMMENT,           // //
    BLOCK_COMMENT,          // /**/
    GENERATOR_COMMENT,      // --
    
    // CHTL关键字
    TEXT,               // text
    STYLE,              // style
    HTML5,              // html5
    USE,                // use
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
    
    // 模板和自定义关键字
    TEMPLATE,           // [Template]
    CUSTOM,             // [Custom]
    ORIGIN,             // [Origin]
    IMPORT,             // [Import]
    NAMESPACE,          // [Namespace]
    CONFIGURATION,      // [Configuration]
    INFO,               // [Info]
    EXPORT,             // [Export]
    NAME,               // [Name]
    ORIGIN_TYPE,        // [OriginType]
    
    // 类型标识符
    AT_STYLE,           // @Style
    AT_ELEMENT,         // @Element
    AT_VAR,             // @Var
    AT_HTML,            // @Html
    AT_JAVASCRIPT,      // @JavaScript
    AT_CONFIG,          // @Config
    AT_CHTL,            // @Chtl
    AT_CJMOD,           // @CJmod
    
    // CSS选择器相关
    CLASS_SELECTOR,     // .className
    ID_SELECTOR,        // #idName
    AMPERSAND,          // & (上下文推导)
    
    // HTML元素名称
    HTML_ELEMENT,       // div, span, p, h1, etc.
    
    // 标识符
    IDENTIFIER,         // 通用标识符
    
    // 索引访问
    INDEX_ACCESS,       // [0], [1], etc.
    
    // 特殊符号
    NEWLINE,            // \n
    WHITESPACE,         // 空白字符
    EOF_TOKEN,          // 文件结束
    
    // 错误处理
    UNKNOWN,            // 未知token
    INVALID             // 无效token
};

/**
 * Token位置信息
 */
struct Position {
    size_t line;        // 行号(从1开始)
    size_t column;      // 列号(从1开始)
    size_t offset;      // 在源码中的偏移量
    
    Position(size_t l = 1, size_t c = 1, size_t o = 0) 
        : line(l), column(c), offset(o) {}
};

/**
 * CHTL Token类
 * 表示CHTL词法分析的最小单元
 */
class Token {
public:
    TokenType type;         // Token类型
    std::string value;      // Token的文本值
    Position position;      // Token在源码中的位置
    
    Token(TokenType t, const std::string& v, const Position& pos)
        : type(t), value(v), position(pos) {}
    
    Token() : type(TokenType::UNKNOWN), value(""), position() {}
    
    // 获取Token类型的字符串表示
    std::string getTypeString() const;
    
    // 判断是否为关键字
    bool isKeyword() const;
    
    // 判断是否为字面量
    bool isLiteral() const;
    
    // 判断是否为注释
    bool isComment() const;
    
    // 判断是否为选择器
    bool isSelector() const;
    
    // 判断是否为模板/自定义类型标识
    bool isTemplateCustomType() const;
    
    // 判断是否为HTML元素
    bool isHtmlElement() const;
    
    // 判断是否为空白字符
    bool isWhitespace() const;
    
    // 判断是否有效
    bool isValid() const;
    
    // 转换为字符串表示
    std::string toString() const;
};

/**
 * Token序列类型
 */
using TokenSequence = std::vector<Token>;
using TokenPtr = std::shared_ptr<Token>;
using TokenSequencePtr = std::shared_ptr<TokenSequence>;

} // namespace CHTL