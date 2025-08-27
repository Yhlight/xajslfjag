#include "Token.h"

namespace CHTL {

std::string Token::getTypeString() const {
    switch (type) {
        case TokenType::TEXT: return "TEXT";
        case TokenType::STRING_LITERAL: return "STRING_LITERAL";
        case TokenType::SINGLE_STRING_LITERAL: return "SINGLE_STRING_LITERAL";
        case TokenType::UNQUOTED_LITERAL: return "UNQUOTED_LITERAL";
        case TokenType::NUMBER: return "NUMBER";
        case TokenType::IDENTIFIER: return "IDENTIFIER";
        case TokenType::KEYWORD_TEXT: return "KEYWORD_TEXT";
        case TokenType::KEYWORD_STYLE: return "KEYWORD_STYLE";
        case TokenType::KEYWORD_SCRIPT: return "KEYWORD_SCRIPT";
        case TokenType::KEYWORD_CUSTOM: return "KEYWORD_CUSTOM";
        case TokenType::KEYWORD_TEMPLATE: return "KEYWORD_TEMPLATE";
        case TokenType::KEYWORD_ORIGIN: return "KEYWORD_ORIGIN";
        case TokenType::KEYWORD_IMPORT: return "KEYWORD_IMPORT";
        case TokenType::KEYWORD_NAMESPACE: return "KEYWORD_NAMESPACE";
        case TokenType::KEYWORD_CONFIGURATION: return "KEYWORD_CONFIGURATION";
        case TokenType::KEYWORD_INFO: return "KEYWORD_INFO";
        case TokenType::KEYWORD_EXPORT: return "KEYWORD_EXPORT";
        case TokenType::KEYWORD_USE: return "KEYWORD_USE";
        case TokenType::KEYWORD_DELETE: return "KEYWORD_DELETE";
        case TokenType::KEYWORD_INSERT: return "KEYWORD_INSERT";
        case TokenType::KEYWORD_INHERIT: return "KEYWORD_INHERIT";
        case TokenType::KEYWORD_FROM: return "KEYWORD_FROM";
        case TokenType::KEYWORD_AS: return "KEYWORD_AS";
        case TokenType::KEYWORD_EXCEPT: return "KEYWORD_EXCEPT";
        case TokenType::KEYWORD_AFTER: return "KEYWORD_AFTER";
        case TokenType::KEYWORD_BEFORE: return "KEYWORD_BEFORE";
        case TokenType::KEYWORD_REPLACE: return "KEYWORD_REPLACE";
        case TokenType::KEYWORD_AT: return "KEYWORD_AT";
        case TokenType::KEYWORD_TOP: return "KEYWORD_TOP";
        case TokenType::KEYWORD_BOTTOM: return "KEYWORD_BOTTOM";
        case TokenType::AT: return "AT";
        case TokenType::DOT: return "DOT";
        case TokenType::COLON: return "COLON";
        case TokenType::SEMICOLON: return "SEMICOLON";
        case TokenType::COMMA: return "COMMA";
        case TokenType::EQUALS: return "EQUALS";
        case TokenType::LEFT_BRACE: return "LEFT_BRACE";
        case TokenType::RIGHT_BRACE: return "RIGHT_BRACE";
        case TokenType::LEFT_BRACKET: return "LEFT_BRACKET";
        case TokenType::RIGHT_BRACKET: return "RIGHT_BRACKET";
        case TokenType::LEFT_PAREN: return "LEFT_PAREN";
        case TokenType::RIGHT_PAREN: return "RIGHT_PAREN";
        case TokenType::AMPERSAND: return "AMPERSAND";
        case TokenType::HASH: return "HASH";
        case TokenType::COMMENT_SINGLE: return "COMMENT_SINGLE";
        case TokenType::COMMENT_MULTI: return "COMMENT_MULTI";
        case TokenType::COMMENT_GENERATE: return "COMMENT_GENERATE";
        case TokenType::NEWLINE: return "NEWLINE";
        case TokenType::WHITESPACE: return "WHITESPACE";
        case TokenType::EOF_TOKEN: return "EOF_TOKEN";
        case TokenType::UNKNOWN: return "UNKNOWN";
        default: return "UNDEFINED";
    }
}

bool Token::isKeyword() const {
    return type >= TokenType::KEYWORD_TEXT && type <= TokenType::KEYWORD_BOTTOM;
}

bool Token::isLiteral() const {
    return type >= TokenType::TEXT && type <= TokenType::NUMBER;
}

bool Token::isSymbol() const {
    return type >= TokenType::AT && type <= TokenType::HASH;
}

bool Token::isComment() const {
    return type >= TokenType::COMMENT_SINGLE && type <= TokenType::COMMENT_GENERATE;
}

} // namespace CHTL