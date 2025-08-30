#include "Lexer.h"
#include <iostream>

namespace CHTL {

Lexer::Lexer() : m_debugMode(false) {}
Lexer::~Lexer() = default;

std::vector<Token> Lexer::tokenize(const std::string& code) {
    std::vector<Token> tokens;
    
    size_t pos = 0;
    size_t line = 1;
    size_t column = 1;
    
    while (pos < code.length()) {
        char c = code[pos];
        
        // 跳过空白字符
        if (isWhitespace(c)) {
            if (c == '\n') {
                line++;
                column = 1;
            } else {
                column++;
            }
            pos++;
            continue;
        }
        
        // 识别Token
        if (c == '{') {
            tokens.push_back(createToken(TokenType::BRACE_OPEN, "{", line, column, pos));
        } else if (c == '}') {
            tokens.push_back(createToken(TokenType::BRACE_CLOSE, "}", line, column, pos));
        } else if (c == '[') {
            tokens.push_back(createToken(TokenType::BRACKET_OPEN, "[", line, column, pos));
        } else if (c == ']') {
            tokens.push_back(createToken(TokenType::BRACKET_CLOSE, "]", line, column, pos));
        } else if (c == ':') {
            tokens.push_back(createToken(TokenType::COLON, ":", line, column, pos));
        } else if (c == ';') {
            tokens.push_back(createToken(TokenType::SEMICOLON, ";", line, column, pos));
        } else if (c == '@') {
            tokens.push_back(createToken(TokenType::AT, "@", line, column, pos));
        } else if (isIdentifierChar(c)) {
            // 标识符
            size_t start = pos;
            while (pos < code.length() && (isIdentifierChar(code[pos]) || isDigit(code[pos]))) {
                pos++;
            }
            
            std::string identifier = code.substr(start, pos - start);
            tokens.push_back(createToken(TokenType::IDENTIFIER, identifier, line, column, start));
            column += pos - start;
            continue;
        } else {
            // 未知字符
            tokens.push_back(createToken(TokenType::UNKNOWN, std::string(1, c), line, column, pos));
        }
        
        pos++;
        column++;
    }
    
    // 添加EOF Token
    tokens.push_back(createToken(TokenType::EOF_TOKEN, "", line, column, pos));
    
    if (m_debugMode) {
        std::cout << "  ✓ 词法分析完成: " << tokens.size() << " 个Token" << std::endl;
    }
    
    return tokens;
}

Token Lexer::createToken(TokenType type, const std::string& value, size_t line, size_t column, size_t position) {
    return Token(type, value, line, column, position);
}

bool Lexer::isIdentifierChar(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

bool Lexer::isDigit(char c) {
    return c >= '0' && c <= '9';
}

bool Lexer::isWhitespace(char c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

}