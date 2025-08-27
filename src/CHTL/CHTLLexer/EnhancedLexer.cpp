#include "EnhancedLexer.h"
#include <cctype>
#include <algorithm>
#include <unordered_set>
#include <regex>

namespace CHTL {

EnhancedLexer::EnhancedLexer(std::shared_ptr<GlobalMap> gMap)
    : position(0), currentLine(1), currentColumn(1), isAnalyzed(false) {
    globalMap = gMap ? gMap : std::make_shared<GlobalMap>();
    contextStack.push(LexerContext::GLOBAL);
}

EnhancedLexer::~EnhancedLexer() = default;

void EnhancedLexer::SetSource(const std::string& sourceCode) {
    source = sourceCode;
    Reset();
}

void EnhancedLexer::Analyze() {
    if (isAnalyzed) {
        return;
    }
    
    Reset();
    
    while (!IsEOF()) {
        SkipWhitespace();
        
        if (IsEOF()) {
            break;
        }
        
        try {
            Token token = ReadNextToken();
            if (token.type != TokenType::UNKNOWN) {
                AddToken(token);
            }
        } catch (const std::exception& e) {
            AddError("词法分析异常: " + std::string(e.what()));
            // 跳过当前字符，继续分析
            if (!IsEOF()) {
                GetChar();
            }
        }
    }
    
    // 添加EOF token
    AddToken(Token(TokenType::EOF_TOKEN, "", GetCurrentPosition()));
    
    isAnalyzed = true;
}

const std::vector<Token>& EnhancedLexer::GetTokens() const {
    return tokenBuffer;
}

const std::vector<std::string>& EnhancedLexer::GetErrors() const {
    return errors;
}

// === 基础字符处理 ===

char EnhancedLexer::PeekChar() const {
    return (position < source.length()) ? source[position] : '\0';
}

char EnhancedLexer::GetChar() {
    if (IsEOF()) {
        return '\0';
    }
    
    char c = source[position++];
    UpdatePosition(c);
    return c;
}

bool EnhancedLexer::IsEOF() const {
    return position >= source.length();
}

void EnhancedLexer::SkipWhitespace() {
    while (!IsEOF() && IsWhitespace(PeekChar()) && !IsNewline(PeekChar())) {
        GetChar();
    }
}

void EnhancedLexer::UpdatePosition(char c) {
    if (c == '\n') {
        currentLine++;
        currentColumn = 1;
    } else {
        currentColumn++;
    }
}

// === 上下文管理 ===

LexerContext EnhancedLexer::GetCurrentContext() const {
    return contextStack.empty() ? LexerContext::GLOBAL : contextStack.top();
}

void EnhancedLexer::PushContext(LexerContext context) {
    contextStack.push(context);
}

void EnhancedLexer::PopContext() {
    if (contextStack.size() > 1) {
        contextStack.pop();
    }
}

void EnhancedLexer::DetectContextChange(const std::string& identifier) {
    if (identifier == "style") {
        PushContext(LexerContext::STYLE_BLOCK);
    } else if (identifier == "script") {
        PushContext(LexerContext::SCRIPT_BLOCK);
    } else if (identifier == "text") {
        PushContext(LexerContext::TEXT_BLOCK);
    }
}

// === Token读取方法 ===

Token EnhancedLexer::ReadNextToken() {
    char c = PeekChar();
    
    // 处理换行符
    if (IsNewline(c)) {
        TokenPosition pos = GetCurrentPosition();
        GetChar();
        return Token(TokenType::NEWLINE, "\n", pos);
    }
    
    // 处理注释
    if (c == '/' && position + 1 < source.length()) {
        char next = source[position + 1];
        if (next == '/' || next == '*') {
            return ReadComment();
        }
    }
    
    // 处理生成器注释
    if (c == '-' && position + 1 < source.length() && source[position + 1] == '-') {
        TokenPosition pos = GetCurrentPosition();
        GetChar(); GetChar(); // 跳过 '--'
        
        std::string value;
        while (!IsEOF() && !IsNewline(PeekChar())) {
            value += GetChar();
        }
        
        return Token(TokenType::COMMENT_GENERATOR, value, pos);
    }
    
    // 根据当前上下文选择合适的读取方法
    LexerContext context = GetCurrentContext();
    
    switch (context) {
        case LexerContext::STYLE_BLOCK:
            return ReadStyleToken();
        case LexerContext::SCRIPT_BLOCK:
            return ReadScriptToken();
        case LexerContext::TEXT_BLOCK:
            return ReadTextToken();
        default:
            return ReadGlobalToken();
    }
}

Token EnhancedLexer::ReadGlobalToken() {
    char c = PeekChar();
    
    // 处理字符串字面量
    if (c == '"' || c == '\'') {
        return ReadStringLiteral(c);
    }
    
    // 处理数字
    if (IsDigit(c)) {
        return ReadNumber();
    }
    
    // 处理标识符和关键字
    if (IsAlpha(c) || c == '_') {
        return ReadIdentifier();
    }
    
    // 处理括号类型 [Template], [Custom] 等
    if (c == '[') {
        return ReadBracketType();
    }
    
    // 处理类型标识符 @Style, @Element 等
    if (c == '@') {
        return ReadTypeIdentifier();
    }
    
    // 处理符号
    return ReadSymbol();
}

Token EnhancedLexer::ReadStyleToken() {
    char c = PeekChar();
    
    // 在样式块中，优先处理CSS语法
    
    // 处理选择器（以.、#、&开头）
    if (c == '.' || c == '#' || c == '&') {
        return ReadCssSelector();
    }
    
    // 处理颜色值（以#开头的十六进制）
    if (c == '#' && position + 1 < source.length() && IsHexDigit(source[position + 1])) {
        return ReadColorValue();
    }
    
    // 处理百分比
    if (IsDigit(c)) {
        Token numToken = ReadNumber();
        // 检查是否后跟%
        if (!IsEOF() && PeekChar() == '%') {
            GetChar(); // 消费%
            return Token(TokenType::UNQUOTED_LITERAL, numToken.value + "%", numToken.position);
        }
        return numToken;
    }
    
    // 处理CSS属性名（可能包含连字符）
    if (IsAlpha(c)) {
        std::string identifier;
        TokenPosition startPos = GetCurrentPosition();
        
        while (!IsEOF() && IsCssIdentifierChar(PeekChar())) {
            identifier += GetChar();
        }
        
        // 检查是否为CSS属性
        if (IsCssProperty(identifier)) {
            return Token(TokenType::IDENTIFIER, identifier, startPos);
        }
        
        // 检查是否为关键字
        TokenType type = ClassifyIdentifier(identifier);
        return Token(type, identifier, startPos);
    }
    
    // 处理字符串
    if (c == '"' || c == '\'') {
        return ReadStringLiteral(c);
    }
    
    // 处理CSS值（无引号）
    if (IsCssValueChar(c)) {
        return ReadCssValue();
    }
    
    // 处理符号
    return ReadSymbol();
}

Token EnhancedLexer::ReadScriptToken() {
    // 在脚本块中，基本按照JavaScript语法处理
    return ReadGlobalToken();
}

Token EnhancedLexer::ReadTextToken() {
    char c = PeekChar();
    
    // 在文本块中，优先处理无修饰字面量
    if (c == '"' || c == '\'') {
        return ReadStringLiteral(c);
    }
    
    // 无修饰字面量：除了保留字符外的所有字符
    if (IsUnquotedLiteralChar(c)) {
        return ReadUnquotedLiteral();
    }
    
    return ReadSymbol();
}

Token EnhancedLexer::ReadIdentifier() {
    std::string identifier;
    TokenPosition startPos = GetCurrentPosition();
    
    while (!IsEOF() && (IsAlphaNumeric(PeekChar()) || PeekChar() == '_' || PeekChar() == '-')) {
        identifier += GetChar();
    }
    
    // 检测上下文变化
    DetectContextChange(identifier);
    
    // 分类标识符
    TokenType type = ClassifyIdentifier(identifier);
    return Token(type, identifier, startPos);
}

Token EnhancedLexer::ReadCssSelector() {
    std::string selector;
    TokenPosition startPos = GetCurrentPosition();
    
    char c = GetChar(); // 获取第一个字符（.、#、&）
    selector += c;
    
    if (c == '&') {
        // 处理&引用选择器
        if (!IsEOF() && PeekChar() == ':') {
            selector += GetChar(); // ':'
            // 读取伪类名
            while (!IsEOF() && IsAlphaNumeric(PeekChar())) {
                selector += GetChar();
            }
        }
    } else {
        // 处理类选择器或ID选择器
        while (!IsEOF() && (IsAlphaNumeric(PeekChar()) || PeekChar() == '-' || PeekChar() == '_')) {
            selector += GetChar();
        }
    }
    
    return Token(TokenType::IDENTIFIER, selector, startPos);
}

Token EnhancedLexer::ReadCssValue() {
    std::string value;
    TokenPosition startPos = GetCurrentPosition();
    
    while (!IsEOF() && IsCssValueChar(PeekChar())) {
        char c = PeekChar();
        
        // 遇到分隔符时停止
        if (c == ';' || c == '}' || c == '{' || IsWhitespace(c)) {
            break;
        }
        
        value += GetChar();
    }
    
    return Token(TokenType::UNQUOTED_LITERAL, value, startPos);
}

Token EnhancedLexer::ReadNumber() {
    std::string value;
    TokenPosition startPos = GetCurrentPosition();
    
    // 读取整数部分
    while (!IsEOF() && IsDigit(PeekChar())) {
        value += GetChar();
    }
    
    // 处理小数点
    if (!IsEOF() && PeekChar() == '.') {
        char next = (position + 1 < source.length()) ? source[position + 1] : '\0';
        if (IsDigit(next)) {
            value += GetChar(); // 小数点
            while (!IsEOF() && IsDigit(PeekChar())) {
                value += GetChar();
            }
        }
    }
    
    // 检查CSS单位
    if (!IsEOF() && IsAlpha(PeekChar())) {
        std::string unit;
        while (!IsEOF() && IsAlpha(PeekChar())) {
            unit += GetChar();
        }
        
        // 常见CSS单位
        static std::unordered_set<std::string> cssUnits = {
            "px", "em", "rem", "pt", "pc", "in", "cm", "mm", "ex", "ch",
            "vw", "vh", "vmin", "vmax", "fr", "deg", "rad", "grad", "turn",
            "s", "ms", "Hz", "kHz", "dpi", "dpcm", "dppx"
        };
        
        if (cssUnits.count(unit)) {
            value += unit;
        } else {
            // 不是CSS单位，回退
            for (int i = unit.length() - 1; i >= 0; --i) {
                position--;
                if (currentColumn > 1) {
                    currentColumn--;
                }
            }
        }
    }
    
    return Token(TokenType::NUMBER, value, startPos);
}

Token EnhancedLexer::ReadColorValue() {
    std::string value;
    TokenPosition startPos = GetCurrentPosition();
    
    value += GetChar(); // '#'
    
    // 读取十六进制颜色值
    while (!IsEOF() && IsHexDigit(PeekChar()) && value.length() < 7) {
        value += GetChar();
    }
    
    return Token(TokenType::UNQUOTED_LITERAL, value, startPos);
}

Token EnhancedLexer::ReadStringLiteral(char quote) {
    std::string value;
    TokenPosition startPos = GetCurrentPosition();
    
    GetChar(); // 跳过开始引号
    
    while (!IsEOF() && PeekChar() != quote) {
        char c = GetChar();
        if (c == '\\' && !IsEOF()) {
            // 处理转义字符
            char escaped = GetChar();
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
    
    if (!IsEOF()) {
        GetChar(); // 跳过结束引号
    } else {
        AddError("未闭合的字符串字面量");
    }
    
    return Token(TokenType::STRING_LITERAL, value, startPos);
}

Token EnhancedLexer::ReadUnquotedLiteral() {
    std::string value;
    TokenPosition startPos = GetCurrentPosition();
    
    while (!IsEOF() && IsUnquotedLiteralChar(PeekChar())) {
        char c = PeekChar();
        
        // 遇到CHTL结构字符时停止
        if (c == '{' || c == '}' || c == ';' || IsWhitespace(c) || IsNewline(c)) {
            break;
        }
        
        value += GetChar();
    }
    
    return Token(TokenType::UNQUOTED_LITERAL, value, startPos);
}

Token EnhancedLexer::ReadComment() {
    GetChar(); // '/'
    char next = GetChar();
    
    TokenPosition startPos = GetCurrentPosition();
    std::string value;
    
    if (next == '/') {
        // 单行注释
        while (!IsEOF() && !IsNewline(PeekChar())) {
            value += GetChar();
        }
        return Token(TokenType::COMMENT_LINE, value, startPos);
    } else if (next == '*') {
        // 块注释
        while (!IsEOF()) {
            char c = GetChar();
            if (c == '*' && !IsEOF() && PeekChar() == '/') {
                GetChar(); // 跳过 '/'
                break;
            }
            value += c;
        }
        return Token(TokenType::COMMENT_BLOCK, value, startPos);
    }
    
    // 不是注释，回退
    position -= 2;
    currentColumn -= 2;
    return ReadSymbol();
}

Token EnhancedLexer::ReadSymbol() {
    char c = GetChar();
    TokenPosition startPos = GetCurrentPosition();
    
    // 处理双字符符号
    if (!IsEOF()) {
        std::string twoChar = std::string(1, c) + PeekChar();
        
        // 检查是否为双字符符号
        if (twoChar == "==" || twoChar == "!=" || twoChar == "<=" || twoChar == ">=" ||
            twoChar == "&&" || twoChar == "||" || twoChar == "++" || twoChar == "--" ||
            twoChar == "->" || twoChar == "::") {
            GetChar(); // 消费第二个字符
            return Token(TokenType::UNKNOWN, twoChar, startPos); // 暂时使用UNKNOWN，稍后可扩展TokenType
        }
    }
    
    // 单字符符号
    std::string symbol(1, c);
    TokenType type = TokenType::UNKNOWN;
    
    switch (c) {
        case '{': 
            if (GetCurrentContext() != LexerContext::GLOBAL) {
                PushContext(LexerContext::ELEMENT_CONTENT);
            }
            type = TokenType::LEFT_BRACE; 
            break;
        case '}': 
            PopContext();
            type = TokenType::RIGHT_BRACE; 
            break;
        case '[': type = TokenType::LEFT_BRACKET; break;
        case ']': type = TokenType::RIGHT_BRACKET; break;
        case '(': type = TokenType::LEFT_PAREN; break;
        case ')': type = TokenType::RIGHT_PAREN; break;
        case ';': type = TokenType::SEMICOLON; break;
        case ':': type = TokenType::COLON; break;
        case '=': type = TokenType::EQUALS; break;
        case ',': type = TokenType::COMMA; break;
        case '.': type = TokenType::DOT; break;
        case '+': type = TokenType::PLUS; break;
        case '-': type = TokenType::MINUS; break;
        case '*': type = TokenType::STAR; break;
        case '/': type = TokenType::SLASH; break;
        case '%': type = TokenType::UNKNOWN; break; // 百分比符号，暂时使用UNKNOWN
        case '!': type = TokenType::UNKNOWN; break; // 感叹号，暂时使用UNKNOWN
        case '?': type = TokenType::UNKNOWN; break; // 问号，暂时使用UNKNOWN
        case '<': type = TokenType::UNKNOWN; break; // 小于号，暂时使用UNKNOWN
        case '>': type = TokenType::UNKNOWN; break; // 大于号，暂时使用UNKNOWN
        case '&': type = TokenType::AMPERSAND; break;
        case '|': type = TokenType::UNKNOWN; break; // 管道符，暂时使用UNKNOWN
        case '^': type = TokenType::UNKNOWN; break; // 异或符，暂时使用UNKNOWN
        case '~': type = TokenType::UNKNOWN; break; // 波浪符，暂时使用UNKNOWN
        case '@': type = TokenType::UNKNOWN; break; // @符号，可能需要特殊处理
        case '#': type = TokenType::HASH; break;
        default: type = TokenType::UNKNOWN; break;
    }
    
    return Token(type, symbol, startPos);
}

Token EnhancedLexer::ReadBracketType() {
    std::string value;
    TokenPosition startPos = GetCurrentPosition();
    
    // 读取整个[xxx]结构
    while (!IsEOF() && PeekChar() != ']') {
        value += GetChar();
    }
    
    if (!IsEOF() && PeekChar() == ']') {
        value += GetChar(); // 添加']'
    }
    
    // 查找对应的Token类型
    TokenType type = globalMap->LookupBracketType(value);
    if (type == TokenType::UNKNOWN) {
        type = TokenType::LEFT_BRACKET; // 如果不是预定义的括号类型，当作普通左括号处理
        // 回退除了第一个'['之外的所有字符
        for (int i = value.length() - 1; i > 0; --i) {
            position--;
            if (currentColumn > 1) {
                currentColumn--;
            }
        }
        value = "[";
    }
    
    return Token(type, value, startPos);
}

Token EnhancedLexer::ReadTypeIdentifier() {
    std::string value;
    TokenPosition startPos = GetCurrentPosition();
    
    value += GetChar(); // 添加'@'
    
    // 读取类型名称
    while (!IsEOF() && IsAlphaNumeric(PeekChar())) {
        value += GetChar();
    }
    
    // 查找对应的Token类型
    TokenType type = globalMap->LookupTypeIdentifier(value);
    if (type == TokenType::UNKNOWN) {
        type = TokenType::UNKNOWN; // @开头但不是预定义类型
    }
    
    return Token(type, value, startPos);
}

// === 字符分类方法 ===

bool EnhancedLexer::IsAlpha(char c) const {
    return std::isalpha(c) || c == '_';
}

bool EnhancedLexer::IsDigit(char c) const {
    return std::isdigit(c);
}

bool EnhancedLexer::IsAlphaNumeric(char c) const {
    return std::isalnum(c) || c == '_';
}

bool EnhancedLexer::IsWhitespace(char c) const {
    return c == ' ' || c == '\t' || c == '\r';
}

bool EnhancedLexer::IsNewline(char c) const {
    return c == '\n';
}

bool EnhancedLexer::IsHexDigit(char c) const {
    return std::isxdigit(c);
}

bool EnhancedLexer::IsCssIdentifierChar(char c) const {
    return IsAlphaNumeric(c) || c == '-' || c == '_';
}

bool EnhancedLexer::IsUnquotedLiteralChar(char c) const {
    // 无修饰字面量可以包含大多数字符，除了CHTL结构字符
    return c != '{' && c != '}' && c != '[' && c != ']' && 
           c != ';' && c != '"' && c != '\'' && 
           !IsWhitespace(c) && !IsNewline(c) && c != '\0';
}

bool EnhancedLexer::IsCssValueChar(char c) const {
    return IsAlphaNumeric(c) || c == '-' || c == '_' || c == '.' || 
           c == '#' || c == '%' || c == '+' || c == '(' || c == ')' ||
           c == ',' || c == '/' || c == '*';
}

// === Token分类和验证 ===

TokenType EnhancedLexer::ClassifyIdentifier(const std::string& identifier) const {
    // 首先检查全局映射表
    TokenType type = globalMap->LookupKeyword(identifier);
    if (type != TokenType::UNKNOWN) {
        return type;
    }
    
    // 检查是否为HTML元素（简化处理）
    // 常见HTML元素直接作为标识符处理
    static std::unordered_set<std::string> htmlElements = {
        "html", "head", "body", "title", "div", "span", "p", "h1", "h2", "h3",
        "h4", "h5", "h6", "a", "img", "ul", "ol", "li", "table", "tr", "td",
        "th", "form", "input", "button", "select", "option", "textarea"
    };
    
    if (htmlElements.count(identifier)) {
        return TokenType::IDENTIFIER; // HTML元素作为标识符处理
    }
    
    return TokenType::IDENTIFIER;
}

bool EnhancedLexer::IsCssProperty(const std::string& name) const {
    // 常见CSS属性列表
    static std::unordered_set<std::string> cssProperties = {
        "width", "height", "margin", "padding", "border", "background", "color",
        "font-size", "font-family", "font-weight", "text-align", "line-height",
        "display", "position", "top", "left", "right", "bottom", "float",
        "clear", "overflow", "z-index", "opacity", "visibility",
        "background-color", "background-image", "background-repeat",
        "background-position", "background-size", "border-radius",
        "box-shadow", "text-shadow", "transform", "transition",
        "cursor", "pointer-events", "user-select", "max-width", "min-width",
        "max-height", "min-height", "border-color", "border-width", "border-style"
    };
    
    return cssProperties.count(name) > 0;
}

bool EnhancedLexer::IsCssSelector(const std::string& value) const {
    return !value.empty() && (value[0] == '.' || value[0] == '#' || value[0] == '&');
}

bool EnhancedLexer::IsColorValue(const std::string& value) const {
    if (value.empty()) return false;
    if (value[0] == '#') {
        return value.length() == 4 || value.length() == 7; // #RGB 或 #RRGGBB
    }
    return false;
}

bool EnhancedLexer::HasCssUnit(const std::string& value) const {
    if (value.length() < 3) return false;
    
    std::string suffix = value.substr(value.length() - 2);
    static std::unordered_set<std::string> units = {
        "px", "em", "rem", "pt", "pc", "in", "cm", "mm", "ex", "ch",
        "vw", "vh", "%"
    };
    
    return units.count(suffix) > 0;
}

// === 辅助方法 ===

void EnhancedLexer::AddToken(const Token& token) {
    tokenBuffer.push_back(token);
}

void EnhancedLexer::AddError(const std::string& message) {
    std::string fullMessage = "第" + std::to_string(currentLine) + 
                             "行:" + std::to_string(currentColumn) + 
                             " " + message;
    errors.push_back(fullMessage);
}

void EnhancedLexer::Reset() {
    position = 0;
    currentLine = 1;
    currentColumn = 1;
    tokenBuffer.clear();
    errors.clear();
    isAnalyzed = false;
    
    // 重置上下文栈
    while (!contextStack.empty()) {
        contextStack.pop();
    }
    contextStack.push(LexerContext::GLOBAL);
}

TokenPosition EnhancedLexer::GetCurrentPosition() const {
    return TokenPosition(currentLine, currentColumn, position);
}

std::string EnhancedLexer::PeekString(size_t length) const {
    if (position + length > source.length()) {
        length = source.length() - position;
    }
    return source.substr(position, length);
}

void EnhancedLexer::SkipChars(size_t count) {
    for (size_t i = 0; i < count && !IsEOF(); ++i) {
        GetChar();
    }
}

bool EnhancedLexer::ValidateTokenSequence() const {
    // 基础验证：检查括号匹配
    int braceCount = 0, bracketCount = 0, parenCount = 0;
    
    for (const auto& token : tokenBuffer) {
        switch (token.type) {
            case TokenType::LEFT_BRACE: braceCount++; break;
            case TokenType::RIGHT_BRACE: braceCount--; break;
            case TokenType::LEFT_BRACKET: bracketCount++; break;
            case TokenType::RIGHT_BRACKET: bracketCount--; break;
            case TokenType::LEFT_PAREN: parenCount++; break;
            case TokenType::RIGHT_PAREN: parenCount--; break;
            default: break;
        }
    }
    
    return braceCount == 0 && bracketCount == 0 && parenCount == 0;
}

} // namespace CHTL