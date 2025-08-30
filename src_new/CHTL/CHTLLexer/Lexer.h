#pragma once

#include "Token.h"
#include <string>
#include <vector>

namespace CHTL {

class Lexer {
public:
    Lexer();
    ~Lexer();
    
    std::vector<Token> tokenize(const std::string& code);
    
    void enableDebugMode(bool enable) { m_debugMode = enable; }
    
private:
    bool m_debugMode;
    
    Token createToken(TokenType type, const std::string& value, size_t line, size_t column, size_t position);
    bool isIdentifierChar(char c);
    bool isDigit(char c);
    bool isWhitespace(char c);
};

}