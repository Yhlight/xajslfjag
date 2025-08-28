#include "CHTLJSGlobalMap.h"
#include <algorithm>

namespace CHTLJS {

// CHTLJSKeywordMap 实现
CHTLJSKeywordMap::CHTLJSKeywordMap() {
    initializeDefaultKeywords();
    initializeCHTLJSSpecificKeywords();
}

void CHTLJSKeywordMap::initializeDefaultKeywords() {
    // CHTL JS特有关键字
    keywordMap["module"] = CHTLJSTokenType::MODULE;
    keywordMap["load"] = CHTLJSTokenType::LOAD;
    keywordMap["listen"] = CHTLJSTokenType::LISTEN;
    keywordMap["animate"] = CHTLJSTokenType::ANIMATE;
    keywordMap["delegate"] = CHTLJSTokenType::DELEGATE;
    keywordMap["vir"] = CHTLJSTokenType::VIR;
    keywordMap["target"] = CHTLJSTokenType::TARGET;
    keywordMap["duration"] = CHTLJSTokenType::DURATION;
    keywordMap["easing"] = CHTLJSTokenType::EASING;
    keywordMap["begin"] = CHTLJSTokenType::BEGIN;
    keywordMap["when"] = CHTLJSTokenType::WHEN;
    keywordMap["end"] = CHTLJSTokenType::END;
    keywordMap["loop"] = CHTLJSTokenType::LOOP;
    keywordMap["direction"] = CHTLJSTokenType::DIRECTION;
    keywordMap["delay"] = CHTLJSTokenType::DELAY;
    keywordMap["callback"] = CHTLJSTokenType::CALLBACK;
    keywordMap["at"] = CHTLJSTokenType::AT;
    
    // JavaScript关键字（CHTL JS支持的子集）
    keywordMap["function"] = CHTLJSTokenType::FUNCTION;
    keywordMap["const"] = CHTLJSTokenType::CONST;
    keywordMap["let"] = CHTLJSTokenType::LET;
    keywordMap["var"] = CHTLJSTokenType::VAR;
    keywordMap["if"] = CHTLJSTokenType::IF;
    keywordMap["else"] = CHTLJSTokenType::ELSE;
    keywordMap["for"] = CHTLJSTokenType::FOR;
    keywordMap["while"] = CHTLJSTokenType::WHILE;
    keywordMap["do"] = CHTLJSTokenType::DO;
    keywordMap["break"] = CHTLJSTokenType::BREAK;
    keywordMap["continue"] = CHTLJSTokenType::CONTINUE;
    keywordMap["return"] = CHTLJSTokenType::RETURN;
}

void CHTLJSKeywordMap::initializeCHTLJSSpecificKeywords() {
    chtljsSpecificKeywords = {
        "module", "load", "listen", "animate", "delegate", "vir", 
        "target", "duration", "easing", "begin", "when", "end",
        "loop", "direction", "delay", "callback", "at"
    };
}

bool CHTLJSKeywordMap::isKeyword(const std::string& text) const {
    return keywordMap.find(text) != keywordMap.end();
}

CHTLJSTokenType CHTLJSKeywordMap::getKeywordType(const std::string& text) const {
    auto it = keywordMap.find(text);
    return it != keywordMap.end() ? it->second : CHTLJSTokenType::IDENTIFIER;
}

void CHTLJSKeywordMap::addKeyword(const std::string& keyword, CHTLJSTokenType type) {
    keywordMap[keyword] = type;
}

void CHTLJSKeywordMap::removeKeyword(const std::string& keyword) {
    keywordMap.erase(keyword);
    chtljsSpecificKeywords.erase(keyword);
}

std::vector<std::string> CHTLJSKeywordMap::getAllKeywords() const {
    std::vector<std::string> keywords;
    keywords.reserve(keywordMap.size());
    for (const auto& pair : keywordMap) {
        keywords.push_back(pair.first);
    }
    return keywords;
}

bool CHTLJSKeywordMap::isCHTLJSSpecificKeyword(const std::string& text) const {
    return chtljsSpecificKeywords.find(text) != chtljsSpecificKeywords.end();
}

// CHTLJSEventMap 实现
CHTLJSEventMap::CHTLJSEventMap() {
    initializeMouseEvents();
    initializeKeyboardEvents();
    initializeFormEvents();
    initializeTouchEvents();
}

void CHTLJSEventMap::initializeMouseEvents() {
    mouseEvents = {
        "click", "dblclick", "mousedown", "mouseup", "mousemove",
        "mouseover", "mouseout", "mouseenter", "mouseleave",
        "contextmenu", "wheel", "drag", "dragstart", "dragend",
        "dragover", "dragenter", "dragleave", "drop"
    };
}

void CHTLJSEventMap::initializeKeyboardEvents() {
    keyboardEvents = {
        "keydown", "keyup", "keypress"
    };
}

void CHTLJSEventMap::initializeFormEvents() {
    formEvents = {
        "submit", "reset", "focus", "blur", "change", "input",
        "select", "invalid", "search"
    };
}

void CHTLJSEventMap::initializeTouchEvents() {
    touchEvents = {
        "touchstart", "touchend", "touchmove", "touchcancel"
    };
}

bool CHTLJSEventMap::isValidDOMEvent(const std::string& eventName) const {
    return isMouseEvent(eventName) || isKeyboardEvent(eventName) || 
           isFormEvent(eventName) || isTouchEvent(eventName) ||
           customEvents.find(eventName) != customEvents.end();
}

bool CHTLJSEventMap::isMouseEvent(const std::string& eventName) const {
    return mouseEvents.find(eventName) != mouseEvents.end();
}

bool CHTLJSEventMap::isKeyboardEvent(const std::string& eventName) const {
    return keyboardEvents.find(eventName) != keyboardEvents.end();
}

bool CHTLJSEventMap::isFormEvent(const std::string& eventName) const {
    return formEvents.find(eventName) != formEvents.end();
}

bool CHTLJSEventMap::isTouchEvent(const std::string& eventName) const {
    return touchEvents.find(eventName) != touchEvents.end();
}

void CHTLJSEventMap::addCustomEvent(const std::string& eventName) {
    customEvents.insert(eventName);
}

std::string CHTLJSEventMap::getEventCategory(const std::string& eventName) const {
    if (isMouseEvent(eventName)) return "mouse";
    if (isKeyboardEvent(eventName)) return "keyboard";
    if (isFormEvent(eventName)) return "form";
    if (isTouchEvent(eventName)) return "touch";
    if (customEvents.find(eventName) != customEvents.end()) return "custom";
    return "unknown";
}

// CHTLJSModuleMap 实现
CHTLJSModuleMap::CHTLJSModuleMap() {
}

void CHTLJSModuleMap::registerModule(const std::string& name, const std::string& path) {
    ModuleInfo info;
    info.name = name;
    info.path = path;
    info.isLoaded = false;
    moduleMap[name] = info;
}

bool CHTLJSModuleMap::hasModule(const std::string& name) const {
    return moduleMap.find(name) != moduleMap.end();
}

const CHTLJSModuleMap::ModuleInfo* CHTLJSModuleMap::getModuleInfo(const std::string& name) const {
    auto it = moduleMap.find(name);
    return it != moduleMap.end() ? &it->second : nullptr;
}

void CHTLJSModuleMap::setModuleLoaded(const std::string& name, bool loaded) {
    auto it = moduleMap.find(name);
    if (it != moduleMap.end()) {
        it->second.isLoaded = loaded;
    }
}

void CHTLJSModuleMap::addModuleDependency(const std::string& module, const std::string& dependency) {
    auto it = moduleMap.find(module);
    if (it != moduleMap.end()) {
        it->second.dependencies.push_back(dependency);
    }
}

std::vector<std::string> CHTLJSModuleMap::getLoadOrder() const {
    std::vector<std::string> result;
    std::unordered_set<std::string> visited;
    
    // 简单的拓扑排序
    for (const auto& pair : moduleMap) {
        if (visited.find(pair.first) == visited.end()) {
            // 这里应该实现完整的拓扑排序，暂时简化
            result.push_back(pair.first);
            visited.insert(pair.first);
        }
    }
    
    return result;
}

bool CHTLJSModuleMap::hasCircularDependency() const {
    std::unordered_set<std::string> visited;
    std::unordered_set<std::string> recursionStack;
    
    for (const auto& pair : moduleMap) {
        if (visited.find(pair.first) == visited.end()) {
            if (detectCircularDependencyRecursive(pair.first, visited, recursionStack)) {
                return true;
            }
        }
    }
    
    return false;
}

bool CHTLJSModuleMap::detectCircularDependencyRecursive(const std::string& module, 
                                                       std::unordered_set<std::string>& visited,
                                                       std::unordered_set<std::string>& recursionStack) const {
    visited.insert(module);
    recursionStack.insert(module);
    
    auto it = moduleMap.find(module);
    if (it != moduleMap.end()) {
        for (const auto& dependency : it->second.dependencies) {
            if (recursionStack.find(dependency) != recursionStack.end()) {
                return true; // 发现循环依赖
            }
            
            if (visited.find(dependency) == visited.end()) {
                if (detectCircularDependencyRecursive(dependency, visited, recursionStack)) {
                    return true;
                }
            }
        }
    }
    
    recursionStack.erase(module);
    return false;
}

// CHTLJSVirtualObjectMap 实现
CHTLJSVirtualObjectMap::CHTLJSVirtualObjectMap() {
}

void CHTLJSVirtualObjectMap::registerVirtualObject(const std::string& name) {
    VirtualObject obj;
    obj.name = name;
    virtualObjects[name] = obj;
}

void CHTLJSVirtualObjectMap::addProperty(const std::string& objectName, const std::string& propertyName, const std::string& type) {
    auto it = virtualObjects.find(objectName);
    if (it != virtualObjects.end()) {
        it->second.properties[propertyName] = type;
    }
}

void CHTLJSVirtualObjectMap::addMethod(const std::string& objectName, const std::string& methodName, const std::vector<std::string>& paramTypes) {
    auto it = virtualObjects.find(objectName);
    if (it != virtualObjects.end()) {
        it->second.methods[methodName] = paramTypes;
    }
}

bool CHTLJSVirtualObjectMap::hasVirtualObject(const std::string& name) const {
    return virtualObjects.find(name) != virtualObjects.end();
}

const CHTLJSVirtualObjectMap::VirtualObject* CHTLJSVirtualObjectMap::getVirtualObject(const std::string& name) const {
    auto it = virtualObjects.find(name);
    return it != virtualObjects.end() ? &it->second : nullptr;
}

bool CHTLJSVirtualObjectMap::hasProperty(const std::string& objectName, const std::string& propertyName) const {
    auto it = virtualObjects.find(objectName);
    if (it != virtualObjects.end()) {
        return it->second.properties.find(propertyName) != it->second.properties.end();
    }
    return false;
}

bool CHTLJSVirtualObjectMap::hasMethod(const std::string& objectName, const std::string& methodName) const {
    auto it = virtualObjects.find(objectName);
    if (it != virtualObjects.end()) {
        return it->second.methods.find(methodName) != it->second.methods.end();
    }
    return false;
}

// CHTLJSConfigMap 实现
CHTLJSConfigMap::CHTLJSConfigMap() {
    initializeDefaultConfigs();
}

void CHTLJSConfigMap::initializeDefaultConfigs() {
    // CHTL JS默认配置
    setConfig("ENABLE_MODULE_SYSTEM", true);
    setConfig("ENABLE_VIRTUAL_OBJECTS", true);
    setConfig("ENABLE_EVENT_DELEGATION", true);
    setConfig("ENABLE_ANIMATION_SYSTEM", true);
    setConfig("STRICT_SELECTOR_CHECKING", false);
    setConfig("AUTO_EVENT_BINDING", true);
    setConfig("DEFAULT_ANIMATION_DURATION", 300);
    setConfig("DEFAULT_EASING", "ease-in-out");
}

void CHTLJSConfigMap::setConfig(const std::string& key, const std::string& value) {
    ConfigValue config;
    config.type = ConfigValue::STRING;
    config.stringValue = value;
    configMap[key] = config;
}

void CHTLJSConfigMap::setConfig(const std::string& key, bool value) {
    ConfigValue config;
    config.type = ConfigValue::BOOL;
    config.boolValue = value;
    configMap[key] = config;
}

void CHTLJSConfigMap::setConfig(const std::string& key, int value) {
    ConfigValue config;
    config.type = ConfigValue::INT;
    config.intValue = value;
    configMap[key] = config;
}

void CHTLJSConfigMap::setConfig(const std::string& key, const std::vector<std::string>& value) {
    ConfigValue config;
    config.type = ConfigValue::ARRAY;
    config.arrayValue = value;
    configMap[key] = config;
}

std::string CHTLJSConfigMap::getStringConfig(const std::string& key, const std::string& defaultValue) const {
    auto it = configMap.find(key);
    if (it != configMap.end() && it->second.type == ConfigValue::STRING) {
        return it->second.stringValue;
    }
    return defaultValue;
}

bool CHTLJSConfigMap::getBoolConfig(const std::string& key, bool defaultValue) const {
    auto it = configMap.find(key);
    if (it != configMap.end() && it->second.type == ConfigValue::BOOL) {
        return it->second.boolValue;
    }
    return defaultValue;
}

int CHTLJSConfigMap::getIntConfig(const std::string& key, int defaultValue) const {
    auto it = configMap.find(key);
    if (it != configMap.end() && it->second.type == ConfigValue::INT) {
        return it->second.intValue;
    }
    return defaultValue;
}

std::vector<std::string> CHTLJSConfigMap::getArrayConfig(const std::string& key) const {
    auto it = configMap.find(key);
    if (it != configMap.end() && it->second.type == ConfigValue::ARRAY) {
        return it->second.arrayValue;
    }
    return std::vector<std::string>();
}

bool CHTLJSConfigMap::hasConfig(const std::string& key) const {
    return configMap.find(key) != configMap.end();
}

void CHTLJSConfigMap::removeConfig(const std::string& key) {
    configMap.erase(key);
}

// CHTLJSGlobalMap 实现
CHTLJSGlobalMap::CHTLJSGlobalMap() {
}

void CHTLJSGlobalMap::reset() {
    keywordMap = CHTLJSKeywordMap();
    eventMap = CHTLJSEventMap();
    moduleMap = CHTLJSModuleMap();
    virtualObjectMap = CHTLJSVirtualObjectMap();
    configMap = CHTLJSConfigMap();
}

bool CHTLJSGlobalMap::loadFromConfigFile(const std::string& configFilePath) {
    // TODO: 实现从配置文件加载
    return false;
}

bool CHTLJSGlobalMap::saveToConfigFile(const std::string& configFilePath) const {
    // TODO: 实现保存到配置文件
    return false;
}

} // namespace CHTLJS