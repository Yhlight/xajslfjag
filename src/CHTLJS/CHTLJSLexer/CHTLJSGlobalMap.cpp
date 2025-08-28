#include "CHTLJSGlobalMap.h"
#include <regex>

namespace CHTLJS {

CHTLJSGlobalMap& CHTLJSGlobalMap::Instance() {
    static CHTLJSGlobalMap instance;
    return instance;
}

void CHTLJSGlobalMap::Initialize() {
    InitializeKeywords();
    InitializeCHTLJSFunctions();
    InitializeJSEvents();
    InitializeEasingFunctions();
}

void CHTLJSGlobalMap::InitializeKeywords() {
    keywords_ = {
        // CHTL JS特有关键字
        {"module", CHTLJSTokenType::MODULE},
        {"load", CHTLJSTokenType::LOAD},
        {"listen", CHTLJSTokenType::LISTEN},
        {"delegate", CHTLJSTokenType::DELEGATE},
        {"animate", CHTLJSTokenType::ANIMATE},
        {"vir", CHTLJSTokenType::VIR},
        {"target", CHTLJSTokenType::TARGET},
        {"duration", CHTLJSTokenType::DURATION},
        {"easing", CHTLJSTokenType::EASING},
        {"begin", CHTLJSTokenType::BEGIN},
        {"when", CHTLJSTokenType::WHEN},
        {"end", CHTLJSTokenType::END},
        {"loop", CHTLJSTokenType::LOOP},
        {"direction", CHTLJSTokenType::DIRECTION},
        {"delay", CHTLJSTokenType::DELAY},
        {"callback", CHTLJSTokenType::CALLBACK},
        {"at", CHTLJSTokenType::AT},
        
        // JavaScript基础关键字
        {"function", CHTLJSTokenType::FUNCTION},
        {"const", CHTLJSTokenType::CONST},
        {"let", CHTLJSTokenType::LET},
        {"var", CHTLJSTokenType::VAR},
        {"if", CHTLJSTokenType::IF},
        {"else", CHTLJSTokenType::ELSE},
        {"for", CHTLJSTokenType::FOR},
        {"while", CHTLJSTokenType::WHILE},
        {"return", CHTLJSTokenType::RETURN}
    };
}

void CHTLJSGlobalMap::InitializeCHTLJSFunctions() {
    chtljsFunctions_ = {
        "module", "listen", "delegate", "animate"
    };
}

void CHTLJSGlobalMap::InitializeJSEvents() {
    // 所有标准DOM事件
    validEvents_ = {
        // 鼠标事件
        "click", "dblclick", "mousedown", "mouseup", "mousemove", "mouseover", 
        "mouseout", "mouseenter", "mouseleave", "contextmenu", "wheel",
        
        // 键盘事件
        "keydown", "keyup", "keypress",
        
        // 表单事件
        "submit", "reset", "focus", "blur", "change", "input", "select",
        
        // 窗口事件
        "load", "unload", "beforeunload", "resize", "scroll", "error",
        
        // 触摸事件
        "touchstart", "touchend", "touchmove", "touchcancel",
        
        // 拖拽事件
        "drag", "dragstart", "dragend", "dragover", "dragenter", "dragleave", "drop",
        
        // 剪贴板事件
        "copy", "cut", "paste",
        
        // 媒体事件
        "play", "pause", "ended", "volumechange", "timeupdate", "loadstart",
        "loadeddata", "loadedmetadata", "canplay", "canplaythrough",
        
        // 其他常用事件
        "abort", "animationend", "animationiteration", "animationstart",
        "transitionend", "online", "offline", "popstate", "storage",
        "hashchange", "pageshow", "pagehide", "beforeprint", "afterprint"
    };
}

void CHTLJSGlobalMap::InitializeEasingFunctions() {
    validEasingFunctions_ = {
        // CSS缓动函数
        "linear", "ease", "ease-in", "ease-out", "ease-in-out",
        
        // 自定义缓动
        "easeInSine", "easeOutSine", "easeInOutSine",
        "easeInQuad", "easeOutQuad", "easeInOutQuad",
        "easeInCubic", "easeOutCubic", "easeInOutCubic",
        "easeInQuart", "easeOutQuart", "easeInOutQuart",
        "easeInQuint", "easeOutQuint", "easeInOutQuint",
        "easeInExpo", "easeOutExpo", "easeInOutExpo",
        "easeInCirc", "easeOutCirc", "easeInOutCirc",
        "easeInBack", "easeOutBack", "easeInOutBack",
        "easeInElastic", "easeOutElastic", "easeInOutElastic",
        "easeInBounce", "easeOutBounce", "easeInOutBounce"
    };
}

bool CHTLJSGlobalMap::IsKeyword(const std::string& keyword) const {
    return keywords_.find(keyword) != keywords_.end();
}

CHTLJSTokenType CHTLJSGlobalMap::GetKeywordType(const std::string& keyword) const {
    auto it = keywords_.find(keyword);
    return it != keywords_.end() ? it->second : CHTLJSTokenType::IDENTIFIER;
}

bool CHTLJSGlobalMap::IsCHTLJSFunction(const std::string& name) const {
    return chtljsFunctions_.find(name) != chtljsFunctions_.end();
}

void CHTLJSGlobalMap::RegisterCHTLJSFunction(const std::string& name) {
    chtljsFunctions_.insert(name);
}

void CHTLJSGlobalMap::RegisterVirtualObject(const std::string& name, 
                                           const std::unordered_map<std::string, std::string>& properties) {
    virtualObjects_.insert(name);
    virtualProperties_[name] = properties;
}

bool CHTLJSGlobalMap::IsVirtualObject(const std::string& name) const {
    return virtualObjects_.find(name) != virtualObjects_.end();
}

bool CHTLJSGlobalMap::HasVirtualProperty(const std::string& objectName, 
                                        const std::string& propertyName) const {
    auto objIt = virtualProperties_.find(objectName);
    if (objIt != virtualProperties_.end()) {
        return objIt->second.find(propertyName) != objIt->second.end();
    }
    return false;
}

std::string CHTLJSGlobalMap::GetVirtualPropertyType(const std::string& objectName, 
                                                    const std::string& propertyName) const {
    auto objIt = virtualProperties_.find(objectName);
    if (objIt != virtualProperties_.end()) {
        auto propIt = objIt->second.find(propertyName);
        if (propIt != objIt->second.end()) {
            return propIt->second;
        }
    }
    return "";
}

bool CHTLJSGlobalMap::IsValidEvent(const std::string& eventName) const {
    return validEvents_.find(eventName) != validEvents_.end();
}

const std::unordered_set<std::string>& CHTLJSGlobalMap::GetAllEvents() const {
    return validEvents_;
}

bool CHTLJSGlobalMap::IsValidEasingFunction(const std::string& easingName) const {
    return validEasingFunctions_.find(easingName) != validEasingFunctions_.end();
}

bool CHTLJSGlobalMap::IsValidSelector(const std::string& selector) const {
    // 基础选择器验证
    if (selector.empty()) return false;
    
    // 支持的选择器格式：
    // - .class
    // - #id  
    // - element
    // - .class element
    // - #id element
    // - element[n] (索引访问)
    
    std::regex selectorPattern(R"(^([.#]?[\w-]+(\s+[\w-]+)*(\[\d+\])?)$)");
    return std::regex_match(selector, selectorPattern);
}

void CHTLJSGlobalMap::RegisterLoadedModule(const std::string& modulePath) {
    loadedModules_.insert(modulePath);
}

bool CHTLJSGlobalMap::IsModuleLoaded(const std::string& modulePath) const {
    return loadedModules_.find(modulePath) != loadedModules_.end();
}

void CHTLJSGlobalMap::RegisterCJMODFunction(const std::string& signature, 
                                           const std::string& implementation) {
    cjmodFunctions_[signature] = implementation;
}

bool CHTLJSGlobalMap::IsCJMODFunction(const std::string& name) const {
    for (const auto& pair : cjmodFunctions_) {
        if (pair.first.find(name) == 0) {
            return true;
        }
    }
    return false;
}

std::string CHTLJSGlobalMap::GetCJMODImplementation(const std::string& signature) const {
    auto it = cjmodFunctions_.find(signature);
    return it != cjmodFunctions_.end() ? it->second : "";
}

void CHTLJSGlobalMap::Clear() {
    virtualObjects_.clear();
    virtualProperties_.clear();
    loadedModules_.clear();
    cjmodFunctions_.clear();
}

} // namespace CHTLJS