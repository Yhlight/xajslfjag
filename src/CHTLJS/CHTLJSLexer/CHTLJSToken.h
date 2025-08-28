#pragma once

#include <string>
#include <vector>
#include <memory>

namespace CHTLJS {

enum class CHTLJSTokenType {
    // 基础字面量和标识符
    IDENTIFIER = 0,
    STRING_LITERAL,
    NUMBER_LITERAL,
    UNQUOTED_LITERAL,
    
    // 分隔符和运算符
    LBRACE,          // {
    RBRACE,          // }
    LBRACKET,        // [
    RBRACKET,        // ]
    LPAREN,          // (
    RPAREN,          // )
    SEMICOLON,       // ;
    COMMA,           // ,
    COLON,           // :
    EQUALS,          // =
    DOT,             // .
    ARROW,           // ->
    AMPERSAND,       // &
    AMPERSAND_ARROW, // &->
    
    // CHTL JS特有的操作符
    TEMPLATE_LITERAL_START,  // `
    TEMPLATE_LITERAL_END,    // `
    DOLLAR_BRACE,           // ${
    
    // 注释
    SINGLE_LINE_COMMENT,    // //
    MULTI_LINE_COMMENT,     // /* */
    GENERATOR_COMMENT,      // --
    
    // CHTL JS关键字
    MODULE,         // module
    LOAD,           // load
    LISTEN,         // listen
    ANIMATE,        // animate
    DELEGATE,       // delegate
    VIR,            // vir
    TARGET,         // target
    DURATION,       // duration
    EASING,         // easing
    BEGIN,          // begin
    WHEN,           // when
    END,            // end
    LOOP,           // loop
    DIRECTION,      // direction
    DELAY,          // delay
    CALLBACK,       // callback
    AT,             // at
    
    // JavaScript关键字（CHTL JS支持的子集）
    FUNCTION,       // function
    CONST,          // const
    LET,            // let
    VAR,            // var
    IF,             // if
    ELSE,           // else
    FOR,            // for
    WHILE,          // while
    DO,             // do
    BREAK,          // break
    CONTINUE,       // continue
    RETURN,         // return
    
    // 选择器引用（CHTL JS特有）
    SELECTOR_REF,   // {{selector}}
    CLASS_REF,      // {{.class}}
    ID_REF,         // {{#id}}
    REF_AMPERSAND,  // {{&}}
    
    // CHTL JS事件操作符
    EVENT_BIND,     // &->
    
    // 文件结束
    END_OF_FILE,
    
    // 错误标记
    ERROR_TOKEN,
    
    // 空白符
    WHITESPACE,
    NEWLINE
};

class CHTLJSToken {
public:
    CHTLJSTokenType type;
    std::string text;
    size_t line;
    size_t column;
    size_t position;
    
    CHTLJSToken() = default;
    CHTLJSToken(CHTLJSTokenType type, const std::string& text, size_t line, size_t column, size_t position);
    
    // 获取Token类型的字符串表示
    std::string getTypeName() const;
    
    // 判断是否为关键字
    bool isKeyword() const;
    
    // 判断是否为CHTL JS特有关键字
    bool isCHTLJSKeyword() const;
    
    // 判断是否为JavaScript关键字
    bool isJavaScriptKeyword() const;
    
    // 判断是否为操作符
    bool isOperator() const;
    
    // 判断是否为字面量
    bool isLiteral() const;
    
    // 判断是否为选择器引用
    bool isSelectorRef() const;
    
    // 判断是否为注释
    bool isComment() const;
    
    // 判断是否为空白符
    bool isWhitespace() const;
    
    // 调试输出
    std::string toString() const;
};

using CHTLJSTokenPtr = std::shared_ptr<CHTLJSToken>;
using CHTLJSTokenList = std::vector<CHTLJSTokenPtr>;

} // namespace CHTLJS