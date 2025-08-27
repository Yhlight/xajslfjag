#include "Lexer.h"
#include <cctype>
#include <sstream>
#include <algorithm>

namespace CHTL {

Lexer::Lexer(const std::string& src) 
    : source(src), position(0), line(1), column(1) {
    globalMap = GlobalMap::getInstance();
}

Lexer::~Lexer() {
    // GlobalMap是单例，不在这里销毁
}

char Lexer::getCurrentChar() const {
    if (isAtEnd()) return '\0';
    return source[position];
}

char Lexer::peekChar(int offset) const {
    size_t pos = position + offset;
    if (pos >= source.length()) return '\0';
    return source[pos];
}

void Lexer::advance() {
    if (!isAtEnd()) {
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
    while (!isAtEnd() && isWhitespace(getCurrentChar()) && getCurrentChar() != '\n') {
        advance();
    }
}

void Lexer::skipNewline() {
    while (!isAtEnd() && getCurrentChar() == '\n') {
        advance();
    }
}

Token Lexer::getNextToken() {
    skipWhitespace();
    
    if (isAtEnd()) {
        return makeToken(TokenType::EOF_TOKEN, "", column);
    }
    
    char current = getCurrentChar();
    
    // 处理换行
    if (current == '\n') {
        Token token = makeToken(TokenType::NEWLINE, "\n", column);
        advance();
        return token;
    }
    
    // 处理注释
    if (current == '/') {
        if (peekChar() == '/') {
            return scanSingleLineComment();
        } else if (peekChar() == '*') {
            return scanMultiLineComment();
        }
    }
    
    // 处理生成注释
    if (current == '-' && peekChar() == '-') {
        return scanGenerateComment();
    }
    
    // 处理字符串
    if (current == '"') {
        return scanString('"');
    }
    if (current == '\'') {
        return scanString('\'');
    }
    
    // 处理数字
    if (isDigit(current)) {
        return scanNumber();
    }
    
    // 处理标识符和关键字
    if (isAlpha(current) || current == '_') {
        return scanIdentifierOrKeyword();
    }
    
    // 处理方括号组合（如[Custom]）
    if (current == '[') {
        int startCol = column;
        std::string value = "[";
        advance();
        
        while (!isAtEnd() && getCurrentChar() != ']') {
            value += getCurrentChar();
            advance();
        }
        
        if (getCurrentChar() == ']') {
            value += ']';
            advance();
            
            // 检查是否为关键字
            TokenType type = globalMap->getKeywordType(value);
            if (type != TokenType::UNKNOWN) {
                return makeToken(type, value, startCol);
            }
        }
        
        // 如果不是关键字，作为符号处理
        return makeToken(TokenType::LEFT_BRACKET, "[", startCol);
    }
    
    // 处理符号
    return scanSymbol();
}

Token Lexer::scanIdentifierOrKeyword() {
    int startCol = column;
    std::string value;
    
    while (!isAtEnd() && (isAlphaNumeric(getCurrentChar()) || getCurrentChar() == '_')) {
        value += getCurrentChar();
        advance();
    }
    
    // 检查是否为关键字
    TokenType type = globalMap->getKeywordType(value);
    if (type != TokenType::UNKNOWN) {
        return makeToken(type, value, startCol);
    }
    
    // 检查是否为HTML元素
    if (globalMap->isHtmlElement(value)) {
        return makeToken(TokenType::IDENTIFIER, value, startCol);
    }
    
    return makeToken(TokenType::IDENTIFIER, value, startCol);
}

Token Lexer::scanString(char quote) {
    int startCol = column;
    std::string value;
    advance(); // 跳过开始引号
    
    while (!isAtEnd() && getCurrentChar() != quote) {
        if (getCurrentChar() == '\\') {
            advance();
            if (!isAtEnd()) {
                // 处理转义字符
                char escaped = getCurrentChar();
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
            value += getCurrentChar();
            advance();
        }
    }
    
    if (getCurrentChar() == quote) {
        advance(); // 跳过结束引号
    }
    
    TokenType type = (quote == '"') ? TokenType::STRING_LITERAL : TokenType::SINGLE_STRING_LITERAL;
    return makeToken(type, value, startCol);
}

Token Lexer::scanUnquotedLiteral() {
    int startCol = column;
    std::string value;
    
    while (!isAtEnd() && isValidUnquotedChar(getCurrentChar())) {
        value += getCurrentChar();
        advance();
    }
    
    return makeToken(TokenType::UNQUOTED_LITERAL, value, startCol);
}

Token Lexer::scanNumber() {
    int startCol = column;
    std::string value;
    
    while (!isAtEnd() && isDigit(getCurrentChar())) {
        value += getCurrentChar();
        advance();
    }
    
    // 处理小数点
    if (getCurrentChar() == '.' && isDigit(peekChar())) {
        value += getCurrentChar();
        advance();
        
        while (!isAtEnd() && isDigit(getCurrentChar())) {
            value += getCurrentChar();
            advance();
        }
    }
    
    return makeToken(TokenType::NUMBER, value, startCol);
}

Token Lexer::scanSingleLineComment() {
    int startCol = column;
    std::string value;
    
    advance(); // 跳过第一个 /
    advance(); // 跳过第二个 /
    
    while (!isAtEnd() && getCurrentChar() != '\n') {
        value += getCurrentChar();
        advance();
    }
    
    return makeToken(TokenType::COMMENT_SINGLE, value, startCol);
}

Token Lexer::scanMultiLineComment() {
    int startCol = column;
    std::string value;
    
    advance(); // 跳过 /
    advance(); // 跳过 *
    
    while (!isAtEnd()) {
        if (getCurrentChar() == '*' && peekChar() == '/') {
            advance(); // 跳过 *
            advance(); // 跳过 /
            break;
        }
        value += getCurrentChar();
        advance();
    }
    
    return makeToken(TokenType::COMMENT_MULTI, value, startCol);
}

Token Lexer::scanGenerateComment() {
    int startCol = column;
    std::string value;
    
    advance(); // 跳过第一个 -
    advance(); // 跳过第二个 -
    
    while (!isAtEnd() && getCurrentChar() != '\n') {
        value += getCurrentChar();
        advance();
    }
    
    return makeToken(TokenType::COMMENT_GENERATE, value, startCol);
}

Token Lexer::scanSymbol() {
    int startCol = column;
    char current = getCurrentChar();
    advance();
    
    switch (current) {
        case '@': return makeToken(TokenType::AT, "@", startCol);
        case '.': return makeToken(TokenType::DOT, ".", startCol);
        case ':': return makeToken(TokenType::COLON, ":", startCol);
        case ';': return makeToken(TokenType::SEMICOLON, ";", startCol);
        case ',': return makeToken(TokenType::COMMA, ",", startCol);
        case '=': return makeToken(TokenType::EQUALS, "=", startCol);
        case '{': return makeToken(TokenType::LEFT_BRACE, "{", startCol);
        case '}': return makeToken(TokenType::RIGHT_BRACE, "}", startCol);
        case '[': return makeToken(TokenType::LEFT_BRACKET, "[", startCol);
        case ']': return makeToken(TokenType::RIGHT_BRACKET, "]", startCol);
        case '(': return makeToken(TokenType::LEFT_PAREN, "(", startCol);
        case ')': return makeToken(TokenType::RIGHT_PAREN, ")", startCol);
        case '&': return makeToken(TokenType::AMPERSAND, "&", startCol);
        case '#': return makeToken(TokenType::HASH, "#", startCol);
        default:
            return makeToken(TokenType::UNKNOWN, std::string(1, current), startCol);
    }
}

bool Lexer::isAtEnd() const {
    return position >= source.length();
}

bool Lexer::isAlpha(char c) const {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

bool Lexer::isDigit(char c) const {
    return c >= '0' && c <= '9';
}

bool Lexer::isAlphaNumeric(char c) const {
    return isAlpha(c) || isDigit(c);
}

bool Lexer::isWhitespace(char c) const {
    return c == ' ' || c == '\t' || c == '\r';
}

bool Lexer::isValidUnquotedChar(char c) const {
    // 无引号字面量的有效字符
    return isAlphaNumeric(c) || c == '_' || c == '-' || c == '.';
}

Token Lexer::makeToken(TokenType type, const std::string& value, int startColumn) {
    int length = column - startColumn;
    return Token(type, value, line, startColumn, length);
}

std::vector<Token> Lexer::tokenize() {
    tokens.clear();
    reset();
    
    while (!isAtEnd()) {
        Token token = getNextToken();
        tokens.push_back(token);
    }
    
    // 添加EOF token
    tokens.push_back(makeToken(TokenType::EOF_TOKEN, "", column));
    
    return tokens;
}

void Lexer::reset() {
    position = 0;
    line = 1;
    column = 1;
    tokens.clear();
}

void Lexer::setSource(const std::string& src) {
    source = src;
    reset();
}

std::vector<Token> Lexer::getFilteredTokens() {
    std::vector<Token> filtered;
    
    for (const auto& token : tokens) {
        if (token.type != TokenType::WHITESPACE && 
            token.type != TokenType::NEWLINE &&
            !token.isComment()) {
            filtered.push_back(token);
        }
    }
    
    return filtered;
}

std::string Lexer::getErrorContext() const {
    std::stringstream ss;
    ss << "Line " << line << ", Column " << column;
    
    // 获取当前行的内容
    size_t lineStart = position;
    while (lineStart > 0 && source[lineStart - 1] != '\n') {
        lineStart--;
    }
    
    size_t lineEnd = position;
    while (lineEnd < source.length() && source[lineEnd] != '\n') {
        lineEnd++;
    }
    
    if (lineStart < lineEnd) {
        ss << "\n" << source.substr(lineStart, lineEnd - lineStart);
        ss << "\n" << std::string(column - 1, ' ') << "^";
    }
    
    return ss.str();
}

} // namespace CHTL