#include "Token.h"
#include <unordered_set>
#include <sstream>

namespace CHTL {

std::string Token::getTypeString() const {
    switch (type) {
        case TokenType::LEFT_BRACE: return "LEFT_BRACE";
        case TokenType::RIGHT_BRACE: return "RIGHT_BRACE";
        case TokenType::LEFT_BRACKET: return "LEFT_BRACKET";
        case TokenType::RIGHT_BRACKET: return "RIGHT_BRACKET";
        case TokenType::LEFT_PAREN: return "LEFT_PAREN";
        case TokenType::RIGHT_PAREN: return "RIGHT_PAREN";
        case TokenType::SEMICOLON: return "SEMICOLON";
        case TokenType::COLON: return "COLON";
        case TokenType::EQUAL: return "EQUAL";
        case TokenType::COMMA: return "COMMA";
        case TokenType::DOT: return "DOT";
        
        case TokenType::DOUBLE_QUOTED_STRING: return "DOUBLE_QUOTED_STRING";
        case TokenType::SINGLE_QUOTED_STRING: return "SINGLE_QUOTED_STRING";
        case TokenType::UNQUOTED_LITERAL: return "UNQUOTED_LITERAL";
        
        case TokenType::LINE_COMMENT: return "LINE_COMMENT";
        case TokenType::BLOCK_COMMENT: return "BLOCK_COMMENT";
        case TokenType::GENERATOR_COMMENT: return "GENERATOR_COMMENT";
        
        case TokenType::TEXT: return "TEXT";
        case TokenType::STYLE: return "STYLE";
        case TokenType::HTML5: return "HTML5";
        case TokenType::USE: return "USE";
        case TokenType::INHERIT: return "INHERIT";
        case TokenType::DELETE: return "DELETE";
        case TokenType::INSERT: return "INSERT";
        case TokenType::AFTER: return "AFTER";
        case TokenType::BEFORE: return "BEFORE";
        case TokenType::REPLACE: return "REPLACE";
        case TokenType::AT_TOP: return "AT_TOP";
        case TokenType::AT_BOTTOM: return "AT_BOTTOM";
        case TokenType::FROM: return "FROM";
        case TokenType::AS: return "AS";
        case TokenType::EXCEPT: return "EXCEPT";
        
        case TokenType::TEMPLATE: return "TEMPLATE";
        case TokenType::CUSTOM: return "CUSTOM";
        case TokenType::ORIGIN: return "ORIGIN";
        case TokenType::IMPORT: return "IMPORT";
        case TokenType::NAMESPACE: return "NAMESPACE";
        case TokenType::CONFIGURATION: return "CONFIGURATION";
        case TokenType::INFO: return "INFO";
        case TokenType::EXPORT: return "EXPORT";
        case TokenType::NAME: return "NAME";
        case TokenType::ORIGIN_TYPE: return "ORIGIN_TYPE";
        
        case TokenType::AT_STYLE: return "AT_STYLE";
        case TokenType::AT_ELEMENT: return "AT_ELEMENT";
        case TokenType::AT_VAR: return "AT_VAR";
        case TokenType::AT_HTML: return "AT_HTML";
        case TokenType::AT_JAVASCRIPT: return "AT_JAVASCRIPT";
        case TokenType::AT_CONFIG: return "AT_CONFIG";
        case TokenType::AT_CHTL: return "AT_CHTL";
        case TokenType::AT_CJMOD: return "AT_CJMOD";
        
        case TokenType::CLASS_SELECTOR: return "CLASS_SELECTOR";
        case TokenType::ID_SELECTOR: return "ID_SELECTOR";
        case TokenType::AMPERSAND: return "AMPERSAND";
        
        case TokenType::HTML_ELEMENT: return "HTML_ELEMENT";
        case TokenType::IDENTIFIER: return "IDENTIFIER";
        case TokenType::INDEX_ACCESS: return "INDEX_ACCESS";
        
        case TokenType::NEWLINE: return "NEWLINE";
        case TokenType::WHITESPACE: return "WHITESPACE";
        case TokenType::EOF_TOKEN: return "EOF_TOKEN";
        case TokenType::UNKNOWN: return "UNKNOWN";
        case TokenType::INVALID: return "INVALID";
        
        default: return "UNKNOWN";
    }
}

bool Token::isKeyword() const {
    static const std::unordered_set<TokenType> keywords = {
        TokenType::TEXT, TokenType::STYLE, TokenType::HTML5, TokenType::USE,
        TokenType::INHERIT, TokenType::DELETE, TokenType::INSERT, TokenType::AFTER,
        TokenType::BEFORE, TokenType::REPLACE, TokenType::AT_TOP, TokenType::AT_BOTTOM,
        TokenType::FROM, TokenType::AS, TokenType::EXCEPT,
        TokenType::TEMPLATE, TokenType::CUSTOM, TokenType::ORIGIN, TokenType::IMPORT,
        TokenType::NAMESPACE, TokenType::CONFIGURATION, TokenType::INFO, TokenType::EXPORT,
        TokenType::NAME, TokenType::ORIGIN_TYPE
    };
    return keywords.find(type) != keywords.end();
}

bool Token::isLiteral() const {
    return type == TokenType::DOUBLE_QUOTED_STRING ||
           type == TokenType::SINGLE_QUOTED_STRING ||
           type == TokenType::UNQUOTED_LITERAL;
}

bool Token::isComment() const {
    return type == TokenType::LINE_COMMENT ||
           type == TokenType::BLOCK_COMMENT ||
           type == TokenType::GENERATOR_COMMENT;
}

bool Token::isSelector() const {
    return type == TokenType::CLASS_SELECTOR ||
           type == TokenType::ID_SELECTOR ||
           type == TokenType::AMPERSAND;
}

bool Token::isTemplateCustomType() const {
    return type == TokenType::AT_STYLE ||
           type == TokenType::AT_ELEMENT ||
           type == TokenType::AT_VAR ||
           type == TokenType::AT_HTML ||
           type == TokenType::AT_JAVASCRIPT ||
           type == TokenType::AT_CONFIG ||
           type == TokenType::AT_CHTL ||
           type == TokenType::AT_CJMOD;
}

bool Token::isHtmlElement() const {
    return type == TokenType::HTML_ELEMENT;
}

bool Token::isWhitespace() const {
    return type == TokenType::WHITESPACE || type == TokenType::NEWLINE;
}

bool Token::isValid() const {
    return type != TokenType::UNKNOWN && type != TokenType::INVALID;
}

std::string Token::toString() const {
    std::stringstream ss;
    ss << "[" << getTypeString() << "] '" << value << "' at " 
       << position.line << ":" << position.column;
    return ss.str();
}

} // namespace CHTL