#include "Token.hpp"
#include <unordered_set>

namespace CHTL {

// 关键字映射表的定义
const std::unordered_map<std::string, TokenType> KeywordMap::keywords = {
    {"text", TokenType::TEXT},
    {"style", TokenType::STYLE},
    {"script", TokenType::SCRIPT},
    {"inherit", TokenType::INHERIT},
    {"delete", TokenType::DELETE},
    {"insert", TokenType::INSERT},
    {"after", TokenType::AFTER},
    {"before", TokenType::BEFORE},
    {"replace", TokenType::REPLACE},
    {"at", TokenType::AT_TOP},     // "at top"需要特殊处理
    {"top", TokenType::AT_TOP},
    {"bottom", TokenType::AT_BOTTOM},
    {"from", TokenType::FROM},
    {"as", TokenType::AS},
    {"except", TokenType::EXCEPT},
    {"use", TokenType::USE},
    {"html5", TokenType::HTML5}
};

const std::unordered_map<std::string, TokenType> KeywordMap::blockTypes = {
    {"[Template]", TokenType::TEMPLATE},
    {"[Custom]", TokenType::CUSTOM},
    {"[Origin]", TokenType::ORIGIN},
    {"[Import]", TokenType::IMPORT},
    {"[Configuration]", TokenType::CONFIGURATION},
    {"[Namespace]", TokenType::NAMESPACE},
    {"[Name]", TokenType::NAME},
    {"[OriginType]", TokenType::ORIGIN_TYPE},
    {"[Info]", TokenType::INFO},
    {"[Export]", TokenType::EXPORT}
};

const std::unordered_map<std::string, TokenType> KeywordMap::typePrefix = {
    {"@Style", TokenType::AT_STYLE},
    {"@Element", TokenType::AT_ELEMENT},
    {"@Var", TokenType::AT_VAR},
    {"@Html", TokenType::AT_HTML},
    {"@JavaScript", TokenType::AT_JAVASCRIPT},
    {"@Chtl", TokenType::AT_CHTL},
    {"@CJmod", TokenType::AT_CJMOD},
    {"@Config", TokenType::AT_CONFIG}
};

// HTML元素集合 - 包含所有标准HTML元素
const std::unordered_set<std::string> htmlElements = {
    // 文档结构
    "html", "head", "body", "title", "meta", "link", "style", "script",
    "base", "noscript",
    
    // 文本内容
    "h1", "h2", "h3", "h4", "h5", "h6", "p", "div", "span", "section",
    "article", "aside", "nav", "header", "footer", "main", "figure",
    "figcaption", "details", "summary", "dialog",
    
    // 列表
    "ul", "ol", "li", "dl", "dt", "dd",
    
    // 表格
    "table", "thead", "tbody", "tfoot", "tr", "th", "td", "caption",
    "colgroup", "col",
    
    // 表单
    "form", "input", "button", "select", "option", "optgroup", "textarea",
    "label", "fieldset", "legend", "datalist", "output", "progress", "meter",
    
    // 多媒体
    "img", "video", "audio", "source", "track", "canvas", "svg", "picture",
    
    // 交互元素
    "a", "area", "map", "time", "code", "pre", "blockquote", "cite", "q",
    "abbr", "dfn", "mark", "small", "strong", "em", "b", "i", "u", "s",
    "sub", "sup", "var", "kbd", "samp", "data",
    
    // 分组内容
    "br", "hr", "wbr",
    
    // 嵌入内容
    "iframe", "embed", "object", "param",
    
    // Web组件
    "template", "slot"
};

TokenType KeywordMap::getKeywordType(const std::string& word) {
    auto it = keywords.find(word);
    return (it != keywords.end()) ? it->second : TokenType::UNKNOWN;
}

TokenType KeywordMap::getBlockType(const std::string& word) {
    auto it = blockTypes.find(word);
    return (it != blockTypes.end()) ? it->second : TokenType::UNKNOWN;
}

TokenType KeywordMap::getTypePrefix(const std::string& word) {
    auto it = typePrefix.find(word);
    return (it != typePrefix.end()) ? it->second : TokenType::UNKNOWN;
}

bool KeywordMap::isValidHTMLElement(const std::string& element) {
    return htmlElements.find(element) != htmlElements.end();
}

bool Token::isKeyword() const {
    return type >= TokenType::TEXT && type <= TokenType::HTML5;
}

bool Token::isOperator() const {
    return type >= TokenType::LBRACE && type <= TokenType::AT;
}

bool Token::isLiteral() const {
    return type == TokenType::STRING_LITERAL || 
           type == TokenType::UNQUOTED_LITERAL || 
           type == TokenType::NUMBER;
}

bool Token::isBlockType() const {
    return type >= TokenType::TEMPLATE && type <= TokenType::EXPORT;
}

bool Token::isHTMLElement() const {
    return type == TokenType::HTML_ELEMENT;
}

std::string Token::toString() const {
    return "[" + std::to_string(static_cast<int>(type)) + "] " + value + 
           " @" + std::to_string(line) + ":" + std::to_string(column);
}

}