#include "Token.h"
#include <sstream>

namespace CHTL {

// 默认构造函数
Token::Token() 
    : type_(TokenType::UNKNOWN), lexeme_(""), value_(""), line_(0), column_(0) {
}

// 基本构造函数
Token::Token(TokenType type, const std::string& lexeme, int line, int column)
    : type_(type), lexeme_(lexeme), value_(lexeme), line_(line), column_(column) {
}

// 完整构造函数
Token::Token(TokenType type, const std::string& lexeme, const std::string& value, int line, int column)
    : type_(type), lexeme_(lexeme), value_(value), line_(line), column_(column) {
}

// 判断是否为字面量
bool Token::isLiteral() const {
    return type_ == TokenType::STRING_LITERAL || 
           type_ == TokenType::SINGLE_QUOTE_STRING ||
           type_ == TokenType::UNQUOTED_LITERAL ||
           type_ == TokenType::NUMBER;
}

// 判断是否为关键字
bool Token::isKeyword() const {
    return type_ >= TokenType::TEXT && type_ <= TokenType::EXCEPT;
}

// 判断是否为操作符
bool Token::isOperator() const {
    return type_ >= TokenType::LEFT_BRACE && type_ <= TokenType::HASH;
}

// 判断是否为注释
bool Token::isComment() const {
    return type_ == TokenType::SINGLE_LINE_COMMENT ||
           type_ == TokenType::MULTI_LINE_COMMENT ||
           type_ == TokenType::GENERATOR_COMMENT;
}

// 判断是否为@前缀类型
bool Token::isAtType() const {
    return type_ >= TokenType::AT_STYLE && type_ <= TokenType::AT_CUSTOM_TYPE;
}

// 判断是否为方括号关键字
bool Token::isBracketKeyword() const {
    return type_ >= TokenType::TEMPLATE && type_ <= TokenType::CONFIGURATION;
}

// 获取Token类型名称
std::string Token::getTypeName(TokenType type) {
    switch (type) {
        case TokenType::EOF_TOKEN: return "EOF";
        
        // 字面量
        case TokenType::STRING_LITERAL: return "STRING_LITERAL";
        case TokenType::SINGLE_QUOTE_STRING: return "SINGLE_QUOTE_STRING";
        case TokenType::UNQUOTED_LITERAL: return "UNQUOTED_LITERAL";
        case TokenType::NUMBER: return "NUMBER";
        
        // 标识符
        case TokenType::IDENTIFIER: return "IDENTIFIER";
        
        // 关键字
        case TokenType::TEXT: return "TEXT";
        case TokenType::STYLE: return "STYLE";
        case TokenType::SCRIPT: return "SCRIPT";
        case TokenType::USE: return "USE";
        case TokenType::INHERIT: return "INHERIT";
        case TokenType::DELETE_KW: return "DELETE";
        case TokenType::INSERT: return "INSERT";
        case TokenType::AFTER: return "AFTER";
        case TokenType::BEFORE: return "BEFORE";
        case TokenType::REPLACE: return "REPLACE";
        case TokenType::AT: return "AT";
        case TokenType::TOP: return "TOP";
        case TokenType::BOTTOM: return "BOTTOM";
        case TokenType::FROM: return "FROM";
        case TokenType::AS: return "AS";
        case TokenType::EXCEPT: return "EXCEPT";
        
        // 方括号关键字
        case TokenType::TEMPLATE: return "[Template]";
        case TokenType::CUSTOM: return "[Custom]";
        case TokenType::ORIGIN: return "[Origin]";
        case TokenType::IMPORT: return "[Import]";
        case TokenType::NAMESPACE: return "[Namespace]";
        case TokenType::CONFIGURATION: return "[Configuration]";
        
        // @前缀类型
        case TokenType::AT_STYLE: return "@Style";
        case TokenType::AT_ELEMENT: return "@Element";
        case TokenType::AT_VAR: return "@Var";
        case TokenType::AT_HTML: return "@Html";
        case TokenType::AT_JAVASCRIPT: return "@JavaScript";
        case TokenType::AT_CHTL: return "@Chtl";
        case TokenType::AT_CJMOD: return "@CJmod";
        case TokenType::AT_CONFIG: return "@Config";
        case TokenType::AT_CUSTOM_TYPE: return "@CustomType";
        
        // 注释
        case TokenType::SINGLE_LINE_COMMENT: return "SINGLE_LINE_COMMENT";
        case TokenType::MULTI_LINE_COMMENT: return "MULTI_LINE_COMMENT";
        case TokenType::GENERATOR_COMMENT: return "GENERATOR_COMMENT";
        
        // 操作符和分隔符
        case TokenType::LEFT_BRACE: return "{";
        case TokenType::RIGHT_BRACE: return "}";
        case TokenType::LEFT_BRACKET: return "[";
        case TokenType::RIGHT_BRACKET: return "]";
        case TokenType::LEFT_PAREN: return "(";
        case TokenType::RIGHT_PAREN: return ")";
        case TokenType::SEMICOLON: return ";";
        case TokenType::COLON: return ":";
        case TokenType::EQUALS: return "=";
        case TokenType::COMMA: return ",";
        case TokenType::DOT: return ".";
        case TokenType::SLASH: return "/";
        case TokenType::STAR: return "*";
        case TokenType::AMPERSAND: return "&";
        case TokenType::HASH: return "#";
        
        // 特殊标记
        case TokenType::WHITESPACE: return "WHITESPACE";
        case TokenType::NEWLINE: return "NEWLINE";
        case TokenType::UNKNOWN: return "UNKNOWN";
        
        default: return "UNDEFINED";
    }
}

// 输出Token信息
std::ostream& operator<<(std::ostream& os, const Token& token) {
    os << "Token(" << token.getTypeName() 
       << ", \"" << token.lexeme_ << "\""
       << ", " << token.line_ << ":" << token.column_ << ")";
    return os;
}

} // namespace CHTL