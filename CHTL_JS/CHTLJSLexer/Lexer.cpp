#include "Lexer.h"
#include <cctype>
#include <sstream>

namespace CHTL_JS {

Lexer::Lexer(const std::string& source, std::shared_ptr<GlobalMap> globalMap)
    : source_(source), pos_(0), line_(1), column_(1), globalMap_(globalMap) {
    if (!globalMap_) {
        globalMap_ = std::make_shared<GlobalMap>();
    }
}

Token Lexer::nextToken() {
    skipWhitespace();
    
    if (isEOF()) {
        return Token(TokenType::EOF_TOKEN, makePosition());
    }
    
    char c = currentChar();
    
    // 注释处理
    if (c == '/' && peekChar() == '/') {
        return readComment();
    }
    if (c == '/' && peekChar() == '*') {
        return readComment();
    }
    if (c == '-' && peekChar() == '-') {
        return readGeneratorComment();
    }
    
    // 字符串处理
    if (c == '"') {
        return readString('"');
    }
    if (c == '\'') {
        return readString('\'');
    }
    
    // 数字处理
    if (isDigit(c)) {
        return readNumber();
    }
    
    // CHTL JS增强选择器 {{...}}
    if (c == '{' && peekChar() == '{') {
        return readEnhancedSelector();
    }
    
    // 箭头操作符 ->
    if (c == '-' && peekChar() == '>') {
        return readArrow();
    }
    
    // CSS选择器处理
    if (c == '.' && isAlpha(peekChar())) {
        return readClassSelector();
    }
    if (c == '#' && isAlpha(peekChar())) {
        return readIdSelector();
    }
    
    // 标识符处理
    if (isAlpha(c) || c == '_' || c == '$') {
        return readIdentifier();
    }
    
    // 单字符符号
    return readSymbol();
}

Token Lexer::peekToken() {
    size_t savedPos = pos_;
    size_t savedLine = line_;
    size_t savedColumn = column_;
    
    Token token = nextToken();
    
    pos_ = savedPos;
    line_ = savedLine;
    column_ = savedColumn;
    
    return token;
}

Token Lexer::peekToken(size_t offset) {
    size_t savedPos = pos_;
    size_t savedLine = line_;
    size_t savedColumn = column_;
    
    Token token(TokenType::INVALID, makePosition());
    for (size_t i = 0; i <= offset; ++i) {
        token = nextToken();
    }
    
    pos_ = savedPos;
    line_ = savedLine;
    column_ = savedColumn;
    
    return token;
}

bool Lexer::isEOF() const {
    return pos_ >= source_.length();
}

Position Lexer::getCurrentPosition() const {
    return makePosition();
}

void Lexer::reportError(const std::string& message) {
    std::ostringstream oss;
    oss << "Error at line " << line_ << ", column " << column_ << ": " << message;
    errors_.push_back(oss.str());
}

std::vector<std::string> Lexer::getErrors() const {
    return errors_;
}

bool Lexer::hasErrors() const {
    return !errors_.empty();
}

// 私有方法实现

char Lexer::currentChar() const {
    return isEOF() ? '\0' : source_[pos_];
}

char Lexer::peekChar(size_t offset) const {
    size_t peekPos = pos_ + offset;
    return (peekPos >= source_.length()) ? '\0' : source_[peekPos];
}

void Lexer::advance() {
    if (!isEOF()) {
        updatePosition(currentChar());
        ++pos_;
    }
}

void Lexer::skipWhitespace() {
    while (!isEOF() && isWhitespace(currentChar())) {
        advance();
    }
}

void Lexer::skipLine() {
    while (!isEOF() && !isLineEnding(currentChar())) {
        advance();
    }
    if (!isEOF() && isLineEnding(currentChar())) {
        advance();
    }
}

Token Lexer::readString(char quote) {
    Position startPos = makePosition();
    std::string value;
    
    advance(); // 跳过开始引号
    
    while (!isEOF() && currentChar() != quote) {
        if (currentChar() == '\\') {
            advance();
            if (!isEOF()) {
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
    
    if (isEOF()) {
        reportError("Unterminated string literal");
        return Token(TokenType::INVALID, startPos);
    }
    
    advance(); // 跳过结束引号
    
    TokenType type = (quote == '"') ? TokenType::DOUBLE_QUOTED_STRING : TokenType::SINGLE_QUOTED_STRING;
    return Token(type, value, startPos);
}

Token Lexer::readNumber() {
    Position startPos = makePosition();
    std::string value;
    bool hasDecimal = false;
    
    while (!isEOF() && (isDigit(currentChar()) || currentChar() == '.')) {
        if (currentChar() == '.') {
            if (hasDecimal) break;
            hasDecimal = true;
        }
        value += currentChar();
        advance();
    }
    
    return Token(TokenType::NUMBER, value, startPos);
}

Token Lexer::readIdentifier() {
    Position startPos = makePosition();
    std::string value;
    
    while (!isEOF() && isIdentifierChar(currentChar())) {
        value += currentChar();
        advance();
    }
    
    TokenType type = classifyIdentifier(value);
    return Token(type, value, startPos);
}

Token Lexer::readUnquotedLiteral() {
    Position startPos = makePosition();
    std::string value;
    
    while (!isEOF() && isUnquotedLiteralChar(currentChar())) {
        value += currentChar();
        advance();
    }
    
    return Token(TokenType::UNQUOTED_LITERAL, value, startPos);
}

Token Lexer::readComment() {
    Position startPos = makePosition();
    std::string value;
    
    if (currentChar() == '/' && peekChar() == '/') {
        // 单行注释
        advance(); // /
        advance(); // /
        
        while (!isEOF() && !isLineEnding(currentChar())) {
            value += currentChar();
            advance();
        }
        
        return Token(TokenType::LINE_COMMENT, value, startPos);
    } else if (currentChar() == '/' && peekChar() == '*') {
        // 多行注释
        advance(); // /
        advance(); // *
        
        while (!isEOF()) {
            if (currentChar() == '*' && peekChar() == '/') {
                advance(); // *
                advance(); // /
                break;
            }
            value += currentChar();
            advance();
        }
        
        return Token(TokenType::BLOCK_COMMENT, value, startPos);
    }
    
    return Token(TokenType::INVALID, startPos);
}

Token Lexer::readGeneratorComment() {
    Position startPos = makePosition();
    std::string value;
    
    advance(); // -
    advance(); // -
    
    while (!isEOF() && !isLineEnding(currentChar())) {
        value += currentChar();
        advance();
    }
    
    return Token(TokenType::GENERATOR_COMMENT, value, startPos);
}

Token Lexer::readEnhancedSelector() {
    Position startPos = makePosition();
    std::string value;
    
    advance(); // {
    advance(); // {
    
    while (!isEOF() && !(currentChar() == '}' && peekChar() == '}')) {
        value += currentChar();
        advance();
    }
    
    if (isEOF()) {
        reportError("Unterminated enhanced selector");
        return Token(TokenType::INVALID, startPos);
    }
    
    advance(); // }
    advance(); // }
    
    // 注册增强选择器到全局映射
    if (globalMap_) {
        globalMap_->addEnhancedSelector("{{" + value + "}}", value);
    }
    
    return Token(TokenType::ENHANCED_SELECTOR, "{{" + value + "}}", startPos);
}

Token Lexer::readArrow() {
    Position startPos = makePosition();
    
    advance(); // -
    advance(); // >
    
    return Token(TokenType::ARROW, std::string("->"), startPos);
}

Token Lexer::readClassSelector() {
    Position startPos = makePosition();
    std::string value = ".";
    
    advance(); // .
    
    while (!isEOF() && isIdentifierChar(currentChar())) {
        value += currentChar();
        advance();
    }
    
    return Token(TokenType::CLASS_SELECTOR, value, startPos);
}

Token Lexer::readIdSelector() {
    Position startPos = makePosition();
    std::string value = "#";
    
    advance(); // #
    
    while (!isEOF() && isIdentifierChar(currentChar())) {
        value += currentChar();
        advance();
    }
    
    return Token(TokenType::ID_SELECTOR, value, startPos);
}

Token Lexer::readSymbol() {
    Position startPos = makePosition();
    char c = currentChar();
    advance();
    
    switch (c) {
        case '{': 
            state_.braceDepth++;
            return Token(TokenType::LEFT_BRACE, startPos);
        case '}': 
            if (state_.braceDepth > 0) state_.braceDepth--;
            return Token(TokenType::RIGHT_BRACE, startPos);
        case '[': 
            state_.bracketDepth++;
            return Token(TokenType::LEFT_BRACKET, startPos);
        case ']': 
            if (state_.bracketDepth > 0) state_.bracketDepth--;
            return Token(TokenType::RIGHT_BRACKET, startPos);
        case '(': 
            state_.parenDepth++;
            return Token(TokenType::LEFT_PAREN, startPos);
        case ')': 
            if (state_.parenDepth > 0) state_.parenDepth--;
            return Token(TokenType::RIGHT_PAREN, startPos);
        case ';': return Token(TokenType::SEMICOLON, startPos);
        case ':': return Token(TokenType::COLON, startPos);
        case '=': return Token(TokenType::EQUALS, startPos);
        case ',': return Token(TokenType::COMMA, startPos);
        case '.': return Token(TokenType::DOT, startPos);
        case '&': return Token(TokenType::AMPERSAND, startPos);
        case '\n': return Token(TokenType::NEWLINE, startPos);
        default:
            reportError("Unexpected character: " + std::string(1, c));
            return Token(TokenType::INVALID, startPos);
    }
}

// 辅助方法

bool Lexer::isAlpha(char c) const {
    return std::isalpha(c) || c == '_' || c == '$';
}

bool Lexer::isDigit(char c) const {
    return std::isdigit(c);
}

bool Lexer::isAlphaNumeric(char c) const {
    return isAlpha(c) || isDigit(c);
}

bool Lexer::isIdentifierChar(char c) const {
    return isAlphaNumeric(c) || c == '-' || c == '_' || c == '$';
}

bool Lexer::isUnquotedLiteralChar(char c) const {
    return !isWhitespace(c) && c != ';' && c != '{' && c != '}' && 
           c != ':' && c != '=' && c != '"' && c != '\'' && c != '/' && 
           c != '\n' && c != '\r' && c != ',' && c != ')' && c != ']';
}

bool Lexer::isWhitespace(char c) const {
    return c == ' ' || c == '\t' || c == '\r';
}

bool Lexer::isLineEnding(char c) const {
    return c == '\n';
}

TokenType Lexer::classifyIdentifier(const std::string& text) const {
    return getKeywordType(text);
}

Position Lexer::makePosition() const {
    return Position(line_, column_, pos_);
}

void Lexer::updatePosition(char c) {
    if (c == '\n') {
        ++line_;
        column_ = 1;
    } else {
        ++column_;
    }
}

bool Lexer::isInLocalScriptBlock() const {
    return state_.inLocalScriptBlock;
}

bool Lexer::isInListenBlock() const {
    return state_.inListenBlock;
}

bool Lexer::isInDelegateBlock() const {
    return state_.inDelegateBlock;
}

bool Lexer::isInAnimateBlock() const {
    return state_.inAnimateBlock;
}

} // namespace CHTL_JS