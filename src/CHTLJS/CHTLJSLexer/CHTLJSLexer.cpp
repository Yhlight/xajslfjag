#include "CHTLJSLexer.h"
#include <sstream>
#include <cctype>

namespace CHTLJS {

CHTLJSLexer::CHTLJSLexer(std::shared_ptr<CHTLJSGlobalMap> globalMap) 
    : globalMap(globalMap ? globalMap : std::make_shared<CHTLJSGlobalMap>())
    , currentPosition(0)
    , currentLine(1)
    , currentColumn(1) {
}

void CHTLJSLexer::setInput(const std::string& input) {
    this->input = input;
    reset();
}

void CHTLJSLexer::setInput(std::istream& inputStream) {
    std::ostringstream oss;
    oss << inputStream.rdbuf();
    setInput(oss.str());
}

void CHTLJSLexer::setFileName(const std::string& fileName) {
    this->fileName = fileName;
}

void CHTLJSLexer::reset() {
    currentPosition = 0;
    currentLine = 1;
    currentColumn = 1;
    errors.clear();
}

CHTLJSTokenList CHTLJSLexer::tokenize() {
    reset();
    CHTLJSTokenList tokens;
    
    while (!isAtEnd()) {
        auto token = nextToken();
        if (token) {
            tokens.push_back(token);
            if (token->type == CHTLJSTokenType::END_OF_FILE) {
                break;
            }
        }
    }
    
    // 确保最后有EOF token
    if (tokens.empty() || tokens.back()->type != CHTLJSTokenType::END_OF_FILE) {
        tokens.push_back(createToken(CHTLJSTokenType::END_OF_FILE, "", currentLine, currentColumn, currentPosition));
    }
    
    return tokens;
}

CHTLJSTokenPtr CHTLJSLexer::nextToken() {
    // 跳过空白符但保留换行符
    while (!isAtEnd() && isWhitespace(currentChar()) && !isNewline(currentChar())) {
        advance();
    }
    
    if (isAtEnd()) {
        return createToken(CHTLJSTokenType::END_OF_FILE, "", currentLine, currentColumn, currentPosition);
    }
    
    size_t startLine = currentLine;
    size_t startColumn = currentColumn;
    size_t startPosition = currentPosition;
    
    char ch = currentChar();
    
    // 处理换行符
    if (isNewline(ch)) {
        advance();
        return createToken(CHTLJSTokenType::NEWLINE, "\\n", startLine, startColumn, startPosition);
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
    
    // 处理模板字面量
    if (ch == '`') {
        return readTemplateLiteral();
    }
    
    // 处理数字
    if (isDigit(ch)) {
        return readNumber();
    }
    
    // 处理选择器引用 {{...}}
    if (ch == '{' && peekChar() == '{') {
        return readSelectorReference();
    }
    
    // 处理单字符操作符
    switch (ch) {
        case '{':
            advance();
            return createToken(CHTLJSTokenType::LBRACE, "{", startLine, startColumn, startPosition);
        case '}':
            advance();
            return createToken(CHTLJSTokenType::RBRACE, "}", startLine, startColumn, startPosition);
        case '(':
            advance();
            return createToken(CHTLJSTokenType::LPAREN, "(", startLine, startColumn, startPosition);
        case ')':
            advance();
            return createToken(CHTLJSTokenType::RPAREN, ")", startLine, startColumn, startPosition);
        case '[':
            advance();
            return createToken(CHTLJSTokenType::LBRACKET, "[", startLine, startColumn, startPosition);
        case ']':
            advance();
            return createToken(CHTLJSTokenType::RBRACKET, "]", startLine, startColumn, startPosition);
        case ';':
            advance();
            return createToken(CHTLJSTokenType::SEMICOLON, ";", startLine, startColumn, startPosition);
        case ',':
            advance();
            return createToken(CHTLJSTokenType::COMMA, ",", startLine, startColumn, startPosition);
        case ':':
            advance();
            return createToken(CHTLJSTokenType::COLON, ":", startLine, startColumn, startPosition);
        case '=':
            advance();
            return createToken(CHTLJSTokenType::EQUALS, "=", startLine, startColumn, startPosition);
        case '.':
            advance();
            return createToken(CHTLJSTokenType::DOT, ".", startLine, startColumn, startPosition);
    }
    
    // 处理复合操作符
    if (ch == '-' && peekChar() == '>') {
        advance();
        advance();
        return createToken(CHTLJSTokenType::ARROW, "->", startLine, startColumn, startPosition);
    }
    
    if (ch == '&') {
        advance();
        if (currentChar() == '-' && peekChar() == '>') {
            advance();
            advance();
            return createToken(CHTLJSTokenType::AMPERSAND_ARROW, "&->", startLine, startColumn, startPosition);
        }
        return createToken(CHTLJSTokenType::AMPERSAND, "&", startLine, startColumn, startPosition);
    }
    
    // 处理${}模板字面量表达式
    if (ch == '$' && peekChar() == '{') {
        advance();
        advance();
        return createToken(CHTLJSTokenType::DOLLAR_BRACE, "${", startLine, startColumn, startPosition);
    }
    
    // 处理标识符和关键字
    if (isAlpha(ch) || ch == '_' || ch == '$') {
        return readIdentifierOrKeyword();
    }
    
    // 处理正则表达式字面量
    if (ch == '/') {
        return readRegexLiteral();
    }
    
    // 处理无修饰字面量（CHTL JS特有）
    if (!isWhitespace(ch)) {
        return readUnquotedLiteral();
    }
    
    // 未知字符，报告错误
    std::string errorChar(1, ch);
    reportError("Unexpected character: " + errorChar);
    advance();
    return createToken(CHTLJSTokenType::ERROR_TOKEN, errorChar, startLine, startColumn, startPosition);
}

char CHTLJSLexer::currentChar() const {
    if (isAtEnd()) return '\0';
    return input[currentPosition];
}

char CHTLJSLexer::peekChar(size_t offset) const {
    size_t pos = currentPosition + offset;
    if (pos >= input.length()) return '\0';
    return input[pos];
}

void CHTLJSLexer::advance() {
    if (isAtEnd()) return;
    
    if (currentChar() == '\n') {
        currentLine++;
        currentColumn = 1;
    } else {
        currentColumn++;
    }
    currentPosition++;
}

bool CHTLJSLexer::isAtEnd() const {
    return currentPosition >= input.length();
}

CHTLJSTokenPtr CHTLJSLexer::readStringLiteral() {
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
                case 'b': value += '\b'; break;
                case 'f': value += '\f'; break;
                case 'v': value += '\v'; break;
                case '0': value += '\0'; break;
                case '\\': value += '\\'; break;
                case '"': value += '"'; break;
                case '\'': value += '\''; break;
                case 'x': // 十六进制转义序列
                    advance();
                    if (!isAtEnd() && isHexDigit(currentChar())) {
                        char hex1 = currentChar();
                        advance();
                        if (!isAtEnd() && isHexDigit(currentChar())) {
                            char hex2 = currentChar();
                            // 简化处理，实际应该转换十六进制
                            value += "\\x";
                            value += hex1;
                            value += hex2;
                        } else {
                            value += "\\x";
                            value += hex1;
                            currentPosition--; // 回退
                        }
                    } else {
                        value += "\\x";
                        currentPosition--; // 回退
                    }
                    break;
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
        return createToken(CHTLJSTokenType::ERROR_TOKEN, value, startLine, startColumn, startPosition);
    }
    
    advance(); // 跳过结束引号
    return createToken(CHTLJSTokenType::STRING_LITERAL, value, startLine, startColumn, startPosition);
}

CHTLJSTokenPtr CHTLJSLexer::readTemplateLiteral() {
    size_t startLine = currentLine;
    size_t startColumn = currentColumn;
    size_t startPosition = currentPosition;
    
    advance(); // 跳过开始的`
    
    std::string value;
    
    while (!isAtEnd() && currentChar() != '`') {
        if (currentChar() == '\\') {
            value += currentChar();
            advance();
            if (!isAtEnd()) {
                value += currentChar();
                advance();
            }
        } else {
            value += currentChar();
            advance();
        }
    }
    
    if (isAtEnd()) {
        reportError("Unterminated template literal");
        return createToken(CHTLJSTokenType::ERROR_TOKEN, value, startLine, startColumn, startPosition);
    }
    
    advance(); // 跳过结束的`
    return createToken(CHTLJSTokenType::TEMPLATE_LITERAL_START, value, startLine, startColumn, startPosition);
}

CHTLJSTokenPtr CHTLJSLexer::readNumber() {
    size_t startLine = currentLine;
    size_t startColumn = currentColumn;
    size_t startPosition = currentPosition;
    
    std::string value;
    
    // 处理十六进制数字
    if (currentChar() == '0' && (peekChar() == 'x' || peekChar() == 'X')) {
        value += currentChar(); advance();
        value += currentChar(); advance();
        
        while (!isAtEnd() && isHexDigit(currentChar())) {
            value += currentChar();
            advance();
        }
        
        return createToken(CHTLJSTokenType::NUMBER_LITERAL, value, startLine, startColumn, startPosition);
    }
    
    // 处理八进制数字
    if (currentChar() == '0' && (peekChar() == 'o' || peekChar() == 'O')) {
        value += currentChar(); advance();
        value += currentChar(); advance();
        
        while (!isAtEnd() && currentChar() >= '0' && currentChar() <= '7') {
            value += currentChar();
            advance();
        }
        
        return createToken(CHTLJSTokenType::NUMBER_LITERAL, value, startLine, startColumn, startPosition);
    }
    
    // 处理二进制数字
    if (currentChar() == '0' && (peekChar() == 'b' || peekChar() == 'B')) {
        value += currentChar(); advance();
        value += currentChar(); advance();
        
        while (!isAtEnd() && (currentChar() == '0' || currentChar() == '1')) {
            value += currentChar();
            advance();
        }
        
        return createToken(CHTLJSTokenType::NUMBER_LITERAL, value, startLine, startColumn, startPosition);
    }
    
    // 处理常规数字
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
    
    // 处理科学计数法
    if (!isAtEnd() && (currentChar() == 'e' || currentChar() == 'E')) {
        value += currentChar();
        advance();
        
        if (!isAtEnd() && (currentChar() == '+' || currentChar() == '-')) {
            value += currentChar();
            advance();
        }
        
        while (!isAtEnd() && isDigit(currentChar())) {
            value += currentChar();
            advance();
        }
    }
    
    return createToken(CHTLJSTokenType::NUMBER_LITERAL, value, startLine, startColumn, startPosition);
}

CHTLJSTokenPtr CHTLJSLexer::readIdentifierOrKeyword() {
    size_t startLine = currentLine;
    size_t startColumn = currentColumn;
    size_t startPosition = currentPosition;
    
    std::string value;
    
    while (!isAtEnd() && (isAlphaNumeric(currentChar()) || currentChar() == '_' || currentChar() == '$')) {
        value += currentChar();
        advance();
    }
    
    // 检查是否为关键字
    if (globalMap->getKeywordMap().isKeyword(value)) {
        return createToken(globalMap->getKeywordMap().getKeywordType(value), value, startLine, startColumn, startPosition);
    }
    
    return createToken(CHTLJSTokenType::IDENTIFIER, value, startLine, startColumn, startPosition);
}

CHTLJSTokenPtr CHTLJSLexer::readSingleLineComment() {
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
    
    return createToken(CHTLJSTokenType::SINGLE_LINE_COMMENT, value, startLine, startColumn, startPosition);
}

CHTLJSTokenPtr CHTLJSLexer::readMultiLineComment() {
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
    
    return createToken(CHTLJSTokenType::MULTI_LINE_COMMENT, value, startLine, startColumn, startPosition);
}

CHTLJSTokenPtr CHTLJSLexer::readGeneratorComment() {
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
    
    return createToken(CHTLJSTokenType::GENERATOR_COMMENT, value, startLine, startColumn, startPosition);
}

CHTLJSTokenPtr CHTLJSLexer::readSelectorReference() {
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
        return createToken(CHTLJSTokenType::REF_AMPERSAND, value, startLine, startColumn, startPosition);
    } else if (!selector.empty() && selector[0] == '.') {
        return createToken(CHTLJSTokenType::CLASS_REF, value, startLine, startColumn, startPosition);
    } else if (!selector.empty() && selector[0] == '#') {
        return createToken(CHTLJSTokenType::ID_REF, value, startLine, startColumn, startPosition);
    } else {
        return createToken(CHTLJSTokenType::SELECTOR_REF, value, startLine, startColumn, startPosition);
    }
}

