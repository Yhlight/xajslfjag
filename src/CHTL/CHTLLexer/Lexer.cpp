#include "Lexer.h"
#include <cctype>
#include <iostream>

namespace CHTL {

Lexer::Lexer(const String& source, const LexerConfig& config)
    : source(source), current(0), position(Position{}), state(LexerState::NORMAL), config(config) {
    reset();
}

Token Lexer::nextToken() {
    if (!config.skipWhitespace) {
        skipWhitespace();
    }
    
    if (isAtEnd()) {
        return Token(TokenType::EOF_TOKEN, "", makePosition());
    }
    
    return scanToken();
}

Token Lexer::peekToken(size_t lookahead) {
    size_t savedCurrent = current;
    Position savedPosition = position;
    LexerState savedState = state;
    
    Token token;
    for (size_t i = 0; i < lookahead; ++i) {
        token = nextToken();
        if (token.type == TokenType::EOF_TOKEN) {
            break;
        }
    }
    
    // 恢复状态
    current = savedCurrent;
    position = savedPosition;
    state = savedState;
    
    return token;
}

bool Lexer::hasMoreTokens() const {
    return !isAtEnd();
}

void Lexer::reset() {
    current = 0;
    position = Position{1, 1, 0};
    state = LexerState::NORMAL;
    errors.clear();
    while (!stateStack.empty()) {
        stateStack.pop();
    }
}

Token Lexer::scanToken() {
    char c = advance();
    
    switch (state) {
        case LexerState::IN_STRING:
            return scanString(c);
        case LexerState::IN_COMMENT_LINE:
            return scanComment();
        case LexerState::IN_COMMENT_BLOCK:
            return scanComment();
        case LexerState::IN_CHTLJS_SELECTOR:
            return scanCHTLJSSelector();
        default:
            break;
    }
    
    // 跳过空白字符
    if (isWhitespace(c)) {
        if (!config.skipWhitespace) {
            String whitespace(1, c);
            while (!isAtEnd() && isWhitespace(peek())) {
                whitespace += advance();
            }
            return Token(TokenType::WHITESPACE, whitespace, makePosition());
        }
        return nextToken();
    }
    
    // 处理换行
    if (isNewline(c)) {
        return Token(TokenType::NEWLINE, String(1, c), makePosition());
    }
    
    // 处理注释
    if (c == '/' && peek() == '/') {
        advance(); // 跳过第二个'/'
        setState(LexerState::IN_COMMENT_LINE);
        return scanComment();
    }
    
    if (c == '/' && peek() == '*') {
        advance(); // 跳过'*'
        setState(LexerState::IN_COMMENT_BLOCK);
        return scanComment();
    }
    
    if (c == '-' && peek() == '-') {
        advance(); // 跳过第二个'-'
        return Token(TokenType::COMMENT_GENERATOR, "--", makePosition());
    }
    
    // 处理CHTL JS选择器 {{
    if (c == '{' && peek() == '{') {
        advance(); // 跳过第二个'{'
        setState(LexerState::IN_CHTLJS_SELECTOR);
        return scanCHTLJSSelector();
    }
    
    // 处理字符串
    if (c == '"' || c == '\'') {
        return scanString(c);
    }
    
    // 处理数字
    if (isDigit(c)) {
        return scanNumber();
    }
    
    // 处理标识符和关键字
    if (isIdentifierStart(c)) {
        return scanIdentifier();
    }
    
    // 处理操作符
    if (SymbolMap::isOperatorChar(c)) {
        return scanOperator();
    }
    
    // 处理选择器
    if (SymbolMap::isSelectorChar(c)) {
        return scanSelector();
    }
    
    // 处理单字符符号
    TokenType symbolType = SymbolMap::getSymbolType(c);
    if (symbolType != TokenType::UNKNOWN) {
        return Token(symbolType, String(1, c), makePosition());
    }
    
    // 处理方括号表达式
    if (c == '[') {
        return scanBracketExpression();
    }
    
    // 未知字符
    reportUnexpectedCharacter(c);
    return Token(TokenType::UNKNOWN, String(1, c), makePosition());
}

Token Lexer::scanIdentifier() {
    Position startPos = makePosition();
    String identifier;
    
    // 向前回退一个字符，因为advance()已经被调用
    current--;
    
    while (!isAtEnd() && isIdentifierPart(peek())) {
        identifier += advance();
    }
    
    // 检查是否是关键字
    TokenType keywordType = KeywordMap::getKeywordType(identifier);
    if (keywordType != TokenType::IDENTIFIER) {
        return Token(keywordType, identifier, startPos);
    }
    
    // 检查是否是HTML元素
    if (HTMLElementMap::isHTMLElement(identifier)) {
        return Token(TokenType::HTML_ELEMENT, identifier, startPos);
    }
    
    // 检查是否是无修饰字面量
    if (config.enableUnquotedLiterals && state == LexerState::IN_ATTRIBUTE_VALUE) {
        return Token(TokenType::LITERAL_UNQUOTED, identifier, startPos);
    }
    
    return Token(TokenType::IDENTIFIER, identifier, startPos);
}

Token Lexer::scanString(char quote) {
    Position startPos = makePosition();
    String str;
    
    if (state != LexerState::IN_STRING) {
        setState(LexerState::IN_STRING);
        // quote字符已经被advance()处理，所以不包含在字符串中
    }
    
    while (!isAtEnd() && peek() != quote) {
        char c = advance();
        if (c == '\\' && !isAtEnd()) {
            // 处理转义字符
            char escaped = advance();
            switch (escaped) {
                case 'n': str += '\n'; break;
                case 't': str += '\t'; break;
                case 'r': str += '\r'; break;
                case '\\': str += '\\'; break;
                case '"': str += '"'; break;
                case '\'': str += '\''; break;
                default: str += escaped; break;
            }
        } else {
            str += c;
        }
    }
    
    if (isAtEnd()) {
        reportUnterminatedString();
        setState(LexerState::NORMAL);
        return Token(TokenType::LITERAL_STRING, str, startPos);
    }
    
    // 消费结束引号
    advance();
    setState(LexerState::NORMAL);
    
    return Token(TokenType::LITERAL_STRING, str, startPos);
}

Token Lexer::scanNumber() {
    Position startPos = makePosition();
    String number;
    
    // 向前回退一个字符
    current--;
    
    while (!isAtEnd() && isDigit(peek())) {
        number += advance();
    }
    
    // 处理小数点
    if (!isAtEnd() && peek() == '.' && current + 1 < source.length() && isDigit(source[current + 1])) {
        number += advance(); // 消费'.'
        while (!isAtEnd() && isDigit(peek())) {
            number += advance();
        }
    }
    
    return Token(TokenType::LITERAL_NUMBER, number, startPos);
}

Token Lexer::scanUnquotedLiteral() {
    Position startPos = makePosition();
    String literal;
    
    // 向前回退一个字符
    current--;
    
    while (!isAtEnd()) {
        char c = peek();
        if (isWhitespace(c) || c == ';' || c == '}' || c == ')' || c == ']') {
            break;
        }
        literal += advance();
    }
    
    return Token(TokenType::LITERAL_UNQUOTED, literal, startPos);
}

Token Lexer::scanComment() {
    Position startPos = makePosition();
    String comment;
    
    if (state == LexerState::IN_COMMENT_LINE) {
        while (!isAtEnd() && !isNewline(peek())) {
            comment += advance();
        }
        setState(LexerState::NORMAL);
        return Token(TokenType::COMMENT_LINE, comment, startPos);
    } else if (state == LexerState::IN_COMMENT_BLOCK) {
        while (!isAtEnd()) {
            if (peek() == '*' && current + 1 < source.length() && source[current + 1] == '/') {
                advance(); // 消费'*'
                advance(); // 消费'/'
                break;
            }
            comment += advance();
        }
        setState(LexerState::NORMAL);
        return Token(TokenType::COMMENT_BLOCK, comment, startPos);
    }
    
    return Token(TokenType::UNKNOWN, comment, startPos);
}

Token Lexer::scanOperator() {
    Position startPos = makePosition();
    String op(1, source[current - 1]);
    
    // 检查双字符操作符
    if (!isAtEnd()) {
        String twoCharOp = op + peek();
        TokenType twoCharType = SymbolMap::getTwoCharSymbolType(twoCharOp);
        if (twoCharType != TokenType::UNKNOWN) {
            advance();
            return Token(twoCharType, twoCharOp, startPos);
        }
        
        // 检查三字符操作符（如 &->）
        if (current + 1 < source.length()) {
            String threeCharOp = twoCharOp + source[current + 1];
            TokenType threeCharType = SymbolMap::getTwoCharSymbolType(threeCharOp);
            if (threeCharType != TokenType::UNKNOWN) {
                advance(); // 第二个字符
                advance(); // 第三个字符
                return Token(threeCharType, threeCharOp, startPos);
            }
        }
    }
    
    // 单字符操作符
    TokenType singleType = SymbolMap::getSymbolType(op[0]);
    return Token(singleType, op, startPos);
}

Token Lexer::scanSelector() {
    Position startPos = makePosition();
    char selectorChar = source[current - 1];
    String selector(1, selectorChar);
    
    // 扫描选择器名称
    while (!isAtEnd() && (isAlphaNumeric(peek()) || peek() == '-' || peek() == '_')) {
        selector += advance();
    }
    
    if (selectorChar == '.') {
        return Token(TokenType::CLASS_SELECTOR, selector, startPos);
    } else if (selectorChar == '#') {
        return Token(TokenType::ID_SELECTOR, selector, startPos);
    } else if (selectorChar == '&') {
        return Token(TokenType::AMPERSAND, selector, startPos);
    }
    
    return Token(TokenType::UNKNOWN, selector, startPos);
}

Token Lexer::scanCHTLJSSelector() {
    Position startPos = makePosition();
    String selector = "{{";
    
    while (!isAtEnd()) {
        if (peek() == '}' && current + 1 < source.length() && source[current + 1] == '}') {
            selector += advance(); // 第一个'}'
            selector += advance(); // 第二个'}'
            break;
        }
        selector += advance();
    }
    
    setState(LexerState::NORMAL);
    return Token(TokenType::DOUBLE_LBRACE, selector, startPos);
}

Token Lexer::scanBracketExpression() {
    Position startPos = makePosition();
    String expr = "[";
    
    setState(LexerState::IN_BRACKET_EXPR);
    
    while (!isAtEnd() && peek() != ']') {
        expr += advance();
    }
    
    if (!isAtEnd()) {
        expr += advance(); // 消费']'
    }
    
    setState(LexerState::NORMAL);
    
    // 检查是否是索引访问
    if (expr.length() > 2) {
        String content = expr.substr(1, expr.length() - 2);
        if (std::all_of(content.begin(), content.end(), ::isdigit)) {
            return Token(TokenType::INDEX_ACCESS, expr, startPos);
        }
    }
    
    // 检查是否是CHTL关键字
    if (expr == "[Template]") return Token(TokenType::TEMPLATE, expr, startPos);
    if (expr == "[Custom]") return Token(TokenType::CUSTOM, expr, startPos);
    if (expr == "[Origin]") return Token(TokenType::ORIGIN, expr, startPos);
    if (expr == "[Import]") return Token(TokenType::IMPORT, expr, startPos);
    if (expr == "[Configuration]") return Token(TokenType::CONFIGURATION, expr, startPos);
    if (expr == "[Namespace]") return Token(TokenType::NAMESPACE, expr, startPos);
    
    return Token(TokenType::LBRACKET, "[", startPos);
}

// 工具方法实现
char Lexer::advance() {
    if (isAtEnd()) return '\0';
    
    char c = source[current++];
    updatePosition(c);
    return c;
}

char Lexer::peek(size_t offset) const {
    size_t pos = current + offset;
    return (pos < source.length()) ? source[pos] : '\0';
}

bool Lexer::isAtEnd() const {
    return current >= source.length();
}

bool Lexer::match(char expected) {
    if (isAtEnd() || peek() != expected) {
        return false;
    }
    advance();
    return true;
}

bool Lexer::match(const String& expected) {
    if (current + expected.length() > source.length()) {
        return false;
    }
    
    for (size_t i = 0; i < expected.length(); ++i) {
        if (source[current + i] != expected[i]) {
            return false;
        }
    }
    
    // 消费匹配的字符
    for (size_t i = 0; i < expected.length(); ++i) {
        advance();
    }
    
    return true;
}

void Lexer::skipWhitespace() {
    while (!isAtEnd() && isWhitespace(peek())) {
        advance();
    }
}

bool Lexer::isAlpha(char c) const {
    return std::isalpha(c);
}

bool Lexer::isDigit(char c) const {
    return std::isdigit(c);
}

bool Lexer::isAlphaNumeric(char c) const {
    return std::isalnum(c);
}

bool Lexer::isIdentifierStart(char c) const {
    return isAlpha(c) || c == '_' || c == '@';
}

bool Lexer::isIdentifierPart(char c) const {
    return isAlphaNumeric(c) || c == '_' || c == '-';
}

bool Lexer::isWhitespace(char c) const {
    return c == ' ' || c == '\t' || c == '\r';
}

bool Lexer::isNewline(char c) const {
    return c == '\n';
}

void Lexer::pushState(LexerState newState) {
    stateStack.push(state);
    state = newState;
}

void Lexer::popState() {
    if (!stateStack.empty()) {
        state = stateStack.top();
        stateStack.pop();
    }
}

void Lexer::reportError(const String& message) {
    String error = "Lexer error at line " + std::to_string(position.line) + 
                   ", column " + std::to_string(position.column) + ": " + message;
    errors.push_back(error);
}

void Lexer::reportUnexpectedCharacter(char c) {
    reportError("Unexpected character '" + String(1, c) + "'");
}

void Lexer::reportUnterminatedString() {
    reportError("Unterminated string literal");
}

void Lexer::reportUnterminatedComment() {
    reportError("Unterminated comment");
}

void Lexer::updatePosition(char c) {
    position.offset++;
    if (c == '\n') {
        position.line++;
        position.column = 1;
    } else {
        position.column++;
    }
}

Position Lexer::makePosition() const {
    return position;
}

String Lexer::getSourceSubstring(size_t start, size_t length) const {
    if (start >= source.length()) return "";
    return source.substr(start, std::min(length, source.length() - start));
}

String Lexer::getCurrentLine() const {
    size_t lineStart = current;
    while (lineStart > 0 && source[lineStart - 1] != '\n') {
        lineStart--;
    }
    
    size_t lineEnd = current;
    while (lineEnd < source.length() && source[lineEnd] != '\n') {
        lineEnd++;
    }
    
    return source.substr(lineStart, lineEnd - lineStart);
}

void Lexer::skipToNextLine() {
    while (!isAtEnd() && !isNewline(peek())) {
        advance();
    }
    if (!isAtEnd()) {
        advance(); // 跳过换行符
    }
}

void Lexer::skipToEndOfBlock() {
    int braceCount = 1;
    while (!isAtEnd() && braceCount > 0) {
        char c = advance();
        if (c == '{') braceCount++;
        else if (c == '}') braceCount--;
    }
}

// 工厂方法实现
std::unique_ptr<Lexer> LexerFactory::createCHTLLexer(const String& source) {
    LexerConfig config;
    config.enableCHTLJS = true;
    config.enableUnquotedLiterals = true;
    config.skipWhitespace = false;
    config.skipComments = false;
    return std::make_unique<Lexer>(source, config);
}

std::unique_ptr<Lexer> LexerFactory::createCHTLJSLexer(const String& source) {
    LexerConfig config;
    config.enableCHTLJS = true;
    config.enableUnquotedLiterals = true;
    config.skipWhitespace = true;
    config.skipComments = true;
    return std::make_unique<Lexer>(source, config);
}

std::unique_ptr<Lexer> LexerFactory::createStyleLexer(const String& source) {
    LexerConfig config;
    config.enableCHTLJS = false;
    config.enableUnquotedLiterals = true;
    config.skipWhitespace = true;
    config.skipComments = false;
    return std::make_unique<Lexer>(source, config);
}

std::unique_ptr<Lexer> LexerFactory::createScriptLexer(const String& source) {
    LexerConfig config;
    config.enableCHTLJS = true;
    config.enableUnquotedLiterals = false;
    config.skipWhitespace = true;
    config.skipComments = false;
    return std::make_unique<Lexer>(source, config);
}

} // namespace CHTL