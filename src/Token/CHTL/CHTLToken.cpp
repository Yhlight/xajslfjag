#include "CHTLToken.h"
#include <algorithm>
#include <cctype>

namespace CHTL {
namespace Token {

// 静态成员初始化
std::vector<std::string> TokenUtils::htmlTags;
std::unordered_map<TokenType, std::string> TokenUtils::tokenTypeNames;

// CHTLToken方法实现
std::string CHTLToken::toString() const {
    return "Token[" + TokenUtils::getTokenTypeName(type) + ": \"" + value + "\" at " + 
           std::to_string(line) + ":" + std::to_string(column) + "]";
}

bool CHTLToken::isKeywordBlock() const {
    return type == TokenType::TEMPLATE || type == TokenType::CUSTOM || 
           type == TokenType::ORIGIN || type == TokenType::IMPORT ||
           type == TokenType::CONFIGURATION || type == TokenType::NAMESPACE ||
           type == TokenType::INFO || type == TokenType::EXPORT;
}

bool CHTLToken::isTypeIdentifier() const {
    return type == TokenType::STYLE_TYPE || type == TokenType::ELEMENT_TYPE ||
           type == TokenType::VAR_TYPE || type == TokenType::HTML_TYPE ||
           type == TokenType::JAVASCRIPT_TYPE || type == TokenType::CONFIG_TYPE ||
           type == TokenType::CHTL_TYPE || type == TokenType::CJMOD_TYPE;
}

bool CHTLToken::isHTMLTag() const {
    return type == TokenType::HTML_TAG;
}

// TokenUtils方法实现
const std::vector<std::string>& TokenUtils::getHTMLTags() {
    if (htmlTags.empty()) {
        initializeHTMLTags();
    }
    return htmlTags;
}

bool TokenUtils::isHTMLTag(const std::string& tag) {
    if (htmlTags.empty()) {
        initializeHTMLTags();
    }
    
    std::string lowerTag = tag;
    std::transform(lowerTag.begin(), lowerTag.end(), lowerTag.begin(), ::tolower);
    
    return std::find(htmlTags.begin(), htmlTags.end(), lowerTag) != htmlTags.end();
}

std::string TokenUtils::getTokenTypeName(TokenType type) {
    if (tokenTypeNames.empty()) {
        initializeTokenTypeNames();
    }
    
    auto it = tokenTypeNames.find(type);
    return it != tokenTypeNames.end() ? it->second : "UNKNOWN";
}

bool TokenUtils::isIdentifierChar(char c) {
    return std::isalnum(c) || c == '_' || c == '-';
}

bool TokenUtils::isDigit(char c) {
    return std::isdigit(c);
}

bool TokenUtils::isWhitespace(char c) {
    return c == ' ' || c == '\t' || c == '\r';
}

bool TokenUtils::isEquivalentSymbol(char c) {
    return c == ':' || c == '='; // CE对等式
}

void TokenUtils::initializeHTMLTags() {
    // 根据CHTL语法文档，支持所有HTML标签
    htmlTags = {
        // 基本结构标签
        "html", "head", "body", "title", "meta", "link", "style", "script",
        
        // 文档结构标签
        "header", "nav", "main", "section", "article", "aside", "footer",
        "h1", "h2", "h3", "h4", "h5", "h6", "p", "div", "span",
        
        // 列表标签
        "ul", "ol", "li", "dl", "dt", "dd",
        
        // 表格标签
        "table", "thead", "tbody", "tfoot", "tr", "th", "td", "caption",
        "colgroup", "col",
        
        // 表单标签
        "form", "input", "textarea", "select", "option", "optgroup",
        "button", "label", "fieldset", "legend", "datalist", "output",
        
        // 媒体标签
        "img", "video", "audio", "source", "track", "canvas", "svg",
        "figure", "figcaption", "picture",
        
        // 文本格式化标签
        "a", "strong", "b", "em", "i", "u", "s", "small", "sub", "sup",
        "mark", "del", "ins", "q", "cite", "abbr", "time", "code",
        "kbd", "samp", "var", "pre", "blockquote",
        
        // 交互标签
        "details", "summary", "dialog", "menu", "menuitem",
        
        // 内联框架和嵌入
        "iframe", "embed", "object", "param",
        
        // 其他标签
        "br", "hr", "wbr", "area", "map", "base", "noscript",
        "template", "slot", "progress", "meter"
    };
    
    // 排序以便快速查找
    std::sort(htmlTags.begin(), htmlTags.end());
}

void TokenUtils::initializeTokenTypeNames() {
    tokenTypeNames = {
        {TokenType::IDENTIFIER, "IDENTIFIER"},
        {TokenType::STRING_LITERAL, "STRING_LITERAL"},
        {TokenType::SINGLE_STRING_LITERAL, "SINGLE_STRING_LITERAL"},
        {TokenType::UNQUOTED_LITERAL, "UNQUOTED_LITERAL"},
        {TokenType::NUMBER_LITERAL, "NUMBER_LITERAL"},
        
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
        {TokenType::HASHTAG, "HASHTAG"},
        
        {TokenType::SINGLE_COMMENT, "SINGLE_COMMENT"},
        {TokenType::MULTI_COMMENT, "MULTI_COMMENT"},
        {TokenType::GENERATOR_COMMENT, "GENERATOR_COMMENT"},
        
        {TokenType::TEMPLATE, "TEMPLATE"},
        {TokenType::CUSTOM, "CUSTOM"},
        {TokenType::ORIGIN, "ORIGIN"},
        {TokenType::IMPORT, "IMPORT"},
        {TokenType::CONFIGURATION, "CONFIGURATION"},
        {TokenType::NAMESPACE, "NAMESPACE"},
        {TokenType::INFO, "INFO"},
        {TokenType::EXPORT, "EXPORT"},
        
        {TokenType::STYLE_TYPE, "STYLE_TYPE"},
        {TokenType::ELEMENT_TYPE, "ELEMENT_TYPE"},
        {TokenType::VAR_TYPE, "VAR_TYPE"},
        {TokenType::HTML_TYPE, "HTML_TYPE"},
        {TokenType::JAVASCRIPT_TYPE, "JAVASCRIPT_TYPE"},
        {TokenType::CONFIG_TYPE, "CONFIG_TYPE"},
        {TokenType::CHTL_TYPE, "CHTL_TYPE"},
        {TokenType::CJMOD_TYPE, "CJMOD_TYPE"},
        
        {TokenType::TEXT, "TEXT"},
        {TokenType::STYLE, "STYLE"},
        {TokenType::SCRIPT, "SCRIPT"},
        {TokenType::INHERIT, "INHERIT"},
        {TokenType::DELETE, "DELETE"},
        {TokenType::INSERT, "INSERT"},
        {TokenType::AFTER, "AFTER"},
        {TokenType::BEFORE, "BEFORE"},
        {TokenType::REPLACE, "REPLACE"},
        {TokenType::AT_TOP, "AT_TOP"},
        {TokenType::AT_BOTTOM, "AT_BOTTOM"},
        {TokenType::FROM, "FROM"},
        {TokenType::AS, "AS"},
        {TokenType::EXCEPT, "EXCEPT"},
        {TokenType::USE, "USE"},
        {TokenType::VIR, "VIR"},
        
        {TokenType::HTML5, "HTML5"},
        {TokenType::HTML_TAG, "HTML_TAG"},
        
        {TokenType::AMPERSAND, "AMPERSAND"},
        {TokenType::ARROW, "ARROW"},
        
        {TokenType::EOF_TOKEN, "EOF_TOKEN"},
        {TokenType::NEWLINE, "NEWLINE"},
        {TokenType::WHITESPACE, "WHITESPACE"},
        
        {TokenType::UNKNOWN, "UNKNOWN"},
        {TokenType::ERROR, "ERROR"}
    };
}

} // namespace Token
} // namespace CHTL