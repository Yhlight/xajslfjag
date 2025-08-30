#include "Token.h"
#include <sstream>

namespace CHTLJS {

std::string Position::toString() const {
    std::ostringstream oss;
    oss << line << ":" << column;
    return oss.str();
}

bool Token::isKeyword() const {
    return type == TokenType::SCRIPT || type == TokenType::MODULE || 
           type == TokenType::LOAD || type == TokenType::LISTEN ||
           type == TokenType::DELEGATE || type == TokenType::ANIMATE ||
           type == TokenType::VIR;
}

bool Token::isOperator() const {
    return type == TokenType::ARROW || type == TokenType::EVENT_BINDING ||
           type == TokenType::EQUAL || type == TokenType::COLON;
}

bool Token::isLiteral() const {
    return type == TokenType::DOUBLE_QUOTED_STRING || 
           type == TokenType::SINGLE_QUOTED_STRING ||
           type == TokenType::UNQUOTED_LITERAL;
}

bool Token::isSelector() const {
    return type == TokenType::CLASS_SELECTOR || type == TokenType::ID_SELECTOR ||
           type == TokenType::AMPERSAND;
}

bool Token::isEvent() const {
    return type == TokenType::CLICK || type == TokenType::MOUSEENTER ||
           type == TokenType::MOUSELEAVE || type == TokenType::KEYDOWN ||
           type == TokenType::KEYUP || type == TokenType::SUBMIT ||
           type == TokenType::CHANGE || type == TokenType::FOCUS ||
           type == TokenType::BLUR;
}

std::string Token::toString() const {
    std::ostringstream oss;
    oss << getTypeName() << "(\"" << value << "\") at " << position.toString();
    return oss.str();
}

std::string Token::getTypeName() const {
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
        case TokenType::ARROW: return "ARROW";
        case TokenType::ENHANCED_SELECTOR_START: return "ENHANCED_SELECTOR_START";
        case TokenType::ENHANCED_SELECTOR_END: return "ENHANCED_SELECTOR_END";
        case TokenType::EVENT_BINDING: return "EVENT_BINDING";
        case TokenType::DOUBLE_QUOTED_STRING: return "DOUBLE_QUOTED_STRING";
        case TokenType::SINGLE_QUOTED_STRING: return "SINGLE_QUOTED_STRING";
        case TokenType::UNQUOTED_LITERAL: return "UNQUOTED_LITERAL";
        case TokenType::LINE_COMMENT: return "LINE_COMMENT";
        case TokenType::BLOCK_COMMENT: return "BLOCK_COMMENT";
        case TokenType::GENERATOR_COMMENT: return "GENERATOR_COMMENT";
        case TokenType::SCRIPT: return "SCRIPT";
        case TokenType::MODULE: return "MODULE";
        case TokenType::LOAD: return "LOAD";
        case TokenType::LISTEN: return "LISTEN";
        case TokenType::DELEGATE: return "DELEGATE";
        case TokenType::ANIMATE: return "ANIMATE";
        case TokenType::VIR: return "VIR";
        case TokenType::CLICK: return "CLICK";
        case TokenType::MOUSEENTER: return "MOUSEENTER";
        case TokenType::MOUSELEAVE: return "MOUSELEAVE";
        case TokenType::KEYDOWN: return "KEYDOWN";
        case TokenType::KEYUP: return "KEYUP";
        case TokenType::SUBMIT: return "SUBMIT";
        case TokenType::CHANGE: return "CHANGE";
        case TokenType::FOCUS: return "FOCUS";
        case TokenType::BLUR: return "BLUR";
        case TokenType::CLASS_SELECTOR: return "CLASS_SELECTOR";
        case TokenType::ID_SELECTOR: return "ID_SELECTOR";
        case TokenType::AMPERSAND: return "AMPERSAND";
        case TokenType::IDENTIFIER: return "IDENTIFIER";
        case TokenType::EOF_TOKEN: return "EOF_TOKEN";
        case TokenType::UNKNOWN_TOKEN: return "UNKNOWN_TOKEN";
        case TokenType::INVALID_TOKEN: return "INVALID_TOKEN";
        default: return "UNKNOWN";
    }
}

} // namespace CHTLJS