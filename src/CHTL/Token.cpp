#include "CHTL/Token.h"
#include <algorithm>

namespace CHTL {

// Token实现
bool Token::IsKeyword() const {
    return type >= TokenType::TEXT && type <= TokenType::HTML5;
}

bool Token::IsBlockMarker() const {
    return type >= TokenType::TEMPLATE_MARKER && type <= TokenType::EXPORT_MARKER;
}

bool Token::IsAtTypeMarker() const {
    return type >= TokenType::AT_STYLE && type <= TokenType::AT_CONFIG;
}

bool Token::IsLiteral() const {
    return type == TokenType::IDENTIFIER ||
           type == TokenType::STRING_LITERAL ||
           type == TokenType::SINGLE_STRING_LITERAL ||
           type == TokenType::UNQUOTED_LITERAL ||
           type == TokenType::NUMBER;
}

bool Token::IsComment() const {
    return type == TokenType::LINE_COMMENT ||
           type == TokenType::BLOCK_COMMENT ||
           type == TokenType::GENERATOR_COMMENT;
}

String Token::GetTypeName() const {
    return TokenUtils::TokenTypeToString(type);
}

bool Token::IsEquivalent(const Token& other) const {
    // CE对等式：冒号和等号等价
    if ((type == TokenType::COLON && other.type == TokenType::EQUALS) ||
        (type == TokenType::EQUALS && other.type == TokenType::COLON)) {
        return true;
    }
    return type == other.type && value == other.value;
}

// TokenUtils实现
TokenType TokenUtils::StringToTokenType(const String& str) {
    // 关键字映射
    static const HashMap<String, TokenType> keywordMap = {
        {"text", TokenType::TEXT},
        {"style", TokenType::STYLE},
        {"script", TokenType::SCRIPT},
        {"inherit", TokenType::INHERIT},
        {"delete", TokenType::DELETE},
        {"insert", TokenType::INSERT},
        {"after", TokenType::AFTER},
        {"before", TokenType::BEFORE},
        {"replace", TokenType::REPLACE},
        {"at", TokenType::AT_TOP},  // "at top"
        {"from", TokenType::FROM},
        {"as", TokenType::AS},
        {"except", TokenType::EXCEPT},
        {"use", TokenType::USE},
        {"html5", TokenType::HTML5}
    };
    
    // 块标记映射
    static const HashMap<String, TokenType> blockMarkerMap = {
        {"[Template]", TokenType::TEMPLATE_MARKER},
        {"[Custom]", TokenType::CUSTOM_MARKER},
        {"[Origin]", TokenType::ORIGIN_MARKER},
        {"[Import]", TokenType::IMPORT_MARKER},
        {"[Namespace]", TokenType::NAMESPACE_MARKER},
        {"[Configuration]", TokenType::CONFIGURATION_MARKER},
        {"[Info]", TokenType::INFO_MARKER},
        {"[Export]", TokenType::EXPORT_MARKER}
    };
    
    // @类型标记映射
    static const HashMap<String, TokenType> atTypeMap = {
        {"@Style", TokenType::AT_STYLE},
        {"@Element", TokenType::AT_ELEMENT},
        {"@Var", TokenType::AT_VAR},
        {"@Html", TokenType::AT_HTML},
        {"@JavaScript", TokenType::AT_JAVASCRIPT},
        {"@Chtl", TokenType::AT_CHTL},
        {"@CJmod", TokenType::AT_CJMOD},
        {"@Config", TokenType::AT_CONFIG}
    };
    
    // 检查关键字
    auto keywordIt = keywordMap.find(str);
    if (keywordIt != keywordMap.end()) {
        return keywordIt->second;
    }
    
    // 检查块标记
    auto blockIt = blockMarkerMap.find(str);
    if (blockIt != blockMarkerMap.end()) {
        return blockIt->second;
    }
    
    // 检查@类型标记
    auto atIt = atTypeMap.find(str);
    if (atIt != atTypeMap.end()) {
        return atIt->second;
    }
    
    // 特殊处理 "at top" 和 "at bottom"
    if (str == "at top") return TokenType::AT_TOP;
    if (str == "at bottom") return TokenType::AT_BOTTOM;
    
    return TokenType::UNKNOWN;
}

String TokenUtils::TokenTypeToString(TokenType type) {
    switch (type) {
        case TokenType::EOF_TOKEN: return "EOF";
        case TokenType::UNKNOWN: return "UNKNOWN";
        
        // 字面量
        case TokenType::IDENTIFIER: return "IDENTIFIER";
        case TokenType::STRING_LITERAL: return "STRING_LITERAL";
        case TokenType::SINGLE_STRING_LITERAL: return "SINGLE_STRING_LITERAL";
        case TokenType::UNQUOTED_LITERAL: return "UNQUOTED_LITERAL";
        case TokenType::NUMBER: return "NUMBER";
        
        // 运算符和标点
        case TokenType::LEFT_BRACE: return "LEFT_BRACE";
        case TokenType::RIGHT_BRACE: return "RIGHT_BRACE";
        case TokenType::LEFT_BRACKET: return "LEFT_BRACKET";
        case TokenType::RIGHT_BRACKET: return "RIGHT_BRACKET";
        case TokenType::LEFT_PAREN: return "LEFT_PAREN";
        case TokenType::RIGHT_PAREN: return "RIGHT_PAREN";
        case TokenType::SEMICOLON: return "SEMICOLON";
        case TokenType::COLON: return "COLON";
        case TokenType::EQUALS: return "EQUALS";
        case TokenType::COMMA: return "COMMA";
        case TokenType::DOT: return "DOT";
        case TokenType::AT: return "AT";
        case TokenType::AMPERSAND: return "AMPERSAND";
        case TokenType::EXCLAMATION: return "EXCLAMATION";
        
        // CHTL关键字
        case TokenType::TEXT: return "TEXT";
        case TokenType::STYLE: return "STYLE";
        case TokenType::SCRIPT: return "SCRIPT";
        case TokenType::INHERIT: return "INHERIT";
        case TokenType::DELETE: return "DELETE";
        case TokenType::INSERT: return "INSERT";
        case TokenType::AFTER: return "AFTER";
        case TokenType::BEFORE: return "BEFORE";
        case TokenType::REPLACE: return "REPLACE";
        case TokenType::AT_TOP: return "AT_TOP";
        case TokenType::AT_BOTTOM: return "AT_BOTTOM";
        case TokenType::FROM: return "FROM";
        case TokenType::AS: return "AS";
        case TokenType::EXCEPT: return "EXCEPT";
        case TokenType::USE: return "USE";
        case TokenType::HTML5: return "HTML5";
        
        // 块标记
        case TokenType::TEMPLATE_MARKER: return "TEMPLATE_MARKER";
        case TokenType::CUSTOM_MARKER: return "CUSTOM_MARKER";
        case TokenType::ORIGIN_MARKER: return "ORIGIN_MARKER";
        case TokenType::IMPORT_MARKER: return "IMPORT_MARKER";
        case TokenType::NAMESPACE_MARKER: return "NAMESPACE_MARKER";
        case TokenType::CONFIGURATION_MARKER: return "CONFIGURATION_MARKER";
        case TokenType::INFO_MARKER: return "INFO_MARKER";
        case TokenType::EXPORT_MARKER: return "EXPORT_MARKER";
        
        // @类型标记
        case TokenType::AT_STYLE: return "AT_STYLE";
        case TokenType::AT_ELEMENT: return "AT_ELEMENT";
        case TokenType::AT_VAR: return "AT_VAR";
        case TokenType::AT_HTML: return "AT_HTML";
        case TokenType::AT_JAVASCRIPT: return "AT_JAVASCRIPT";
        case TokenType::AT_CHTL: return "AT_CHTL";
        case TokenType::AT_CJMOD: return "AT_CJMOD";
        case TokenType::AT_CONFIG: return "AT_CONFIG";
        
        // 注释
        case TokenType::LINE_COMMENT: return "LINE_COMMENT";
        case TokenType::BLOCK_COMMENT: return "BLOCK_COMMENT";
        case TokenType::GENERATOR_COMMENT: return "GENERATOR_COMMENT";
        
        // 特殊
        case TokenType::WHITESPACE: return "WHITESPACE";
        case TokenType::NEWLINE: return "NEWLINE";
        
        default: return "UNKNOWN";
    }
}

bool TokenUtils::IsKeyword(const String& str) {
    static const HashSet<String> keywords = {
        "text", "style", "script", "inherit", "delete", "insert",
        "after", "before", "replace", "at", "from", "as", "except",
        "use", "html5", "top", "bottom"
    };
    return keywords.find(str) != keywords.end();
}

bool TokenUtils::IsBlockMarker(const String& str) {
    return str.length() >= 3 && str[0] == '[' && str[str.length()-1] == ']';
}

bool TokenUtils::IsAtTypeMarker(const String& str) {
    return str.length() >= 2 && str[0] == '@';
}

Vector<String> TokenUtils::GetKeywords() {
    return {
        "text", "style", "script", "inherit", "delete", "insert",
        "after", "before", "replace", "at", "top", "bottom",
        "from", "as", "except", "use", "html5"
    };
}

Vector<String> TokenUtils::GetBlockMarkers() {
    return {
        "[Template]", "[Custom]", "[Origin]", "[Import]",
        "[Namespace]", "[Configuration]", "[Info]", "[Export]"
    };
}

Vector<String> TokenUtils::GetAtTypeMarkers() {
    return {
        "@Style", "@Element", "@Var", "@Html", "@JavaScript",
        "@Chtl", "@CJmod", "@Config"
    };
}

bool TokenUtils::AreEquivalent(TokenType t1, TokenType t2) {
    // CE对等式：冒号和等号等价
    return (t1 == TokenType::COLON && t2 == TokenType::EQUALS) ||
           (t1 == TokenType::EQUALS && t2 == TokenType::COLON) ||
           (t1 == t2);
}

// TokenStream实现
void TokenStream::AddToken(const Token& token) {
    tokens_.push_back(token);
}

void TokenStream::AddToken(TokenType type, const String& value, const SourceLocation& location) {
    tokens_.emplace_back(type, value, location);
}

const Token& TokenStream::Current() const {
    if (IsAtEnd()) {
        static Token eofToken(TokenType::EOF_TOKEN, "", SourceLocation());
        return eofToken;
    }
    return tokens_[current_];
}

const Token& TokenStream::Next() {
    if (!IsAtEnd()) {
        current_++;
    }
    return Current();
}

const Token& TokenStream::Peek(size_t offset) const {
    size_t pos = current_ + offset;
    if (pos >= tokens_.size()) {
        static Token eofToken(TokenType::EOF_TOKEN, "", SourceLocation());
        return eofToken;
    }
    return tokens_[pos];
}

bool TokenStream::IsAtEnd() const {
    return current_ >= tokens_.size();
}

void TokenStream::Clear() {
    tokens_.clear();
    current_ = 0;
}

bool TokenStream::Match(TokenType type) {
    if (IsAtEnd()) return false;
    if (Current().type == type) {
        Next();
        return true;
    }
    return false;
}

bool TokenStream::Match(const Vector<TokenType>& types) {
    for (TokenType type : types) {
        if (Match(type)) {
            return true;
        }
    }
    return false;
}

Token TokenStream::Consume(TokenType type, const String& errorMessage) {
    if (IsAtEnd() || Current().type != type) {
        throw SyntaxError(errorMessage, Current().location);
    }
    Token token = Current();
    Next();
    return token;
}

void TokenStream::Synchronize() {
    // 错误恢复：跳到下一个语句开始位置
    while (!IsAtEnd()) {
        if (Current().type == TokenType::SEMICOLON) {
            Next();
            return;
        }
        
        switch (Peek().type) {
            case TokenType::TEXT:
            case TokenType::STYLE:
            case TokenType::SCRIPT:
            case TokenType::TEMPLATE_MARKER:
            case TokenType::CUSTOM_MARKER:
            case TokenType::ORIGIN_MARKER:
            case TokenType::IMPORT_MARKER:
            case TokenType::NAMESPACE_MARKER:
            case TokenType::CONFIGURATION_MARKER:
            case TokenType::USE:
                return;
            default:
                break;
        }
        
        Next();
    }
}

} // namespace CHTL