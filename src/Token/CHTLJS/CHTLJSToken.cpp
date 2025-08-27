#include "CHTLJSToken.h"
#include <algorithm>
#include <cctype>
#include <regex>

namespace CHTL {
namespace Token {

// 静态成员初始化
std::vector<std::string> CHTLJSTokenUtils::eventTypes;
std::vector<std::string> CHTLJSTokenUtils::easingFunctions;
std::unordered_map<CHTLJSTokenType, std::string> CHTLJSTokenUtils::tokenTypeNames;

// CHTLJSToken方法实现
std::string CHTLJSToken::toString() const {
    return "CHTLJSToken[" + CHTLJSTokenUtils::getTokenTypeName(type) + ": \"" + value + "\" at " + 
           std::to_string(line) + ":" + std::to_string(column) + "]";
}

bool CHTLJSToken::isSelector() const {
    return type == CHTLJSTokenType::CSS_SELECTOR || 
           type == CHTLJSTokenType::CLASS_SELECTOR ||
           type == CHTLJSTokenType::ID_SELECTOR ||
           type == CHTLJSTokenType::TAG_SELECTOR ||
           type == CHTLJSTokenType::INDEXED_SELECTOR;
}

bool CHTLJSToken::isEventType() const {
    return type >= CHTLJSTokenType::CLICK_EVENT && type <= CHTLJSTokenType::SCROLL_EVENT;
}

bool CHTLJSToken::isCHTLJSSpecific() const {
    return type == CHTLJSTokenType::ARROW ||
           type == CHTLJSTokenType::DOUBLE_LEFT_BRACE ||
           type == CHTLJSTokenType::DOUBLE_RIGHT_BRACE ||
           type == CHTLJSTokenType::VIR ||
           type == CHTLJSTokenType::LISTEN ||
           type == CHTLJSTokenType::DELEGATE ||
           type == CHTLJSTokenType::ANIMATE ||
           isSelector();
}

// CHTLJSTokenUtils方法实现
const std::vector<std::string>& CHTLJSTokenUtils::getEventTypes() {
    if (eventTypes.empty()) {
        initializeEventTypes();
    }
    return eventTypes;
}

bool CHTLJSTokenUtils::isEventType(const std::string& event) {
    if (eventTypes.empty()) {
        initializeEventTypes();
    }
    
    return std::find(eventTypes.begin(), eventTypes.end(), event) != eventTypes.end();
}

const std::vector<std::string>& CHTLJSTokenUtils::getEasingFunctions() {
    if (easingFunctions.empty()) {
        initializeEasingFunctions();
    }
    return easingFunctions;
}

bool CHTLJSTokenUtils::isEasingFunction(const std::string& easing) {
    if (easingFunctions.empty()) {
        initializeEasingFunctions();
    }
    
    return std::find(easingFunctions.begin(), easingFunctions.end(), easing) != easingFunctions.end();
}

std::string CHTLJSTokenUtils::getTokenTypeName(CHTLJSTokenType type) {
    if (tokenTypeNames.empty()) {
        initializeTokenTypeNames();
    }
    
    auto it = tokenTypeNames.find(type);
    return it != tokenTypeNames.end() ? it->second : "UNKNOWN";
}

bool CHTLJSTokenUtils::isSelectorChar(char c) {
    return std::isalnum(c) || c == '_' || c == '-' || c == '.' || c == '#' || 
           c == '[' || c == ']' || c == ':' || c == ' ';
}

bool CHTLJSTokenUtils::isValidCSSSelector(const std::string& selector) {
    if (selector.empty()) {
        return false;
    }
    
    // 基本的CSS选择器验证
    // 类选择器
    if (selector[0] == '.') {
        return selector.length() > 1 && std::isalpha(selector[1]);
    }
    
    // ID选择器
    if (selector[0] == '#') {
        return selector.length() > 1 && std::isalpha(selector[1]);
    }
    
    // 标签选择器
    if (std::isalpha(selector[0])) {
        return true;
    }
    
    return false;
}

CHTLJSTokenType CHTLJSTokenUtils::parseSelectorType(const std::string& selector) {
    if (selector.empty()) {
        return CHTLJSTokenType::UNKNOWN;
    }
    
    if (selector[0] == '.') {
        return CHTLJSTokenType::CLASS_SELECTOR;
    }
    
    if (selector[0] == '#') {
        return CHTLJSTokenType::ID_SELECTOR;
    }
    
    // 检查是否包含索引
    if (selector.find('[') != std::string::npos && selector.find(']') != std::string::npos) {
        return CHTLJSTokenType::INDEXED_SELECTOR;
    }
    
    if (std::isalpha(selector[0])) {
        return CHTLJSTokenType::TAG_SELECTOR;
    }
    
    return CHTLJSTokenType::CSS_SELECTOR;
}

void CHTLJSTokenUtils::initializeEventTypes() {
    // 根据CHTL语法文档，CHTL JS支持所有JS事件
    eventTypes = {
        // 鼠标事件
        "click", "dblclick", "mousedown", "mouseup", "mousemove",
        "mouseenter", "mouseleave", "mouseover", "mouseout",
        "contextmenu", "wheel",
        
        // 键盘事件
        "keydown", "keyup", "keypress",
        
        // 表单事件
        "focus", "blur", "change", "input", "submit", "reset",
        "select", "invalid",
        
        // 窗口事件
        "load", "unload", "beforeunload", "resize", "scroll",
        "hashchange", "popstate",
        
        // 触摸事件
        "touchstart", "touchend", "touchmove", "touchcancel",
        
        // 拖放事件
        "drag", "dragstart", "dragend", "dragenter", "dragover",
        "dragleave", "drop",
        
        // 媒体事件
        "play", "pause", "ended", "volumechange", "timeupdate",
        "loadstart", "loadeddata", "loadedmetadata", "canplay",
        "canplaythrough",
        
        // 其他事件
        "error", "abort", "animationstart", "animationend",
        "animationiteration", "transitionstart", "transitionend",
        "orientationchange", "devicemotion", "deviceorientation"
    };
    
    std::sort(eventTypes.begin(), eventTypes.end());
}

void CHTLJSTokenUtils::initializeEasingFunctions() {
    // 根据CHTL语法文档，支持标准的缓动函数
    easingFunctions = {
        "linear",
        "ease", "ease-in", "ease-out", "ease-in-out",
        "ease-in-sine", "ease-out-sine", "ease-in-out-sine",
        "ease-in-quad", "ease-out-quad", "ease-in-out-quad",
        "ease-in-cubic", "ease-out-cubic", "ease-in-out-cubic",
        "ease-in-quart", "ease-out-quart", "ease-in-out-quart",
        "ease-in-quint", "ease-out-quint", "ease-in-out-quint",
        "ease-in-expo", "ease-out-expo", "ease-in-out-expo",
        "ease-in-circ", "ease-out-circ", "ease-in-out-circ",
        "ease-in-back", "ease-out-back", "ease-in-out-back",
        "ease-in-elastic", "ease-out-elastic", "ease-in-out-elastic",
        "ease-in-bounce", "ease-out-bounce", "ease-in-out-bounce"
    };
    
    std::sort(easingFunctions.begin(), easingFunctions.end());
}

void CHTLJSTokenUtils::initializeTokenTypeNames() {
    tokenTypeNames = {
        {CHTLJSTokenType::IDENTIFIER, "IDENTIFIER"},
        {CHTLJSTokenType::STRING_LITERAL, "STRING_LITERAL"},
        {CHTLJSTokenType::NUMBER_LITERAL, "NUMBER_LITERAL"},
        {CHTLJSTokenType::UNQUOTED_LITERAL, "UNQUOTED_LITERAL"},
        
        {CHTLJSTokenType::LEFT_BRACE, "LEFT_BRACE"},
        {CHTLJSTokenType::RIGHT_BRACE, "RIGHT_BRACE"},
        {CHTLJSTokenType::LEFT_BRACKET, "LEFT_BRACKET"},
        {CHTLJSTokenType::RIGHT_BRACKET, "RIGHT_BRACKET"},
        {CHTLJSTokenType::LEFT_PAREN, "LEFT_PAREN"},
        {CHTLJSTokenType::RIGHT_PAREN, "RIGHT_PAREN"},
        {CHTLJSTokenType::SEMICOLON, "SEMICOLON"},
        {CHTLJSTokenType::COLON, "COLON"},
        {CHTLJSTokenType::EQUALS, "EQUALS"},
        {CHTLJSTokenType::COMMA, "COMMA"},
        {CHTLJSTokenType::DOT, "DOT"},
        
        {CHTLJSTokenType::ARROW, "ARROW"},
        {CHTLJSTokenType::AMPERSAND, "AMPERSAND"},
        {CHTLJSTokenType::DOUBLE_LEFT_BRACE, "DOUBLE_LEFT_BRACE"},
        {CHTLJSTokenType::DOUBLE_RIGHT_BRACE, "DOUBLE_RIGHT_BRACE"},
        
        {CHTLJSTokenType::VIR, "VIR"},
        {CHTLJSTokenType::LISTEN, "LISTEN"},
        {CHTLJSTokenType::DELEGATE, "DELEGATE"},
        {CHTLJSTokenType::ANIMATE, "ANIMATE"},
        
        {CHTLJSTokenType::I_NEVER_AWAY, "I_NEVER_AWAY"},
        {CHTLJSTokenType::PRINT_MY_LOVE, "PRINT_MY_LOVE"},
        
        {CHTLJSTokenType::CSS_SELECTOR, "CSS_SELECTOR"},
        {CHTLJSTokenType::CLASS_SELECTOR, "CLASS_SELECTOR"},
        {CHTLJSTokenType::ID_SELECTOR, "ID_SELECTOR"},
        {CHTLJSTokenType::TAG_SELECTOR, "TAG_SELECTOR"},
        {CHTLJSTokenType::INDEXED_SELECTOR, "INDEXED_SELECTOR"},
        
        {CHTLJSTokenType::CLICK_EVENT, "CLICK_EVENT"},
        {CHTLJSTokenType::MOUSEENTER_EVENT, "MOUSEENTER_EVENT"},
        {CHTLJSTokenType::MOUSELEAVE_EVENT, "MOUSELEAVE_EVENT"},
        {CHTLJSTokenType::MOUSEMOVE_EVENT, "MOUSEMOVE_EVENT"},
        {CHTLJSTokenType::KEYDOWN_EVENT, "KEYDOWN_EVENT"},
        {CHTLJSTokenType::KEYUP_EVENT, "KEYUP_EVENT"},
        {CHTLJSTokenType::FOCUS_EVENT, "FOCUS_EVENT"},
        {CHTLJSTokenType::BLUR_EVENT, "BLUR_EVENT"},
        {CHTLJSTokenType::CHANGE_EVENT, "CHANGE_EVENT"},
        {CHTLJSTokenType::SUBMIT_EVENT, "SUBMIT_EVENT"},
        {CHTLJSTokenType::LOAD_EVENT, "LOAD_EVENT"},
        {CHTLJSTokenType::RESIZE_EVENT, "RESIZE_EVENT"},
        {CHTLJSTokenType::SCROLL_EVENT, "SCROLL_EVENT"},
        
        {CHTLJSTokenType::EASING_FUNCTION, "EASING_FUNCTION"},
        {CHTLJSTokenType::ANIMATION_PROPERTY, "ANIMATION_PROPERTY"},
        
        {CHTLJSTokenType::SINGLE_COMMENT, "SINGLE_COMMENT"},
        {CHTLJSTokenType::MULTI_COMMENT, "MULTI_COMMENT"},
        {CHTLJSTokenType::GENERATOR_COMMENT, "GENERATOR_COMMENT"},
        
        {CHTLJSTokenType::EOF_TOKEN, "EOF_TOKEN"},
        {CHTLJSTokenType::NEWLINE, "NEWLINE"},
        {CHTLJSTokenType::WHITESPACE, "WHITESPACE"},
        
        {CHTLJSTokenType::UNKNOWN, "UNKNOWN"},
        {CHTLJSTokenType::ERROR, "ERROR"}
    };
}

} // namespace Token
} // namespace CHTL