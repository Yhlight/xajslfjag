#ifndef CHTL_LEXER_H
#define CHTL_LEXER_H

#include <string>
#include <vector>
#include <unordered_map>

namespace CHTL {

enum class TokenType {
    // CHTL Literals and Identifiers
    STRING_LITERAL,           // "string" 'string'
    UNQUOTED_LITERAL,         // unquoted values like CSS
    NUMBER,                   // 123 123.45
    IDENTIFIER,               // element names, property names
    
    // CHTL Core Keywords  
    TEXT,                     // text keyword
    STYLE,                    // style keyword for local styles
    SCRIPT,                   // script keyword for local scripts
    USE,                      // use declarations
    
    // CHTL Inheritance and Modification Keywords
    INHERIT,                  // inherit keyword
    DELETE,                   // delete keyword for removal
    INSERT,                   // insert keyword for addition
    AFTER, BEFORE, REPLACE,   // position keywords for insert
    AT_TOP, AT_BOTTOM,        // at top, at bottom keywords
    
    // CHTL Import Keywords
    FROM,                     // from keyword in imports
    AS,                       // as keyword for aliases
    EXCEPT,                   // except keyword for exclusions
    
    // CHTL Declaration Prefixes
    TEMPLATE,                 // [Template] prefix
    CUSTOM,                   // [Custom] prefix  
    ORIGIN,                   // [Origin] prefix
    IMPORT,                   // [Import] prefix
    NAMESPACE,                // [Namespace] prefix
    CONFIGURATION,            // [Configuration] prefix
    INFO,                     // [Info] prefix (for CMOD)
    EXPORT,                   // [Export] prefix (for CMOD)
    
    // CHTL Configuration Block Types
    NAME,                     // [Name] config block
    ORIGINTYPE,               // [OriginType] config block
    
    // CHTL Type Identifiers
    AT_STYLE,                 // @Style
    AT_ELEMENT,               // @Element  
    AT_VAR,                   // @Var
    AT_HTML,                  // @Html
    AT_JAVASCRIPT,            // @JavaScript
    AT_CHTL,                  // @Chtl
    AT_CJMOD,                 // @CJmod
    AT_CONFIG,                // @Config
    
    // CHTL Structural Symbols
    LBRACE, RBRACE,           // { } - blocks
    LPAREN, RPAREN,           // ( ) - parameters
    LBRACKET, RBRACKET,       // [ ] - prefixes and arrays
    SEMICOLON,                // ; - statement terminator
    COLON,                    // : - property separator (CE equal)
    EQUAL,                    // = - assignment (CE equal)
    COMMA,                    // , - separator
    DOT,                      // . - path separator
    SLASH,                    // / - path and comments
    STAR,                     // * - comments and wildcards
    AMPERSAND,                // & - reference selector
    HASH,                     // # - for ID selectors
    
    // CHTL Selectors (for local styles)
    CLASS_SELECTOR,           // .className
    ID_SELECTOR,              // #idName
    
    // CHTL Comments (3 types as specified)
    LINE_COMMENT,             // // - ignored by generator
    BLOCK_COMMENT,            // /* */ - ignored by generator  
    GENERATOR_COMMENT,        // -- - processed by generator
    
    // Special Tokens
    NEWLINE,                  // line breaks
    EOF_TOKEN,                // end of file
    INVALID                   // error token
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