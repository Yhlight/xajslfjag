#pragma once

#include "../Core/Token.h"
#include <string>
#include <vector>
#include <map>

namespace CHTL_JS {

    class Lexer {
    public:
        Lexer(const std::string& source);

        std::vector<Token> ScanTokens();

    private:
        void ScanToken();
        bool IsAtEnd();
        char Advance();
        bool Match(char expected);
        char Peek();
        char PeekNext();

        void AddToken(TokenType type);
        void AddToken(TokenType type, const std::string& literal);

        void HandleIdentifier();
        // ... other handlers

    private:
        const std::string& m_Source;
        std::vector<Token> m_Tokens;
        std::map<std::string, TokenType> m_Keywords;
        size_t m_Start = 0;
        size_t m_Current = 0;
        unsigned int m_Line = 1;
        unsigned int m_Column = 1;
    };

}
