#ifndef CHTLJS_TOKEN_H
#define CHTLJS_TOKEN_H

#include "Core/Common.h"

namespace CHTLJS {

// CHTL JS Token类型
enum class TokenType {
    // 基本标记
    EOF_TOKEN,              // 文件结束
    UNKNOWN,                // 未知token
    
    // 字面量
    IDENTIFIER,             // 标识符
    STRING_LITERAL,         // 字符串字面量（双引号）
    SINGLE_STRING_LITERAL,  // 字符串字面量（单引号）
    UNQUOTED_LITERAL,       // 无修饰字面量（CHTL JS特有）
    NUMBER,                 // 数字
    BOOLEAN,                // 布尔值（true/false）
    NULL_LITERAL,           // null
    UNDEFINED,              // undefined
    
    // 运算符和标点
    LEFT_BRACE,             // {
    RIGHT_BRACE,            // }
    LEFT_BRACKET,           // [
    RIGHT_BRACKET,          // ]
    LEFT_PAREN,             // (
    RIGHT_PAREN,            // )
    SEMICOLON,              // ;
    COLON,                  // :
    EQUALS,                 // =
    COMMA,                  // ,
    DOT,                    // .
    QUESTION,               // ?
    EXCLAMATION,            // !
    PLUS,                   // +
    MINUS,                  // -
    MULTIPLY,               // *
    DIVIDE,                 // /
    MODULO,                 // %
    
    // CHTL JS特有标记
    DOUBLE_LEFT_BRACE,      // {{ （增强选择器开始）
    DOUBLE_RIGHT_BRACE,     // }} （增强选择器结束）
    ARROW,                  // -> （箭头操作符）
    DOUBLE_COLON,           // :: （官方模块前缀）
    
    // CHTL JS关键字
    LISTEN,                 // listen
    DELEGATE,               // delegate
    ANIMATE,                // animate
    VIR,                    // vir
    FROM,                   // from
    TO,                     // to
    BY,                     // by
    DURATION,               // duration
    DELAY,                  // delay
    EASING,                 // easing
    REPEAT,                 // repeat
    ALTERNATE,              // alternate
    FILL,                   // fill
    
    // JavaScript关键字（CHTL JS编译到JS时需要）
    VAR,                    // var
    LET,                    // let
    CONST,                  // const
    FUNCTION,               // function
    IF,                     // if
    ELSE,                   // else
    FOR,                    // for
    WHILE,                  // while
    DO,                     // do
    RETURN,                 // return
    THIS,                   // this
    NEW,                    // new
    TRY,                    // try
    CATCH,                  // catch
    FINALLY,                // finally
    THROW,                  // throw
    
    // JavaScript事件（所有标准DOM事件）
    EVENT_CLICK,            // click
    EVENT_DBLCLICK,         // dblclick
    EVENT_MOUSEDOWN,        // mousedown
    EVENT_MOUSEUP,          // mouseup
    EVENT_MOUSEMOVE,        // mousemove
    EVENT_MOUSEOVER,        // mouseover
    EVENT_MOUSEOUT,         // mouseout
    EVENT_MOUSEENTER,       // mouseenter
    EVENT_MOUSELEAVE,       // mouseleave
    EVENT_KEYDOWN,          // keydown
    EVENT_KEYUP,            // keyup
    EVENT_KEYPRESS,         // keypress
    EVENT_FOCUS,            // focus
    EVENT_BLUR,             // blur
    EVENT_CHANGE,           // change
    EVENT_INPUT,            // input
    EVENT_SUBMIT,           // submit
    EVENT_RESET,            // reset
    EVENT_LOAD,             // load
    EVENT_UNLOAD,           // unload
    EVENT_RESIZE,           // resize
    EVENT_SCROLL,           // scroll
    EVENT_CUSTOM,           // 自定义事件
    
    // 注释
    LINE_COMMENT,           // // 注释
    BLOCK_COMMENT,          // /* 注释 */
    
    // 特殊
    WHITESPACE,             // 空白字符
    NEWLINE,                // 换行符
};

// Token结构
struct Token {
    TokenType type;
    String value;
    SourceLocation location;
    
