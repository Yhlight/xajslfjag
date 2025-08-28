#include "CHTLJSToken.h"
#include <unordered_set>

namespace CHTLJS {

CHTLJSToken::CHTLJSToken(CHTLJSTokenType type, const std::string& value, 
                         size_t line, size_t column, size_t position)
    : type(type), value(value), line(line), column(column), position(position) {
}

bool CHTLJSToken::Is(CHTLJSTokenType tokenType) const {
    return type == tokenType;
}

bool CHTLJSToken::IsKeyword() const {
    static const std::unordered_set<CHTLJSTokenType> keywords = {
        CHTLJSTokenType::MODULE, CHTLJSTokenType::LOAD, CHTLJSTokenType::LISTEN,
        CHTLJSTokenType::DELEGATE, CHTLJSTokenType::ANIMATE, CHTLJSTokenType::VIR,
        CHTLJSTokenType::TARGET, CHTLJSTokenType::DURATION, CHTLJSTokenType::EASING,
        CHTLJSTokenType::BEGIN, CHTLJSTokenType::WHEN, CHTLJSTokenType::END,
        CHTLJSTokenType::LOOP, CHTLJSTokenType::DIRECTION, CHTLJSTokenType::DELAY,
        CHTLJSTokenType::CALLBACK, CHTLJSTokenType::AT,
        CHTLJSTokenType::FUNCTION, CHTLJSTokenType::CONST, CHTLJSTokenType::LET,
        CHTLJSTokenType::VAR, CHTLJSTokenType::IF, CHTLJSTokenType::ELSE,
        CHTLJSTokenType::FOR, CHTLJSTokenType::WHILE, CHTLJSTokenType::RETURN
    };
    return keywords.find(type) != keywords.end();
}

bool CHTLJSToken::IsOperator() const {
    static const std::unordered_set<CHTLJSTokenType> operators = {
        CHTLJSTokenType::COLON, CHTLJSTokenType::EQUALS, CHTLJSTokenType::DOT,
        CHTLJSTokenType::HASH, CHTLJSTokenType::AMPERSAND, CHTLJSTokenType::ASTERISK,
        CHTLJSTokenType::SLASH, CHTLJSTokenType::ARROW, CHTLJSTokenType::EVENT_BIND
    };
    return operators.find(type) != operators.end();
}

bool CHTLJSToken::IsSelector() const {
    return type == CHTLJSTokenType::CLASS_SELECTOR || 
           type == CHTLJSTokenType::ID_SELECTOR || 
           type == CHTLJSTokenType::REFERENCE_SELECTOR ||
           type == CHTLJSTokenType::SELECTOR_START ||
           type == CHTLJSTokenType::SELECTOR_END;
}

bool CHTLJSToken::IsComment() const {
    return type == CHTLJSTokenType::SINGLE_LINE_COMMENT ||
           type == CHTLJSTokenType::MULTI_LINE_COMMENT ||
           type == CHTLJSTokenType::GENERATOR_COMMENT;
}

bool CHTLJSToken::IsCHTLJSSpecific() const {
    static const std::unordered_set<CHTLJSTokenType> chtljsSpecific = {
        CHTLJSTokenType::MODULE, CHTLJSTokenType::LOAD, CHTLJSTokenType::LISTEN,
        CHTLJSTokenType::DELEGATE, CHTLJSTokenType::ANIMATE, CHTLJSTokenType::VIR,
        CHTLJSTokenType::SELECTOR_START, CHTLJSTokenType::SELECTOR_END,
        CHTLJSTokenType::EVENT_BIND, CHTLJSTokenType::REFERENCE_SELECTOR
    };
    return chtljsSpecific.find(type) != chtljsSpecific.end();
}

std::string CHTLJSToken::ToString() const {
    return TypeToString(type) + ": \"" + value + "\" at (" + 
           std::to_string(line) + "," + std::to_string(column) + ")";
}

std::string CHTLJSToken::TypeToString(CHTLJSTokenType type) {
    switch (type) {
        case CHTLJSTokenType::IDENTIFIER: return "IDENTIFIER";
        case CHTLJSTokenType::STRING_LITERAL: return "STRING_LITERAL";
        case CHTLJSTokenType::UNQUOTED_LITERAL: return "UNQUOTED_LITERAL";
        case CHTLJSTokenType::NUMERIC_LITERAL: return "NUMERIC_LITERAL";
        case CHTLJSTokenType::MODULE: return "MODULE";
        case CHTLJSTokenType::LOAD: return "LOAD";
        case CHTLJSTokenType::LISTEN: return "LISTEN";
        case CHTLJSTokenType::DELEGATE: return "DELEGATE";
        case CHTLJSTokenType::ANIMATE: return "ANIMATE";
        case CHTLJSTokenType::VIR: return "VIR";
        case CHTLJSTokenType::TARGET: return "TARGET";
        case CHTLJSTokenType::DURATION: return "DURATION";
        case CHTLJSTokenType::EASING: return "EASING";
        case CHTLJSTokenType::BEGIN: return "BEGIN";
        case CHTLJSTokenType::WHEN: return "WHEN";
        case CHTLJSTokenType::END: return "END";
        case CHTLJSTokenType::LOOP: return "LOOP";
        case CHTLJSTokenType::DIRECTION: return "DIRECTION";
        case CHTLJSTokenType::DELAY: return "DELAY";
        case CHTLJSTokenType::CALLBACK: return "CALLBACK";
        case CHTLJSTokenType::AT: return "AT";
        case CHTLJSTokenType::SELECTOR_START: return "SELECTOR_START";
        case CHTLJSTokenType::SELECTOR_END: return "SELECTOR_END";
        case CHTLJSTokenType::CLASS_SELECTOR: return "CLASS_SELECTOR";
        case CHTLJSTokenType::ID_SELECTOR: return "ID_SELECTOR";
        case CHTLJSTokenType::REFERENCE_SELECTOR: return "REFERENCE_SELECTOR";
        case CHTLJSTokenType::LEFT_BRACE: return "LEFT_BRACE";
        case CHTLJSTokenType::RIGHT_BRACE: return "RIGHT_BRACE";
        case CHTLJSTokenType::LEFT_BRACKET: return "LEFT_BRACKET";
        case CHTLJSTokenType::RIGHT_BRACKET: return "RIGHT_BRACKET";
        case CHTLJSTokenType::LEFT_PAREN: return "LEFT_PAREN";
        case CHTLJSTokenType::RIGHT_PAREN: return "RIGHT_PAREN";
        case CHTLJSTokenType::SEMICOLON: return "SEMICOLON";
        case CHTLJSTokenType::COMMA: return "COMMA";
        case CHTLJSTokenType::COLON: return "COLON";
        case CHTLJSTokenType::EQUALS: return "EQUALS";
        case CHTLJSTokenType::DOT: return "DOT";
        case CHTLJSTokenType::HASH: return "HASH";
        case CHTLJSTokenType::AMPERSAND: return "AMPERSAND";
        case CHTLJSTokenType::ASTERISK: return "ASTERISK";
        case CHTLJSTokenType::SLASH: return "SLASH";
        case CHTLJSTokenType::ARROW: return "ARROW";
        case CHTLJSTokenType::EVENT_BIND: return "EVENT_BIND";
        case CHTLJSTokenType::FUNCTION: return "FUNCTION";
        case CHTLJSTokenType::CONST: return "CONST";
        case CHTLJSTokenType::LET: return "LET";
        case CHTLJSTokenType::VAR: return "VAR";
        case CHTLJSTokenType::IF: return "IF";
        case CHTLJSTokenType::ELSE: return "ELSE";
        case CHTLJSTokenType::FOR: return "FOR";
        case CHTLJSTokenType::WHILE: return "WHILE";
        case CHTLJSTokenType::RETURN: return "RETURN";
        case CHTLJSTokenType::SINGLE_LINE_COMMENT: return "SINGLE_LINE_COMMENT";
        case CHTLJSTokenType::MULTI_LINE_COMMENT: return "MULTI_LINE_COMMENT";
        case CHTLJSTokenType::GENERATOR_COMMENT: return "GENERATOR_COMMENT";
        case CHTLJSTokenType::WHITESPACE: return "WHITESPACE";
        case CHTLJSTokenType::NEWLINE: return "NEWLINE";
        case CHTLJSTokenType::EOF_TOKEN: return "EOF_TOKEN";
        default: return "UNKNOWN";
    }
}

} // namespace CHTLJS