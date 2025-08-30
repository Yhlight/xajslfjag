#include "Token.h"

namespace CHTL {

bool Token::isKeyword() const {
    return type == TokenType::TEXT || type == TokenType::STYLE || type == TokenType::SCRIPT ||
           type == TokenType::TEMPLATE || type == TokenType::CUSTOM || type == TokenType::ORIGIN ||
           type == TokenType::IMPORT || type == TokenType::CONFIGURATION || type == TokenType::NAMESPACE ||
           type == TokenType::INHERIT || type == TokenType::DELETE || type == TokenType::INSERT ||
           type == TokenType::FROM || type == TokenType::AS || type == TokenType::EXCEPT ||
           type == TokenType::USE || type == TokenType::HTML5 || type == TokenType::CHTL ||
           type == TokenType::CJMOD || type == TokenType::CONFIG || type == TokenType::MODULE ||
           type == TokenType::LISTEN || type == TokenType::DELEGATE || type == TokenType::ANIMATE ||
           type == TokenType::VIR || type == TokenType::LOAD;
}

bool Token::isOperator() const {
    return type == TokenType::COLON || type == TokenType::EQUALS || type == TokenType::ARROW ||
           type == TokenType::DOUBLE_COLON || type == TokenType::EVENT_BIND;
}

bool Token::isLiteral() const {
    return type == TokenType::LITERAL_STRING || type == TokenType::LITERAL_UNQUOTED ||
           type == TokenType::LITERAL_NUMBER;
}

bool Token::isIdentifier() const {
    return type == TokenType::IDENTIFIER || type == TokenType::HTML_ELEMENT;
}

bool Token::isSymbol() const {
    return type == TokenType::LBRACE || type == TokenType::RBRACE ||
           type == TokenType::LBRACKET || type == TokenType::RBRACKET ||
           type == TokenType::LPAREN || type == TokenType::RPAREN ||
           type == TokenType::SEMICOLON || type == TokenType::COMMA ||
           type == TokenType::DOT || type == TokenType::HASH ||
           type == TokenType::AMPERSAND || type == TokenType::ASTERISK;
}

bool Token::isComment() const {
    return type == TokenType::COMMENT_LINE || type == TokenType::COMMENT_BLOCK ||
           type == TokenType::COMMENT_GENERATOR;
}

bool Token::isWhitespace() const {
    return type == TokenType::WHITESPACE || type == TokenType::NEWLINE;
}

String Token::toString() const {
    return "Token{" + tokenTypeToString(type) + ", \"" + value + "\", " +
           std::to_string(position.line) + ":" + std::to_string(position.column) + "}";
}

String Token::tokenTypeToString(TokenType type) {
    switch (type) {
        case TokenType::IDENTIFIER: return "IDENTIFIER";
        case TokenType::LITERAL_STRING: return "LITERAL_STRING";
        case TokenType::LITERAL_UNQUOTED: return "LITERAL_UNQUOTED";
        case TokenType::LITERAL_NUMBER: return "LITERAL_NUMBER";
        case TokenType::HTML_ELEMENT: return "HTML_ELEMENT";
        case TokenType::TEXT: return "TEXT";
        case TokenType::STYLE: return "STYLE";
        case TokenType::SCRIPT: return "SCRIPT";
        case TokenType::TEMPLATE: return "TEMPLATE";
        case TokenType::CUSTOM: return "CUSTOM";
        case TokenType::AT_STYLE: return "AT_STYLE";
        case TokenType::AT_ELEMENT: return "AT_ELEMENT";
        case TokenType::AT_VAR: return "AT_VAR";
        case TokenType::ORIGIN: return "ORIGIN";
        case TokenType::AT_HTML: return "AT_HTML";
        case TokenType::AT_JAVASCRIPT: return "AT_JAVASCRIPT";
        case TokenType::IMPORT: return "IMPORT";
        case TokenType::FROM: return "FROM";
        case TokenType::AS: return "AS";
        case TokenType::EXCEPT: return "EXCEPT";
        case TokenType::CHTL: return "CHTL";
        case TokenType::CJMOD: return "CJMOD";
        case TokenType::CONFIG: return "CONFIG";
        case TokenType::CONFIGURATION: return "CONFIGURATION";
        case TokenType::NAMESPACE: return "NAMESPACE";
        case TokenType::MODULE: return "MODULE";
        case TokenType::LISTEN: return "LISTEN";
        case TokenType::DELEGATE: return "DELEGATE";
        case TokenType::ANIMATE: return "ANIMATE";
        case TokenType::VIR: return "VIR";
        case TokenType::LOAD: return "LOAD";
        case TokenType::INHERIT: return "INHERIT";
        case TokenType::DELETE: return "DELETE";
        case TokenType::INSERT: return "INSERT";
        case TokenType::LBRACE: return "LBRACE";
        case TokenType::RBRACE: return "RBRACE";
        case TokenType::SEMICOLON: return "SEMICOLON";
        case TokenType::COLON: return "COLON";
        case TokenType::EQUALS: return "EQUALS";
        case TokenType::EOF_TOKEN: return "EOF";
        default: return "UNKNOWN";
    }
}

} // namespace CHTL