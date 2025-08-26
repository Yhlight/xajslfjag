#include "CHTLLexer.h"
#include <cctype>
#include <algorithm>

namespace CHTL {

CHTLLexer::CHTLLexer(const std::string& sourceCode) 
    : source(sourceCode), position(0), line(1), column(1), start(0), currentState(CHTLLexerState::Normal) {
}

void CHTLLexer::reset(const std::string& newSource) {
    if (!newSource.empty()) {
        source = newSource;
    }
    position = 0;
    line = 1;
    column = 1;
    start = 0;
    currentState = CHTLLexerState::Normal;
    errors.clear();
}

std::vector<CHTLToken> CHTLLexer::tokenize() {
    std::vector<CHTLToken> tokens;
    
    while (!isAtEnd()) {
        start = position;
        CHTLToken token = nextToken();
        
        if (token.type != CHTLTokenType::INVALID) {
            tokens.push_back(token);
        }
    }
    
    // 添加EOF token
    tokens.push_back(makeToken(CHTLTokenType::EOF_TOKEN, ""));
    
    return tokens;
}

CHTLToken CHTLLexer::nextToken() {
    skipWhitespace();
    
    if (isAtEnd()) {
        return makeToken(CHTLTokenType::EOF_TOKEN, "");
    }
    
    start = position;
    return scanToken();
}

CHTLToken CHTLLexer::scanToken() {
    char c = advance();
    
    // 字符串字面量
    if (c == '"' || c == '\'') {
        return scanStringLiteral(c);
    }
    
    // 数字
    if (isDigit(c)) {
        return scanNumber();
    }
    
    // 注释处理
    if (c == '/') {
        if (match('/')) {
            return scanLineComment();
        } else if (match('*')) {
            return scanBlockComment();
        } else {
            return makeToken(CHTLTokenType::SLASH, "/");
        }
    }
    
    // 生成器注释 --
    if (c == '-' && match('-')) {
        return scanGeneratorComment();
    }
    
    // 方括号关键字 [Template], [Custom] 等
    if (c == '[') {
        size_t savedPos = position;
        CHTLToken bracketToken = scanBracketKeyword();
        if (bracketToken.type != CHTLTokenType::INVALID) {
            return bracketToken;
        }
        // 如果不是方括号关键字，回退并返回普通左括号
        position = savedPos;
        return makeToken(CHTLTokenType::LBRACKET, "[");
    }
    
    // 类型标识符 @Style, @Element 等
    if (c == '@') {
        return scanTypeIdentifier();
    }
    
    // CSS选择器 .class, #id
    if (c == '.' || c == '#') {
        return scanSelector();
    }
    
    // 增强选择器 {{selector}}
    if (c == '{' && peek() == '{') {
        advance(); // 跳过第二个 {
        return scanEnhancedSelector();
    }
    
    // 箭头操作符 ->
    if (c == '-' && peek() == '>') {
        advance(); // 跳过 >
        return makeToken(CHTLTokenType::ARROW, "->");
    }
    
    // 标识符和关键字
    if (isValidIdentifierStart(c)) {
        position--; // 回退一个字符，让scanIdentifier处理
        return scanIdentifier();
    }
    
    // 单字符token
    switch (c) {
        case '{': return makeToken(CHTLTokenType::LBRACE, "{");
        case '}': return makeToken(CHTLTokenType::RBRACE, "}");
        case '(': return makeToken(CHTLTokenType::LPAREN, "(");
        case ')': return makeToken(CHTLTokenType::RPAREN, ")");
        case ']': return makeToken(CHTLTokenType::RBRACKET, "]");
        case ';': return makeToken(CHTLTokenType::SEMICOLON, ";");
        case ',': return makeToken(CHTLTokenType::COMMA, ",");
        case '*': return makeToken(CHTLTokenType::STAR, "*");
        case '&': return makeToken(CHTLTokenType::AMPERSAND, "&");
        case ':': return scanCEEquivalent(c);
        case '=': return scanCEEquivalent(c);
        case '\n': 
            line++;
            column = 1;
            return makeToken(CHTLTokenType::NEWLINE, "\n");
        default:
            // 尝试无引号字面量
            if (isValidUnquotedChar(c)) {
                position--; // 回退
                return scanUnquotedLiteral();
            }
            
            // 无法识别的字符
            std::string errorChar(1, c);
            addError("Unexpected character: " + errorChar);
            return makeErrorToken("Unexpected character: " + errorChar);
    }
}

CHTLToken CHTLLexer::scanStringLiteral(char quote) {
    std::string value;
    
    while (!isAtEnd() && peek() != quote) {
        if (peek() == '\n') {
            line++;
            column = 1;
        }
        
        char c = advance();
        
        // 处理转义字符
        if (c == '\\' && !isAtEnd()) {
            char escaped = advance();
            switch (escaped) {
                case 'n': value += '\n'; break;
                case 't': value += '\t'; break;
                case 'r': value += '\r'; break;
                case '\\': value += '\\'; break;
                case '"': value += '"'; break;
                case '\'': value += '\''; break;
                default: 
                    value += '\\';
                    value += escaped;
                    break;
            }
        } else {
            value += c;
        }
    }
    
    if (isAtEnd()) {
        addError("Unterminated string literal");
        return makeErrorToken("Unterminated string literal");
    }
    
    // 跳过结束引号
    advance();
    
    return makeToken(CHTLTokenType::STRING_LITERAL, value);
}

CHTLToken CHTLLexer::scanUnquotedLiteral() {
    std::string value;
    
    while (!isAtEnd() && isValidUnquotedChar(peek())) {
        value += advance();
    }
    
    return makeToken(CHTLTokenType::UNQUOTED_LITERAL, value);
}

CHTLToken CHTLLexer::scanNumber() {
    std::string value;
    value += source[position - 1]; // 添加当前数字字符
    
    while (!isAtEnd() && (isDigit(peek()) || peek() == '.')) {
        value += advance();
    }
    
    return makeToken(CHTLTokenType::NUMBER, value);
}

CHTLToken CHTLLexer::scanIdentifier() {
    std::string value;
    
    while (!isAtEnd() && isValidIdentifierChar(peek())) {
        value += advance();
    }
    
    // 检查多词关键字 "at top", "at bottom"
    if (value == "at") {
        skipWhitespace();
        if (position + 3 <= source.length() && source.substr(position, 3) == "top") {
            position += 3;
            return makeToken(CHTLTokenType::AT_TOP, "at top");
        } else if (position + 6 <= source.length() && source.substr(position, 6) == "bottom") {
            position += 6;
            return makeToken(CHTLTokenType::AT_BOTTOM, "at bottom");
        }
    }
    
    // 检查是否为关键字
    CHTLTokenType tokenType = CHTLTokenUtils::stringToTokenType(value);
    return makeToken(tokenType, value);
}

CHTLToken CHTLLexer::scanBracketKeyword() {
    std::string value = "[";
    
    // 扫描到 ] 或者遇到不合法字符
    while (!isAtEnd() && peek() != ']') {
        char c = peek();
        if (isAlpha(c)) {
            value += advance();
        } else {
            // 不是有效的方括号关键字
            return makeToken(CHTLTokenType::INVALID, "");
        }
    }
    
    if (isAtEnd() || peek() != ']') {
        return makeToken(CHTLTokenType::INVALID, "");
    }
    
    value += advance(); // 添加 ]
    
    // 检查是否为已知的方括号关键字
    CHTLTokenType tokenType = CHTLTokenUtils::stringToTokenType(value);
    if (tokenType != CHTLTokenType::IDENTIFIER) {
        return makeToken(tokenType, value);
    }
    
    return makeToken(CHTLTokenType::INVALID, "");
}

CHTLToken CHTLLexer::scanTypeIdentifier() {
    std::string value = "@";
    
    if (isAtEnd() || !isAlpha(peek())) {
        addError("Expected type identifier after @");
        return makeErrorToken("Expected type identifier after @");
    }
    
    while (!isAtEnd() && isValidIdentifierChar(peek())) {
        value += advance();
    }
    
    // 检查是否为已知的类型标识符
    CHTLTokenType tokenType = CHTLTokenUtils::stringToTokenType(value);
    if (tokenType != CHTLTokenType::IDENTIFIER) {
        return makeToken(tokenType, value);
    }
    
    // 未知的类型标识符，仍然返回，但标记为IDENTIFIER
    return makeToken(CHTLTokenType::IDENTIFIER, value);
}

CHTLToken CHTLLexer::scanSelector() {
    char selectorType = source[position - 1]; // . 或 #
    std::string value(1, selectorType);
    
    if (isAtEnd() || (!isAlpha(peek()) && peek() != '_')) {
        // 只有 . 或 # 没有后续标识符，返回DOT或HASH
        return makeToken(selectorType == '.' ? CHTLTokenType::DOT : CHTLTokenType::HASH, value);
    }
    
    while (!isAtEnd() && isValidIdentifierChar(peek())) {
        value += advance();
    }
    
    // 返回相应的选择器类型
    CHTLTokenType tokenType = selectorType == '.' ? CHTLTokenType::CLASS_SELECTOR : CHTLTokenType::ID_SELECTOR;
    return makeToken(tokenType, value);
}

CHTLToken CHTLLexer::scanEnhancedSelector() {
    std::string value = "{{";
    
    while (!isAtEnd() && !(peek() == '}' && position + 1 < source.length() && source[position + 1] == '}')) {
        value += advance();
    }
    
    if (isAtEnd() || peek() != '}') {
        addError("Unterminated enhanced selector");
        return makeErrorToken("Unterminated enhanced selector");
    }
    
    advance(); // 跳过第一个 }
    advance(); // 跳过第二个 }
    value += "}}";
    
    return makeToken(CHTLTokenType::ENHANCED_SELECTOR, value);
}

CHTLToken CHTLLexer::scanLineComment() {
    std::string value = "//";
    
    while (!isAtEnd() && peek() != '\n') {
        value += advance();
    }
    
    return makeToken(CHTLTokenType::LINE_COMMENT, value);
}

CHTLToken CHTLLexer::scanBlockComment() {
    std::string value = "/*";
    
    while (!isAtEnd()) {
        if (peek() == '*' && position + 1 < source.length() && source[position + 1] == '/') {
            value += advance(); // *
            value += advance(); // /
            break;
        }
        if (peek() == '\n') {
            line++;
            column = 1;
        }
        value += advance();
    }
    
    return makeToken(CHTLTokenType::BLOCK_COMMENT, value);
}

CHTLToken CHTLLexer::scanGeneratorComment() {
    std::string value = "--";
    
    while (!isAtEnd() && peek() != '\n') {
        value += advance();
    }
    
    return makeToken(CHTLTokenType::GENERATOR_COMMENT, value);
}

CHTLToken CHTLLexer::scanCEEquivalent(char c) {
    // CE对等式：: 和 = 等价
    CHTLTokenType tokenType = (c == ':') ? CHTLTokenType::COLON : CHTLTokenType::EQUAL;
    return makeToken(tokenType, std::string(1, c));
}

// === 字符操作 ===
char CHTLLexer::peek(size_t offset) const {
    size_t pos = position + offset;
    return pos >= source.length() ? '\0' : source[pos];
}

char CHTLLexer::advance() {
    if (isAtEnd()) return '\0';
    
    char c = source[position++];
    column++;
    return c;
}

bool CHTLLexer::isAtEnd() const {
    return position >= source.length();
}

bool CHTLLexer::match(char expected) {
    if (isAtEnd() || source[position] != expected) {
        return false;
    }
    
    position++;
    column++;
    return true;
}

// === 字符分类 ===
bool CHTLLexer::isAlpha(char c) const {
    return std::isalpha(c) || c == '_';
}

bool CHTLLexer::isDigit(char c) const {
    return std::isdigit(c);
}

bool CHTLLexer::isAlphaNumeric(char c) const {
    return isAlpha(c) || isDigit(c);
}

bool CHTLLexer::isWhitespace(char c) const {
    return c == ' ' || c == '\t' || c == '\r';
}

bool CHTLLexer::isValidIdentifierStart(char c) const {
    return isAlpha(c);
}

bool CHTLLexer::isValidIdentifierChar(char c) const {
    return isAlphaNumeric(c) || c == '-' || c == '_';
}

bool CHTLLexer::isValidUnquotedChar(char c) const {
    return isAlphaNumeric(c) || c == '-' || c == '_' || c == '%' || c == '.' && isDigit(peek(1));
}

// === Token创建 ===
CHTLToken CHTLLexer::makeToken(CHTLTokenType type, const std::string& value) {
    std::string tokenValue = value.empty() ? getCurrentLexeme() : value;
    return CHTLToken(type, tokenValue, line, column - tokenValue.length(), start);
}

CHTLToken CHTLLexer::makeErrorToken(const std::string& message) {
    return CHTLToken(CHTLTokenType::INVALID, message, line, column, start);
}

// === 错误处理 ===
void CHTLLexer::addError(const std::string& message) {
    std::string errorMsg = "Lexer error at line " + std::to_string(line) + 
                          ", column " + std::to_string(column) + ": " + message;
    errors.push_back(errorMsg);
}

// === 工具方法 ===
void CHTLLexer::skipWhitespace() {
    while (!isAtEnd() && isWhitespace(peek())) {
        if (peek() == '\t') column += 4; // Tab作为4个空格
        advance();
    }
}

void CHTLLexer::skipToEndOfLine() {
    while (!isAtEnd() && peek() != '\n') {
        advance();
    }
}

std::string CHTLLexer::getCurrentLexeme() const {
    if (start >= source.length()) return "";
    return source.substr(start, position - start);
}

bool CHTLLexer::checkKeyword(const std::string& keyword) const {
    if (position + keyword.length() > source.length()) {
        return false;
    }
    
    return source.substr(position, keyword.length()) == keyword;
}

bool CHTLLexer::checkBracketKeyword(const std::string& keyword) const {
    return checkKeyword(keyword);
}

} // namespace CHTL