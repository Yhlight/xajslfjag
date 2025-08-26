#include "CHTLJSLexer.h"
#include <cctype>
#include <algorithm>
#include <regex>

namespace CHTLJS {

CHTLJSLexer::CHTLJSLexer(const std::string& sourceCode) 
    : source(sourceCode), position(0), line(1), column(1), start(0),
      currentState(CHTLJSLexerState::Normal), templateNestingLevel(0), canBeRegex(true) {
}

void CHTLJSLexer::reset(const std::string& newSource) {
    if (!newSource.empty()) {
        source = newSource;
    }
    position = 0;
    line = 1;
    column = 1;
    start = 0;
    errors.clear();
    currentState = CHTLJSLexerState::Normal;
    stateStack.clear();
    templateNestingLevel = 0;
    canBeRegex = true;
}

std::vector<CHTLJSToken> CHTLJSLexer::tokenize() {
    std::vector<CHTLJSToken> tokens;
    
    while (!isAtEnd()) {
        start = position;
        CHTLJSToken token = nextToken();
        
        if (token.type != CHTLJSTokenType::INVALID) {
            tokens.push_back(token);
        }
    }
    
    // 添加EOF token
    tokens.emplace_back(CHTLJSTokenType::EOF_TOKEN, "", line, column, position, 0);
    
    return tokens;
}

CHTLJSToken CHTLJSLexer::nextToken() {
    // 跳过空白字符
    skipWhitespace();
    
    if (isAtEnd()) {
        return makeToken(CHTLJSTokenType::EOF_TOKEN);
    }
    
    start = position;
    
    return scanToken();
}

CHTLJSToken CHTLJSLexer::scanToken() {
    char c = advance();
    
    // 处理当前状态
    switch (currentState) {
        case CHTLJSLexerState::InStringLiteral:
            return scanStringLiteral(c);
        case CHTLJSLexerState::InTemplateLiteral:
            return scanTemplateLiteral();
        case CHTLJSLexerState::InRegexLiteral:
            return scanRegexLiteral();
        case CHTLJSLexerState::InLineComment:
            return scanLineComment();
        case CHTLJSLexerState::InBlockComment:
            return scanBlockComment();
        case CHTLJSLexerState::InJSDocComment:
            return scanJSDocComment();
        default:
            break;
    }
    
    // 数字
    if (isDigit(c)) {
        return scanNumber();
    }
    
    // 标识符和关键字
    if (isIdentifierStart(c)) {
        return scanIdentifier();
    }
    
    // 字符串字面量
    if (c == '"' || c == '\'') {
        return scanStringLiteral(c);
    }
    
    // 模板字面量
    if (c == '`') {
        return scanTemplateLiteral();
    }
    
    // 注释
    if (c == '/') {
        if (peek() == '/') {
            return scanLineComment();
        } else if (peek() == '*') {
            char next = peek(1);
            if (next == '*') {
                return scanJSDocComment();
            } else {
                return scanBlockComment();
            }
        } else if (canBeRegex) {
            return scanRegexLiteral();
        }
    }
    
    // 单字符操作符和符号
    switch (c) {
        case '(': canBeRegex = true; return makeToken(CHTLJSTokenType::LPAREN);
        case ')': canBeRegex = false; return makeToken(CHTLJSTokenType::RPAREN);
        case '{': canBeRegex = true; return makeToken(CHTLJSTokenType::LBRACE);
        case '}': canBeRegex = false; return makeToken(CHTLJSTokenType::RBRACE);
        case '[': canBeRegex = true; return makeToken(CHTLJSTokenType::LBRACKET);
        case ']': canBeRegex = false; return makeToken(CHTLJSTokenType::RBRACKET);
        case ';': canBeRegex = true; return makeToken(CHTLJSTokenType::SEMICOLON);
        case ',': canBeRegex = true; return makeToken(CHTLJSTokenType::COMMA);
        case '?':
            if (peek() == '.') {
                advance();
                return makeToken(CHTLJSTokenType::CHAIN_OP, "?.");
            } else if (peek() == '?') {
                advance();
                return makeToken(CHTLJSTokenType::NULL_COALESCE, "??");
            }
            return makeToken(CHTLJSTokenType::QUESTION);
        case ':':
            if (peek() == ':') {
                advance();
                return makeToken(CHTLJSTokenType::BIND_OP, "::");
            }
            return makeToken(CHTLJSTokenType::COLON);
        case '@': return makeToken(CHTLJSTokenType::AT_SYMBOL);
        case '#': return makeToken(CHTLJSTokenType::HASH);
        case '$': return makeToken(CHTLJSTokenType::DOLLAR);
        case '%': return makeToken(CHTLJSTokenType::PERCENT);
        default:
            break;
    }
    
    // 多字符操作符
    if (isOperatorStart(c)) {
        return scanOperator();
    }
    
    addError("意外的字符: " + std::string(1, c));
    return makeErrorToken("意外的字符");
}

CHTLJSToken CHTLJSLexer::scanStringLiteral(char quote) {
    std::string value;
    
    while (!isAtEnd() && peek() != quote) {
        char c = peek();
        
        if (c == '\n') {
            addError("字符串字面量中不能包含换行符");
            return makeErrorToken("字符串错误");
        }
        
        if (c == '\\') {
            advance(); // 跳过反斜杠
            char escaped = processEscapeSequence();
            value += escaped;
        } else {
            value += advance();
        }
    }
    
    if (isAtEnd()) {
        addError("未结束的字符串字面量");
        return makeErrorToken("字符串错误");
    }
    
    // 消费结束引号
    advance();
    canBeRegex = false;
    
    return makeToken(CHTLJSTokenType::STRING_LITERAL, value);
}

CHTLJSToken CHTLJSLexer::scanTemplateLiteral() {
    std::string value;
    
    while (!isAtEnd()) {
        char c = peek();
        
        if (c == '`') {
            advance();
            canBeRegex = false;
            return makeToken(CHTLJSTokenType::TEMPLATE_LITERAL, value);
        }
        
        if (c == '$' && peek(1) == '{') {
            // 模板表达式开始
            if (!value.empty()) {
                return makeToken(CHTLJSTokenType::TEMPLATE_MIDDLE, value);
            }
            advance(); // $
            advance(); // {
            enterTemplateExpression();
            return makeToken(CHTLJSTokenType::TEMPLATE_EXPR_START, "${");
        }
        
        if (c == '\\') {
            advance();
            char escaped = processEscapeSequence();
            value += escaped;
        } else {
            if (c == '\n') {
                line++;
                column = 1;
            }
            value += advance();
        }
    }
    
    addError("未结束的模板字面量");
    return makeErrorToken("模板字面量错误");
}

CHTLJSToken CHTLJSLexer::scanNumber() {
    // 处理不同进制
    if (peek(-1) == '0') {
        char next = peek();
        if (next == 'x' || next == 'X') {
            return scanHexNumber();
        } else if (next == 'b' || next == 'B') {
            return scanBinaryNumber();
        } else if (next == 'o' || next == 'O') {
            return scanOctalNumber();
        }
    }
    
    return scanDecimalNumber();
}

CHTLJSToken CHTLJSLexer::scanDecimalNumber() {
    // 扫描整数部分
    while (isDigit(peek())) {
        advance();
    }
    
    // 处理小数点
    if (peek() == '.' && isDigit(peek(1))) {
        advance(); // 消费.
        while (isDigit(peek())) {
            advance();
        }
    }
    
    // 处理科学计数法
    if (peek() == 'e' || peek() == 'E') {
        advance();
        if (peek() == '+' || peek() == '-') {
            advance();
        }
        if (!isDigit(peek())) {
            addError("无效的科学计数法");
            return makeErrorToken("数字错误");
        }
        while (isDigit(peek())) {
            advance();
        }
    }
    
    // 检查BigInt后缀
    if (peek() == 'n') {
        advance();
        canBeRegex = false;
        return makeToken(CHTLJSTokenType::BIGINT, getCurrentLexeme());
    }
    
    canBeRegex = false;
    return makeToken(CHTLJSTokenType::NUMBER, getCurrentLexeme());
}

CHTLJSToken CHTLJSLexer::scanHexNumber() {
    advance(); // 消费x或X
    
    if (!isHexDigit(peek())) {
        addError("十六进制数字必须包含至少一位数字");
        return makeErrorToken("数字错误");
    }
    
    while (isHexDigit(peek())) {
        advance();
    }
    
    if (peek() == 'n') {
        advance();
        return makeToken(CHTLJSTokenType::BIGINT, getCurrentLexeme());
    }
    
    canBeRegex = false;
    return makeToken(CHTLJSTokenType::NUMBER, getCurrentLexeme());
}

CHTLJSToken CHTLJSLexer::scanBinaryNumber() {
    advance(); // 消费b或B
    
    if (!isBinaryDigit(peek())) {
        addError("二进制数字必须包含至少一位数字");
        return makeErrorToken("数字错误");
    }
    
    while (isBinaryDigit(peek())) {
        advance();
    }
    
    if (peek() == 'n') {
        advance();
        return makeToken(CHTLJSTokenType::BIGINT, getCurrentLexeme());
    }
    
    canBeRegex = false;
    return makeToken(CHTLJSTokenType::NUMBER, getCurrentLexeme());
}

CHTLJSToken CHTLJSLexer::scanOctalNumber() {
    advance(); // 消费o或O
    
    if (!isOctalDigit(peek())) {
        addError("八进制数字必须包含至少一位数字");
        return makeErrorToken("数字错误");
    }
    
    while (isOctalDigit(peek())) {
        advance();
    }
    
    if (peek() == 'n') {
        advance();
        return makeToken(CHTLJSTokenType::BIGINT, getCurrentLexeme());
    }
    
    canBeRegex = false;
    return makeToken(CHTLJSTokenType::NUMBER, getCurrentLexeme());
}

CHTLJSToken CHTLJSLexer::scanIdentifier() {
    while (isIdentifierPart(peek())) {
        advance();
    }
    
    std::string text = getCurrentLexeme();
    
    // 检查是否为关键字
    CHTLJSTokenType type = CHTLJSTokenUtils::stringToTokenType(text);
    
    if (type != CHTLJSTokenType::IDENTIFIER) {
        canBeRegex = (type == CHTLJSTokenType::RETURN || 
                     type == CHTLJSTokenType::THROW ||
                     type == CHTLJSTokenType::CASE ||
                     type == CHTLJSTokenType::IN);
        return makeToken(type, text);
    }
    
    // 处理上下文相关的CHTL JS关键字
    return handleContextualKeyword(text);
}

CHTLJSToken CHTLJSLexer::scanOperator() {
    char first = peek(-1);
    
    switch (first) {
        case '+':
            if (peek() == '+') {
                advance();
                canBeRegex = true;
                return makeToken(CHTLJSTokenType::INCREMENT, "++");
            } else if (peek() == '=') {
                advance();
                canBeRegex = true;
                return makeToken(CHTLJSTokenType::PLUS_ASSIGN, "+=");
            }
            canBeRegex = true;
            return makeToken(CHTLJSTokenType::PLUS, "+");
            
        case '-':
            if (peek() == '-') {
                advance();
                canBeRegex = true;
                return makeToken(CHTLJSTokenType::DECREMENT, "--");
            } else if (peek() == '=') {
                advance();
                canBeRegex = true;
                return makeToken(CHTLJSTokenType::MINUS_ASSIGN, "-=");
            }
            canBeRegex = true;
            return makeToken(CHTLJSTokenType::MINUS, "-");
            
        case '*':
            if (peek() == '*') {
                advance();
                if (peek() == '=') {
                    advance();
                    return makeToken(CHTLJSTokenType::EXPONENT, "**=");
                }
                return makeToken(CHTLJSTokenType::EXPONENT, "**");
            } else if (peek() == '=') {
                advance();
                return makeToken(CHTLJSTokenType::MULTIPLY_ASSIGN, "*=");
            }
            canBeRegex = true;
            return makeToken(CHTLJSTokenType::MULTIPLY, "*");
            
        case '/':
            if (peek() == '=') {
                advance();
                canBeRegex = true;
                return makeToken(CHTLJSTokenType::DIVIDE_ASSIGN, "/=");
            }
            canBeRegex = true;
            return makeToken(CHTLJSTokenType::DIVIDE, "/");
            
        case '%':
            if (peek() == '=') {
                advance();
                canBeRegex = true;
                return makeToken(CHTLJSTokenType::MODULO_ASSIGN, "%=");
            }
            canBeRegex = true;
            return makeToken(CHTLJSTokenType::MODULO, "%");
            
        case '=':
            if (peek() == '=') {
                advance();
                if (peek() == '=') {
                    advance();
                    canBeRegex = true;
                    return makeToken(CHTLJSTokenType::STRICT_EQUAL, "===");
                }
                canBeRegex = true;
                return makeToken(CHTLJSTokenType::EQUAL, "==");
            } else if (peek() == '>') {
                advance();
                canBeRegex = true;
                return makeToken(CHTLJSTokenType::ARROW, "=>");
            }
            canBeRegex = true;
            return makeToken(CHTLJSTokenType::ASSIGN, "=");
            
        case '!':
            if (peek() == '=') {
                advance();
                if (peek() == '=') {
                    advance();
                    canBeRegex = true;
                    return makeToken(CHTLJSTokenType::STRICT_NOT_EQUAL, "!==");
                }
                canBeRegex = true;
                return makeToken(CHTLJSTokenType::NOT_EQUAL, "!=");
            }
            canBeRegex = true;
            return makeToken(CHTLJSTokenType::LOGICAL_NOT, "!");
            
        case '<':
            if (peek() == '<') {
                advance();
                return makeToken(CHTLJSTokenType::LEFT_SHIFT, "<<");
            } else if (peek() == '=') {
                advance();
                canBeRegex = true;
                return makeToken(CHTLJSTokenType::LESS_EQUAL, "<=");
            }
            canBeRegex = true;
            return makeToken(CHTLJSTokenType::LESS_THAN, "<");
            
        case '>':
            if (peek() == '>') {
                advance();
                if (peek() == '>') {
                    advance();
                    return makeToken(CHTLJSTokenType::UNSIGNED_RIGHT_SHIFT, ">>>");
                }
                return makeToken(CHTLJSTokenType::RIGHT_SHIFT, ">>");
            } else if (peek() == '=') {
                advance();
                canBeRegex = true;
                return makeToken(CHTLJSTokenType::GREATER_EQUAL, ">=");
            }
            canBeRegex = true;
            return makeToken(CHTLJSTokenType::GREATER_THAN, ">");
            
        case '&':
            if (peek() == '&') {
                advance();
                canBeRegex = true;
                return makeToken(CHTLJSTokenType::LOGICAL_AND, "&&");
            }
            canBeRegex = true;
            return makeToken(CHTLJSTokenType::BITWISE_AND, "&");
            
        case '|':
            if (peek() == '|') {
                advance();
                canBeRegex = true;
                return makeToken(CHTLJSTokenType::LOGICAL_OR, "||");
            } else if (peek() == '>') {
                advance();
                canBeRegex = true;
                return makeToken(CHTLJSTokenType::PIPE_OP, "|>");
            }
            canBeRegex = true;
            return makeToken(CHTLJSTokenType::BITWISE_OR, "|");
            
        case '^':
            canBeRegex = true;
            return makeToken(CHTLJSTokenType::BITWISE_XOR, "^");
            
        case '~':
            canBeRegex = true;
            return makeToken(CHTLJSTokenType::BITWISE_NOT, "~");
            
        case '.':
            if (peek() == '.') {
                advance();
                if (peek() == '.') {
                    advance();
                    return makeToken(CHTLJSTokenType::SPREAD_OP, "...");
                }
                return makeToken(CHTLJSTokenType::RANGE_OP, "..");
            }
            canBeRegex = false;
            return makeToken(CHTLJSTokenType::DOT, ".");
            
        default:
            break;
    }
    
    return makeErrorToken("未知操作符");
}

CHTLJSToken CHTLJSLexer::scanRegexLiteral() {
    std::string pattern = scanRegexPattern();
    std::string flags = scanRegexFlags();
    
    canBeRegex = false;
    return makeToken(CHTLJSTokenType::REGEX_LITERAL, "/" + pattern + "/" + flags);
}

std::string CHTLJSLexer::scanRegexPattern() {
    std::string pattern;
    
    while (!isAtEnd() && peek() != '/') {
        char c = peek();
        
        if (c == '\n') {
            addError("正则表达式中不能包含换行符");
            break;
        }
        
        if (c == '\\') {
            pattern += advance(); // 反斜杠
            if (!isAtEnd()) {
                pattern += advance(); // 被转义的字符
            }
        } else {
            pattern += advance();
        }
    }
    
    if (!isAtEnd()) {
        advance(); // 消费结束的/
    }
    
    return pattern;
}

std::string CHTLJSLexer::scanRegexFlags() {
    std::string flags;
    
    while (!isAtEnd() && isValidRegexFlag(peek())) {
        flags += advance();
    }
    
    return flags;
}

CHTLJSToken CHTLJSLexer::scanLineComment() {
    std::string comment;
    
    if (peek(-1) == '/' && peek() == '/') {
        advance(); // 消费第二个/
    }
    
    while (!isAtEnd() && !isNewline(peek())) {
        comment += advance();
    }
    
    canBeRegex = true;
    return makeToken(CHTLJSTokenType::LINE_COMMENT, comment);
}

CHTLJSToken CHTLJSLexer::scanBlockComment() {
    std::string comment;
    
    if (peek(-1) == '/' && peek() == '*') {
        advance(); // 消费*
    }
    
    while (!isAtEnd()) {
        if (peek() == '*' && peek(1) == '/') {
            advance(); // 消费*
            advance(); // 消费/
            break;
        }
        
        char c = advance();
        if (c == '\n') {
            line++;
            column = 1;
        }
        comment += c;
    }
    
    canBeRegex = true;
    return makeToken(CHTLJSTokenType::BLOCK_COMMENT, comment);
}

CHTLJSToken CHTLJSLexer::scanJSDocComment() {
    std::string comment;
    
    if (peek(-1) == '/' && peek() == '*' && peek(1) == '*') {
        advance(); // 消费第一个*
        advance(); // 消费第二个*
    }
    
    while (!isAtEnd()) {
        if (peek() == '*' && peek(1) == '/') {
            advance(); // 消费*
            advance(); // 消费/
            break;
        }
        
        char c = advance();
        if (c == '\n') {
            line++;
            column = 1;
        }
        comment += c;
    }
    
    canBeRegex = true;
    return makeToken(CHTLJSTokenType::JSDOC_COMMENT, comment);
}

// === 字符操作实现 ===

char CHTLJSLexer::peek(size_t offset) const {
    size_t pos = position + offset;
    if (pos >= source.length()) {
        return '\0';
    }
    return source[pos];
}

char CHTLJSLexer::advance() {
    if (isAtEnd()) {
        return '\0';
    }
    
    char c = source[position++];
    if (c == '\n') {
        line++;
        column = 1;
    } else {
        column++;
    }
    
    return c;
}

bool CHTLJSLexer::isAtEnd() const {
    return position >= source.length();
}

bool CHTLJSLexer::match(char expected) {
    if (isAtEnd() || source[position] != expected) {
        return false;
    }
    
    position++;
    column++;
    return true;
}

// === 字符分类实现 ===

bool CHTLJSLexer::isAlpha(char c) const {
    return std::isalpha(c) || c == '_' || c == '$';
}

bool CHTLJSLexer::isAlphaNumeric(char c) const {
    return std::isalnum(c) || c == '_' || c == '$';
}

bool CHTLJSLexer::isDigit(char c) const {
    return std::isdigit(c);
}

bool CHTLJSLexer::isHexDigit(char c) const {
    return std::isxdigit(c);
}

bool CHTLJSLexer::isBinaryDigit(char c) const {
    return c == '0' || c == '1';
}

bool CHTLJSLexer::isOctalDigit(char c) const {
    return c >= '0' && c <= '7';
}

bool CHTLJSLexer::isWhitespace(char c) const {
    return c == ' ' || c == '\t' || c == '\r';
}

bool CHTLJSLexer::isNewline(char c) const {
    return c == '\n';
}

bool CHTLJSLexer::isIdentifierStart(char c) const {
    return std::isalpha(c) || c == '_' || c == '$';
}

bool CHTLJSLexer::isIdentifierPart(char c) const {
    return std::isalnum(c) || c == '_' || c == '$';
}

bool CHTLJSLexer::isOperatorStart(char c) const {
    return c == '+' || c == '-' || c == '*' || c == '/' || c == '%' ||
           c == '=' || c == '!' || c == '<' || c == '>' ||
           c == '&' || c == '|' || c == '^' || c == '~' || c == '.';
}

bool CHTLJSLexer::isValidRegexFlag(char c) const {
    return c == 'g' || c == 'i' || c == 'm' || c == 's' || c == 'u' || c == 'y';
}

// === 工具方法实现 ===

CHTLJSToken CHTLJSLexer::makeToken(CHTLJSTokenType type) {
    return makeToken(type, getCurrentLexeme());
}

CHTLJSToken CHTLJSLexer::makeToken(CHTLJSTokenType type, const std::string& value) {
    size_t length = position - start;
    return CHTLJSToken(type, value, line, column - length, start, length);
}

CHTLJSToken CHTLJSLexer::makeErrorToken(const std::string& message) {
    size_t length = position - start;
    return CHTLJSToken(CHTLJSTokenType::INVALID, message, line, column - length, start, length);
}

std::string CHTLJSLexer::getCurrentLexeme() const {
    if (start >= source.length() || position > source.length()) {
        return "";
    }
    return source.substr(start, position - start);
}

void CHTLJSLexer::skipWhitespace() {
    while (!isAtEnd()) {
        char c = peek();
        if (isWhitespace(c)) {
            advance();
        } else if (isNewline(c)) {
            advance();
        } else {
            break;
        }
    }
}

void CHTLJSLexer::addError(const std::string& message) {
    errors.push_back("行 " + std::to_string(line) + "，列 " + std::to_string(column) + ": " + message);
}

char CHTLJSLexer::processEscapeSequence() {
    if (isAtEnd()) {
        addError("未完成的转义序列");
        return '\0';
    }
    
    char c = advance();
    switch (c) {
        case 'n': return '\n';
        case 't': return '\t';
        case 'r': return '\r';
        case 'b': return '\b';
        case 'f': return '\f';
        case 'v': return '\v';
        case '0': return '\0';
        case '\\': return '\\';
        case '\'': return '\'';
        case '"': return '"';
        case '/': return '/';
        default:
            addError("无效的转义序列: \\" + std::string(1, c));
            return c;
    }
}

CHTLJSToken CHTLJSLexer::handleContextualKeyword(const std::string& identifier) {
    // 检查是否在特定上下文中
    if (isSelectorContext() && CHTLJSTokenUtils::isSelectorKeyword(identifier)) {
        return resolveCHTLJSKeyword(identifier);
    }
    
    if (isAnimationContext() && CHTLJSTokenUtils::isAnimationKeyword(identifier)) {
        return resolveCHTLJSKeyword(identifier);
    }
    
    if (isVirtualObjectContext() && CHTLJSTokenUtils::isVirtualObjectKeyword(identifier)) {
        return resolveCHTLJSKeyword(identifier);
    }
    
    if (isEventDelegationContext() && (CHTLJSTokenUtils::isListenerKeyword(identifier) || CHTLJSTokenUtils::isEventType(identifier))) {
        return resolveCHTLJSKeyword(identifier);
    }
    
    canBeRegex = false;
    return makeToken(CHTLJSTokenType::IDENTIFIER, identifier);
}

CHTLJSToken CHTLJSLexer::resolveCHTLJSKeyword(const std::string& keyword) {
    CHTLJSTokenType type = CHTLJSTokenUtils::stringToTokenType(keyword);
    canBeRegex = false;
    return makeToken(type, keyword);
}

// === 上下文检测实现 ===

bool CHTLJSLexer::isSelectorContext() const {
    // 简化的上下文检测 - 实际实现需要更复杂的逻辑
    return false;
}

bool CHTLJSLexer::isAnimationContext() const {
    return false;
}

bool CHTLJSLexer::isVirtualObjectContext() const {
    return false;
}

bool CHTLJSLexer::isEventDelegationContext() const {
    return false;
}

void CHTLJSLexer::enterTemplateExpression() {
    templateNestingLevel++;
    pushState(CHTLJSLexerState::InTemplateExpression);
}

void CHTLJSLexer::exitTemplateExpression() {
    if (templateNestingLevel > 0) {
        templateNestingLevel--;
    }
    popState();
}

bool CHTLJSLexer::isInTemplateExpression() const {
    return currentState == CHTLJSLexerState::InTemplateExpression;
}

// === 状态管理实现 ===

void CHTLJSLexer::pushState(CHTLJSLexerState state) {
    stateStack.push_back(currentState);
    currentState = state;
}

void CHTLJSLexer::popState() {
    if (!stateStack.empty()) {
        currentState = stateStack.back();
        stateStack.pop_back();
    } else {
        currentState = CHTLJSLexerState::Normal;
    }
}

CHTLJSLexerState CHTLJSLexer::getCurrentState() const {
    return currentState;
}

} // namespace CHTLJS