CHTLJSTokenPtr CHTLJSLexer::readUnquotedLiteral() {
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
           currentChar() != '.' && currentChar() != '/') {
        value += currentChar();
        advance();
    }
    
    // 检查是否应该作为无修饰字面量处理
    if (shouldTreatAsUnquotedLiteral(value)) {
        return createToken(CHTLJSTokenType::UNQUOTED_LITERAL, value, startLine, startColumn, startPosition);
    }
    
    // 否则作为标识符处理
    return createToken(CHTLJSTokenType::IDENTIFIER, value, startLine, startColumn, startPosition);
}

CHTLJSTokenPtr CHTLJSLexer::readRegexLiteral() {
    size_t startLine = currentLine;
    size_t startColumn = currentColumn;
    size_t startPosition = currentPosition;
    
    std::string value = "/";
    advance(); // 跳过 '/'
    
    bool escaped = false;
    while (!isAtEnd() && (currentChar() != '/' || escaped)) {
        if (escaped) {
            escaped = false;
        } else if (currentChar() == '\\') {
            escaped = true;
        }
        value += currentChar();
        advance();
    }
    
    if (!isAtEnd()) {
        value += '/';
        advance(); // 跳过结束的 '/'
        
        // 读取正则表达式标志
        while (!isAtEnd() && isAlpha(currentChar())) {
            value += currentChar();
            advance();
        }
    }
    
    return createToken(CHTLJSTokenType::STRING_LITERAL, value, startLine, startColumn, startPosition);
}

