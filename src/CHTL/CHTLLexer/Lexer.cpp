#include "Lexer.h"
#include <algorithm>
#include <cctype>
#include <sstream>

namespace CHTL {

Lexer::Lexer(const std::string& source) 
    : m_source(source)
    , m_current(0)
    , m_position(1, 1, 0)
    , m_state(LexerState::NORMAL)
    , m_errorCallback(nullptr) {
}

TokenSequence Lexer::tokenize() {
    TokenSequence tokens;
    
    while (!isAtEnd()) {
        Token token = nextToken();
        if (token.isValid()) {
            tokens.push_back(token);
        }
    }
    
    // 添加EOF token
    tokens.push_back(makeToken(TokenType::EOF_TOKEN, ""));
    
    return tokens;
}

Token Lexer::nextToken() {
    // 跳过空白字符（除了有意义的换行符）
    skipWhitespace();
    
    if (isAtEnd()) {
        return makeToken(TokenType::EOF_TOKEN, "");
    }
    
    Position startPos = m_position;
    char c = currentChar();
    
    // 处理单字符token
    switch (c) {
        case '{': advance(); return makeToken(TokenType::LEFT_BRACE, "{", startPos);
        case '}': advance(); return makeToken(TokenType::RIGHT_BRACE, "}", startPos);
        case '(': advance(); return makeToken(TokenType::LEFT_PAREN, "(", startPos);
        case ')': advance(); return makeToken(TokenType::RIGHT_PAREN, ")", startPos);
        case ';': advance(); return makeToken(TokenType::SEMICOLON, ";", startPos);
        case ':': advance(); return makeToken(TokenType::COLON, ":", startPos);
        case '=': advance(); return makeToken(TokenType::EQUAL, "=", startPos);
        case ',': advance(); return makeToken(TokenType::COMMA, ",", startPos);
        case '\n': advance(); return makeToken(TokenType::NEWLINE, "\n", startPos);
    }
    
    // 处理复杂token
    
    // 字符串
    if (c == '"') {
        return scanString('"');
    }
    if (c == '\'') {
        return scanString('\'');
    }
    
    // 注释
    if (c == '/' && peekChar() == '/') {
        return scanLineComment();
    }
    if (c == '/' && peekChar() == '*') {
        return scanBlockComment();
    }
    if (c == '-' && peekChar() == '-') {
        return scanGeneratorComment();
    }
    
    // 方括号表达式
    if (c == '[') {
        return scanBracketExpression();
    }
    
    // 索引访问的右方括号
    if (c == ']') {
        advance();
        return makeToken(TokenType::RIGHT_BRACKET, "]", startPos);
    }
    
    // @表达式
    if (c == '@') {
        return scanAtExpression();
    }
    
    // CSS选择器
    if (c == '.') {
        return scanCssSelector('.');
    }
    if (c == '#') {
        return scanCssSelector('#');
    }
    
    // &符号
    if (c == '&') {
        advance();
        return makeToken(TokenType::AMPERSAND, "&", startPos);
    }
    
    // 标识符或关键字
    if (isIdentifierStart(c)) {
        return scanIdentifier();
    }
    
    // 数字
    if (isDigit(c)) {
        return scanNumber();
    }
    
    // 无修饰字面量
    if (!isWhitespace(c) && c != '{' && c != '}' && c != ';' && c != ':' && c != '=' && c != ',') {
        return scanUnquotedLiteral();
    }
    
    // 未知字符
    std::string unknownChar(1, c);
    advance();
    reportError(LexerError::INVALID_CHARACTER, "Unexpected character: " + unknownChar, startPos);
    return makeToken(TokenType::INVALID, unknownChar, startPos);
}

Token Lexer::peekToken() {
    size_t savedCurrent = m_current;
    Position savedPosition = m_position;
    LexerState savedState = m_state;
    
    Token token = nextToken();
    
    m_current = savedCurrent;
    m_position = savedPosition;
    m_state = savedState;
    
    return token;
}

bool Lexer::isAtEnd() const {
    return m_current >= m_source.length();
}

Position Lexer::getCurrentPosition() const {
    return m_position;
}

const std::vector<LexerErrorInfo>& Lexer::getErrors() const {
    return m_errors;
}

bool Lexer::hasErrors() const {
    return !m_errors.empty();
}

void Lexer::reset(const std::string& source) {
    m_source = source;
    m_current = 0;
    m_position = Position(1, 1, 0);
    m_state = LexerState::NORMAL;
    m_errors.clear();
}

void Lexer::setErrorCallback(std::function<void(const LexerErrorInfo&)> callback) {
    m_errorCallback = callback;
}

// ========== 私有辅助方法实现 ==========

char Lexer::currentChar() const {
    if (isAtEnd()) return '\0';
    return m_source[m_current];
}

char Lexer::peekChar(size_t offset) const {
    size_t pos = m_current + offset;
    if (pos >= m_source.length()) return '\0';
    return m_source[pos];
}

void Lexer::advance() {
    if (!isAtEnd()) {
        updatePosition(currentChar());
        m_current++;
    }
}

void Lexer::skipWhitespace() {
    while (!isAtEnd()) {
        char c = currentChar();
        if (c == ' ' || c == '\r' || c == '\t') {
            advance();
        } else {
            break;
        }
    }
}

bool Lexer::match(char expected) {
    if (isAtEnd() || currentChar() != expected) {
        return false;
    }
    advance();
    return true;
}

bool Lexer::match(const std::string& expected) {
    for (size_t i = 0; i < expected.length(); i++) {
        if (peekChar(i) != expected[i]) {
            return false;
        }
    }
    for (size_t i = 0; i < expected.length(); i++) {
        advance();
    }
    return true;
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

bool Lexer::isIdentifierStart(char c) const {
    return isAlpha(c);
}

bool Lexer::isIdentifierChar(char c) const {
    return isAlphaNumeric(c) || c == '-';
}

bool Lexer::isWhitespace(char c) const {
    return c == ' ' || c == '\r' || c == '\t' || c == '\n';
}

bool Lexer::isNewline(char c) const {
    return c == '\n';
}

Token Lexer::makeToken(TokenType type, const std::string& value) {
    return Token(type, value, m_position);
}

Token Lexer::makeToken(TokenType type, const std::string& value, const Position& startPos) {
    return Token(type, value, startPos);
}

void Lexer::reportError(LexerError type, const std::string& message) {
    reportError(type, message, m_position);
}

void Lexer::reportError(LexerError type, const std::string& message, const Position& pos) {
    LexerErrorInfo error(type, message, pos);
    m_errors.push_back(error);
    
    if (m_errorCallback) {
        m_errorCallback(error);
    }
}

// ========== Token扫描方法实现 ==========

Token Lexer::scanString(char quote) {
    Position startPos = m_position;
    std::string value;
    
    advance(); // 跳过开始的引号
    
    while (!isAtEnd() && currentChar() != quote) {
        char c = currentChar();
        
        if (c == '\\') {
            advance();
            if (isAtEnd()) {
                reportError(LexerError::UNTERMINATED_STRING, "Unterminated string", startPos);
                return makeToken(TokenType::INVALID, value, startPos);
            }
            
            char escaped = currentChar();
            switch (escaped) {
                case 'n': value += '\n'; break;
                case 't': value += '\t'; break;
                case 'r': value += '\r'; break;
                case '\\': value += '\\'; break;
                case '"': value += '"'; break;
                case '\'': value += '\''; break;
                default:
                    reportError(LexerError::INVALID_ESCAPE_SEQUENCE, 
                               "Invalid escape sequence: \\" + std::string(1, escaped), m_position);
                    value += escaped;
                    break;
            }
        } else {
            value += c;
        }
        
        advance();
    }
    
    if (isAtEnd()) {
        reportError(LexerError::UNTERMINATED_STRING, "Unterminated string", startPos);
        return makeToken(TokenType::INVALID, value, startPos);
    }
    
    advance(); // 跳过结束的引号
    
    TokenType type = (quote == '"') ? TokenType::DOUBLE_QUOTED_STRING : TokenType::SINGLE_QUOTED_STRING;
    return makeToken(type, value, startPos);
}

Token Lexer::scanUnquotedLiteral() {
    Position startPos = m_position;
    std::string value;
    
    while (!isAtEnd()) {
        char c = currentChar();
        
        // 检查终止条件
        if (isWhitespace(c) || c == '{' || c == '}' || c == ';' || c == ':' || c == '=' || c == ',' || c == ')') {
            break;
        }
        
        value += c;
        advance();
    }
    
    if (value.empty()) {
        reportError(LexerError::MALFORMED_TOKEN, "Empty unquoted literal", startPos);
        return makeToken(TokenType::INVALID, "", startPos);
    }
    
    return makeToken(TokenType::UNQUOTED_LITERAL, value, startPos);
}

Token Lexer::scanIdentifier() {
    Position startPos = m_position;
    std::string value;
    
    while (!isAtEnd() && isIdentifierChar(currentChar())) {
        value += currentChar();
        advance();
    }
    
    TokenType type = getIdentifierType(value);
    return makeToken(type, value, startPos);
}

Token Lexer::scanNumber() {
    Position startPos = m_position;
    std::string value;
    
    while (!isAtEnd() && isDigit(currentChar())) {
        value += currentChar();
        advance();
    }
    
    return makeToken(TokenType::UNQUOTED_LITERAL, value, startPos);
}

Token Lexer::scanLineComment() {
    Position startPos = m_position;
    std::string value;
    
    advance(); // 跳过第一个 '/'
    advance(); // 跳过第二个 '/'
    
    while (!isAtEnd() && currentChar() != '\n') {
        value += currentChar();
        advance();
    }
    
    return makeToken(TokenType::LINE_COMMENT, value, startPos);
}

Token Lexer::scanBlockComment() {
    Position startPos = m_position;
    std::string value;
    
    advance(); // 跳过 '/'
    advance(); // 跳过 '*'
    
    while (!isAtEnd()) {
        if (currentChar() == '*' && peekChar() == '/') {
            advance(); // 跳过 '*'
            advance(); // 跳过 '/'
            break;
        }
        value += currentChar();
        advance();
    }
    
    if (isAtEnd() && (m_source.length() < 2 || m_source.substr(m_source.length()-2) != "*/")) {
        reportError(LexerError::UNTERMINATED_COMMENT, "Unterminated block comment", startPos);
        return makeToken(TokenType::INVALID, value, startPos);
    }
    
    return makeToken(TokenType::BLOCK_COMMENT, value, startPos);
}

Token Lexer::scanGeneratorComment() {
    Position startPos = m_position;
    std::string value;
    
    advance(); // 跳过第一个 '-'
    advance(); // 跳过第二个 '-'
    
    while (!isAtEnd() && currentChar() != '\n') {
        value += currentChar();
        advance();
    }
    
    return makeToken(TokenType::GENERATOR_COMMENT, value, startPos);
}

Token Lexer::scanBracketExpression() {
    Position startPos = m_position;
    std::string value = "[";
    
    advance(); // 跳过 '['
    
    while (!isAtEnd() && currentChar() != ']') {
        value += currentChar();
        advance();
    }
    
    if (isAtEnd()) {
        reportError(LexerError::MALFORMED_TOKEN, "Unterminated bracket expression", startPos);
        return makeToken(TokenType::INVALID, value, startPos);
    }
    
    value += ']';
    advance(); // 跳过 ']'
    
    // 判断是否为关键字表达式
    if (value == "[Template]") return makeToken(TokenType::TEMPLATE, value, startPos);
    if (value == "[Custom]") return makeToken(TokenType::CUSTOM, value, startPos);
    if (value == "[Origin]") return makeToken(TokenType::ORIGIN, value, startPos);
    if (value == "[Import]") return makeToken(TokenType::IMPORT, value, startPos);
    if (value == "[Namespace]") return makeToken(TokenType::NAMESPACE, value, startPos);
    if (value == "[Configuration]") return makeToken(TokenType::CONFIGURATION, value, startPos);
    if (value == "[Info]") return makeToken(TokenType::INFO, value, startPos);
    if (value == "[Export]") return makeToken(TokenType::EXPORT, value, startPos);
    if (value == "[Name]") return makeToken(TokenType::NAME, value, startPos);
    if (value == "[OriginType]") return makeToken(TokenType::ORIGIN_TYPE, value, startPos);
    
    // 检查是否为索引访问
    std::string inner = value.substr(1, value.length() - 2);
    if (std::all_of(inner.begin(), inner.end(), ::isdigit)) {
        return makeToken(TokenType::INDEX_ACCESS, value, startPos);
    }
    
    return makeToken(TokenType::IDENTIFIER, value, startPos);
}

Token Lexer::scanAtExpression() {
    Position startPos = m_position;
    std::string value = "@";
    
    advance(); // 跳过 '@'
    
    while (!isAtEnd() && isIdentifierChar(currentChar())) {
        value += currentChar();
        advance();
    }
    
    if (value.length() == 1) {
        reportError(LexerError::MALFORMED_TOKEN, "Invalid @ expression", startPos);
        return makeToken(TokenType::INVALID, value, startPos);
    }
    
    // 判断@表达式类型
    if (value == "@Style") return makeToken(TokenType::AT_STYLE, value, startPos);
    if (value == "@Element") return makeToken(TokenType::AT_ELEMENT, value, startPos);
    if (value == "@Var") return makeToken(TokenType::AT_VAR, value, startPos);
    if (value == "@Html") return makeToken(TokenType::AT_HTML, value, startPos);
    if (value == "@JavaScript") return makeToken(TokenType::AT_JAVASCRIPT, value, startPos);
    if (value == "@Config") return makeToken(TokenType::AT_CONFIG, value, startPos);
    if (value == "@Chtl") return makeToken(TokenType::AT_CHTL, value, startPos);
    if (value == "@CJmod") return makeToken(TokenType::AT_CJMOD, value, startPos);
    
    return makeToken(TokenType::IDENTIFIER, value, startPos);
}

Token Lexer::scanCssSelector(char prefix) {
    Position startPos = m_position;
    std::string value;
    value += prefix;
    
    advance(); // 跳过前缀字符
    
    while (!isAtEnd() && isIdentifierChar(currentChar())) {
        value += currentChar();
        advance();
    }
    
    if (value.length() == 1) {
        // 只有前缀字符，按点号处理
        return makeToken(TokenType::DOT, ".", startPos);
    }
    
    TokenType type = (prefix == '.') ? TokenType::CLASS_SELECTOR : TokenType::ID_SELECTOR;
    return makeToken(type, value, startPos);
}

// ========== Token类型判断方法实现 ==========

TokenType Lexer::getIdentifierType(const std::string& identifier) {
    if (isKeyword(identifier)) {
        if (identifier == "text") return TokenType::TEXT;
        if (identifier == "style") return TokenType::STYLE;
        if (identifier == "html5") return TokenType::HTML5;
        if (identifier == "use") return TokenType::USE;
        if (identifier == "inherit") return TokenType::INHERIT;
        if (identifier == "delete") return TokenType::DELETE;
        if (identifier == "insert") return TokenType::INSERT;
        if (identifier == "after") return TokenType::AFTER;
        if (identifier == "before") return TokenType::BEFORE;
        if (identifier == "replace") return TokenType::REPLACE;
        if (identifier == "from") return TokenType::FROM;
        if (identifier == "as") return TokenType::AS;
        if (identifier == "except") return TokenType::EXCEPT;
        // 处理复合关键字
        if (identifier == "at" && peekChar() == ' ') {
            // 需要进一步处理 "at top" 和 "at bottom"
            return TokenType::IDENTIFIER; // 临时返回，需要在上层处理
        }
    }
    
    if (isHtmlElement(identifier)) {
        return TokenType::HTML_ELEMENT;
    }
    
    return TokenType::IDENTIFIER;
}

bool Lexer::isKeyword(const std::string& word) {
    return GLOBAL_MAP.isKeyword(word);
}

bool Lexer::isHtmlElement(const std::string& element) {
    return GLOBAL_MAP.isHtmlElement(element);
}

void Lexer::updatePosition(char c) {
    m_position.offset = m_current;
    if (c == '\n') {
        m_position.line++;
        m_position.column = 1;
    } else {
        m_position.column++;
    }
}

} // namespace CHTL