#include "Lexer.h"
#include <cctype>
#include <sstream>

namespace CHTL {

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
    
    // 标识符处理
    if (isAlpha(c) || c == '_') {
        return readIdentifier();
    }
    
    // CSS选择器处理
    if (c == '.' && isAlpha(peekChar())) {
        return readClassSelector();
    }
    if (c == '#' && isAlpha(peekChar())) {
        return readIdSelector();
    }
    if (c == ':' && (peekChar() == ':' || isAlpha(peekChar()))) {
        return readPseudoSelector();
    }
    
    // 括号表达式处理
    if (c == '[') {
        return readBracketExpression();
    }
    
    // @表达式处理
    if (c == '@') {
        return readAtExpression();
    }
    
    // 多字符操作符
    if (c == 'a' && source_.substr(pos_, 6) == "at top") {
        Token token(TokenType::AT_TOP, std::string("at top"), makePosition());
        pos_ += 6;
        column_ += 6;
        return token;
    }
    if (c == 'a' && source_.substr(pos_, 9) == "at bottom") {
        Token token(TokenType::AT_BOTTOM, std::string("at bottom"), makePosition());
        pos_ += 9;
        column_ += 9;
        return token;
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
        case '(': return Token(TokenType::LEFT_PAREN, startPos);
        case ')': return Token(TokenType::RIGHT_PAREN, startPos);
        case ';': return Token(TokenType::SEMICOLON, startPos);
        case ':': return Token(TokenType::COLON, startPos);
        case '=': return Token(TokenType::EQUALS, startPos);
        case ',': return Token(TokenType::COMMA, startPos);
        case '.': return Token(TokenType::DOT, startPos);
        case '/': return Token(TokenType::SLASH, startPos);
        case '&': return Token(TokenType::AMPERSAND, startPos);
        case '\n': return Token(TokenType::NEWLINE, startPos);
        default:
            reportError("Unexpected character: " + std::string(1, c));
            return Token(TokenType::INVALID, startPos);
    }
}

Token Lexer::readBracketExpression() {
    Position startPos = makePosition();
    std::string value;
    
    advance(); // [
    
    while (!isEOF() && currentChar() != ']') {
        value += currentChar();
        advance();
    }
    
    if (isEOF()) {
        reportError("Unterminated bracket expression");
        return Token(TokenType::INVALID, startPos);
    }
    
    advance(); // ]
    
    // 识别括号表达式类型
    if (value == "Template") {
        return Token(TokenType::TEMPLATE, value, startPos);
    } else if (value == "Custom") {
        return Token(TokenType::CUSTOM, value, startPos);
    } else if (value == "Origin") {
        return Token(TokenType::ORIGIN, value, startPos);
    } else if (value == "Import") {
        return Token(TokenType::IMPORT, value, startPos);
    } else if (value == "Configuration") {
        return Token(TokenType::CONFIGURATION, value, startPos);
    } else if (value == "Namespace") {
        return Token(TokenType::NAMESPACE, value, startPos);
    }
    
    return Token(TokenType::IDENTIFIER, value, startPos);
}

Token Lexer::readAtExpression() {
    Position startPos = makePosition();
    std::string value = "@";
    
    advance(); // @
    
    while (!isEOF() && isIdentifierChar(currentChar())) {
        value += currentChar();
        advance();
    }
    
    // 识别@类型
    if (value == "@Style") {
        return Token(TokenType::AT_STYLE, value, startPos);
    } else if (value == "@Element") {
        return Token(TokenType::AT_ELEMENT, value, startPos);
    } else if (value == "@Var") {
        return Token(TokenType::AT_VAR, value, startPos);
    } else if (value == "@Html") {
        return Token(TokenType::AT_HTML, value, startPos);
    } else if (value == "@JavaScript") {
        return Token(TokenType::AT_JAVASCRIPT, value, startPos);
    } else if (value == "@Chtl") {
        return Token(TokenType::AT_CHTL, value, startPos);
    } else if (value == "@CJmod") {
        return Token(TokenType::AT_CJMOD, value, startPos);
    } else if (value == "@Config") {
        return Token(TokenType::AT_CONFIG, value, startPos);
    }
    
    return Token(TokenType::IDENTIFIER, value, startPos);
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

Token Lexer::readPseudoSelector() {
    Position startPos = makePosition();
    std::string value;
    
    if (currentChar() == ':' && peekChar() == ':') {
        // 伪元素 ::
        value = "::";
        advance();
        advance();
        
        while (!isEOF() && isIdentifierChar(currentChar())) {
            value += currentChar();
            advance();
        }
        
        return Token(TokenType::PSEUDO_ELEMENT, value, startPos);
    } else if (currentChar() == ':') {
        // 伪类 :
        value = ":";
        advance();
        
        while (!isEOF() && isIdentifierChar(currentChar())) {
            value += currentChar();
            advance();
        }
        
        return Token(TokenType::PSEUDO_CLASS, value, startPos);
    }
    
    return Token(TokenType::COLON, startPos);
}

// 辅助方法

bool Lexer::isAlpha(char c) const {
    return std::isalpha(c) || c == '_';
}

bool Lexer::isDigit(char c) const {
    return std::isdigit(c);
}

bool Lexer::isAlphaNumeric(char c) const {
    return isAlpha(c) || isDigit(c);
}

bool Lexer::isIdentifierChar(char c) const {
    return isAlphaNumeric(c) || c == '-' || c == '_';
}

bool Lexer::isUnquotedLiteralChar(char c) const {
    return !isWhitespace(c) && c != ';' && c != '{' && c != '}' && 
           c != ':' && c != '=' && c != '"' && c != '\'' && c != '/' && 
           c != '\n' && c != '\r';
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

bool Lexer::isInStyleBlock() const {
    return state_.inStyleBlock;
}

bool Lexer::isInScriptBlock() const {
    return state_.inScriptBlock;
}

bool Lexer::isInTemplateBlock() const {
    return state_.inTemplateBlock;
}

} // namespace CHTL