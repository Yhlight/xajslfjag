#include "CHTLJSToken.h"
#include <sstream>

namespace CHTLJS {

CHTLJSToken::CHTLJSToken(CHTLJSTokenType type, const std::string& value, size_t line, size_t column)
    : type_(type), value_(value), line_(line), column_(column) {}

std::string CHTLJSToken::toString() const {
    std::ostringstream oss;
    oss << "CHTLJSToken{type=";
    
    // 将Token类型转换为字符串
    switch (type_) {
        case CHTLJSTokenType::END_OF_FILE: oss << "END_OF_FILE"; break;
        case CHTLJSTokenType::IDENTIFIER: oss << "IDENTIFIER"; break;
        case CHTLJSTokenType::STRING_LITERAL: oss << "STRING_LITERAL"; break;
        case CHTLJSTokenType::UNQUOTED_LITERAL: oss << "UNQUOTED_LITERAL"; break;
        case CHTLJSTokenType::NUMBER: oss << "NUMBER"; break;
        case CHTLJSTokenType::VIRTUAL_OBJECT: oss << "VIRTUAL_OBJECT"; break;
        case CHTLJSTokenType::ENHANCED_SELECTOR: oss << "ENHANCED_SELECTOR"; break;
        case CHTLJSTokenType::CHAIN_OPERATOR: oss << "CHAIN_OPERATOR"; break;
        case CHTLJSTokenType::EVENT_BINDING: oss << "EVENT_BINDING"; break;
        case CHTLJSTokenType::LISTEN_BLOCK: oss << "LISTEN_BLOCK"; break;
        case CHTLJSTokenType::DELEGATE_BLOCK: oss << "DELEGATE_BLOCK"; break;
        case CHTLJSTokenType::ANIMATE_BLOCK: oss << "ANIMATE_BLOCK"; break;
        case CHTLJSTokenType::I_NEVER_AWAY: oss << "I_NEVER_AWAY"; break;
        case CHTLJSTokenType::PRINT_MY_LOVE: oss << "PRINT_MY_LOVE"; break;
        case CHTLJSTokenType::KEYWORD_VIR: oss << "KEYWORD_VIR"; break;
        case CHTLJSTokenType::KEYWORD_LISTEN: oss << "KEYWORD_LISTEN"; break;
        case CHTLJSTokenType::KEYWORD_DELEGATE: oss << "KEYWORD_DELEGATE"; break;
        case CHTLJSTokenType::KEYWORD_ANIMATE: oss << "KEYWORD_ANIMATE"; break;
        case CHTLJSTokenType::KEYWORD_TARGET: oss << "KEYWORD_TARGET"; break;
        case CHTLJSTokenType::KEYWORD_DURATION: oss << "KEYWORD_DURATION"; break;
        case CHTLJSTokenType::KEYWORD_EASING: oss << "KEYWORD_EASING"; break;
        case CHTLJSTokenType::KEYWORD_BEGIN: oss << "KEYWORD_BEGIN"; break;
        case CHTLJSTokenType::KEYWORD_WHEN: oss << "KEYWORD_WHEN"; break;
        case CHTLJSTokenType::KEYWORD_END: oss << "KEYWORD_END"; break;
        case CHTLJSTokenType::KEYWORD_LOOP: oss << "KEYWORD_LOOP"; break;
        case CHTLJSTokenType::KEYWORD_DIRECTION: oss << "KEYWORD_DIRECTION"; break;
        case CHTLJSTokenType::KEYWORD_DELAY: oss << "KEYWORD_DELAY"; break;
        case CHTLJSTokenType::KEYWORD_CALLBACK: oss << "KEYWORD_CALLBACK"; break;
        case CHTLJSTokenType::KEYWORD_AT: oss << "KEYWORD_AT"; break;
        case CHTLJSTokenType::LEFT_BRACE: oss << "LEFT_BRACE"; break;
        case CHTLJSTokenType::RIGHT_BRACE: oss << "RIGHT_BRACE"; break;
        case CHTLJSTokenType::LEFT_BRACKET: oss << "LEFT_BRACKET"; break;
        case CHTLJSTokenType::RIGHT_BRACKET: oss << "RIGHT_BRACKET"; break;
        case CHTLJSTokenType::LEFT_PAREN: oss << "LEFT_PAREN"; break;
        case CHTLJSTokenType::RIGHT_PAREN: oss << "RIGHT_PAREN"; break;
        case CHTLJSTokenType::SEMICOLON: oss << "SEMICOLON"; break;
        case CHTLJSTokenType::COLON: oss << "COLON"; break;
        case CHTLJSTokenType::EQUAL: oss << "EQUAL"; break;
        case CHTLJSTokenType::COMMA: oss << "COMMA"; break;
        case CHTLJSTokenType::DOT: oss << "DOT"; break;
        case CHTLJSTokenType::AMPERSAND: oss << "AMPERSAND"; break;
        case CHTLJSTokenType::HASH: oss << "HASH"; break;
        case CHTLJSTokenType::WHITESPACE: oss << "WHITESPACE"; break;
        case CHTLJSTokenType::NEWLINE: oss << "NEWLINE"; break;
        case CHTLJSTokenType::UNKNOWN: oss << "UNKNOWN"; break;
    }
    
    oss << ", value='" << value_ << "', line=" << line_ << ", column=" << column_ << "}";
    return oss.str();
}

std::shared_ptr<CHTLJSToken> createToken(CHTLJSTokenType type, const std::string& value, size_t line, size_t column) {
    return std::make_shared<CHTLJSToken>(type, value, line, column);
}

} // namespace CHTLJS