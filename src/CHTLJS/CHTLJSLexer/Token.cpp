#include "Token.h"
#include <sstream>
#include <unordered_map>

namespace CHTLJS {

std::string Token::GetTypeName() const {
    static const std::unordered_map<TokenType, std::string> typeNames = {
        {TokenType::IDENTIFIER, "IDENTIFIER"},
        {TokenType::STRING_LITERAL, "STRING_LITERAL"},
        {TokenType::UNQUOTED_LITERAL, "UNQUOTED_LITERAL"},
        {TokenType::NUMBER, "NUMBER"},
        {TokenType::VIR, "VIR"},
        {TokenType::LISTEN, "LISTEN"},
        {TokenType::DELEGATE, "DELEGATE"},
        {TokenType::ANIMATE, "ANIMATE"},
        {TokenType::TARGET, "TARGET"},
        {TokenType::DURATION, "DURATION"},
        {TokenType::EASING, "EASING"},
        {TokenType::BEGIN, "BEGIN"},
        {TokenType::WHEN, "WHEN"},
        {TokenType::END, "END"},
        {TokenType::LOOP, "LOOP"},
        {TokenType::DIRECTION, "DIRECTION"},
        {TokenType::DELAY, "DELAY"},
        {TokenType::CALLBACK, "CALLBACK"},
        {TokenType::AT, "AT"},
        {TokenType::CLICK, "CLICK"},
        {TokenType::MOUSEENTER, "MOUSEENTER"},
        {TokenType::MOUSELEAVE, "MOUSELEAVE"},
        {TokenType::MOUSEMOVE, "MOUSEMOVE"},
        {TokenType::SELECTOR_START, "SELECTOR_START"},
        {TokenType::SELECTOR_END, "SELECTOR_END"},
        {TokenType::SELECTOR_CLASS, "SELECTOR_CLASS"},
        {TokenType::SELECTOR_ID, "SELECTOR_ID"},
        {TokenType::SELECTOR_TAG, "SELECTOR_TAG"},
        {TokenType::SELECTOR_INDEX, "SELECTOR_INDEX"},
        {TokenType::ARROW, "ARROW"},
        {TokenType::EVENT_BIND, "EVENT_BIND"},
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
        {TokenType::HASH, "HASH"},
        {TokenType::AMPERSAND, "AMPERSAND"},
        {TokenType::PLUS, "PLUS"},
        {TokenType::MINUS, "MINUS"},
        {TokenType::STAR, "STAR"},
        {TokenType::SLASH, "SLASH"},
        {TokenType::QUESTION, "QUESTION"},
        {TokenType::EXCLAMATION, "EXCLAMATION"},
        {TokenType::COMMENT_LINE, "COMMENT_LINE"},
        {TokenType::COMMENT_BLOCK, "COMMENT_BLOCK"},
        {TokenType::COMMENT_GENERATOR, "COMMENT_GENERATOR"},
        {TokenType::EOF_TOKEN, "EOF_TOKEN"},
        {TokenType::NEWLINE, "NEWLINE"},
        {TokenType::WHITESPACE, "WHITESPACE"},
        {TokenType::UNKNOWN, "UNKNOWN"}
    };
    
    auto it = typeNames.find(type);
    return it != typeNames.end() ? it->second : "UNKNOWN";
}

bool Token::IsKeyword() const {
    return type >= TokenType::VIR && type <= TokenType::MOUSEMOVE;
}

bool Token::IsSymbol() const {
    return type >= TokenType::LEFT_BRACE && type <= TokenType::EXCLAMATION;
}

bool Token::IsLiteral() const {
    return type == TokenType::STRING_LITERAL || 
           type == TokenType::UNQUOTED_LITERAL || 
           type == TokenType::NUMBER;
}

bool Token::IsComment() const {
    return type == TokenType::COMMENT_LINE || 
           type == TokenType::COMMENT_BLOCK || 
           type == TokenType::COMMENT_GENERATOR;
}

bool Token::IsWhitespace() const {
    return type == TokenType::WHITESPACE || type == TokenType::NEWLINE;
}

bool Token::IsSelector() const {
    return type >= TokenType::SELECTOR_START && type <= TokenType::SELECTOR_INDEX;
}

bool Token::IsCHTLJSOperator() const {
    return type == TokenType::ARROW || type == TokenType::EVENT_BIND;
}

std::string Token::ToString() const {
    std::ostringstream oss;
    oss << "[" << GetTypeName() << "] ";
    if (!value.empty()) {
        oss << "'" << value << "' ";
    }
    oss << "at (" << position.line << ":" << position.column << ")";
    return oss.str();
}

std::unique_ptr<Token> Token::Clone() const {
    return std::make_unique<Token>(type, value, position);
}

} // namespace CHTLJS