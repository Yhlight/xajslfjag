#include "Lexer.h"
#include <iostream>
#include <cctype>
#include <algorithm>

namespace CHTL {

Lexer::Lexer(const std::string& source, std::shared_ptr<GlobalMap> globalMap)
    : source(source), position(0), line(1), column(1),
      tokenStartPos(0), tokenStartLine(1), tokenStartColumn(1),
      globalMap(globalMap) {}

// 主要接口
std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    
    while (!isAtEnd()) {
        Token token = nextToken();
        if (token.type != TokenType::WHITESPACE) {
            tokens.push_back(token);
        }
        if (token.type == TokenType::EOF_TOKEN) {
            break;
        }
    }
    
    return tokens;
}

Token Lexer::nextToken() {
    skipWhitespace();
    
    if (isAtEnd()) {
        return Token(TokenType::EOF_TOKEN, "", line, column, position);
    }
    
    tokenStartPos = position;
    tokenStartLine = line;
    tokenStartColumn = column;
    
    char c = peek();
    
    // 处理特殊序列
    Token specialToken = handleSpecialSequences();
    if (specialToken.type != TokenType::UNKNOWN) {
        return specialToken;
    }
    
    // 处理字符串
    if (c == '"' || c == '\'') {
        return scanString(c);
    }
    
    // 处理注释
    if (c == '/') {
        if (peek(1) == '/') {
            return scanComment();
        } else if (peek(1) == '*') {
            return scanMultiLineComment();
        }
    }
    
    // 处理生成器注释
    if (c == '-' && peek(1) == '-') {
        return scanGeneratorComment();
    }
    
    // 处理方括号关键字
    if (c == '[') {
        Token bracketToken = scanBracketKeyword();
        if (bracketToken.type != TokenType::LEFT_BRACKET) {
            return bracketToken;
        }
    }
    
    // 处理@类型
    if (c == '@') {
        return scanAtType();
    }
    
    // 处理标识符和关键字
    if (isAlpha(c) || c == '_') {
        return scanIdentifier();
    }
    
    // 处理数字
    if (isDigit(c)) {
        return scanNumber();
    }
    
    // 处理符号
    return scanSymbol();
}

// 辅助方法
char Lexer::peek(size_t offset) const {
    size_t pos = position + offset;
    if (pos >= source.length()) {
        return '\0';
    }
    return source[pos];
}

