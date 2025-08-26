#ifndef CHTLJS_TOKEN_H
#define CHTLJS_TOKEN_H

#include <string>
#include <variant>
#include "../chtl/CHTLToken.h"

namespace chtl {
namespace js {

// CHTL JS特有的Token类型
enum class CHTLJSTokenType {
    // 继承CHTL基础Token类型
    BASE_TOKEN = 0,
    
    // CHTL JS特有符号
    DOUBLE_LEFT_BRACE,      // {{
    DOUBLE_RIGHT_BRACE,     // }}
    ARROW,                  // ->
    
    // CHTL JS关键字
    VIR,                    // vir
    LISTEN,                 // listen
    DELEGATE,               // delegate
    ANIMATE,                // animate
    
    // CJMOD扩展函数
    PRINT_MY_LOVE,          // printMylove
    I_NEVER_AWAY,           // iNeverAway
    
    // 选择器类型
    SELECTOR_CLASS,         // .className
    SELECTOR_ID,            // #id
    SELECTOR_TAG,           // tagName
    SELECTOR_COMPOUND,      // 复合选择器
    SELECTOR_INDEX,         // [index]
    
    // 事件类型
    EVENT_NAME,             // click, mouseenter等
    
    // 动画相关
    DURATION,               // 动画持续时间
    EASING,                 // 缓动函数
    AT,                     // at关键字（动画时间点）
    LOOP,                   // loop关键字
    DIRECTION,              // direction关键字
    DELAY,                  // delay关键字
    BEGIN,                  // begin关键字
    END,                    // end关键字
    WHEN,                   // when关键字
    CALLBACK,               // callback关键字
    TARGET,                 // target关键字
    
    // JavaScript基础Token（简化版）
    FUNCTION,               // function
    CONST,                  // const
    LET,                    // let
    VAR,                    // var
    IF,                     // if
    ELSE,                   // else
    FOR,                    // for
    WHILE,                  // while
    RETURN,                 // return
    NEW,                    // new
    THIS,                   // this
    
    // 操作符
    PLUS,                   // +
    MINUS,                  // -
    MULTIPLY,               // *
    DIVIDE,                 // /
    MODULO,                 // %
    ASSIGN,                 // =
    PLUS_ASSIGN,            // +=
    MINUS_ASSIGN,           // -=
    EQUAL,                  // ==
    NOT_EQUAL,              // !=
    STRICT_EQUAL,           // ===
    STRICT_NOT_EQUAL,       // !==
    LESS_THAN,              // <
    GREATER_THAN,           // >
    LESS_EQUAL,             // <=
    GREATER_EQUAL,          // >=
    LOGICAL_AND,            // &&
    LOGICAL_OR,             // ||
    LOGICAL_NOT,            // !
    
    // 特殊值
    TRUE,                   // true
    FALSE,                  // false
    NULL_VALUE,             // null
    UNDEFINED,              // undefined
    
    // 扩展Token
    REGEX,                  // 正则表达式
    TEMPLATE_STRING,        // 模板字符串
    SPREAD,                 // ...
    ARROW_FUNCTION,         // =>
    
    // CHTL JS特殊功能
    PLACEHOLDER             // $ 占位符
};

// CHTL JS Token结构
struct CHTLJSToken : public Token {
    CHTLJSTokenType jsType;
    
    // 选择器相关信息
    std::string selectorType;    // class/id/tag
    std::string selectorName;    // 选择器名称
    int selectorIndex = -1;      // 选择器索引
    
    // 事件相关信息
    std::string eventType;       // 事件类型
    
    // 动画相关信息
    double animationValue = 0.0; // 动画数值（时间、进度等）
    
    CHTLJSToken() : Token(), jsType(CHTLJSTokenType::BASE_TOKEN) {}
    
    CHTLJSToken(TokenType baseType, const std::string& txt = "", 
                const TokenLocation& loc = TokenLocation())
        : Token(baseType, txt, loc), jsType(CHTLJSTokenType::BASE_TOKEN) {}
    
    CHTLJSToken(CHTLJSTokenType jsType, const std::string& txt = "", 
                const TokenLocation& loc = TokenLocation())
        : Token(TokenType::UNKNOWN, txt, loc), jsType(jsType) {}
    
    // 辅助方法
    bool isSelector() const;
    bool isEvent() const;
    bool isAnimationKeyword() const;
    bool isCHTLJSKeyword() const;
    bool isJavaScriptKeyword() const;
    
    std::string getSelectorString() const;
    std::string toDebugString() const;
};

// CHTL JS Token工具类
class CHTLJSTokenUtils {
public:
    static std::string tokenTypeToString(CHTLJSTokenType type);
    static CHTLJSTokenType stringToTokenType(const std::string& str);
    static bool isCHTLJSKeyword(const std::string& str);
    static CHTLJSTokenType getCHTLJSKeywordType(const std::string& str);
    
    // 选择器解析
    static bool isSelector(const std::string& str);
    static std::pair<std::string, std::string> parseSelector(const std::string& str);
    
    // 事件名称验证
    static bool isValidEventName(const std::string& name);
    static std::vector<std::string> getSupportedEvents();
};

} // namespace js
} // namespace chtl

#endif // CHTLJS_TOKEN_H