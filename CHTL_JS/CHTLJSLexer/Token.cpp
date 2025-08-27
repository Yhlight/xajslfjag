#include "Token.h"
#include <unordered_set>
#include <unordered_map>

namespace CHTL_JS {

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
        case TokenType::ARROW: return "ARROW";
        case TokenType::AMPERSAND: return "AMPERSAND";
        
        case TokenType::SCRIPT: return "SCRIPT";
        case TokenType::LISTEN: return "LISTEN";
        case TokenType::DELEGATE: return "DELEGATE";
        case TokenType::ANIMATE: return "ANIMATE";
        case TokenType::VIR: return "VIR";
        
        case TokenType::ENHANCED_SELECTOR: return "ENHANCED_SELECTOR";
        case TokenType::CLASS_SELECTOR: return "CLASS_SELECTOR";
        case TokenType::ID_SELECTOR: return "ID_SELECTOR";
        case TokenType::TAG_SELECTOR: return "TAG_SELECTOR";
        

        
        case TokenType::TEMPLATE_VAR: return "TEMPLATE_VAR";
        case TokenType::CUSTOM_VAR: return "CUSTOM_VAR";
        
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
    // HTML5标准标签列表 (与CHTL相同)
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
    // CHTL JS关键字映射表
    static const std::unordered_map<std::string, TokenType> keywords = {
        // CHTL JS特有关键字
        {"script", TokenType::SCRIPT},
        {"listen", TokenType::LISTEN},
        {"delegate", TokenType::DELEGATE},
        {"animate", TokenType::ANIMATE},
        {"vir", TokenType::VIR},
        

    };
    
    auto it = keywords.find(str);
    if (it != keywords.end()) {
        return it->second;
    }
    
    // 检查是否为HTML标签 (在CHTL JS中作为TAG_SELECTOR)
    if (isHTMLTag(str)) {
        return TokenType::TAG_SELECTOR;
    }
    
    return TokenType::IDENTIFIER;
}

} // namespace CHTL_JS