char Lexer::advance() {
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

void Lexer::skipWhitespace() {
    while (!isAtEnd() && isWhitespace(peek())) {
        advance();
    }
}

bool Lexer::isAtEnd() const {
    return position >= source.length();
}

bool Lexer::match(char expected) {
    if (isAtEnd()) return false;
    if (peek() != expected) return false;
    advance();
    return true;
}

bool Lexer::matchSequence(const std::string& sequence) {
    for (size_t i = 0; i < sequence.length(); i++) {
        if (peek(i) != sequence[i]) {
            return false;
        }
    }
    
    // 消费匹配的字符
    for (size_t i = 0; i < sequence.length(); i++) {
        advance();
    }
    return true;
}

// 字符判断
bool Lexer::isAlpha(char c) const {
    return std::isalpha(c) || c == '_';
}

bool Lexer::isDigit(char c) const {
    return std::isdigit(c);
}

bool Lexer::isAlphaNumeric(char c) const {
    return isAlpha(c) || isDigit(c);
}

bool Lexer::isWhitespace(char c) const {
    return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}

bool Lexer::isSymbol(char c) const {
    return c == '{' || c == '}' || c == '[' || c == ']' ||
           c == '(' || c == ')' || c == ';' || c == ':' ||
           c == '=' || c == ',' || c == '.' || c == '&' ||
           c == '#' || c == '*' || c == '/';
}

// Token扫描方法
Token Lexer::scanString(char quote) {
    advance();  // 消费引号
    
    std::string value;
    while (!isAtEnd() && peek() != quote) {
        if (peek() == '\\' && peek(1) == quote) {
            advance();  // 消费反斜杠
            value += advance();  // 添加转义的引号
        } else {
            value += advance();
        }
    }
    
    if (isAtEnd()) {
        reportError("Unterminated string");
        return Token(TokenType::UNKNOWN, value, tokenStartLine, tokenStartColumn, tokenStartPos);
    }
    
    advance();  // 消费结束引号
    return Token(TokenType::STRING_LITERAL, value, tokenStartLine, tokenStartColumn, tokenStartPos);
}

Token Lexer::scanUnquotedLiteral() {
    std::string value;
    
    // 无引号字面量可以包含字母、数字、连字符、下划线等
    while (!isAtEnd() && !isWhitespace(peek()) && peek() != ';' && 
           peek() != '{' && peek() != '}' && peek() != ',' && peek() != ':') {
        value += advance();
    }
    
    return Token(TokenType::UNQUOTED_LITERAL, value, tokenStartLine, tokenStartColumn, tokenStartPos);
}

Token Lexer::scanIdentifier() {
    std::string value;
    
    while (!isAtEnd() && (isAlphaNumeric(peek()) || peek() == '-')) {
        value += advance();
    }
    
    // 检查是否为关键字
    TokenType type = getKeywordType(value);
    if (type != TokenType::IDENTIFIER) {
        return Token(type, value, tokenStartLine, tokenStartColumn, tokenStartPos);
    }
    
    return Token(TokenType::IDENTIFIER, value, tokenStartLine, tokenStartColumn, tokenStartPos);
}

Token Lexer::scanNumber() {
    std::string value;
    
    while (!isAtEnd() && isDigit(peek())) {
        value += advance();
    }
    
    // 处理小数
    if (peek() == '.' && isDigit(peek(1))) {
        value += advance();  // 消费 .
        while (!isAtEnd() && isDigit(peek())) {
            value += advance();
        }
    }
    
    return Token(TokenType::UNQUOTED_LITERAL, value, tokenStartLine, tokenStartColumn, tokenStartPos);
}

Token Lexer::scanComment() {
    advance();  // 消费第一个 /
    advance();  // 消费第二个 /
    
    std::string value;
    while (!isAtEnd() && peek() != '\n') {
        value += advance();
    }
    
    return Token(TokenType::SINGLE_COMMENT, value, tokenStartLine, tokenStartColumn, tokenStartPos);
}

Token Lexer::scanMultiLineComment() {
    advance();  // 消费 /
    advance();  // 消费 *
    
    std::string value;
    while (!isAtEnd()) {
        if (peek() == '*' && peek(1) == '/') {
            advance();  // 消费 *
            advance();  // 消费 /
            break;
        }
        value += advance();
    }
    
    return Token(TokenType::MULTI_COMMENT, value, tokenStartLine, tokenStartColumn, tokenStartPos);
}

Token Lexer::scanGeneratorComment() {
    advance();  // 消费第一个 -
    advance();  // 消费第二个 -
    
    std::string value;
    while (!isAtEnd() && peek() != '\n') {
        value += advance();
    }
    
    return Token(TokenType::GENERATOR_COMMENT, value, tokenStartLine, tokenStartColumn, tokenStartPos);
}

Token Lexer::scanBracketKeyword() {
    size_t startPos = position;
    advance();  // 消费 [
    
    std::string keyword;
    while (!isAtEnd() && peek() != ']' && isAlpha(peek())) {
        keyword += advance();
    }
    
    if (peek() != ']') {
        // 不是方括号关键字，回退
        position = startPos;
        column = tokenStartColumn;
        return scanSymbol();
    }
    
    advance();  // 消费 ]
    
    // 检查是否为有效的方括号关键字
    if (keyword == "Template") {
        return Token(TokenType::TEMPLATE, "[Template]", tokenStartLine, tokenStartColumn, tokenStartPos);
    } else if (keyword == "Custom") {
        return Token(TokenType::CUSTOM, "[Custom]", tokenStartLine, tokenStartColumn, tokenStartPos);
    } else if (keyword == "Origin") {
        return Token(TokenType::ORIGIN, "[Origin]", tokenStartLine, tokenStartColumn, tokenStartPos);
    } else if (keyword == "Import") {
        return Token(TokenType::IMPORT, "[Import]", tokenStartLine, tokenStartColumn, tokenStartPos);
    } else if (keyword == "Namespace") {
        return Token(TokenType::NAMESPACE, "[Namespace]", tokenStartLine, tokenStartColumn, tokenStartPos);
    } else if (keyword == "Configuration") {
        return Token(TokenType::CONFIGURATION, "[Configuration]", tokenStartLine, tokenStartColumn, tokenStartPos);
    } else if (keyword == "Info") {
        return Token(TokenType::INFO, "[Info]", tokenStartLine, tokenStartColumn, tokenStartPos);
    } else if (keyword == "Export") {
        return Token(TokenType::EXPORT, "[Export]", tokenStartLine, tokenStartColumn, tokenStartPos);
    }
    
    // 不是已知的方括号关键字，回退
    position = startPos;
    column = tokenStartColumn;
    return scanSymbol();
}

Token Lexer::scanAtType() {
    std::string value = "@";
    advance();  // 消费 @
    
    // 收集类型名
    while (!isAtEnd() && isAlpha(peek())) {
        value += advance();
    }
    
    // 检查是否为有效的@类型
    auto it = AT_TYPES.find(value);
    if (it != AT_TYPES.end()) {
        return Token(it->second, value, tokenStartLine, tokenStartColumn, tokenStartPos);
    }
    
    // 如果不是已知的@类型，作为标识符返回
    return Token(TokenType::IDENTIFIER, value, tokenStartLine, tokenStartColumn, tokenStartPos);
}

Token Lexer::scanSymbol() {
    char c = advance();
    
    switch (c) {
        case '{':
            return Token(TokenType::LEFT_BRACE, "{", tokenStartLine, tokenStartColumn, tokenStartPos);
            
        case '}':
            return Token(TokenType::RIGHT_BRACE, "}", tokenStartLine, tokenStartColumn, tokenStartPos);
            
        case '[':
            return Token(TokenType::LEFT_BRACKET, "[", tokenStartLine, tokenStartColumn, tokenStartPos);
            
        case ']':
            return Token(TokenType::RIGHT_BRACKET, "]", tokenStartLine, tokenStartColumn, tokenStartPos);
            
        case '(':
            return Token(TokenType::LEFT_PAREN, "(", tokenStartLine, tokenStartColumn, tokenStartPos);
            
        case ')':
            return Token(TokenType::RIGHT_PAREN, ")", tokenStartLine, tokenStartColumn, tokenStartPos);
            
        case ';':
            return Token(TokenType::SEMICOLON, ";", tokenStartLine, tokenStartColumn, tokenStartPos);
            
        case ':':
            return Token(TokenType::COLON, ":", tokenStartLine, tokenStartColumn, tokenStartPos);
            
        case '=':
            return Token(TokenType::EQUALS, "=", tokenStartLine, tokenStartColumn, tokenStartPos);
            
        case ',':
            return Token(TokenType::COMMA, ",", tokenStartLine, tokenStartColumn, tokenStartPos);
            
        case '.':
            return Token(TokenType::DOT, ".", tokenStartLine, tokenStartColumn, tokenStartPos);
            
        case '&':
            return Token(TokenType::AMPERSAND, "&", tokenStartLine, tokenStartColumn, tokenStartPos);
            
        case '#':
            return Token(TokenType::HASH, "#", tokenStartLine, tokenStartColumn, tokenStartPos);
            
        case '*':
            return Token(TokenType::STAR, "*", tokenStartLine, tokenStartColumn, tokenStartPos);
            
        case '/':
            return Token(TokenType::SLASH, "/", tokenStartLine, tokenStartColumn, tokenStartPos);
            
        case '-':
            // 单独的 - 作为标识符的一部分或未知token
            return Token(TokenType::UNKNOWN, std::string(1, c), tokenStartLine, tokenStartColumn, tokenStartPos);
            
        default:
            return Token(TokenType::UNKNOWN, std::string(1, c), tokenStartLine, tokenStartColumn, tokenStartPos);
    }
}

// 特殊处理
Token Lexer::handleSpecialSequences() {
    // 处理 "at top" 和 "at bottom"
    if (matchSequence("at")) {
        skipWhitespace();
        if (matchSequence("top")) {
            return Token(TokenType::AT_TOP, "at top", tokenStartLine, tokenStartColumn, tokenStartPos);
        } else if (matchSequence("bottom")) {
            return Token(TokenType::AT_BOTTOM, "at bottom", tokenStartLine, tokenStartColumn, tokenStartPos);
        }
        // 如果不是 "at top" 或 "at bottom"，回退
        position = tokenStartPos;
        column = tokenStartColumn;
    }
    
    return Token(TokenType::UNKNOWN, "", tokenStartLine, tokenStartColumn, tokenStartPos);
}

TokenType Lexer::getKeywordType(const std::string& word) const {
    auto it = KEYWORDS.find(word);
    if (it != KEYWORDS.end()) {
        return it->second;
    }
    return TokenType::IDENTIFIER;
}

// 错误处理
void Lexer::reportError(const std::string& message) {
    std::cerr << "Lexer Error at line " << line << ", column " << column 
              << ": " << message << std::endl;
}

// 重置
void Lexer::reset() {
    position = 0;
    line = 1;
    column = 1;
    tokenStartPos = 0;
    tokenStartLine = 1;
    tokenStartColumn = 1;
}

// 设置源代码
void Lexer::setSource(const std::string& newSource) {
    source = newSource;
    reset();
}

// 调试
void Lexer::dumpTokens(const std::vector<Token>& tokens) const {
    std::cout << "=== Tokens ===" << std::endl;
    for (const auto& token : tokens) {
        std::cout << token.toString() << std::endl;
    }
}

} // namespace CHTL