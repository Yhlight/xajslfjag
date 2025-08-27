#include "Token.h"
#include <sstream>

namespace CHTLJS {

std::string Token::getTypeString() const {
    switch (type_) {
        case TokenType::IDENTIFIER: return "IDENTIFIER";
        case TokenType::STRING: return "STRING";
        case TokenType::NUMBER: return "NUMBER";
        case TokenType::LITERAL: return "LITERAL";
        
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
        
        case TokenType::ARROW: return "ARROW";
        case TokenType::BIND_OPERATOR: return "BIND_OPERATOR";
        case TokenType::SELECTOR_START: return "SELECTOR_START";
        case TokenType::SELECTOR_END: return "SELECTOR_END";
        
        case TokenType::SINGLE_COMMENT: return "SINGLE_COMMENT";
        case TokenType::MULTI_COMMENT: return "MULTI_COMMENT";
        
        case TokenType::SCRIPT: return "SCRIPT";
        case TokenType::FUNCTION: return "FUNCTION";
        case TokenType::CONST: return "CONST";
        case TokenType::LET: return "LET";
        case TokenType::VAR: return "VAR";
        case TokenType::IF: return "IF";
        case TokenType::ELSE: return "ELSE";
        case TokenType::FOR: return "FOR";
        case TokenType::WHILE: return "WHILE";
        case TokenType::RETURN: return "RETURN";
        case TokenType::TRUE: return "TRUE";
        case TokenType::FALSE: return "FALSE";
        case TokenType::NULL_TOKEN: return "NULL";
        case TokenType::UNDEFINED: return "UNDEFINED";
        
        case TokenType::LISTEN: return "LISTEN";
        case TokenType::DELEGATE: return "DELEGATE";
        case TokenType::ANIMATE: return "ANIMATE";
        case TokenType::VIR: return "VIR";
        case TokenType::WHEN: return "WHEN";
        case TokenType::BEGIN: return "BEGIN";
        case TokenType::END: return "END";
        case TokenType::AT: return "AT";
        case TokenType::TARGET: return "TARGET";
        case TokenType::DURATION: return "DURATION";
        case TokenType::EASING: return "EASING";
        case TokenType::LOOP: return "LOOP";
        case TokenType::DIRECTION: return "DIRECTION";
        case TokenType::DELAY: return "DELAY";
        case TokenType::CALLBACK: return "CALLBACK";
        
        case TokenType::NEWLINE: return "NEWLINE";
        case TokenType::WHITESPACE: return "WHITESPACE";
        case TokenType::EOF_TOKEN: return "EOF_TOKEN";
        case TokenType::UNKNOWN: return "UNKNOWN";
        
        default: return "UNKNOWN";
    }
}

std::string Token::toString() const {
    std::ostringstream oss;
    oss << "Token{type=" << getTypeString() 
        << ", value=\"" << value_ << "\""
        << ", start=(" << start_pos_.line << ":" << start_pos_.column << ")"
        << ", end=(" << end_pos_.line << ":" << end_pos_.column << ")"
        << "}";
    return oss.str();
}

} // namespace CHTLJS