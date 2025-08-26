#ifndef CHTL_LEXER_H
#define CHTL_LEXER_H

#include <string>
#include <vector>
#include <unordered_map>

namespace CHTL {

enum class TokenType {
    // Literals and Identifiers
    STRING_LITERAL,
    UNQUOTED_LITERAL,
    NUMBER,
    IDENTIFIER,
    
    // Keywords
    TEXT, STYLE, SCRIPT,
    INHERIT, DELETE, INSERT, AFTER, BEFORE, REPLACE,
    FROM, AS, EXCEPT, USE,
    
    // Prefixes
    TEMPLATE, CUSTOM, ORIGIN, IMPORT, NAMESPACE, CONFIGURATION,
    INFO, EXPORT, NAME, ORIGINTYPE,
    
    // Type Identifiers
    AT_STYLE, AT_ELEMENT, AT_VAR, AT_HTML, AT_JAVASCRIPT, AT_CHTL, AT_CJMOD, AT_CONFIG,
    
    // Symbols
    LBRACE, RBRACE,           // { }
    LPAREN, RPAREN,           // ( )
    LBRACKET, RBRACKET,       // [ ]
    SEMICOLON, COLON, EQUAL,  // ; : =
    COMMA, DOT,               // , .
    SLASH, STAR,              // / *
    AMPERSAND,                // &
    HASH,                     // #
    
    // Selectors
    CLASS_SELECTOR,           // .className
    ID_SELECTOR,              // #idName
    
    // Comments
    LINE_COMMENT,             // //
    BLOCK_COMMENT,            // /* */
    GENERATOR_COMMENT,        // --
    
    // Special
    NEWLINE,
    EOF_TOKEN,
    INVALID
};

struct Token {
    TokenType type;
    std::string value;
    size_t line;
    size_t column;
    
    Token(TokenType t = TokenType::INVALID, const std::string& v = "", size_t l = 0, size_t c = 0)
        : type(t), value(v), line(l), column(c) {}
};

class CHTLLexer {
private:
    std::string source;
    size_t position;
    size_t line;
    size_t column;
    std::vector<std::string> errors;
    
    static std::unordered_map<std::string, TokenType> keywords;
    static std::unordered_map<std::string, TokenType> prefixes;
    static std::unordered_map<std::string, TokenType> type_identifiers;
    
    char peek(size_t offset = 0) const;
    char advance();
    bool isAtEnd() const;
    bool isAlpha(char c) const;
    bool isDigit(char c) const;
    bool isAlphaNumeric(char c) const;
    bool isWhitespace(char c) const;
    
    void skipWhitespace();
    Token makeToken(TokenType type, const std::string& value = "");
    Token makeErrorToken(const std::string& message);
    
    Token scanString(char quote);
    Token scanNumber();
    Token scanIdentifier();
    Token scanLineComment();
    Token scanBlockComment();
    Token scanGeneratorComment();
    Token scanPrefix();
    Token scanTypeIdentifier();
    Token scanSelector();
    
    TokenType getIdentifierType(const std::string& text);
    
public:
    CHTLLexer(const std::string& source);
    
    std::vector<Token> tokenize();
    Token nextToken();
    
    const std::vector<std::string>& getErrors() const { return errors; }
    bool hasErrors() const { return !errors.empty(); }
    
    void reset(const std::string& newSource = "");
    
private:
    void addError(const std::string& message);
    static void initializeKeywords();
};

} // namespace CHTL

#endif // CHTL_LEXER_H