#pragma once

#include "CHTL/Core/Token.h"
#include "CHTL/Core/Config.h"
#include <string>
#include <vector>
#include <map>

namespace CHTL {

    class Lexer {
    public:
        Lexer(const std::string& source, const Config& config);

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

        void HandleString(char quoteType);
        void HandleIdentifier();
        void HandleUnquotedLiteral();
        void HandleLineComment();
        void HandleBlockComment();
        void HandleGeneratorComment();

        bool IsDigit(char c);
        bool IsAlpha(char c);
        bool IsAlphaNumeric(char c);
        bool IsWhitespace(char c);

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
