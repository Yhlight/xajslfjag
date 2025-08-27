#include "GlobalMap.h"
#include <algorithm>
#include <sstream>

namespace CHTL_JS {

// 支持的所有JavaScript事件类型
const std::unordered_set<std::string> GlobalMap::SUPPORTED_EVENTS = {
    // 鼠标事件
    "click", "dblclick", "mousedown", "mouseup", "mousemove", "mouseover", 
    "mouseout", "mouseenter", "mouseleave", "contextmenu", "wheel",
    
    // 键盘事件
    "keydown", "keyup", "keypress",
    
    // 表单事件
    "submit", "reset", "change", "input", "focus", "blur", "select",
    
    // 文档事件
    "load", "unload", "resize", "scroll", "error",
    
    // 拖拽事件
    "drag", "dragstart", "dragend", "dragover", "dragenter", "dragleave", "drop",
    
    // 触摸事件
    "touchstart", "touchend", "touchmove", "touchcancel",
    
    // 动画事件
    "animationstart", "animationend", "animationiteration",
    
    // 过渡事件
    "transitionstart", "transitionend", "transitionrun", "transitioncancel"
};

std::string GlobalMap::makeListenerKey(const std::string& selector, const std::string& event) const {
    return selector + ":" + event;
}

std::string GlobalMap::generateFunctionName(const std::string& baseName) const {
    static size_t counter = 0;
    std::ostringstream oss;
    oss << "__chtl_js_" << baseName << "_" << ++counter;
    return oss.str();
}

// 虚对象管理
void GlobalMap::registerVirtualObject(const std::string& name, 
                                     std::shared_ptr<VirtualObjectNode> object) {
    virtualObjects_[name] = object;
}

std::shared_ptr<VirtualObjectNode> GlobalMap::getVirtualObject(const std::string& name) const {
    auto it = virtualObjects_.find(name);
    return (it != virtualObjects_.end()) ? it->second : nullptr;
}

bool GlobalMap::hasVirtualObject(const std::string& name) const {
    return virtualObjects_.find(name) != virtualObjects_.end();
}

void GlobalMap::removeVirtualObject(const std::string& name) {
    virtualObjects_.erase(name);
}

// 监听器管理
void GlobalMap::registerListener(const std::string& selector, const std::string& event,
                                std::shared_ptr<ListenNode> listener) {
    std::string key = makeListenerKey(selector, event);
    listeners_[key].push_back(listener);
}

std::vector<std::shared_ptr<ListenNode>> GlobalMap::getListeners(const std::string& selector) const {
    std::vector<std::shared_ptr<ListenNode>> result;
    
    for (const auto& pair : listeners_) {
        if (pair.first.substr(0, pair.first.find(':')) == selector) {
            result.insert(result.end(), pair.second.begin(), pair.second.end());
        }
    }
    
    return result;
}

bool GlobalMap::hasListener(const std::string& selector, const std::string& event) const {
    std::string key = makeListenerKey(selector, event);
    auto it = listeners_.find(key);
    return it != listeners_.end() && !it->second.empty();
}

// 事件委托管理
void GlobalMap::registerDelegate(const std::string& parentSelector,
                                std::shared_ptr<DelegateNode> delegate) {
    delegates_[parentSelector].push_back(delegate);
}

std::vector<std::shared_ptr<DelegateNode>> GlobalMap::getDelegates(const std::string& parentSelector) const {
    auto it = delegates_.find(parentSelector);
    return (it != delegates_.end()) ? it->second : std::vector<std::shared_ptr<DelegateNode>>{};
}

bool GlobalMap::hasDelegate(const std::string& parentSelector) const {
    auto it = delegates_.find(parentSelector);
    return it != delegates_.end() && !it->second.empty();
}

// 动画管理
void GlobalMap::registerAnimation(const std::string& name,
                                 std::shared_ptr<AnimateNode> animation) {
    animations_[name] = animation;
}

std::shared_ptr<AnimateNode> GlobalMap::getAnimation(const std::string& name) const {
    auto it = animations_.find(name);
    return (it != animations_.end()) ? it->second : nullptr;
}

bool GlobalMap::hasAnimation(const std::string& name) const {
    return animations_.find(name) != animations_.end();
}

// 增强选择器管理
void GlobalMap::addEnhancedSelector(const std::string& selector, const std::string& resolvedSelector) {
    enhancedSelectors_[selector] = resolvedSelector;
}

std::string GlobalMap::resolveSelector(const std::string& selector) const {
    auto it = enhancedSelectors_.find(selector);
    return (it != enhancedSelectors_.end()) ? it->second : selector;
}

bool GlobalMap::isSelectorEnhanced(const std::string& selector) const {
    return enhancedSelectors_.find(selector) != enhancedSelectors_.end();
}

// 全局函数注册表
void GlobalMap::registerGlobalFunction(const std::string& name, const std::string& jsCode) {
    globalFunctions_[name] = jsCode;
}

std::string GlobalMap::getGlobalFunction(const std::string& name) const {
    auto it = globalFunctions_.find(name);
    return (it != globalFunctions_.end()) ? it->second : "";
}

std::unordered_map<std::string, std::string> GlobalMap::getAllGlobalFunctions() const {
    return globalFunctions_;
}

void GlobalMap::clearGlobalFunctions() {
    globalFunctions_.clear();
}

// 事件类型支持检查
bool GlobalMap::isSupportedEvent(const std::string& eventType) const {
    return SUPPORTED_EVENTS.find(eventType) != SUPPORTED_EVENTS.end();
}

std::vector<std::string> GlobalMap::getSupportedEvents() const {
    return std::vector<std::string>(SUPPORTED_EVENTS.begin(), SUPPORTED_EVENTS.end());
}

// DOM元素引用计数
void GlobalMap::incrementSelectorUsage(const std::string& selector) {
    selectorUsage_[selector]++;
}

void GlobalMap::decrementSelectorUsage(const std::string& selector) {
    auto it = selectorUsage_.find(selector);
    if (it != selectorUsage_.end() && it->second > 0) {
        it->second--;
        if (it->second == 0) {
            selectorUsage_.erase(it);
        }
    }
}

size_t GlobalMap::getSelectorUsage(const std::string& selector) const {
    auto it = selectorUsage_.find(selector);
    return (it != selectorUsage_.end()) ? it->second : 0;
}

// 变量组引用
void GlobalMap::registerVariableGroup(const std::string& groupName, 
                                     const std::unordered_map<std::string, std::string>& variables) {
    variableGroups_[groupName] = variables;
}

std::string GlobalMap::getVariable(const std::string& groupName, const std::string& varName) const {
    auto groupIt = variableGroups_.find(groupName);
    if (groupIt != variableGroups_.end()) {
        auto varIt = groupIt->second.find(varName);
        if (varIt != groupIt->second.end()) {
            return varIt->second;
        }
    }
    return "";
}

bool GlobalMap::hasVariableGroup(const std::string& groupName) const {
    return variableGroups_.find(groupName) != variableGroups_.end();
}

// 清理方法
void GlobalMap::clear() {
    clearVirtualObjects();
    clearListeners();
    clearDelegates();
    clearAnimations();
    enhancedSelectors_.clear();
    globalFunctions_.clear();
    selectorUsage_.clear();
    variableGroups_.clear();
}

void GlobalMap::clearVirtualObjects() {
    virtualObjects_.clear();
}

void GlobalMap::clearListeners() {
    listeners_.clear();
}

void GlobalMap::clearDelegates() {
    delegates_.clear();
}

void GlobalMap::clearAnimations() {
    animations_.clear();
}

} // namespace CHTL_JS