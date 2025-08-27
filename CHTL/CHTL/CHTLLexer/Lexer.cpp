#include "Lexer.h"
#include <cctype>
#include <stdexcept>

namespace CHTL {

Lexer::Lexer(const std::string& source, GlobalMapPtr global_map)
    : source_(source), current_(0), line_(1), column_(1),
      global_map_(global_map ? global_map : std::make_shared<GlobalMap>()),
      ignore_whitespace_(true), ignore_comments_(false) {
}

TokenList Lexer::tokenize() {
    tokens_.clear();
    current_ = 0;
    line_ = 1;
    column_ = 1;

    while (!isAtEnd()) {
        Position start_pos = getCurrentPosition();
        
        // 跳过空白字符
        if (ignore_whitespace_ && isWhitespace(peek())) {
            skipWhitespace();
            continue;
        }

        char c = peek();
        TokenPtr token = nullptr;

        switch (c) {
            case ' ':
            case '\r':
            case '\t':
                if (!ignore_whitespace_) {
                    token = std::make_shared<Token>(TokenType::WHITESPACE, std::string(1, advance()), start_pos, getCurrentPosition());
                } else {
                    advance();
                }
                break;

            case '\n':
                token = std::make_shared<Token>(TokenType::NEWLINE, std::string(1, advance()), start_pos, getCurrentPosition());
                line_++;
                column_ = 1;
                break;

            case '{':
                token = std::make_shared<Token>(TokenType::LEFT_BRACE, std::string(1, advance()), start_pos, getCurrentPosition());
                break;

            case '}':
                token = std::make_shared<Token>(TokenType::RIGHT_BRACE, std::string(1, advance()), start_pos, getCurrentPosition());
                break;

            case '[':
                // 可能是块类型标识符
                token = scanBlockType();
                if (!token) {
                    token = std::make_shared<Token>(TokenType::LEFT_BRACKET, std::string(1, advance()), start_pos, getCurrentPosition());
                }
                break;

            case ']':
                token = std::make_shared<Token>(TokenType::RIGHT_BRACKET, std::string(1, advance()), start_pos, getCurrentPosition());
                break;

            case '(':
                token = std::make_shared<Token>(TokenType::LEFT_PAREN, std::string(1, advance()), start_pos, getCurrentPosition());
                break;

            case ')':
                token = std::make_shared<Token>(TokenType::RIGHT_PAREN, std::string(1, advance()), start_pos, getCurrentPosition());
                break;

            case ';':
                token = std::make_shared<Token>(TokenType::SEMICOLON, std::string(1, advance()), start_pos, getCurrentPosition());
                break;

            case ':':
                token = std::make_shared<Token>(TokenType::COLON, std::string(1, advance()), start_pos, getCurrentPosition());
                break;

            case '=':
                token = std::make_shared<Token>(TokenType::EQUAL, std::string(1, advance()), start_pos, getCurrentPosition());
                break;

            case ',':
                token = std::make_shared<Token>(TokenType::COMMA, std::string(1, advance()), start_pos, getCurrentPosition());
                break;

            case '.':
                token = std::make_shared<Token>(TokenType::DOT, std::string(1, advance()), start_pos, getCurrentPosition());
                break;

            case '@':
                // 类型标识符
                token = scanTypeIdentifier();
                break;

            case '&':
                advance();
                if (match('-') && match('>')) {
                    token = std::make_shared<Token>(TokenType::BIND_OPERATOR, "&->", start_pos, getCurrentPosition());
                } else {
                    token = std::make_shared<Token>(TokenType::AMPERSAND, "&", start_pos, getCurrentPosition());
                }
                break;

            case '-':
                advance();
                if (match('>')) {
                    token = std::make_shared<Token>(TokenType::ARROW, "->", start_pos, getCurrentPosition());
                } else if (match('-')) {
                    token = scanGeneratorComment();
                } else {
                    // 可能是数字的负号或字面量的一部分
                    current_--; // 回退
                    column_--;
                    if (isDigit(peekNext())) {
                        token = scanNumber();
                    } else {
                        token = scanLiteral();
                    }
                }
                break;

            case '/':
                advance();
                if (match('/')) {
                    token = scanSingleLineComment();
                } else if (match('*')) {
                    token = scanMultiLineComment();
                } else {
                    current_--; // 回退
                    column_--;
                    token = scanLiteral();
                }
                break;

            case '"':
            case '\'':
                token = scanString(c);
                break;

            default:
                if (isDigit(c)) {
                    token = scanNumber();
                } else if (isIdentifierStart(c)) {
                    token = scanIdentifier();
                } else {
                    // 作为字面量处理
                    token = scanLiteral();
                }
                break;
        }

        if (token) {
            // 处理组合关键字
            token = handleCombinedKeywords(token);
            
            // 如果设置了忽略注释，则跳过注释Token
            if (ignore_comments_ && token->isComment()) {
                continue;
            }
            
            tokens_.push_back(token);
        }
    }

    // 添加EOF Token
    Position end_pos = getCurrentPosition();
    tokens_.push_back(std::make_shared<Token>(TokenType::EOF_TOKEN, "", end_pos, end_pos));

    return tokens_;
}

void Lexer::reset(const std::string& source) {
    source_ = source;
    current_ = 0;
    line_ = 1;
    column_ = 1;
    tokens_.clear();
}

Position Lexer::getCurrentPosition() const {
    return Position(line_, column_, current_);
}

void Lexer::printTokens() const {
    for (const auto& token : tokens_) {
        std::cout << token->toString() << std::endl;
    }
}

bool Lexer::isAtEnd() const {
    return current_ >= source_.length();
}

char Lexer::peek() const {
    if (isAtEnd()) return '\0';
    return source_[current_];
}

char Lexer::peekNext() const {
    if (current_ + 1 >= source_.length()) return '\0';
    return source_[current_ + 1];
}

char Lexer::advance() {
    if (isAtEnd()) return '\0';
    column_++;
    return source_[current_++];
}

bool Lexer::match(char expected) {
    if (isAtEnd()) return false;
    if (source_[current_] != expected) return false;
    current_++;
    column_++;
    return true;
}

void Lexer::addToken(TokenType type, const std::string& value, const Position& start_pos) {
    Position end_pos = getCurrentPosition();
    tokens_.push_back(std::make_shared<Token>(type, value, start_pos, end_pos));
}

void Lexer::skipWhitespace() {
    while (!isAtEnd() && isWhitespace(peek()) && peek() != '\n') {
        advance();
    }
}

TokenPtr Lexer::scanString(char quote) {
    Position start_pos = getCurrentPosition();
    advance(); // 消费开始引号

    std::string value;
    while (!isAtEnd() && peek() != quote) {
        if (peek() == '\n') {
            line_++;
            column_ = 1;
        }
        
        // 处理转义字符
        if (peek() == '\\') {
            advance();
            if (!isAtEnd()) {
                char escaped = peek();
                switch (escaped) {
                    case 'n': value += '\n'; break;
                    case 't': value += '\t'; break;
                    case 'r': value += '\r'; break;
                    case '\\': value += '\\'; break;
                    case '"': value += '"'; break;
                    case '\'': value += '\''; break;
                    default: value += escaped; break;
                }
                advance();
            }
        } else {
            value += advance();
        }
    }

    if (isAtEnd()) {
        error("Unterminated string");
        return nullptr;
    }

    advance(); // 消费结束引号
    return std::make_shared<Token>(TokenType::STRING, value, start_pos, getCurrentPosition());
}

TokenPtr Lexer::scanNumber() {
    Position start_pos = getCurrentPosition();
    std::string value;

    // 处理负号
    if (peek() == '-') {
        value += advance();
    }

    // 处理整数部分
    while (!isAtEnd() && isDigit(peek())) {
        value += advance();
    }

    // 处理小数部分
    if (!isAtEnd() && peek() == '.' && isDigit(peekNext())) {
        value += advance(); // 消费 '.'
        while (!isAtEnd() && isDigit(peek())) {
            value += advance();
        }
    }

    return std::make_shared<Token>(TokenType::NUMBER, value, start_pos, getCurrentPosition());
}

TokenPtr Lexer::scanIdentifier() {
    Position start_pos = getCurrentPosition();
    std::string value;

    while (!isAtEnd() && isIdentifierPart(peek())) {
        value += advance();
    }

    // 检查是否为关键字
    TokenType type = global_map_->getKeywordType(value);
    if (type == TokenType::IDENTIFIER && global_map_->isHtmlElement(value)) {
        // HTML元素作为标识符处理，但可以在语义分析阶段区分
        type = TokenType::IDENTIFIER;
    }

    return std::make_shared<Token>(type, value, start_pos, getCurrentPosition());
}

TokenPtr Lexer::scanLiteral() {
    Position start_pos = getCurrentPosition();
    std::string value;

    // 扫描直到遇到分隔符或空白字符
    while (!isAtEnd()) {
        char c = peek();
        if (isWhitespace(c) || c == '{' || c == '}' || c == '[' || c == ']' ||
            c == '(' || c == ')' || c == ';' || c == ':' || c == '=' || c == ',') {
            break;
        }
        value += advance();
    }

    if (value.empty()) {
        advance(); // 跳过未知字符
        return std::make_shared<Token>(TokenType::UNKNOWN, std::string(1, source_[current_ - 1]), start_pos, getCurrentPosition());
    }

    return std::make_shared<Token>(TokenType::LITERAL, value, start_pos, getCurrentPosition());
}

TokenPtr Lexer::scanSingleLineComment() {
    Position start_pos = getCurrentPosition();
    std::string value = "//";

    while (!isAtEnd() && peek() != '\n') {
        value += advance();
    }

    return std::make_shared<Token>(TokenType::SINGLE_COMMENT, value, start_pos, getCurrentPosition());
}

TokenPtr Lexer::scanMultiLineComment() {
    Position start_pos = getCurrentPosition();
    std::string value = "/*";

    while (!isAtEnd()) {
        if (peek() == '*' && peekNext() == '/') {
            value += advance(); // *
            value += advance(); // /
            break;
        }
        
        if (peek() == '\n') {
            line_++;
            column_ = 1;
        }
        
        value += advance();
    }

    return std::make_shared<Token>(TokenType::MULTI_COMMENT, value, start_pos, getCurrentPosition());
}

TokenPtr Lexer::scanGeneratorComment() {
    Position start_pos = getCurrentPosition();
    std::string value = "--";

    while (!isAtEnd() && peek() != '\n') {
        value += advance();
    }

    return std::make_shared<Token>(TokenType::GEN_COMMENT, value, start_pos, getCurrentPosition());
}

TokenPtr Lexer::scanBlockType() {
    Position start_pos = getCurrentPosition();
    
    if (peek() != '[') {
        return nullptr;
    }

    size_t save_current = current_;
    size_t save_column = column_;
    
    std::string value;
    value += advance(); // [

    while (!isAtEnd() && peek() != ']') {
        value += advance();
    }

    if (isAtEnd() || peek() != ']') {
        // 不是完整的块类型，回退
        current_ = save_current;
        column_ = save_column;
        return nullptr;
    }

    value += advance(); // ]

    TokenType type = global_map_->getBlockType(value);
    if (type == TokenType::UNKNOWN) {
        // 不是已知的块类型，回退
        current_ = save_current;
        column_ = save_column;
        return nullptr;
    }

    return std::make_shared<Token>(type, value, start_pos, getCurrentPosition());
}

TokenPtr Lexer::scanTypeIdentifier() {
    Position start_pos = getCurrentPosition();
    
    if (peek() != '@') {
        return nullptr;
    }

    std::string value;
    value += advance(); // @

    while (!isAtEnd() && isIdentifierPart(peek())) {
        value += advance();
    }

    TokenType type = global_map_->getTypeIdentifier(value);
    if (type == TokenType::UNKNOWN) {
        // 可能是自定义类型标识符
        type = TokenType::IDENTIFIER;
    }

    return std::make_shared<Token>(type, value, start_pos, getCurrentPosition());
}

bool Lexer::isAlpha(char c) const {
    return std::isalpha(c) || c == '_';
}

bool Lexer::isDigit(char c) const {
    return std::isdigit(c);
}

bool Lexer::isAlphaNumeric(char c) const {
    return isAlpha(c) || isDigit(c);
}

bool Lexer::isWhitespace(char c) const {
    return c == ' ' || c == '\r' || c == '\t';
}

bool Lexer::isIdentifierStart(char c) const {
    return isAlpha(c);
}

bool Lexer::isIdentifierPart(char c) const {
    return isAlphaNumeric(c) || c == '-';
}

TokenPtr Lexer::handleCombinedKeywords(TokenPtr current_token) {
    if (!current_token || current_token->getType() != TokenType::IDENTIFIER) {
        return current_token;
    }

    // 处理 "at top" 和 "at bottom"
    if (current_token->getValue() == "at") {
        // 查看下一个非空白Token
        size_t save_current = current_;
        size_t save_line = line_;
        size_t save_column = column_;
        
        // 跳过空白
        while (!isAtEnd() && isWhitespace(peek())) {
            advance();
        }
        
        if (!isAtEnd()) {
            TokenPtr next_token = scanIdentifier();
            if (next_token && next_token->getValue() == "top") {
                current_token->setType(TokenType::AT_TOP);
                current_token->setValue("at top");
                current_token->setEndPos(getCurrentPosition());
                return current_token;
            } else if (next_token && next_token->getValue() == "bottom") {
                current_token->setType(TokenType::AT_BOTTOM);
                current_token->setValue("at bottom");
                current_token->setEndPos(getCurrentPosition());
                return current_token;
            }
        }
        
        // 回退
        current_ = save_current;
        line_ = save_line;
        column_ = save_column;
    }

    return current_token;
}

void Lexer::error(const std::string& message) const {
    throw std::runtime_error("Lexer error at line " + std::to_string(line_) + 
                            ", column " + std::to_string(column_) + ": " + message);
}

} // namespace CHTL