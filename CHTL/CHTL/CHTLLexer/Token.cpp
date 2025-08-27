#include "Token.h"
#include <sstream>

namespace CHTL {

std::string Token::getTypeString() const {
    switch (type_) {
        case TokenType::IDENTIFIER: return "IDENTIFIER";
        case TokenType::STRING: return "STRING";
        case TokenType::NUMBER: return "NUMBER";
        case TokenType::LITERAL: return "LITERAL";
        
        case TokenType::LEFT_BRACE: return "LEFT_BRACE";
        case TokenType::RIGHT_BRACE: return "RIGHT_BRACE";
        case TokenType::LEFT_BRACKET: return "LEFT_BRACKET";
        case TokenType::RIGHT_BRACKET: return "RIGHT_BRACKET";
        case TokenType::LEFT_PAREN: return "LEFT_PAREN";
        case TokenType::RIGHT_PAREN: return "RIGHT_PAREN";
        case TokenType::SEMICOLON: return "SEMICOLON";
        case TokenType::COLON: return "COLON";
        case TokenType::EQUAL: return "EQUAL";
        case TokenType::COMMA: return "COMMA";
        case TokenType::DOT: return "DOT";
        case TokenType::AT: return "AT";
        case TokenType::AMPERSAND: return "AMPERSAND";
        
        case TokenType::SINGLE_COMMENT: return "SINGLE_COMMENT";
        case TokenType::MULTI_COMMENT: return "MULTI_COMMENT";
        case TokenType::GEN_COMMENT: return "GEN_COMMENT";
        
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
        case TokenType::USE: return "USE";
        case TokenType::HTML5: return "HTML5";
        
        case TokenType::TEMPLATE: return "TEMPLATE";
        case TokenType::CUSTOM: return "CUSTOM";
        case TokenType::ORIGIN: return "ORIGIN";
        case TokenType::IMPORT: return "IMPORT";
        case TokenType::CONFIGURATION: return "CONFIGURATION";
        case TokenType::NAMESPACE: return "NAMESPACE";
        case TokenType::INFO: return "INFO";
        case TokenType::EXPORT: return "EXPORT";
        
        case TokenType::STYLE_TYPE: return "STYLE_TYPE";
        case TokenType::ELEMENT_TYPE: return "ELEMENT_TYPE";
        case TokenType::VAR_TYPE: return "VAR_TYPE";
        case TokenType::HTML_TYPE: return "HTML_TYPE";
        case TokenType::JAVASCRIPT_TYPE: return "JAVASCRIPT_TYPE";
        case TokenType::CONFIG_TYPE: return "CONFIG_TYPE";
        case TokenType::CHTL_TYPE: return "CHTL_TYPE";
        case TokenType::CJMOD_TYPE: return "CJMOD_TYPE";
        
        case TokenType::ARROW: return "ARROW";
        case TokenType::BIND_OPERATOR: return "BIND_OPERATOR";
        
        case TokenType::NEWLINE: return "NEWLINE";
        case TokenType::WHITESPACE: return "WHITESPACE";
        case TokenType::EOF_TOKEN: return "EOF_TOKEN";
        case TokenType::UNKNOWN: return "UNKNOWN";
        
        default: return "UNKNOWN";
    }
}

std::string Token::toString() const {
    std::ostringstream oss;
    oss << "Token{type=" << getTypeString() 
        << ", value=\"" << value_ << "\""
        << ", start=(" << start_pos_.line << ":" << start_pos_.column << ")"
        << ", end=(" << end_pos_.line << ":" << end_pos_.column << ")"
        << "}";
    return oss.str();
}

} // namespace CHTL