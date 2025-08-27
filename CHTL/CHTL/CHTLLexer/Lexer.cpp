#include "Lexer.h"
#include "../CHTLNode/ConfigNode.h"
#include <cctype>
#include <sstream>
#include <algorithm>

namespace CHTL {

Lexer::Lexer(const std::string& source, std::shared_ptr<GlobalMap> globalMap)
    : source(source), globalMap(globalMap), position(0), line(1), column(1), debugMode(false) {
}

Lexer::~Lexer() {}

Token Lexer::NextToken() {
    // 如果缓冲区有Token，先返回缓冲区的
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
    
    if (current == '-' && PeekChar() == '-') {
        return ReadGeneratorComment();
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
            if (IsStartOfPrefixKeyword()) {
                return ReadPrefixKeyword();
            }
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
        case '@':
            if (IsStartOfTypeIdentifier()) {
                return ReadTypeIdentifier();
            }
            Advance();
            return Token(TokenType::AT, "@", startLine, startColumn, startPosition);
        case '#':
            Advance();
            return Token(TokenType::HASH, "#", startLine, startColumn, startPosition);
        case '&':
            Advance();
            return Token(TokenType::AMPERSAND, "&", startLine, startColumn, startPosition);
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
    if (std::isalpha(current) || current == '_') {
        return ReadIdentifier();
    }
    
    // 处理无引号字面量
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
    // 确保缓冲区有足够的Token
    while (tokenBuffer.size() <= static_cast<size_t>(offset)) {
        int savedPosition = position;
        int savedLine = line;
        int savedColumn = column;
        
        Token token = NextToken();
        tokenBuffer.push_back(token);
        
        // 如果NextToken没有从缓冲区读取，需要恢复位置
        if (tokenBuffer.size() == 1) {
            position = savedPosition;
            line = savedLine;
            column = savedColumn;
        }
    }
    
    return tokenBuffer[offset];
}

bool Lexer::IsEOF() const {
    return position >= static_cast<int>(source.length());
}

void Lexer::ReportError(const std::string& message) {
    std::stringstream ss;
    ss << "错误 [" << line << ":" << column << "]: " << message;
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
    
    while (!IsEOF() && (std::isalnum(CurrentChar()) || CurrentChar() == '_')) {
        value += CurrentChar();
        Advance();
    }
    
    // 特殊处理 "at top" 和 "at bottom"
    if (value == "at") {
        return HandleAtKeyword();
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
                // 处理转义字符
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
        Advance(); // 跳过结束引号
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
    
    while (!IsEOF() && IsValidUnquotedChar(CurrentChar())) {
        value += CurrentChar();
        Advance();
    }
    
    return Token(TokenType::UNQUOTED_LITERAL, value, startLine, startColumn, startPosition);
}

Token Lexer::ReadComment() {
    int startLine = line;
    int startColumn = column;
    int startPosition = position;
    std::string value;
    
    if (CurrentChar() == '/' && PeekChar() == '/') {
        Advance(); Advance(); // 跳过 //
        while (!IsEOF() && CurrentChar() != '\n') {
            value += CurrentChar();
            Advance();
        }
        return Token(TokenType::SINGLE_COMMENT, value, startLine, startColumn, startPosition);
    } else if (CurrentChar() == '/' && PeekChar() == '*') {
        Advance(); Advance(); // 跳过 /*
        while (!IsEOF() && !(CurrentChar() == '*' && PeekChar() == '/')) {
            value += CurrentChar();
            Advance();
        }
        if (CurrentChar() == '*' && PeekChar() == '/') {
            Advance(); Advance(); // 跳过 */
        } else {
            ReportError("未闭合的多行注释");
        }
        return Token(TokenType::MULTI_COMMENT, value, startLine, startColumn, startPosition);
    }
    
    return Token(TokenType::INVALID, "", startLine, startColumn, startPosition);
}

Token Lexer::ReadGeneratorComment() {
    int startLine = line;
    int startColumn = column;
    int startPosition = position;
    std::string value;
    
    Advance(); Advance(); // 跳过 --
    
    while (!IsEOF() && CurrentChar() != '\n') {
        value += CurrentChar();
        Advance();
    }
    
    return Token(TokenType::GEN_COMMENT, value, startLine, startColumn, startPosition);
}

Token Lexer::ReadPrefixKeyword() {
    int startLine = line;
    int startColumn = column;
    int startPosition = position;
    std::string value;
    
    value += CurrentChar(); // [
    Advance();
    
    while (!IsEOF() && CurrentChar() != ']') {
        value += CurrentChar();
        Advance();
    }
    
    if (CurrentChar() == ']') {
        value += CurrentChar();
        Advance();
    } else {
        ReportError("未闭合的前缀关键字");
    }
    
    TokenType type = TokenUtils::StringToTokenType(value);
    if (type == TokenType::IDENTIFIER) {
        // 不是有效的前缀关键字
        ReportError("未知的前缀关键字: " + value);
        return Token(TokenType::INVALID, value, startLine, startColumn, startPosition);
    }
    
    return Token(type, value, startLine, startColumn, startPosition);
}

Token Lexer::ReadTypeIdentifier() {
    int startLine = line;
    int startColumn = column;
    int startPosition = position;
    std::string value;
    
    value += CurrentChar(); // @
    Advance();
    
    while (!IsEOF() && (std::isalnum(CurrentChar()) || CurrentChar() == '_')) {
        value += CurrentChar();
        Advance();
    }
    
    TokenType type = TokenUtils::StringToTokenType(value);
    return Token(type, value, startLine, startColumn, startPosition);
}

bool Lexer::IsStartOfPrefixKeyword() const {
    if (CurrentChar() != '[') return false;
    
    // 查找对应的 ]
    int offset = 1;
    while (PeekChar(offset) != '\0' && PeekChar(offset) != ']') {
        offset++;
    }
    
    if (PeekChar(offset) == ']') {
        std::string potential;
        for (int i = 0; i <= offset; i++) {
            potential += PeekChar(i - 1);
        }
        return TokenUtils::IsPrefixKeyword(potential);
    }
    
    return false;
}

bool Lexer::IsStartOfTypeIdentifier() const {
    if (CurrentChar() != '@') return false;
    
    // 检查后面是否跟着字母
    return std::isalpha(PeekChar()) || PeekChar() == '_';
}

bool Lexer::IsValidUnquotedChar(char c) const {
    // 无引号字面量允许的字符
    return std::isalnum(c) || c == '_' || c == '-' || c == '.' || c == '#' || c == '%' || c == '!';
}

TokenType Lexer::DetermineIdentifierType(const std::string& value) {
    TokenType type = TokenUtils::StringToTokenType(value);
    
    // 如果不是关键字，检查是否是HTML标签
    if (type == TokenType::IDENTIFIER && TokenUtils::IsHTMLTag(value)) {
        return TokenType::HTML_TAG;
    }
    
    return type;
}

Token Lexer::HandleAtKeyword() {
    int startLine = line;
    int startColumn = column;
    int startPosition = position - 2; // "at" 已经被读取
    
    SkipWhitespace();
    
    if (!IsEOF() && std::isalpha(CurrentChar())) {
        std::string nextWord;
        int peekOffset = 0;
        
        while (PeekChar(peekOffset) != '\0' && std::isalpha(PeekChar(peekOffset))) {
            nextWord += PeekChar(peekOffset);
            peekOffset++;
        }
        
        if (nextWord == "top") {
            // 消费 "top"
            for (int i = 0; i < peekOffset; i++) {
                Advance();
            }
            return Token(TokenType::AT_TOP, "at top", startLine, startColumn, startPosition);
        } else if (nextWord == "bottom") {
            // 消费 "bottom"
            for (int i = 0; i < peekOffset; i++) {
                Advance();
            }
            return Token(TokenType::AT_BOTTOM, "at bottom", startLine, startColumn, startPosition);
        }
    }
    
    // 不是 "at top" 或 "at bottom"，只是普通的标识符
    return Token(TokenType::IDENTIFIER, "at", startLine, startColumn, startPosition);
}

void Lexer::ApplyConfiguration(std::shared_ptr<ConfigNode> config) {
    if (!config) return;
    
    // 检查是否禁用了Name配置组
    std::string disableNameGroup = config->GetConfigValue("DISABLE_NAME_GROUP");
    if (disableNameGroup == "true") {
        return;  // 不应用自定义关键字
    }
    
    // 应用Name配置中的自定义关键字
    auto nameConfigs = config->GetAllNameConfigs();
    
    for (const auto& [key, item] : nameConfigs) {
        if (item.isArray && !item.arrayValues.empty()) {
            // 组选项 - 第一个值作为主要映射
            std::string primaryValue = item.arrayValues[0];
            
            // 根据key更新对应的关键字映射
            if (key == "KEYWORD_TEXT") {
                TokenUtils::UpdateKeywordMapping("text", TokenUtils::StringToTokenType(primaryValue));
            } else if (key == "KEYWORD_STYLE") {
                TokenUtils::UpdateKeywordMapping("style", TokenUtils::StringToTokenType(primaryValue));
            } else if (key == "KEYWORD_SCRIPT") {
                TokenUtils::UpdateKeywordMapping("script", TokenUtils::StringToTokenType(primaryValue));
            }
            // TODO: 处理其他关键字映射
            
            // 处理组选项中的其他值
            for (size_t i = 1; i < item.arrayValues.size(); ++i) {
                // 添加额外的映射
                // TODO: 实现组选项的完整支持
            }
        } else if (!item.value.empty()) {
            // 单一值
            // TODO: 处理单一值的关键字映射
        }
    }
}

} // namespace CHTL