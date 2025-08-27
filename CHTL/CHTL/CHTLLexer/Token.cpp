#include "Token.h"
#include <algorithm>
#include <cctype>

namespace CHTL {

// 静态成员初始化
std::unordered_map<std::string, TokenType> TokenUtils::keywordMap;
std::unordered_map<std::string, TokenType> TokenUtils::prefixKeywordMap;
std::unordered_map<std::string, TokenType> TokenUtils::typeIdentifierMap;
std::unordered_map<TokenType, std::string> TokenUtils::tokenTypeNameMap;
bool TokenUtils::mapsInitialized = false;

// HTML标签列表
static const std::vector<std::string> htmlTags = {
    "html", "head", "title", "body", "div", "span", "p", "a", "img", "ul", "ol", "li",
    "table", "tr", "td", "th", "form", "input", "button", "select", "option", "textarea",
    "h1", "h2", "h3", "h4", "h5", "h6", "header", "footer", "nav", "section", "article",
    "aside", "main", "figure", "figcaption", "video", "audio", "canvas", "svg", "iframe",
    "script", "style", "link", "meta", "br", "hr", "strong", "em", "code", "pre", "blockquote"
};

void TokenUtils::InitializeMaps() {
    if (mapsInitialized) return;
    
    // 初始化关键字映射
    keywordMap = {
        {"text", TokenType::TEXT},
        {"style", TokenType::STYLE},
        {"script", TokenType::SCRIPT},
        {"inherit", TokenType::INHERIT},
        {"delete", TokenType::DELETE},
        {"insert", TokenType::INSERT},
        {"after", TokenType::AFTER},
        {"before", TokenType::BEFORE},
        {"replace", TokenType::REPLACE},
        {"at", TokenType::AT_TOP},  // 需要特殊处理 "at top" 和 "at bottom"
        {"from", TokenType::FROM},
        {"as", TokenType::AS},
        {"except", TokenType::EXCEPT},
        {"use", TokenType::USE}
    };
    
    // 初始化前缀关键字映射
    prefixKeywordMap = {
        {"[Custom]", TokenType::CUSTOM},
        {"[Template]", TokenType::TEMPLATE},
        {"[Origin]", TokenType::ORIGIN},
        {"[Import]", TokenType::IMPORT},
        {"[Namespace]", TokenType::NAMESPACE},
        {"[Configuration]", TokenType::CONFIGURATION},
        {"[Info]", TokenType::INFO},
        {"[Export]", TokenType::EXPORT},
        {"[Name]", TokenType::NAME},
        {"[OriginType]", TokenType::ORIGIN_TYPE}
    };
    
    // 初始化类型标识符映射
    typeIdentifierMap = {
        {"@Style", TokenType::TYPE_STYLE},
        {"@Element", TokenType::TYPE_ELEMENT},
        {"@Var", TokenType::TYPE_VAR},
        {"@Html", TokenType::TYPE_HTML},
        {"@JavaScript", TokenType::TYPE_JAVASCRIPT},
        {"@Config", TokenType::TYPE_CONFIG},
        {"@Chtl", TokenType::TYPE_CHTL},
        {"@CJmod", TokenType::TYPE_CJMOD}
    };
    
    // 初始化Token类型名称映射
    tokenTypeNameMap = {
        {TokenType::EOF_TOKEN, "EOF"},
        {TokenType::NEWLINE, "NEWLINE"},
        {TokenType::WHITESPACE, "WHITESPACE"},
        {TokenType::LEFT_BRACE, "LEFT_BRACE"},
        {TokenType::RIGHT_BRACE, "RIGHT_BRACE"},
        {TokenType::LEFT_BRACKET, "LEFT_BRACKET"},
        {TokenType::RIGHT_BRACKET, "RIGHT_BRACKET"},
        {TokenType::LEFT_PAREN, "LEFT_PAREN"},
        {TokenType::RIGHT_PAREN, "RIGHT_PAREN"},
        {TokenType::SEMICOLON, "SEMICOLON"},
        {TokenType::COLON, "COLON"},
        {TokenType::EQUALS, "EQUALS"},
        {TokenType::COMMA, "COMMA"},
        {TokenType::DOT, "DOT"},
        {TokenType::AT, "AT"},
        {TokenType::HASH, "HASH"},
        {TokenType::AMPERSAND, "AMPERSAND"},
        {TokenType::STRING_LITERAL, "STRING_LITERAL"},
        {TokenType::UNQUOTED_LITERAL, "UNQUOTED_LITERAL"},
        {TokenType::NUMBER, "NUMBER"},
        {TokenType::IDENTIFIER, "IDENTIFIER"},
        {TokenType::SINGLE_COMMENT, "SINGLE_COMMENT"},
        {TokenType::MULTI_COMMENT, "MULTI_COMMENT"},
        {TokenType::GEN_COMMENT, "GEN_COMMENT"},
        {TokenType::TEXT, "TEXT"},
        {TokenType::STYLE, "STYLE"},
        {TokenType::SCRIPT, "SCRIPT"},
        {TokenType::HTML_TAG, "HTML_TAG"},
        {TokenType::INVALID, "INVALID"}
    };
    
    mapsInitialized = true;
}

bool Token::IsKeyword() const {
    return TokenUtils::IsKeyword(value);
}

bool Token::IsTypeIdentifier() const {
    return TokenUtils::IsTypeIdentifier(value);
}

bool Token::IsPrefixKeyword() const {
    return TokenUtils::IsPrefixKeyword(value);
}

bool Token::IsHTMLTag() const {
    return TokenUtils::IsHTMLTag(value);
}

std::string Token::ToString() const {
    return TokenUtils::TokenTypeToString(type) + ": \"" + value + "\" at " + 
           std::to_string(line) + ":" + std::to_string(column);
}

std::string TokenUtils::TokenTypeToString(TokenType type) {
    InitializeMaps();
    auto it = tokenTypeNameMap.find(type);
    if (it != tokenTypeNameMap.end()) {
        return it->second;
    }
    return "UNKNOWN";
}

TokenType TokenUtils::StringToTokenType(const std::string& str) {
    InitializeMaps();
    
    // 检查是否是关键字
    auto it = keywordMap.find(str);
    if (it != keywordMap.end()) {
        return it->second;
    }
    
    // 检查是否是前缀关键字
    it = prefixKeywordMap.find(str);
    if (it != prefixKeywordMap.end()) {
        return it->second;
    }
    
    // 检查是否是类型标识符
    it = typeIdentifierMap.find(str);
    if (it != typeIdentifierMap.end()) {
        return it->second;
    }
    
    // 检查是否是HTML标签
    if (IsHTMLTag(str)) {
        return TokenType::HTML_TAG;
    }
    
    return TokenType::IDENTIFIER;
}

bool TokenUtils::IsKeyword(const std::string& str) {
    InitializeMaps();
    return keywordMap.find(str) != keywordMap.end();
}

bool TokenUtils::IsHTMLTag(const std::string& str) {
    std::string lower = str;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    return std::find(htmlTags.begin(), htmlTags.end(), lower) != htmlTags.end();
}

bool TokenUtils::IsPrefixKeyword(const std::string& str) {
    InitializeMaps();
    return prefixKeywordMap.find(str) != prefixKeywordMap.end();
}

bool TokenUtils::IsTypeIdentifier(const std::string& str) {
    InitializeMaps();
    return typeIdentifierMap.find(str) != typeIdentifierMap.end();
}

} // namespace CHTL