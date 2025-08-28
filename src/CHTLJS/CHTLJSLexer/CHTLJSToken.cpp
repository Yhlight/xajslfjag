#include "CHTLJSToken.h"
#include <sstream>
#include <map>

namespace CHTLJS {

CHTLJSToken::CHTLJSToken(CHTLJSTokenType type, const std::string& text, size_t line, size_t column, size_t position)
    : type(type), text(text), line(line), column(column), position(position) {
}

std::string CHTLJSToken::getTypeName() const {
    static const std::map<CHTLJSTokenType, std::string> typeNames = {
        {CHTLJSTokenType::IDENTIFIER, "IDENTIFIER"},
        {CHTLJSTokenType::STRING_LITERAL, "STRING_LITERAL"},
        {CHTLJSTokenType::NUMBER_LITERAL, "NUMBER_LITERAL"},
        {CHTLJSTokenType::UNQUOTED_LITERAL, "UNQUOTED_LITERAL"},
        {CHTLJSTokenType::LBRACE, "LBRACE"},
        {CHTLJSTokenType::RBRACE, "RBRACE"},
        {CHTLJSTokenType::LBRACKET, "LBRACKET"},
        {CHTLJSTokenType::RBRACKET, "RBRACKET"},
        {CHTLJSTokenType::LPAREN, "LPAREN"},
        {CHTLJSTokenType::RPAREN, "RPAREN"},
        {CHTLJSTokenType::SEMICOLON, "SEMICOLON"},
        {CHTLJSTokenType::COMMA, "COMMA"},
        {CHTLJSTokenType::COLON, "COLON"},
        {CHTLJSTokenType::EQUALS, "EQUALS"},
        {CHTLJSTokenType::DOT, "DOT"},
        {CHTLJSTokenType::ARROW, "ARROW"},
        {CHTLJSTokenType::AMPERSAND, "AMPERSAND"},
        {CHTLJSTokenType::AMPERSAND_ARROW, "AMPERSAND_ARROW"},
        {CHTLJSTokenType::TEMPLATE_LITERAL_START, "TEMPLATE_LITERAL_START"},
        {CHTLJSTokenType::TEMPLATE_LITERAL_END, "TEMPLATE_LITERAL_END"},
        {CHTLJSTokenType::DOLLAR_BRACE, "DOLLAR_BRACE"},
        {CHTLJSTokenType::SINGLE_LINE_COMMENT, "SINGLE_LINE_COMMENT"},
        {CHTLJSTokenType::MULTI_LINE_COMMENT, "MULTI_LINE_COMMENT"},
        {CHTLJSTokenType::GENERATOR_COMMENT, "GENERATOR_COMMENT"},
        {CHTLJSTokenType::MODULE, "MODULE"},
        {CHTLJSTokenType::LOAD, "LOAD"},
        {CHTLJSTokenType::LISTEN, "LISTEN"},
        {CHTLJSTokenType::ANIMATE, "ANIMATE"},
        {CHTLJSTokenType::DELEGATE, "DELEGATE"},
        {CHTLJSTokenType::VIR, "VIR"},
        {CHTLJSTokenType::TARGET, "TARGET"},
        {CHTLJSTokenType::DURATION, "DURATION"},
        {CHTLJSTokenType::EASING, "EASING"},
        {CHTLJSTokenType::BEGIN, "BEGIN"},
        {CHTLJSTokenType::WHEN, "WHEN"},
        {CHTLJSTokenType::END, "END"},
        {CHTLJSTokenType::LOOP, "LOOP"},
        {CHTLJSTokenType::DIRECTION, "DIRECTION"},
        {CHTLJSTokenType::DELAY, "DELAY"},
        {CHTLJSTokenType::CALLBACK, "CALLBACK"},
        {CHTLJSTokenType::AT, "AT"},
        {CHTLJSTokenType::FUNCTION, "FUNCTION"},
        {CHTLJSTokenType::CONST, "CONST"},
        {CHTLJSTokenType::LET, "LET"},
        {CHTLJSTokenType::VAR, "VAR"},
        {CHTLJSTokenType::IF, "IF"},
        {CHTLJSTokenType::ELSE, "ELSE"},
        {CHTLJSTokenType::FOR, "FOR"},
        {CHTLJSTokenType::WHILE, "WHILE"},
        {CHTLJSTokenType::DO, "DO"},
        {CHTLJSTokenType::BREAK, "BREAK"},
        {CHTLJSTokenType::CONTINUE, "CONTINUE"},
        {CHTLJSTokenType::RETURN, "RETURN"},
        {CHTLJSTokenType::SELECTOR_REF, "SELECTOR_REF"},
        {CHTLJSTokenType::CLASS_REF, "CLASS_REF"},
        {CHTLJSTokenType::ID_REF, "ID_REF"},
        {CHTLJSTokenType::REF_AMPERSAND, "REF_AMPERSAND"},
        {CHTLJSTokenType::EVENT_BIND, "EVENT_BIND"},
        {CHTLJSTokenType::END_OF_FILE, "END_OF_FILE"},
        {CHTLJSTokenType::ERROR_TOKEN, "ERROR_TOKEN"},
        {CHTLJSTokenType::WHITESPACE, "WHITESPACE"},
        {CHTLJSTokenType::NEWLINE, "NEWLINE"}
    };
    
    auto it = typeNames.find(type);
    return it != typeNames.end() ? it->second : "UNKNOWN";
}

bool CHTLJSToken::isKeyword() const {
    return isCHTLJSKeyword() || isJavaScriptKeyword();
}

bool CHTLJSToken::isCHTLJSKeyword() const {
    return type >= CHTLJSTokenType::MODULE && type <= CHTLJSTokenType::AT;
}

bool CHTLJSToken::isJavaScriptKeyword() const {
    return type >= CHTLJSTokenType::FUNCTION && type <= CHTLJSTokenType::RETURN;
}

bool CHTLJSToken::isOperator() const {
    return (type >= CHTLJSTokenType::LBRACE && type <= CHTLJSTokenType::DOLLAR_BRACE) ||
           type == CHTLJSTokenType::EVENT_BIND;
}

bool CHTLJSToken::isLiteral() const {
    return type >= CHTLJSTokenType::IDENTIFIER && type <= CHTLJSTokenType::UNQUOTED_LITERAL;
}

bool CHTLJSToken::isSelectorRef() const {
    return type >= CHTLJSTokenType::SELECTOR_REF && type <= CHTLJSTokenType::REF_AMPERSAND;
}

bool CHTLJSToken::isComment() const {
    return type >= CHTLJSTokenType::SINGLE_LINE_COMMENT && type <= CHTLJSTokenType::GENERATOR_COMMENT;
}

bool CHTLJSToken::isWhitespace() const {
    return type == CHTLJSTokenType::WHITESPACE || type == CHTLJSTokenType::NEWLINE;
}

std::string CHTLJSToken::toString() const {
    std::ostringstream oss;
    oss << "CHTLJSToken{type=" << getTypeName() 
        << ", text=\"" << text << "\""
        << ", line=" << line 
        << ", column=" << column 
        << ", position=" << position << "}";
    return oss.str();
}

} // namespace CHTLJS