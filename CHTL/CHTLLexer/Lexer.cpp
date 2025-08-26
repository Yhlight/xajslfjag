#include "Lexer.h"
#include <iostream>
#include <sstream>
#include <cctype>

namespace CHTL {

Lexer::Lexer(const std::string& source, GlobalMap* globalMap) 
    : source(source), position(0), line(1), column(1), start_pos(0), global_map(globalMap) {
}

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

bool Lexer::isAtEnd() const {
    return position >= source.length();
}

bool Lexer::isAlpha(char c) const {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

bool Lexer::isDigit(char c) const {
    return c >= '0' && c <= '9';
}

bool Lexer::isAlphaNumeric(char c) const {
    return isAlpha(c) || isDigit(c);
}

bool Lexer::isWhitespace(char c) const {
    return c == ' ' || c == '\t' || c == '\r';
}

bool Lexer::match(char expected) {
    if (isAtEnd()) return false;
    if (source[position] != expected) return false;
    position++;
    column++;
    return true;
}

bool Lexer::matchSequence(const std::string& sequence) {
    if (position + sequence.length() > source.length()) {
        return false;
    }
    
    for (size_t i = 0; i < sequence.length(); i++) {
        if (source[position + i] != sequence[i]) {
            return false;
        }
    }
    
    // 匹配成功，前进位置
    for (size_t i = 0; i < sequence.length(); i++) {
        advance();
    }
    return true;
}

void Lexer::skipWhitespace() {
    while (!isAtEnd()) {
        char c = peek();
        if (isWhitespace(c)) {
            advance();
        } else {
            break;
        }
    }
}

Token Lexer::makeToken(TokenType type, const std::string& value) {
    std::string tokenValue = value.empty() ? 
        source.substr(start_pos, position - start_pos) : value;
    return Token(type, tokenValue, line, column - tokenValue.length(), start_pos, position);
}

Token Lexer::makeErrorToken(const std::string& message) {
    addError(message);
    return Token(TokenType::INVALID, message, line, column, start_pos, position);
}

Token Lexer::scanString(char quote) {
    std::string value;
    
    while (peek() != quote && !isAtEnd()) {
        if (peek() == '\n') {
            line++;
            column = 1;
        }
        
        char c = advance();
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
        return makeErrorToken("Unterminated string");
    }
    
    // 跳过结束引号
    advance();
    return makeToken(TokenType::STRING_LITERAL, value);
}

Token Lexer::scanUnquotedLiteral() {
    while (!isAtEnd()) {
        char c = peek();
        if (isAlphaNumeric(c) || c == '-' || c == '_') {
            advance();
        } else {
            break;
        }
    }
    return makeToken(TokenType::UNQUOTED_LITERAL);
}

Token Lexer::scanNumber() {
    while (isDigit(peek())) {
        advance();
    }
    
    // 查看是否有小数点
    if (peek() == '.' && isDigit(peek(1))) {
        advance(); // 消耗 '.'
        while (isDigit(peek())) {
            advance();
        }
    }
    
    return makeToken(TokenType::NUMBER);
}

Token Lexer::scanIdentifier() {
    while (isAlphaNumeric(peek()) || peek() == '-') {
        advance();
    }
    
    std::string text = source.substr(start_pos, position - start_pos);
    TokenType type = getIdentifierType(text);
    return makeToken(type, text);
}

Token Lexer::scanLineComment() {
    // 跳过 '//'
    advance();
    advance();
    
    std::string value;
    while (peek() != '\n' && !isAtEnd()) {
        value += advance();
    }
    
    return makeToken(TokenType::LINE_COMMENT, value);
}

Token Lexer::scanBlockComment() {
    // 跳过 '/*'
    advance();
    advance();
    
    std::string value;
    while (!isAtEnd()) {
        if (peek() == '*' && peek(1) == '/') {
            advance(); // '*'
            advance(); // '/'
            break;
        }
        value += advance();
    }
    
    return makeToken(TokenType::BLOCK_COMMENT, value);
}

Token Lexer::scanGeneratorComment() {
    // 跳过 '--'
    advance();
    advance();
    
    std::string value;
    while (peek() != '\n' && !isAtEnd()) {
        value += advance();
    }
    
    return makeToken(TokenType::GENERATOR_COMMENT, value);
}

Token Lexer::scanPrefix() {
    // 扫描 [Keyword] 形式的前缀
    advance(); // 跳过 '['
    
    std::string keyword;
    while (peek() != ']' && !isAtEnd()) {
        keyword += advance();
    }
    
    if (isAtEnd()) {
        return makeErrorToken("Unterminated prefix");
    }
    
    advance(); // 跳过 ']'
    
    // 确定前缀类型
    TokenType type = TokenType::INVALID;
    if (keyword == "Template") type = TokenType::TEMPLATE_PREFIX;
    else if (keyword == "Custom") type = TokenType::CUSTOM_PREFIX;
    else if (keyword == "Origin") type = TokenType::ORIGIN_PREFIX;
    else if (keyword == "Import") type = TokenType::IMPORT_PREFIX;
    else if (keyword == "Namespace") type = TokenType::NAMESPACE_PREFIX;
    else if (keyword == "Configuration") type = TokenType::CONFIGURATION_PREFIX;
    else if (keyword == "Info") type = TokenType::INFO_PREFIX;
    else if (keyword == "Export") type = TokenType::EXPORT_PREFIX;
    else if (keyword == "OriginType") type = TokenType::ORIGINTYPE_PREFIX;
    else if (keyword == "Name") type = TokenType::NAME_PREFIX;
    
    if (type == TokenType::INVALID) {
        return makeErrorToken("Unknown prefix: " + keyword);
    }
    
    return makeToken(type, "[" + keyword + "]");
}

Token Lexer::scanTypeIdentifier() {
    // 扫描 @Type 形式的类型标识符
    advance(); // 跳过 '@'
    
    std::string type_name;
    while (isAlphaNumeric(peek())) {
        type_name += advance();
    }
    
    // 确定类型
    TokenType type = TokenType::INVALID;
    if (type_name == "Style") type = TokenType::AT_STYLE;
    else if (type_name == "Element") type = TokenType::AT_ELEMENT;
    else if (type_name == "Var") type = TokenType::AT_VAR;
    else if (type_name == "Html") type = TokenType::AT_HTML;
    else if (type_name == "JavaScript") type = TokenType::AT_JAVASCRIPT;
    else if (type_name == "Chtl") type = TokenType::AT_CHTL;
    else if (type_name == "CJmod") type = TokenType::AT_CJMOD;
    else if (type_name == "Config") type = TokenType::AT_CONFIG;
    
    if (type == TokenType::INVALID) {
        return makeToken(TokenType::IDENTIFIER, "@" + type_name);
    }
    
    return makeToken(type, "@" + type_name);
}

Token Lexer::scanSelector() {
    char first = peek();
    if (first == '.') {
        advance(); // 跳过 '.'
        std::string class_name;
        while (isAlphaNumeric(peek()) || peek() == '-' || peek() == '_') {
            class_name += advance();
        }
        return makeToken(TokenType::CLASS_SELECTOR, "." + class_name);
    } else if (first == '#') {
        advance(); // 跳过 '#'
        std::string id_name;
        while (isAlphaNumeric(peek()) || peek() == '-' || peek() == '_') {
            id_name += advance();
        }
        return makeToken(TokenType::ID_SELECTOR, "#" + id_name);
    }
    
    return makeErrorToken("Invalid selector");
}

TokenType Lexer::getIdentifierType(const std::string& text) {
    if (global_map) {
        if (global_map->isKeyword(text)) {
            return global_map->getKeywordType(text);
        }
        if (global_map->isHtmlTag(text)) {
            return TokenType::HTML_TAG;
        }
    }
    
    // 检查内置关键字
    if (text == "text") return TokenType::TEXT_KEYWORD;
    if (text == "style") return TokenType::STYLE_KEYWORD;
    if (text == "script") return TokenType::SCRIPT_KEYWORD;
    if (text == "inherit") return TokenType::INHERIT;
    if (text == "delete") return TokenType::DELETE;
    if (text == "insert") return TokenType::INSERT;
    if (text == "after") return TokenType::AFTER;
    if (text == "before") return TokenType::BEFORE;
    if (text == "replace") return TokenType::REPLACE;
    if (text == "from") return TokenType::FROM;
    if (text == "as") return TokenType::AS;
    if (text == "except") return TokenType::EXCEPT;
    if (text == "use") return TokenType::USE;
    if (text == "html5") return TokenType::HTML5;
    
    return TokenType::IDENTIFIER;
}

std::vector<Token> Lexer::tokenize() {
    tokens.clear();
    errors.clear();
    
    while (!isAtEnd()) {
        Token token = nextToken();
        if (token.type != TokenType::WHITESPACE) {
            tokens.push_back(token);
        }
    }
    
    tokens.push_back(Token(TokenType::EOF_TOKEN, "", line, column, position, position));
    return tokens;
}

Token Lexer::nextToken() {
    skipWhitespace();
    
    start_pos = position;
    
    if (isAtEnd()) {
        return makeToken(TokenType::EOF_TOKEN);
    }
    
    char c = peek();
    
    // 单字符token
    switch (c) {
        case '{': advance(); return makeToken(TokenType::LBRACE);
        case '}': advance(); return makeToken(TokenType::RBRACE);
        case '(': advance(); return makeToken(TokenType::LPAREN);
        case ')': advance(); return makeToken(TokenType::RPAREN);
        case '[': 
            if (isAlpha(peek(1))) {
                return scanPrefix();
            } else {
                advance(); 
                return makeToken(TokenType::LBRACKET);
            }
        case ']': advance(); return makeToken(TokenType::RBRACKET);
        case ';': advance(); return makeToken(TokenType::SEMICOLON);
        case ':': 
            if (peek(1) == ':') {
                advance(); advance();
                return makeToken(TokenType::DOUBLE_COLON);
            }
            advance(); 
            return makeToken(TokenType::COLON);
        case '=': advance(); return makeToken(TokenType::EQUAL);
        case ',': advance(); return makeToken(TokenType::COMMA);
        case '.': 
            if (isAlpha(peek(1))) {
                return scanSelector();
            } else if (isDigit(peek(1))) {
                return scanNumber();
            }
            advance(); 
            return makeToken(TokenType::DOT);
        case '#': 
            if (isAlpha(peek(1))) {
                return scanSelector();
            }
            advance(); 
            return makeToken(TokenType::HASH);
        case '&': advance(); return makeToken(TokenType::AMPERSAND);
        case '*':
            if (peek(1) == '*') {
                advance(); advance();
                return makeToken(TokenType::DOUBLE_STAR);
            }
            advance();
            return makeToken(TokenType::STAR);
        case '\n':
            advance();
            return makeToken(TokenType::NEWLINE);
    }
    
    // 多字符token
    if (c == '/') {
        if (peek(1) == '/') {
            return scanLineComment();
        } else if (peek(1) == '*') {
            return scanBlockComment();
        } else {
            advance();
            return makeToken(TokenType::SLASH);
        }
    }
    
    if (c == '-') {
        if (peek(1) == '-') {
            return scanGeneratorComment();
        } else if (peek(1) == '>') {
            advance(); advance();
            return makeToken(TokenType::ARROW);
        } else if (isAlphaNumeric(peek(1))) {
            return scanUnquotedLiteral();
        } else {
            advance();
            return makeToken(TokenType::IDENTIFIER, "-");
        }
    }
    
    if (c == '@') {
        return scanTypeIdentifier();
    }
    
    if (c == '"' || c == '\'') {
        advance();
        return scanString(c);
    }
    
    if (isDigit(c)) {
        return scanNumber();
    }
    
    if (isAlpha(c)) {
        return scanIdentifier();
    }
    
    // 未识别的字符
    advance();
    return makeErrorToken("Unexpected character: " + std::string(1, c));
}

void Lexer::reset() {
    position = 0;
    line = 1;
    column = 1;
    start_pos = 0;
    tokens.clear();
    errors.clear();
}

void Lexer::reset(const std::string& newSource) {
    source = newSource;
    reset();
}

const std::vector<Token>& Lexer::getTokens() const {
    return tokens;
}

size_t Lexer::getCurrentPosition() const {
    return position;
}

size_t Lexer::getCurrentLine() const {
    return line;
}

size_t Lexer::getCurrentColumn() const {
    return column;
}

std::vector<std::string> Lexer::getErrors() const {
    return errors;
}

bool Lexer::hasErrors() const {
    return !errors.empty();
}

void Lexer::printTokens() const {
    std::cout << "=== Tokens ===" << std::endl;
    for (const auto& token : tokens) {
        std::cout << token.getTypeName() << ": '" << token.value 
                  << "' at " << token.line << ":" << token.column << std::endl;
    }
}

void Lexer::addError(const std::string& message) {
    std::stringstream ss;
    ss << "Error at line " << line << ", column " << column << ": " << message;
    errors.push_back(ss.str());
}

// 上下文相关方法的简单实现
bool Lexer::isInStyleBlock() const {
    // 简化实现 - 实际需要更复杂的上下文跟踪
    return false;
}

bool Lexer::isInScriptBlock() const {
    // 简化实现 - 实际需要更复杂的上下文跟踪
    return false;
}

bool Lexer::isInConfigBlock() const {
    // 简化实现 - 实际需要更复杂的上下文跟踪
    return false;
}

Token Lexer::scanPath() {
    // 路径扫描的简化实现
    return scanIdentifier();
}

bool Lexer::isReservedKeyword(const std::string& text) const {
    static const std::unordered_set<std::string> reserved = {
        "text", "style", "script", "inherit", "delete", "insert",
        "after", "before", "replace", "from", "as", "except", "use", "html5"
    };
    return reserved.find(text) != reserved.end();
}

} // namespace CHTL