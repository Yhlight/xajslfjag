#include "CHTLToken.h"
#include <sstream>
#include <map>

namespace CHTL {

CHTLToken::CHTLToken(CHTLTokenType type, const std::string& text, size_t line, size_t column, size_t position)
    : type(type), text(text), line(line), column(column), position(position) {
}

std::string CHTLToken::getTypeName() const {
    static const std::map<CHTLTokenType, std::string> typeNames = {
        {CHTLTokenType::IDENTIFIER, "IDENTIFIER"},
        {CHTLTokenType::STRING_LITERAL, "STRING_LITERAL"},
        {CHTLTokenType::NUMBER_LITERAL, "NUMBER_LITERAL"},
        {CHTLTokenType::UNQUOTED_LITERAL, "UNQUOTED_LITERAL"},
        {CHTLTokenType::LBRACE, "LBRACE"},
        {CHTLTokenType::RBRACE, "RBRACE"},
        {CHTLTokenType::LBRACKET, "LBRACKET"},
        {CHTLTokenType::RBRACKET, "RBRACKET"},
        {CHTLTokenType::LPAREN, "LPAREN"},
        {CHTLTokenType::RPAREN, "RPAREN"},
        {CHTLTokenType::SEMICOLON, "SEMICOLON"},
        {CHTLTokenType::COMMA, "COMMA"},
        {CHTLTokenType::COLON, "COLON"},
        {CHTLTokenType::EQUALS, "EQUALS"},
        {CHTLTokenType::DOT, "DOT"},
        {CHTLTokenType::ARROW, "ARROW"},
        {CHTLTokenType::AMPERSAND, "AMPERSAND"},
        {CHTLTokenType::AMPERSAND_ARROW, "AMPERSAND_ARROW"},
        {CHTLTokenType::SINGLE_LINE_COMMENT, "SINGLE_LINE_COMMENT"},
        {CHTLTokenType::MULTI_LINE_COMMENT, "MULTI_LINE_COMMENT"},
        {CHTLTokenType::GENERATOR_COMMENT, "GENERATOR_COMMENT"},
        {CHTLTokenType::TEXT, "TEXT"},
        {CHTLTokenType::STYLE, "STYLE"},
        {CHTLTokenType::SCRIPT, "SCRIPT"},
        {CHTLTokenType::INHERIT, "INHERIT"},
        {CHTLTokenType::DELETE, "DELETE"},
        {CHTLTokenType::INSERT, "INSERT"},
        {CHTLTokenType::AFTER, "AFTER"},
        {CHTLTokenType::BEFORE, "BEFORE"},
        {CHTLTokenType::REPLACE, "REPLACE"},
        {CHTLTokenType::AT_TOP, "AT_TOP"},
        {CHTLTokenType::AT_BOTTOM, "AT_BOTTOM"},
        {CHTLTokenType::FROM, "FROM"},
        {CHTLTokenType::AS, "AS"},
        {CHTLTokenType::EXCEPT, "EXCEPT"},
        {CHTLTokenType::USE, "USE"},
        {CHTLTokenType::TEMPLATE, "TEMPLATE"},
        {CHTLTokenType::CUSTOM, "CUSTOM"},
        {CHTLTokenType::ORIGIN, "ORIGIN"},
        {CHTLTokenType::IMPORT, "IMPORT"},
        {CHTLTokenType::NAMESPACE, "NAMESPACE"},
        {CHTLTokenType::CONFIGURATION, "CONFIGURATION"},
        {CHTLTokenType::INFO, "INFO"},
        {CHTLTokenType::EXPORT, "EXPORT"},
        {CHTLTokenType::AT_STYLE, "AT_STYLE"},
        {CHTLTokenType::AT_ELEMENT, "AT_ELEMENT"},
        {CHTLTokenType::AT_VAR, "AT_VAR"},
        {CHTLTokenType::AT_HTML, "AT_HTML"},
        {CHTLTokenType::AT_JAVASCRIPT, "AT_JAVASCRIPT"},
        {CHTLTokenType::AT_CHTL, "AT_CHTL"},
        {CHTLTokenType::AT_CJMOD, "AT_CJMOD"},
        {CHTLTokenType::AT_CONFIG, "AT_CONFIG"},
        {CHTLTokenType::AT_VUE, "AT_VUE"},
        {CHTLTokenType::SELECTOR_REF, "SELECTOR_REF"},
        {CHTLTokenType::CLASS_REF, "CLASS_REF"},
        {CHTLTokenType::ID_REF, "ID_REF"},
        {CHTLTokenType::REF_AMPERSAND, "REF_AMPERSAND"},
        {CHTLTokenType::HTML5, "HTML5"},
        {CHTLTokenType::END_OF_FILE, "END_OF_FILE"},
        {CHTLTokenType::ERROR_TOKEN, "ERROR_TOKEN"},
        {CHTLTokenType::WHITESPACE, "WHITESPACE"},
        {CHTLTokenType::NEWLINE, "NEWLINE"}
    };
    
    auto it = typeNames.find(type);
    return it != typeNames.end() ? it->second : "UNKNOWN";
}

bool CHTLToken::isKeyword() const {
    return type >= CHTLTokenType::TEXT && type <= CHTLTokenType::USE;
}

bool CHTLToken::isOperator() const {
    return type >= CHTLTokenType::LBRACE && type <= CHTLTokenType::AMPERSAND_ARROW;
}

bool CHTLToken::isLiteral() const {
    return type >= CHTLTokenType::IDENTIFIER && type <= CHTLTokenType::UNQUOTED_LITERAL;
}

bool CHTLToken::isMarker() const {
    return type >= CHTLTokenType::TEMPLATE && type <= CHTLTokenType::EXPORT;
}

bool CHTLToken::isTypeMarker() const {
    return type >= CHTLTokenType::AT_STYLE && type <= CHTLTokenType::AT_VUE;
}

bool CHTLToken::isSelectorRef() const {
    return type >= CHTLTokenType::SELECTOR_REF && type <= CHTLTokenType::REF_AMPERSAND;
}

bool CHTLToken::isComment() const {
    return type >= CHTLTokenType::SINGLE_LINE_COMMENT && type <= CHTLTokenType::GENERATOR_COMMENT;
}

bool CHTLToken::isWhitespace() const {
    return type == CHTLTokenType::WHITESPACE || type == CHTLTokenType::NEWLINE;
}

std::string CHTLToken::toString() const {
    std::ostringstream oss;
    oss << "CHTLToken{type=" << getTypeName() 
        << ", text=\"" << text << "\""
        << ", line=" << line 
        << ", column=" << column 
        << ", position=" << position << "}";
    return oss.str();
}

} // namespace CHTL