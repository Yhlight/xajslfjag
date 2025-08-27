#include "Lexer.h"
#include <cctype>
#include <sstream>
#include <algorithm>

namespace CHTLJS {

Lexer::Lexer(const std::string& src) 
    : source(src), position(0), line(1), column(1), inCHTLSelector(false) {
    globalMap = GlobalMap::getInstance();
}

Lexer::~Lexer() {
    // GlobalMap是单例，不在这里销毁
}

char Lexer::getCurrentChar() const {
    if (isAtEnd()) return '\0';
    return source[position];
}

char Lexer::peekChar(int offset) const {
    size_t pos = position + offset;
    if (pos >= source.length()) return '\0';
    return source[pos];
}

void Lexer::advance() {
    if (!isAtEnd()) {
        if (source[position] == '\n') {
            line++;
            column = 1;
        } else {
            column++;
        }
        position++;
    }
}

void Lexer::skipWhitespace() {
    while (!isAtEnd() && isWhitespace(getCurrentChar()) && getCurrentChar() != '\n') {
        advance();
    }
}

void Lexer::skipNewline() {
    while (!isAtEnd() && getCurrentChar() == '\n') {
        advance();
    }
}

Token Lexer::getNextToken() {
    skipWhitespace();
    
    if (isAtEnd()) {
        return makeToken(TokenType::EOF_TOKEN, "", column);
    }
    
    char current = getCurrentChar();
    
    // 处理换行
    if (current == '\n') {
        Token token = makeToken(TokenType::NEWLINE, "\n", column);
        advance();
        return token;
    }
    
    // 处理注释
    if (current == '/' && (peekChar() == '/' || peekChar() == '*')) {
        return scanComment();
    }
    
    // 处理CHTL选择器
    if (current == '{' && peekChar() == '{') {
        return scanCHTLSelector();
    }
    
    // 处理字符串
    if (current == '"' || current == '\'' || current == '`') {
        return scanString();
    }
    
    // 处理数字
    if (isDigit(current)) {
        return scanNumber();
    }
    
    // 处理标识符和关键字
    if (isAlpha(current) || current == '_' || current == '$') {
        return scanIdentifierOrKeyword();
    }
    
    // 处理操作符和符号
    return scanOperator();
}

Token Lexer::scanIdentifierOrKeyword() {
    int startCol = column;
    std::string value;
    
    while (!isAtEnd() && (isAlphaNumeric(getCurrentChar()) || 
                          getCurrentChar() == '_' || 
                          getCurrentChar() == '$')) {
        value += getCurrentChar();
        advance();
    }
    
    // 检查是否为关键字
    TokenType type = globalMap->getKeywordType(value);
    if (type != TokenType::UNKNOWN) {
        return makeToken(type, value, startCol);
    }
    
    // 检查是否为布尔字面量
    if (value == "true" || value == "false") {
        return makeToken(TokenType::BOOLEAN_LITERAL, value, startCol);
    }
    
    // 检查是否为null字面量
    if (value == "null") {
        return makeToken(TokenType::NULL_LITERAL, value, startCol);
    }
    
    return makeToken(TokenType::IDENTIFIER, value, startCol);
}

Token Lexer::scanString() {
    int startCol = column;
    char quote = getCurrentChar();
    std::string value;
    advance(); // 跳过开始引号
    
    while (!isAtEnd() && getCurrentChar() != quote) {
        if (getCurrentChar() == '\\') {
            advance();
            if (!isAtEnd()) {
                // 处理转义字符
                char escaped = getCurrentChar();
                switch (escaped) {
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
                    case '`': value += '`'; break;
                    default: value += escaped; break;
                }
                advance();
            }
        } else {
            value += getCurrentChar();
            advance();
        }
    }
    
    if (getCurrentChar() == quote) {
        advance(); // 跳过结束引号
    }
    
    return makeToken(TokenType::STRING_LITERAL, value, startCol);
}

Token Lexer::scanNumber() {
    int startCol = column;
    std::string value;
    
    // 整数部分
    while (!isAtEnd() && isDigit(getCurrentChar())) {
        value += getCurrentChar();
        advance();
    }
    
    // 小数部分
    if (getCurrentChar() == '.' && isDigit(peekChar())) {
        value += getCurrentChar();
        advance();
        
        while (!isAtEnd() && isDigit(getCurrentChar())) {
            value += getCurrentChar();
            advance();
        }
    }
    
    // 科学记数法
    if ((getCurrentChar() == 'e' || getCurrentChar() == 'E') &&
        (isDigit(peekChar()) || 
         ((peekChar() == '+' || peekChar() == '-') && isDigit(peekChar(2))))) {
        value += getCurrentChar();
        advance();
        
        if (getCurrentChar() == '+' || getCurrentChar() == '-') {
            value += getCurrentChar();
            advance();
        }
        
        while (!isAtEnd() && isDigit(getCurrentChar())) {
            value += getCurrentChar();
            advance();
        }
    }
    
    return makeToken(TokenType::NUMBER_LITERAL, value, startCol);
}

Token Lexer::scanOperator() {
    int startCol = column;
    char current = getCurrentChar();
    
    // 处理多字符操作符
    Token multiChar = scanMultiCharOperator(current);
    if (multiChar.type != TokenType::UNKNOWN) {
        return multiChar;
    }
    
    // 处理单字符操作符
    advance();
    switch (current) {
        case '+': return makeToken(TokenType::PLUS, "+", startCol);
        case '-': return makeToken(TokenType::MINUS, "-", startCol);
        case '*': return makeToken(TokenType::MULTIPLY, "*", startCol);
        case '/': return makeToken(TokenType::DIVIDE, "/", startCol);
        case '%': return makeToken(TokenType::MODULO, "%", startCol);
        case '(': return makeToken(TokenType::LEFT_PAREN, "(", startCol);
        case ')': return makeToken(TokenType::RIGHT_PAREN, ")", startCol);
        case '{': return makeToken(TokenType::LEFT_BRACE, "{", startCol);
        case '}': return makeToken(TokenType::RIGHT_BRACE, "}", startCol);
        case '[': return makeToken(TokenType::LEFT_BRACKET, "[", startCol);
        case ']': return makeToken(TokenType::RIGHT_BRACKET, "]", startCol);
        case ';': return makeToken(TokenType::SEMICOLON, ";", startCol);
        case ',': return makeToken(TokenType::COMMA, ",", startCol);
        case '.': return makeToken(TokenType::DOT, ".", startCol);
        case ':': return makeToken(TokenType::COLON, ":", startCol);
        case '?': return makeToken(TokenType::QUESTION, "?", startCol);
        case '~': return makeToken(TokenType::BITWISE_NOT, "~", startCol);
        default:
            return makeToken(TokenType::UNKNOWN, std::string(1, current), startCol);
    }
}

Token Lexer::scanMultiCharOperator(char first) {
    int startCol = column;
    
    switch (first) {
        case '+':
            if (peekChar() == '+') {
                advance(); advance();
                return makeToken(TokenType::INCREMENT, "++", startCol);
            } else if (peekChar() == '=') {
                advance(); advance();
                return makeToken(TokenType::PLUS_ASSIGN, "+=", startCol);
            }
            break;
            
        case '-':
            if (peekChar() == '-') {
                advance(); advance();
                return makeToken(TokenType::DECREMENT, "--", startCol);
            } else if (peekChar() == '=') {
                advance(); advance();
                return makeToken(TokenType::MINUS_ASSIGN, "-=", startCol);
            } else if (peekChar() == '>') {
                advance(); advance();
                return makeToken(TokenType::ARROW, "->", startCol);
            }
            break;
            
        case '*':
            if (peekChar() == '=') {
                advance(); advance();
                return makeToken(TokenType::MULTIPLY_ASSIGN, "*=", startCol);
            }
            break;
            
        case '/':
            if (peekChar() == '=') {
                advance(); advance();
                return makeToken(TokenType::DIVIDE_ASSIGN, "/=", startCol);
            }
            break;
            
        case '%':
            if (peekChar() == '=') {
                advance(); advance();
                return makeToken(TokenType::MODULO_ASSIGN, "%=", startCol);
            }
            break;
            
        case '=':
            if (peekChar() == '=') {
                if (peekChar(2) == '=') {
                    advance(); advance(); advance();
                    return makeToken(TokenType::STRICT_EQUALS, "===", startCol);
                } else {
                    advance(); advance();
                    return makeToken(TokenType::EQUALS, "==", startCol);
                }
            } else if (peekChar() == '>') {
                advance(); advance();
                return makeToken(TokenType::DOUBLE_ARROW, "=>", startCol);
            } else {
                advance();
                return makeToken(TokenType::ASSIGN, "=", startCol);
            }
            break;
            
        case '!':
            if (peekChar() == '=') {
                if (peekChar(2) == '=') {
                    advance(); advance(); advance();
                    return makeToken(TokenType::STRICT_NOT_EQUALS, "!==", startCol);
                } else {
                    advance(); advance();
                    return makeToken(TokenType::NOT_EQUALS, "!=", startCol);
                }
            } else {
                advance();
                return makeToken(TokenType::LOGICAL_NOT, "!", startCol);
            }
            break;
            
        case '<':
            if (peekChar() == '=') {
                advance(); advance();
                return makeToken(TokenType::LESS_EQUALS, "<=", startCol);
            } else if (peekChar() == '<') {
                advance(); advance();
                return makeToken(TokenType::LEFT_SHIFT, "<<", startCol);
            } else {
                advance();
                return makeToken(TokenType::LESS_THAN, "<", startCol);
            }
            break;
            
        case '>':
            if (peekChar() == '=') {
                advance(); advance();
                return makeToken(TokenType::GREATER_EQUALS, ">=", startCol);
            } else if (peekChar() == '>') {
                if (peekChar(2) == '>') {
                    advance(); advance(); advance();
                    return makeToken(TokenType::UNSIGNED_RIGHT_SHIFT, ">>>", startCol);
                } else {
                    advance(); advance();
                    return makeToken(TokenType::RIGHT_SHIFT, ">>", startCol);
                }
            } else {
                advance();
                return makeToken(TokenType::GREATER_THAN, ">", startCol);
            }
            break;
            
        case '&':
            if (peekChar() == '&') {
                advance(); advance();
                return makeToken(TokenType::LOGICAL_AND, "&&", startCol);
            } else if (peekChar() == '-' && peekChar(2) == '>') {
                advance(); advance(); advance();
                return makeToken(TokenType::BIND_ARROW, "&->", startCol);
            } else {
                advance();
                return makeToken(TokenType::BITWISE_AND, "&", startCol);
            }
            break;
            
        case '|':
            if (peekChar() == '|') {
                advance(); advance();
                return makeToken(TokenType::LOGICAL_OR, "||", startCol);
            } else {
                advance();
                return makeToken(TokenType::BITWISE_OR, "|", startCol);
            }
            break;
            
        case '^':
            advance();
            return makeToken(TokenType::BITWISE_XOR, "^", startCol);
            
        case '{':
            if (peekChar() == '{') {
                // 由scanCHTLSelector处理
                return makeToken(TokenType::UNKNOWN, "", startCol);
            }
            break;
            
        case '}':
            if (peekChar() == '}' && inCHTLSelector) {
                advance(); advance();
                inCHTLSelector = false;
                return makeToken(TokenType::DOUBLE_RIGHT_BRACE, "}}", startCol);
            }
            break;
    }
    
    return makeToken(TokenType::UNKNOWN, "", startCol);
}

Token Lexer::scanComment() {
    int startCol = column;
    std::string value;
    
    advance(); // 跳过第一个 /
    
    if (getCurrentChar() == '/') {
        // 单行注释
        advance(); // 跳过第二个 /
        
        while (!isAtEnd() && getCurrentChar() != '\n') {
            value += getCurrentChar();
            advance();
        }
    } else if (getCurrentChar() == '*') {
        // 多行注释
        advance(); // 跳过 *
        
        while (!isAtEnd()) {
            if (getCurrentChar() == '*' && peekChar() == '/') {
                advance(); // 跳过 *
                advance(); // 跳过 /
                break;
            }
            value += getCurrentChar();
            advance();
        }
    }
    
    return makeToken(TokenType::COMMENT, value, startCol);
}

Token Lexer::scanCHTLSelector() {
    int startCol = column;
    std::string value;
    
    advance(); // 跳过第一个 {
    advance(); // 跳过第二个 {
    
    inCHTLSelector = true;
    
    // 扫描选择器内容直到找到 }}
    while (!isAtEnd()) {
        if (getCurrentChar() == '}' && peekChar() == '}') {
            // 找到结束标记，但不在这里处理
            break;
        }
        value += getCurrentChar();
        advance();
    }
    
    return makeToken(TokenType::DOUBLE_LEFT_BRACE, value, startCol);
}

bool Lexer::isAtEnd() const {
    return position >= source.length();
}

bool Lexer::isAlpha(char c) const {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
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

bool Lexer::isOperatorStart(char c) const {
    return c == '+' || c == '-' || c == '*' || c == '/' || c == '%' ||
           c == '=' || c == '!' || c == '<' || c == '>' || c == '&' ||
           c == '|' || c == '^' || c == '~' || c == '?' || c == ':' ||
           c == '(' || c == ')' || c == '{' || c == '}' || c == '[' ||
           c == ']' || c == ';' || c == ',' || c == '.';
}

Token Lexer::makeToken(TokenType type, const std::string& value, int startColumn) {
    int length = column - startColumn;
    return Token(type, value, line, startColumn, length);
}

std::vector<Token> Lexer::tokenize() {
    tokens.clear();
    reset();
    
    while (!isAtEnd()) {
        Token token = getNextToken();
        tokens.push_back(token);
    }
    
    // 添加EOF token
    tokens.push_back(makeToken(TokenType::EOF_TOKEN, "", column));
    
    return tokens;
}

void Lexer::reset() {
    position = 0;
    line = 1;
    column = 1;
    tokens.clear();
    inCHTLSelector = false;
}

void Lexer::setSource(const std::string& src) {
    source = src;
    reset();
}

std::vector<Token> Lexer::getFilteredTokens() {
    std::vector<Token> filtered;
    
    for (const auto& token : tokens) {
        if (token.type != TokenType::WHITESPACE && 
            token.type != TokenType::NEWLINE &&
            token.type != TokenType::COMMENT) {
            filtered.push_back(token);
        }
    }
    
    return filtered;
}

std::string Lexer::getErrorContext() const {
    std::stringstream ss;
    ss << "Line " << line << ", Column " << column;
    
    // 获取当前行的内容
    size_t lineStart = position;
    while (lineStart > 0 && source[lineStart - 1] != '\n') {
        lineStart--;
    }
    
    size_t lineEnd = position;
    while (lineEnd < source.length() && source[lineEnd] != '\n') {
        lineEnd++;
    }
    
    if (lineStart < lineEnd) {
        ss << "\n" << source.substr(lineStart, lineEnd - lineStart);
        ss << "\n" << std::string(column - 1, ' ') << "^";
    }
    
    return ss.str();
}

} // namespace CHTLJS