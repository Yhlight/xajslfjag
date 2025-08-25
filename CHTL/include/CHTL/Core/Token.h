#pragma once

#include <string>
#include <vector>

namespace CHTL {

    enum TokenType {
        Identifier,      // div, p, my-variable
        String,          // "Hello World"
        Colon,           // :
        Semicolon,       // ;
        LBrace,          // {
        RBrace,          // }
        LBracket,        // [
        RBracket,        // ]
        LParen,          // (
        RParen,          // )
        Dot,             // .
        Hash,            // #
        Ampersand,       // &
        At,              // @
        Inherit,         // inherit
        Delete,          // delete
        Custom,          // Custom (for [Custom])
        Template,        // Template (for [Template])
        Number,          // 123
        // Insert keywords
        Insert,          // insert
        After,           // after
        Before,          // before
        Replace,         // replace
        AtTop,           // at top
        AtBottom,        // at bottom
        Origin,          // [Origin]
        Import,          // [Import]
        From,            // from
        As,              // as
        Namespace,       // [Namespace]
        Except,          // except
        Configuration,   // [Configuration]
        TypeIdentifier,  // @Identifier
        Use,             // use
        Html5,           // html5
        Text,            // text
        Style,           // style
        Script,          // script
        Comma,           // ,
        Equal,           // =
        Arrow,           // ->
        LiteralString,   // Unquoted literal
        GeneratorComment,// -- comment
        EndOfFile,
        Unknown
    };

    struct Token {
        TokenType Type;
        std::string Lexeme;
        unsigned int Line = 0;
        unsigned int Column = 0;
    };

} // namespace CHTL
