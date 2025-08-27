#include "Token.h"
#include <unordered_set>
#include <unordered_map>

namespace CHTL {

std::string tokenTypeToString(TokenType type) {
    switch (type) {
        case TokenType::IDENTIFIER: return "IDENTIFIER";
        case TokenType::UNQUOTED_LITERAL: return "UNQUOTED_LITERAL";
        case TokenType::DOUBLE_QUOTED_STRING: return "DOUBLE_QUOTED_STRING";
        case TokenType::SINGLE_QUOTED_STRING: return "SINGLE_QUOTED_STRING";
        case TokenType::NUMBER: return "NUMBER";
        
        case TokenType::LINE_COMMENT: return "LINE_COMMENT";
        case TokenType::BLOCK_COMMENT: return "BLOCK_COMMENT";
        case TokenType::GENERATOR_COMMENT: return "GENERATOR_COMMENT";
        
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
        case TokenType::SLASH: return "SLASH";
        case TokenType::AMPERSAND: return "AMPERSAND";
        
        case TokenType::TEXT: return "TEXT";
        case TokenType::STYLE: return "STYLE";
        case TokenType::SCRIPT: return "SCRIPT";
        
        case TokenType::TEMPLATE: return "TEMPLATE";
        case TokenType::CUSTOM: return "CUSTOM";
        case TokenType::ORIGIN: return "ORIGIN";
        case TokenType::IMPORT: return "IMPORT";
        case TokenType::CONFIGURATION: return "CONFIGURATION";
        case TokenType::NAMESPACE: return "NAMESPACE";
        
        case TokenType::AT_STYLE: return "AT_STYLE";
        case TokenType::AT_ELEMENT: return "AT_ELEMENT";
        case TokenType::AT_VAR: return "AT_VAR";
        case TokenType::AT_HTML: return "AT_HTML";
        case TokenType::AT_JAVASCRIPT: return "AT_JAVASCRIPT";
        case TokenType::AT_CHTL: return "AT_CHTL";
        case TokenType::AT_CJMOD: return "AT_CJMOD";
        case TokenType::AT_CONFIG: return "AT_CONFIG";
        
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
        
        case TokenType::HTML_TAG: return "HTML_TAG";
        case TokenType::CLASS_SELECTOR: return "CLASS_SELECTOR";
        case TokenType::ID_SELECTOR: return "ID_SELECTOR";
        case TokenType::PSEUDO_CLASS: return "PSEUDO_CLASS";
        case TokenType::PSEUDO_ELEMENT: return "PSEUDO_ELEMENT";
        
        case TokenType::EOF_TOKEN: return "EOF_TOKEN";
        case TokenType::NEWLINE: return "NEWLINE";
        case TokenType::WHITESPACE: return "WHITESPACE";
        case TokenType::INVALID: return "INVALID";
        
        default: return "UNKNOWN";
    }
}

std::string Token::toString() const {
    return tokenTypeToString(type) + "(" + getString() + ")";
}

bool isHTMLTag(const std::string& str) {
    // HTML5标准标签列表
    static const std::unordered_set<std::string> htmlTags = {
        // 基础结构
        "html", "head", "body", "title", "meta", "link", "style", "script",
        
        // 分段内容
        "section", "nav", "article", "aside", "header", "footer", "main",
        "address", "h1", "h2", "h3", "h4", "h5", "h6", "hgroup",
        
        // 文本内容
        "div", "p", "hr", "pre", "blockquote", "ol", "ul", "li", "dl", "dt", "dd",
        "figure", "figcaption",
        
        // 内联文本语义
        "a", "em", "strong", "small", "s", "cite", "q", "dfn", "abbr", "ruby",
        "rt", "rp", "data", "time", "code", "var", "samp", "kbd", "sub", "sup",
        "i", "b", "u", "mark", "bdi", "bdo", "span", "br", "wbr",
        
        // 嵌入内容
        "area", "audio", "img", "map", "track", "video", "embed", "iframe",
        "object", "param", "picture", "portal", "source",
        
        // 表格
        "table", "caption", "colgroup", "col", "tbody", "thead", "tfoot",
        "tr", "td", "th",
        
        // 表单
        "form", "label", "input", "button", "select", "datalist", "optgroup",
        "option", "textarea", "output", "progress", "meter", "fieldset",
        "legend",
        
        // 交互元素
        "details", "summary", "dialog",
        
        // 脚本支持
        "canvas", "noscript", "template", "slot"
    };
    
    return htmlTags.find(str) != htmlTags.end();
}

TokenType getKeywordType(const std::string& str) {
    // CHTL关键字映射表
    static const std::unordered_map<std::string, TokenType> keywords = {
        {"text", TokenType::TEXT},
        {"style", TokenType::STYLE},
        {"script", TokenType::SCRIPT},
        
        {"inherit", TokenType::INHERIT},
        {"delete", TokenType::DELETE},
        {"insert", TokenType::INSERT},
        {"after", TokenType::AFTER},
        {"before", TokenType::BEFORE},
        {"replace", TokenType::REPLACE},
        {"from", TokenType::FROM},
        {"as", TokenType::AS},
        {"except", TokenType::EXCEPT},
        
        {"use", TokenType::USE},
        {"html5", TokenType::HTML5}
    };
    
    auto it = keywords.find(str);
    if (it != keywords.end()) {
        return it->second;
    }
    
    // 检查是否为HTML标签
    if (isHTMLTag(str)) {
        return TokenType::HTML_TAG;
    }
    
    return TokenType::IDENTIFIER;
}

} // namespace CHTL