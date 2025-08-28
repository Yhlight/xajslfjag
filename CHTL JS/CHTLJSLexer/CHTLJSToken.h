#pragma once

#include <string>
#include <memory>

namespace CHTLJS {

// CHTL JS Token类型枚举
enum class CHTLJSTokenType {
    // 基础Token
    END_OF_FILE,
    IDENTIFIER,
    STRING_LITERAL,
    UNQUOTED_LITERAL,
    NUMBER,
    
    // CHTL JS特有Token
    VIRTUAL_OBJECT,      // vir
    ENHANCED_SELECTOR,   // {{...}}
    CHAIN_OPERATOR,      // ->
    EVENT_BINDING,       // &->
    LISTEN_BLOCK,        // listen {...}
    DELEGATE_BLOCK,      // delegate {...}
    ANIMATE_BLOCK,       // animate {...}
    I_NEVER_AWAY,        // iNeverAway {...}
    PRINT_MY_LOVE,       // printMylove {...}
    
    // 关键字
    KEYWORD_VIR,
    KEYWORD_LISTEN,
    KEYWORD_DELEGATE,
    KEYWORD_ANIMATE,
    KEYWORD_TARGET,
    KEYWORD_DURATION,
    KEYWORD_EASING,
    KEYWORD_BEGIN,
    KEYWORD_WHEN,
    KEYWORD_END,
    KEYWORD_LOOP,
    KEYWORD_DIRECTION,
    KEYWORD_DELAY,
    KEYWORD_CALLBACK,
    KEYWORD_AT,
    
    // 分隔符
    LEFT_BRACE,      // {
    RIGHT_BRACE,     // }
    LEFT_BRACKET,    // [
    RIGHT_BRACKET,   // ]
    LEFT_PAREN,      // (
    RIGHT_PAREN,     // )
    SEMICOLON,       // ;
    COLON,           // :
    EQUAL,           // =
    COMMA,           // ,
    DOT,             // .
    AMPERSAND,       // &
    HASH,            // #
    
    // 空白字符
    WHITESPACE,
    NEWLINE,
    
    // 特殊Token
    UNKNOWN
};

// CHTL JS Token类
class CHTLJSToken {
public:
    CHTLJSToken(CHTLJSTokenType type, const std::string& value, size_t line, size_t column);
    
    CHTLJSTokenType getType() const { return type_; }
    const std::string& getValue() const { return value_; }
    size_t getLine() const { return line_; }
    size_t getColumn() const { return column_; }
    
    std::string toString() const;
    
private:
    CHTLJSTokenType type_;
    std::string value_;
    size_t line_;
    size_t column_;
};

// Token工厂函数
std::shared_ptr<CHTLJSToken> createToken(CHTLJSTokenType type, const std::string& value, size_t line, size_t column);

} // namespace CHTLJS