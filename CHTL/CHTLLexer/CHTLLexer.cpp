#include "CHTLLexer.h"
#include <iostream>
#include <sstream>

namespace CHTL {

std::unordered_map<std::string, TokenType> CHTLLexer::keywords;
std::unordered_map<std::string, TokenType> CHTLLexer::prefixes;
std::unordered_map<std::string, TokenType> CHTLLexer::type_identifiers;

CHTLLexer::CHTLLexer(const std::string& source) : source(source), position(0), line(1), column(1) {
    if (keywords.empty()) {
        initializeKeywords();
    }
}

void CHTLLexer::initializeKeywords() {
    keywords = {
        {"text", TokenType::TEXT},
        {"style", TokenType::STYLE},
        {"script", TokenType::SCRIPT},
        {"inherit", TokenType::INHERIT},
        {"delete", TokenType::DELETE},
        {"insert", TokenType::INSERT},
        {"after", TokenType::AFTER},
        {"before", TokenType::BEFORE},
        {"replace", TokenType::REPLACE},
        {"from", TokenType::FROM},
        {"as", TokenType::AS},
        {"except", TokenType::EXCEPT},
        {"use", TokenType::USE}
    };

    prefixes = {
        {"Template", TokenType::TEMPLATE},
        {"Custom", TokenType::CUSTOM},
        {"Origin", TokenType::ORIGIN},
        {"Import", TokenType::IMPORT},
        {"Namespace", TokenType::NAMESPACE},
        {"Configuration", TokenType::CONFIGURATION},
        {"Info", TokenType::INFO},
        {"Export", TokenType::EXPORT},
        {"Name", TokenType::NAME},
        {"OriginType", TokenType::ORIGINTYPE}
    };

    type_identifiers = {
        {"@Style", TokenType::AT_STYLE},
        {"@Element", TokenType::AT_ELEMENT},
        {"@Var", TokenType::AT_VAR},
        {"@Html", TokenType::AT_HTML},
        {"@JavaScript", TokenType::AT_JAVASCRIPT},
        {"@Chtl", TokenType::AT_CHTL},
        {"@CJmod", TokenType::AT_CJMOD},
        {"@Config", TokenType::AT_CONFIG}
    };
}

char CHTLLexer::peek(size_t offset) const {
    size_t pos = position + offset;
    if (pos >= source.length()) return '\0';
    return source[pos];
}

char CHTLLexer::advance() {
    if (isAtEnd()) return '\0';
    
    char c = source[position++];
    if (c == '\n') {
        line++;
        column = 1;
    } else {
        column++;
    }
    return c;
}

bool CHTLLexer::isAtEnd() const {
    return position >= source.length();
}

bool CHTLLexer::isAlpha(char c) const {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

bool CHTLLexer::isDigit(char c) const {
    return c >= '0' && c <= '9';
}

bool CHTLLexer::isAlphaNumeric(char c) const {
    return isAlpha(c) || isDigit(c);
}

bool CHTLLexer::isWhitespace(char c) const {
    return c == ' ' || c == '\t' || c == '\r';
}

void CHTLLexer::skipWhitespace() {
    while (!isAtEnd() && isWhitespace(peek())) {
        advance();
    }
}

Token CHTLLexer::makeToken(TokenType type, const std::string& value) {
    if (value.empty()) {
        size_t start = position - 1;
        return Token(type, std::string(1, source[start]), line, column - 1);
    }
    return Token(type, value, line, column - value.length());
}

Token CHTLLexer::makeErrorToken(const std::string& message) {
    addError(message);
    return Token(TokenType::INVALID, message, line, column);
}

Token CHTLLexer::scanString(char quote) {
    std::string value;
    
    while (peek() != quote && !isAtEnd()) {
        if (peek() == '\n') {
            line++;
            column = 1;
        }
        
        char c = advance();
        if (c == '\\' && !isAtEnd()) {
            char escaped = advance();
            switch (escaped) {
                case 'n': value += '\n'; break;
                case 't': value += '\t'; break;
                case 'r': value += '\r'; break;
                case '\\': value += '\\'; break;
                case '"': value += '"'; break;
                case '\'': value += '\''; break;
                default: 
                    value += '\\';
                    value += escaped;
                    break;
            }
        } else {
            value += c;
        }
    }
    
    if (isAtEnd()) {
        return makeErrorToken("Unterminated string");
    }
    
    advance(); // closing quote
    return makeToken(TokenType::STRING_LITERAL, value);
}

Token CHTLLexer::scanNumber() {
    while (isDigit(peek())) {
        advance();
    }
    
    if (peek() == '.' && isDigit(peek(1))) {
        advance(); // consume '.'
        while (isDigit(peek())) {
            advance();
        }
    }
    
    size_t start = position - 1;
    while (start > 0 && (isDigit(source[start - 1]) || source[start - 1] == '.')) {
        start--;
    }
    
    std::string value = source.substr(start, position - start);
    return makeToken(TokenType::NUMBER, value);
}

Token CHTLLexer::scanIdentifier() {
    size_t start = position - 1;
    
    while (isAlphaNumeric(peek()) || peek() == '-') {
        advance();
    }
    
    std::string text = source.substr(start, position - start);
    TokenType type = getIdentifierType(text);
    return makeToken(type, text);
}

Token CHTLLexer::scanLineComment() {
    advance(); // first '/'
    advance(); // second '/'
    
    std::string value;
    while (peek() != '\n' && !isAtEnd()) {
        value += advance();
    }
    
    return makeToken(TokenType::LINE_COMMENT, value);
}

Token CHTLLexer::scanBlockComment() {
    advance(); // '/'
    advance(); // '*'
    
    std::string value;
    while (!isAtEnd()) {
        if (peek() == '*' && peek(1) == '/') {
            advance(); // '*'
            advance(); // '/'
            break;
        }
        value += advance();
    }
    
    return makeToken(TokenType::BLOCK_COMMENT, value);
}

Token CHTLLexer::scanGeneratorComment() {
    advance(); // first '-'
    advance(); // second '-'
    
    std::string value;
    while (peek() != '\n' && !isAtEnd()) {
        value += advance();
    }
    
    return makeToken(TokenType::GENERATOR_COMMENT, value);
}

Token CHTLLexer::scanPrefix() {
    advance(); // '['
    
    std::string keyword;
    while (peek() != ']' && !isAtEnd()) {
        keyword += advance();
    }
    
    if (isAtEnd()) {
        return makeErrorToken("Unterminated prefix");
    }
    
    advance(); // ']'
    
    auto it = prefixes.find(keyword);
    if (it != prefixes.end()) {
        return makeToken(it->second, "[" + keyword + "]");
    }
    
    return makeErrorToken("Unknown prefix: " + keyword);
}

Token CHTLLexer::scanTypeIdentifier() {
    size_t start = position - 1; // include '@'
    advance(); // skip '@'
    
    while (isAlphaNumeric(peek())) {
        advance();
    }
    
    std::string text = source.substr(start, position - start);
    auto it = type_identifiers.find(text);
    if (it != type_identifiers.end()) {
        return makeToken(it->second, text);
    }
    
    return makeToken(TokenType::IDENTIFIER, text);
}

Token CHTLLexer::scanSelector() {
    char first = peek();
    size_t start = position;
    
    if (first == '.') {
        advance(); // '.'
        while (isAlphaNumeric(peek()) || peek() == '-' || peek() == '_') {
            advance();
        }
        std::string text = source.substr(start, position - start);
        return makeToken(TokenType::CLASS_SELECTOR, text);
    } else if (first == '#') {
        advance(); // '#'
        while (isAlphaNumeric(peek()) || peek() == '-' || peek() == '_') {
            advance();
        }
        std::string text = source.substr(start, position - start);
        return makeToken(TokenType::ID_SELECTOR, text);
    }
    
    return makeErrorToken("Invalid selector");
}

TokenType CHTLLexer::getIdentifierType(const std::string& text) {
    auto it = keywords.find(text);
    if (it != keywords.end()) {
        return it->second;
    }
    return TokenType::IDENTIFIER;
}

std::vector<Token> CHTLLexer::tokenize() {
    std::vector<Token> tokens;
    errors.clear();
    
    while (!isAtEnd()) {
        Token token = nextToken();
        if (token.type != TokenType::INVALID) {
            tokens.push_back(token);
        }
    }
    
    tokens.push_back(Token(TokenType::EOF_TOKEN, "", line, column));
    return tokens;
}

Token CHTLLexer::nextToken() {
    skipWhitespace();
    
    if (isAtEnd()) {
        return Token(TokenType::EOF_TOKEN, "", line, column);
    }
    
    char c = peek();
    
    switch (c) {
        case '{': advance(); return makeToken(TokenType::LBRACE);
        case '}': advance(); return makeToken(TokenType::RBRACE);
        case '(': advance(); return makeToken(TokenType::LPAREN);
        case ')': advance(); return makeToken(TokenType::RPAREN);
        case '[': 
            if (isAlpha(peek(1))) {
                return scanPrefix();
            } else {
                advance();
                return makeToken(TokenType::LBRACKET);
            }
        case ']': advance(); return makeToken(TokenType::RBRACKET);
        case ';': advance(); return makeToken(TokenType::SEMICOLON);
        case ':': advance(); return makeToken(TokenType::COLON);
        case '=': advance(); return makeToken(TokenType::EQUAL);
        case ',': advance(); return makeToken(TokenType::COMMA);
        case '.':
            if (isAlpha(peek(1))) {
                return scanSelector();
            } else if (isDigit(peek(1))) {
                return scanNumber();
            } else {
                advance();
                return makeToken(TokenType::DOT);
            }
        case '#':
            if (isAlpha(peek(1))) {
                return scanSelector();
            } else {
                advance();
                return makeToken(TokenType::HASH);
            }
        case '/':
            if (peek(1) == '/') {
                return scanLineComment();
            } else if (peek(1) == '*') {
                return scanBlockComment();
            } else {
                advance();
                return makeToken(TokenType::SLASH);
            }
        case '*': advance(); return makeToken(TokenType::STAR);
        case '&': advance(); return makeToken(TokenType::AMPERSAND);
        case '-':
            if (peek(1) == '-') {
                return scanGeneratorComment();
            } else {
                advance();
                return makeToken(TokenType::IDENTIFIER, "-");
            }
        case '@':
            return scanTypeIdentifier();
        case '"':
        case '\'':
            advance();
            return scanString(c);
        case '\n':
            advance();
            return makeToken(TokenType::NEWLINE);
    }
    
    if (isDigit(c)) {
        return scanNumber();
    }
    
    if (isAlpha(c)) {
        return scanIdentifier();
    }
    
    advance();
    return makeErrorToken("Unexpected character: " + std::string(1, c));
}

void CHTLLexer::reset(const std::string& newSource) {
    if (!newSource.empty()) {
        source = newSource;
    }
    position = 0;
    line = 1;
    column = 1;
    errors.clear();
}

void CHTLLexer::addError(const std::string& message) {
    std::stringstream ss;
    ss << "Lexer error at line " << line << ", column " << column << ": " << message;
    errors.push_back(ss.str());
}

} // namespace CHTL