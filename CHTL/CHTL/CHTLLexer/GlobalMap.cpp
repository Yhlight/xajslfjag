#include "GlobalMap.h"

namespace CHTL {

GlobalMap* GlobalMap::instance = nullptr;

GlobalMap::GlobalMap() {
    initializeDefaultKeywords();
    initializeHtmlElements();
}

GlobalMap* GlobalMap::getInstance() {
    if (instance == nullptr) {
        instance = new GlobalMap();
    }
    return instance;
}

void GlobalMap::destroyInstance() {
    if (instance != nullptr) {
        delete instance;
        instance = nullptr;
    }
}

TokenType GlobalMap::getKeywordType(const std::string& word) const {
    auto it = keywordMap.find(word);
    if (it != keywordMap.end()) {
        return it->second;
    }
    
    // 检查可配置关键字
    for (const auto& [configKey, values] : configurableKeywords) {
        for (const auto& value : values) {
            if (value == word) {
                // 根据配置键返回对应的TokenType
                if (configKey == "CUSTOM_STYLE" || configKey == "TEMPLATE_STYLE") {
                    return TokenType::KEYWORD_STYLE;
                }
                // ... 其他映射
            }
        }
    }
    
    return TokenType::UNKNOWN;
}

bool GlobalMap::isHtmlElement(const std::string& element) const {
    return htmlElements.find(element) != htmlElements.end();
}

void GlobalMap::updateConfigurableKeyword(const std::string& key, const std::vector<std::string>& values) {
    configurableKeywords[key] = values;
}

void GlobalMap::initializeDefaultKeywords() {
    // 基础关键字
    keywordMap["text"] = TokenType::KEYWORD_TEXT;
    keywordMap["style"] = TokenType::KEYWORD_STYLE;
    keywordMap["script"] = TokenType::KEYWORD_SCRIPT;
    keywordMap["[Custom]"] = TokenType::KEYWORD_CUSTOM;
    keywordMap["[Template]"] = TokenType::KEYWORD_TEMPLATE;
    keywordMap["[Origin]"] = TokenType::KEYWORD_ORIGIN;
    keywordMap["[Import]"] = TokenType::KEYWORD_IMPORT;
    keywordMap["[Namespace]"] = TokenType::KEYWORD_NAMESPACE;
    keywordMap["[Configuration]"] = TokenType::KEYWORD_CONFIGURATION;
    keywordMap["[Info]"] = TokenType::KEYWORD_INFO;
    keywordMap["[Export]"] = TokenType::KEYWORD_EXPORT;
    keywordMap["use"] = TokenType::KEYWORD_USE;
    keywordMap["delete"] = TokenType::KEYWORD_DELETE;
    keywordMap["insert"] = TokenType::KEYWORD_INSERT;
    keywordMap["inherit"] = TokenType::KEYWORD_INHERIT;
    keywordMap["from"] = TokenType::KEYWORD_FROM;
    keywordMap["as"] = TokenType::KEYWORD_AS;
    keywordMap["except"] = TokenType::KEYWORD_EXCEPT;
    keywordMap["after"] = TokenType::KEYWORD_AFTER;
    keywordMap["before"] = TokenType::KEYWORD_BEFORE;
    keywordMap["replace"] = TokenType::KEYWORD_REPLACE;
    keywordMap["at"] = TokenType::KEYWORD_AT;
    keywordMap["top"] = TokenType::KEYWORD_TOP;
    keywordMap["bottom"] = TokenType::KEYWORD_BOTTOM;
    
    // 初始化默认的可配置关键字
    configurableKeywords["CUSTOM_STYLE"] = {"@Style", "@style", "@CSS", "@Css", "@css"};
    configurableKeywords["CUSTOM_ELEMENT"] = {"@Element"};
    configurableKeywords["CUSTOM_VAR"] = {"@Var"};
    configurableKeywords["TEMPLATE_STYLE"] = {"@Style"};
    configurableKeywords["TEMPLATE_ELEMENT"] = {"@Element"};
    configurableKeywords["TEMPLATE_VAR"] = {"@Var"};
    configurableKeywords["ORIGIN_HTML"] = {"@Html"};
    configurableKeywords["ORIGIN_STYLE"] = {"@Style"};
    configurableKeywords["ORIGIN_JAVASCRIPT"] = {"@JavaScript"};
    configurableKeywords["CONFIGURATION_CONFIG"] = {"@Config"};
    configurableKeywords["IMPORT_HTML"] = {"@Html"};
    configurableKeywords["IMPORT_STYLE"] = {"@Style"};
    configurableKeywords["IMPORT_JAVASCRIPT"] = {"@JavaScript"};
    configurableKeywords["IMPORT_CHTL"] = {"@Chtl"};
    configurableKeywords["IMPORT_CJMOD"] = {"@CJmod"};
    configurableKeywords["IMPORT_CONFIG"] = {"@Config"};
}

void GlobalMap::initializeHtmlElements() {
    // HTML5 标准元素
    const std::vector<std::string> elements = {
        // 文档元素
        "html", "head", "body", "title", "meta", "link", "style", "script", "base",
        
        // 文本内容
        "div", "span", "p", "h1", "h2", "h3", "h4", "h5", "h6",
        "blockquote", "pre", "code", "em", "strong", "i", "b", "u", "s",
        "small", "big", "sub", "sup", "mark", "del", "ins", "cite", "q",
        "abbr", "address", "time", "var", "samp", "kbd", "dfn", "bdo", "bdi",
        
        // 列表
        "ul", "ol", "li", "dl", "dt", "dd",
        
        // 表格
        "table", "thead", "tbody", "tfoot", "tr", "th", "td", "caption",
        "col", "colgroup",
        
        // 表单
        "form", "input", "textarea", "button", "select", "option", "optgroup",
        "label", "fieldset", "legend", "datalist", "output", "progress", "meter",
        
        // 媒体
        "img", "audio", "video", "source", "track", "picture", "canvas", "svg",
        "iframe", "embed", "object", "param",
        
        // 语义化元素
        "header", "nav", "main", "article", "section", "aside", "footer",
        "figure", "figcaption", "details", "summary", "dialog",
        
        // 其他
        "br", "hr", "wbr", "area", "map", "template", "slot",
        "ruby", "rt", "rp", "data", "noscript"
    };
    
    for (const auto& elem : elements) {
        htmlElements[elem] = true;
    }
}

void GlobalMap::reset() {
    keywordMap.clear();
    configurableKeywords.clear();
    htmlElements.clear();
    initializeDefaultKeywords();
    initializeHtmlElements();
}

} // namespace CHTL