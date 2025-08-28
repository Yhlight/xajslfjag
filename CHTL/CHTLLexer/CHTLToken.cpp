#include "CHTLToken.h"
#include <sstream>

namespace CHTL {

CHTLToken::CHTLToken(CHTLTokenType type, const std::string& value, size_t line, size_t column)
    : type_(type), value_(value), line_(line), column_(column) {}

std::string CHTLToken::toString() const {
    std::ostringstream oss;
    oss << "Token{type=";
    
    // 将Token类型转换为字符串
    switch (type_) {
        case CHTLTokenType::END_OF_FILE: oss << "END_OF_FILE"; break;
        case CHTLTokenType::IDENTIFIER: oss << "IDENTIFIER"; break;
        case CHTLTokenType::STRING_LITERAL: oss << "STRING_LITERAL"; break;
        case CHTLTokenType::UNQUOTED_LITERAL: oss << "UNQUOTED_LITERAL"; break;
        case CHTLTokenType::NUMBER: oss << "NUMBER"; break;
        case CHTLTokenType::KEYWORD_TEXT: oss << "KEYWORD_TEXT"; break;
        case CHTLTokenType::KEYWORD_STYLE: oss << "KEYWORD_STYLE"; break;
        case CHTLTokenType::KEYWORD_SCRIPT: oss << "KEYWORD_SCRIPT"; break;
        case CHTLTokenType::KEYWORD_TEMPLATE: oss << "KEYWORD_TEMPLATE"; break;
        case CHTLTokenType::KEYWORD_CUSTOM: oss << "KEYWORD_CUSTOM"; break;
        case CHTLTokenType::KEYWORD_ORIGIN: oss << "KEYWORD_ORIGIN"; break;
        case CHTLTokenType::KEYWORD_IMPORT: oss << "KEYWORD_IMPORT"; break;
        case CHTLTokenType::KEYWORD_NAMESPACE: oss << "KEYWORD_NAMESPACE"; break;
        case CHTLTokenType::KEYWORD_CONFIGURATION: oss << "KEYWORD_CONFIGURATION"; break;
        case CHTLTokenType::KEYWORD_INFO: oss << "KEYWORD_INFO"; break;
        case CHTLTokenType::KEYWORD_EXPORT: oss << "KEYWORD_EXPORT"; break;
        case CHTLTokenType::KEYWORD_USE: oss << "KEYWORD_USE"; break;
        case CHTLTokenType::KEYWORD_EXCEPT: oss << "KEYWORD_EXCEPT"; break;
        case CHTLTokenType::KEYWORD_INHERIT: oss << "KEYWORD_INHERIT"; break;
        case CHTLTokenType::KEYWORD_DELETE: oss << "KEYWORD_DELETE"; break;
        case CHTLTokenType::KEYWORD_INSERT: oss << "KEYWORD_INSERT"; break;
        case CHTLTokenType::KEYWORD_AFTER: oss << "KEYWORD_AFTER"; break;
        case CHTLTokenType::KEYWORD_BEFORE: oss << "KEYWORD_BEFORE"; break;
        case CHTLTokenType::KEYWORD_REPLACE: oss << "KEYWORD_REPLACE"; break;
        case CHTLTokenType::KEYWORD_AT_TOP: oss << "KEYWORD_AT_TOP"; break;
        case CHTLTokenType::KEYWORD_AT_BOTTOM: oss << "KEYWORD_AT_BOTTOM"; break;
        case CHTLTokenType::KEYWORD_FROM: oss << "KEYWORD_FROM"; break;
        case CHTLTokenType::KEYWORD_AS: oss << "KEYWORD_AS"; break;
        case CHTLTokenType::TEMPLATE_STYLE: oss << "TEMPLATE_STYLE"; break;
        case CHTLTokenType::TEMPLATE_ELEMENT: oss << "TEMPLATE_ELEMENT"; break;
        case CHTLTokenType::TEMPLATE_VAR: oss << "TEMPLATE_VAR"; break;
        case CHTLTokenType::CUSTOM_STYLE: oss << "CUSTOM_STYLE"; break;
        case CHTLTokenType::CUSTOM_ELEMENT: oss << "CUSTOM_ELEMENT"; break;
        case CHTLTokenType::CUSTOM_VAR: oss << "CUSTOM_VAR"; break;
        case CHTLTokenType::ORIGIN_HTML: oss << "ORIGIN_HTML"; break;
        case CHTLTokenType::ORIGIN_STYLE: oss << "ORIGIN_STYLE"; break;
        case CHTLTokenType::ORIGIN_JAVASCRIPT: oss << "ORIGIN_JAVASCRIPT"; break;
        case CHTLTokenType::IMPORT_HTML: oss << "IMPORT_HTML"; break;
        case CHTLTokenType::IMPORT_STYLE: oss << "IMPORT_STYLE"; break;
        case CHTLTokenType::IMPORT_JAVASCRIPT: oss << "IMPORT_JAVASCRIPT"; break;
        case CHTLTokenType::IMPORT_CHTL: oss << "IMPORT_CHTL"; break;
        case CHTLTokenType::IMPORT_CJMOD: oss << "IMPORT_CJMOD"; break;
        case CHTLTokenType::IMPORT_CONFIG: oss << "IMPORT_CONFIG"; break;
        case CHTLTokenType::CONFIGURATION_CONFIG: oss << "CONFIGURATION_CONFIG"; break;
        case CHTLTokenType::LEFT_BRACE: oss << "LEFT_BRACE"; break;
        case CHTLTokenType::RIGHT_BRACE: oss << "RIGHT_BRACE"; break;
        case CHTLTokenType::LEFT_BRACKET: oss << "LEFT_BRACKET"; break;
        case CHTLTokenType::RIGHT_BRACKET: oss << "RIGHT_BRACKET"; break;
        case CHTLTokenType::LEFT_PAREN: oss << "LEFT_PAREN"; break;
        case CHTLTokenType::RIGHT_PAREN: oss << "RIGHT_PAREN"; break;
        case CHTLTokenType::SEMICOLON: oss << "SEMICOLON"; break;
        case CHTLTokenType::COLON: oss << "COLON"; break;
        case CHTLTokenType::EQUAL: oss << "EQUAL"; break;
        case CHTLTokenType::COMMA: oss << "COMMA"; break;
        case CHTLTokenType::DOT: oss << "DOT"; break;
        case CHTLTokenType::AT: oss << "AT"; break;
        case CHTLTokenType::HASH: oss << "HASH"; break;
        case CHTLTokenType::AMPERSAND: oss << "AMPERSAND"; break;
        case CHTLTokenType::COMMENT_SINGLE: oss << "COMMENT_SINGLE"; break;
        case CHTLTokenType::COMMENT_MULTI: oss << "COMMENT_MULTI"; break;
        case CHTLTokenType::COMMENT_GENERATOR: oss << "COMMENT_GENERATOR"; break;
        case CHTLTokenType::WHITESPACE: oss << "WHITESPACE"; break;
        case CHTLTokenType::NEWLINE: oss << "NEWLINE"; break;
        case CHTLTokenType::UNKNOWN: oss << "UNKNOWN"; break;
    }
    
    oss << ", value='" << value_ << "', line=" << line_ << ", column=" << column_ << "}";
    return oss.str();
}

std::shared_ptr<CHTLToken> createToken(CHTLTokenType type, const std::string& value, size_t line, size_t column) {
    return std::make_shared<CHTLToken>(type, value, line, column);
}

} // namespace CHTL