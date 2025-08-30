#include "Lexer.h"
#include <cctype>
#include <sstream>
#include <algorithm>

namespace CHTL {

Lexer::Lexer(const std::string& source, std::shared_ptr<CompileContext> context,
             const LexerConfig& config)
    : source_(source), context_(context), config_(config) {
    context_->setPosition(line_, column_);
}

std::shared_ptr<Token> Lexer::nextToken() {
    // 如果缓冲区有Token，先返回缓冲区的
    if (!tokenBuffer_.empty()) {
        auto token = tokenBuffer_.front();
        tokenBuffer_.pop();
        return token;
    }
    
    return scanToken();
}

std::shared_ptr<Token> Lexer::peekToken() {
    if (tokenBuffer_.empty()) {
        auto token = scanToken();
        tokenBuffer_.push(token);
    }
    return tokenBuffer_.front();
}

std::vector<std::shared_ptr<Token>> Lexer::peekTokens(size_t count) {
    // 确保缓冲区有足够的Token
    while (tokenBuffer_.size() < count && !isAtEnd()) {
        tokenBuffer_.push(scanToken());
    }
    
    // 返回缓冲区中的Token
    std::vector<std::shared_ptr<Token>> result;
    std::queue<std::shared_ptr<Token>> temp = tokenBuffer_;
    
    while (!temp.empty() && result.size() < count) {
        result.push_back(temp.front());
        temp.pop();
    }
    
    return result;
}

void Lexer::reset() {
    current_ = 0;
    line_ = 1;
    column_ = 1;
    while (!tokenBuffer_.empty()) {
        tokenBuffer_.pop();
    }
}

std::vector<std::shared_ptr<Token>> Lexer::tokenizeAll() {
    std::vector<std::shared_ptr<Token>> tokens;
    reset();
    
    while (!isAtEnd()) {
        auto token = nextToken();
        tokens.push_back(token);
        if (token->getType() == TokenType::EOF_TOKEN) {
            break;
        }
    }
    
    return tokens;
}

std::shared_ptr<Token> Lexer::scanToken() {
    // 跳过空白字符
    if (config_.skipWhitespace) {
        skipWhitespace();
    }
    
    if (isAtEnd()) {
        return makeToken(TokenType::EOF_TOKEN);
    }
    
    // 记录Token开始位置
    tokenStart_ = current_;
    tokenStartLine_ = line_;
    tokenStartColumn_ = column_;
    
    char c = advance();
    
    // 单字符Token
    switch (c) {
        case '{': return makeToken(TokenType::LEFT_BRACE);
        case '}': return makeToken(TokenType::RIGHT_BRACE);
        case '(': return makeToken(TokenType::LEFT_PAREN);
        case ')': return makeToken(TokenType::RIGHT_PAREN);
        case ';': return makeToken(TokenType::SEMICOLON);
        case ',': return makeToken(TokenType::COMMA);
        case '.': return makeToken(TokenType::DOT);
        case '&': return makeToken(TokenType::AMPERSAND);
        case '#': return makeToken(TokenType::HASH);
        case '@':
            // 检查是否为类型标识符
            if (isAlpha(peek())) {
                return scanTypeIdentifier();
            }
            return makeToken(TokenType::AT);
            
        case ':':
            // CE对等式支持
            return makeToken(config_.enableCEEquivalence ? TokenType::COLON : TokenType::COLON);
            
        case '=':
            // CE对等式支持
            return makeToken(config_.enableCEEquivalence ? TokenType::COLON : TokenType::EQUAL);
            
        case '[':
            // 可能是方括号关键字如 [Template]
            if (isAlpha(peek())) {
                return scanBracketKeyword();
            }
            return makeToken(TokenType::LEFT_BRACKET);
            
        case ']':
            return makeToken(TokenType::RIGHT_BRACKET);
            
        case '"':
        case '\'':
            return scanString(c);
            
        case '-':
            if (match('-')) {
                // 生成器注释
                return scanGeneratorComment();
            }
            // 否则可能是标识符的一部分
            break;
            
        case '/':
            if (match('/')) {
                // 单行注释
                skipSingleLineComment();
                if (config_.skipComments) {
                    return scanToken();  // 递归获取下一个Token
                } else {
                    return makeToken(TokenType::SINGLE_LINE_COMMENT);
                }
            } else if (match('*')) {
                // 多行注释
                skipMultiLineComment();
                if (config_.skipComments) {
                    return scanToken();  // 递归获取下一个Token
                } else {
                    return makeToken(TokenType::MULTI_LINE_COMMENT);
                }
            }
            break;
    }
    
    // 数字
    if (isDigit(c)) {
        current_--;  // 回退以重新扫描
        column_--;
        return scanNumber();
    }
    
    // 标识符或关键字
    if (isIdentifierStart(c)) {
        current_--;  // 回退以重新扫描
        column_--;
        return scanIdentifier();
    }
    
    // 无修饰字面量（在特定上下文中）
    if (config_.enableUnquotedLiterals && 
        (context_->getStateManager().isInState(StateType::IN_TEXT_BLOCK) ||
         context_->getStateManager().isInState(StateType::IN_ATTRIBUTE_VALUE) ||
         context_->getStateManager().isInState(StateType::IN_STYLE_PROPERTY))) {
        current_--;  // 回退以重新扫描
        column_--;
        return scanUnquotedLiteral();
    }
    
    // 未知字符
    return errorToken("Unexpected character");
}

char Lexer::advance() {
    if (isAtEnd()) return '\0';
    char c = source_[current_++];
    updatePosition(c);
    return c;
}

bool Lexer::match(char expected) {
    if (isAtEnd()) return false;
    if (source_[current_] != expected) return false;
    current_++;
    updatePosition(expected);
    return true;
}

bool Lexer::match(const std::string& expected) {
    for (size_t i = 0; i < expected.length(); i++) {
        if (isAtEnd(i) || source_[current_ + i] != expected[i]) {
            return false;
        }
    }
    
    // 匹配成功，更新位置
    for (size_t i = 0; i < expected.length(); i++) {
        advance();
    }
    return true;
}

char Lexer::peek(size_t offset) const {
    if (isAtEnd(offset)) return '\0';
    return source_[current_ + offset];
}

bool Lexer::isAtEnd(size_t offset) const {
    return current_ + offset >= source_.length();
}

// Provide the no-argument version
bool Lexer::isAtEnd() const {
    return current_ >= source_.length() && tokenBuffer_.empty();
}

void Lexer::skipWhitespace() {
    while (!isAtEnd()) {
        char c = peek();
        if (c == ' ' || c == '\r' || c == '\t' || c == '\n') {
            advance();
        } else {
            break;
        }
    }
}

void Lexer::skipSingleLineComment() {
    // 跳过直到行尾
    while (!isAtEnd() && peek() != '\n') {
        advance();
    }
}

void Lexer::skipMultiLineComment() {
    // 跳过直到找到 */
    while (!isAtEnd()) {
        if (peek() == '*' && peek(1) == '/') {
            advance();  // 跳过 *
            advance();  // 跳过 /
            break;
        }
        advance();
    }
}

std::shared_ptr<Token> Lexer::scanGeneratorComment() {
    size_t start = current_;
    
    // 读取到行尾
    while (!isAtEnd() && peek() != '\n') {
        advance();
    }
    
    std::string comment = source_.substr(start, current_ - start);
    return makeToken(TokenType::GENERATOR_COMMENT, comment);
}

std::shared_ptr<Token> Lexer::scanString(char quote) {
    std::string value;
    
    while (!isAtEnd() && peek() != quote) {
        if (peek() == '\\') {
            advance();  // 跳过反斜杠
            if (!isAtEnd()) {
                char escaped = advance();
                // 处理转义字符
                switch (escaped) {
                    case 'n': value += '\n'; break;
                    case 't': value += '\t'; break;
                    case 'r': value += '\r'; break;
                    case '\\': value += '\\'; break;
                    case '"': value += '"'; break;
                    case '\'': value += '\''; break;
                    default: value += escaped; break;
                }
            }
        } else {
            value += advance();
        }
    }
    
    if (isAtEnd()) {
        return errorToken("Unterminated string");
    }
    
    // 跳过结束引号
    advance();
    
    return makeToken(TokenType::STRING_LITERAL, value);
}

std::shared_ptr<Token> Lexer::scanNumber() {
    while (isDigit(peek())) {
        advance();
    }
    
    // 检查小数点
    if (peek() == '.' && isDigit(peek(1))) {
        advance();  // 消费 .
        while (isDigit(peek())) {
            advance();
        }
    }
    
    std::string numberStr = source_.substr(tokenStart_, current_ - tokenStart_);
    
    // 尝试解析为整数或浮点数
    if (numberStr.find('.') != std::string::npos) {
        double value = std::stod(numberStr);
        return makeToken(TokenType::NUMBER_LITERAL, value);
    } else {
        int64_t value = std::stoll(numberStr);
        return makeToken(TokenType::NUMBER_LITERAL, value);
    }
}

std::shared_ptr<Token> Lexer::scanIdentifier() {
    while (isIdentifierPart(peek())) {
        advance();
    }
    
    std::string identifier = source_.substr(tokenStart_, current_ - tokenStart_);
    
    // 检查是否为 "at top" 或 "at bottom"
    if (identifier == "at" && checkAtTopBottom()) {
        return makeToken(peek() == 't' ? TokenType::KEYWORD_AT_TOP : TokenType::KEYWORD_AT_BOTTOM);
    }
    
    // 检查是否为关键字
    TokenType type = getKeywordType(identifier);
    
    return makeToken(type);
}

std::shared_ptr<Token> Lexer::scanUnquotedLiteral() {
    while (!isAtEnd() && isUnquotedLiteralChar(peek())) {
        advance();
    }
    
    std::string literal = source_.substr(tokenStart_, current_ - tokenStart_);
    return makeToken(TokenType::UNQUOTED_LITERAL, literal);
}

std::shared_ptr<Token> Lexer::scanBracketKeyword() {
    // 已经消费了 [，现在扫描到 ]
    while (!isAtEnd() && peek() != ']') {
        advance();
    }
    
    if (isAtEnd()) {
        return errorToken("Unterminated bracket keyword");
    }
    
    // 消费 ]
    advance();
    
    std::string keyword = source_.substr(tokenStart_, current_ - tokenStart_);
    TokenType type = getKeywordType(keyword);
    
    return makeToken(type, keyword);
}

std::shared_ptr<Token> Lexer::scanTypeIdentifier() {
    // 已经消费了 @，现在扫描标识符部分
    while (isAlphaNumeric(peek())) {
        advance();
    }
    
    std::string typeId = source_.substr(tokenStart_, current_ - tokenStart_);
    TokenType type = getTypeIdentifierType(typeId);
    
    return makeToken(type, typeId);
}

bool Lexer::checkAtTopBottom() {
    // 保存当前状态
    size_t savedCurrent = current_;
    size_t savedLine = line_;
    size_t savedColumn = column_;
    
    // 跳过空白
    while (!isAtEnd() && (peek() == ' ' || peek() == '\t')) {
        advance();
    }
    
    bool result = false;
    if (match("top") || match("bottom")) {
        // 检查后面是否为有效的分隔符
        if (isAtEnd() || !isAlphaNumeric(peek())) {
            result = true;
        }
    }
    
    // 如果不匹配，恢复状态
    if (!result) {
        current_ = savedCurrent;
        line_ = savedLine;
        column_ = savedColumn;
    }
    
    return result;
}

bool Lexer::isDigit(char c) const {
    return c >= '0' && c <= '9';
}

bool Lexer::isAlpha(char c) const {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

bool Lexer::isAlphaNumeric(char c) const {
    return isAlpha(c) || isDigit(c);
}

bool Lexer::isIdentifierStart(char c) const {
    return isAlpha(c) || c == '_';
}

bool Lexer::isIdentifierPart(char c) const {
    return isAlphaNumeric(c) || c == '_' || c == '-';
}

bool Lexer::isUnquotedLiteralChar(char c) const {
    // 无修饰字面量可以包含的字符
    return isAlphaNumeric(c) || c == '_' || c == '-' || c == '.' || 
           c == ':' || c == '#' || c == '%' || c == '(' || c == ')' ||
           c == ' ' || c == '\t';  // 允许空格和制表符
}

std::shared_ptr<Token> Lexer::makeToken(TokenType type) const {
    std::string lexeme = source_.substr(tokenStart_, current_ - tokenStart_);
    TokenLocation loc(tokenStartLine_, tokenStartColumn_, tokenStart_, 
                      current_ - tokenStart_);
    return std::make_shared<Token>(type, lexeme, loc);
}

std::shared_ptr<Token> Lexer::makeToken(TokenType type, const std::string& lexeme) const {
    TokenLocation loc(tokenStartLine_, tokenStartColumn_, tokenStart_, 
                      current_ - tokenStart_);
    return std::make_shared<Token>(type, lexeme, loc);
}

std::shared_ptr<Token> Lexer::makeToken(TokenType type, const TokenValue& value) const {
    std::string lexeme = source_.substr(tokenStart_, current_ - tokenStart_);
    TokenLocation loc(tokenStartLine_, tokenStartColumn_, tokenStart_, 
                      current_ - tokenStart_);
    return std::make_shared<Token>(type, lexeme, loc, value);
}

std::shared_ptr<Token> Lexer::errorToken(const std::string& message) const {
    context_->addError(message, tokenStartLine_, tokenStartColumn_);
    return makeToken(TokenType::UNKNOWN);
}

void Lexer::updatePosition(char c) {
    if (c == '\n') {
        line_++;
        column_ = 1;
    } else {
        column_++;
    }
    context_->setPosition(line_, column_);
}

} // namespace CHTL