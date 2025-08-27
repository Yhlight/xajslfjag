#include "GlobalMap.h"
#include <algorithm>

namespace CHTLJS {

GlobalMap::GlobalMap() {
    initKeywords();
    initOperators();
    initEnhancedKeywords();
    initDomFunctions();
    initCssProperties();
}

void GlobalMap::initKeywords() {
    // 基础JavaScript关键字
    keywords_["script"] = TokenType::SCRIPT;
    keywords_["function"] = TokenType::FUNCTION;
    keywords_["const"] = TokenType::CONST;
    keywords_["let"] = TokenType::LET;
    keywords_["var"] = TokenType::VAR;
    keywords_["if"] = TokenType::IF;
    keywords_["else"] = TokenType::ELSE;
    keywords_["for"] = TokenType::FOR;
    keywords_["while"] = TokenType::WHILE;
    keywords_["return"] = TokenType::RETURN;
    keywords_["true"] = TokenType::TRUE;
    keywords_["false"] = TokenType::FALSE;
    keywords_["null"] = TokenType::NULL_TOKEN;
    keywords_["undefined"] = TokenType::UNDEFINED;
}

void GlobalMap::initOperators() {
    operators_["->"] = TokenType::ARROW;
    operators_["&->"] = TokenType::BIND_OPERATOR;
    operators_["{{"] = TokenType::SELECTOR_START;
    operators_["}}"] = TokenType::SELECTOR_END;
}

void GlobalMap::initEnhancedKeywords() {
    // CHTL JS增强语法关键字
    enhanced_keywords_["listen"] = TokenType::LISTEN;
    enhanced_keywords_["delegate"] = TokenType::DELEGATE;
    enhanced_keywords_["animate"] = TokenType::ANIMATE;
    enhanced_keywords_["vir"] = TokenType::VIR;
    enhanced_keywords_["when"] = TokenType::WHEN;
    enhanced_keywords_["begin"] = TokenType::BEGIN;
    enhanced_keywords_["end"] = TokenType::END;
    enhanced_keywords_["at"] = TokenType::AT;
    enhanced_keywords_["target"] = TokenType::TARGET;
    enhanced_keywords_["duration"] = TokenType::DURATION;
    enhanced_keywords_["easing"] = TokenType::EASING;
    enhanced_keywords_["loop"] = TokenType::LOOP;
    enhanced_keywords_["direction"] = TokenType::DIRECTION;
    enhanced_keywords_["delay"] = TokenType::DELAY;
    enhanced_keywords_["callback"] = TokenType::CALLBACK;
}

void GlobalMap::initDomFunctions() {
    // 常用DOM方法
    std::vector<std::string> functions = {
        // 元素操作
        "textContent", "innerHTML", "innerText", "outerHTML",
        "getAttribute", "setAttribute", "removeAttribute", "hasAttribute",
        "classList", "className", "id", "tagName",
        
        // 事件处理
        "addEventListener", "removeEventListener", "dispatchEvent",
        
        // 样式操作
        "getComputedStyle", "style",
        
        // 查询方法
        "querySelector", "querySelectorAll", "getElementById", 
        "getElementsByClassName", "getElementsByTagName",
        
        // 节点操作
        "appendChild", "insertBefore", "removeChild", "replaceChild",
        "cloneNode", "parentNode", "childNodes", "firstChild", "lastChild",
        "nextSibling", "previousSibling",
        
        // 表单操作
        "value", "checked", "disabled", "focus", "blur",
        
        // 动画相关
        "animate", "getAnimations"
    };
    
    for (const auto& func : functions) {
        dom_functions_.insert(func);
    }
}

void GlobalMap::initCssProperties() {
    // 常用CSS属性
    std::vector<std::string> properties = {
        // 布局相关
        "width", "height", "margin", "padding", "border",
        "display", "position", "top", "right", "bottom", "left",
        "float", "clear", "overflow", "visibility",
        
        // 文本相关
        "color", "font-size", "font-family", "font-weight", "font-style",
        "text-align", "text-decoration", "line-height", "letter-spacing",
        
        // 背景相关
        "background", "background-color", "background-image", "background-repeat",
        "background-position", "background-size",
        
        // 变换和动画
        "transform", "transition", "animation", "opacity",
        
        // Flexbox
        "flex", "flex-direction", "justify-content", "align-items", "align-content",
        
        // Grid
        "grid", "grid-template-columns", "grid-template-rows", "grid-gap",
        
        // 其他
        "z-index", "cursor", "box-shadow", "border-radius"
    };
    
    for (const auto& prop : properties) {
        css_properties_.insert(prop);
    }
}

TokenType GlobalMap::getKeywordType(const std::string& keyword) const {
    auto it = keywords_.find(keyword);
    return (it != keywords_.end()) ? it->second : TokenType::IDENTIFIER;
}

TokenType GlobalMap::getOperatorType(const std::string& op) const {
    auto it = operators_.find(op);
    return (it != operators_.end()) ? it->second : TokenType::UNKNOWN;
}

TokenType GlobalMap::getEnhancedKeywordType(const std::string& keyword) const {
    auto it = enhanced_keywords_.find(keyword);
    return (it != enhanced_keywords_.end()) ? it->second : TokenType::IDENTIFIER;
}

bool GlobalMap::isDomFunction(const std::string& function) const {
    return dom_functions_.find(function) != dom_functions_.end();
}

bool GlobalMap::isCssProperty(const std::string& property) const {
    return css_properties_.find(property) != css_properties_.end();
}

void GlobalMap::addDomFunction(const std::string& function) {
    dom_functions_.insert(function);
}

void GlobalMap::addCssProperty(const std::string& property) {
    css_properties_.insert(property);
}

std::vector<std::string> GlobalMap::getAllKeywords() const {
    std::vector<std::string> keywords;
    for (const auto& pair : keywords_) {
        keywords.push_back(pair.first);
    }
    std::sort(keywords.begin(), keywords.end());
    return keywords;
}

std::vector<std::string> GlobalMap::getAllEnhancedKeywords() const {
    std::vector<std::string> keywords;
    for (const auto& pair : enhanced_keywords_) {
        keywords.push_back(pair.first);
    }
    std::sort(keywords.begin(), keywords.end());
    return keywords;
}

bool GlobalMap::isReservedWord(const std::string& str) const {
    return keywords_.find(str) != keywords_.end() ||
           enhanced_keywords_.find(str) != enhanced_keywords_.end() ||
           operators_.find(str) != operators_.end();
}

void GlobalMap::reset() {
    keywords_.clear();
    operators_.clear();
    enhanced_keywords_.clear();
    dom_functions_.clear();
    css_properties_.clear();
    
    initKeywords();
    initOperators();
    initEnhancedKeywords();
    initDomFunctions();
    initCssProperties();
}

} // namespace CHTLJS