bool CHTLJSLexer::isAlpha(char ch) const {
    return std::isalpha(ch) || ch == '_' || ch == '$';
}

bool CHTLJSLexer::isDigit(char ch) const {
    return std::isdigit(ch);
}

bool CHTLJSLexer::isAlphaNumeric(char ch) const {
    return isAlpha(ch) || isDigit(ch);
}

bool CHTLJSLexer::isWhitespace(char ch) const {
    return ch == ' ' || ch == '\t' || ch == '\r';
}

bool CHTLJSLexer::isNewline(char ch) const {
    return ch == '\n';
}

bool CHTLJSLexer::isHexDigit(char ch) const {
    return isDigit(ch) || (ch >= 'a' && ch <= 'f') || (ch >= 'A' && ch <= 'F');
}

bool CHTLJSLexer::shouldTreatAsUnquotedLiteral(const std::string& text) const {
    // CHTL JS的无修饰字面量规则
    // 如果文本不是关键字且包含特定模式，则作为无修饰字面量
    if (globalMap->getKeywordMap().isKeyword(text)) {
        return false;
    }
    
    // CSS单位、颜色值等应该作为无修饰字面量
    if (text.find("px") != std::string::npos ||
        text.find("em") != std::string::npos ||
        text.find("rem") != std::string::npos ||
        text.find("%") != std::string::npos ||
        text.find("#") == 0) {
        return true;
    }
    
    return false;
}

void CHTLJSLexer::reportError(const std::string& message) {
    LexerError error;
    error.message = message;
    error.line = currentLine;
    error.column = currentColumn;
    error.position = currentPosition;
    errors.push_back(error);
}

CHTLJSTokenPtr CHTLJSLexer::createToken(CHTLJSTokenType type, const std::string& text, size_t startLine, size_t startColumn, size_t startPosition) {
    return std::make_shared<CHTLJSToken>(type, text, startLine, startColumn, startPosition);
}

} // namespace CHTLJS