#include "Token.h"

namespace CHTL {

// Token类型名称映射
const std::unordered_map<TokenType, std::string> TOKEN_TYPE_NAMES = {
    {TokenType::EOF_TOKEN, "EOF"},
    {TokenType::STRING_LITERAL, "STRING_LITERAL"},
    {TokenType::UNQUOTED_LITERAL, "UNQUOTED_LITERAL"},
    {TokenType::IDENTIFIER, "IDENTIFIER"},
    {TokenType::TEXT, "TEXT"},
    {TokenType::STYLE, "STYLE"},
    {TokenType::SCRIPT, "SCRIPT"},
    {TokenType::TEMPLATE, "TEMPLATE"},
    {TokenType::CUSTOM, "CUSTOM"},
    {TokenType::ORIGIN, "ORIGIN"},
    {TokenType::IMPORT, "IMPORT"},
    {TokenType::NAMESPACE, "NAMESPACE"},
    {TokenType::CONFIGURATION, "CONFIGURATION"},
    {TokenType::INFO, "INFO"},
    {TokenType::EXPORT, "EXPORT"},
    {TokenType::USE, "USE"},
    {TokenType::AT_STYLE, "@STYLE"},
    {TokenType::AT_ELEMENT, "@ELEMENT"},
    {TokenType::AT_VAR, "@VAR"},
    {TokenType::AT_HTML, "@HTML"},
    {TokenType::AT_JAVASCRIPT, "@JAVASCRIPT"},
    {TokenType::AT_CHTL, "@CHTL"},
    {TokenType::AT_CJMOD, "@CJMOD"},
    {TokenType::AT_CONFIG, "@CONFIG"},
    {TokenType::INHERIT, "INHERIT"},
    {TokenType::DELETE, "DELETE"},
    {TokenType::INSERT, "INSERT"},
    {TokenType::AFTER, "AFTER"},
    {TokenType::BEFORE, "BEFORE"},
    {TokenType::REPLACE, "REPLACE"},
    {TokenType::AT_TOP, "AT_TOP"},
    {TokenType::AT_BOTTOM, "AT_BOTTOM"},
    {TokenType::FROM, "FROM"},
    {TokenType::AS, "AS"},
    {TokenType::EXCEPT, "EXCEPT"},
    {TokenType::LEFT_BRACE, "LEFT_BRACE"},
    {TokenType::RIGHT_BRACE, "RIGHT_BRACE"},
    {TokenType::LEFT_BRACKET, "LEFT_BRACKET"},
    {TokenType::RIGHT_BRACKET, "RIGHT_BRACKET"},
    {TokenType::LEFT_PAREN, "LEFT_PAREN"},
    {TokenType::RIGHT_PAREN, "RIGHT_PAREN"},
    {TokenType::SEMICOLON, "SEMICOLON"},
    {TokenType::COLON, "COLON"},
    {TokenType::EQUALS, "EQUALS"},
    {TokenType::COMMA, "COMMA"},
    {TokenType::DOT, "DOT"},
    {TokenType::ARROW, "ARROW"},
    {TokenType::DOUBLE_LEFT_BRACE, "DOUBLE_LEFT_BRACE"},
    {TokenType::DOUBLE_RIGHT_BRACE, "DOUBLE_RIGHT_BRACE"},
    {TokenType::AMPERSAND, "AMPERSAND"},
    {TokenType::HASH, "HASH"},
    {TokenType::STAR, "STAR"},
    {TokenType::SLASH, "SLASH"},
    {TokenType::SINGLE_COMMENT, "SINGLE_COMMENT"},
    {TokenType::MULTI_COMMENT, "MULTI_COMMENT"},
    {TokenType::GENERATOR_COMMENT, "GENERATOR_COMMENT"},
    {TokenType::NEWLINE, "NEWLINE"},
    {TokenType::WHITESPACE, "WHITESPACE"},
    {TokenType::UNKNOWN, "UNKNOWN"}
};

// 关键字映射
const std::unordered_map<std::string, TokenType> KEYWORDS = {
    {"text", TokenType::TEXT},
    {"style", TokenType::STYLE},
    {"script", TokenType::SCRIPT},
    {"use", TokenType::USE},
    {"inherit", TokenType::INHERIT},
    {"delete", TokenType::DELETE},
    {"insert", TokenType::INSERT},
    {"after", TokenType::AFTER},
    {"before", TokenType::BEFORE},
    {"replace", TokenType::REPLACE},
    {"at", TokenType::IDENTIFIER},  // "at top" 和 "at bottom" 需要特殊处理
    {"top", TokenType::IDENTIFIER},
    {"bottom", TokenType::IDENTIFIER},
    {"from", TokenType::FROM},
    {"as", TokenType::AS},
    {"except", TokenType::EXCEPT}
};

// @类型映射
const std::unordered_map<std::string, TokenType> AT_TYPES = {
    {"@Style", TokenType::AT_STYLE},
    {"@Element", TokenType::AT_ELEMENT},
    {"@Var", TokenType::AT_VAR},
    {"@Html", TokenType::AT_HTML},
    {"@JavaScript", TokenType::AT_JAVASCRIPT},
    {"@Chtl", TokenType::AT_CHTL},
    {"@CJmod", TokenType::AT_CJMOD},
    {"@Config", TokenType::AT_CONFIG}
};

bool Token::isKeyword() const {
    return type >= TokenType::TEXT && type <= TokenType::USE;
}

bool Token::isLiteral() const {
    return type == TokenType::STRING_LITERAL || 
           type == TokenType::UNQUOTED_LITERAL;
}

bool Token::isOperator() const {
    return type >= TokenType::INHERIT && type <= TokenType::EXCEPT;
}

bool Token::isSymbol() const {
    return type >= TokenType::LEFT_BRACE && type <= TokenType::SLASH;
}

bool Token::isComment() const {
    return type == TokenType::SINGLE_COMMENT || 
           type == TokenType::MULTI_COMMENT ||
           type == TokenType::GENERATOR_COMMENT;
}

bool Token::isAtType() const {
    return type >= TokenType::AT_STYLE && type <= TokenType::AT_CONFIG;
}

std::string Token::toString() const {
    auto it = TOKEN_TYPE_NAMES.find(type);
    std::string typeName = (it != TOKEN_TYPE_NAMES.end()) ? it->second : "UNKNOWN";
    
    return "[" + typeName + ": \"" + value + "\" at " + 
           std::to_string(line) + ":" + std::to_string(column) + "]";
}

} // namespace CHTL