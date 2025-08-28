#include "Lexer.h"
#include <iostream>
#include <sstream>
#include <cctype>

namespace CHTL {

// 构造函数
Lexer::Lexer() 
    : current_(0), line_(1), column_(1), 
      startLine_(1), startColumn_(1), startPos_(0) {
}

Lexer::Lexer(const std::string& source) 
    : source_(source), current_(0), line_(1), column_(1),
      startLine_(1), startColumn_(1), startPos_(0) {
}

// 设置源代码
void Lexer::setSource(const std::string& source) {
    source_ = source;
    reset();
}

void Lexer::setSource(std::istream& input) {
    std::stringstream buffer;
    buffer << input.rdbuf();
    source_ = buffer.str();
    reset();
}

// 重置词法分析器
void Lexer::reset() {
    current_ = 0;
    line_ = 1;
    column_ = 1;
    tokens_.clear();
    errors_.clear();
}

// 扫描所有Token
std::vector<Token> Lexer::scanTokens() {
    tokens_.clear();
    errors_.clear();
    
    while (!isAtEnd()) {
        markStart();
        scanToken();
    }
    
    // 添加EOF Token
    tokens_.push_back(Token(TokenType::EOF_TOKEN, "", line_, column_));
    return tokens_;
}

// 获取下一个Token
Token Lexer::nextToken() {
    if (isAtEnd()) {
        return Token(TokenType::EOF_TOKEN, "", line_, column_);
    }
    
    markStart();
    scanToken();
    
    if (!tokens_.empty()) {
        Token token = tokens_.back();
        tokens_.pop_back();
        return token;
    }
    
    return Token(TokenType::UNKNOWN, "", line_, column_);
}

// 标记起始位置
void Lexer::markStart() {
    startPos_ = current_;
    startLine_ = line_;
    startColumn_ = column_;
}

// 获取当前词素
std::string Lexer::getCurrentLexeme() const {
    return source_.substr(startPos_, current_ - startPos_);
}

// 检查是否到达末尾
bool Lexer::isAtEnd() const {
    return current_ >= source_.length();
}

// 查看当前字符
char Lexer::peek() const {
    if (isAtEnd()) return '\0';
    return source_[current_];
}

// 查看下一个字符
char Lexer::peekNext() const {
    if (current_ + 1 >= source_.length()) return '\0';
    return source_[current_ + 1];
}

// 前进并返回当前字符
char Lexer::advance() {
    if (isAtEnd()) return '\0';
    
    char c = source_[current_++];
    if (c == '\n') {
        line_++;
        column_ = 1;
    } else {
        column_++;
    }
    return c;
}

// 匹配期望的字符
bool Lexer::match(char expected) {
    if (isAtEnd()) return false;
    if (source_[current_] != expected) return false;
    advance();
    return true;
}

// 匹配字符序列
bool Lexer::matchSequence(const std::string& sequence) {
    for (size_t i = 0; i < sequence.length(); ++i) {
        if (current_ + i >= source_.length() || 
            source_[current_ + i] != sequence[i]) {
            return false;
        }
    }
    
    // 匹配成功，前进
    for (size_t i = 0; i < sequence.length(); ++i) {
        advance();
    }
    return true;
}

// 字符判断方法
bool Lexer::isDigit(char c) const {
    return c >= '0' && c <= '9';
}

bool Lexer::isAlpha(char c) const {
    return (c >= 'a' && c <= 'z') || 
           (c >= 'A' && c <= 'Z') || 
           c == '_';
}

bool Lexer::isAlphaNumeric(char c) const {
    return isAlpha(c) || isDigit(c);
}

bool Lexer::isWhitespace(char c) const {
    return c == ' ' || c == '\r' || c == '\t';
}

bool Lexer::isIdentifierStart(char c) const {
    return isAlpha(c) || c == '_';
}

bool Lexer::isIdentifierPart(char c) const {
    return isAlphaNumeric(c) || c == '_' || c == '-';
}

// 创建Token
Token Lexer::makeToken(TokenType type) const {
    return Token(type, getCurrentLexeme(), startLine_, startColumn_);
}

Token Lexer::makeToken(TokenType type, const std::string& lexeme) const {
    return Token(type, lexeme, startLine_, startColumn_);
}

Token Lexer::makeToken(TokenType type, const std::string& lexeme, const std::string& value) const {
    return Token(type, lexeme, value, startLine_, startColumn_);
}

// 添加错误
void Lexer::addError(const std::string& message) {
    std::stringstream ss;
    ss << "Error at " << line_ << ":" << column_ << " - " << message;
    errors_.push_back(ss.str());
}

// 扫描单个Token
void Lexer::scanToken() {
    char c = advance();
    
    switch (c) {
        // 空白字符
        case ' ':
        case '\r':
        case '\t':
            // 跳过空白字符
            break;
            
        case '\n':
            // 换行符作为Token（某些情况下需要）
            tokens_.push_back(makeToken(TokenType::NEWLINE));
            break;
            
        // 单字符操作符
        case '(': tokens_.push_back(makeToken(TokenType::LEFT_PAREN)); break;
        case ')': tokens_.push_back(makeToken(TokenType::RIGHT_PAREN)); break;
        case ';': tokens_.push_back(makeToken(TokenType::SEMICOLON)); break;
        case ',': tokens_.push_back(makeToken(TokenType::COMMA)); break;
        case '.': tokens_.push_back(makeToken(TokenType::DOT)); break;
        case '*': tokens_.push_back(makeToken(TokenType::STAR)); break;
        case '&': tokens_.push_back(makeToken(TokenType::AMPERSAND)); break;
        case '#': tokens_.push_back(makeToken(TokenType::HASH)); break;
        
        // 可能是多字符的操作符
        case '{':
            if (match('{')) {
                tokens_.push_back(makeToken(TokenType::DOUBLE_BRACE_OPEN));
            } else {
                tokens_.push_back(makeToken(TokenType::LEFT_BRACE));
            }
            break;
            
        case '}':
            if (match('}')) {
                tokens_.push_back(makeToken(TokenType::DOUBLE_BRACE_CLOSE));
            } else {
                tokens_.push_back(makeToken(TokenType::RIGHT_BRACE));
            }
            break;
            
        case '[':
            // 检查是否是方括号关键字
            if (isAlpha(peek())) {
                scanBracketKeyword();
            } else {
                tokens_.push_back(makeToken(TokenType::LEFT_BRACKET));
            }
            break;
            
        case ']':
            tokens_.push_back(makeToken(TokenType::RIGHT_BRACKET));
            break;
            
        case ':':
            // CE对等式: : 和 = 等价
            tokens_.push_back(makeToken(TokenType::COLON));
            break;
            
        case '=':
            // CE对等式: : 和 = 等价
            tokens_.push_back(makeToken(TokenType::EQUALS));
            break;
            
        case '/':
            if (match('/')) {
                // 单行注释
                scanComment();
            } else if (match('*')) {
                // 多行注释
                scanMultiLineComment();
            } else {
                tokens_.push_back(makeToken(TokenType::SLASH));
            }
            break;
            
        case '-':
            handleDash();
            break;
            
        case '"':
            scanString('"');
            break;
            
        case '\'':
            scanString('\'');
            break;
            
        case '@':
            scanAtType();
            break;
            
        default:
            if (isDigit(c)) {
                scanNumber();
            } else if (isIdentifierStart(c)) {
                scanIdentifier();
            } else if ((unsigned char)c >= 128) {
                // 处理非ASCII字符（如中文）作为无修饰字面量的一部分
                scanUnquotedLiteral();
            } else {
                addError("Unexpected character: " + std::string(1, c));
                tokens_.push_back(makeToken(TokenType::UNKNOWN));
            }
            break;
    }
}

// 扫描无修饰字面量
void Lexer::scanUnquotedLiteral() {
    // 回退一个字符，因为第一个字符已经被advance()消费了
    current_--;
    column_--;
    if (source_[current_] == '\n') {
        line_--;
    }
    
    // 重新标记开始位置
    markStart();
    
    // 扫描直到遇到分隔符
    while (!isAtEnd()) {
        char c = peek();
        
        // 检查是否是结束符号
        if (c == ';' || c == ',' || c == '\n' || c == '\r' ||
            c == '{' || c == '}' || c == '[' || c == ']' ||
            c == '(' || c == ')' || c == ':' || c == '=' ||
            c == '"' || c == '\'' || c == '/' || c == '*') {
            break;
        }
        
        // 检查是否是空白字符（但换行符除外）
        if (c == ' ' || c == '\t') {
            // 检查后续是否还有非空白字符
            size_t lookahead = current_ + 1;
            while (lookahead < source_.length() && 
                   (source_[lookahead] == ' ' || source_[lookahead] == '\t')) {
                lookahead++;
            }
            
            // 如果后面是分隔符或换行，则结束
            if (lookahead >= source_.length() || 
                source_[lookahead] == '\n' || source_[lookahead] == '\r' ||
                source_[lookahead] == ';' || source_[lookahead] == ',' ||
                source_[lookahead] == ':' || source_[lookahead] == '=' ||
                source_[lookahead] == '{' || source_[lookahead] == '}') {
                break;
            }
        }
        
        advance();
    }
    
    // 去除尾部空白
    std::string lexeme = getCurrentLexeme();
    size_t end = lexeme.find_last_not_of(" \t");
    if (end != std::string::npos) {
        lexeme = lexeme.substr(0, end + 1);
    }
    
    // 创建无修饰字面量Token
    tokens_.push_back(Token(TokenType::UNQUOTED_LITERAL, lexeme, lexeme, startLine_, startColumn_));
}

// 扫描字符串
void Lexer::scanString(char quote) {
    std::string value;
    
    while (peek() != quote && !isAtEnd()) {
        if (peek() == '\n') {
            addError("Unterminated string literal");
            return;
        }
        
        if (peek() == '\\') {
            advance(); // 跳过反斜杠
            char escaped = advance();
            switch (escaped) {
                case 'n': value += '\n'; break;
                case 't': value += '\t'; break;
                case 'r': value += '\r'; break;
                case '\\': value += '\\'; break;
                case '"': value += '"'; break;
                case '\'': value += '\''; break;
                default:
                    value += escaped;
                    break;
            }
        } else {
            value += advance();
        }
    }
    
    if (isAtEnd()) {
        addError("Unterminated string literal");
        return;
    }
    
    // 消费结束引号
    advance();
    
    TokenType type = (quote == '"') ? TokenType::STRING_LITERAL : TokenType::SINGLE_QUOTE_STRING;
    tokens_.push_back(makeToken(type, getCurrentLexeme(), value));
}

// 扫描数字
void Lexer::scanNumber() {
    while (isDigit(peek())) {
        advance();
    }
    
    // 小数部分
    if (peek() == '.' && isDigit(peekNext())) {
        advance(); // 消费 '.'
        while (isDigit(peek())) {
            advance();
        }
    }
    
    tokens_.push_back(makeToken(TokenType::NUMBER));
}

// 扫描标识符
void Lexer::scanIdentifier() {
    while (isIdentifierPart(peek())) {
        advance();
    }
    
    std::string lexeme = getCurrentLexeme();
    GlobalMap& globalMap = GlobalMap::getInstance();
    
    // 检查是否是关键字
    if (globalMap.isKeyword(lexeme)) {
        tokens_.push_back(makeToken(globalMap.getKeywordType(lexeme)));
    } else {
        tokens_.push_back(makeToken(TokenType::IDENTIFIER));
    }
}

// 扫描单行注释
void Lexer::scanComment() {
    // 跳过注释内容直到行尾
    while (peek() != '\n' && !isAtEnd()) {
        advance();
    }
    
    tokens_.push_back(makeToken(TokenType::SINGLE_LINE_COMMENT));
}

// 扫描多行注释
void Lexer::scanMultiLineComment() {
    while (!isAtEnd()) {
        if (peek() == '*' && peekNext() == '/') {
            advance(); // 消费 '*'
            advance(); // 消费 '/'
            break;
        }
        advance();
    }
    
    if (isAtEnd() && (current_ < 2 || source_[current_-2] != '*' || source_[current_-1] != '/')) {
        addError("Unterminated multi-line comment");
        return;
    }
    
    tokens_.push_back(makeToken(TokenType::MULTI_LINE_COMMENT));
}

// 扫描生成器注释
void Lexer::scanGeneratorComment() {
    // 跳过注释内容直到行尾
    while (peek() != '\n' && !isAtEnd()) {
        advance();
    }
    
    tokens_.push_back(makeToken(TokenType::GENERATOR_COMMENT));
}

// 扫描方括号关键字
void Lexer::scanBracketKeyword() {
    // 已经消费了 '['
    while (isAlpha(peek()) || isDigit(peek())) {
        advance();
    }
    
    if (peek() != ']') {
        addError("Expected ']' after bracket keyword");
        return;
    }
    
    advance(); // 消费 ']'
    
    std::string lexeme = getCurrentLexeme();
    GlobalMap& globalMap = GlobalMap::getInstance();
    
    if (globalMap.isBracketKeyword(lexeme)) {
        tokens_.push_back(makeToken(globalMap.getBracketKeywordType(lexeme)));
    } else {
        addError("Unknown bracket keyword: " + lexeme);
        tokens_.push_back(makeToken(TokenType::UNKNOWN));
    }
}

// 扫描@前缀类型
void Lexer::scanAtType() {
    // 已经消费了 '@'
    if (!isAlpha(peek())) {
        addError("Expected identifier after '@'");
        tokens_.push_back(makeToken(TokenType::UNKNOWN));
        return;
    }
    
    while (isAlphaNumeric(peek())) {
        advance();
    }
    
    std::string lexeme = getCurrentLexeme();
    GlobalMap& globalMap = GlobalMap::getInstance();
    
    TokenType type = globalMap.getAtType(lexeme);
    if (type != TokenType::UNKNOWN) {
        tokens_.push_back(makeToken(type));
    } else {
        addError("Unknown @ type: " + lexeme);
        tokens_.push_back(makeToken(TokenType::UNKNOWN));
    }
}

// 处理破折号
void Lexer::handleDash() {
    if (match('-')) {
        // 生成器注释 --
        scanGeneratorComment();
    } else if (match('>')) {
        // 箭头操作符 ->
        tokens_.push_back(makeToken(TokenType::ARROW));
    } else {
        // 可能是标识符的一部分或错误
        addError("Unexpected '-' character");
        tokens_.push_back(makeToken(TokenType::UNKNOWN));
    }
}

// 调试输出
void Lexer::printTokens() const {
    std::cout << "=== CHTL Lexer Token Stream ===" << std::endl;
    for (const auto& token : tokens_) {
        std::cout << token << std::endl;
    }
    
    if (!errors_.empty()) {
        std::cout << "\n=== Lexer Errors ===" << std::endl;
        for (const auto& error : errors_) {
            std::cout << error << std::endl;
        }
    }
    std::cout << "================================" << std::endl;
}

} // namespace CHTL