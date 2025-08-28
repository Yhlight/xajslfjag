#pragma once
#include <string>
#include <vector>
#include <memory>

namespace CHTLJS {

// CHTL JS词法单元类型 - 完全独立于CHTL
enum class CHTLJSTokenType {
    // 基础标识符和文字
    IDENTIFIER,
    STRING_LITERAL,
    UNQUOTED_LITERAL,   // 无修饰字面量
    NUMERIC_LITERAL,
    
    // CHTL JS特有关键字
    MODULE,             // module
    LOAD,               // load
    LISTEN,             // listen
    DELEGATE,           // delegate
    ANIMATE,            // animate
    VIR,                // vir (虚对象)
    TARGET,             // target
    DURATION,           // duration
    EASING,             // easing
    BEGIN,              // begin
    WHEN,               // when
    END,                // end
    LOOP,               // loop
    DIRECTION,          // direction
    DELAY,              // delay
    CALLBACK,           // callback
    AT,                 // at (动画关键帧)
    
    // 选择器相关
    SELECTOR_START,     // {{
    SELECTOR_END,       // }}
    CLASS_SELECTOR,     // .classname
    ID_SELECTOR,        // #idname
    REFERENCE_SELECTOR, // &
    
    // 符号和操作符
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
    ARROW,              // ->
    EVENT_BIND,         // &->
    
    // JavaScript基础语法元素
    FUNCTION,           // function
    CONST,              // const
    LET,                // let
    VAR,                // var
    IF,                 // if
    ELSE,               // else
    FOR,                // for
    WHILE,              // while
    RETURN,             // return
    
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

// CHTL JS词法单元
class CHTLJSToken {
public:
    CHTLJSTokenType type;
    std::string value;
    size_t line;
    size_t column;
    size_t position;
    
    CHTLJSToken(CHTLJSTokenType type = CHTLJSTokenType::UNKNOWN, 
                const std::string& value = "",
                size_t line = 0, 
                size_t column = 0, 
                size_t position = 0);
                
    // 判断是否为特定类型
    bool Is(CHTLJSTokenType type) const;
    bool IsKeyword() const;
    bool IsOperator() const;
    bool IsSelector() const;
    bool IsComment() const;
    bool IsCHTLJSSpecific() const;
    
    // 获取字符串表示
    std::string ToString() const;
    static std::string TypeToString(CHTLJSTokenType type);
};

using CHTLJSTokenPtr = std::shared_ptr<CHTLJSToken>;

} // namespace CHTLJS