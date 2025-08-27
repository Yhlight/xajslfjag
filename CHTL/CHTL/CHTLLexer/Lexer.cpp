#include "Lexer.hpp"
#include <iostream>
#include <sstream>
#include <cctype>

namespace CHTL {

Lexer::Lexer() : position(0), line(1), column(1) {}

Lexer::Lexer(const std::string& source) : source(source), position(0), line(1), column(1) {}

void Lexer::setSource(const std::string& source) {
    this->source = source;
    reset();
}

void Lexer::reset() {
    position = 0;
    line = 1;
    column = 1;
    tokens.clear();
}

char Lexer::currentChar() const {
    if (position >= source.length()) {
        return '\0';
    }
    return source[position];
}

char Lexer::peekChar(size_t offset) const {
    size_t peekPos = position + offset;
    if (peekPos >= source.length()) {
        return '\0';
    }
    return source[peekPos];
}

void Lexer::advance() {
    if (position < source.length()) {
        if (source[position] == '\n') {
            line++;
            column = 1;
        } else {
            column++;
        }
        position++;
    }
}

void Lexer::skipWhitespace() {
    while (position < source.length() && std::isspace(currentChar())) {
        advance();
    }
}

bool Lexer::isAlpha(char c) const {
    return std::isalpha(c) || c == '_';
}

bool Lexer::isDigit(char c) const {
    return std::isdigit(c);
}

bool Lexer::isAlphaNumeric(char c) const {
    return isAlpha(c) || isDigit(c);
}

bool Lexer::isValidIdentifierChar(char c) const {
    return isAlphaNumeric(c) || c == '-' || c == '_';
}

Token Lexer::readString() {
    size_t startPos = position;
    size_t startLine = line;
    size_t startColumn = column;
    
    char quote = currentChar();
    advance(); // 跳过开始引号
    
    std::string value;
    while (position < source.length() && currentChar() != quote) {
        if (currentChar() == '\\') {
            advance();
            if (position < source.length()) {
                char escaped = currentChar();
                switch (escaped) {
                    case 'n': value += '\n'; break;
                    case 't': value += '\t'; break;
                    case 'r': value += '\r'; break;
                    case '\\': value += '\\'; break;
                    case '"': value += '"'; break;
                    case '\'': value += '\''; break;
                    default: value += escaped; break;
                }
                advance();
            }
        } else {
            value += currentChar();
            advance();
        }
    }
    
    if (position >= source.length()) {
        reportError("未闭合的字符串字面量");
    }
    
    advance(); // 跳过结束引号
    
    return Token(TokenType::STRING_LITERAL, value, startLine, startColumn, startPos);
}

Token Lexer::readUnquotedLiteral() {
    size_t startPos = position;
    size_t startLine = line;
    size_t startColumn = column;
    
    std::string value;
    while (position < source.length() && 
           isValidIdentifierChar(currentChar()) && 
           currentChar() != '{' && currentChar() != '}' && 
           currentChar() != ';' && currentChar() != ':' &&
           currentChar() != '=' && currentChar() != ',' &&
           !std::isspace(currentChar())) {
        value += currentChar();
        advance();
    }
    
    return Token(TokenType::UNQUOTED_LITERAL, value, startLine, startColumn, startPos);
}

Token Lexer::readNumber() {
    size_t startPos = position;
    size_t startLine = line;
    size_t startColumn = column;
    
    std::string value;
    bool hasDecimal = false;
    
    while (position < source.length() && (isDigit(currentChar()) || currentChar() == '.')) {
        if (currentChar() == '.') {
            if (hasDecimal) break; // 只允许一个小数点
            hasDecimal = true;
        }
        value += currentChar();
        advance();
    }
    
    return Token(TokenType::NUMBER, value, startLine, startColumn, startPos);
}

Token Lexer::readIdentifier() {
    size_t startPos = position;
    size_t startLine = line;
    size_t startColumn = column;
    
    std::string value;
    while (position < source.length() && isValidIdentifierChar(currentChar())) {
        value += currentChar();
        advance();
    }
    
    TokenType type = classifyIdentifier(value);
    return Token(type, value, startLine, startColumn, startPos);
}

Token Lexer::readComment() {
    size_t startPos = position;
    size_t startLine = line;
    size_t startColumn = column;
    
    if (currentChar() == '/' && peekChar() == '/') {
        // 单行注释
        advance(); // /
        advance(); // /
        
        std::string value;
        while (position < source.length() && currentChar() != '\n') {
            value += currentChar();
            advance();
        }
        
        return Token(TokenType::SINGLE_LINE_COMMENT, value, startLine, startColumn, startPos);
    } else if (currentChar() == '/' && peekChar() == '*') {
        // 多行注释
        advance(); // /
        advance(); // *
        
        std::string value;
        while (position < source.length() - 1) {
            if (currentChar() == '*' && peekChar() == '/') {
                advance(); // *
                advance(); // /
                break;
            }
            value += currentChar();
            advance();
        }
        
        return Token(TokenType::MULTI_LINE_COMMENT, value, startLine, startColumn, startPos);
    } else if (currentChar() == '-' && peekChar() == '-') {
        // 生成器注释
        advance(); // -
        advance(); // -
        
        std::string value;
        while (position < source.length() && currentChar() != '\n') {
            value += currentChar();
            advance();
        }
        
        return Token(TokenType::GENERATOR_COMMENT, value, startLine, startColumn, startPos);
    }
    
    return Token(TokenType::UNKNOWN, "", startLine, startColumn, startPos);
}

Token Lexer::readBlockType() {
    size_t startPos = position;
    size_t startLine = line;
    size_t startColumn = column;
    
    std::string value;
    while (position < source.length() && currentChar() != ']') {
        value += currentChar();
        advance();
    }
    
    if (position < source.length() && currentChar() == ']') {
        value += currentChar();
        advance();
    }
    
    TokenType type = KeywordMap::getBlockType(value);
    if (type == TokenType::UNKNOWN) {
        type = TokenType::IDENTIFIER;
    }
    
    return Token(type, value, startLine, startColumn, startPos);
}

Token Lexer::readTypePrefix() {
    size_t startPos = position;
    size_t startLine = line;
    size_t startColumn = column;
    
    std::string value = "@";
    advance(); // @
    
    while (position < source.length() && isAlpha(currentChar())) {
        value += currentChar();
        advance();
    }
    
    TokenType type = KeywordMap::getTypePrefix(value);
    if (type == TokenType::UNKNOWN) {
        type = TokenType::IDENTIFIER;
    }
    
    return Token(type, value, startLine, startColumn, startPos);
}

Token Lexer::handleAtSymbol() {
    if (isAlpha(peekChar())) {
        return readTypePrefix();
    }
    
    size_t startPos = position;
    size_t startLine = line;
    size_t startColumn = column;
    advance();
    
    return Token(TokenType::AT, "@", startLine, startColumn, startPos);
}

Token Lexer::handleBracket() {
    if (currentChar() == '[') {
        return readBlockType();
    }
    
    size_t startPos = position;
    size_t startLine = line;
    size_t startColumn = column;
    
    TokenType type = (currentChar() == '[') ? TokenType::LBRACKET : TokenType::RBRACKET;
    advance();
    
    return Token(type, std::string(1, source[startPos]), startLine, startColumn, startPos);
}

Token Lexer::handleColonEquals() {
    size_t startPos = position;
    size_t startLine = line;
    size_t startColumn = column;
    
    char c = currentChar();
    advance();
    
    TokenType type = (c == ':') ? TokenType::COLON : TokenType::EQUALS;
    return Token(type, std::string(1, c), startLine, startColumn, startPos);
}

Token Lexer::handleDotSharp() {
    size_t startPos = position;
    size_t startLine = line;
    size_t startColumn = column;
    
    char c = currentChar();
    advance();
    
    TokenType type = (c == '.') ? TokenType::DOT : TokenType::SHARP;
    return Token(type, std::string(1, c), startLine, startColumn, startPos);
}

TokenType Lexer::classifyIdentifier(const std::string& identifier) const {
    // 首先检查是否是关键字
    TokenType keywordType = KeywordMap::getKeywordType(identifier);
    if (keywordType != TokenType::UNKNOWN) {
        return keywordType;
    }
    
    // 检查是否是HTML元素
    if (KeywordMap::isValidHTMLElement(identifier)) {
        return TokenType::HTML_ELEMENT;
    }
    
    // 默认为标识符
    return TokenType::IDENTIFIER;
}

Token Lexer::nextToken() {
    skipWhitespace();
    
    if (position >= source.length()) {
        return Token(TokenType::END_OF_FILE, "", line, column, position);
    }
    
    char c = currentChar();
    
    // 字符串字面量
    if (c == '"' || c == '\'') {
        return readString();
    }
    
    // 数字
    if (isDigit(c)) {
        return readNumber();
    }
    
    // 标识符或关键字
    if (isAlpha(c)) {
        return readIdentifier();
    }
    
    // 注释
    if (c == '/' && (peekChar() == '/' || peekChar() == '*')) {
        return readComment();
    }
    
    if (c == '-' && peekChar() == '-') {
        return readComment();
    }
    
    // 特殊符号
    switch (c) {
        case '{': advance(); return Token(TokenType::LBRACE, "{", line, column-1, position-1);
        case '}': advance(); return Token(TokenType::RBRACE, "}", line, column-1, position-1);
        case '(': advance(); return Token(TokenType::LPAREN, "(", line, column-1, position-1);
        case ')': advance(); return Token(TokenType::RPAREN, ")", line, column-1, position-1);
        case ';': advance(); return Token(TokenType::SEMICOLON, ";", line, column-1, position-1);
        case ',': advance(); return Token(TokenType::COMMA, ",", line, column-1, position-1);
        case '&': advance(); return Token(TokenType::AMPERSAND, "&", line, column-1, position-1);
        case '@': return handleAtSymbol();
        case '[': return handleBracket();
        case ']': advance(); return Token(TokenType::RBRACKET, "]", line, column-1, position-1);
        case ':': return handleColonEquals();
        case '=': return handleColonEquals();
        case '.': return handleDotSharp();
        case '#': return handleDotSharp();
        default:
            // 尝试读取无修饰字面量
            if (isValidIdentifierChar(c)) {
                return readUnquotedLiteral();
            }
            
            // 未知字符
            advance();
            return Token(TokenType::UNKNOWN, std::string(1, c), line, column-1, position-1);
    }
}

std::vector<Token> Lexer::tokenize() {
    tokens.clear();
    
    while (hasMoreTokens()) {
        Token token = nextToken();
        tokens.push_back(token);
        
        if (token.type == TokenType::END_OF_FILE) {
            break;
        }
    }
    
    return tokens;
}

const std::vector<Token>& Lexer::getTokens() const {
    return tokens;
}

bool Lexer::hasMoreTokens() const {
    return position < source.length();
}

size_t Lexer::getCurrentPosition() const {
    return position;
}

size_t Lexer::getCurrentLine() const {
    return line;
}

size_t Lexer::getCurrentColumn() const {
    return column;
}

void Lexer::dumpTokens() const {
    std::cout << "=== CHTL Lexer Tokens ===\n";
    for (const auto& token : tokens) {
        std::cout << token.toString() << "\n";
    }
    std::cout << "========================\n";
}

std::string Lexer::getTokensAsString() const {
    std::stringstream ss;
    for (const auto& token : tokens) {
        ss << token.toString() << "\n";
    }
    return ss.str();
}

void Lexer::reportError(const std::string& message) const {
    std::cerr << "CHTL词法错误 [" << line << ":" << column << "]: " << message << std::endl;
}

// LexerFactory 实现
std::unique_ptr<Lexer> LexerFactory::createCHTLLexer() {
    return std::make_unique<Lexer>();
}

std::unique_ptr<Lexer> LexerFactory::createCHTLLexerWithConfig(const GlobalMap::Configuration& config) {
    auto lexer = std::make_unique<Lexer>();
    GlobalMap::getInstance().setConfiguration(config);
    return lexer;
}

}