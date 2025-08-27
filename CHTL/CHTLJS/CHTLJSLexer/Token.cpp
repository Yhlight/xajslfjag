#include "Token.h"

namespace CHTLJS {

std::string Token::getTypeString() const {
    switch (type) {
        case TokenType::STRING_LITERAL: return "STRING_LITERAL";
        case TokenType::NUMBER_LITERAL: return "NUMBER_LITERAL";
        case TokenType::BOOLEAN_LITERAL: return "BOOLEAN_LITERAL";
        case TokenType::NULL_LITERAL: return "NULL_LITERAL";
        case TokenType::IDENTIFIER: return "IDENTIFIER";
        case TokenType::KEYWORD_CONST: return "KEYWORD_CONST";
        case TokenType::KEYWORD_LET: return "KEYWORD_LET";
        case TokenType::KEYWORD_VAR: return "KEYWORD_VAR";
        case TokenType::KEYWORD_FUNCTION: return "KEYWORD_FUNCTION";
        case TokenType::KEYWORD_RETURN: return "KEYWORD_RETURN";
        case TokenType::KEYWORD_IF: return "KEYWORD_IF";
        case TokenType::KEYWORD_ELSE: return "KEYWORD_ELSE";
        case TokenType::KEYWORD_FOR: return "KEYWORD_FOR";
        case TokenType::KEYWORD_WHILE: return "KEYWORD_WHILE";
        case TokenType::KEYWORD_DO: return "KEYWORD_DO";
        case TokenType::KEYWORD_BREAK: return "KEYWORD_BREAK";
        case TokenType::KEYWORD_CONTINUE: return "KEYWORD_CONTINUE";
        case TokenType::KEYWORD_TRUE: return "KEYWORD_TRUE";
        case TokenType::KEYWORD_FALSE: return "KEYWORD_FALSE";
        case TokenType::KEYWORD_NULL: return "KEYWORD_NULL";
        case TokenType::KEYWORD_VIR: return "KEYWORD_VIR";
        case TokenType::KEYWORD_LISTEN: return "KEYWORD_LISTEN";
        case TokenType::KEYWORD_ANIMATE: return "KEYWORD_ANIMATE";
        case TokenType::KEYWORD_DELEGATE: return "KEYWORD_DELEGATE";
        case TokenType::KEYWORD_TARGET: return "KEYWORD_TARGET";
        case TokenType::KEYWORD_DURATION: return "KEYWORD_DURATION";
        case TokenType::KEYWORD_EASING: return "KEYWORD_EASING";
        case TokenType::KEYWORD_BEGIN: return "KEYWORD_BEGIN";
        case TokenType::KEYWORD_WHEN: return "KEYWORD_WHEN";
        case TokenType::KEYWORD_END: return "KEYWORD_END";
        case TokenType::KEYWORD_AT: return "KEYWORD_AT";
        case TokenType::KEYWORD_LOOP: return "KEYWORD_LOOP";
        case TokenType::KEYWORD_DIRECTION: return "KEYWORD_DIRECTION";
        case TokenType::KEYWORD_DELAY: return "KEYWORD_DELAY";
        case TokenType::KEYWORD_CALLBACK: return "KEYWORD_CALLBACK";
        case TokenType::PLUS: return "PLUS";
        case TokenType::MINUS: return "MINUS";
        case TokenType::MULTIPLY: return "MULTIPLY";
        case TokenType::DIVIDE: return "DIVIDE";
        case TokenType::MODULO: return "MODULO";
        case TokenType::ASSIGN: return "ASSIGN";
        case TokenType::PLUS_ASSIGN: return "PLUS_ASSIGN";
        case TokenType::MINUS_ASSIGN: return "MINUS_ASSIGN";
        case TokenType::MULTIPLY_ASSIGN: return "MULTIPLY_ASSIGN";
        case TokenType::DIVIDE_ASSIGN: return "DIVIDE_ASSIGN";
        case TokenType::MODULO_ASSIGN: return "MODULO_ASSIGN";
        case TokenType::EQUALS: return "EQUALS";
        case TokenType::NOT_EQUALS: return "NOT_EQUALS";
        case TokenType::STRICT_EQUALS: return "STRICT_EQUALS";
        case TokenType::STRICT_NOT_EQUALS: return "STRICT_NOT_EQUALS";
        case TokenType::LESS_THAN: return "LESS_THAN";
        case TokenType::GREATER_THAN: return "GREATER_THAN";
        case TokenType::LESS_EQUALS: return "LESS_EQUALS";
        case TokenType::GREATER_EQUALS: return "GREATER_EQUALS";
        case TokenType::LOGICAL_AND: return "LOGICAL_AND";
        case TokenType::LOGICAL_OR: return "LOGICAL_OR";
        case TokenType::LOGICAL_NOT: return "LOGICAL_NOT";
        case TokenType::BITWISE_AND: return "BITWISE_AND";
        case TokenType::BITWISE_OR: return "BITWISE_OR";
        case TokenType::BITWISE_XOR: return "BITWISE_XOR";
        case TokenType::BITWISE_NOT: return "BITWISE_NOT";
        case TokenType::LEFT_SHIFT: return "LEFT_SHIFT";
        case TokenType::RIGHT_SHIFT: return "RIGHT_SHIFT";
        case TokenType::UNSIGNED_RIGHT_SHIFT: return "UNSIGNED_RIGHT_SHIFT";
        case TokenType::INCREMENT: return "INCREMENT";
        case TokenType::DECREMENT: return "DECREMENT";
        case TokenType::QUESTION: return "QUESTION";
        case TokenType::ARROW: return "ARROW";
        case TokenType::DOUBLE_ARROW: return "DOUBLE_ARROW";
        case TokenType::BIND_ARROW: return "BIND_ARROW";
        case TokenType::LEFT_PAREN: return "LEFT_PAREN";
        case TokenType::RIGHT_PAREN: return "RIGHT_PAREN";
        case TokenType::LEFT_BRACE: return "LEFT_BRACE";
        case TokenType::RIGHT_BRACE: return "RIGHT_BRACE";
        case TokenType::LEFT_BRACKET: return "LEFT_BRACKET";
        case TokenType::RIGHT_BRACKET: return "RIGHT_BRACKET";
        case TokenType::SEMICOLON: return "SEMICOLON";
        case TokenType::COMMA: return "COMMA";
        case TokenType::DOT: return "DOT";
        case TokenType::COLON: return "COLON";
        case TokenType::DOUBLE_LEFT_BRACE: return "DOUBLE_LEFT_BRACE";
        case TokenType::DOUBLE_RIGHT_BRACE: return "DOUBLE_RIGHT_BRACE";
        case TokenType::NEWLINE: return "NEWLINE";
        case TokenType::WHITESPACE: return "WHITESPACE";
        case TokenType::COMMENT: return "COMMENT";
        case TokenType::EOF_TOKEN: return "EOF_TOKEN";
        case TokenType::UNKNOWN: return "UNKNOWN";
        default: return "UNDEFINED";
    }
}

bool Token::isKeyword() const {
    return type >= TokenType::KEYWORD_CONST && type <= TokenType::KEYWORD_CALLBACK;
}

bool Token::isLiteral() const {
    return type >= TokenType::STRING_LITERAL && type <= TokenType::NULL_LITERAL;
}

bool Token::isOperator() const {
    return type >= TokenType::PLUS && type <= TokenType::BIND_ARROW;
}

bool Token::isAssignmentOperator() const {
    return type >= TokenType::ASSIGN && type <= TokenType::MODULO_ASSIGN;
}

bool Token::isComparisonOperator() const {
    return type >= TokenType::EQUALS && type <= TokenType::GREATER_EQUALS;
}

bool Token::isLogicalOperator() const {
    return type >= TokenType::LOGICAL_AND && type <= TokenType::LOGICAL_NOT;
}

bool Token::isCHTLJSFeature() const {
    return type == TokenType::KEYWORD_VIR ||
           type == TokenType::KEYWORD_LISTEN ||
           type == TokenType::KEYWORD_ANIMATE ||
           type == TokenType::KEYWORD_DELEGATE ||
           type == TokenType::ARROW ||
           type == TokenType::BIND_ARROW ||
           type == TokenType::DOUBLE_LEFT_BRACE ||
           type == TokenType::DOUBLE_RIGHT_BRACE;
}

} // namespace CHTLJS