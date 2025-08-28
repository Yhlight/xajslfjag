#include "CHTLLexer.h"
#include <sstream>
#include <functional>
#include <cctype>

namespace CHTL {

CHTLLexer::CHTLLexer(std::shared_ptr<CHTLGlobalMap> globalMap) 
    : globalMap(globalMap ? globalMap : std::make_shared<CHTLGlobalMap>())
    , currentPosition(0)
    , currentLine(1)
    , currentColumn(1) {
}

void CHTLLexer::setInput(const std::string& input) {
    this->input = input;
    reset();
}

void CHTLLexer::setInput(std::istream& inputStream) {
    std::ostringstream oss;
    oss << inputStream.rdbuf();
    setInput(oss.str());
}

void CHTLLexer::setFileName(const std::string& fileName) {
    this->fileName = fileName;
}

void CHTLLexer::reset() {
    currentPosition = 0;
    currentLine = 1;
    currentColumn = 1;
    errors.clear();
}

CHTLTokenList CHTLLexer::tokenize() {
    reset();
    CHTLTokenList tokens;
    
    while (!isAtEnd()) {
        auto token = nextToken();
        if (token) {
            tokens.push_back(token);
            if (token->type == CHTLTokenType::END_OF_FILE) {
                break;
            }
        }
    }
    
    // 确保最后有EOF token
    if (tokens.empty() || tokens.back()->type != CHTLTokenType::END_OF_FILE) {
        tokens.push_back(createToken(CHTLTokenType::END_OF_FILE, "", currentLine, currentColumn, currentPosition));
    }
    
    return tokens;
}

CHTLTokenPtr CHTLLexer::nextToken() {
    // 跳过空白符但保留换行符
    while (!isAtEnd() && isWhitespace(currentChar()) && !isNewline(currentChar())) {
        advance();
    }
    
    if (isAtEnd()) {
        return createToken(CHTLTokenType::END_OF_FILE, "", currentLine, currentColumn, currentPosition);
    }
    
    size_t startLine = currentLine;
    size_t startColumn = currentColumn;
    size_t startPosition = currentPosition;
    
    char ch = currentChar();
    
    // 处理换行符
    if (isNewline(ch)) {
        advance();
        return createToken(CHTLTokenType::NEWLINE, "\\n", startLine, startColumn, startPosition);
    }
    
    // 处理注释
    if (ch == '/' && peekChar() == '/') {
        return readSingleLineComment();
    }
    
    if (ch == '/' && peekChar() == '*') {
        return readMultiLineComment();
    }
    
    if (ch == '-' && peekChar() == '-') {
        return readGeneratorComment();
    }
    
    // 处理字符串字面量
    if (ch == '"' || ch == '\'') {
        return readStringLiteral();
    }
    
    // 处理数字
    if (isDigit(ch)) {
        return readNumber();
    }
    
    // 处理选择器引用 {{...}}
    if (ch == '{' && peekChar() == '{') {
        return readSelectorReference();
    }
    
    // 处理标记 [...]
    if (ch == '[') {
        return readMarker();
    }
    
    // 处理类型标记 @...
    if (ch == '@') {
        return readTypeMarker();
    }
    
    // 处理单字符操作符
    switch (ch) {
        case '{':
            advance();
            return createToken(CHTLTokenType::LBRACE, "{", startLine, startColumn, startPosition);
        case '}':
            advance();
            return createToken(CHTLTokenType::RBRACE, "}", startLine, startColumn, startPosition);
        case '(':
            advance();
            return createToken(CHTLTokenType::LPAREN, "(", startLine, startColumn, startPosition);
        case ')':
            advance();
            return createToken(CHTLTokenType::RPAREN, ")", startLine, startColumn, startPosition);
        case '[':
            advance();
            return createToken(CHTLTokenType::LBRACKET, "[", startLine, startColumn, startPosition);
        case ']':
            advance();
            return createToken(CHTLTokenType::RBRACKET, "]", startLine, startColumn, startPosition);
        case ';':
            advance();
            return createToken(CHTLTokenType::SEMICOLON, ";", startLine, startColumn, startPosition);
        case ',':
            advance();
            return createToken(CHTLTokenType::COMMA, ",", startLine, startColumn, startPosition);
        case ':':
            advance();
            return createToken(CHTLTokenType::COLON, ":", startLine, startColumn, startPosition);
        case '=':
            advance();
            return createToken(CHTLTokenType::EQUALS, "=", startLine, startColumn, startPosition);
        case '.':
            advance();
            return createToken(CHTLTokenType::DOT, ".", startLine, startColumn, startPosition);
    }
    
    // 处理复合操作符
    if (ch == '-' && peekChar() == '>') {
        advance();
        advance();
        return createToken(CHTLTokenType::ARROW, "->", startLine, startColumn, startPosition);
    }
    
    if (ch == '&') {
        advance();
        if (currentChar() == '-' && peekChar() == '>') {
            advance();
            advance();
            return createToken(CHTLTokenType::AMPERSAND_ARROW, "&->", startLine, startColumn, startPosition);
        }
        return createToken(CHTLTokenType::AMPERSAND, "&", startLine, startColumn, startPosition);
    }
    
    // 处理标识符和关键字
    if (isAlpha(ch) || ch == '_') {
        return readIdentifierOrKeyword();
    }
    
    // 处理无修饰字面量（任何非特殊字符的序列）
    if (!isWhitespace(ch)) {
        return readUnquotedLiteral();
    }
    
    // 未知字符，报告错误
    std::string errorChar(1, ch);
    reportError("Unexpected character: " + errorChar);
    advance();
    return createToken(CHTLTokenType::ERROR_TOKEN, errorChar, startLine, startColumn, startPosition);
}

char CHTLLexer::currentChar() const {
    if (isAtEnd()) return '\0';
    return input[currentPosition];
}

char CHTLLexer::peekChar(size_t offset) const {
    size_t pos = currentPosition + offset;
    if (pos >= input.length()) return '\0';
    return input[pos];
}

void CHTLLexer::advance() {
    if (isAtEnd()) return;
    
    if (currentChar() == '\n') {
        currentLine++;
        currentColumn = 1;
    } else {
        currentColumn++;
    }
    currentPosition++;
}

bool CHTLLexer::isAtEnd() const {
    return currentPosition >= input.length();
}

CHTLTokenPtr CHTLLexer::readStringLiteral() {
    size_t startLine = currentLine;
    size_t startColumn = currentColumn;
    size_t startPosition = currentPosition;
    
    char quote = currentChar();
    advance(); // 跳过开始引号
    
    std::string value;
    bool escaped = false;
    
    while (!isAtEnd() && (currentChar() != quote || escaped)) {
        if (escaped) {
            switch (currentChar()) {
                case 'n': value += '\n'; break;
                case 't': value += '\t'; break;
                case 'r': value += '\r'; break;
                case '\\': value += '\\'; break;
                case '"': value += '"'; break;
                case '\'': value += '\''; break;
                default: 
                    value += '\\';
                    value += currentChar();
                    break;
            }
            escaped = false;
        } else if (currentChar() == '\\') {
            escaped = true;
        } else {
            value += currentChar();
        }
        advance();
    }
    
    if (isAtEnd()) {
        reportError("Unterminated string literal");
        return createToken(CHTLTokenType::ERROR_TOKEN, value, startLine, startColumn, startPosition);
    }
    
    advance(); // 跳过结束引号
    return createToken(CHTLTokenType::STRING_LITERAL, value, startLine, startColumn, startPosition);
}

CHTLTokenPtr CHTLLexer::readNumber() {
    size_t startLine = currentLine;
    size_t startColumn = currentColumn;
    size_t startPosition = currentPosition;
    
    std::string value;
    
    while (!isAtEnd() && isDigit(currentChar())) {
        value += currentChar();
        advance();
    }
    
    // 处理小数点
    if (!isAtEnd() && currentChar() == '.' && isDigit(peekChar())) {
        value += currentChar();
        advance();
        
        while (!isAtEnd() && isDigit(currentChar())) {
            value += currentChar();
            advance();
        }
    }
    
    return createToken(CHTLTokenType::NUMBER_LITERAL, value, startLine, startColumn, startPosition);
}

CHTLTokenPtr CHTLLexer::readIdentifierOrKeyword() {
    size_t startLine = currentLine;
    size_t startColumn = currentColumn;
    size_t startPosition = currentPosition;
    
    std::string value;
    
    while (!isAtEnd() && (isAlphaNumeric(currentChar()) || currentChar() == '_' || currentChar() == '-')) {
        value += currentChar();
        advance();
    }
    
    // 检查复合关键字（如 "at top", "at bottom"）
    if (value == "at" && !isAtEnd() && isWhitespace(currentChar())) {
        size_t savedPos = currentPosition;
        size_t savedLine = currentLine;
        size_t savedColumn = currentColumn;
        
        // 跳过空白符
        while (!isAtEnd() && isWhitespace(currentChar()) && !isNewline(currentChar())) {
            advance();
        }
        
        std::string nextWord;
        while (!isAtEnd() && isAlpha(currentChar())) {
            nextWord += currentChar();
            advance();
        }
        
        std::string compound = value + " " + nextWord;
        if (globalMap->getKeywordMap().isKeyword(compound)) {
            return createToken(globalMap->getKeywordMap().getKeywordType(compound), compound, startLine, startColumn, startPosition);
        } else {
            // 恢复位置
            currentPosition = savedPos;
            currentLine = savedLine;
            currentColumn = savedColumn;
        }
    }
    
    // 检查是否为关键字
    if (globalMap->getKeywordMap().isKeyword(value)) {
        return createToken(globalMap->getKeywordMap().getKeywordType(value), value, startLine, startColumn, startPosition);
    }
    
    return createToken(CHTLTokenType::IDENTIFIER, value, startLine, startColumn, startPosition);
}

CHTLTokenPtr CHTLLexer::readSingleLineComment() {
    size_t startLine = currentLine;
    size_t startColumn = currentColumn;
    size_t startPosition = currentPosition;
    
    std::string value = "//";
    advance(); // 跳过第一个 '/'
    advance(); // 跳过第二个 '/'
    
    while (!isAtEnd() && !isNewline(currentChar())) {
        value += currentChar();
        advance();
    }
    
    return createToken(CHTLTokenType::SINGLE_LINE_COMMENT, value, startLine, startColumn, startPosition);
}

CHTLTokenPtr CHTLLexer::readMultiLineComment() {
    size_t startLine = currentLine;
    size_t startColumn = currentColumn;
    size_t startPosition = currentPosition;
    
    std::string value = "/*";
    advance(); // 跳过 '/'
    advance(); // 跳过 '*'
    
    while (!isAtEnd()) {
        if (currentChar() == '*' && peekChar() == '/') {
            value += "*/";
            advance();
            advance();
            break;
        }
        value += currentChar();
        advance();
    }
    
    return createToken(CHTLTokenType::MULTI_LINE_COMMENT, value, startLine, startColumn, startPosition);
}

CHTLTokenPtr CHTLLexer::readGeneratorComment() {
    size_t startLine = currentLine;
    size_t startColumn = currentColumn;
    size_t startPosition = currentPosition;
    
    std::string value = "--";
    advance(); // 跳过第一个 '-'
    advance(); // 跳过第二个 '-'
    
    while (!isAtEnd() && !isNewline(currentChar())) {
        value += currentChar();
        advance();
    }
    
    return createToken(CHTLTokenType::GENERATOR_COMMENT, value, startLine, startColumn, startPosition);
}

CHTLTokenPtr CHTLLexer::readSelectorReference() {
    size_t startLine = currentLine;
    size_t startColumn = currentColumn;
    size_t startPosition = currentPosition;
    
    std::string value = "{{";
    advance(); // 跳过第一个 '{'
    advance(); // 跳过第二个 '{'
    
    while (!isAtEnd() && !(currentChar() == '}' && peekChar() == '}')) {
        value += currentChar();
        advance();
    }
    
    if (!isAtEnd()) {
        value += "}}";
        advance(); // 跳过第一个 '}'
        advance(); // 跳过第二个 '}'
    }
    
    // 根据内容判断选择器类型
    std::string selector = value.substr(2, value.length() - 4); // 去掉{{}}
    
    if (selector == "&") {
        return createToken(CHTLTokenType::REF_AMPERSAND, value, startLine, startColumn, startPosition);
    } else if (!selector.empty() && selector[0] == '.') {
        return createToken(CHTLTokenType::CLASS_REF, value, startLine, startColumn, startPosition);
    } else if (!selector.empty() && selector[0] == '#') {
        return createToken(CHTLTokenType::ID_REF, value, startLine, startColumn, startPosition);
    } else {
        return createToken(CHTLTokenType::SELECTOR_REF, value, startLine, startColumn, startPosition);
    }
}

CHTLTokenPtr CHTLLexer::readUnquotedLiteral() {
    size_t startLine = currentLine;
    size_t startColumn = currentColumn;
    size_t startPosition = currentPosition;
    
    std::string value;
    
    while (!isAtEnd() && !isWhitespace(currentChar()) && 
           currentChar() != '{' && currentChar() != '}' &&
           currentChar() != '(' && currentChar() != ')' &&
           currentChar() != '[' && currentChar() != ']' &&
           currentChar() != ';' && currentChar() != ',' &&
           currentChar() != ':' && currentChar() != '=' &&
           currentChar() != '@' && currentChar() != '/') {
        value += currentChar();
        advance();
    }
    
    return createToken(CHTLTokenType::UNQUOTED_LITERAL, value, startLine, startColumn, startPosition);
}

CHTLTokenPtr CHTLLexer::readMarker() {
    size_t startLine = currentLine;
    size_t startColumn = currentColumn;
    size_t startPosition = currentPosition;
    
    std::string value = "[";
    advance(); // 跳过 '['
    
    while (!isAtEnd() && currentChar() != ']') {
        value += currentChar();
        advance();
    }
    
    if (!isAtEnd()) {
        value += ']';
        advance(); // 跳过 ']'
    }
    
    // 检查是否为已知标记
    if (globalMap->getKeywordMap().isKeyword(value)) {
        return createToken(globalMap->getKeywordMap().getKeywordType(value), value, startLine, startColumn, startPosition);
    }
    
    return createToken(CHTLTokenType::LBRACKET, "[", startLine, startColumn, startPosition);
}

CHTLTokenPtr CHTLLexer::readTypeMarker() {
    size_t startLine = currentLine;
    size_t startColumn = currentColumn;
    size_t startPosition = currentPosition;
    
    std::string value = "@";
    advance(); // 跳过 '@'
    
    while (!isAtEnd() && (isAlphaNumeric(currentChar()) || currentChar() == '_')) {
        value += currentChar();
        advance();
    }
    
    // 检查是否为已知类型标记
    if (globalMap->getKeywordMap().isKeyword(value)) {
        return createToken(globalMap->getKeywordMap().getKeywordType(value), value, startLine, startColumn, startPosition);
    }
    
    // 自定义类型标记，动态创建
    return createToken(CHTLTokenType::AT_STYLE, value, startLine, startColumn, startPosition); // 默认类型
}

bool CHTLLexer::isAlpha(char ch) const {
    return std::isalpha(ch) || ch == '_';
}

bool CHTLLexer::isDigit(char ch) const {
    return std::isdigit(ch);
}

bool CHTLLexer::isAlphaNumeric(char ch) const {
    return isAlpha(ch) || isDigit(ch);
}

bool CHTLLexer::isWhitespace(char ch) const {
    return ch == ' ' || ch == '\t' || ch == '\r';
}

bool CHTLLexer::isNewline(char ch) const {
    return ch == '\n';
}

void CHTLLexer::reportError(const std::string& message) {
    LexerError error;
    error.message = message;
    error.line = currentLine;
    error.column = currentColumn;
    error.position = currentPosition;
    errors.push_back(error);
}

CHTLTokenPtr CHTLLexer::createToken(CHTLTokenType type, const std::string& text, size_t startLine, size_t startColumn, size_t startPosition) {
    return std::make_shared<CHTLToken>(type, text, startLine, startColumn, startPosition);
}

} // namespace CHTL