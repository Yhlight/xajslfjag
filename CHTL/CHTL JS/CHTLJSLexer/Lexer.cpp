#include "Lexer.h"
#include <cctype>

namespace CHTLJS {

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
                advance();
                if (peek() == '{') {
                    advance();
                    token = std::make_shared<Token>(TokenType::SELECTOR_START, "{{", start_pos, getCurrentPosition());
                } else {
                    token = std::make_shared<Token>(TokenType::LEFT_BRACE, "{", start_pos, getCurrentPosition());
                }
                break;

            case '}':
                advance();
                if (peek() == '}') {
                    advance();
                    token = std::make_shared<Token>(TokenType::SELECTOR_END, "}}", start_pos, getCurrentPosition());
                } else {
                    token = std::make_shared<Token>(TokenType::RIGHT_BRACE, "}", start_pos, getCurrentPosition());
                }
                break;

            case '[':
                token = std::make_shared<Token>(TokenType::LEFT_BRACKET, std::string(1, advance()), start_pos, getCurrentPosition());
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

            case '&':
                advance();
                if (match('-') && match('>')) {
                    token = std::make_shared<Token>(TokenType::BIND_OPERATOR, "&->", start_pos, getCurrentPosition());
                } else {
                    // 回退并作为未知字符处理
                    current_--;
                    column_--;
                    token = std::make_shared<Token>(TokenType::UNKNOWN, std::string(1, advance()), start_pos, getCurrentPosition());
                }
                break;

            case '-':
                advance();
                if (match('>')) {
                    token = std::make_shared<Token>(TokenType::ARROW, "->", start_pos, getCurrentPosition());
                } else {
                    // 可能是数字的负号
                    current_--; 
                    column_--;
                    if (isDigit(peekNext())) {
                        token = scanNumber();
                    } else {
                        token = std::make_shared<Token>(TokenType::UNKNOWN, std::string(1, advance()), start_pos, getCurrentPosition());
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
                    current_--; 
                    column_--;
                    token = std::make_shared<Token>(TokenType::UNKNOWN, std::string(1, advance()), start_pos, getCurrentPosition());
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
                    token = std::make_shared<Token>(TokenType::UNKNOWN, std::string(1, advance()), start_pos, getCurrentPosition());
                }
                break;
        }

        if (token) {
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

    if (!isAtEnd()) {
        advance(); // 消费结束引号
    }

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
    if (type == TokenType::IDENTIFIER) {
        // 检查是否为增强语法关键字
        type = global_map_->getEnhancedKeywordType(value);
    }

    return std::make_shared<Token>(type, value, start_pos, getCurrentPosition());
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

bool Lexer::isAlpha(char c) const {
    return std::isalpha(c) || c == '_' || c == '$';
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

} // namespace CHTLJS