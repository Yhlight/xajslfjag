#pragma once

#include "../../Util/Common.h"
#include "../../CHTL/CHTLLexer/Token.h"  // 导入Position定义
#include <unordered_map>

namespace CHTL {
namespace JS {

// CHTL JS Token类型枚举
enum class CHTLJSTokenType {
    // 基础JavaScript Token
    IDENTIFIER,
    NUMBER,
    STRING,
    BOOLEAN,
    NULL_LITERAL,
    UNDEFINED,
    
    // JavaScript关键字
    CONST,
    LET,
    VAR,
    FUNCTION,
    RETURN,
    IF,
    ELSE,
    FOR,
    WHILE,
    DO,
    BREAK,
    CONTINUE,
    CLASS,
    EXTENDS,
    NEW,
    THIS,
    TRY,
    CATCH,
    FINALLY,
    THROW,
    
    // CHTL JS特有关键字
    LISTEN,
    DELEGATE,
    ANIMATE,
    VIR,
    MODULE,
    LOAD,
    
    // CHTL JS特殊操作符
    ARROW_OP,           // ->
    DOUBLE_ARROW,       // =>
    EVENT_BIND,         // &->
    SELECTOR_START,     // {{
    SELECTOR_END,       // }}
    
    // 标准操作符
    PLUS,               // +
    MINUS,              // -
    MULTIPLY,           // *
    DIVIDE,             // /
    MODULO,             // %
    ASSIGN,             // =
    PLUS_ASSIGN,        // +=
    MINUS_ASSIGN,       // -=
    MULT_ASSIGN,        // *=
    DIV_ASSIGN,         // /=
    MOD_ASSIGN,         // %=
    POWER,              // **
    INCREMENT,          // ++
    DECREMENT,          // --
    
    // 比较操作符
    EQUAL,              // ==
    NOT_EQUAL,          // !=
    STRICT_EQUAL,       // ===
    STRICT_NOT_EQUAL,   // !==
    LESS_THAN,          // <
    GREATER_THAN,       // >
    LESS_EQUAL,         // <=
    GREATER_EQUAL,      // >=
    
    // 逻辑操作符
    LOGICAL_AND,        // &&
    LOGICAL_OR,         // ||
    LOGICAL_NOT,        // !
    
    // 位操作符
    BITWISE_AND,        // &
    BITWISE_OR,         // |
    BITWISE_XOR,        // ^
    BITWISE_NOT,        // ~
    LEFT_SHIFT,         // <<
    RIGHT_SHIFT,        // >>
    UNSIGNED_RIGHT_SHIFT, // >>>
    
    // 分隔符
    SEMICOLON,          // ;
    COMMA,              // ,
    DOT,                // .
    COLON,              // :
    QUESTION,           // ?
    
    // 括号
    LEFT_PAREN,         // (
    RIGHT_PAREN,        // )
    LEFT_BRACE,         // {
    RIGHT_BRACE,        // }
    LEFT_BRACKET,       // [
    RIGHT_BRACKET,      // ]
    
    // 特殊Token
    NEWLINE,
    WHITESPACE,
    COMMENT_SINGLE,     // //
    COMMENT_MULTI,      // /* */
    
    // 文件结束
    EOF_TOKEN,
    
    // 错误Token
    INVALID,
    
    // CJMOD关键字 (用于扩展)
    CJMOD_KEYWORD
};

// CHTL JS Token类
class CHTLJSToken {
public:
    CHTLJSTokenType type;
    String value;
    Position position;
    size_t length;
    
    // 构造函数
    CHTLJSToken();
    CHTLJSToken(CHTLJSTokenType type, const String& value, const Position& pos, size_t len = 0);
    
    // 工具方法
    bool isKeyword() const;
    bool isOperator() const;
    bool isCHTLJSSpecific() const;
    bool isSelector() const;
    bool isEventBinding() const;
    bool isCJMODKeyword() const;
    String toString() const;
    String getTypeString() const;
    
    // 比较操作符
    bool operator==(const CHTLJSToken& other) const;
    bool operator!=(const CHTLJSToken& other) const;
};

// Token类型转换工具
class CHTLJSTokenTypeConverter {
public:
    static String typeToString(CHTLJSTokenType type);
    static CHTLJSTokenType stringToType(const String& typeStr);
    static bool isValidTokenType(const String& typeStr);
    
    // 关键字检查
    static bool isJavaScriptKeyword(const String& word);
    static bool isCHTLJSKeyword(const String& word);
    static bool isCJMODKeyword(const String& word);
    
    // 操作符检查
    static bool isOperator(const String& op);
    static bool isCHTLJSOperator(const String& op);
    
private:
    static const std::unordered_map<String, CHTLJSTokenType> keywordMap;
    static const std::unordered_map<String, CHTLJSTokenType> operatorMap;
    static const std::unordered_map<CHTLJSTokenType, String> typeStringMap;
};

} // namespace JS
} // namespace CHTL