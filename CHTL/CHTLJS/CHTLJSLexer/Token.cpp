#include "Token.h"
#include <algorithm>

namespace CHTLJS {

// 静态成员初始化
std::unordered_map<std::string, TokenType> TokenUtils::chtljsKeywordMap;
std::unordered_map<std::string, TokenType> TokenUtils::jsKeywordMap;
std::unordered_map<std::string, TokenType> TokenUtils::functionPropertyMap;
std::unordered_map<TokenType, std::string> TokenUtils::tokenTypeNameMap;
bool TokenUtils::mapsInitialized = false;

void TokenUtils::InitializeMaps() {
    if (mapsInitialized) return;
    
    // CHTL JS关键字
    chtljsKeywordMap = {
        {"vir", TokenType::VIR},
        {"listen", TokenType::LISTEN},
        {"delegate", TokenType::DELEGATE},
        {"animate", TokenType::ANIMATE}
    };
    
    // JavaScript关键字
    jsKeywordMap = {
        {"const", TokenType::CONST},
        {"let", TokenType::LET},
        {"var", TokenType::VAR},
        {"function", TokenType::FUNCTION},
        {"return", TokenType::RETURN},
        {"if", TokenType::IF},
        {"else", TokenType::ELSE},
        {"for", TokenType::FOR},
        {"while", TokenType::WHILE},
        {"do", TokenType::DO},
        {"break", TokenType::BREAK},
        {"continue", TokenType::CONTINUE},
        {"true", TokenType::BOOLEAN},
        {"false", TokenType::BOOLEAN},
        {"null", TokenType::NULL_LITERAL},
        {"undefined", TokenType::UNDEFINED}
    };
    
    // CHTL JS函数属性
    functionPropertyMap = {
        {"target", TokenType::TARGET},
        {"duration", TokenType::DURATION},
        {"easing", TokenType::EASING},
        {"begin", TokenType::BEGIN},
        {"when", TokenType::WHEN},
        {"end", TokenType::END},
        {"loop", TokenType::LOOP},
        {"direction", TokenType::DIRECTION},
        {"delay", TokenType::DELAY},
        {"callback", TokenType::CALLBACK},
        {"click", TokenType::CLICK},
        {"at", TokenType::AT}
    };
    
    // Token类型名称映射
    tokenTypeNameMap = {
        {TokenType::EOF_TOKEN, "EOF"},
        {TokenType::NEWLINE, "NEWLINE"},
        {TokenType::WHITESPACE, "WHITESPACE"},
        {TokenType::LEFT_BRACE, "LEFT_BRACE"},
        {TokenType::RIGHT_BRACE, "RIGHT_BRACE"},
        {TokenType::LEFT_BRACKET, "LEFT_BRACKET"},
        {TokenType::RIGHT_BRACKET, "RIGHT_BRACKET"},
        {TokenType::LEFT_PAREN, "LEFT_PAREN"},
        {TokenType::RIGHT_PAREN, "RIGHT_PAREN"},
        {TokenType::SEMICOLON, "SEMICOLON"},
        {TokenType::COLON, "COLON"},
        {TokenType::EQUALS, "EQUALS"},
        {TokenType::COMMA, "COMMA"},
        {TokenType::DOT, "DOT"},
        {TokenType::ARROW, "ARROW"},
        {TokenType::DOUBLE_LEFT_BRACE, "DOUBLE_LEFT_BRACE"},
        {TokenType::DOUBLE_RIGHT_BRACE, "DOUBLE_RIGHT_BRACE"},
        {TokenType::AMPERSAND_ARROW, "AMPERSAND_ARROW"},
        {TokenType::DOUBLE_STAR, "DOUBLE_STAR"},
        {TokenType::STRING_LITERAL, "STRING_LITERAL"},
        {TokenType::UNQUOTED_LITERAL, "UNQUOTED_LITERAL"},
        {TokenType::NUMBER, "NUMBER"},
        {TokenType::BOOLEAN, "BOOLEAN"},
        {TokenType::NULL_LITERAL, "NULL"},
        {TokenType::UNDEFINED, "UNDEFINED"},
        {TokenType::IDENTIFIER, "IDENTIFIER"},
        {TokenType::VIR, "VIR"},
        {TokenType::LISTEN, "LISTEN"},
        {TokenType::DELEGATE, "DELEGATE"},
        {TokenType::ANIMATE, "ANIMATE"},
        {TokenType::ARROW_FUNCTION, "ARROW_FUNCTION"},
        {TokenType::INVALID, "INVALID"}
    };
    
    mapsInitialized = true;
}

bool Token::IsCHTLJSKeyword() const {
    return TokenUtils::IsCHTLJSKeyword(value);
}

bool Token::IsJSKeyword() const {
    return TokenUtils::IsJSKeyword(value);
}

bool Token::IsCHTLJSOperator() const {
    return type == TokenType::ARROW || 
           type == TokenType::DOUBLE_LEFT_BRACE ||
           type == TokenType::DOUBLE_RIGHT_BRACE ||
           type == TokenType::AMPERSAND_ARROW ||
           type == TokenType::DOUBLE_STAR;
}

std::string Token::ToString() const {
    return TokenUtils::TokenTypeToString(type) + ": \"" + value + "\" at " + 
           std::to_string(line) + ":" + std::to_string(column);
}

std::string TokenUtils::TokenTypeToString(TokenType type) {
    InitializeMaps();
    auto it = tokenTypeNameMap.find(type);
    if (it != tokenTypeNameMap.end()) {
        return it->second;
    }
    return "UNKNOWN";
}

TokenType TokenUtils::StringToTokenType(const std::string& str) {
    InitializeMaps();
    
    // 检查CHTL JS关键字
    auto it = chtljsKeywordMap.find(str);
    if (it != chtljsKeywordMap.end()) {
        return it->second;
    }
    
    // 检查JS关键字
    it = jsKeywordMap.find(str);
    if (it != jsKeywordMap.end()) {
        return it->second;
    }
    
    // 检查函数属性
    it = functionPropertyMap.find(str);
    if (it != functionPropertyMap.end()) {
        return it->second;
    }
    
    return TokenType::IDENTIFIER;
}

bool TokenUtils::IsCHTLJSKeyword(const std::string& str) {
    InitializeMaps();
    return chtljsKeywordMap.find(str) != chtljsKeywordMap.end();
}

bool TokenUtils::IsJSKeyword(const std::string& str) {
    InitializeMaps();
    return jsKeywordMap.find(str) != jsKeywordMap.end();
}

bool TokenUtils::IsCHTLJSFunctionProperty(const std::string& str) {
    InitializeMaps();
    return functionPropertyMap.find(str) != functionPropertyMap.end();
}

} // namespace CHTLJS