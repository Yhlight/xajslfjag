#include "GlobalMap.h"

namespace CHTL {

// HTML元素映射实现
const std::unordered_set<String> HTMLElementMap::htmlElements = {
    "html", "head", "body", "div", "span", "p", "h1", "h2", "h3", "h4", "h5", "h6",
    "a", "img", "ul", "ol", "li", "table", "tr", "td", "th", "form", "input", "button",
    "textarea", "select", "option", "label", "header", "footer", "section", "article",
    "nav", "aside", "main", "figure", "figcaption", "video", "audio", "canvas", "svg",
    "script", "style", "link", "meta", "title", "base", "noscript", "iframe", "embed",
    "object", "param", "source", "track", "map", "area", "details", "summary", "dialog",
    "menu", "menuitem", "command", "keygen", "output", "progress", "meter", "time",
    "mark", "ruby", "rt", "rp", "bdi", "bdo", "wbr", "datalist", "optgroup", "fieldset",
    "legend", "code", "pre", "kbd", "samp", "var", "sub", "sup", "small", "strong",
    "em", "i", "b", "u", "s", "del", "ins", "q", "cite", "abbr", "dfn", "address",
    "blockquote", "dl", "dt", "dd", "hr", "br"
};

const std::unordered_set<String> HTMLElementMap::singleTags = {
    "img", "br", "hr", "input", "meta", "link", "base", "area", "source", "track",
    "embed", "param", "col", "wbr", "keygen"
};

const std::unordered_set<String> HTMLElementMap::blockElements = {
    "div", "p", "h1", "h2", "h3", "h4", "h5", "h6", "ul", "ol", "li", "table", "tr",
    "td", "th", "form", "header", "footer", "section", "article", "nav", "aside",
    "main", "figure", "blockquote", "pre", "hr", "fieldset", "legend", "address",
    "details", "summary", "dl", "dt", "dd"
};

const std::unordered_set<String> HTMLElementMap::inlineElements = {
    "span", "a", "img", "strong", "em", "b", "i", "u", "s", "small", "mark", "del",
    "ins", "sub", "sup", "q", "cite", "abbr", "dfn", "time", "code", "var", "samp",
    "kbd", "input", "button", "select", "textarea", "label", "output", "progress",
    "meter", "ruby", "rt", "rp", "bdi", "bdo", "wbr"
};

bool HTMLElementMap::isHTMLElement(const String& name) {
    return htmlElements.find(name) != htmlElements.end();
}

bool HTMLElementMap::isSingleTag(const String& name) {
    return singleTags.find(name) != singleTags.end();
}

bool HTMLElementMap::isBlockElement(const String& name) {
    return blockElements.find(name) != blockElements.end();
}

bool HTMLElementMap::isInlineElement(const String& name) {
    return inlineElements.find(name) != inlineElements.end();
}

// 关键字映射实现
const StringUnorderedMap KeywordMap::keywordMap = {
    {"text", TokenType::TEXT},
    {"style", TokenType::STYLE},
    {"script", TokenType::SCRIPT},
    {"[Template]", TokenType::TEMPLATE},
    {"[Custom]", TokenType::CUSTOM},
    {"@Style", TokenType::AT_STYLE},
    {"@Element", TokenType::AT_ELEMENT},
    {"@Var", TokenType::AT_VAR},
    {"[Origin]", TokenType::ORIGIN},
    {"@Html", TokenType::AT_HTML},
    {"@JavaScript", TokenType::AT_JAVASCRIPT},
    {"[Import]", TokenType::IMPORT},
    {"from", TokenType::FROM},
    {"as", TokenType::AS},
    {"except", TokenType::EXCEPT},
    {"[Configuration]", TokenType::CONFIGURATION},
    {"[Namespace]", TokenType::NAMESPACE},
    {"inherit", TokenType::INHERIT},
    {"delete", TokenType::DELETE},
    {"insert", TokenType::INSERT},
    {"after", TokenType::AFTER},
    {"before", TokenType::BEFORE},
    {"replace", TokenType::REPLACE},
    {"at top", TokenType::AT_TOP},
    {"at bottom", TokenType::AT_BOTTOM},
    {"use", TokenType::USE},
    {"html5", TokenType::HTML5}
};

const std::unordered_set<String> KeywordMap::chtlKeywords = {
    "text", "style", "[Template]", "[Custom]", "@Style", "@Element", "@Var",
    "[Origin]", "@Html", "@JavaScript", "[Import]", "from", "as", "except",
    "[Configuration]", "[Namespace]", "inherit", "delete", "insert", "after",
    "before", "replace", "at top", "at bottom", "use", "html5"
};

const std::unordered_set<String> KeywordMap::chtljsKeywords = {
    "script", "module", "load", "listen", "animate", "delegate", "vir", "printMyLove",
    "iNeverAway"
};

TokenType KeywordMap::getKeywordType(const String& keyword) {
    auto it = keywordMap.find(keyword);
    return (it != keywordMap.end()) ? it->second : TokenType::IDENTIFIER;
}

bool KeywordMap::isKeyword(const String& word) {
    return keywordMap.find(word) != keywordMap.end();
}

bool KeywordMap::isCHTLKeyword(const String& word) {
    return chtlKeywords.find(word) != chtlKeywords.end();
}

bool KeywordMap::isCHTLJSKeyword(const String& word) {
    return chtljsKeywords.find(word) != chtljsKeywords.end();
}

// CSS属性映射实现
const std::unordered_set<String> CSSPropertyMap::cssProperties = {
    "width", "height", "margin", "padding", "border", "color", "background",
    "font-size", "font-family", "font-weight", "text-align", "display", "position",
    "top", "right", "bottom", "left", "z-index", "opacity", "transform",
    "transition", "animation", "float", "clear", "overflow", "visibility",
    "line-height", "text-decoration", "text-transform", "letter-spacing",
    "word-spacing", "white-space", "vertical-align", "cursor", "pointer-events",
    "box-shadow", "border-radius", "outline", "resize", "box-sizing"
};

const StringUnorderedMap CSSPropertyMap::shorthandMap = {
    {"margin", {"margin-top", "margin-right", "margin-bottom", "margin-left"}},
    {"padding", {"padding-top", "padding-right", "padding-bottom", "padding-left"}},
    {"border", {"border-width", "border-style", "border-color"}},
    {"font", {"font-style", "font-variant", "font-weight", "font-size", "line-height", "font-family"}},
    {"background", {"background-color", "background-image", "background-repeat", "background-position"}}
};

bool CSSPropertyMap::isCSSProperty(const String& property) {
    return cssProperties.find(property) != cssProperties.end();
}

bool CSSPropertyMap::isShorthandProperty(const String& property) {
    return shorthandMap.find(property) != shorthandMap.end();
}

StringVector CSSPropertyMap::getShorthandProperties(const String& shorthand) {
    auto it = shorthandMap.find(shorthand);
    return (it != shorthandMap.end()) ? it->second : StringVector{};
}

// JavaScript事件映射实现
const std::unordered_set<String> JSEventMap::allEvents = {
    "click", "dblclick", "mousedown", "mouseup", "mousemove", "mouseover", "mouseout",
    "mouseenter", "mouseleave", "contextmenu", "wheel", "keydown", "keyup", "keypress",
    "focus", "blur", "change", "input", "submit", "reset", "select", "load", "unload",
    "resize", "scroll", "error", "abort", "canplay", "canplaythrough", "durationchange",
    "emptied", "ended", "loadeddata", "loadedmetadata", "loadstart", "pause", "play",
    "playing", "progress", "ratechange", "seeked", "seeking", "stalled", "suspend",
    "timeupdate", "volumechange", "waiting", "animationend", "animationiteration",
    "animationstart", "transitionend", "drag", "dragend", "dragenter", "dragleave",
    "dragover", "dragstart", "drop", "touchstart", "touchend", "touchmove", "touchcancel"
};

const std::unordered_set<String> JSEventMap::mouseEvents = {
    "click", "dblclick", "mousedown", "mouseup", "mousemove", "mouseover", "mouseout",
    "mouseenter", "mouseleave", "contextmenu", "wheel"
};

const std::unordered_set<String> JSEventMap::keyboardEvents = {
    "keydown", "keyup", "keypress"
};

const std::unordered_set<String> JSEventMap::formEvents = {
    "focus", "blur", "change", "input", "submit", "reset", "select"
};

bool JSEventMap::isJSEvent(const String& event) {
    return allEvents.find(event) != allEvents.end();
}

bool JSEventMap::isMouseEvent(const String& event) {
    return mouseEvents.find(event) != mouseEvents.end();
}

bool JSEventMap::isKeyboardEvent(const String& event) {
    return keyboardEvents.find(event) != keyboardEvents.end();
}

bool JSEventMap::isFormEvent(const String& event) {
    return formEvents.find(event) != formEvents.end();
}

StringVector JSEventMap::getAllEvents() {
    return StringVector(allEvents.begin(), allEvents.end());
}

// 符号映射实现
const std::unordered_map<char, TokenType> SymbolMap::singleCharMap = {
    {'{', TokenType::LBRACE},
    {'}', TokenType::RBRACE},
    {'[', TokenType::LBRACKET},
    {']', TokenType::RBRACKET},
    {'(', TokenType::LPAREN},
    {')', TokenType::RPAREN},
    {';', TokenType::SEMICOLON},
    {':', TokenType::COLON},
    {'=', TokenType::EQUALS},
    {',', TokenType::COMMA},
    {'.', TokenType::DOT},
    {'#', TokenType::HASH},
    {'&', TokenType::AMPERSAND},
    {'*', TokenType::ASTERISK},
    {'/', TokenType::SLASH},
    {'?', TokenType::QUESTION},
    {'!', TokenType::EXCLAMATION},
    {'_', TokenType::UNDERSCORE},
    {'$', TokenType::DOLLAR}
};

const StringUnorderedMap SymbolMap::twoCharMap = {
    {"->", TokenType::ARROW},
    {"::", TokenType::DOUBLE_COLON},
    {"&->", TokenType::EVENT_BIND},
    {"{{", TokenType::DOUBLE_LBRACE},
    {"}}", TokenType::DOUBLE_RBRACE},
    {"//", TokenType::COMMENT_LINE},
    {"/*", TokenType::COMMENT_BLOCK},
    {"--", TokenType::COMMENT_GENERATOR},
    {"...", TokenType::ELLIPSIS}
};

TokenType SymbolMap::getSymbolType(char c) {
    auto it = singleCharMap.find(c);
    return (it != singleCharMap.end()) ? it->second : TokenType::UNKNOWN;
}

TokenType SymbolMap::getTwoCharSymbolType(const String& symbol) {
    auto it = twoCharMap.find(symbol);
    return (it != twoCharMap.end()) ? it->second : TokenType::UNKNOWN;
}

bool SymbolMap::isOperatorChar(char c) {
    return c == ':' || c == '=' || c == '-' || c == '>' || c == '&';
}

bool SymbolMap::isSelectorChar(char c) {
    return c == '.' || c == '#' || c == '&';
}

// CJMOD语法映射实现
const std::unordered_set<String> CJMODSyntaxMap::cjmodFunctions = {
    "printMyLove", "iNeverAway", "listen", "animate", "delegate"
};

const StringUnorderedMap CJMODSyntaxMap::functionParameters = {
    {"printMyLove", {"url", "mode", "width", "height", "scale"}},
    {"animate", {"target", "duration", "easing", "begin", "when", "end", "loop", "direction", "delay", "callback"}},
    {"listen", {"click", "mouseenter", "mouseleave", "mousemove", "keydown", "keyup"}},
    {"delegate", {"target", "click", "mouseenter", "mouseleave"}}
};

const std::unordered_set<String> CJMODSyntaxMap::placeholders = {
    "$", "$?", "$!", "$_", "...", "$!_", "$?_"
};

bool CJMODSyntaxMap::isCJMODFunction(const String& name) {
    return cjmodFunctions.find(name) != cjmodFunctions.end();
}

StringVector CJMODSyntaxMap::getCJMODParameters(const String& functionName) {
    auto it = functionParameters.find(functionName);
    return (it != functionParameters.end()) ? it->second : StringVector{};
}

bool CJMODSyntaxMap::isPlaceholder(const String& token) {
    return placeholders.find(token) != placeholders.end();
}

String CJMODSyntaxMap::getPlaceholderType(const String& placeholder) {
    if (placeholder == "$") return "basic";
    if (placeholder == "$?") return "optional";
    if (placeholder == "$!") return "required";
    if (placeholder == "$_") return "unordered";
    if (placeholder == "...") return "variadic";
    if (placeholder == "$!_") return "required_unordered";
    if (placeholder == "$?_") return "optional_unordered";
    return "unknown";
}

} // namespace CHTL