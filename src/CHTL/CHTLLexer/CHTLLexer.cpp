#include "CHTLLexer.h"
#include <iostream>
#include <cctype>
#include <algorithm>

namespace CHTL {

CHTLLexer::CHTLLexer() 
    : inputStream(nullptr)
    , isFileInput(false)
    , currentLine(1)
    , currentColumn(0)
    , currentPosition(0)
    , currentChar('\0')
    , nextChar('\0')
    , currentTokenIndex(0)
    , isInString(false)
    , isInComment(false)
    , isInGeneratorComment(false)
    , isInBracket(false)
    , isInParen(false)
    , isInBrace(false)
    , stringDelimiter('\0')
    , isMultiLineComment(false)
    , commentNestLevel(0) {
}

CHTLLexer::CHTLLexer(const std::string& filename) : CHTLLexer() {
    initialize(filename);
}

CHTLLexer::CHTLLexer(std::istream& stream) : CHTLLexer() {
    initialize(stream);
}

CHTLLexer::~CHTLLexer() {
    if (isFileInput && fileStream.is_open()) {
        fileStream.close();
    }
}

void CHTLLexer::initialize(const std::string& filename) {
    fileStream.open(filename);
    if (!fileStream.is_open()) {
        throw std::runtime_error("无法打开文件: " + filename);
    }
    
    inputStream = &fileStream;
    isFileInput = true;
    resetPosition();
    
    // 读取第一个字符
    advance();
}

void CHTLLexer::initialize(std::istream& stream) {
    inputStream = &stream;
    isFileInput = false;
    resetPosition();
    
    // 读取第一个字符
    advance();
}

void CHTLLexer::setGlobalMap(std::shared_ptr<CHTLGlobalMap> map) {
    globalMap = map;
}

CHTLToken CHTLLexer::getNextToken() {
    // 如果有缓冲的Token，从缓冲区获取
    if (hasBufferedTokens()) {
        return getFromBuffer();
    }
    
    // 跳过空白字符
    skipWhitespace();
    
    // 检查是否到达文件末尾
    if (isEOF()) {
        return CHTLToken(CHTLTokenType::END_OF_FILE, "", currentLine, currentColumn, currentPosition);
    }
    
    // 根据当前字符决定处理方式
    if (isInString) {
        return handleString();
    }
    
    if (isInComment) {
        if (isMultiLineComment) {
            return handleMultiLineComment();
        } else {
            return handleSingleLineComment();
        }
    }
    
    if (isInGeneratorComment) {
        return handleGeneratorComment();
    }
    
    // 处理各种Token类型
    if (currentChar == '/' && nextChar == '/') {
        return handleSingleLineComment();
    }
    
    if (currentChar == '/' && nextChar == '*') {
        return handleMultiLineComment();
    }
    
    if (currentChar == '-' && nextChar == '-') {
        return handleGeneratorComment();
    }
    
    if (currentChar == '"' || currentChar == '\'') {
        return handleString();
    }
    
    if (isDigit(currentChar)) {
        return handleNumber();
    }
    
    if (isLetter(currentChar) || currentChar == '_') {
        return handleIdentifier();
    }
    
    if (isPunctuation(currentChar)) {
        return handlePunctuation();
    }
    
    if (isOperator(currentChar)) {
        return handleOperator();
    }
    
    if (isSpecialSymbol(currentChar)) {
        return handleSpecialSymbol();
    }
    
    // 处理无修饰字面量（文本）
    if (!isWhitespace(currentChar) && !isNewline(currentChar)) {
        return handleTextLiteral();
    }
    
    // 未知字符，报告错误
    reportError("未知字符: " + std::string(1, currentChar));
    advance();
    return createErrorToken("未知字符");
}

std::vector<CHTLToken> CHTLLexer::getAllTokens() {
    std::vector<CHTLToken> tokens;
    reset();
    
    while (hasMoreTokens()) {
        CHTLToken token = getNextToken();
        if (token.type != CHTLTokenType::END_OF_FILE) {
            tokens.push_back(token);
        } else {
            break;
        }
    }
    
    return tokens;
}

void CHTLLexer::reset() {
    if (inputStream) {
        inputStream->clear();
        inputStream->seekg(0);
    }
    resetPosition();
    clearBuffer();
    advance();
}

bool CHTLLexer::hasMoreTokens() const {
    return !isEOF() || hasBufferedTokens();
}

size_t CHTLLexer::getCurrentLine() const {
    return currentLine;
}

size_t CHTLLexer::getCurrentColumn() const {
    return currentColumn;
}

size_t CHTLLexer::getCurrentPosition() const {
    return currentPosition;
}

void CHTLLexer::printTokens() const {
    std::cout << "=== CHTL Lexer Tokens ===" << std::endl;
    for (size_t i = 0; i < tokenBuffer.size(); ++i) {
        const auto& token = tokenBuffer[i];
        std::cout << "[" << i << "] " 
                  << CHTLTokenUtils::tokenTypeToString(token.type)
                  << " '" << token.value << "' "
                  << "(" << token.line << ":" << token.column << ")"
                  << std::endl;
    }
    std::cout << "=========================" << std::endl;
}

void CHTLLexer::printCurrentState() const {
    std::cout << "=== CHTL Lexer State ===" << std::endl;
    std::cout << "Current Position: " << currentPosition << std::endl;
    std::cout << "Current Line: " << currentLine << std::endl;
    std::cout << "Current Column: " << currentColumn << std::endl;
    std::cout << "Current Char: '" << currentChar << "'" << std::endl;
    std::cout << "Next Char: '" << nextChar << "'" << std::endl;
    std::cout << "In String: " << (isInString ? "true" : "false") << std::endl;
    std::cout << "In Comment: " << (isInComment ? "true" : "false") << std::endl;
    std::cout << "In Generator Comment: " << (isInGeneratorComment ? "true" : "false") << std::endl;
    std::cout << "Buffer Size: " << tokenBuffer.size() << std::endl;
    std::cout << "Current Token Index: " << currentTokenIndex << std::endl;
    std::cout << "=========================" << std::endl;
}

// 私有方法实现
void CHTLLexer::advance() {
    if (inputStream && !inputStream->eof()) {
        currentChar = nextChar;
        inputStream->get(nextChar);
        updatePosition();
    } else {
        currentChar = nextChar;
        nextChar = '\0';
    }
}

char CHTLLexer::peek() {
    return currentChar;
}

char CHTLLexer::peekNext() {
    return nextChar;
}

bool CHTLLexer::isEOF() const {
    return currentChar == '\0' && nextChar == '\0';
}

void CHTLLexer::skipWhitespace() {
    while (isWhitespace(currentChar) && !isEOF()) {
        advance();
    }
}

void CHTLLexer::skipNewline() {
    if (isNewline(currentChar)) {
        advance();
        currentLine++;
        currentColumn = 1;
    }
}

CHTLToken CHTLLexer::handleSingleLineComment() {
    std::string comment;
    size_t startLine = currentLine;
    size_t startColumn = currentColumn;
    size_t startPosition = currentPosition;
    
    // 跳过 //
    advance(); // 跳过第一个 /
    advance(); // 跳过第二个 /
    
    // 读取注释内容直到换行或EOF
    while (!isEOF() && !isNewline(currentChar)) {
        comment += currentChar;
        advance();
    }
    
    return CHTLToken(CHTLTokenType::SINGLE_LINE_COMMENT, comment, startLine, startColumn, startPosition);
}

CHTLToken CHTLLexer::handleMultiLineComment() {
    std::string comment;
    size_t startLine = currentLine;
    size_t startColumn = currentColumn;
    size_t startPosition = currentPosition;
    
    // 跳过 /*
    advance(); // 跳过 /
    advance(); // 跳过 *
    
    // 读取注释内容直到 */
    while (!isEOF()) {
        if (currentChar == '*' && nextChar == '/') {
            advance(); // 跳过 *
            advance(); // 跳过 /
            break;
        }
        comment += currentChar;
        advance();
    }
    
    return CHTLToken(CHTLTokenType::MULTI_LINE_COMMENT, comment, startLine, startColumn, startPosition);
}

CHTLToken CHTLLexer::handleGeneratorComment() {
    std::string comment;
    size_t startLine = currentLine;
    size_t startColumn = currentColumn;
    size_t startPosition = currentPosition;
    
    // 跳过 --
    advance(); // 跳过第一个 -
    advance(); // 跳过第二个 -
    
    // 读取注释内容直到换行或EOF
    while (!isEOF() && !isNewline(currentChar)) {
        comment += currentChar;
        advance();
    }
    
    return CHTLToken(CHTLTokenType::GENERATOR_COMMENT, comment, startLine, startColumn, startPosition);
}

CHTLToken CHTLLexer::handleString() {
    std::string str;
    size_t startLine = currentLine;
    size_t startColumn = currentColumn;
    size_t startPosition = currentPosition;
    
    char delimiter = currentChar;
    advance(); // 跳过开始引号
    
    // 读取字符串内容
    while (!isEOF() && currentChar != delimiter) {
        if (currentChar == '\\' && nextChar != '\0') {
            // 处理转义字符
            advance(); // 跳过 \
            str += currentChar;
        } else {
            str += currentChar;
        }
        advance();
    }
    
    if (currentChar == delimiter) {
        advance(); // 跳过结束引号
    }
    
    CHTLTokenType tokenType = (delimiter == '"') ? 
        CHTLTokenType::DOUBLE_QUOTE_STRING : CHTLTokenType::SINGLE_QUOTE_STRING;
    
    return CHTLToken(tokenType, str, startLine, startColumn, startPosition);
}

CHTLToken CHTLLexer::handleTextLiteral() {
    std::string literal;
    size_t startLine = currentLine;
    size_t startColumn = currentColumn;
    size_t startPosition = currentPosition;
    
    // 读取无修饰字面量直到遇到空白、标点符号或操作符
    while (!isEOF() && !isWhitespace(currentChar) && !isPunctuation(currentChar) && 
           !isOperator(currentChar) && !isSpecialSymbol(currentChar)) {
        literal += currentChar;
        advance();
    }
    
    return CHTLToken(CHTLTokenType::TEXT_LITERAL, literal, startLine, startColumn, startPosition);
}

CHTLToken CHTLLexer::handleNumber() {
    std::string number;
    size_t startLine = currentLine;
    size_t startColumn = currentColumn;
    size_t startPosition = currentPosition;
    
    // 读取数字
    while (!isEOF() && (isDigit(currentChar) || currentChar == '.')) {
        number += currentChar;
        advance();
    }
    
    return CHTLToken(CHTLTokenType::NUMBER, number, startLine, startColumn, startPosition);
}

CHTLToken CHTLLexer::handleIdentifier() {
    std::string identifier;
    size_t startLine = currentLine;
    size_t startColumn = currentColumn;
    size_t startPosition = currentPosition;
    
    // 读取标识符
    while (!isEOF() && (isLetter(currentChar) || isDigit(currentChar) || currentChar == '_')) {
        identifier += currentChar;
        advance();
    }
    
    // 检查是否为关键字
    if (isKeyword(identifier)) {
        return handleKeyword(identifier);
    }
    
    // 检查是否为类型标识符
    if (isTypeIdentifier(identifier)) {
        return handleTypeIdentifier(identifier);
    }
    
    // 检查是否为操作符关键字
    if (isOperatorKeyword(identifier)) {
        return handleKeyword(identifier);
    }
    
    return CHTLToken(CHTLTokenType::IDENTIFIER, identifier, startLine, startColumn, startPosition);
}

CHTLToken CHTLLexer::handleKeyword(const std::string& text) {
    // 这里需要根据具体的CHTL语法文档实现关键字识别
    // 暂时返回标识符，后续完善
    return CHTLToken(CHTLTokenType::IDENTIFIER, text, currentLine, currentColumn, currentPosition);
}

CHTLToken CHTLLexer::handleTypeIdentifier(const std::string& text) {
    // 这里需要根据具体的CHTL语法文档实现类型标识符识别
    // 暂时返回标识符，后续完善
    return CHTLToken(CHTLTokenType::IDENTIFIER, text, currentLine, currentColumn, currentPosition);
}

CHTLToken CHTLLexer::handlePunctuation() {
    CHTLTokenType tokenType = CHTLTokenType::UNKNOWN;
    
    switch (currentChar) {
        case '{': tokenType = CHTLTokenType::LEFT_BRACE; break;
        case '}': tokenType = CHTLTokenType::RIGHT_BRACE; break;
        case '[': tokenType = CHTLTokenType::LEFT_BRACKET; break;
        case ']': tokenType = CHTLTokenType::RIGHT_BRACKET; break;
        case '(': tokenType = CHTLTokenType::LEFT_PAREN; break;
        case ')': tokenType = CHTLTokenType::RIGHT_PAREN; break;
        case ';': tokenType = CHTLTokenType::SEMICOLON; break;
        case ',': tokenType = CHTLTokenType::COMMA; break;
        default: break;
    }
    
    CHTLToken token(tokenType, std::string(1, currentChar), currentLine, currentColumn, currentPosition);
    advance();
    return token;
}

CHTLToken CHTLLexer::handleOperator() {
    CHTLTokenType tokenType = CHTLTokenType::UNKNOWN;
    
    switch (currentChar) {
        case ':': tokenType = CHTLTokenType::COLON; break;
        case '=': tokenType = CHTLTokenType::EQUALS; break;
        case '.': tokenType = CHTLTokenType::DOT; break;
        case '/': tokenType = CHTLTokenType::SLASH; break;
        case '*': tokenType = CHTLTokenType::ASTERISK; break;
        default: break;
    }
    
    CHTLToken token(tokenType, std::string(1, currentChar), currentLine, currentColumn, currentPosition);
    advance();
    return token;
}

CHTLToken CHTLLexer::handleSpecialSymbol() {
    CHTLTokenType tokenType = CHTLTokenType::UNKNOWN;
    
    switch (currentChar) {
        case '&': tokenType = CHTLTokenType::AMPERSAND; break;
        case '$': tokenType = CHTLTokenType::DOLLAR_SIGN; break;
        case '#': tokenType = CHTLTokenType::HASH_SIGN; break;
        default: break;
    }
    
    CHTLToken token(tokenType, std::string(1, currentChar), currentLine, currentColumn, currentPosition);
    advance();
    return token;
}

// 辅助方法实现
bool CHTLLexer::isLetter(char c) const {
    return std::isalpha(c) != 0;
}

bool CHTLLexer::isDigit(char c) const {
    return std::isdigit(c) != 0;
}

bool CHTLLexer::isHexDigit(char c) const {
    return std::isxdigit(c) != 0;
}

bool CHTLLexer::isWhitespace(char c) const {
    return std::isspace(c) != 0 && c != '\n' && c != '\r';
}

bool CHTLLexer::isNewline(char c) const {
    return c == '\n' || c == '\r';
}

bool CHTLLexer::isPunctuation(char c) const {
    return c == '{' || c == '}' || c == '[' || c == ']' || c == '(' || c == ')' || c == ';' || c == ',';
}

bool CHTLLexer::isOperator(char c) const {
    return c == ':' || c == '=' || c == '.' || c == '/' || c == '*';
}

bool CHTLLexer::isSpecialSymbol(char c) const {
    return c == '&' || c == '$' || c == '#';
}

bool CHTLLexer::isKeyword(const std::string& text) const {
    // 这里需要根据CHTL语法文档实现完整的关键字识别
    // 暂时返回false，后续完善
    return false;
}

bool CHTLLexer::isTypeIdentifier(const std::string& text) const {
    // 这里需要根据CHTL语法文档实现完整的类型标识符识别
    // 暂时返回false，后续完善
    return false;
}

bool CHTLLexer::isOperatorKeyword(const std::string& text) const {
    // 这里需要根据CHTL语法文档实现完整的操作符关键字识别
    // 暂时返回false，后续完善
    return false;
}

void CHTLLexer::reportError(const std::string& message) {
    std::cerr << "CHTL Lexer Error at line " << currentLine 
              << ", column " << currentColumn << ": " << message << std::endl;
}

CHTLToken CHTLLexer::createErrorToken(const std::string& message) {
    return CHTLToken(CHTLTokenType::UNKNOWN, message, currentLine, currentColumn, currentPosition);
}

void CHTLLexer::updatePosition() {
    currentPosition++;
    if (isNewline(currentChar)) {
        currentLine++;
        currentColumn = 1;
    } else {
        currentColumn++;
    }
}

void CHTLLexer::resetPosition() {
    currentLine = 1;
    currentColumn = 0;
    currentPosition = 0;
}

void CHTLLexer::addToBuffer(const CHTLToken& token) {
    tokenBuffer.push_back(token);
}

CHTLToken CHTLLexer::getFromBuffer() {
    if (currentTokenIndex < tokenBuffer.size()) {
        return tokenBuffer[currentTokenIndex++];
    }
    return CHTLToken(CHTLTokenType::UNKNOWN, "", 0, 0, 0);
}

void CHTLLexer::clearBuffer() {
    tokenBuffer.clear();
    currentTokenIndex = 0;
}

bool CHTLLexer::hasBufferedTokens() const {
    return currentTokenIndex < tokenBuffer.size();
}

// 其他方法的占位符实现
CHTLToken CHTLLexer::handleCSSSelector() { return CHTLToken(); }
CHTLToken CHTLLexer::handlePseudoClass() { return CHTLToken(); }
CHTLToken CHTLLexer::handlePseudoElement() { return CHTLToken(); }
CHTLToken CHTLLexer::handleConfigKey() { return CHTLToken(); }
CHTLToken CHTLLexer::handleConfigValue() { return CHTLToken(); }
CHTLToken CHTLLexer::handleImportPath() { return CHTLToken(); }
CHTLToken CHTLLexer::handleModuleName() { return CHTLToken(); }
CHTLToken CHTLLexer::handleNamespaceName() { return CHTLToken(); }
CHTLToken CHTLLexer::handleTemplateName() { return CHTLToken(); }
CHTLToken CHTLLexer::handleTemplateParameter() { return CHTLToken(); }
CHTLToken CHTLLexer::handleCustomName() { return CHTLToken(); }
CHTLToken CHTLLexer::handleCustomParameter() { return CHTLToken(); }
CHTLToken CHTLLexer::handleOriginType() { return CHTLToken(); }
CHTLToken CHTLLexer::handleOriginName() { return CHTLToken(); }
CHTLToken CHTLLexer::handleStyleGroupName() { return CHTLToken(); }
CHTLToken CHTLLexer::handleStyleProperty() { return CHTLToken(); }
CHTLToken CHTLLexer::handleStyleValue() { return CHTLToken(); }
CHTLToken CHTLLexer::handleElementName() { return CHTLToken(); }
CHTLToken CHTLLexer::handleElementAttribute() { return CHTLToken(); }
CHTLToken CHTLLexer::handleElementAttributeValue() { return CHTLToken(); }

} // namespace CHTL