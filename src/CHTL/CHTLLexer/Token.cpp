#include "Token.h"
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <cctype>
#include <sstream>

namespace CHTL {

// Token类型名称映射表
static const std::unordered_map<TokenType, const char*> tokenTypeNames = {
    {TokenType::STRING_LITERAL, "STRING_LITERAL"},
    {TokenType::UNQUOTED_LITERAL, "UNQUOTED_LITERAL"},
    {TokenType::NUMBER_LITERAL, "NUMBER_LITERAL"},
    {TokenType::IDENTIFIER, "IDENTIFIER"},
    {TokenType::HTML_TAG, "HTML_TAG"},
    {TokenType::KEYWORD_TEXT, "text"},
    {TokenType::KEYWORD_STYLE, "style"},
    {TokenType::KEYWORD_SCRIPT, "script"},
    {TokenType::KEYWORD_TEMPLATE, "[Template]"},
    {TokenType::KEYWORD_CUSTOM, "[Custom]"},
    {TokenType::KEYWORD_ORIGIN, "[Origin]"},
    {TokenType::KEYWORD_IMPORT, "[Import]"},
    {TokenType::KEYWORD_CONFIGURATION, "[Configuration]"},
    {TokenType::KEYWORD_NAMESPACE, "[Namespace]"},
    {TokenType::KEYWORD_INFO, "[Info]"},
    {TokenType::KEYWORD_EXPORT, "[Export]"},
    {TokenType::KEYWORD_INHERIT, "inherit"},
    {TokenType::KEYWORD_DELETE, "delete"},
    {TokenType::KEYWORD_INSERT, "insert"},
    {TokenType::KEYWORD_USE, "use"},
    {TokenType::KEYWORD_EXCEPT, "except"},
    {TokenType::KEYWORD_FROM, "from"},
    {TokenType::KEYWORD_AS, "as"},
    {TokenType::KEYWORD_AFTER, "after"},
    {TokenType::KEYWORD_BEFORE, "before"},
    {TokenType::KEYWORD_REPLACE, "replace"},
    {TokenType::KEYWORD_AT_TOP, "at top"},
    {TokenType::KEYWORD_AT_BOTTOM, "at bottom"},
    {TokenType::KEYWORD_HTML5, "html5"},
    {TokenType::TYPE_STYLE, "@Style"},
    {TokenType::TYPE_ELEMENT, "@Element"},
    {TokenType::TYPE_VAR, "@Var"},
    {TokenType::TYPE_HTML, "@Html"},
    {TokenType::TYPE_JAVASCRIPT, "@JavaScript"},
    {TokenType::TYPE_CHTL, "@Chtl"},
    {TokenType::TYPE_CJMOD, "@CJmod"},
    {TokenType::TYPE_CONFIG, "@Config"},
    {TokenType::TYPE_CUSTOM_ORIGIN, "@CustomOrigin"},
    {TokenType::COLON, ":"},
    {TokenType::EQUAL, "="},
    {TokenType::SEMICOLON, ";"},
    {TokenType::COMMA, ","},
    {TokenType::DOT, "."},
    {TokenType::AMPERSAND, "&"},
    {TokenType::HASH, "#"},
    {TokenType::AT, "@"},
    {TokenType::LEFT_BRACE, "{"},
    {TokenType::RIGHT_BRACE, "}"},
    {TokenType::LEFT_BRACKET, "["},
    {TokenType::RIGHT_BRACKET, "]"},
    {TokenType::LEFT_PAREN, "("},
    {TokenType::RIGHT_PAREN, ")"},
    {TokenType::SINGLE_LINE_COMMENT, "SINGLE_LINE_COMMENT"},
    {TokenType::MULTI_LINE_COMMENT, "MULTI_LINE_COMMENT"},
    {TokenType::GENERATOR_COMMENT, "GENERATOR_COMMENT"},
    {TokenType::WHITESPACE, "WHITESPACE"},
    {TokenType::NEWLINE, "NEWLINE"},
    {TokenType::EOF_TOKEN, "EOF"},
    {TokenType::UNKNOWN, "UNKNOWN"}
};

// 关键字映射表
static const std::unordered_map<std::string, TokenType> keywordMap = {
    {"text", TokenType::KEYWORD_TEXT},
    {"style", TokenType::KEYWORD_STYLE},
    {"script", TokenType::KEYWORD_SCRIPT},
    {"[Template]", TokenType::KEYWORD_TEMPLATE},
    {"[Custom]", TokenType::KEYWORD_CUSTOM},
    {"[Origin]", TokenType::KEYWORD_ORIGIN},
    {"[Import]", TokenType::KEYWORD_IMPORT},
    {"[Configuration]", TokenType::KEYWORD_CONFIGURATION},
    {"[Namespace]", TokenType::KEYWORD_NAMESPACE},
    {"[Info]", TokenType::KEYWORD_INFO},
    {"[Export]", TokenType::KEYWORD_EXPORT},
    {"inherit", TokenType::KEYWORD_INHERIT},
    {"delete", TokenType::KEYWORD_DELETE},
    {"insert", TokenType::KEYWORD_INSERT},
    {"use", TokenType::KEYWORD_USE},
    {"except", TokenType::KEYWORD_EXCEPT},
    {"from", TokenType::KEYWORD_FROM},
    {"as", TokenType::KEYWORD_AS},
    {"after", TokenType::KEYWORD_AFTER},
    {"before", TokenType::KEYWORD_BEFORE},
    {"replace", TokenType::KEYWORD_REPLACE},
    {"at", TokenType::KEYWORD_AT_TOP},  // 特殊处理 "at top" 和 "at bottom"
    {"html5", TokenType::KEYWORD_HTML5}
};

// 类型标识符映射表
static const std::unordered_map<std::string, TokenType> typeIdentifierMap = {
    {"@Style", TokenType::TYPE_STYLE},
    {"@Element", TokenType::TYPE_ELEMENT},
    {"@Var", TokenType::TYPE_VAR},
    {"@Html", TokenType::TYPE_HTML},
    {"@JavaScript", TokenType::TYPE_JAVASCRIPT},
    {"@Chtl", TokenType::TYPE_CHTL},
    {"@CJmod", TokenType::TYPE_CJMOD},
    {"@Config", TokenType::TYPE_CONFIG}
};

// HTML标签列表（常见标签）
static const std::unordered_set<std::string> htmlTags = {
    "html", "head", "body", "div", "span", "p", "a", "img", "ul", "ol", "li",
    "table", "tr", "td", "th", "form", "input", "button", "select", "option",
    "textarea", "label", "h1", "h2", "h3", "h4", "h5", "h6", "header", "footer",
    "nav", "section", "article", "aside", "main", "figure", "figcaption",
    "video", "audio", "canvas", "svg", "iframe", "embed", "object", "param",
    "meta", "link", "title", "base", "br", "hr", "area", "col", "wbr",
    "strong", "em", "b", "i", "u", "s", "mark", "small", "del", "ins", "sub", "sup",
    "code", "pre", "kbd", "samp", "var", "cite", "q", "blockquote", "abbr", "address",
    "details", "summary", "dialog", "menu", "menuitem", "data", "time", "progress", "meter"
};

std::string Token::toString() const {
    std::stringstream ss;
    ss << "Token[" << getTokenTypeName(type_) << ", \"" << lexeme_ << "\", "
       << location_.line << ":" << location_.column << "]";
    return ss.str();
}

const char* getTokenTypeName(TokenType type) {
    auto it = tokenTypeNames.find(type);
    if (it != tokenTypeNames.end()) {
        return it->second;
    }
    return "UNKNOWN_TYPE";
}

TokenType getKeywordType(const std::string& keyword) {
    auto it = keywordMap.find(keyword);
    if (it != keywordMap.end()) {
        return it->second;
    }
    
    // 特殊处理 "at top" 和 "at bottom"
    if (keyword == "at top") {
        return TokenType::KEYWORD_AT_TOP;
    } else if (keyword == "at bottom") {
        return TokenType::KEYWORD_AT_BOTTOM;
    }
    
    // 检查是否为HTML标签
    std::string lowerKeyword = keyword;
    std::transform(lowerKeyword.begin(), lowerKeyword.end(), lowerKeyword.begin(), ::tolower);
    if (htmlTags.find(lowerKeyword) != htmlTags.end()) {
        return TokenType::HTML_TAG;
    }
    
    return TokenType::IDENTIFIER;
}

TokenType getTypeIdentifierType(const std::string& typeId) {
    auto it = typeIdentifierMap.find(typeId);
    if (it != typeIdentifierMap.end()) {
        return it->second;
    }
    
    // 检查是否为自定义原始嵌入类型（以@开头）
    if (!typeId.empty() && typeId[0] == '@') {
        return TokenType::TYPE_CUSTOM_ORIGIN;
    }
    
    return TokenType::IDENTIFIER;
}

} // namespace CHTL