#include "Token.h"
#include <unordered_set>

namespace CHTL {

std::string Token::getTypeName() const {
    switch (type) {
        case TokenType::LBRACE: return "LBRACE";
        case TokenType::RBRACE: return "RBRACE";
        case TokenType::LPAREN: return "LPAREN";
        case TokenType::RPAREN: return "RPAREN";
        case TokenType::LBRACKET: return "LBRACKET";
        case TokenType::RBRACKET: return "RBRACKET";
        case TokenType::SEMICOLON: return "SEMICOLON";
        case TokenType::COLON: return "COLON";
        case TokenType::EQUAL: return "EQUAL";
        case TokenType::COMMA: return "COMMA";
        case TokenType::DOT: return "DOT";
        case TokenType::SLASH: return "SLASH";
        case TokenType::STAR: return "STAR";
        case TokenType::AMPERSAND: return "AMPERSAND";
        case TokenType::STRING_LITERAL: return "STRING_LITERAL";
        case TokenType::UNQUOTED_LITERAL: return "UNQUOTED_LITERAL";
        case TokenType::NUMBER: return "NUMBER";
        case TokenType::LINE_COMMENT: return "LINE_COMMENT";
        case TokenType::BLOCK_COMMENT: return "BLOCK_COMMENT";
        case TokenType::GENERATOR_COMMENT: return "GENERATOR_COMMENT";
        case TokenType::HTML_TAG: return "HTML_TAG";
        case TokenType::TEXT_KEYWORD: return "TEXT_KEYWORD";
        case TokenType::STYLE_KEYWORD: return "STYLE_KEYWORD";
        case TokenType::SCRIPT_KEYWORD: return "SCRIPT_KEYWORD";
        case TokenType::TEMPLATE_PREFIX: return "TEMPLATE_PREFIX";
        case TokenType::CUSTOM_PREFIX: return "CUSTOM_PREFIX";
        case TokenType::ORIGIN_PREFIX: return "ORIGIN_PREFIX";
        case TokenType::IMPORT_PREFIX: return "IMPORT_PREFIX";
        case TokenType::NAMESPACE_PREFIX: return "NAMESPACE_PREFIX";
        case TokenType::CONFIGURATION_PREFIX: return "CONFIGURATION_PREFIX";
        case TokenType::INFO_PREFIX: return "INFO_PREFIX";
        case TokenType::EXPORT_PREFIX: return "EXPORT_PREFIX";
        case TokenType::ORIGINTYPE_PREFIX: return "ORIGINTYPE_PREFIX";
        case TokenType::NAME_PREFIX: return "NAME_PREFIX";
        case TokenType::AT_STYLE: return "AT_STYLE";
        case TokenType::AT_ELEMENT: return "AT_ELEMENT";
        case TokenType::AT_VAR: return "AT_VAR";
        case TokenType::AT_HTML: return "AT_HTML";
        case TokenType::AT_JAVASCRIPT: return "AT_JAVASCRIPT";
        case TokenType::AT_CHTL: return "AT_CHTL";
        case TokenType::AT_CJMOD: return "AT_CJMOD";
        case TokenType::AT_CONFIG: return "AT_CONFIG";
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
        case TokenType::CLASS_SELECTOR: return "CLASS_SELECTOR";
        case TokenType::ID_SELECTOR: return "ID_SELECTOR";
        case TokenType::ELEMENT_SELECTOR: return "ELEMENT_SELECTOR";
        case TokenType::INDEX_ACCESS: return "INDEX_ACCESS";
        case TokenType::USE: return "USE";
        case TokenType::HTML5: return "HTML5";
        case TokenType::HASH: return "HASH";
        case TokenType::DOUBLE_COLON: return "DOUBLE_COLON";
        case TokenType::ARROW: return "ARROW";
        case TokenType::WILDCARD: return "WILDCARD";
        case TokenType::CONFIG_IDENTIFIER: return "CONFIG_IDENTIFIER";
        case TokenType::CONFIG_VALUE: return "CONFIG_VALUE";
        case TokenType::PLACEHOLDER: return "PLACEHOLDER";
        case TokenType::EOF_TOKEN: return "EOF_TOKEN";
        case TokenType::INVALID: return "INVALID";
        case TokenType::IDENTIFIER: return "IDENTIFIER";
        case TokenType::PATH_SEPARATOR: return "PATH_SEPARATOR";
        case TokenType::FILE_EXTENSION: return "FILE_EXTENSION";
        case TokenType::DOUBLE_STAR: return "DOUBLE_STAR";
        case TokenType::NEWLINE: return "NEWLINE";
        case TokenType::WHITESPACE: return "WHITESPACE";
        default: return "UNKNOWN";
    }
}

bool Token::isKeyword() const {
    static const std::unordered_set<TokenType> keywords = {
        TokenType::TEXT_KEYWORD,
        TokenType::STYLE_KEYWORD,
        TokenType::SCRIPT_KEYWORD,
        TokenType::INHERIT,
        TokenType::DELETE,
        TokenType::INSERT,
        TokenType::AFTER,
        TokenType::BEFORE,
        TokenType::REPLACE,
        TokenType::AT_TOP,
        TokenType::AT_BOTTOM,
        TokenType::FROM,
        TokenType::AS,
        TokenType::EXCEPT,
        TokenType::USE,
        TokenType::HTML5
    };
    return keywords.find(type) != keywords.end();
}

bool Token::isTypeIdentifier() const {
    static const std::unordered_set<TokenType> typeIdentifiers = {
        TokenType::AT_STYLE,
        TokenType::AT_ELEMENT,
        TokenType::AT_VAR,
        TokenType::AT_HTML,
        TokenType::AT_JAVASCRIPT,
        TokenType::AT_CHTL,
        TokenType::AT_CJMOD,
        TokenType::AT_CONFIG
    };
    return typeIdentifiers.find(type) != typeIdentifiers.end();
}

bool Token::isSelector() const {
    static const std::unordered_set<TokenType> selectors = {
        TokenType::CLASS_SELECTOR,
        TokenType::ID_SELECTOR,
        TokenType::ELEMENT_SELECTOR
    };
    return selectors.find(type) != selectors.end();
}

bool Token::isPrefix() const {
    static const std::unordered_set<TokenType> prefixes = {
        TokenType::TEMPLATE_PREFIX,
        TokenType::CUSTOM_PREFIX,
        TokenType::ORIGIN_PREFIX,
        TokenType::IMPORT_PREFIX,
        TokenType::NAMESPACE_PREFIX,
        TokenType::CONFIGURATION_PREFIX,
        TokenType::INFO_PREFIX,
        TokenType::EXPORT_PREFIX,
        TokenType::ORIGINTYPE_PREFIX,
        TokenType::NAME_PREFIX
    };
    return prefixes.find(type) != prefixes.end();
}

bool Token::isOperationKeyword() const {
    static const std::unordered_set<TokenType> operations = {
        TokenType::INHERIT,
        TokenType::DELETE,
        TokenType::INSERT,
        TokenType::AFTER,
        TokenType::BEFORE,
        TokenType::REPLACE,
        TokenType::AT_TOP,
        TokenType::AT_BOTTOM,
        TokenType::FROM,
        TokenType::AS,
        TokenType::EXCEPT
    };
    return operations.find(type) != operations.end();
}

} // namespace CHTL