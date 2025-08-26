#include "CHTLLexer.h"
#include <sstream>
#include <cctype>

namespace CHTL {

// 构造函数
CHTLLexer::CHTLLexer(const std::string& sourceCode) 
    : source(sourceCode), position(0), line(1), column(1), currentState(LexerState::Normal) {
}

// 主词法分析函数
std::vector<CHTLToken> CHTLLexer::tokenize() {
    std::vector<CHTLToken> tokens;
    errors.clear();
    
    while (!isAtEnd()) {
        CHTLToken token = nextToken();
        if (token.isValid()) {
            tokens.push_back(token);
        }
    }
    
    // 添加EOF token
    tokens.push_back(makeToken(CHTLTokenType::EOF_TOKEN));
    
    return tokens;
}

// 获取下一个Token
CHTLToken CHTLLexer::nextToken() {
    skipWhitespace();
    
    if (isAtEnd()) {
        return makeToken(CHTLTokenType::EOF_TOKEN);
    }
    
    char c = peek();
    
    // 处理换行
    if (c == '\n') {
        advance();
        return makeToken(CHTLTokenType::NEWLINE, "\n");
    }
    
    // 处理字符串字面量
    if (c == '"' || c == '\'') {
        return scanStringLiteral(c);
    }
    
    // 处理数字
    if (isDigit(c)) {
        return scanNumber();
    }
    
    // 处理前缀 [Template], [Custom] 等
    if (c == '[') {
        return scanPrefix();
    }
    
    // 处理类型标识符 @Style, @Element 等
    if (c == '@') {
        return scanTypeIdentifier();
    }
    
    // 处理选择器 .class, #id
    if (c == '.' || c == '#') {
        return scanSelector();
    }
    
    // 处理注释
    if (c == '/') {
        char next = peek(1);
        if (next == '/') {
            return scanLineComment();
        } else if (next == '*') {
            return scanBlockComment();
        }
    }
    
    // 处理生成器注释 --
    if (c == '-' && peek(1) == '-') {
        return scanGeneratorComment();
    }
    
    // 处理标识符和关键字
    if (isValidIdentifierStart(c)) {
        return scanIdentifier();
    }
    
    // 处理单字符符号
    switch (c) {
        case '{': advance(); return makeToken(CHTLTokenType::LBRACE, "{");
        case '}': advance(); return makeToken(CHTLTokenType::RBRACE, "}");
        case '(': advance(); return makeToken(CHTLTokenType::LPAREN, "(");
        case ')': advance(); return makeToken(CHTLTokenType::RPAREN, ")");
        case '<': advance(); return makeToken(CHTLTokenType::LT, "<");
        case '>': advance(); return makeToken(CHTLTokenType::GT, ">");
        case ';': advance(); return makeToken(CHTLTokenType::SEMICOLON, ";");
        case ',': advance(); return makeToken(CHTLTokenType::COMMA, ",");
        case '*': advance(); return makeToken(CHTLTokenType::STAR, "*");
        case '/': advance(); return makeToken(CHTLTokenType::SLASH, "/");
        case '&': advance(); return makeToken(CHTLTokenType::AMPERSAND, "&");
        case ':': return handleCEEquivalent(c);
        case '=': return handleCEEquivalent(c);
        default:
            // 尝试无引号字面量
            if (isValidUnquotedChar(c)) {
                return scanUnquotedLiteral();
            }
            
            // 无法识别的字符
            std::string errorChar(1, c);
            advance();
            return makeErrorToken("Unexpected character: " + errorChar);
    }
}

// 扫描字符串字面量
CHTLToken CHTLLexer::scanStringLiteral(char quote) {
    size_t start = position;
    advance(); // 跳过开始引号
    
    std::string value;
    
    while (!isAtEnd() && peek() != quote) {
        char c = peek();
        
        if (c == '\n') {
            line++;
            column = 1;
        }
        
        if (c == '\\' && !isAtEnd()) {
            advance(); // 跳过反斜杠
            if (!isAtEnd()) {
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
            }
        } else {
            value += advance();
        }
    }
    
    if (isAtEnd()) {
        return makeErrorToken("Unterminated string literal");
    }
    
    advance(); // 跳过结束引号
    return makeToken(CHTLTokenType::STRING_LITERAL, value);
}

// 扫描数字
CHTLToken CHTLLexer::scanNumber() {
    size_t start = position;
    
    while (isDigit(peek())) {
        advance();
    }
    
    // 处理小数点
    if (peek() == '.' && isDigit(peek(1))) {
        advance(); // 消费 '.'
        while (isDigit(peek())) {
            advance();
        }
    }
    
    std::string value = source.substr(start, position - start);
    return makeToken(CHTLTokenType::NUMBER, value);
}

// 扫描标识符
CHTLToken CHTLLexer::scanIdentifier() {
    size_t start = position;
    
    while (isValidIdentifierChar(peek())) {
        advance();
    }
    
    std::string value = source.substr(start, position - start);
    
    // 处理多词关键字
    if (value == "at") {
        // 检查 "at top" 或 "at bottom"
        size_t savedPos = position;
        skipWhitespace();
        
        if (peek() == 't' && source.substr(position, 3) == "top") {
            position += 3;
            column += 3;
            return makeToken(CHTLTokenType::AT_TOP, "at top");
        } else if (peek() == 'b' && source.substr(position, 6) == "bottom") {
            position += 6;
            column += 6;
            return makeToken(CHTLTokenType::AT_BOTTOM, "at bottom");
        } else {
            position = savedPos;
        }
    }
    
    // 检查是否为关键字
    CHTLTokenType tokenType = CHTLTokenUtils::stringToTokenType(value);
    return makeToken(tokenType, value);
}

// 扫描前缀
CHTLToken CHTLLexer::scanPrefix() {
    if (peek() != '[') {
        return makeErrorToken("Expected '[' for prefix");
    }
    
    size_t start = position;
    advance(); // 跳过 '['
    
    std::string prefixContent;
    while (!isAtEnd() && peek() != ']') {
        prefixContent += advance();
    }
    
    if (isAtEnd()) {
        return makeErrorToken("Unterminated prefix, expected ']'");
    }
    
    advance(); // 跳过 ']'
    
    std::string fullPrefix = "[" + prefixContent + "]";
    CHTLTokenType tokenType = CHTLTokenUtils::stringToTokenType(fullPrefix);
    
    return makeToken(tokenType, fullPrefix);
}

// 扫描类型标识符
CHTLToken CHTLLexer::scanTypeIdentifier() {
    if (peek() != '@') {
        return makeErrorToken("Expected '@' for type identifier");
    }
    
    size_t start = position;
    advance(); // 跳过 '@'
    
    while (isValidIdentifierChar(peek())) {
        advance();
    }
    
    std::string value = source.substr(start, position - start);
    CHTLTokenType tokenType = CHTLTokenUtils::stringToTokenType(value);
    
    return makeToken(tokenType, value);
}

// 扫描选择器
CHTLToken CHTLLexer::scanSelector() {
    char selectorType = peek();
    advance(); // 跳过 '.' 或 '#'
    
    size_t start = position - 1;
    
    while (isValidIdentifierChar(peek())) {
        advance();
    }
    
    std::string value = source.substr(start, position - start);
    
    if (selectorType == '.') {
        return makeToken(CHTLTokenType::CLASS_SELECTOR, value);
    } else {
        return makeToken(CHTLTokenType::ID_SELECTOR, value);
    }
}

// 扫描行注释
CHTLToken CHTLLexer::scanLineComment() {
    size_t start = position;
    advance(); // 跳过第一个 '/'
    advance(); // 跳过第二个 '/'
    
    while (!isAtEnd() && peek() != '\n') {
        advance();
    }
    
    std::string value = source.substr(start, position - start);
    return makeToken(CHTLTokenType::LINE_COMMENT, value);
}

// 扫描块注释
CHTLToken CHTLLexer::scanBlockComment() {
    size_t start = position;
    advance(); // 跳过 '/'
    advance(); // 跳过 '*'
    
    while (!isAtEnd()) {
        if (peek() == '*' && peek(1) == '/') {
            advance(); // 跳过 '*'
            advance(); // 跳过 '/'
            break;
        }
        
        if (peek() == '\n') {
            line++;
            column = 1;
        }
        advance();
    }
    
    std::string value = source.substr(start, position - start);
    return makeToken(CHTLTokenType::BLOCK_COMMENT, value);
}

// 扫描生成器注释
CHTLToken CHTLLexer::scanGeneratorComment() {
    size_t start = position;
    advance(); // 跳过第一个 '-'
    advance(); // 跳过第二个 '-'
    
    while (!isAtEnd() && peek() != '\n') {
        advance();
    }
    
    std::string value = source.substr(start, position - start);
    return makeToken(CHTLTokenType::GENERATOR_COMMENT, value);
}

// 扫描无引号字面量
CHTLToken CHTLLexer::scanUnquotedLiteral() {
    size_t start = position;
    
    while (isValidUnquotedChar(peek())) {
        advance();
    }
    
    std::string value = source.substr(start, position - start);
    return makeToken(CHTLTokenType::UNQUOTED_LITERAL, value);
}

// 处理CE对等式
CHTLToken CHTLLexer::handleCEEquivalent(char c) {
    advance();
    if (c == ':') {
        return makeToken(CHTLTokenType::COLON, ":");
    } else {
        return makeToken(CHTLTokenType::EQUAL, "=");
    }
}

// 字符检查方法
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

bool CHTLLexer::isDigit(char c) const {
    return c >= '0' && c <= '9';
}

bool CHTLLexer::isAlpha(char c) const {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
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
    return isAlphaNumeric(c) || c == '-';
}

bool CHTLLexer::isValidUnquotedChar(char c) const {
    // CHTL支持CSS风格的无引号字符串
    return isAlphaNumeric(c) || c == '-' || c == '_' || c == '%';
}

void CHTLLexer::skipWhitespace() {
    while (!isAtEnd() && isWhitespace(peek())) {
        advance();
    }
}

CHTLToken CHTLLexer::makeToken(CHTLTokenType type, const std::string& value) {
    return CHTLToken(type, value, line, column - value.length(), position - value.length());
}

CHTLToken CHTLLexer::makeErrorToken(const std::string& message) {
    addError(message);
    return CHTLToken(CHTLTokenType::INVALID, message, line, column, position);
}

void CHTLLexer::addError(const std::string& message) {
    std::stringstream ss;
    ss << "Lexer error at line " << line << ", column " << column << ": " << message;
    errors.push_back(ss.str());
}

void CHTLLexer::reset(const std::string& newSource) {
    if (!newSource.empty()) {
        source = newSource;
    }
    position = 0;
    line = 1;
    column = 1;
    currentState = LexerState::Normal;
    errors.clear();
}

} // namespace CHTL