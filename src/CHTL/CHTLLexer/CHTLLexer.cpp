#include "CHTLLexer.h"
#include <cctype>
#include <sstream>

namespace CHTL {

CHTLLexer::CHTLLexer(const std::string& input)
    : input_(input), position_(0), line_(1), column_(1), 
      debugMode_(false), globalMap_(CHTLGlobalMap::Instance()) {
    globalMap_.Initialize();
}

void CHTLLexer::SetInput(const std::string& input) {
    input_ = input;
    position_ = 0;
    line_ = 1;
    column_ = 1;
    errors_.clear();
}

void CHTLLexer::SetInput(std::istream& stream) {
    std::ostringstream buffer;
    buffer << stream.rdbuf();
    SetInput(buffer.str());
}

std::vector<CHTLTokenPtr> CHTLLexer::Tokenize() {
    std::vector<CHTLTokenPtr> tokens;
    
    while (!IsAtEnd()) {
        auto token = NextToken();
        if (token && token->type != CHTLTokenType::WHITESPACE) {
            tokens.push_back(token);
        }
    }
    
    // 添加EOF标记
    tokens.push_back(CreateToken(CHTLTokenType::EOF_TOKEN));
    
    if (debugMode_) {
        PrintTokens(tokens);
    }
    
    return tokens;
}

CHTLTokenPtr CHTLLexer::NextToken() {
    SkipWhitespace();
    
    if (IsAtEnd()) {
        return CreateToken(CHTLTokenType::EOF_TOKEN);
    }
    
    char current = CurrentChar();
    
    // 注释处理
    if (current == '/' && PeekChar() == '/') {
        return ReadComment();
    }
    if (current == '/' && PeekChar() == '*') {
        return ReadComment();
    }
    if (current == '-' && PeekChar() == '-') {
        return ReadGeneratorComment();
    }
    
    // 字符串字面量
    if (current == '"' || current == '\'') {
        return ReadString(current);
    }
    
    // 数字字面量
    if (IsDigit(current)) {
        return ReadNumericLiteral();
    }
    
    // 方括号表达式 [Template], [Custom] 等
    if (current == '[') {
        return ReadBracketExpression();
    }
    
    // @ 表达式
    if (current == '@') {
        return ReadAtExpression();
    }
    
    // 选择器
    if (current == '.' || current == '#' || current == '&') {
        return ReadSelector();
    }
    
    // 多字符操作符
    auto multiChar = ReadMultiCharOperator();
    if (multiChar) {
        return multiChar;
    }
    
    // 单字符标记
    switch (current) {
        case '{': Advance(); return CreateToken(CHTLTokenType::LEFT_BRACE, current);
        case '}': Advance(); return CreateToken(CHTLTokenType::RIGHT_BRACE, current);
        case '[': Advance(); return CreateToken(CHTLTokenType::LEFT_BRACKET, current);
        case ']': Advance(); return CreateToken(CHTLTokenType::RIGHT_BRACKET, current);
        case '(': Advance(); return CreateToken(CHTLTokenType::LEFT_PAREN, current);
        case ')': Advance(); return CreateToken(CHTLTokenType::RIGHT_PAREN, current);
        case ';': Advance(); return CreateToken(CHTLTokenType::SEMICOLON, current);
        case ',': Advance(); return CreateToken(CHTLTokenType::COMMA, current);
        case ':': Advance(); return CreateToken(CHTLTokenType::COLON, current);
        case '=': Advance(); return CreateToken(CHTLTokenType::EQUALS, current);
        case '*': Advance(); return CreateToken(CHTLTokenType::ASTERISK, current);
        case '/': Advance(); return CreateToken(CHTLTokenType::SLASH, current);
    }
    
    // 标识符或关键字
    if (IsValidIdentifierStart(current)) {
        return ReadIdentifier();
    }
    
    // 无修饰字面量 (除了特殊字符外的任何可见字符)
    if (std::isgraph(current) && current != '{' && current != '}' && 
        current != '[' && current != ']' && current != '(' && current != ')' &&
        current != ';' && current != ',' && current != ':' && current != '=' &&
        current != '*' && current != '/' && current != '"' && current != '\'' &&
        current != '.' && current != '#' && current != '&' && current != '@') {
        return ReadUnquotedLiteral();
    }
    
    // 未知字符
    ReportError("Unexpected character: " + std::string(1, current));
    Advance();
    return CreateToken(CHTLTokenType::UNKNOWN, current);
}

bool CHTLLexer::IsAtEnd() const {
    return position_ >= input_.length();
}

size_t CHTLLexer::GetPosition() const {
    return position_;
}

size_t CHTLLexer::GetLine() const {
    return line_;
}

size_t CHTLLexer::GetColumn() const {
    return column_;
}

bool CHTLLexer::HasErrors() const {
    return !errors_.empty();
}

const std::vector<std::string>& CHTLLexer::GetErrors() const {
    return errors_;
}

void CHTLLexer::ClearErrors() {
    errors_.clear();
}

void CHTLLexer::EnableDebugMode(bool enable) {
    debugMode_ = enable;
}

void CHTLLexer::PrintTokens(const std::vector<CHTLTokenPtr>& tokens) const {
    std::cout << "=== CHTL Lexer Debug Output ===" << std::endl;
    for (const auto& token : tokens) {
        std::cout << token->ToString() << std::endl;
    }
    std::cout << "=== End Debug Output ===" << std::endl;
}

char CHTLLexer::CurrentChar() const {
    return IsAtEnd() ? '\0' : input_[position_];
}

char CHTLLexer::PeekChar(size_t offset) const {
    size_t peekPos = position_ + offset;
    return peekPos >= input_.length() ? '\0' : input_[peekPos];
}

void CHTLLexer::Advance() {
    if (!IsAtEnd()) {
        if (input_[position_] == '\n') {
            line_++;
            column_ = 1;
        } else {
            column_++;
        }
        position_++;
    }
}

void CHTLLexer::SkipWhitespace() {
    while (!IsAtEnd() && IsWhitespace(CurrentChar())) {
        Advance();
    }
}

CHTLTokenPtr CHTLLexer::ReadString(char delimiter) {
    std::string value;
    Advance(); // 跳过开始引号
    
    while (!IsAtEnd() && CurrentChar() != delimiter) {
        if (CurrentChar() == '\\' && PeekChar() == delimiter) {
            // 转义字符
            Advance(); // 跳过反斜杠
            value += CurrentChar();
            Advance();
        } else {
            value += CurrentChar();
            Advance();
        }
    }
    
    if (IsAtEnd()) {
        ReportError("Unterminated string literal");
        return CreateErrorToken("Unterminated string");
    }
    
    Advance(); // 跳过结束引号
    return CreateToken(CHTLTokenType::STRING_LITERAL, value);
}

CHTLTokenPtr CHTLLexer::ReadUnquotedLiteral() {
    std::string value;
    
    while (!IsAtEnd() && !IsWhitespace(CurrentChar()) && 
           CurrentChar() != '{' && CurrentChar() != '}' &&
           CurrentChar() != '[' && CurrentChar() != ']' &&
           CurrentChar() != '(' && CurrentChar() != ')' &&
           CurrentChar() != ';' && CurrentChar() != ',' &&
           CurrentChar() != ':' && CurrentChar() != '=') {
        value += CurrentChar();
        Advance();
    }
    
    return CreateToken(CHTLTokenType::UNQUOTED_LITERAL, value);
}

CHTLTokenPtr CHTLLexer::ReadNumericLiteral() {
    std::string value;
    
    while (!IsAtEnd() && (IsDigit(CurrentChar()) || CurrentChar() == '.')) {
        value += CurrentChar();
        Advance();
    }
    
    return CreateToken(CHTLTokenType::NUMERIC_LITERAL, value);
}

CHTLTokenPtr CHTLLexer::ReadIdentifier() {
    std::string value;
    
    while (!IsAtEnd() && IsValidIdentifierChar(CurrentChar())) {
        value += CurrentChar();
        Advance();
    }
    
    return ProcessIdentifierOrKeyword(value);
}

CHTLTokenPtr CHTLLexer::ReadComment() {
    std::string value;
    
    if (CurrentChar() == '/' && PeekChar() == '/') {
        // 单行注释
        Advance(); Advance(); // 跳过 //
        while (!IsAtEnd() && !IsLineBreak(CurrentChar())) {
            value += CurrentChar();
            Advance();
        }
        return CreateToken(CHTLTokenType::SINGLE_LINE_COMMENT, value);
    } else if (CurrentChar() == '/' && PeekChar() == '*') {
        // 多行注释
        Advance(); Advance(); // 跳过 /*
        while (!IsAtEnd() && !(CurrentChar() == '*' && PeekChar() == '/')) {
            value += CurrentChar();
            Advance();
        }
        if (!IsAtEnd()) {
            Advance(); Advance(); // 跳过 */
        }
        return CreateToken(CHTLTokenType::MULTI_LINE_COMMENT, value);
    }
    
    return CreateErrorToken("Invalid comment");
}

CHTLTokenPtr CHTLLexer::ReadGeneratorComment() {
    std::string value;
    Advance(); Advance(); // 跳过 --
    
    while (!IsAtEnd() && !IsLineBreak(CurrentChar())) {
        value += CurrentChar();
        Advance();
    }
    
    return CreateToken(CHTLTokenType::GENERATOR_COMMENT, value);
}

CHTLTokenPtr CHTLLexer::ReadSelector() {
    char selectorChar = CurrentChar();
    Advance();
    
    std::string selectorName;
    while (!IsAtEnd() && IsValidIdentifierChar(CurrentChar())) {
        selectorName += CurrentChar();
        Advance();
    }
    
    std::string fullSelector = std::string(1, selectorChar) + selectorName;
    
    switch (selectorChar) {
        case '.':
            return CreateToken(CHTLTokenType::CLASS_SELECTOR, fullSelector);
        case '#':
            return CreateToken(CHTLTokenType::ID_SELECTOR, fullSelector);
        case '&':
            return CreateToken(CHTLTokenType::REFERENCE_SELECTOR, fullSelector);
        default:
            return CreateToken(CHTLTokenType::DOT, std::string(1, selectorChar));
    }
}

CHTLTokenPtr CHTLLexer::ReadBracketExpression() {
    size_t startPos = position_;
    std::string value;
    
    Advance(); // 跳过 [
    while (!IsAtEnd() && CurrentChar() != ']') {
        value += CurrentChar();
        Advance();
    }
    
    if (IsAtEnd()) {
        ReportError("Unterminated bracket expression");
        return CreateErrorToken("Unterminated bracket");
    }
    
    Advance(); // 跳过 ]
    
    std::string fullExpression = "[" + value + "]";
    
    // 检查是否为已知的括号表达式
    if (globalMap_.IsKeyword(fullExpression)) {
        return CreateToken(globalMap_.GetKeywordType(fullExpression), fullExpression);
    }
    
    // 如果不是已知关键字，回退并按普通左括号处理
    position_ = startPos;
    // TODO: 这里需要正确恢复行列号，暂时简化处理
    Advance();
    return CreateToken(CHTLTokenType::LEFT_BRACKET, "[");
}

CHTLTokenPtr CHTLLexer::ReadAtExpression() {
    std::string value = "@";
    Advance(); // 跳过 @
    
    while (!IsAtEnd() && IsValidIdentifierChar(CurrentChar())) {
        value += CurrentChar();
        Advance();
    }
    
    // 检查是否为已知的@表达式
    if (globalMap_.IsKeyword(value)) {
        return CreateToken(globalMap_.GetKeywordType(value), value);
    }
    
    return CreateToken(CHTLTokenType::IDENTIFIER, value);
}

CHTLTokenPtr CHTLLexer::ProcessIdentifierOrKeyword(const std::string& value) {
    // 检查是否为HTML元素
    if (globalMap_.IsHTMLElement(value)) {
        return CreateToken(CHTLTokenType::HTML_ELEMENT, value);
    }
    
    // 检查是否为关键字
    if (globalMap_.IsKeyword(value)) {
        return CreateToken(globalMap_.GetKeywordType(value), value);
    }
    
    // 普通标识符
    return CreateToken(CHTLTokenType::IDENTIFIER, value);
}

bool CHTLLexer::IsValidIdentifierStart(char c) const {
    return std::isalpha(c) || c == '_';
}

bool CHTLLexer::IsValidIdentifierChar(char c) const {
    return std::isalnum(c) || c == '_' || c == '-';
}

bool CHTLLexer::IsDigit(char c) const {
    return std::isdigit(c);
}

bool CHTLLexer::IsWhitespace(char c) const {
    return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}

bool CHTLLexer::IsLineBreak(char c) const {
    return c == '\n' || c == '\r';
}

CHTLTokenPtr CHTLLexer::ReadMultiCharOperator() {
    if (CurrentChar() == ':' && PeekChar() == ':') {
        Advance(); Advance();
        return CreateToken(CHTLTokenType::DOUBLE_COLON, "::");
    }
    
    if (CurrentChar() == '-' && PeekChar() == '>') {
        Advance(); Advance();
        return CreateToken(CHTLTokenType::ARROW, "->");
    }
    
    return nullptr;
}

bool CHTLLexer::MatchSequence(const std::string& sequence) const {
    for (size_t i = 0; i < sequence.length(); ++i) {
        if (PeekChar(i) != sequence[i]) {
            return false;
        }
    }
    return true;
}

void CHTLLexer::ReportError(const std::string& message) {
    std::string error = "Line " + std::to_string(line_) + 
                       ", Column " + std::to_string(column_) + ": " + message;
    errors_.push_back(error);
}

CHTLTokenPtr CHTLLexer::CreateErrorToken(const std::string& message) {
    ReportError(message);
    return CreateToken(CHTLTokenType::UNKNOWN, message);
}

CHTLTokenPtr CHTLLexer::CreateToken(CHTLTokenType type, const std::string& value) {
    return std::make_shared<CHTLToken>(type, value, line_, column_, position_);
}

CHTLTokenPtr CHTLLexer::CreateToken(CHTLTokenType type, char value) {
    return CreateToken(type, std::string(1, value));
}

} // namespace CHTL