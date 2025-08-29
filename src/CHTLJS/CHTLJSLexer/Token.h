#ifndef CHTLJS_TOKEN_H
#define CHTLJS_TOKEN_H

#include <string>
#include <memory>
#include <variant>

namespace CHTLJS {

// CHTL JS词法单元类型
enum class TokenType {
    // 字面量
    STRING_LITERAL,         // "string" 或 'string'
    UNQUOTED_LITERAL,      // 无修饰字面量
    NUMBER_LITERAL,        // 数字
    BOOLEAN_LITERAL,       // true/false
    NULL_LITERAL,          // null
    
    // 标识符
    IDENTIFIER,            // 标识符
    
    // CHTL JS特有关键字
    KEYWORD_MODULE,        // module
    KEYWORD_LOAD,          // load
    KEYWORD_LISTEN,        // listen
    KEYWORD_DELEGATE,      // delegate
    KEYWORD_ANIMATE,       // animate
    KEYWORD_VIR,           // vir
    KEYWORD_INEVERAWAY,    // iNeverAway
    
    // 动画相关关键字
    KEYWORD_TARGET,        // target
    KEYWORD_DURATION,      // duration
    KEYWORD_EASING,        // easing
    KEYWORD_BEGIN,         // begin
    KEYWORD_WHEN,          // when
    KEYWORD_END,           // end
    KEYWORD_AT,            // at
    KEYWORD_LOOP,          // loop
    KEYWORD_DIRECTION,     // direction
    KEYWORD_DELAY,         // delay
    KEYWORD_CALLBACK,      // callback
    
    // JavaScript关键字（基础）
    KEYWORD_CONST,         // const
    KEYWORD_LET,           // let
    KEYWORD_VAR,           // var
    KEYWORD_FUNCTION,      // function
    KEYWORD_RETURN,        // return
    KEYWORD_IF,            // if
    KEYWORD_ELSE,          // else
    KEYWORD_FOR,           // for
    KEYWORD_WHILE,         // while
    KEYWORD_THIS,          // this
    
    // 操作符
    ARROW,                 // ->
    EVENT_BIND,            // &->
    DOUBLE_LEFT_BRACE,     // {{
    DOUBLE_RIGHT_BRACE,    // }}
    COLON,                 // :
    SEMICOLON,             // ;
    COMMA,                 // ,
    DOT,                   // .
    AMPERSAND,             // &
    EQUAL,                 // =
    PLUS,                  // +
    MINUS,                 // -
    MULTIPLY,              // *
    DIVIDE,                // /
    MODULO,                // %
    
    // 比较操作符
    EQUAL_EQUAL,           // ==
    NOT_EQUAL,             // !=
    LESS_THAN,             // <
    GREATER_THAN,          // >
    LESS_EQUAL,            // <=
    GREATER_EQUAL,         // >=
    
    // 逻辑操作符
    AND,                   // &&
    OR,                    // ||
    NOT,                   // !
    
    // 括号
    LEFT_BRACE,            // {
    RIGHT_BRACE,           // }
    LEFT_BRACKET,          // [
    RIGHT_BRACKET,         // ]
    LEFT_PAREN,            // (
    RIGHT_PAREN,           // )
    
    // 增强选择器相关
    SELECTOR_CLASS,        // .className
    SELECTOR_ID,           // #id
    SELECTOR_TAG,          // tagName
    SELECTOR_COMPOUND,     // 复合选择器
    SELECTOR_REF,          // & 引用选择器
    
    // 注释
    SINGLE_LINE_COMMENT,   // //
    MULTI_LINE_COMMENT,    // /* */
    
    // 特殊符号
    WHITESPACE,            // 空白字符
    NEWLINE,               // 换行
    EOF_TOKEN,             // 文件结束
    
    // 错误
    UNKNOWN                // 未知词法单元
};

// Token位置信息（复用CHTL的定义）
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
    double,            // 浮点数值
    bool               // 布尔值
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
    
    // 判断是否为CHTL JS关键字
    bool isCHTLJSKeyword() const {
        return type_ >= TokenType::KEYWORD_MODULE && 
               type_ <= TokenType::KEYWORD_CALLBACK;
    }
    
    // 判断是否为JavaScript关键字
    bool isJSKeyword() const {
        return type_ >= TokenType::KEYWORD_CONST && 
               type_ <= TokenType::KEYWORD_THIS;
    }
    
    // 判断是否为字面量
    bool isLiteral() const {
        return type_ >= TokenType::STRING_LITERAL &&
               type_ <= TokenType::NULL_LITERAL;
    }
    
    // 判断是否为选择器
    bool isSelector() const {
        return type_ >= TokenType::SELECTOR_CLASS &&
               type_ <= TokenType::SELECTOR_REF;
    }
    
    // 判断是否为注释
    bool isComment() const {
        return type_ == TokenType::SINGLE_LINE_COMMENT ||
               type_ == TokenType::MULTI_LINE_COMMENT;
    }
    
    // 判断是否为操作符
    bool isOperator() const {
        return (type_ >= TokenType::ARROW && type_ <= TokenType::MODULO) ||
               (type_ >= TokenType::EQUAL_EQUAL && type_ <= TokenType::NOT);
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

// 判断字符串是否为CHTL JS关键字
TokenType getCHTLJSKeywordType(const std::string& keyword);

// 判断字符串是否为JavaScript关键字
TokenType getJSKeywordType(const std::string& keyword);

} // namespace CHTLJS

#endif // CHTLJS_TOKEN_H