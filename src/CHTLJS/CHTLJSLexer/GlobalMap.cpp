#include "GlobalMap.h"

namespace CHTLJS {

GlobalMap::GlobalMap() {
    InitializeDefaults();
}

void GlobalMap::InitializeDefaults() {
    InitializeKeywords();
    InitializeSymbols();
    InitializeOperators();
    InitializeJSEvents();
    InitializeEasingFunctions();
    InitializeDOMProperties();
    InitializeDOMMethods();
}

void GlobalMap::InitializeKeywords() {
    keywords = {
        {"vir", TokenType::VIR},
        {"listen", TokenType::LISTEN},
        {"delegate", TokenType::DELEGATE},
        {"animate", TokenType::ANIMATE},
        {"target", TokenType::TARGET},
        {"duration", TokenType::DURATION},
        {"easing", TokenType::EASING},
        {"begin", TokenType::BEGIN},
        {"when", TokenType::WHEN},
        {"end", TokenType::END},
        {"loop", TokenType::LOOP},
        {"direction", TokenType::DIRECTION},
        {"delay", TokenType::DELAY},
        {"callback", TokenType::CALLBACK},
        {"at", TokenType::AT}
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
        {"?", TokenType::QUESTION},
        {"!", TokenType::EXCLAMATION}
    };
}

void GlobalMap::InitializeOperators() {
    operators = {
        {"->", TokenType::ARROW},
        {"&->", TokenType::EVENT_BIND},
        {"{{", TokenType::SELECTOR_START},
        {"}}", TokenType::SELECTOR_END}
    };
}

void GlobalMap::InitializeJSEvents() {
    jsEvents = {
        // 鼠标事件
        "click", "dblclick", "mousedown", "mouseup", "mousemove", 
        "mouseenter", "mouseleave", "mouseover", "mouseout", "contextmenu",
        
        // 键盘事件
        "keydown", "keyup", "keypress",
        
        // 表单事件
        "submit", "change", "input", "focus", "blur", "select", "reset",
        
        // 窗口事件
        "load", "unload", "resize", "scroll", "beforeunload",
        
        // 触摸事件
        "touchstart", "touchend", "touchmove", "touchcancel",
        
        // 拖拽事件
        "dragstart", "drag", "dragend", "dragenter", "dragover", "dragleave", "drop",
        
        // 媒体事件
        "play", "pause", "ended", "volumechange", "timeupdate",
        
        // 其他事件
        "animationstart", "animationend", "animationiteration",
        "transitionstart", "transitionend", "transitionrun", "transitioncancel"
    };
}

void GlobalMap::InitializeEasingFunctions() {
    easingFunctions = {
        // CSS标准缓动函数
        "linear", "ease", "ease-in", "ease-out", "ease-in-out",
        
        // 贝塞尔曲线函数
        "cubic-bezier",
        
        // 步进函数
        "steps", "step-start", "step-end",
        
        // 弹性函数
        "bounce", "elastic", "back",
        
        // 指数函数
        "expo", "circ", "sine", "quad", "cubic", "quart", "quint"
    };
}

void GlobalMap::InitializeDOMProperties() {
    domProperties = {
        // 元素属性
        "innerHTML", "textContent", "value", "checked", "selected", "disabled",
        "hidden", "readOnly", "required", "multiple",
        
        // 样式属性
        "style", "className", "classList", "id",
        
        // 尺寸属性
        "offsetWidth", "offsetHeight", "clientWidth", "clientHeight",
        "scrollWidth", "scrollHeight", "scrollTop", "scrollLeft",
        
        // 位置属性
        "offsetTop", "offsetLeft", "offsetParent",
        
        // 节点属性
        "nodeType", "nodeName", "nodeValue", "parentNode", "childNodes",
        "firstChild", "lastChild", "nextSibling", "previousSibling",
        
        // 表单属性
        "action", "method", "enctype", "target", "elements", "length"
    };
}

void GlobalMap::InitializeDOMMethods() {
    domMethods = {
        // 查询方法
        "querySelector", "querySelectorAll", "getElementById", "getElementsByClassName",
        "getElementsByTagName", "getElementsByName",
        
        // 创建和插入方法
        "createElement", "createTextNode", "appendChild", "insertBefore",
        "insertAfter", "removeChild", "replaceChild", "cloneNode",
        
        // 属性操作方法
        "getAttribute", "setAttribute", "removeAttribute", "hasAttribute",
        "getAttributeNames",
        
        // 类操作方法
        "add", "remove", "toggle", "contains", // classList方法
        
        // 事件方法
        "addEventListener", "removeEventListener", "dispatchEvent",
        
        // 样式方法
        "getComputedStyle", "setProperty", "getPropertyValue", "removeProperty",
        
        // 表单方法
        "submit", "reset", "checkValidity", "reportValidity",
        
        // 焦点方法
        "focus", "blur", "click", "select",
        
        // 滚动方法
        "scrollIntoView", "scroll", "scrollTo", "scrollBy"
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

TokenType GlobalMap::LookupOperator(const std::string& op) const {
    auto it = operators.find(op);
    return it != operators.end() ? it->second : TokenType::UNKNOWN;
}

bool GlobalMap::IsValidJSEvent(const std::string& event) const {
    return jsEvents.find(event) != jsEvents.end();
}

bool GlobalMap::IsValidEasingFunction(const std::string& easing) const {
    return easingFunctions.find(easing) != easingFunctions.end();
}

bool GlobalMap::IsValidDOMProperty(const std::string& property) const {
    return domProperties.find(property) != domProperties.end();
}

bool GlobalMap::IsValidDOMMethod(const std::string& method) const {
    return domMethods.find(method) != domMethods.end();
}

void GlobalMap::RegisterVirtualObjectFunction(const std::string& funcName, const std::vector<std::string>& params) {
    virtualObjectFunctions[funcName] = params;
}

std::vector<std::string> GlobalMap::GetVirtualObjectFunctionParams(const std::string& funcName) const {
    auto it = virtualObjectFunctions.find(funcName);
    return it != virtualObjectFunctions.end() ? it->second : std::vector<std::string>();
}

bool GlobalMap::HasVirtualObjectFunction(const std::string& funcName) const {
    return virtualObjectFunctions.find(funcName) != virtualObjectFunctions.end();
}

void GlobalMap::AddUserEvent(const std::string& event) {
    jsEvents.insert(event);
}

void GlobalMap::Clear() {
    keywords.clear();
    symbols.clear();
    operators.clear();
    jsEvents.clear();
    easingFunctions.clear();
    domProperties.clear();
    domMethods.clear();
    virtualObjectFunctions.clear();
}

void GlobalMap::Reset() {
    Clear();
    InitializeDefaults();
}

} // namespace CHTLJS