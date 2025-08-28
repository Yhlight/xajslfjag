/**
 * CHTL词法分析器Token实现
 * 
 * 实现Token类的所有方法
 * 
 * @author CHTL Team
 * @version 1.0.0
 * @license MIT
 */

#include "Token.h"
#include <sstream>
#include <algorithm>

namespace CHTL {

// Token构造函数
Token::Token(TokenType type, const std::string& value, size_t line, size_t column)
    : m_type(type), m_value(value), m_line(line), m_column(column) {
}

// 拷贝构造函数
Token::Token(const Token& other)
    : m_type(other.m_type), m_value(other.m_value), m_line(other.m_line), m_column(other.m_column) {
}

// 赋值操作符
Token& Token::operator=(const Token& other) {
    if (this != &other) {
        m_type = other.m_type;
        m_value = other.m_value;
        m_line = other.m_line;
        m_column = other.m_column;
    }
    return *this;
}

// 判断是否为关键字Token
bool Token::isKeyword() const {
    switch (m_type) {
        case TokenType::TEMPLATE:
        case TokenType::TEMPLATE_STYLE:
        case TokenType::TEMPLATE_ELEMENT:
        case TokenType::TEMPLATE_VAR:
        case TokenType::CUSTOM:
        case TokenType::CUSTOM_STYLE:
        case TokenType::CUSTOM_ELEMENT:
        case TokenType::CUSTOM_VAR:
        case TokenType::ORIGIN:
        case TokenType::ORIGIN_HTML:
        case TokenType::ORIGIN_STYLE:
        case TokenType::ORIGIN_JAVASCRIPT:
        case TokenType::IMPORT:
        case TokenType::IMPORT_HTML:
        case TokenType::IMPORT_STYLE:
        case TokenType::IMPORT_JAVASCRIPT:
        case TokenType::IMPORT_CHTL:
        case TokenType::IMPORT_CJMOD:
        case TokenType::IMPORT_CONFIG:
        case TokenType::CONFIGURATION:
        case TokenType::CONFIGURATION_CONFIG:
        case TokenType::NAMESPACE:
        case TokenType::TEXT:
        case TokenType::STYLE:
        case TokenType::SCRIPT:
        case TokenType::INHERIT:
        case TokenType::DELETE:
        case TokenType::INSERT:
        case TokenType::AFTER:
        case TokenType::BEFORE:
        case TokenType::REPLACE:
        case TokenType::AT_TOP:
        case TokenType::AT_BOTTOM:
        case TokenType::FROM:
        case TokenType::AS:
        case TokenType::EXCEPT:
        case TokenType::NAME:
        case TokenType::ORIGIN_TYPE:
        case TokenType::USE:
        case TokenType::HTML5:
            return true;
        default:
            return false;
    }
}

// 判断是否为操作符Token
bool Token::isOperator() const {
    switch (m_type) {
        case TokenType::COLON:
        case TokenType::EQUALS:
        case TokenType::DOT:
        case TokenType::SLASH:
        case TokenType::BACKSLASH:
        case TokenType::COMMA:
        case TokenType::SEMICOLON:
            return true;
        default:
            return false;
    }
}

// 判断是否为字面量Token
bool Token::isLiteral() const {
    switch (m_type) {
        case TokenType::STRING_LITERAL:
        case TokenType::UNQUOTED_LITERAL:
        case TokenType::NUMBER_LITERAL:
            return true;
        default:
            return false;
    }
}

// 判断是否为标识符Token
bool Token::isIdentifier() const {
    return m_type == TokenType::IDENTIFIER;
}

// 判断是否为注释Token
bool Token::isComment() const {
    switch (m_type) {
        case TokenType::SINGLE_LINE_COMMENT:
        case TokenType::MULTI_LINE_COMMENT:
        case TokenType::GENERATOR_COMMENT:
            return true;
        default:
            return false;
    }
}

// 获取Token类型名称（用于调试）
std::string Token::getTypeName() const {
    switch (m_type) {
        case TokenType::UNKNOWN: return "UNKNOWN";
        case TokenType::END_OF_FILE: return "END_OF_FILE";
        case TokenType::WHITESPACE: return "WHITESPACE";
        case TokenType::NEWLINE: return "NEWLINE";
        case TokenType::SINGLE_LINE_COMMENT: return "SINGLE_LINE_COMMENT";
        case TokenType::MULTI_LINE_COMMENT: return "MULTI_LINE_COMMENT";
        case TokenType::GENERATOR_COMMENT: return "GENERATOR_COMMENT";
        case TokenType::STRING_LITERAL: return "STRING_LITERAL";
        case TokenType::UNQUOTED_LITERAL: return "UNQUOTED_LITERAL";
        case TokenType::NUMBER_LITERAL: return "NUMBER_LITERAL";
        case TokenType::LEFT_BRACE: return "LEFT_BRACE";
        case TokenType::RIGHT_BRACE: return "RIGHT_BRACE";
        case TokenType::LEFT_BRACKET: return "LEFT_BRACKET";
        case TokenType::RIGHT_BRACKET: return "RIGHT_BRACKET";
        case TokenType::LEFT_PAREN: return "LEFT_PAREN";
        case TokenType::RIGHT_PAREN: return "RIGHT_PAREN";
        case TokenType::SEMICOLON: return "SEMICOLON";
        case TokenType::COLON: return "COLON";
        case TokenType::EQUALS: return "EQUALS";
        case TokenType::COMMA: return "COMMA";
        case TokenType::DOT: return "DOT";
        case TokenType::SLASH: return "SLASH";
        case TokenType::BACKSLASH: return "BACKSLASH";
        case TokenType::TEMPLATE: return "TEMPLATE";
        case TokenType::TEMPLATE_STYLE: return "TEMPLATE_STYLE";
        case TokenType::TEMPLATE_ELEMENT: return "TEMPLATE_ELEMENT";
        case TokenType::TEMPLATE_VAR: return "TEMPLATE_VAR";
        case TokenType::CUSTOM: return "CUSTOM";
        case TokenType::CUSTOM_STYLE: return "CUSTOM_STYLE";
        case TokenType::CUSTOM_ELEMENT: return "CUSTOM_ELEMENT";
        case TokenType::CUSTOM_VAR: return "CUSTOM_VAR";
        case TokenType::ORIGIN: return "ORIGIN";
        case TokenType::ORIGIN_HTML: return "ORIGIN_HTML";
        case TokenType::ORIGIN_STYLE: return "ORIGIN_STYLE";
        case TokenType::ORIGIN_JAVASCRIPT: return "ORIGIN_JAVASCRIPT";
        case TokenType::IMPORT: return "IMPORT";
        case TokenType::IMPORT_HTML: return "IMPORT_HTML";
        case TokenType::IMPORT_STYLE: return "IMPORT_STYLE";
        case TokenType::IMPORT_JAVASCRIPT: return "IMPORT_JAVASCRIPT";
        case TokenType::IMPORT_CHTL: return "IMPORT_CHTL";
        case TokenType::IMPORT_CJMOD: return "IMPORT_CJMOD";
        case TokenType::IMPORT_CONFIG: return "IMPORT_CONFIG";
        case TokenType::CONFIGURATION: return "CONFIGURATION";
        case TokenType::CONFIGURATION_CONFIG: return "CONFIGURATION_CONFIG";
        case TokenType::NAMESPACE: return "NAMESPACE";
        case TokenType::TEXT: return "TEXT";
        case TokenType::STYLE: return "STYLE";
        case TokenType::SCRIPT: return "SCRIPT";
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
        case TokenType::NAME: return "NAME";
        case TokenType::ORIGIN_TYPE: return "ORIGIN_TYPE";
        case TokenType::USE: return "USE";
        case TokenType::HTML5: return "HTML5";
        case TokenType::IDENTIFIER: return "IDENTIFIER";
        case TokenType::AMPERSAND: return "AMPERSAND";
        case TokenType::AT_SYMBOL: return "AT_SYMBOL";
        case TokenType::HASH_SYMBOL: return "HASH_SYMBOL";
        case TokenType::PERIOD: return "PERIOD";
        case TokenType::CLASS_SELECTOR: return "CLASS_SELECTOR";
        case TokenType::ID_SELECTOR: return "ID_SELECTOR";
        case TokenType::PSEUDO_CLASS: return "PSEUDO_CLASS";
        case TokenType::PSEUDO_ELEMENT: return "PSEUDO_ELEMENT";
        case TokenType::TEMPLATE_REFERENCE: return "TEMPLATE_REFERENCE";
        case TokenType::CUSTOM_REFERENCE: return "CUSTOM_REFERENCE";
        case TokenType::VARIABLE_REFERENCE: return "VARIABLE_REFERENCE";
        case TokenType::INHERIT_REFERENCE: return "INHERIT_REFERENCE";
        case TokenType::DELETE_REFERENCE: return "DELETE_REFERENCE";
        case TokenType::INSERT_REFERENCE: return "INSERT_REFERENCE";
        case TokenType::INDEX_ACCESS: return "INDEX_ACCESS";
        case TokenType::WILDCARD: return "WILDCARD";
        case TokenType::WILDCARD_EXTENSION: return "WILDCARD_EXTENSION";
        case TokenType::PATH_SEPARATOR: return "PATH_SEPARATOR";
        case TokenType::MODULE_PREFIX: return "MODULE_PREFIX";
        case TokenType::CONFIG_VALUE: return "CONFIG_VALUE";
        case TokenType::CONFIG_ASSIGNMENT: return "CONFIG_ASSIGNMENT";
        case TokenType::CONFIG_OPTION_COUNT: return "CONFIG_OPTION_COUNT";
        default: return "UNKNOWN_TYPE";
    }
}

// 获取Token的字符串表示
std::string Token::toString() const {
    std::ostringstream oss;
    oss << "Token(" << getTypeName() << ", \"" << m_value << "\", " 
        << m_line << ":" << m_column << ")";
    return oss.str();
}

// TokenFactory实现

// 创建基础Token
std::shared_ptr<Token> TokenFactory::createToken(TokenType type, const std::string& value, size_t line, size_t column) {
    return std::make_shared<Token>(type, value, line, column);
}

// 创建关键字Token
std::shared_ptr<Token> TokenFactory::createKeyword(const std::string& keyword, size_t line, size_t column) {
    // 根据关键字字符串确定Token类型
    std::string lowerKeyword = keyword;
    std::transform(lowerKeyword.begin(), lowerKeyword.end(), lowerKeyword.begin(), ::tolower);
    
    TokenType type = TokenType::IDENTIFIER; // 默认为标识符
    
    if (lowerKeyword == "template") type = TokenType::TEMPLATE;
    else if (lowerKeyword == "custom") type = TokenType::CUSTOM;
    else if (lowerKeyword == "origin") type = TokenType::ORIGIN;
    else if (lowerKeyword == "import") type = TokenType::IMPORT;
    else if (lowerKeyword == "configuration") type = TokenType::CONFIGURATION;
    else if (lowerKeyword == "namespace") type = TokenType::NAMESPACE;
    else if (lowerKeyword == "text") type = TokenType::TEXT;
    else if (lowerKeyword == "style") type = TokenType::STYLE;
    else if (lowerKeyword == "script") type = TokenType::SCRIPT;
    else if (lowerKeyword == "inherit") type = TokenType::INHERIT;
    else if (lowerKeyword == "delete") type = TokenType::DELETE;
    else if (lowerKeyword == "insert") type = TokenType::INSERT;
    else if (lowerKeyword == "after") type = TokenType::AFTER;
    else if (lowerKeyword == "before") type = TokenType::BEFORE;
    else if (lowerKeyword == "replace") type = TokenType::REPLACE;
    else if (lowerKeyword == "from") type = TokenType::FROM;
    else if (lowerKeyword == "as") type = TokenType::AS;
    else if (lowerKeyword == "except") type = TokenType::EXCEPT;
    else if (lowerKeyword == "use") type = TokenType::USE;
    else if (lowerKeyword == "html5") type = TokenType::HTML5;
    
    return std::make_shared<Token>(type, keyword, line, column);
}

// 创建标识符Token
std::shared_ptr<Token> TokenFactory::createIdentifier(const std::string& name, size_t line, size_t column) {
    return std::make_shared<Token>(TokenType::IDENTIFIER, name, line, column);
}

// 创建字符串字面量Token
std::shared_ptr<Token> TokenFactory::createStringLiteral(const std::string& value, size_t line, size_t column) {
    return std::make_shared<Token>(TokenType::STRING_LITERAL, value, line, column);
}

// 创建无修饰字面量Token
std::shared_ptr<Token> TokenFactory::createUnquotedLiteral(const std::string& value, size_t line, size_t column) {
    return std::make_shared<Token>(TokenType::UNQUOTED_LITERAL, value, line, column);
}

// 创建数字字面量Token
std::shared_ptr<Token> TokenFactory::createNumberLiteral(const std::string& value, size_t line, size_t column) {
    return std::make_shared<Token>(TokenType::NUMBER_LITERAL, value, line, column);
}

// 创建操作符Token
std::shared_ptr<Token> TokenFactory::createOperator(const std::string& op, size_t line, size_t column) {
    TokenType type = TokenType::UNKNOWN;
    
    if (op == ":") type = TokenType::COLON;
    else if (op == "=") type = TokenType::EQUALS;
    else if (op == ".") type = TokenType::DOT;
    else if (op == "/") type = TokenType::SLASH;
    else if (op == "\\") type = TokenType::BACKSLASH;
    else if (op == ",") type = TokenType::COMMA;
    else if (op == ";") type = TokenType::SEMICOLON;
    
    return std::make_shared<Token>(type, op, line, column);
}

// 创建分隔符Token
std::shared_ptr<Token> TokenFactory::createDelimiter(const std::string& delimiter, size_t line, size_t column) {
    TokenType type = TokenType::UNKNOWN;
    
    if (delimiter == "{") type = TokenType::LEFT_BRACE;
    else if (delimiter == "}") type = TokenType::RIGHT_BRACE;
    else if (delimiter == "[") type = TokenType::LEFT_BRACKET;
    else if (delimiter == "]") type = TokenType::RIGHT_BRACKET;
    else if (delimiter == "(") type = TokenType::LEFT_PAREN;
    else if (delimiter == ")") type = TokenType::RIGHT_PAREN;
    
    return std::make_shared<Token>(type, delimiter, line, column);
}

// 创建注释Token
std::shared_ptr<Token> TokenFactory::createComment(const std::string& comment, TokenType type, size_t line, size_t column) {
    return std::make_shared<Token>(type, comment, line, column);
}

// 创建特殊Token
std::shared_ptr<Token> TokenFactory::createSpecialToken(TokenType type, const std::string& value, size_t line, size_t column) {
    return std::make_shared<Token>(type, value, line, column);
}

} // namespace CHTL