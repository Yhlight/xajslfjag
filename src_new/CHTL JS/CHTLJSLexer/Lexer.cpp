#include "Lexer.h"
#include "GlobalMap.h"

namespace CHTLJS {

Lexer::Lexer(const std::string& source,
             CHTLJSStateMachine* stateMachine,
             CHTLJSContext* context,
             ErrorReporter* errorReporter)
    : m_source(source), m_position(0), m_line(1), m_column(1),
      m_state(LexerInternalState::NORMAL),
      m_stateMachine(stateMachine), m_context(context), m_errorReporter(errorReporter) {
}

TokenSequence Lexer::tokenize() {
    TokenSequence tokens;
    
    while (!isAtEnd()) {
        Token token = nextToken();
        if (token.type != TokenType::UNKNOWN_TOKEN) {
            tokens.push_back(token);
        }
    }
    
    // 添加EOF token
    tokens.emplace_back(TokenType::EOF_TOKEN, "", getCurrentPosition());
    
    return tokens;
}

Token Lexer::nextToken() {
    skipWhitespace();
    
    if (isAtEnd()) {
        return Token(TokenType::EOF_TOKEN, "", getCurrentPosition());
    }
    
    return scanToken();
}

Token Lexer::peekToken() {
    size_t savedPos = m_position;
    size_t savedLine = m_line;
    size_t savedColumn = m_column;
    LexerInternalState savedState = m_state;
    
    Token token = nextToken();
    
    m_position = savedPos;
    m_line = savedLine;
    m_column = savedColumn;
    m_state = savedState;
    
    return token;
}

bool Lexer::isAtEnd() const {
    return m_position >= m_source.length();
}

Position Lexer::getCurrentPosition() const {
    return Position(m_line, m_column, m_position);
}

void Lexer::reset(const std::string& source) {
    m_source = source;
    m_position = 0;
    m_line = 1;
    m_column = 1;
    m_state = LexerInternalState::NORMAL;
}

Token Lexer::scanToken() {
    char c = currentChar();
    Position pos = getCurrentPosition();
    
    switch (c) {
        case '{':
            // 检查是否为增强选择器开始 {{
            if (peekChar() == '{') {
                nextChar(); // 跳过第二个{
                nextChar();
                return Token(TokenType::ENHANCED_SELECTOR_START, "{{", pos);
            }
            nextChar();
            return Token(TokenType::LEFT_BRACE, "{", pos);
            
        case '}':
            // 检查是否为增强选择器结束 }}
            if (peekChar() == '}') {
                nextChar(); // 跳过第二个}
                nextChar();
                return Token(TokenType::ENHANCED_SELECTOR_END, "}}", pos);
            }
            nextChar();
            return Token(TokenType::RIGHT_BRACE, "}", pos);
            
        case '[':
            nextChar();
            return Token(TokenType::LEFT_BRACKET, "[", pos);
            
        case ']':
            nextChar();
            return Token(TokenType::RIGHT_BRACKET, "]", pos);
            
        case '(':
            nextChar();
            return Token(TokenType::LEFT_PAREN, "(", pos);
            
        case ')':
            nextChar();
            return Token(TokenType::RIGHT_PAREN, ")", pos);
            
        case ';':
            nextChar();
            return Token(TokenType::SEMICOLON, ";", pos);
            
        case ':':
            nextChar();
            return Token(TokenType::COLON, ":", pos);
            
        case '=':
            nextChar();
            return Token(TokenType::EQUAL, "=", pos);
            
        case ',':
            nextChar();
            return Token(TokenType::COMMA, ",", pos);
            
        case '.':
            nextChar();
            return Token(TokenType::DOT, ".", pos);
            
        case '-':
            if (peekChar() == '>') {
                nextChar(); // 跳过>
                nextChar();
                return Token(TokenType::ARROW, "->", pos);
            }
            if (peekChar() == '-') {
                nextChar(); // 跳过第二个-
                nextChar();
                return Token(TokenType::GENERATOR_COMMENT, "--", pos);
            }
            nextChar();
            return Token(TokenType::IDENTIFIER, "-", pos);
            
        case '&':
            if (peekChar() == '-' && m_source.length() > m_position + 2 && m_source[m_position + 2] == '>') {
                nextChar(); // 跳过-
                nextChar(); // 跳过>
                nextChar();
                return Token(TokenType::EVENT_BINDING, "&->", pos);
            }
            nextChar();
            return Token(TokenType::AMPERSAND, "&", pos);
            
        case '/':
            if (peekChar() == '/') {
                // 行注释
                return scanComment();
            } else if (peekChar() == '*') {
                // 块注释
                return scanComment();
            }
            nextChar();
            return Token(TokenType::IDENTIFIER, "/", pos);
            
        case '"':
            return scanString('"');
            
        case '\'':
            return scanString('\'');
            
        case '#':
            // ID选择器
            nextChar();
            std::string idName = "";
            while (!isAtEnd() && (isAlphaNumeric(currentChar()) || currentChar() == '-' || currentChar() == '_')) {
                idName += currentChar();
                nextChar();
            }
            return Token(TokenType::ID_SELECTOR, "#" + idName, pos);
            
        case '.':
            // 类选择器
            nextChar();
            std::string className = "";
            while (!isAtEnd() && (isAlphaNumeric(currentChar()) || currentChar() == '-' || currentChar() == '_')) {
                className += currentChar();
                nextChar();
            }
            return Token(TokenType::CLASS_SELECTOR, "." + className, pos);
            
        default:
            if (isAlpha(c)) {
                return scanIdentifierOrKeyword();
            } else if (isDigit(c)) {
                return scanNumber();
            } else {
                nextChar();
                return Token(TokenType::UNKNOWN_TOKEN, std::string(1, c), pos);
            }
    }
}

Token Lexer::scanString(char quote) {
    Position pos = getCurrentPosition();
    nextChar(); // 跳过开始引号
    
    std::string value = "";
    
    while (!isAtEnd() && currentChar() != quote) {
        if (currentChar() == '\\' && !isAtEnd()) {
            nextChar(); // 跳过转义字符
            if (!isAtEnd()) {
                char escaped = currentChar();
                switch (escaped) {
                    case 'n': value += '\n'; break;
                    case 't': value += '\t'; break;
                    case 'r': value += '\r'; break;
                    case '\\': value += '\\'; break;
                    case '"': value += '"'; break;
                    case '\'': value += '\''; break;
                    default: value += escaped; break;
                }
                nextChar();
            }
        } else {
            value += currentChar();
            nextChar();
        }
    }
    
    if (isAtEnd()) {
        reportError("Unterminated string");
        return Token(TokenType::INVALID_TOKEN, value, pos);
    }
    
    nextChar(); // 跳过结束引号
    
    TokenType type = (quote == '"') ? TokenType::DOUBLE_QUOTED_STRING : TokenType::SINGLE_QUOTED_STRING;
    return Token(type, value, pos);
}

Token Lexer::scanComment() {
    Position pos = getCurrentPosition();
    char first = currentChar();
    char second = peekChar();
    
    if (first == '/' && second == '/') {
        // 行注释
        nextChar(); // 跳过第一个/
        nextChar(); // 跳过第二个/
        
        std::string comment = "";
        while (!isAtEnd() && currentChar() != '\n') {
            comment += currentChar();
            nextChar();
        }
        
        return Token(TokenType::LINE_COMMENT, comment, pos);
    } else if (first == '/' && second == '*') {
        // 块注释
        nextChar(); // 跳过/
        nextChar(); // 跳过*
        
        std::string comment = "";
        while (!isAtEnd()) {
            if (currentChar() == '*' && peekChar() == '/') {
                nextChar(); // 跳过*
                nextChar(); // 跳过/
                break;
            }
            comment += currentChar();
            nextChar();
        }
        
        return Token(TokenType::BLOCK_COMMENT, comment, pos);
    }
    
    return Token(TokenType::UNKNOWN_TOKEN, "", pos);
}

Token Lexer::scanEnhancedSelector() {
    Position pos = getCurrentPosition();
    
    // 已经在{{中，扫描到}}
    std::string selector = "";
    while (!isAtEnd()) {
        if (currentChar() == '}' && peekChar() == '}') {
            nextChar(); // 跳过第一个}
            nextChar(); // 跳过第二个}
            break;
        }
        selector += currentChar();
        nextChar();
    }
    
    return Token(TokenType::IDENTIFIER, selector, pos); // 增强选择器内容作为标识符
}

Token Lexer::scanIdentifierOrKeyword() {
    Position pos = getCurrentPosition();
    std::string value = "";
    
    while (!isAtEnd() && (isAlphaNumeric(currentChar()) || currentChar() == '_')) {
        value += currentChar();
        nextChar();
    }
    
    // 检查是否为CHTL JS关键字
    if (CHTLJS_GLOBAL_MAP.isCHTLJSKeyword(value)) {
        if (value == "script") return Token(TokenType::SCRIPT, value, pos);
        else if (value == "module") return Token(TokenType::MODULE, value, pos);
        else if (value == "load") return Token(TokenType::LOAD, value, pos);
        else if (value == "listen") return Token(TokenType::LISTEN, value, pos);
        else if (value == "delegate") return Token(TokenType::DELEGATE, value, pos);
        else if (value == "animate") return Token(TokenType::ANIMATE, value, pos);
        else if (value == "vir") return Token(TokenType::VIR, value, pos);
    }
    
    // 检查是否为JavaScript事件
    if (CHTLJS_GLOBAL_MAP.isJSEvent(value)) {
        if (value == "click") return Token(TokenType::CLICK, value, pos);
        else if (value == "mouseenter") return Token(TokenType::MOUSEENTER, value, pos);
        else if (value == "mouseleave") return Token(TokenType::MOUSELEAVE, value, pos);
        else if (value == "keydown") return Token(TokenType::KEYDOWN, value, pos);
        else if (value == "keyup") return Token(TokenType::KEYUP, value, pos);
        else if (value == "submit") return Token(TokenType::SUBMIT, value, pos);
        else if (value == "change") return Token(TokenType::CHANGE, value, pos);
        else if (value == "focus") return Token(TokenType::FOCUS, value, pos);
        else if (value == "blur") return Token(TokenType::BLUR, value, pos);
    }
    
    return Token(TokenType::IDENTIFIER, value, pos);
}

Token Lexer::scanNumber() {
    Position pos = getCurrentPosition();
    std::string value = "";
    
    while (!isAtEnd() && (isDigit(currentChar()) || currentChar() == '.')) {
        value += currentChar();
        nextChar();
    }
    
    return Token(TokenType::UNQUOTED_LITERAL, value, pos);
}

char Lexer::currentChar() const {
    if (isAtEnd()) return '\0';
    return m_source[m_position];
}

char Lexer::nextChar() {
    if (isAtEnd()) return '\0';
    
    char c = m_source[m_position++];
    if (c == '\n') {
        m_line++;
        m_column = 1;
    } else {
        m_column++;
    }
    
    return c;
}

char Lexer::peekChar() const {
    if (m_position + 1 >= m_source.length()) return '\0';
    return m_source[m_position + 1];
}

void Lexer::skipWhitespace() {
    while (!isAtEnd()) {
        char c = currentChar();
        if (c == ' ' || c == '\t' || c == '\r' || c == '\n') {
            nextChar();
        } else {
            break;
        }
    }
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

void Lexer::reportError(const std::string& message) {
    if (m_errorReporter) {
        m_errorReporter->error(ErrorType::SYNTAX_ERROR, message, "", m_line, m_column);
    }
}

} // namespace CHTLJS