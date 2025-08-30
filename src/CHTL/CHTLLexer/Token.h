#ifndef CHTL_TOKEN_H
#define CHTL_TOKEN_H

#include <string>
#include <memory>
#include <variant>

namespace CHTL {

// CHTL词法单元类型
enum class TokenType {
    // 字面量
    STRING_LITERAL,         // "string" 或 'string'
    UNQUOTED_LITERAL,      // 无修饰字面量
    NUMBER_LITERAL,        // 数字
    
    // 标识符
    IDENTIFIER,            // 标识符
    HTML_TAG,              // HTML标签名
    
    // 关键字
    KEYWORD_TEXT,          // text
    KEYWORD_STYLE,         // style
    KEYWORD_SCRIPT,        // script
    KEYWORD_TEMPLATE,      // [Template]
    KEYWORD_CUSTOM,        // [Custom]
    KEYWORD_ORIGIN,        // [Origin]
    KEYWORD_IMPORT,        // [Import]
    KEYWORD_CONFIGURATION, // [Configuration]
    KEYWORD_NAMESPACE,     // [Namespace]
    KEYWORD_INFO,          // [Info]
    KEYWORD_EXPORT,        // [Export]
    KEYWORD_INHERIT,       // inherit
    KEYWORD_DELETE,        // delete
    KEYWORD_INSERT,        // insert
    KEYWORD_USE,           // use
    KEYWORD_EXCEPT,        // except
    KEYWORD_FROM,          // from
    KEYWORD_AS,            // as
    KEYWORD_AFTER,         // after
    KEYWORD_BEFORE,        // before
    KEYWORD_REPLACE,       // replace
    KEYWORD_AT_TOP,        // at top
    KEYWORD_AT_BOTTOM,     // at bottom
    KEYWORD_HTML5,         // html5
    KEYWORD_MODULE,        // module (CJJS)
    KEYWORD_LISTEN,        // listen (CJJS)
    KEYWORD_LOAD,          // load (CJJS)
    
    // 类型标识符
    TYPE_STYLE,            // @Style
    TYPE_ELEMENT,          // @Element
    TYPE_VAR,              // @Var
    TYPE_HTML,             // @Html
    TYPE_JAVASCRIPT,       // @JavaScript
    TYPE_CHTL,             // @Chtl
    TYPE_CJMOD,            // @CJmod
    TYPE_CONFIG,           // @Config
    TYPE_CUSTOM_ORIGIN,    // 自定义原始嵌入类型 @Vue等
    
    // 操作符
    COLON,                 // :
    EQUAL,                 // = (CE对等式)
    SEMICOLON,             // ;
    COMMA,                 // ,
    DOT,                   // .
    AMPERSAND,             // &
    HASH,                  // #
    AT,                    // @
    ARROW,                 // -> (CJJS)
    
    // 括号
    LEFT_BRACE,            // {
    RIGHT_BRACE,           // }
    LEFT_BRACKET,          // [
    RIGHT_BRACKET,         // ]
    LEFT_PAREN,            // (
    RIGHT_PAREN,           // )
    
    // 注释
    SINGLE_LINE_COMMENT,   // //
    MULTI_LINE_COMMENT,    // /* */
    GENERATOR_COMMENT,     // --
    
    // 特殊符号
    WHITESPACE,            // 空白字符
    NEWLINE,               // 换行
    EOF_TOKEN,             // 文件结束
    
    // 错误
    UNKNOWN                // 未知词法单元
};

// Token位置信息
struct TokenLocation {
    size_t line;
    size_t column;
    size_t offset;     // 在源文件中的字节偏移
    size_t length;     // Token长度
    
    TokenLocation(size_t l = 1, size_t c = 1, size_t o = 0, size_t len = 0)
        : line(l), column(c), offset(o), length(len) {}
};

// Token值类型
using TokenValue = std::variant<
    std::monostate,    // 无值
    std::string,       // 字符串值
    int64_t,           // 整数值
    double             // 浮点数值
>;

// Token类
class Token {
public:
    Token(TokenType type, const std::string& lexeme, 
          const TokenLocation& loc, const TokenValue& value = std::monostate{})
        : type_(type), lexeme_(lexeme), location_(loc), value_(value) {}
    
    // 获取Token类型
    TokenType getType() const { return type_; }
    
    // 获取Token的原始文本
    const std::string& getLexeme() const { return lexeme_; }
    
    // 获取Token位置
    const TokenLocation& getLocation() const { return location_; }
    
    // 获取Token值
    const TokenValue& getValue() const { return value_; }
    
    // 判断是否为关键字
    bool isKeyword() const {
        return type_ >= TokenType::KEYWORD_TEXT && 
               type_ <= TokenType::KEYWORD_HTML5;
    }
    
    // 判断是否为类型标识符
    bool isTypeIdentifier() const {
        return type_ >= TokenType::TYPE_STYLE && 
               type_ <= TokenType::TYPE_CUSTOM_ORIGIN;
    }
    
    // 判断是否为字面量
    bool isLiteral() const {
        return type_ == TokenType::STRING_LITERAL ||
               type_ == TokenType::UNQUOTED_LITERAL ||
               type_ == TokenType::NUMBER_LITERAL;
    }
    
    // 判断是否为注释
    bool isComment() const {
        return type_ == TokenType::SINGLE_LINE_COMMENT ||
               type_ == TokenType::MULTI_LINE_COMMENT ||
               type_ == TokenType::GENERATOR_COMMENT;
    }
    
    // 获取字符串表示
    std::string toString() const;
    
    // 静态工厂方法
    static std::shared_ptr<Token> createEOF(const TokenLocation& loc) {
        return std::make_shared<Token>(TokenType::EOF_TOKEN, "", loc);
    }
    
    static std::shared_ptr<Token> createUnknown(const std::string& lexeme, 
                                                const TokenLocation& loc) {
        return std::make_shared<Token>(TokenType::UNKNOWN, lexeme, loc);
    }

private:
    TokenType type_;
    std::string lexeme_;
    TokenLocation location_;
    TokenValue value_;
};

// Token类型名称映射
const char* getTokenTypeName(TokenType type);

// 判断字符串是否为CHTL关键字
TokenType getKeywordType(const std::string& keyword);

// 判断字符串是否为类型标识符
TokenType getTypeIdentifierType(const std::string& typeId);

} // namespace CHTL

#endif // CHTL_TOKEN_H