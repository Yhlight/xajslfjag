#include "Token.h"
#include <sstream>
#include <unordered_map>

namespace CHTL {

std::string Token::GetTypeName() const {
    static const std::unordered_map<TokenType, std::string> typeNames = {
        {TokenType::IDENTIFIER, "IDENTIFIER"},
        {TokenType::STRING_LITERAL, "STRING_LITERAL"},
        {TokenType::UNQUOTED_LITERAL, "UNQUOTED_LITERAL"},
        {TokenType::NUMBER, "NUMBER"},
        {TokenType::TEXT, "TEXT"},
        {TokenType::STYLE, "STYLE"},
        {TokenType::SCRIPT, "SCRIPT"},
        {TokenType::USE, "USE"},
        {TokenType::HTML5, "HTML5"},
        {TokenType::INHERIT, "INHERIT"},
        {TokenType::DELETE, "DELETE"},
        {TokenType::INSERT, "INSERT"},
        {TokenType::AFTER, "AFTER"},
        {TokenType::BEFORE, "BEFORE"},
        {TokenType::REPLACE, "REPLACE"},
        {TokenType::AT_TOP, "AT_TOP"},
        {TokenType::AT_BOTTOM, "AT_BOTTOM"},
        {TokenType::FROM, "FROM"},
        {TokenType::AS, "AS"},
        {TokenType::EXCEPT, "EXCEPT"},
        {TokenType::TEMPLATE, "TEMPLATE"},
        {TokenType::CUSTOM, "CUSTOM"},
        {TokenType::ORIGIN, "ORIGIN"},
        {TokenType::IMPORT, "IMPORT"},
        {TokenType::NAMESPACE, "NAMESPACE"},
        {TokenType::CONFIGURATION, "CONFIGURATION"},
        {TokenType::AT_STYLE, "AT_STYLE"},
        {TokenType::AT_ELEMENT, "AT_ELEMENT"},
        {TokenType::AT_VAR, "AT_VAR"},
        {TokenType::AT_HTML, "AT_HTML"},
        {TokenType::AT_JAVASCRIPT, "AT_JAVASCRIPT"},
        {TokenType::AT_CHTL, "AT_CHTL"},
        {TokenType::AT_CJMOD, "AT_CJMOD"},
        {TokenType::AT_CONFIG, "AT_CONFIG"},
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
    return type >= TokenType::TEXT && type <= TokenType::EXCEPT;
}

bool Token::IsSymbol() const {
    return type >= TokenType::LEFT_BRACE && type <= TokenType::SLASH;
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

} // namespace CHTL