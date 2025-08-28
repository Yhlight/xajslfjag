#include "CHTLToken.h"
#include <unordered_map>

namespace CHTL {

std::string CHTLTokenUtils::tokenTypeToString(CHTLTokenType type) {
    static const std::unordered_map<CHTLTokenType, std::string> typeMap = {
        {CHTLTokenType::UNKNOWN, "UNKNOWN"},
        {CHTLTokenType::END_OF_FILE, "END_OF_FILE"},
        {CHTLTokenType::WHITESPACE, "WHITESPACE"},
        {CHTLTokenType::NEWLINE, "NEWLINE"},
        {CHTLTokenType::SINGLE_LINE_COMMENT, "SINGLE_LINE_COMMENT"},
        {CHTLTokenType::MULTI_LINE_COMMENT, "MULTI_LINE_COMMENT"},
        {CHTLTokenType::GENERATOR_COMMENT, "GENERATOR_COMMENT"},
        {CHTLTokenType::TEXT_LITERAL, "TEXT_LITERAL"},
        {CHTLTokenType::DOUBLE_QUOTE_STRING, "DOUBLE_QUOTE_STRING"},
        {CHTLTokenType::SINGLE_QUOTE_STRING, "SINGLE_QUOTE_STRING"},
        {CHTLTokenType::LEFT_BRACE, "LEFT_BRACE"},
        {CHTLTokenType::RIGHT_BRACE, "RIGHT_BRACE"},
        {CHTLTokenType::LEFT_BRACKET, "LEFT_BRACKET"},
        {CHTLTokenType::RIGHT_BRACKET, "RIGHT_BRACKET"},
        {CHTLTokenType::LEFT_PAREN, "LEFT_PAREN"},
        {CHTLTokenType::RIGHT_PAREN, "RIGHT_PAREN"},
        {CHTLTokenType::SEMICOLON, "SEMICOLON"},
        {CHTLTokenType::COLON, "COLON"},
        {CHTLTokenType::EQUALS, "EQUALS"},
        {CHTLTokenType::COMMA, "COMMA"},
        {CHTLTokenType::DOT, "DOT"},
        {CHTLTokenType::SLASH, "SLASH"},
        {CHTLTokenType::ASTERISK, "ASTERISK"},
        {CHTLTokenType::KEYWORD_TEMPLATE, "KEYWORD_TEMPLATE"},
        {CHTLTokenType::KEYWORD_CUSTOM, "KEYWORD_CUSTOM"},
        {CHTLTokenType::KEYWORD_ORIGIN, "KEYWORD_ORIGIN"},
        {CHTLTokenType::KEYWORD_IMPORT, "KEYWORD_IMPORT"},
        {CHTLTokenType::KEYWORD_NAMESPACE, "KEYWORD_NAMESPACE"},
        {CHTLTokenType::KEYWORD_CONFIGURATION, "KEYWORD_CONFIGURATION"},
        {CHTLTokenType::KEYWORD_INFO, "KEYWORD_INFO"},
        {CHTLTokenType::KEYWORD_EXPORT, "KEYWORD_EXPORT"},
        {CHTLTokenType::TYPE_STYLE, "TYPE_STYLE"},
        {CHTLTokenType::TYPE_ELEMENT, "TYPE_ELEMENT"},
        {CHTLTokenType::TYPE_VAR, "TYPE_VAR"},
        {CHTLTokenType::TYPE_HTML, "TYPE_HTML"},
        {CHTLTokenType::TYPE_JAVASCRIPT, "TYPE_JAVASCRIPT"},
        {CHTLTokenType::TYPE_CHTL, "TYPE_CHTL"},
        {CHTLTokenType::TYPE_CJMOD, "TYPE_CJMOD"},
        {CHTLTokenType::TYPE_CONFIG, "TYPE_CONFIG"},
        {CHTLTokenType::KEYWORD_INHERIT, "KEYWORD_INHERIT"},
        {CHTLTokenType::KEYWORD_DELETE, "KEYWORD_DELETE"},
        {CHTLTokenType::KEYWORD_INSERT, "KEYWORD_INSERT"},
        {CHTLTokenType::KEYWORD_AFTER, "KEYWORD_AFTER"},
        {CHTLTokenType::KEYWORD_BEFORE, "KEYWORD_BEFORE"},
        {CHTLTokenType::KEYWORD_REPLACE, "KEYWORD_REPLACE"},
        {CHTLTokenType::KEYWORD_AT_TOP, "KEYWORD_AT_TOP"},
        {CHTLTokenType::KEYWORD_AT_BOTTOM, "KEYWORD_AT_BOTTOM"},
        {CHTLTokenType::KEYWORD_FROM, "KEYWORD_FROM"},
        {CHTLTokenType::KEYWORD_AS, "KEYWORD_AS"},
        {CHTLTokenType::KEYWORD_EXCEPT, "KEYWORD_EXCEPT"},
        {CHTLTokenType::KEYWORD_TEXT, "KEYWORD_TEXT"},
        {CHTLTokenType::KEYWORD_STYLE, "KEYWORD_STYLE"},
        {CHTLTokenType::KEYWORD_SCRIPT, "KEYWORD_SCRIPT"},
        {CHTLTokenType::KEYWORD_USE, "KEYWORD_USE"},
        {CHTLTokenType::AMPERSAND, "AMPERSAND"},
        {CHTLTokenType::DOLLAR_SIGN, "DOLLAR_SIGN"},
        {CHTLTokenType::HASH_SIGN, "HASH_SIGN"},
        {CHTLTokenType::PERIOD, "PERIOD"},
        {CHTLTokenType::IDENTIFIER, "IDENTIFIER"},
        {CHTLTokenType::NUMBER, "NUMBER"},
        {CHTLTokenType::CSS_SELECTOR, "CSS_SELECTOR"},
        {CHTLTokenType::PSEUDO_CLASS, "PSEUDO_CLASS"},
        {CHTLTokenType::PSEUDO_ELEMENT, "PSEUDO_ELEMENT"},
        {CHTLTokenType::CONFIG_KEY, "CONFIG_KEY"},
        {CHTLTokenType::CONFIG_VALUE, "CONFIG_VALUE"},
        {CHTLTokenType::IMPORT_PATH, "IMPORT_PATH"},
        {CHTLTokenType::MODULE_NAME, "MODULE_NAME"},
        {CHTLTokenType::NAMESPACE_NAME, "NAMESPACE_NAME"},
        {CHTLTokenType::NAMESPACE_SEPARATOR, "NAMESPACE_SEPARATOR"},
        {CHTLTokenType::CONSTRAINT_OPERATOR, "CONSTRAINT_OPERATOR"},
        {CHTLTokenType::TEMPLATE_NAME, "TEMPLATE_NAME"},
        {CHTLTokenType::TEMPLATE_PARAMETER, "TEMPLATE_PARAMETER"},
        {CHTLTokenType::CUSTOM_NAME, "CUSTOM_NAME"},
        {CHTLTokenType::CUSTOM_PARAMETER, "CUSTOM_PARAMETER"},
        {CHTLTokenType::ORIGIN_TYPE, "ORIGIN_TYPE"},
        {CHTLTokenType::ORIGIN_NAME, "ORIGIN_NAME"},
        {CHTLTokenType::STYLE_GROUP_NAME, "STYLE_GROUP_NAME"},
        {CHTLTokenType::STYLE_PROPERTY, "STYLE_PROPERTY"},
        {CHTLTokenType::STYLE_VALUE, "STYLE_VALUE"},
        {CHTLTokenType::ELEMENT_NAME, "ELEMENT_NAME"},
        {CHTLTokenType::ELEMENT_ATTRIBUTE, "ELEMENT_ATTRIBUTE"},
        {CHTLTokenType::ELEMENT_ATTRIBUTE_VALUE, "ELEMENT_ATTRIBUTE_VALUE"}
    };
    
    auto it = typeMap.find(type);
    return it != typeMap.end() ? it->second : "UNKNOWN";
}

bool CHTLTokenUtils::isKeyword(CHTLTokenType type) {
    return type == CHTLTokenType::KEYWORD_TEMPLATE ||
           type == CHTLTokenType::KEYWORD_CUSTOM ||
           type == CHTLTokenType::KEYWORD_ORIGIN ||
           type == CHTLTokenType::KEYWORD_IMPORT ||
           type == CHTLTokenType::KEYWORD_NAMESPACE ||
           type == CHTLTokenType::KEYWORD_CONFIGURATION ||
           type == CHTLTokenType::KEYWORD_INFO ||
           type == CHTLTokenType::KEYWORD_EXPORT ||
           type == CHTLTokenType::KEYWORD_INHERIT ||
           type == CHTLTokenType::KEYWORD_DELETE ||
           type == CHTLTokenType::KEYWORD_INSERT ||
           type == CHTLTokenType::KEYWORD_AFTER ||
           type == CHTLTokenType::KEYWORD_BEFORE ||
           type == CHTLTokenType::KEYWORD_REPLACE ||
           type == CHTLTokenType::KEYWORD_AT_TOP ||
           type == CHTLTokenType::KEYWORD_AT_BOTTOM ||
           type == CHTLTokenType::KEYWORD_FROM ||
           type == CHTLTokenType::KEYWORD_AS ||
           type == CHTLTokenType::KEYWORD_EXCEPT ||
           type == CHTLTokenType::KEYWORD_TEXT ||
           type == CHTLTokenType::KEYWORD_STYLE ||
           type == CHTLTokenType::KEYWORD_SCRIPT ||
           type == CHTLTokenType::KEYWORD_USE;
}

bool CHTLTokenUtils::isOperator(CHTLTokenType type) {
    return type == CHTLTokenType::COLON ||
           type == CHTLTokenType::EQUALS ||
           type == CHTLTokenType::DOT ||
           type == CHTLTokenType::SLASH ||
           type == CHTLTokenType::ASTERISK ||
           type == CHTLTokenType::AMPERSAND ||
           type == CHTLTokenType::DOLLAR_SIGN ||
           type == CHTLTokenType::HASH_SIGN ||
           type == CHTLTokenType::PERIOD;
}

bool CHTLTokenUtils::isPunctuation(CHTLTokenType type) {
    return type == CHTLTokenType::LEFT_BRACE ||
           type == CHTLTokenType::RIGHT_BRACE ||
           type == CHTLTokenType::LEFT_BRACKET ||
           type == CHTLTokenType::RIGHT_BRACKET ||
           type == CHTLTokenType::LEFT_PAREN ||
           type == CHTLTokenType::RIGHT_PAREN ||
           type == CHTLTokenType::SEMICOLON ||
           type == CHTLTokenType::COMMA;
}

bool CHTLTokenUtils::isLiteral(CHTLTokenType type) {
    return type == CHTLTokenType::TEXT_LITERAL ||
           type == CHTLTokenType::DOUBLE_QUOTE_STRING ||
           type == CHTLTokenType::SINGLE_QUOTE_STRING ||
           type == CHTLTokenType::NUMBER;
}

} // namespace CHTL