#include "CHTLToken.h"
#include <unordered_set>

namespace CHTL {

CHTLToken::CHTLToken(CHTLTokenType type, const std::string& value, 
                     size_t line, size_t column, size_t position)
    : type(type), value(value), line(line), column(column), position(position) {
}

bool CHTLToken::Is(CHTLTokenType tokenType) const {
    return type == tokenType;
}

bool CHTLToken::IsKeyword() const {
    static const std::unordered_set<CHTLTokenType> keywords = {
        CHTLTokenType::TEXT, CHTLTokenType::STYLE, CHTLTokenType::SCRIPT,
        CHTLTokenType::TEMPLATE, CHTLTokenType::CUSTOM, CHTLTokenType::ORIGIN,
        CHTLTokenType::CONFIGURATION, CHTLTokenType::IMPORT, CHTLTokenType::NAMESPACE,
        CHTLTokenType::INHERIT, CHTLTokenType::DELETE, CHTLTokenType::INSERT,
        CHTLTokenType::FROM, CHTLTokenType::AS, CHTLTokenType::EXCEPT,
        CHTLTokenType::AFTER, CHTLTokenType::BEFORE, CHTLTokenType::REPLACE,
        CHTLTokenType::AT_TOP, CHTLTokenType::AT_BOTTOM
    };
    return keywords.find(type) != keywords.end();
}

bool CHTLToken::IsOperator() const {
    static const std::unordered_set<CHTLTokenType> operators = {
        CHTLTokenType::COLON, CHTLTokenType::EQUALS, CHTLTokenType::DOT,
        CHTLTokenType::HASH, CHTLTokenType::AMPERSAND, CHTLTokenType::ASTERISK,
        CHTLTokenType::SLASH, CHTLTokenType::DOUBLE_COLON, CHTLTokenType::ARROW
    };
    return operators.find(type) != operators.end();
}

bool CHTLToken::IsSelector() const {
    return type == CHTLTokenType::CLASS_SELECTOR || 
           type == CHTLTokenType::ID_SELECTOR || 
           type == CHTLTokenType::REFERENCE_SELECTOR;
}

bool CHTLToken::IsComment() const {
    return type == CHTLTokenType::SINGLE_LINE_COMMENT ||
           type == CHTLTokenType::MULTI_LINE_COMMENT ||
           type == CHTLTokenType::GENERATOR_COMMENT;
}

std::string CHTLToken::ToString() const {
    return TypeToString(type) + ": \"" + value + "\" at (" + 
           std::to_string(line) + "," + std::to_string(column) + ")";
}

std::string CHTLToken::TypeToString(CHTLTokenType type) {
    switch (type) {
        case CHTLTokenType::IDENTIFIER: return "IDENTIFIER";
        case CHTLTokenType::STRING_LITERAL: return "STRING_LITERAL";
        case CHTLTokenType::UNQUOTED_LITERAL: return "UNQUOTED_LITERAL";
        case CHTLTokenType::NUMERIC_LITERAL: return "NUMERIC_LITERAL";
        case CHTLTokenType::HTML_ELEMENT: return "HTML_ELEMENT";
        case CHTLTokenType::TEXT: return "TEXT";
        case CHTLTokenType::STYLE: return "STYLE";
        case CHTLTokenType::SCRIPT: return "SCRIPT";
        case CHTLTokenType::TEMPLATE: return "TEMPLATE";
        case CHTLTokenType::CUSTOM: return "CUSTOM";
        case CHTLTokenType::STYLE_TEMPLATE: return "STYLE_TEMPLATE";
        case CHTLTokenType::ELEMENT_TEMPLATE: return "ELEMENT_TEMPLATE";
        case CHTLTokenType::VAR_TEMPLATE: return "VAR_TEMPLATE";
        case CHTLTokenType::ORIGIN: return "ORIGIN";
        case CHTLTokenType::ORIGIN_HTML: return "ORIGIN_HTML";
        case CHTLTokenType::ORIGIN_STYLE: return "ORIGIN_STYLE";
        case CHTLTokenType::ORIGIN_JAVASCRIPT: return "ORIGIN_JAVASCRIPT";
        case CHTLTokenType::CONFIGURATION: return "CONFIGURATION";
        case CHTLTokenType::NAME_SECTION: return "NAME_SECTION";
        case CHTLTokenType::ORIGIN_TYPE_SECTION: return "ORIGIN_TYPE_SECTION";
        case CHTLTokenType::IMPORT: return "IMPORT";
        case CHTLTokenType::FROM: return "FROM";
        case CHTLTokenType::AS: return "AS";
        case CHTLTokenType::EXCEPT: return "EXCEPT";
        case CHTLTokenType::NAMESPACE: return "NAMESPACE";
        case CHTLTokenType::INHERIT: return "INHERIT";
        case CHTLTokenType::DELETE: return "DELETE";
        case CHTLTokenType::INSERT: return "INSERT";
        case CHTLTokenType::AFTER: return "AFTER";
        case CHTLTokenType::BEFORE: return "BEFORE";
        case CHTLTokenType::REPLACE: return "REPLACE";
        case CHTLTokenType::AT_TOP: return "AT_TOP";
        case CHTLTokenType::AT_BOTTOM: return "AT_BOTTOM";
        case CHTLTokenType::CLASS_SELECTOR: return "CLASS_SELECTOR";
        case CHTLTokenType::ID_SELECTOR: return "ID_SELECTOR";
        case CHTLTokenType::REFERENCE_SELECTOR: return "REFERENCE_SELECTOR";
        case CHTLTokenType::LEFT_BRACE: return "LEFT_BRACE";
        case CHTLTokenType::RIGHT_BRACE: return "RIGHT_BRACE";
        case CHTLTokenType::LEFT_BRACKET: return "LEFT_BRACKET";
        case CHTLTokenType::RIGHT_BRACKET: return "RIGHT_BRACKET";
        case CHTLTokenType::LEFT_PAREN: return "LEFT_PAREN";
        case CHTLTokenType::RIGHT_PAREN: return "RIGHT_PAREN";
        case CHTLTokenType::SEMICOLON: return "SEMICOLON";
        case CHTLTokenType::COMMA: return "COMMA";
        case CHTLTokenType::COLON: return "COLON";
        case CHTLTokenType::EQUALS: return "EQUALS";
        case CHTLTokenType::DOT: return "DOT";
        case CHTLTokenType::HASH: return "HASH";
        case CHTLTokenType::AMPERSAND: return "AMPERSAND";
        case CHTLTokenType::ASTERISK: return "ASTERISK";
        case CHTLTokenType::SLASH: return "SLASH";
        case CHTLTokenType::DOUBLE_COLON: return "DOUBLE_COLON";
        case CHTLTokenType::ARROW: return "ARROW";
        case CHTLTokenType::SINGLE_LINE_COMMENT: return "SINGLE_LINE_COMMENT";
        case CHTLTokenType::MULTI_LINE_COMMENT: return "MULTI_LINE_COMMENT";
        case CHTLTokenType::GENERATOR_COMMENT: return "GENERATOR_COMMENT";
        case CHTLTokenType::WHITESPACE: return "WHITESPACE";
        case CHTLTokenType::NEWLINE: return "NEWLINE";
        case CHTLTokenType::EOF_TOKEN: return "EOF_TOKEN";
        default: return "UNKNOWN";
    }
}

} // namespace CHTL