#pragma once
#include <string>
#include <variant>

namespace CHTL {

/**
 * CHTL Token类型枚举
 * 严格按照CHTL语法文档定义
 */
enum class TokenType {
    // 基础标识符和字面量
    IDENTIFIER,                   // 标识符
    UNQUOTED_LITERAL,            // 无修饰字面量 (无引号字符串)
    DOUBLE_QUOTED_STRING,        // 双引号字符串 ""
    SINGLE_QUOTED_STRING,        // 单引号字符串 ''
    NUMBER,                      // 数字

    // 注释
    LINE_COMMENT,                // // 注释
    BLOCK_COMMENT,               // /* */ 注释  
    GENERATOR_COMMENT,           // -- 生成器识别注释

    // 符号
    LEFT_BRACE,                  // {
    RIGHT_BRACE,                 // }
    LEFT_BRACKET,                // [
    RIGHT_BRACKET,               // ]
    LEFT_PAREN,                  // (
    RIGHT_PAREN,                 // )
    SEMICOLON,                   // ;
    COLON,                       // :
    EQUALS,                      // =
    COMMA,                       // ,
    DOT,                         // .
    SLASH,                       // /
    AMPERSAND,                   // &

    // CHTL关键字 - 基础
    TEXT,                        // text
    STYLE,                       // style
    SCRIPT,                      // script

    // CHTL关键字 - 模板和自定义
    TEMPLATE,                    // [Template]
    CUSTOM,                      // [Custom]
    ORIGIN,                      // [Origin]
    IMPORT,                      // [Import]
    CONFIGURATION,               // [Configuration]
    NAMESPACE,                   // [Namespace]

    // CHTL关键字 - 类型标识符
    AT_STYLE,                    // @Style
    AT_ELEMENT,                  // @Element
    AT_VAR,                      // @Var
    AT_HTML,                     // @Html
    AT_JAVASCRIPT,               // @JavaScript
    AT_CHTL,                     // @Chtl
    AT_CJMOD,                    // @CJmod
    AT_CONFIG,                   // @Config

    // CHTL关键字 - 操作符
    INHERIT,                     // inherit
    DELETE,                      // delete
    INSERT,                      // insert
    AFTER,                       // after
    BEFORE,                      // before
    REPLACE,                     // replace
    AT_TOP,                      // at top
    AT_BOTTOM,                   // at bottom
    FROM,                        // from
    AS,                          // as
    EXCEPT,                      // except

    // CHTL关键字 - use语法
    USE,                         // use
    HTML5,                       // html5

    // HTML元素标签 (动态识别)
    HTML_TAG,                    // div, span, p, html, head, body等

    // CSS选择器相关
    CLASS_SELECTOR,              // .className
    ID_SELECTOR,                 // #idName
    PSEUDO_CLASS,                // :hover, :focus等
    PSEUDO_ELEMENT,              // ::before, ::after等

    // 特殊符号
    EOF_TOKEN,                   // 文件结束
    NEWLINE,                     // 换行符
    WHITESPACE,                  // 空白字符

    // 错误标记
    INVALID                      // 无效token
};

/**
 * Token值类型
 */
using TokenValue = std::variant<std::string, int, double>;

/**
 * 位置信息
 */
struct Position {
    size_t line;
    size_t column;
    size_t offset;
    
    Position(size_t l = 1, size_t c = 1, size_t o = 0) 
        : line(l), column(c), offset(o) {}
};

/**
 * CHTL Token类
 */
class Token {
public:
    TokenType type;
    TokenValue value;
    Position position;
    
    Token(TokenType t, const TokenValue& v, const Position& pos)
        : type(t), value(v), position(pos) {}
    
    Token(TokenType t, const std::string& v, const Position& pos)
        : type(t), value(v), position(pos) {}
    
    Token(TokenType t, const Position& pos)
        : type(t), value(std::string{}), position(pos) {}
    
    // 获取字符串值
    std::string getString() const {
        if (std::holds_alternative<std::string>(value)) {
            return std::get<std::string>(value);
        }
        return "";
    }
    
    // 获取数字值
    double getNumber() const {
        if (std::holds_alternative<int>(value)) {
            return static_cast<double>(std::get<int>(value));
        }
        if (std::holds_alternative<double>(value)) {
            return std::get<double>(value);
        }
        return 0.0;
    }
    
    // 检查是否为特定类型
    bool isType(TokenType t) const {
        return type == t;
    }
    
    // 检查是否为操作符
    bool isOperator() const {
        return type == TokenType::INHERIT || type == TokenType::DELETE || type == TokenType::INSERT ||
               type == TokenType::AFTER || type == TokenType::BEFORE || type == TokenType::REPLACE ||
               type == TokenType::AT_TOP || type == TokenType::AT_BOTTOM;
    }
    
    // 检查是否为关键字
    bool isKeyword() const {
        return type >= TokenType::TEXT && type <= TokenType::HTML5;
    }
    
    // 转换为字符串表示
    std::string toString() const;
};

/**
 * Token类型到字符串的转换
 */
std::string tokenTypeToString(TokenType type);

/**
 * 检查字符串是否为HTML标签
 */
bool isHTMLTag(const std::string& str);

/**
 * 检查字符串是否为CHTL关键字
 */
TokenType getKeywordType(const std::string& str);

} // namespace CHTL