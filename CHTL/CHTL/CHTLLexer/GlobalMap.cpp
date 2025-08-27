#include "GlobalMap.h"
#include <algorithm>

namespace CHTL {

GlobalMap::GlobalMap() {
    initKeywords();
    initBlockTypes();
    initTypeIdentifiers();
    initOperators();
    initHtmlElements();
}

void GlobalMap::initKeywords() {
    keywords_["text"] = TokenType::TEXT;
    keywords_["style"] = TokenType::STYLE;
    keywords_["script"] = TokenType::SCRIPT;
    keywords_["inherit"] = TokenType::INHERIT;
    keywords_["delete"] = TokenType::DELETE;
    keywords_["insert"] = TokenType::INSERT;
    keywords_["after"] = TokenType::AFTER;
    keywords_["before"] = TokenType::BEFORE;
    keywords_["replace"] = TokenType::REPLACE;
    keywords_["at"] = TokenType::AT_TOP;      // 将在解析时与"top"组合
    keywords_["from"] = TokenType::FROM;
    keywords_["as"] = TokenType::AS;
    keywords_["except"] = TokenType::EXCEPT;
    keywords_["use"] = TokenType::USE;
    keywords_["html5"] = TokenType::HTML5;
}

void GlobalMap::initBlockTypes() {
    block_types_["[Template]"] = TokenType::TEMPLATE;
    block_types_["[Custom]"] = TokenType::CUSTOM;
    block_types_["[Origin]"] = TokenType::ORIGIN;
    block_types_["[Import]"] = TokenType::IMPORT;
    block_types_["[Configuration]"] = TokenType::CONFIGURATION;
    block_types_["[Namespace]"] = TokenType::NAMESPACE;
    block_types_["[Info]"] = TokenType::INFO;
    block_types_["[Export]"] = TokenType::EXPORT;
}

void GlobalMap::initTypeIdentifiers() {
    type_identifiers_["@Style"] = TokenType::STYLE_TYPE;
    type_identifiers_["@Element"] = TokenType::ELEMENT_TYPE;
    type_identifiers_["@Var"] = TokenType::VAR_TYPE;
    type_identifiers_["@Html"] = TokenType::HTML_TYPE;
    type_identifiers_["@JavaScript"] = TokenType::JAVASCRIPT_TYPE;
    type_identifiers_["@Config"] = TokenType::CONFIG_TYPE;
    type_identifiers_["@Chtl"] = TokenType::CHTL_TYPE;
    type_identifiers_["@CJmod"] = TokenType::CJMOD_TYPE;
}

void GlobalMap::initOperators() {
    operators_["->"] = TokenType::ARROW;
    operators_["&->"] = TokenType::BIND_OPERATOR;
}

void GlobalMap::initHtmlElements() {
    // HTML5标准元素
    std::vector<std::string> elements = {
        // 根元素
        "html",
        
        // 文档元数据
        "head", "title", "base", "link", "meta", "style",
        
        // 分区根元素
        "body",
        
        // 内容分区
        "address", "article", "aside", "footer", "header", "h1", "h2", "h3", "h4", "h5", "h6",
        "main", "nav", "section",
        
        // 文本内容
        "blockquote", "dd", "div", "dl", "dt", "figcaption", "figure", "hr", "li", "ol", "p",
        "pre", "ul",
        
        // 内联文本语义
        "a", "abbr", "b", "bdi", "bdo", "br", "cite", "code", "data", "dfn", "em", "i", "kbd",
        "mark", "nobr", "q", "rb", "rp", "rt", "rtc", "ruby", "s", "samp", "small", "span",
        "strong", "sub", "sup", "time", "u", "var", "wbr",
        
        // 图片和多媒体
        "area", "audio", "img", "map", "track", "video",
        
        // 内嵌内容
        "embed", "iframe", "object", "param", "picture", "portal", "source",
        
        // SVG和MathML
        "svg", "math",
        
        // 脚本
        "canvas", "noscript", "script",
        
        // 编辑标识
        "del", "ins",
        
        // 表格内容
        "caption", "col", "colgroup", "table", "tbody", "td", "tfoot", "th", "thead", "tr",
        
        // 表单
        "button", "datalist", "fieldset", "form", "input", "label", "legend", "meter", "optgroup",
        "option", "output", "progress", "select", "textarea",
        
        // 交互元素
        "details", "dialog", "menu", "summary",
        
        // Web组件
        "slot", "template"
    };
    
    for (const auto& element : elements) {
        html_elements_.insert(element);
    }
}

TokenType GlobalMap::getKeywordType(const std::string& keyword) const {
    auto it = keywords_.find(keyword);
    return (it != keywords_.end()) ? it->second : TokenType::IDENTIFIER;
}

TokenType GlobalMap::getBlockType(const std::string& block_type) const {
    auto it = block_types_.find(block_type);
    return (it != block_types_.end()) ? it->second : TokenType::UNKNOWN;
}

TokenType GlobalMap::getTypeIdentifier(const std::string& type_id) const {
    auto it = type_identifiers_.find(type_id);
    if (it != type_identifiers_.end()) {
        return it->second;
    }
    
    // 检查是否为自定义原始嵌入类型
    if (isCustomOriginType(type_id)) {
        return TokenType::IDENTIFIER; // 自定义类型作为标识符处理
    }
    
    return TokenType::UNKNOWN;
}

TokenType GlobalMap::getOperatorType(const std::string& op) const {
    auto it = operators_.find(op);
    return (it != operators_.end()) ? it->second : TokenType::UNKNOWN;
}

bool GlobalMap::isHtmlElement(const std::string& element) const {
    return html_elements_.find(element) != html_elements_.end();
}

void GlobalMap::addCustomOriginType(const std::string& type_name, const std::string& description) {
    custom_origin_types_[type_name] = description;
}

bool GlobalMap::isCustomOriginType(const std::string& type_name) const {
    return custom_origin_types_.find(type_name) != custom_origin_types_.end();
}

std::vector<std::string> GlobalMap::getAllKeywords() const {
    std::vector<std::string> keywords;
    for (const auto& pair : keywords_) {
        keywords.push_back(pair.first);
    }
    std::sort(keywords.begin(), keywords.end());
    return keywords;
}

std::vector<std::string> GlobalMap::getAllHtmlElements() const {
    std::vector<std::string> elements;
    for (const auto& element : html_elements_) {
        elements.push_back(element);
    }
    std::sort(elements.begin(), elements.end());
    return elements;
}

bool GlobalMap::isReservedWord(const std::string& str) const {
    return keywords_.find(str) != keywords_.end() ||
           block_types_.find(str) != block_types_.end() ||
           type_identifiers_.find(str) != type_identifiers_.end() ||
           operators_.find(str) != operators_.end();
}

void GlobalMap::reset() {
    keywords_.clear();
    block_types_.clear();
    type_identifiers_.clear();
    operators_.clear();
    html_elements_.clear();
    custom_origin_types_.clear();
    
    initKeywords();
    initBlockTypes();
    initTypeIdentifiers();
    initOperators();
    initHtmlElements();
}

} // namespace CHTL