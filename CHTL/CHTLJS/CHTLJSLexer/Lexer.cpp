#include "Lexer.h"
#include <cctype>
#include <sstream>

namespace CHTLJS {

Lexer::Lexer(const std::string& source, std::shared_ptr<GlobalMap> globalMap)
    : source(source), globalMap(globalMap), position(0), line(1), column(1), 
      debugMode(false), inEnhancedSelector(false) {
}

Lexer::~Lexer() = default;

Token Lexer::NextToken() {
    if (!tokenBuffer.empty()) {
        Token token = tokenBuffer.front();
        tokenBuffer.erase(tokenBuffer.begin());
        return token;
    }
    
    SkipWhitespace();
    
    if (IsEOF()) {
        return Token(TokenType::EOF_TOKEN, "", line, column, position);
    }
    
    int startLine = line;
    int startColumn = column;
    int startPosition = position;
    
    char current = CurrentChar();
    
    // 处理注释
    if (current == '/' && PeekChar() == '/') {
        return ReadComment();
    }
    
    if (current == '/' && PeekChar() == '*') {
        return ReadComment();
    }
    
    // 处理CHTL JS特殊操作符
    if (current == '{' && PeekChar() == '{') {
        return ReadDoubleLeftBrace();
    }
    
    if (current == '}' && PeekChar() == '}') {
        Advance(); Advance();
        inEnhancedSelector = false;
        return Token(TokenType::DOUBLE_RIGHT_BRACE, "}}", startLine, startColumn, startPosition);
    }
    
    if (current == '-' && PeekChar() == '>') {
        return ReadArrowOperator();
    }
    
    if (current == '&' && PeekChar() == '-' && PeekChar(2) == '>') {
        Advance(); Advance(); Advance();
        return Token(TokenType::AMPERSAND_ARROW, "&->", startLine, startColumn, startPosition);
    }
    
    if (current == '*' && PeekChar() == '*') {
        Advance(); Advance();
        return Token(TokenType::DOUBLE_STAR, "**", startLine, startColumn, startPosition);
    }
    
    if (current == '=' && PeekChar() == '>') {
        Advance(); Advance();
        return Token(TokenType::ARROW_FUNCTION, "=>", startLine, startColumn, startPosition);
    }
    
    // 处理单字符Token
    switch (current) {
        case '{':
            Advance();
            return Token(TokenType::LEFT_BRACE, "{", startLine, startColumn, startPosition);
        case '}':
            Advance();
            return Token(TokenType::RIGHT_BRACE, "}", startLine, startColumn, startPosition);
        case '[':
            Advance();
            return Token(TokenType::LEFT_BRACKET, "[", startLine, startColumn, startPosition);
        case ']':
            Advance();
            return Token(TokenType::RIGHT_BRACKET, "]", startLine, startColumn, startPosition);
        case '(':
            Advance();
            return Token(TokenType::LEFT_PAREN, "(", startLine, startColumn, startPosition);
        case ')':
            Advance();
            return Token(TokenType::RIGHT_PAREN, ")", startLine, startColumn, startPosition);
        case ';':
            Advance();
            return Token(TokenType::SEMICOLON, ";", startLine, startColumn, startPosition);
        case ':':
            Advance();
            return Token(TokenType::COLON, ":", startLine, startColumn, startPosition);
        case '=':
            Advance();
            return Token(TokenType::EQUALS, "=", startLine, startColumn, startPosition);
        case ',':
            Advance();
            return Token(TokenType::COMMA, ",", startLine, startColumn, startPosition);
        case '.':
            Advance();
            return Token(TokenType::DOT, ".", startLine, startColumn, startPosition);
        case '+':
            Advance();
            return Token(TokenType::PLUS, "+", startLine, startColumn, startPosition);
        case '-':
            Advance();
            return Token(TokenType::MINUS, "-", startLine, startColumn, startPosition);
        case '*':
            Advance();
            return Token(TokenType::STAR, "*", startLine, startColumn, startPosition);
        case '/':
            Advance();
            return Token(TokenType::SLASH, "/", startLine, startColumn, startPosition);
        case '%':
            Advance();
            return Token(TokenType::PERCENT, "%", startLine, startColumn, startPosition);
        case '"':
        case '\'':
            return ReadStringLiteral(current);
        case '\n':
            SkipNewline();
            return Token(TokenType::NEWLINE, "\\n", startLine, startColumn, startPosition);
    }
    
    // 处理数字
    if (std::isdigit(current)) {
        return ReadNumber();
    }
    
    // 处理标识符和关键字
    if (IsValidIdentifierStart(current)) {
        return ReadIdentifier();
    }
    
    // 处理无引号字面量（在CHTL JS函数内部）
    if (IsValidUnquotedChar(current)) {
        return ReadUnquotedLiteral();
    }
    
    // 无效字符
    Advance();
    ReportError("无效字符: " + std::string(1, current));
    return Token(TokenType::INVALID, std::string(1, current), startLine, startColumn, startPosition);
}

Token Lexer::PeekToken() {
    return PeekToken(0);
}

Token Lexer::PeekToken(int offset) {
    while (tokenBuffer.size() <= static_cast<size_t>(offset)) {
        int savedPosition = position;
        int savedLine = line;
        int savedColumn = column;
        bool savedInEnhancedSelector = inEnhancedSelector;
        
        Token token = NextToken();
        tokenBuffer.push_back(token);
        
        if (tokenBuffer.size() == 1) {
            position = savedPosition;
            line = savedLine;
            column = savedColumn;
            inEnhancedSelector = savedInEnhancedSelector;
        }
    }
    
    return tokenBuffer[offset];
}

bool Lexer::IsEOF() const {
    return position >= static_cast<int>(source.length());
}

void Lexer::ReportError(const std::string& message) {
    std::stringstream ss;
    ss << "CHTL JS词法错误 [" << line << ":" << column << "]: " << message;
    errors.push_back(ss.str());
}

char Lexer::CurrentChar() const {
    if (IsEOF()) return '\0';
    return source[position];
}

char Lexer::PeekChar(int offset) const {
    int pos = position + offset;
    if (pos >= static_cast<int>(source.length())) return '\0';
    return source[pos];
}

void Lexer::Advance() {
    if (!IsEOF()) {
        if (source[position] == '\n') {
            line++;
            column = 1;
        } else {
            column++;
        }
        position++;
    }
}

void Lexer::SkipWhitespace() {
    while (!IsEOF() && (CurrentChar() == ' ' || CurrentChar() == '\t' || CurrentChar() == '\r')) {
        Advance();
    }
}

void Lexer::SkipNewline() {
    while (!IsEOF() && CurrentChar() == '\n') {
        Advance();
    }
}

Token Lexer::ReadIdentifier() {
    int startLine = line;
    int startColumn = column;
    int startPosition = position;
    std::string value;
    
    while (!IsEOF() && IsValidIdentifierChar(CurrentChar())) {
        value += CurrentChar();
        Advance();
    }
    
    TokenType type = DetermineIdentifierType(value);
    return Token(type, value, startLine, startColumn, startPosition);
}

Token Lexer::ReadNumber() {
    int startLine = line;
    int startColumn = column;
    int startPosition = position;
    std::string value;
    
    while (!IsEOF() && (std::isdigit(CurrentChar()) || CurrentChar() == '.')) {
        value += CurrentChar();
        Advance();
    }
    
    return Token(TokenType::NUMBER, value, startLine, startColumn, startPosition);
}

Token Lexer::ReadStringLiteral(char quote) {
    int startLine = line;
    int startColumn = column;
    int startPosition = position;
    std::string value;
    
    Advance(); // 跳过开始引号
    
    while (!IsEOF() && CurrentChar() != quote) {
        if (CurrentChar() == '\\') {
            Advance();
            if (!IsEOF()) {
                char escaped = CurrentChar();
                switch (escaped) {
                    case 'n': value += '\n'; break;
                    case 't': value += '\t'; break;
                    case 'r': value += '\r'; break;
                    case '\\': value += '\\'; break;
                    case '"': value += '"'; break;
                    case '\'': value += '\''; break;
                    default: value += escaped; break;
                }
                Advance();
            }
        } else {
            value += CurrentChar();
            Advance();
        }
    }
    
    if (CurrentChar() == quote) {
        Advance();
    } else {
        ReportError("未闭合的字符串字面量");
    }
    
    return Token(TokenType::STRING_LITERAL, value, startLine, startColumn, startPosition);
}

Token Lexer::ReadUnquotedLiteral() {
    int startLine = line;
    int startColumn = column;
    int startPosition = position;
    std::string value;
    
    while (!IsEOF() && IsValidUnquotedChar(CurrentChar()) && 
           CurrentChar() != ',' && CurrentChar() != ':' && 
           CurrentChar() != ';' && CurrentChar() != '}') {
        value += CurrentChar();
        Advance();
    }
    
    // 去除尾部空格
    while (!value.empty() && value.back() == ' ') {
        value.pop_back();
    }
    
    return Token(TokenType::UNQUOTED_LITERAL, value, startLine, startColumn, startPosition);
}

Token Lexer::ReadComment() {
    int startLine = line;
    int startColumn = column;
    int startPosition = position;
    std::string value;
    
    if (CurrentChar() == '/' && PeekChar() == '/') {
        Advance(); Advance();
        while (!IsEOF() && CurrentChar() != '\n') {
            value += CurrentChar();
            Advance();
        }
        return Token(TokenType::SINGLE_COMMENT, value, startLine, startColumn, startPosition);
    } else if (CurrentChar() == '/' && PeekChar() == '*') {
        Advance(); Advance();
        while (!IsEOF() && !(CurrentChar() == '*' && PeekChar() == '/')) {
            value += CurrentChar();
            Advance();
        }
        if (CurrentChar() == '*' && PeekChar() == '/') {
            Advance(); Advance();
        } else {
            ReportError("未闭合的多行注释");
        }
        return Token(TokenType::MULTI_COMMENT, value, startLine, startColumn, startPosition);
    }
    
    return Token(TokenType::INVALID, "", startLine, startColumn, startPosition);
}

Token Lexer::ReadDoubleLeftBrace() {
    int startLine = line;
    int startColumn = column;
    int startPosition = position;
    
    Advance(); Advance(); // 跳过 {{
    inEnhancedSelector = true;
    
    return Token(TokenType::DOUBLE_LEFT_BRACE, "{{", startLine, startColumn, startPosition);
}

Token Lexer::ReadArrowOperator() {
    int startLine = line;
    int startColumn = column;
    int startPosition = position;
    
    Advance(); Advance(); // 跳过 ->
    
    return Token(TokenType::ARROW, "->", startLine, startColumn, startPosition);
}

bool Lexer::IsValidIdentifierStart(char c) const {
    return std::isalpha(c) || c == '_' || c == '$';
}

bool Lexer::IsValidIdentifierChar(char c) const {
    return std::isalnum(c) || c == '_' || c == '$';
}

bool Lexer::IsValidUnquotedChar(char c) const {
    // 无引号字面量允许的字符（在CHTL JS函数内部）
    return std::isalnum(c) || c == '_' || c == '-' || c == '.' || 
           c == ' ' || c == '#' || c == '%' || c == '!';
}

TokenType Lexer::DetermineIdentifierType(const std::string& value) {
    return TokenUtils::StringToTokenType(value);
}

} // namespace CHTLJS