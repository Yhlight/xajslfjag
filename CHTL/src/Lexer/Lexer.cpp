#include "CHTL/Lexer/Lexer.h"
#include <cctype>
#include <algorithm>

namespace CHTL {

Lexer::Lexer(const std::string& source, const Config& config)
    : m_Source(source), m_Config(config) {
    InitializeKeywords();
}

void Lexer::InitializeKeywords() {
    // Initialize keywords from config
    m_Keywords["inherit"] = TokenType::Inherit;
    m_Keywords["delete"] = TokenType::Delete;
    m_Keywords["insert"] = TokenType::Insert;
    m_Keywords["after"] = TokenType::After;
    m_Keywords["before"] = TokenType::Before;
    m_Keywords["replace"] = TokenType::Replace;
    m_Keywords["at"] = TokenType::AtTop; // Will be refined based on context
    m_Keywords["from"] = TokenType::From;
    m_Keywords["as"] = TokenType::As;
    m_Keywords["except"] = TokenType::Except;
    m_Keywords["text"] = TokenType::Text;
    m_Keywords["style"] = TokenType::Style;
    m_Keywords["script"] = TokenType::Script;
    m_Keywords["use"] = TokenType::Use;
    m_Keywords["html5"] = TokenType::Html5;
}

std::vector<Token> Lexer::ScanTokens() {
    while (!IsAtEnd()) {
        m_Start = m_Current;
        ScanToken();
    }
    
    AddToken(TokenType::EndOfFile);
    return m_Tokens;
}

void Lexer::ScanToken() {
    char c = Advance();
    
    switch (c) {
        case ' ': case '\r': case '\t': break; // Ignore whitespace
        case '\n': m_Line++; m_Column = 1; break;
        
        // Single character tokens
        case ':': AddToken(TokenType::Colon); break;
        case ';': AddToken(TokenType::Semicolon); break;
        case '{': AddToken(TokenType::LBrace); break;
        case '}': AddToken(TokenType::RBrace); break;
        case '[': AddToken(TokenType::LBracket); break;
        case ']': AddToken(TokenType::RBracket); break;
        case '(': AddToken(TokenType::LParen); break;
        case ')': AddToken(TokenType::RParen); break;
        case '.': AddToken(TokenType::Dot); break;
        case '#': AddToken(TokenType::Hash); break;
        case '&': AddToken(TokenType::Ampersand); break;
        case '@': AddToken(TokenType::At); break;
        case ',': AddToken(TokenType::Comma); break;
        case '=': AddToken(TokenType::Equal); break;
        
        // Multi-character tokens
        case '-':
            if (Match('-')) {
                HandleGeneratorComment();
            } else if (Match('>')) {
                AddToken(TokenType::Arrow);
            } else {
                AddToken(TokenType::Unknown);
            }
            break;
            
        case '/':
            if (Match('/')) {
                HandleLineComment();
            } else if (Match('*')) {
                HandleBlockComment();
            } else {
                AddToken(TokenType::Unknown);
            }
            break;
            
        // String literals
        case '"': HandleString('"'); break;
        case '\'': HandleString('\''); break;
        
        default:
            if (IsDigit(c)) {
                HandleNumber();
            } else if (IsAlpha(c)) {
                HandleIdentifier();
            } else {
                AddToken(TokenType::Unknown);
            }
            break;
    }
}

bool Lexer::IsAtEnd() {
    return m_Current >= m_Source.length();
}

char Lexer::Advance() {
    m_Column++;
    return m_Source[m_Current++];
}

bool Lexer::Match(char expected) {
    if (IsAtEnd()) return false;
    if (m_Source[m_Current] != expected) return false;
    
    m_Current++;
    m_Column++;
    return true;
}

char Lexer::Peek() {
    if (IsAtEnd()) return '\0';
    return m_Source[m_Current];
}

char Lexer::PeekNext() {
    if (m_Current + 1 >= m_Source.length()) return '\0';
    return m_Source[m_Current + 1];
}

void Lexer::AddToken(TokenType type) {
    AddToken(type, "");
}

void Lexer::AddToken(TokenType type, const std::string& literal) {
    Token token;
    token.Type = type;
    token.Lexeme = literal.empty() ? m_Source.substr(m_Start, m_Current - m_Start) : literal;
    token.Line = m_Line;
    token.Column = m_Column - (m_Current - m_Start);
    m_Tokens.push_back(token);
}

void Lexer::HandleString(char quoteType) {
    std::string value;
    
    while (Peek() != quoteType && !IsAtEnd()) {
        if (Peek() == '\n') {
            m_Line++;
            m_Column = 0;
        }
        if (Peek() == '\\' && PeekNext() == quoteType) {
            // Escaped quote
            value += quoteType;
            Advance(); // Skip backslash
            Advance(); // Skip quote
        } else {
            value += Advance();
        }
    }
    
    if (IsAtEnd()) {
        // Unterminated string error
        AddToken(TokenType::Unknown);
        return;
    }
    
    // Consume closing quote
    Advance();
    
    AddToken(TokenType::String, value);
}

void Lexer::HandleIdentifier() {
    while (IsAlphaNumeric(Peek())) Advance();
    
    std::string text = m_Source.substr(m_Start, m_Current - m_Start);
    
    // Check if it's a keyword
    auto it = m_Keywords.find(text);
    TokenType type = (it != m_Keywords.end()) ? it->second : TokenType::Identifier;
    
    // Special handling for block keywords like [Template]
    if (text == "Template" || text == "Custom" || text == "Origin" || 
        text == "Import" || text == "Namespace" || text == "Configuration") {
        // These should have been preceded by [
        if (m_Tokens.size() > 0 && m_Tokens.back().Type == TokenType::LBracket) {
            // Update the type based on the keyword
            if (text == "Template") type = TokenType::Template;
            else if (text == "Custom") type = TokenType::Custom;
            else if (text == "Origin") type = TokenType::Origin;
            else if (text == "Import") type = TokenType::Import;
            else if (text == "Namespace") type = TokenType::Namespace;
            else if (text == "Configuration") type = TokenType::Configuration;
        }
    }
    
    AddToken(type);
}

void Lexer::HandleNumber() {
    while (IsDigit(Peek())) Advance();
    
    // Look for decimal part
    if (Peek() == '.' && IsDigit(PeekNext())) {
        // Consume the '.'
        Advance();
        
        while (IsDigit(Peek())) Advance();
    }
    
    AddToken(TokenType::Number);
}

void Lexer::HandleUnquotedLiteral() {
    // For unquoted literals in contexts like attribute values
    while (!IsAtEnd() && !IsWhitespace(Peek()) && 
           Peek() != ';' && Peek() != '}' && Peek() != ':') {
        Advance();
    }
    
    std::string literal = m_Source.substr(m_Start, m_Current - m_Start);
    AddToken(TokenType::LiteralString, literal);
}

void Lexer::HandleLineComment() {
    // Skip the comment but don't generate a token
    // Regular comments are not passed to the parser
    while (Peek() != '\n' && !IsAtEnd()) Advance();
}

void Lexer::HandleBlockComment() {
    // Skip the comment but don't generate a token
    while (!IsAtEnd()) {
        if (Peek() == '*' && PeekNext() == '/') {
            // End of comment
            Advance(); // *
            Advance(); // /
            break;
        }
        
        if (Peek() == '\n') {
            m_Line++;
            m_Column = 0;
        }
        
        Advance();
    }
}

void Lexer::HandleGeneratorComment() {
    // Generator comments ARE passed to the parser
    std::string comment;
    
    // Skip any whitespace after --
    while (Peek() == ' ' || Peek() == '\t') Advance();
    
    // Collect the comment content
    while (Peek() != '\n' && !IsAtEnd()) {
        comment += Advance();
    }
    
    AddToken(TokenType::GeneratorComment, comment);
}

bool Lexer::IsDigit(char c) {
    return c >= '0' && c <= '9';
}

bool Lexer::IsAlpha(char c) {
    return (c >= 'a' && c <= 'z') ||
           (c >= 'A' && c <= 'Z') ||
           c == '_';
}

bool Lexer::IsAlphaNumeric(char c) {
    return IsAlpha(c) || IsDigit(c);
}

bool Lexer::IsWhitespace(char c) {
    return c == ' ' || c == '\r' || c == '\t' || c == '\n';
}

}