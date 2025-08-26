#ifndef CHTL_LEXER_H
#define CHTL_LEXER_H

#include "Token.h"
#include <vector>
#include <string>

namespace CHTL {

class Lexer {
private:
    std::string source;
    size_t position;
    size_t line;
    size_t column;
    
    char peek() const;
    char advance();
    bool isAtEnd() const;
    bool isAlpha(char c) const;
    bool isDigit(char c) const;
    bool isAlphaNumeric(char c) const;
    
    Token makeToken(TokenType type, const std::string& value = "");
    Token scanString();
    Token scanNumber();
    Token scanIdentifier();
    TokenType getKeywordType(const std::string& text);
    
public:
    Lexer(const std::string& source);
    
    std::vector<Token> tokenize();
    Token nextToken();
    
    const std::vector<std::string>& getErrors() const;
    
private:
    std::vector<std::string> errors;
    void addError(const std::string& message);
};

} // namespace CHTL

#endif