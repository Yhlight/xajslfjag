#include "Lexer.h"
#include <cctype>
#include <stdexcept>

namespace CHTL {

Lexer::Lexer(std::shared_ptr<GlobalMap> gMap) 
    : position(0), currentLine(1), currentColumn(1), isAnalyzed(false) {
    globalMap = gMap ? gMap : std::make_shared<GlobalMap>();
}

Lexer::~Lexer() = default;

void Lexer::SetSource(const std::string& sourceCode) {
    source = sourceCode;
    Reset();
}

void Lexer::SetSource(std::istream& stream) {
    std::ostringstream buffer;
    buffer << stream.rdbuf();
    SetSource(buffer.str());
}

void Lexer::Analyze() {
    if (isAnalyzed) {
        return;
    }
    
    position = 0;
    currentLine = 1;
    currentColumn = 1;
    tokenBuffer.clear();
    errors.clear();
    
    while (!IsEOF()) {
        SkipWhitespace();
        
        if (IsEOF()) {
            break;
        }
        
        char c = PeekChar();
        
        try {
            if (IsAlpha(c) || c == '_') {
                AddToken(ReadIdentifierOrKeyword());
            }
            else if (IsDigit(c)) {
                AddToken(ReadNumber());
            }
            else if (c == '"' || c == '\'') {
                AddToken(ReadStringLiteral(c));
            }
            else if (c == '/' && position + 1 < source.length()) {
                char next = source[position + 1];
                if (next == '/') {
                    AddToken(ReadLineComment());
                } else if (next == '*') {
                    AddToken(ReadBlockComment());
                } else {
                    AddToken(ReadSymbol());
                }
            }
            else if (c == '-' && position + 1 < source.length() && source[position + 1] == '-') {
                AddToken(ReadGeneratorComment());
            }
            else if (c == '[') {
                AddToken(ReadBracketType());
            }
            else if (c == '@') {
                AddToken(ReadTypeIdentifier());
            }
            else if (c == '#' && IsHexColorStart()) {
                AddToken(ReadColorValue());
            }
            else if (c == '{' || c == '}' || c == '(' || c == ')' || c == ']' || 
                     c == ';' || c == ':' || c == '=' || c == ',' || c == '.' || 
                     c == '#' || c == '&' || c == '+' || c == '-' || c == '*') {
                AddToken(ReadSymbol());
            }
            else if (IsValidUnquotedChar(c)) {
                AddToken(ReadUnquotedLiteral());
            }
            else {
                AddError("未知字符: '" + std::string(1, c) + "'");
                GetChar(); // 跳过未知字符
            }
        }
        catch (const std::exception& e) {
            AddError("词法分析错误: " + std::string(e.what()));
            GetChar(); // 跳过错误字符
        }
    }
    
    // 添加EOF token
    AddToken(CreateToken(TokenType::EOF_TOKEN));
    
    // 处理复合关键字
    HandleCompoundKeywords();
    
    // 验证Token序列
    if (!ValidateTokenSequence()) {
        AddError("Token序列验证失败");
    }
    
    isAnalyzed = true;
}

const std::vector<Token>& Lexer::GetTokens() const {
    if (!isAnalyzed) {
        throw std::runtime_error("词法分析尚未完成");
    }
    return tokenBuffer;
}

const Token& Lexer::GetToken(size_t index) const {
    if (!isAnalyzed) {
        throw std::runtime_error("词法分析尚未完成");
    }
    if (index >= tokenBuffer.size()) {
        throw std::out_of_range("Token索引超出范围");
    }
    return tokenBuffer[index];
}

size_t Lexer::GetTokenCount() const {
    return isAnalyzed ? tokenBuffer.size() : 0;
}

void Lexer::Reset() {
    position = 0;
    currentLine = 1;
    currentColumn = 1;
    tokenBuffer.clear();
    errors.clear();
    isAnalyzed = false;
}

bool Lexer::IsAnalyzed() const {
    return isAnalyzed;
}

std::vector<std::string> Lexer::GetErrors() const {
    return errors;
}

void Lexer::SkipWhitespace() {
    while (!IsEOF() && IsWhitespace(PeekChar()) && !IsNewline(PeekChar())) {
        GetChar();
    }
}

char Lexer::PeekChar() const {
    return (position < source.length()) ? source[position] : '\0';
}

char Lexer::GetChar() {
    if (IsEOF()) {
        return '\0';
    }
    
    char c = source[position++];
    UpdatePosition(c);
    return c;
}

void Lexer::UngetChar() {
    if (position > 0) {
        --position;
        if (currentColumn > 1) {
            --currentColumn;
        } else if (currentLine > 1) {
            --currentLine;
            // 这里应该重新计算列号，但为简单起见暂时设为1
            currentColumn = 1;
        }
    }
}

bool Lexer::IsEOF() const {
    return position >= source.length();
}

Token Lexer::ReadIdentifierOrKeyword() {
    std::string value;
    TokenPosition startPos(currentLine, currentColumn, position);
    
    while (!IsEOF() && (IsAlphaNumeric(PeekChar()) || PeekChar() == '-')) {
        value += GetChar();
    }
    
    // 检查是否为关键字
    TokenType type = globalMap->LookupKeyword(value);
    
    return Token(type, value, startPos);
}

Token Lexer::ReadNumber() {
    std::string value;
    TokenPosition startPos(currentLine, currentColumn, position);
    
    // 读取整数部分
    while (!IsEOF() && IsDigit(PeekChar())) {
        value += GetChar();
    }
    
    // 检查小数点
    if (!IsEOF() && PeekChar() == '.') {
        char next = (position + 1 < source.length()) ? source[position + 1] : '\0';
        if (IsDigit(next)) {
            value += GetChar(); // 添加小数点
            // 读取小数部分
            while (!IsEOF() && IsDigit(PeekChar())) {
                value += GetChar();
            }
        }
    }
    
    return Token(TokenType::NUMBER, value, startPos);
}

Token Lexer::ReadStringLiteral(char quote) {
    std::string value;
    TokenPosition startPos(currentLine, currentColumn, position);
    
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

Token Lexer::ReadUnquotedLiteral() {
    std::string value;
    TokenPosition startPos(currentLine, currentColumn, position);
    
    while (!IsEOF() && IsValidUnquotedChar(PeekChar())) {
        value += GetChar();
    }
    
    return Token(TokenType::UNQUOTED_LITERAL, value, startPos);
}

Token Lexer::ReadLineComment() {
    std::string value;
    TokenPosition startPos(currentLine, currentColumn, position);
    
    GetChar(); // 跳过第一个'/'
    GetChar(); // 跳过第二个'/'
    
    while (!IsEOF() && !IsNewline(PeekChar())) {
        value += GetChar();
    }
    
    return Token(TokenType::COMMENT_LINE, value, startPos);
}

Token Lexer::ReadBlockComment() {
    std::string value;
    TokenPosition startPos(currentLine, currentColumn, position);
    
    GetChar(); // 跳过'/'
    GetChar(); // 跳过'*'
    
    while (!IsEOF()) {
        char c = GetChar();
        if (c == '*' && !IsEOF() && PeekChar() == '/') {
            GetChar(); // 跳过'/'
            break;
        }
        value += c;
    }
    
    return Token(TokenType::COMMENT_BLOCK, value, startPos);
}

Token Lexer::ReadGeneratorComment() {
    std::string value;
    TokenPosition startPos(currentLine, currentColumn, position);
    
    GetChar(); // 跳过第一个'-'
    GetChar(); // 跳过第二个'-'
    
    while (!IsEOF() && !IsNewline(PeekChar())) {
        value += GetChar();
    }
    
    return Token(TokenType::COMMENT_GENERATOR, value, startPos);
}

Token Lexer::ReadSymbol() {
    TokenPosition startPos(currentLine, currentColumn, position);
    char c = GetChar();
    std::string value(1, c);
    
    TokenType type = globalMap->LookupSymbol(value);
    
    return Token(type, value, startPos);
}

Token Lexer::ReadBracketType() {
    std::string value;
    TokenPosition startPos(currentLine, currentColumn, position);
    
    // 读取整个[xxx]结构
    while (!IsEOF() && PeekChar() != ']') {
        value += GetChar();
    }
    
    if (!IsEOF()) {
        value += GetChar(); // 添加']'
    }
    
    TokenType type = globalMap->LookupBracketType(value);
    
    return Token(type, value, startPos);
}

Token Lexer::ReadTypeIdentifier() {
    std::string value;
    TokenPosition startPos(currentLine, currentColumn, position);
    
    value += GetChar(); // 添加'@'
    
    // 读取类型名称
    while (!IsEOF() && IsAlphaNumeric(PeekChar())) {
        value += GetChar();
    }
    
    TokenType type = globalMap->LookupTypeIdentifier(value);
    
    return Token(type, value, startPos);
}

Token Lexer::ReadColorValue() {
    std::string value;
    TokenPosition startPos(currentLine, currentColumn, position);
    
    value += GetChar(); // 添加'#'
    
    // 读取十六进制数字
    while (!IsEOF() && IsHexDigit(PeekChar())) {
        value += GetChar();
    }
    
    return Token(TokenType::COLOR_VALUE, value, startPos);
}

bool Lexer::IsHexColorStart() const {
    // 检查#后面是否跟着有效的十六进制字符
    if (position + 1 >= source.length()) {
        return false;
    }
    
    char next = source[position + 1];
    return IsHexDigit(next);
}

bool Lexer::IsAlpha(char c) const {
    return std::isalpha(static_cast<unsigned char>(c));
}

bool Lexer::IsDigit(char c) const {
    return std::isdigit(static_cast<unsigned char>(c));
}

bool Lexer::IsHexDigit(char c) const {
    return IsDigit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}

bool Lexer::IsAlphaNumeric(char c) const {
    return IsAlpha(c) || IsDigit(c) || c == '_';
}

bool Lexer::IsWhitespace(char c) const {
    return c == ' ' || c == '\t' || c == '\r';
}

bool Lexer::IsNewline(char c) const {
    return c == '\n';
}

bool Lexer::IsValidUnquotedChar(char c) const {
    // 无修饰字面量允许的字符：基本字母数字和一些特殊字符
    return IsAlphaNumeric(c) || 
           c == '-' || c == '.' || c == '/' || c == '?' || c == '!' || c == '%' ||
           // 支持中文字符 (UTF-8)
           (static_cast<unsigned char>(c) >= 0x80);
}

void Lexer::UpdatePosition(char c) {
    if (IsNewline(c)) {
        ++currentLine;
        currentColumn = 1;
    } else {
        ++currentColumn;
    }
}

Token Lexer::CreateToken(TokenType type, const std::string& value) {
    return Token(type, value, TokenPosition(currentLine, currentColumn, position));
}

void Lexer::AddToken(const Token& token) {
    tokenBuffer.push_back(token);
}

void Lexer::HandleCompoundKeywords() {
    // 处理"at top"和"at bottom"这样的复合关键字
    for (size_t i = 0; i < tokenBuffer.size() - 1; ++i) {
        if (tokenBuffer[i].type == TokenType::IDENTIFIER && tokenBuffer[i].value == "at") {
            if (i + 1 < tokenBuffer.size()) {
                if (tokenBuffer[i + 1].value == "top") {
                    tokenBuffer[i].type = TokenType::AT_TOP;
                    tokenBuffer[i].value = "at top";
                    tokenBuffer.erase(tokenBuffer.begin() + i + 1);
                } else if (tokenBuffer[i + 1].value == "bottom") {
                    tokenBuffer[i].type = TokenType::AT_BOTTOM;
                    tokenBuffer[i].value = "at bottom";
                    tokenBuffer.erase(tokenBuffer.begin() + i + 1);
                }
            }
        }
    }
}

bool Lexer::ValidateTokenSequence() const {
    // 基本的Token序列验证
    int braceCount = 0;
    int bracketCount = 0;
    int parenCount = 0;
    
    for (const Token& token : tokenBuffer) {
        switch (token.type) {
            case TokenType::LEFT_BRACE:
                ++braceCount;
                break;
            case TokenType::RIGHT_BRACE:
                --braceCount;
                break;
            case TokenType::LEFT_BRACKET:
                ++bracketCount;
                break;
            case TokenType::RIGHT_BRACKET:
                --bracketCount;
                break;
            case TokenType::LEFT_PAREN:
                ++parenCount;
                break;
            case TokenType::RIGHT_PAREN:
                --parenCount;
                break;
            default:
                break;
        }
        
        // 检查是否有负数（表示右括号多于左括号）
        if (braceCount < 0 || bracketCount < 0 || parenCount < 0) {
            return false;
        }
    }
    
    // 检查是否所有括号都匹配
    return braceCount == 0 && bracketCount == 0 && parenCount == 0;
}

void Lexer::AddError(const std::string& message) {
    std::string errorMsg = "行 " + std::to_string(currentLine) + 
                          ", 列 " + std::to_string(currentColumn) + 
                          ": " + message;
    errors.push_back(errorMsg);
}

} // namespace CHTL