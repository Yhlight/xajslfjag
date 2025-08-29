#include "Token.h"
#include <set>

namespace CHTL {
namespace JS {

// 关键字映射表
const std::unordered_map<String, CHTLJSTokenType> CHTLJSTokenTypeConverter::keywordMap = {
    // JavaScript关键字
    {"const", CHTLJSTokenType::CONST},
    {"let", CHTLJSTokenType::LET},
    {"var", CHTLJSTokenType::VAR},
    {"function", CHTLJSTokenType::FUNCTION},
    {"return", CHTLJSTokenType::RETURN},
    {"if", CHTLJSTokenType::IF},
    {"else", CHTLJSTokenType::ELSE},
    {"for", CHTLJSTokenType::FOR},
    {"while", CHTLJSTokenType::WHILE},
    {"do", CHTLJSTokenType::DO},
    {"break", CHTLJSTokenType::BREAK},
    {"continue", CHTLJSTokenType::CONTINUE},
    {"class", CHTLJSTokenType::CLASS},
    {"extends", CHTLJSTokenType::EXTENDS},
    {"new", CHTLJSTokenType::NEW},
    {"this", CHTLJSTokenType::THIS},
    {"try", CHTLJSTokenType::TRY},
    {"catch", CHTLJSTokenType::CATCH},
    {"finally", CHTLJSTokenType::FINALLY},
    {"throw", CHTLJSTokenType::THROW},
    {"true", CHTLJSTokenType::BOOLEAN},
    {"false", CHTLJSTokenType::BOOLEAN},
    {"null", CHTLJSTokenType::NULL_LITERAL},
    {"undefined", CHTLJSTokenType::UNDEFINED},
    
    // CHTL JS特有关键字
    {"listen", CHTLJSTokenType::LISTEN},
    {"delegate", CHTLJSTokenType::DELEGATE},
    {"animate", CHTLJSTokenType::ANIMATE},
    {"vir", CHTLJSTokenType::VIR},
    {"module", CHTLJSTokenType::MODULE},
    {"load", CHTLJSTokenType::LOAD}
};

// 操作符映射表
const std::unordered_map<String, CHTLJSTokenType> CHTLJSTokenTypeConverter::operatorMap = {
    // CHTL JS特殊操作符
    {"->", CHTLJSTokenType::ARROW_OP},
    {"=>", CHTLJSTokenType::DOUBLE_ARROW},
    {"&->", CHTLJSTokenType::EVENT_BIND},
    {"{{", CHTLJSTokenType::SELECTOR_START},
    {"}}", CHTLJSTokenType::SELECTOR_END},
    
    // 标准操作符
    {"+", CHTLJSTokenType::PLUS},
    {"-", CHTLJSTokenType::MINUS},
    {"*", CHTLJSTokenType::MULTIPLY},
    {"/", CHTLJSTokenType::DIVIDE},
    {"%", CHTLJSTokenType::MODULO},
    {"=", CHTLJSTokenType::ASSIGN},
    {"+=", CHTLJSTokenType::PLUS_ASSIGN},
    {"-=", CHTLJSTokenType::MINUS_ASSIGN},
    {"*=", CHTLJSTokenType::MULT_ASSIGN},
    {"/=", CHTLJSTokenType::DIV_ASSIGN},
    {"%=", CHTLJSTokenType::MOD_ASSIGN},
    {"**", CHTLJSTokenType::POWER},
    {"++", CHTLJSTokenType::INCREMENT},
    {"--", CHTLJSTokenType::DECREMENT},
    
    // 比较操作符
    {"==", CHTLJSTokenType::EQUAL},
    {"!=", CHTLJSTokenType::NOT_EQUAL},
    {"===", CHTLJSTokenType::STRICT_EQUAL},
    {"!==", CHTLJSTokenType::STRICT_NOT_EQUAL},
    {"<", CHTLJSTokenType::LESS_THAN},
    {">", CHTLJSTokenType::GREATER_THAN},
    {"<=", CHTLJSTokenType::LESS_EQUAL},
    {">=", CHTLJSTokenType::GREATER_EQUAL},
    
    // 逻辑操作符
    {"&&", CHTLJSTokenType::LOGICAL_AND},
    {"||", CHTLJSTokenType::LOGICAL_OR},
    {"!", CHTLJSTokenType::LOGICAL_NOT},
    
    // 位操作符
    {"&", CHTLJSTokenType::BITWISE_AND},
    {"|", CHTLJSTokenType::BITWISE_OR},
    {"^", CHTLJSTokenType::BITWISE_XOR},
    {"~", CHTLJSTokenType::BITWISE_NOT},
    {"<<", CHTLJSTokenType::LEFT_SHIFT},
    {">>", CHTLJSTokenType::RIGHT_SHIFT},
    {">>>", CHTLJSTokenType::UNSIGNED_RIGHT_SHIFT},
    
    // 分隔符
    {";", CHTLJSTokenType::SEMICOLON},
    {",", CHTLJSTokenType::COMMA},
    {".", CHTLJSTokenType::DOT},
    {":", CHTLJSTokenType::COLON},
    {"?", CHTLJSTokenType::QUESTION},
    
    // 括号
    {"(", CHTLJSTokenType::LEFT_PAREN},
    {")", CHTLJSTokenType::RIGHT_PAREN},
    {"{", CHTLJSTokenType::LEFT_BRACE},
    {"}", CHTLJSTokenType::RIGHT_BRACE},
    {"[", CHTLJSTokenType::LEFT_BRACKET},
    {"]", CHTLJSTokenType::RIGHT_BRACKET}
};

// 类型字符串映射表
const std::unordered_map<CHTLJSTokenType, String> CHTLJSTokenTypeConverter::typeStringMap = {
    {CHTLJSTokenType::IDENTIFIER, "IDENTIFIER"},
    {CHTLJSTokenType::NUMBER, "NUMBER"},
    {CHTLJSTokenType::STRING, "STRING"},
    {CHTLJSTokenType::BOOLEAN, "BOOLEAN"},
    {CHTLJSTokenType::NULL_LITERAL, "NULL"},
    {CHTLJSTokenType::UNDEFINED, "UNDEFINED"},
    
    {CHTLJSTokenType::CONST, "CONST"},
    {CHTLJSTokenType::LET, "LET"},
    {CHTLJSTokenType::VAR, "VAR"},
    {CHTLJSTokenType::FUNCTION, "FUNCTION"},
    {CHTLJSTokenType::RETURN, "RETURN"},
    {CHTLJSTokenType::IF, "IF"},
    {CHTLJSTokenType::ELSE, "ELSE"},
    {CHTLJSTokenType::FOR, "FOR"},
    {CHTLJSTokenType::WHILE, "WHILE"},
    {CHTLJSTokenType::DO, "DO"},
    {CHTLJSTokenType::BREAK, "BREAK"},
    {CHTLJSTokenType::CONTINUE, "CONTINUE"},
    {CHTLJSTokenType::CLASS, "CLASS"},
    {CHTLJSTokenType::EXTENDS, "EXTENDS"},
    {CHTLJSTokenType::NEW, "NEW"},
    {CHTLJSTokenType::THIS, "THIS"},
    {CHTLJSTokenType::TRY, "TRY"},
    {CHTLJSTokenType::CATCH, "CATCH"},
    {CHTLJSTokenType::FINALLY, "FINALLY"},
    {CHTLJSTokenType::THROW, "THROW"},
    
    {CHTLJSTokenType::LISTEN, "LISTEN"},
    {CHTLJSTokenType::DELEGATE, "DELEGATE"},
    {CHTLJSTokenType::ANIMATE, "ANIMATE"},
    {CHTLJSTokenType::VIR, "VIR"},
    {CHTLJSTokenType::MODULE, "MODULE"},
    {CHTLJSTokenType::LOAD, "LOAD"},
    
    {CHTLJSTokenType::ARROW_OP, "ARROW_OP"},
    {CHTLJSTokenType::DOUBLE_ARROW, "DOUBLE_ARROW"},
    {CHTLJSTokenType::EVENT_BIND, "EVENT_BIND"},
    {CHTLJSTokenType::SELECTOR_START, "SELECTOR_START"},
    {CHTLJSTokenType::SELECTOR_END, "SELECTOR_END"},
    
    {CHTLJSTokenType::EOF_TOKEN, "EOF"},
    {CHTLJSTokenType::INVALID, "INVALID"},
    {CHTLJSTokenType::CJMOD_KEYWORD, "CJMOD_KEYWORD"}
};

// CHTLJSToken实现
CHTLJSToken::CHTLJSToken() 
    : type(CHTLJSTokenType::INVALID), value(""), position({0, 0, 0}), length(0) {
}

CHTLJSToken::CHTLJSToken(CHTLJSTokenType type, const String& value, const Position& pos, size_t len)
    : type(type), value(value), position(pos), length(len == 0 ? value.length() : len) {
}

bool CHTLJSToken::isKeyword() const {
    return type >= CHTLJSTokenType::CONST && type <= CHTLJSTokenType::LOAD;
}

bool CHTLJSToken::isOperator() const {
    return type >= CHTLJSTokenType::ARROW_OP && type <= CHTLJSTokenType::RIGHT_BRACKET;
}

bool CHTLJSToken::isCHTLJSSpecific() const {
    return type == CHTLJSTokenType::LISTEN || 
           type == CHTLJSTokenType::DELEGATE ||
           type == CHTLJSTokenType::ANIMATE ||
           type == CHTLJSTokenType::VIR ||
           type == CHTLJSTokenType::MODULE ||
           type == CHTLJSTokenType::ARROW_OP ||
           type == CHTLJSTokenType::EVENT_BIND ||
           type == CHTLJSTokenType::SELECTOR_START ||
           type == CHTLJSTokenType::SELECTOR_END;
}

bool CHTLJSToken::isSelector() const {
    return type == CHTLJSTokenType::SELECTOR_START || type == CHTLJSTokenType::SELECTOR_END;
}

bool CHTLJSToken::isEventBinding() const {
    return type == CHTLJSTokenType::EVENT_BIND;
}

bool CHTLJSToken::isCJMODKeyword() const {
    return type == CHTLJSTokenType::CJMOD_KEYWORD;
}

String CHTLJSToken::toString() const {
    return value;
}

String CHTLJSToken::getTypeString() const {
    return CHTLJSTokenTypeConverter::typeToString(type);
}

bool CHTLJSToken::operator==(const CHTLJSToken& other) const {
    return type == other.type && value == other.value;
}

bool CHTLJSToken::operator!=(const CHTLJSToken& other) const {
    return !(*this == other);
}

// CHTLJSTokenTypeConverter实现
String CHTLJSTokenTypeConverter::typeToString(CHTLJSTokenType type) {
    auto it = typeStringMap.find(type);
    if (it != typeStringMap.end()) {
        return it->second;
    }
    return "UNKNOWN";
}

CHTLJSTokenType CHTLJSTokenTypeConverter::stringToType(const String& typeStr) {
    for (const auto& pair : typeStringMap) {
        if (pair.second == typeStr) {
            return pair.first;
        }
    }
    return CHTLJSTokenType::INVALID;
}

bool CHTLJSTokenTypeConverter::isValidTokenType(const String& typeStr) {
    return stringToType(typeStr) != CHTLJSTokenType::INVALID;
}

bool CHTLJSTokenTypeConverter::isJavaScriptKeyword(const String& word) {
    static const std::set<String> jsKeywords = {
        "const", "let", "var", "function", "return", "if", "else",
        "for", "while", "do", "break", "continue", "class", "extends",
        "new", "this", "try", "catch", "finally", "throw", "true",
        "false", "null", "undefined"
    };
    return jsKeywords.find(word) != jsKeywords.end();
}

bool CHTLJSTokenTypeConverter::isCHTLJSKeyword(const String& word) {
    static const std::set<String> chtljsKeywords = {
        "listen", "delegate", "animate", "vir", "module", "load"
    };
    return chtljsKeywords.find(word) != chtljsKeywords.end();
}

bool CHTLJSTokenTypeConverter::isCJMODKeyword(const String& word) {
    // CJMOD关键字由用户扩展定义，这里提供基础检查
    static const std::set<String> cjmodKeywords = {
        "printMylove", "iNeverAway"  // 文档中提到的示例
    };
    return cjmodKeywords.find(word) != cjmodKeywords.end();
}

bool CHTLJSTokenTypeConverter::isOperator(const String& op) {
    return operatorMap.find(op) != operatorMap.end();
}

bool CHTLJSTokenTypeConverter::isCHTLJSOperator(const String& op) {
    static const std::set<String> chtljsOps = {
        "->", "&->", "{{", "}}"
    };
    return chtljsOps.find(op) != chtljsOps.end();
}

} // namespace JS
} // namespace CHTL