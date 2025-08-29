#include "Token.h"
#include <unordered_map>
#include <sstream>

namespace CHTLJS {

// Token类型名称映射表
static const std::unordered_map<TokenType, const char*> tokenTypeNames = {
    {TokenType::STRING_LITERAL, "STRING_LITERAL"},
    {TokenType::UNQUOTED_LITERAL, "UNQUOTED_LITERAL"},
    {TokenType::NUMBER_LITERAL, "NUMBER_LITERAL"},
    {TokenType::BOOLEAN_LITERAL, "BOOLEAN_LITERAL"},
    {TokenType::NULL_LITERAL, "NULL_LITERAL"},
    {TokenType::IDENTIFIER, "IDENTIFIER"},
    {TokenType::KEYWORD_MODULE, "module"},
    {TokenType::KEYWORD_LOAD, "load"},
    {TokenType::KEYWORD_LISTEN, "listen"},
    {TokenType::KEYWORD_DELEGATE, "delegate"},
    {TokenType::KEYWORD_ANIMATE, "animate"},
    {TokenType::KEYWORD_VIR, "vir"},
    {TokenType::KEYWORD_INEVERAWAY, "iNeverAway"},
    {TokenType::KEYWORD_TARGET, "target"},
    {TokenType::KEYWORD_DURATION, "duration"},
    {TokenType::KEYWORD_EASING, "easing"},
    {TokenType::KEYWORD_BEGIN, "begin"},
    {TokenType::KEYWORD_WHEN, "when"},
    {TokenType::KEYWORD_END, "end"},
    {TokenType::KEYWORD_AT, "at"},
    {TokenType::KEYWORD_LOOP, "loop"},
    {TokenType::KEYWORD_DIRECTION, "direction"},
    {TokenType::KEYWORD_DELAY, "delay"},
    {TokenType::KEYWORD_CALLBACK, "callback"},
    {TokenType::KEYWORD_CONST, "const"},
    {TokenType::KEYWORD_LET, "let"},
    {TokenType::KEYWORD_VAR, "var"},
    {TokenType::KEYWORD_FUNCTION, "function"},
    {TokenType::KEYWORD_RETURN, "return"},
    {TokenType::KEYWORD_IF, "if"},
    {TokenType::KEYWORD_ELSE, "else"},
    {TokenType::KEYWORD_FOR, "for"},
    {TokenType::KEYWORD_WHILE, "while"},
    {TokenType::KEYWORD_THIS, "this"},
    {TokenType::ARROW, "->"},
    {TokenType::EVENT_BIND, "&->"},
    {TokenType::DOUBLE_LEFT_BRACE, "{{"},
    {TokenType::DOUBLE_RIGHT_BRACE, "}}"},
    {TokenType::COLON, ":"},
    {TokenType::SEMICOLON, ";"},
    {TokenType::COMMA, ","},
    {TokenType::DOT, "."},
    {TokenType::AMPERSAND, "&"},
    {TokenType::EQUAL, "="},
    {TokenType::PLUS, "+"},
    {TokenType::MINUS, "-"},
    {TokenType::MULTIPLY, "*"},
    {TokenType::DIVIDE, "/"},
    {TokenType::MODULO, "%"},
    {TokenType::EQUAL_EQUAL, "=="},
    {TokenType::NOT_EQUAL, "!="},
    {TokenType::LESS_THAN, "<"},
    {TokenType::GREATER_THAN, ">"},
    {TokenType::LESS_EQUAL, "<="},
    {TokenType::GREATER_EQUAL, ">="},
    {TokenType::AND, "&&"},
    {TokenType::OR, "||"},
    {TokenType::NOT, "!"},
    {TokenType::LEFT_BRACE, "{"},
    {TokenType::RIGHT_BRACE, "}"},
    {TokenType::LEFT_BRACKET, "["},
    {TokenType::RIGHT_BRACKET, "]"},
    {TokenType::LEFT_PAREN, "("},
    {TokenType::RIGHT_PAREN, ")"},
    {TokenType::SELECTOR_CLASS, "SELECTOR_CLASS"},
    {TokenType::SELECTOR_ID, "SELECTOR_ID"},
    {TokenType::SELECTOR_TAG, "SELECTOR_TAG"},
    {TokenType::SELECTOR_COMPOUND, "SELECTOR_COMPOUND"},
    {TokenType::SELECTOR_REF, "SELECTOR_REF"},
    {TokenType::SINGLE_LINE_COMMENT, "SINGLE_LINE_COMMENT"},
    {TokenType::MULTI_LINE_COMMENT, "MULTI_LINE_COMMENT"},
    {TokenType::WHITESPACE, "WHITESPACE"},
    {TokenType::NEWLINE, "NEWLINE"},
    {TokenType::EOF_TOKEN, "EOF"},
    {TokenType::UNKNOWN, "UNKNOWN"}
};

// CHTL JS关键字映射表
static const std::unordered_map<std::string, TokenType> chtljsKeywordMap = {
    {"module", TokenType::KEYWORD_MODULE},
    {"load", TokenType::KEYWORD_LOAD},
    {"listen", TokenType::KEYWORD_LISTEN},
    {"delegate", TokenType::KEYWORD_DELEGATE},
    {"animate", TokenType::KEYWORD_ANIMATE},
    {"vir", TokenType::KEYWORD_VIR},
    {"iNeverAway", TokenType::KEYWORD_INEVERAWAY},
    {"target", TokenType::KEYWORD_TARGET},
    {"duration", TokenType::KEYWORD_DURATION},
    {"easing", TokenType::KEYWORD_EASING},
    {"begin", TokenType::KEYWORD_BEGIN},
    {"when", TokenType::KEYWORD_WHEN},
    {"end", TokenType::KEYWORD_END},
    {"at", TokenType::KEYWORD_AT},
    {"loop", TokenType::KEYWORD_LOOP},
    {"direction", TokenType::KEYWORD_DIRECTION},
    {"delay", TokenType::KEYWORD_DELAY},
    {"callback", TokenType::KEYWORD_CALLBACK}
};

// JavaScript关键字映射表
static const std::unordered_map<std::string, TokenType> jsKeywordMap = {
    {"const", TokenType::KEYWORD_CONST},
    {"let", TokenType::KEYWORD_LET},
    {"var", TokenType::KEYWORD_VAR},
    {"function", TokenType::KEYWORD_FUNCTION},
    {"return", TokenType::KEYWORD_RETURN},
    {"if", TokenType::KEYWORD_IF},
    {"else", TokenType::KEYWORD_ELSE},
    {"for", TokenType::KEYWORD_FOR},
    {"while", TokenType::KEYWORD_WHILE},
    {"this", TokenType::KEYWORD_THIS},
    {"true", TokenType::BOOLEAN_LITERAL},   // 特殊处理
    {"false", TokenType::BOOLEAN_LITERAL},  // 特殊处理
    {"null", TokenType::NULL_LITERAL}       // 特殊处理
};

std::string Token::toString() const {
    std::stringstream ss;
    ss << "Token[" << getTokenTypeName(type_) << ", \"" << lexeme_ << "\", "
       << location_.line << ":" << location_.column << "]";
    return ss.str();
}

const char* getTokenTypeName(TokenType type) {
    auto it = tokenTypeNames.find(type);
    if (it != tokenTypeNames.end()) {
        return it->second;
    }
    return "UNKNOWN_TYPE";
}

TokenType getCHTLJSKeywordType(const std::string& keyword) {
    // 先检查CHTL JS关键字
    auto chtljsIt = chtljsKeywordMap.find(keyword);
    if (chtljsIt != chtljsKeywordMap.end()) {
        return chtljsIt->second;
    }
    
    // 再检查JavaScript关键字
    auto jsIt = jsKeywordMap.find(keyword);
    if (jsIt != jsKeywordMap.end()) {
        return jsIt->second;
    }
    
    return TokenType::IDENTIFIER;
}

TokenType getJSKeywordType(const std::string& keyword) {
    auto it = jsKeywordMap.find(keyword);
    if (it != jsKeywordMap.end()) {
        return it->second;
    }
    return TokenType::IDENTIFIER;
}

} // namespace CHTLJS