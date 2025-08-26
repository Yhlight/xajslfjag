#ifndef CHTL_TOKEN_H
#define CHTL_TOKEN_H

#include <string>
#include <variant>

namespace chtl {

// Token类型枚举
enum class TokenType {
    // 字面量
    IDENTIFIER,          // 标识符（元素名、属性名等）
    STRING_LITERAL,      // 字符串字面量（带引号）
    UNQUOTED_LITERAL,    // 无修饰字面量
    NUMBER,              // 数字
    
    // 符号
    LEFT_BRACE,          // {
    RIGHT_BRACE,         // }
    LEFT_BRACKET,        // [
    RIGHT_BRACKET,       // ]
    LEFT_PAREN,          // (
    RIGHT_PAREN,         // )
    SEMICOLON,           // ;
    COLON,               // :
    EQUALS,              // =
    COMMA,               // ,
    DOT,                 // .
    SLASH,               // /
    AMPERSAND,           // &
    AT,                  // @
    HASH,                // #
    DOLLAR,              // $
    
    // 注释
    COMMENT,             // // 和 /* */ 注释
    GENERATOR_COMMENT,   // -- 生成器注释
    
    // 关键字 - 块定义
    TEMPLATE,            // [Template]
    CUSTOM,              // [Custom]
    ORIGIN,              // [Origin]
    IMPORT,              // [Import]
    NAMESPACE,           // [Namespace]
    CONFIGURATION,       // [Configuration]
    INFO,                // [Info]
    EXPORT,              // [Export]
    
    // 关键字 - 类型
    TYPE_STYLE,          // @Style
    TYPE_ELEMENT,        // @Element
    TYPE_VAR,            // @Var
    TYPE_HTML,           // @Html
    TYPE_JAVASCRIPT,     // @JavaScript
    TYPE_CHTL,           // @Chtl
    TYPE_CJMOD,          // @CJmod
    TYPE_CONFIG,         // @Config
    
    // 关键字 - 元素
    TEXT,                // text
    STYLE,               // style (局部样式块)
    SCRIPT,              // script (局部脚本块)
    
    // 关键字 - 操作
    INHERIT,             // inherit
    DELETE,              // delete
    INSERT,              // insert
    AFTER,               // after
    BEFORE,              // before
    REPLACE,             // replace
    AT_TOP,              // at top
    AT_BOTTOM,           // at bottom
    FROM,                // from
    AS,                  // as
    EXCEPT,              // except
    
    // 关键字 - 特殊
    USE,                 // use
    HTML5,               // html5
    VIR,                 // vir (虚对象，但这是CHTL JS的)
    
    // 特殊Token
    EOF_TOKEN,           // 文件结束
    UNKNOWN,             // 未知Token
    
    // 行和块边界标记
    NEWLINE,             // 换行符
    INDENT,              // 缩进增加
    DEDENT,              // 缩进减少
};

// Token位置信息
struct TokenLocation {
    size_t line;
    size_t column;
    size_t offset;       // 在源文件中的偏移量
    std::string file;    // 源文件路径
    
    TokenLocation() : line(1), column(1), offset(0) {}
    TokenLocation(size_t l, size_t c, size_t o = 0, const std::string& f = "")
        : line(l), column(c), offset(o), file(f) {}
};

// Token值类型
using TokenValue = std::variant<
    std::monostate,      // 无值
    std::string,         // 字符串值
    long long,           // 整数值
    double              // 浮点数值
>;

// Token结构
struct Token {
    TokenType type;
    TokenValue value;
    std::string text;    // 原始文本
    TokenLocation location;
    
    Token() : type(TokenType::UNKNOWN) {}
    Token(TokenType t, const std::string& txt = "", const TokenLocation& loc = TokenLocation())
        : type(t), text(txt), location(loc) {}
    Token(TokenType t, const TokenValue& val, const std::string& txt, const TokenLocation& loc)
        : type(t), value(val), text(txt), location(loc) {}
    
    // 辅助方法
    bool isKeyword() const;
    bool isLiteral() const;
    bool isSymbol() const;
    bool isTypeKeyword() const;
    bool isBlockKeyword() const;
    
    std::string toString() const;
    std::string getStringValue() const;
    long long getIntValue() const;
    double getFloatValue() const;
};

// Token工具类
class TokenUtils {
public:
    static std::string tokenTypeToString(TokenType type);
    static TokenType stringToTokenType(const std::string& str);
    static bool isKeyword(const std::string& str);
    static TokenType getKeywordType(const std::string& str);
};

} // namespace chtl

#endif // CHTL_TOKEN_H