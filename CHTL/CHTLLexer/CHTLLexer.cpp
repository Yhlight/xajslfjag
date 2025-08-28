#include "CHTLLexer.h"
#include <cctype>
#include <algorithm>

namespace CHTL {

CHTLLexer::CHTLLexer() 
    : currentPos_(0), currentLine_(1), currentColumn_(1) {
    globalMap_ = std::make_shared<CHTLGlobalMap>();
}

void CHTLLexer::setSource(const std::string& source) {
    source_ = source;
    reset();
}

std::shared_ptr<CHTLToken> CHTLLexer::getNextToken() {
    // 跳过空白字符和注释
    while (!isEOF()) {
        skipWhitespace();
        if (peek() == '/' && peek(1) == '/') {
            skipSingleLineComment();
        } else if (peek() == '/' && peek(1) == '*') {
            skipMultiLineComment();
        } else if (peek() == '-' && peek(1) == '-') {
            skipGeneratorComment();
        } else {
            break;
        }
    }
    
    if (isEOF()) {
        return createToken(CHTLTokenType::END_OF_FILE, "", currentLine_, currentColumn_);
    }
    
    char currentChar = peek();
    
    // 处理标识符和关键字
    if (std::isalpha(currentChar) || currentChar == '_') {
        return readIdentifier();
    }
    
    // 处理字符串字面量
    if (currentChar == '"' || currentChar == '\'') {
        return readString();
    }
    
    // 处理无修饰字面量
    if (std::isalnum(currentChar) || currentChar == '_' || currentChar == '-') {
        return readUnquotedLiteral();
    }
    
    // 处理数字
    if (std::isdigit(currentChar)) {
        return readNumber();
    }
    
    // 处理分隔符
    switch (currentChar) {
        case '{':
            advance();
            return createToken(CHTLTokenType::LEFT_BRACE, "{", currentLine_, currentColumn_ - 1);
        case '}':
            advance();
            return createToken(CHTLTokenType::RIGHT_BRACE, "}", currentLine_, currentColumn_ - 1);
        case '[':
            advance();
            return createToken(CHTLTokenType::LEFT_BRACKET, "[", currentLine_, currentColumn_ - 1);
        case ']':
            advance();
            return createToken(CHTLTokenType::RIGHT_BRACKET, "]", currentLine_, currentColumn_ - 1);
        case '(':
            advance();
            return createToken(CHTLTokenType::LEFT_PAREN, "(", currentLine_, currentColumn_ - 1);
        case ')':
            advance();
            return createToken(CHTLTokenType::RIGHT_PAREN, ")", currentLine_, currentColumn_ - 1);
        case ';':
            advance();
            return createToken(CHTLTokenType::SEMICOLON, ";", currentLine_, currentColumn_ - 1);
        case ':':
            advance();
            return createToken(CHTLTokenType::COLON, ":", currentLine_, currentColumn_ - 1);
        case '=':
            advance();
            return createToken(CHTLTokenType::EQUAL, "=", currentLine_, currentColumn_ - 1);
        case ',':
            advance();
            return createToken(CHTLTokenType::COMMA, ",", currentLine_, currentColumn_ - 1);
        case '.':
            advance();
            return createToken(CHTLTokenType::DOT, ".", currentLine_, currentColumn_ - 1);
        case '@':
            advance();
            return createToken(CHTLTokenType::AT, "@", currentLine_, currentColumn_ - 1);
        case '#':
            advance();
            return createToken(CHTLTokenType::HASH, "#", currentLine_, currentColumn_ - 1);
        case '&':
            advance();
            return createToken(CHTLTokenType::AMPERSAND, "&", currentLine_, currentColumn_ - 1);
        default:
            advance();
            return createToken(CHTLTokenType::UNKNOWN, std::string(1, currentChar), currentLine_, currentColumn_ - 1);
    }
}

std::vector<std::shared_ptr<CHTLToken>> CHTLLexer::getAllTokens() {
    std::vector<std::shared_ptr<CHTLToken>> tokens;
    reset();
    
    while (!isEOF()) {
        auto token = getNextToken();
        tokens.push_back(token);
        if (token->getType() == CHTLTokenType::END_OF_FILE) {
            break;
        }
    }
    
    return tokens;
}

void CHTLLexer::reset() {
    currentPos_ = 0;
    currentLine_ = 1;
    currentColumn_ = 1;
}

void CHTLLexer::skipWhitespace() {
    while (!isEOF() && std::isspace(peek())) {
        if (peek() == '\n') {
            advanceLine();
        } else {
            advance();
        }
    }
}

void CHTLLexer::skipComment() {
    if (peek() == '/' && peek(1) == '/') {
        skipSingleLineComment();
    } else if (peek() == '/' && peek(1) == '*') {
        skipMultiLineComment();
    }
}

void CHTLLexer::skipSingleLineComment() {
    while (!isEOF() && peek() != '\n') {
        advance();
    }
}

void CHTLLexer::skipMultiLineComment() {
    // 跳过 /*
    advance();
    advance();
    
    while (!isEOF()) {
        if (peek() == '*' && peek(1) == '/') {
            advance();
            advance();
            break;
        }
        if (peek() == '\n') {
            advanceLine();
        } else {
            advance();
        }
    }
}

void CHTLLexer::skipGeneratorComment() {
    // 跳过 --
    advance();
    advance();
    
    while (!isEOF() && peek() != '\n') {
        advance();
    }
}

std::shared_ptr<CHTLToken> CHTLLexer::readIdentifier() {
    size_t startColumn = currentColumn_;
    std::string identifier;
    
    while (!isEOF() && (std::isalnum(peek()) || peek() == '_')) {
        identifier += peek();
        advance();
    }
    
    // 检查是否是关键字
    CHTLTokenType type = getKeywordType(identifier);
    if (type != CHTLTokenType::IDENTIFIER) {
        return createToken(type, identifier, currentLine_, startColumn);
    }
    
    // 检查是否是模板、自定义、原始嵌入、导入类型
    type = getTemplateType(identifier);
    if (type != CHTLTokenType::IDENTIFIER) {
        return createToken(type, identifier, currentLine_, startColumn);
    }
    
    type = getCustomType(identifier);
    if (type != CHTLTokenType::IDENTIFIER) {
        return createToken(type, identifier, currentLine_, startColumn);
    }
    
    type = getOriginType(identifier);
    if (type != CHTLTokenType::IDENTIFIER) {
        return createToken(type, identifier, currentLine_, startColumn);
    }
    
    type = getImportType(identifier);
    if (type != CHTLTokenType::IDENTIFIER) {
        return createToken(type, identifier, currentLine_, startColumn);
    }
    
    return createToken(CHTLTokenType::IDENTIFIER, identifier, currentLine_, startColumn);
}

std::shared_ptr<CHTLToken> CHTLLexer::readString() {
    char quoteChar = peek();
    size_t startColumn = currentColumn_;
    std::string value;
    
    advance(); // 跳过引号
    
    while (!isEOF() && peek() != quoteChar) {
        if (peek() == '\\') {
            advance(); // 跳过转义字符
            if (!isEOF()) {
                value += peek();
                advance();
            }
        } else {
            value += peek();
            advance();
        }
    }
    
    if (!isEOF()) {
        advance(); // 跳过结束引号
    }
    
    return createToken(CHTLTokenType::STRING_LITERAL, value, currentLine_, startColumn);
}

std::shared_ptr<CHTLToken> CHTLLexer::readUnquotedLiteral() {
    size_t startColumn = currentColumn_;
    std::string value;
    
    while (!isEOF() && (std::isalnum(peek()) || peek() == '_' || peek() == '-')) {
        value += peek();
        advance();
    }
    
    return createToken(CHTLTokenType::UNQUOTED_LITERAL, value, currentLine_, startColumn);
}

std::shared_ptr<CHTLToken> CHTLLexer::readNumber() {
    size_t startColumn = currentColumn_;
    std::string value;
    
    while (!isEOF() && std::isdigit(peek())) {
        value += peek();
        advance();
    }
    
    // 处理小数点
    if (!isEOF() && peek() == '.') {
        value += peek();
        advance();
        while (!isEOF() && std::isdigit(peek())) {
            value += peek();
            advance();
        }
    }
    
    return createToken(CHTLTokenType::NUMBER, value, currentLine_, startColumn);
}

CHTLTokenType CHTLLexer::getKeywordType(const std::string& identifier) {
    if (identifier == "text") return CHTLTokenType::KEYWORD_TEXT;
    if (identifier == "style") return CHTLTokenType::KEYWORD_STYLE;
    if (identifier == "script") return CHTLTokenType::KEYWORD_SCRIPT;
    if (identifier == "Template") return CHTLTokenType::KEYWORD_TEMPLATE;
    if (identifier == "Custom") return CHTLTokenType::KEYWORD_CUSTOM;
    if (identifier == "Origin") return CHTLTokenType::KEYWORD_ORIGIN;
    if (identifier == "Import") return CHTLTokenType::KEYWORD_IMPORT;
    if (identifier == "Namespace") return CHTLTokenType::KEYWORD_NAMESPACE;
    if (identifier == "Configuration") return CHTLTokenType::KEYWORD_CONFIGURATION;
    if (identifier == "Info") return CHTLTokenType::KEYWORD_INFO;
    if (identifier == "Export") return CHTLTokenType::KEYWORD_EXPORT;
    if (identifier == "use") return CHTLTokenType::KEYWORD_USE;
    if (identifier == "except") return CHTLTokenType::KEYWORD_EXCEPT;
    if (identifier == "inherit") return CHTLTokenType::KEYWORD_INHERIT;
    if (identifier == "delete") return CHTLTokenType::KEYWORD_DELETE;
    if (identifier == "insert") return CHTLTokenType::KEYWORD_INSERT;
    if (identifier == "after") return CHTLTokenType::KEYWORD_AFTER;
    if (identifier == "before") return CHTLTokenType::KEYWORD_BEFORE;
    if (identifier == "replace") return CHTLTokenType::KEYWORD_REPLACE;
    if (identifier == "at") return CHTLTokenType::KEYWORD_AT_TOP; // 简化处理
    if (identifier == "from") return CHTLTokenType::KEYWORD_FROM;
    if (identifier == "as") return CHTLTokenType::KEYWORD_AS;
    
    return CHTLTokenType::IDENTIFIER;
}

CHTLTokenType CHTLLexer::getTemplateType(const std::string& identifier) {
    if (identifier == "@Style") return CHTLTokenType::TEMPLATE_STYLE;
    if (identifier == "@Element") return CHTLTokenType::TEMPLATE_ELEMENT;
    if (identifier == "@Var") return CHTLTokenType::TEMPLATE_VAR;
    
    return CHTLTokenType::IDENTIFIER;
}

CHTLTokenType CHTLLexer::getCustomType(const std::string& identifier) {
    if (identifier == "@Style") return CHTLTokenType::CUSTOM_STYLE;
    if (identifier == "@Element") return CHTLTokenType::CUSTOM_ELEMENT;
    if (identifier == "@Var") return CHTLTokenType::CUSTOM_VAR;
    
    return CHTLTokenType::IDENTIFIER;
}

CHTLTokenType CHTLLexer::getOriginType(const std::string& identifier) {
    if (identifier == "@Html") return CHTLTokenType::ORIGIN_HTML;
    if (identifier == "@Style") return CHTLTokenType::ORIGIN_STYLE;
    if (identifier == "@JavaScript") return CHTLTokenType::ORIGIN_JAVASCRIPT;
    
    return CHTLTokenType::IDENTIFIER;
}

CHTLTokenType CHTLLexer::getImportType(const std::string& identifier) {
    if (identifier == "@Html") return CHTLTokenType::IMPORT_HTML;
    if (identifier == "@Style") return CHTLTokenType::IMPORT_STYLE;
    if (identifier == "@JavaScript") return CHTLTokenType::IMPORT_JAVASCRIPT;
    if (identifier == "@Chtl") return CHTLTokenType::IMPORT_CHTL;
    if (identifier == "@CJmod") return CHTLTokenType::IMPORT_CJMOD;
    if (identifier == "@Config") return CHTLTokenType::IMPORT_CONFIG;
    
    return CHTLTokenType::IDENTIFIER;
}

bool CHTLLexer::isKeyword(const std::string& identifier) {
    return getKeywordType(identifier) != CHTLTokenType::IDENTIFIER;
}

bool CHTLLexer::isTemplate(const std::string& identifier) {
    return getTemplateType(identifier) != CHTLTokenType::IDENTIFIER;
}

bool CHTLLexer::isCustom(const std::string& identifier) {
    return getCustomType(identifier) != CHTLTokenType::IDENTIFIER;
}

bool CHTLLexer::isOrigin(const std::string& identifier) {
    return getOriginType(identifier) != CHTLTokenType::IDENTIFIER;
}

bool CHTLLexer::isImport(const std::string& identifier) {
    return getImportType(identifier) != CHTLTokenType::IDENTIFIER;
}

void CHTLLexer::advance() {
    if (!isEOF()) {
        currentPos_++;
        currentColumn_++;
    }
}

void CHTLLexer::advanceLine() {
    if (!isEOF()) {
        currentPos_++;
        currentLine_++;
        currentColumn_ = 1;
    }
}

char CHTLLexer::peek(size_t offset) const {
    size_t pos = currentPos_ + offset;
    if (pos >= source_.length()) {
        return '\0';
    }
    return source_[pos];
}

bool CHTLLexer::match(const std::string& expected) {
    for (size_t i = 0; i < expected.length(); i++) {
        if (peek(i) != expected[i]) {
            return false;
        }
    }
    return true;
}

} // namespace CHTL