    Token() : type(TokenType::UNKNOWN) {}
    
    Token(TokenType t, const String& v, const SourceLocation& loc)
        : type(t), value(v), location(loc) {}
    
    // 判断是否是CHTL JS关键字
    bool IsCHTLJSKeyword() const;
    
    // 判断是否是JavaScript关键字
    bool IsJavaScriptKeyword() const;
    
    // 判断是否是事件名
    bool IsEventName() const;
    
    // 判断是否是字面量
    bool IsLiteral() const;
    
    // 判断是否是运算符
    bool IsOperator() const;
    
    // 判断是否是注释
    bool IsComment() const;
    
    // 获取Token类型名称
    String GetTypeName() const;
};

// Token工具类
class TokenUtils {
public:
    // 将字符串转换为Token类型
    static TokenType StringToTokenType(const String& str);
    
    // 获取Token类型的字符串表示
    static String TokenTypeToString(TokenType type);
    
    // 判断字符串是否是CHTL JS关键字
    static bool IsCHTLJSKeyword(const String& str);
    
    // 判断字符串是否是JavaScript关键字
    static bool IsJavaScriptKeyword(const String& str);
    
    // 判断字符串是否是事件名
    static bool IsEventName(const String& str);
    
    // 获取所有CHTL JS关键字列表
    static Vector<String> GetCHTLJSKeywords();
    
    // 获取所有JavaScript关键字列表
    static Vector<String> GetJavaScriptKeywords();
    
    // 获取所有支持的事件名列表
    static Vector<String> GetEventNames();
    
    // 标准化事件名（去除on前缀等）
    static String NormalizeEventName(const String& eventName);
};

// Token流（与CHTL共享基本结构，但独立实现）
class TokenStream {
private:
    Vector<Token> tokens_;
    size_t current_ = 0;
    
public:
    // 添加Token
    void AddToken(const Token& token);
    void AddToken(TokenType type, const String& value, const SourceLocation& location);
    
    // 获取当前Token
    const Token& Current() const;
    
    // 获取下一个Token
    const Token& Next();
    
    // 查看下一个Token（不移动位置）
    const Token& Peek(size_t offset = 1) const;
    
    // 检查是否到达末尾
    bool IsAtEnd() const;
    
    // 重置位置
    void Reset() { current_ = 0; }
    
    // 获取当前位置
    size_t GetPosition() const { return current_; }
    
    // 设置位置
    void SetPosition(size_t pos) { current_ = pos; }
    
    // 获取所有Token
    const Vector<Token>& GetTokens() const { return tokens_; }
    
    // 清空Token流
    void Clear();
    
    // 获取Token数量
    size_t Size() const { return tokens_.size(); }
    
    // 匹配Token类型
    bool Match(TokenType type);
    bool Match(const Vector<TokenType>& types);
    
    // 消费Token
    Token Consume(TokenType type, const String& errorMessage);
    
    // 同步（用于错误恢复）
    void Synchronize();
    
    // CHTL JS特有的匹配方法
    bool MatchEnhancedSelector();  // 匹配 {{ }}
    bool MatchArrowOperator();      // 匹配 ->
    bool MatchEventName();          // 匹配事件名
};

// CHTL JS特有的Token验证器
class TokenValidator {
public:
    // 验证增强选择器语法
    static bool IsValidEnhancedSelector(const TokenStream& stream, size_t start, size_t end);
    
    // 验证箭头操作符使用
    static bool IsValidArrowOperation(const Token& left, const Token& right);
    
    // 验证事件监听器语法
    static bool IsValidListenerSyntax(const TokenStream& stream, size_t pos);
    
    // 验证动画定义语法
    static bool IsValidAnimateSyntax(const TokenStream& stream, size_t pos);
    
    // 验证虚对象定义
    static bool IsValidVirDefinition(const TokenStream& stream, size_t pos);
    
    // 验证键值对（支持无序和可选）
    static bool IsValidKeyValuePairs(const TokenStream& stream, size_t start, size_t end);
};

} // namespace CHTLJS

#endif // CHTLJS_TOKEN_H