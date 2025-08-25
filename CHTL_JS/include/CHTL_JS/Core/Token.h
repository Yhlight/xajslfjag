#pragma once

#include <string>

namespace CHTL_JS {

    enum class TokenType {
        // CHTL JS specific tokens
        LBraceBrace,    // {{
        RBraceBrace,    // }}
        Arrow,          // ->
        Listen,         // listen
        Delegate,       // delegate
        Animate,        // animate
        Vir,            // vir

        // Standard tokens
        Identifier,
        StringLiteral,
        Number,
        Semicolon,
        Colon,
        Comma,
        Dot,
        LeftParen,
        RightParen,
        LeftBrace,
        RightBrace,
        LeftBracket,
        RightBracket,

        EndOfFile,
        Unknown
    };

    struct Token {
        TokenType Type;
        std::string Lexeme;
        unsigned int Line;
        unsigned int Column;
    };

}
