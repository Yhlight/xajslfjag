#pragma once
#include <string>
#include <vector>
#include <memory>

namespace CHTLJS {

/**
 * CHTL JS Token类型枚举
 * 严格按照CHTL语法文档中的CHTL JS语法定义
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
    EQUAL,              // =
    COMMA,              // ,
    DOT,                // .
    ARROW,              // ->
    
    // CHTL JS特有符号
    ENHANCED_SELECTOR_START,    // {{
    ENHANCED_SELECTOR_END,      // }}
    EVENT_BINDING,              // &->
    
    // 字符串和字面量
    DOUBLE_QUOTED_STRING,       // "string"
    SINGLE_QUOTED_STRING,       // 'string'
    UNQUOTED_LITERAL,           // 无修饰字面量
    
    // 注释
    LINE_COMMENT,               // //
    BLOCK_COMMENT,              // /**/
    GENERATOR_COMMENT,          // --
    
    // CHTL JS关键字
    SCRIPT,                     // script
    MODULE,                     // module
    LOAD,                       // load
    LISTEN,                     // listen
    DELEGATE,                   // delegate
    ANIMATE,                    // animate
    VIR,                        // vir
    
    // JavaScript事件关键字
    CLICK,                      // click
    MOUSEENTER,                 // mouseenter
    MOUSELEAVE,                 // mouseleave
    KEYDOWN,                    // keydown
    KEYUP,                      // keyup
    SUBMIT,                     // submit
    CHANGE,                     // change
    FOCUS,                      // focus
    BLUR,                       // blur
    
    // CSS选择器相关
    CLASS_SELECTOR,             // .className
    ID_SELECTOR,                // #idName
    AMPERSAND,                  // &
    
    // 标识符
    IDENTIFIER,                 // 通用标识符
    
    // 特殊Token
    EOF_TOKEN,                  // 文件结束
    UNKNOWN_TOKEN,              // 未知Token
    INVALID_TOKEN               // 无效Token
};

/**
 * Token位置信息
 */
struct Position {
    size_t line;            // 行号
    size_t column;          // 列号
    size_t offset;          // 字符偏移
    
    Position(size_t l = 1, size_t c = 1, size_t o = 0) 
        : line(l), column(c), offset(o) {}
        
    std::string toString() const;
};

/**
 * CHTL JS Token类
 */
class Token {
public:
    TokenType type;             // Token类型
    std::string value;          // Token值
    Position position;          // 位置信息
    
    Token(TokenType t = TokenType::UNKNOWN_TOKEN, 
          const std::string& v = "", 
          const Position& pos = Position())
        : type(t), value(v), position(pos) {}
    
    // 检查Token类型
    bool isKeyword() const;
    bool isOperator() const;
    bool isLiteral() const;
    bool isSelector() const;
    bool isEvent() const;
    
    // 获取字符串表示
    std::string toString() const;
    std::string getTypeName() const;
};

/**
 * Token序列类型
 */
using TokenSequence = std::vector<Token>;

/**
 * Token序列智能指针
 */
using TokenSequencePtr = std::shared_ptr<TokenSequence>;

} // namespace CHTLJS