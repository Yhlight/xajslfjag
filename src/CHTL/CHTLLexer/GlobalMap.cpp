#include "GlobalMap.h"

namespace CHTL {

GlobalMap::GlobalMap() {
    InitializeDefaults();
}

void GlobalMap::InitializeDefaults() {
    InitializeKeywords();
    InitializeSymbols();
    InitializeTypeIdentifiers();
    InitializeBracketTypes();
    InitializeHtmlElements();
    InitializeCssProperties();
}

void GlobalMap::InitializeKeywords() {
    keywords = {
        {"text", TokenType::TEXT},
        {"style", TokenType::STYLE},
        {"script", TokenType::SCRIPT},
        {"use", TokenType::USE},
        {"html5", TokenType::HTML5},
        {"inherit", TokenType::INHERIT},
        {"delete", TokenType::DELETE},
        {"insert", TokenType::INSERT},
        {"after", TokenType::AFTER},
        {"before", TokenType::BEFORE},
        {"replace", TokenType::REPLACE},
        {"at", TokenType::AT_TOP},      // 需要特殊处理 "at top" 和 "at bottom"
        {"top", TokenType::AT_TOP},
        {"bottom", TokenType::AT_BOTTOM},
        {"from", TokenType::FROM},
        {"as", TokenType::AS},
        {"except", TokenType::EXCEPT}
    };
}

void GlobalMap::InitializeSymbols() {
    symbols = {
        {"{", TokenType::LEFT_BRACE},
        {"}", TokenType::RIGHT_BRACE},
        {"[", TokenType::LEFT_BRACKET},
        {"]", TokenType::RIGHT_BRACKET},
        {"(", TokenType::LEFT_PAREN},
        {")", TokenType::RIGHT_PAREN},
        {";", TokenType::SEMICOLON},
        {":", TokenType::COLON},
        {"=", TokenType::EQUALS},
        {",", TokenType::COMMA},
        {".", TokenType::DOT},
        {"#", TokenType::HASH},
        {"&", TokenType::AMPERSAND},
        {"+", TokenType::PLUS},
        {"-", TokenType::MINUS},
        {"*", TokenType::STAR},
        {"/", TokenType::SLASH},
        {">", TokenType::GREATER_THAN}
    };
}

void GlobalMap::InitializeTypeIdentifiers() {
    typeIdentifiers = {
        {"@Style", TokenType::AT_STYLE},
        {"@Element", TokenType::AT_ELEMENT},
        {"@Var", TokenType::AT_VAR},
        {"@Html", TokenType::AT_HTML},
        {"@JavaScript", TokenType::AT_JAVASCRIPT},
        {"@Chtl", TokenType::AT_CHTL},
        {"@CJmod", TokenType::AT_CJMOD},
        {"@Config", TokenType::AT_CONFIG}
    };
}

void GlobalMap::InitializeBracketTypes() {
    bracketTypes = {
        {"[Template]", TokenType::TEMPLATE},
        {"[Custom]", TokenType::CUSTOM},
        {"[Origin]", TokenType::ORIGIN},
        {"[Import]", TokenType::IMPORT},
        {"[Namespace]", TokenType::NAMESPACE},
        {"[Configuration]", TokenType::CONFIGURATION}
    };
}

void GlobalMap::InitializeHtmlElements() {
    htmlElements = {
        // HTML5标准元素
        "html", "head", "body", "title", "meta", "link", "style", "script",
        "div", "span", "p", "h1", "h2", "h3", "h4", "h5", "h6",
        "a", "img", "ul", "ol", "li", "table", "tr", "td", "th", "thead", "tbody", "tfoot",
        "form", "input", "button", "textarea", "select", "option", "label",
        "header", "footer", "nav", "section", "article", "aside", "main",
        "canvas", "svg", "video", "audio", "source",
        "em", "strong", "b", "i", "u", "s", "small", "mark", "del", "ins", "sub", "sup",
        "br", "hr", "wbr", "pre", "code", "kbd", "samp", "var",
        "blockquote", "cite", "q", "abbr", "dfn", "time", "data",
        "address", "figure", "figcaption", "details", "summary", "dialog",
        "fieldset", "legend", "datalist", "output", "progress", "meter"
    };
}

void GlobalMap::InitializeCssProperties() {
    cssProperties = {
        // 常用CSS属性
        "color", "background", "background-color", "background-image", "background-repeat",
        "background-position", "background-size", "background-attachment",
        "width", "height", "min-width", "max-width", "min-height", "max-height",
        "margin", "margin-top", "margin-right", "margin-bottom", "margin-left",
        "padding", "padding-top", "padding-right", "padding-bottom", "padding-left",
        "border", "border-width", "border-style", "border-color",
        "border-top", "border-right", "border-bottom", "border-left",
        "border-radius", "outline", "box-shadow",
        "display", "position", "top", "right", "bottom", "left", "z-index",
        "float", "clear", "overflow", "overflow-x", "overflow-y", "visibility",
        "opacity", "cursor", "pointer-events",
        "font", "font-family", "font-size", "font-weight", "font-style", "font-variant",
        "line-height", "text-align", "text-decoration", "text-transform", "text-indent",
        "text-shadow", "letter-spacing", "word-spacing", "white-space",
        "vertical-align", "list-style", "list-style-type", "list-style-position",
        "list-style-image", "table-layout", "border-collapse", "border-spacing",
        "caption-side", "empty-cells", "quotes",
        "flex", "flex-direction", "flex-wrap", "flex-flow", "justify-content",
        "align-items", "align-content", "order", "flex-grow", "flex-shrink", "flex-basis",
        "align-self", "grid", "grid-template", "grid-area", "gap",
        "transform", "transition", "animation", "filter"
    };
}

TokenType GlobalMap::LookupKeyword(const std::string& word) const {
    auto it = keywords.find(word);
    return it != keywords.end() ? it->second : TokenType::IDENTIFIER;
}

TokenType GlobalMap::LookupSymbol(const std::string& symbol) const {
    auto it = symbols.find(symbol);
    return it != symbols.end() ? it->second : TokenType::UNKNOWN;
}

TokenType GlobalMap::LookupTypeIdentifier(const std::string& identifier) const {
    auto it = typeIdentifiers.find(identifier);
    return it != typeIdentifiers.end() ? it->second : TokenType::IDENTIFIER;
}

TokenType GlobalMap::LookupBracketType(const std::string& bracket) const {
    auto it = bracketTypes.find(bracket);
    return it != bracketTypes.end() ? it->second : TokenType::UNKNOWN;
}

bool GlobalMap::IsValidHtmlElement(const std::string& element) const {
    return htmlElements.find(element) != htmlElements.end();
}

bool GlobalMap::IsValidCssProperty(const std::string& property) const {
    return cssProperties.find(property) != cssProperties.end();
}

void GlobalMap::AddUserTypeIdentifier(const std::string& identifier, TokenType type) {
    typeIdentifiers[identifier] = type;
}

void GlobalMap::AddConfigurationItem(const std::string& key, const std::string& value) {
    configurationItems[key] = value;
}

std::string GlobalMap::GetConfigurationItem(const std::string& key) const {
    auto it = configurationItems.find(key);
    return it != configurationItems.end() ? it->second : "";
}

void GlobalMap::Clear() {
    keywords.clear();
    symbols.clear();
    typeIdentifiers.clear();
    bracketTypes.clear();
    htmlElements.clear();
    cssProperties.clear();
    configurationItems.clear();
}

void GlobalMap::Reset() {
    Clear();
    InitializeDefaults();
}

} // namespace CHTL