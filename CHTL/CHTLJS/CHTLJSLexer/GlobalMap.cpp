#include "GlobalMap.h"
#include <sstream>

namespace CHTLJS {

// GlobalMap实现
GlobalMap::GlobalMap() 
    : functionCounter(0), variableCounter(0), animationCounter(0) {
}

GlobalMap::~GlobalMap() = default;

void GlobalMap::AddVirtualObject(const std::string& name, std::shared_ptr<VirtualObject> obj) {
    virtualObjects[name] = obj;
}

std::shared_ptr<VirtualObject> GlobalMap::GetVirtualObject(const std::string& name) const {
    auto it = virtualObjects.find(name);
    if (it != virtualObjects.end()) {
        return it->second;
    }
    return nullptr;
}

bool GlobalMap::HasVirtualObject(const std::string& name) const {
    return virtualObjects.find(name) != virtualObjects.end();
}

void GlobalMap::RemoveVirtualObject(const std::string& name) {
    virtualObjects.erase(name);
}

void GlobalMap::RegisterCHTLJSFunction(const std::string& name, std::shared_ptr<CHTLJSFunction> func) {
    chtljsFunctions[name] = func;
}

std::shared_ptr<CHTLJSFunction> GlobalMap::GetCHTLJSFunction(const std::string& name) const {
    auto it = chtljsFunctions.find(name);
    if (it != chtljsFunctions.end()) {
        return it->second;
    }
    return nullptr;
}

bool GlobalMap::IsCHTLJSFunction(const std::string& name) const {
    return chtljsFunctions.find(name) != chtljsFunctions.end();
}

void GlobalMap::CacheSelector(const std::string& selector, const std::string& jsCode) {
    selectorCache[selector] = jsCode;
}

std::string GlobalMap::GetCachedSelector(const std::string& selector) const {
    auto it = selectorCache.find(selector);
    if (it != selectorCache.end()) {
        return it->second;
    }
    return "";
}

bool GlobalMap::HasCachedSelector(const std::string& selector) const {
    return selectorCache.find(selector) != selectorCache.end();
}

void GlobalMap::RegisterEventDelegation(const std::string& parentSelector, 
                                       std::shared_ptr<EventDelegation> delegation) {
    eventDelegations[parentSelector] = delegation;
}

std::shared_ptr<EventDelegation> GlobalMap::GetEventDelegation(const std::string& parentSelector) const {
    auto it = eventDelegations.find(parentSelector);
    if (it != eventDelegations.end()) {
        return it->second;
    }
    return nullptr;
}

std::vector<std::string> GlobalMap::GetAllDelegationParents() const {
    std::vector<std::string> parents;
    for (const auto& pair : eventDelegations) {
        parents.push_back(pair.first);
    }
    return parents;
}

std::string GlobalMap::GenerateAnimationId() {
    return "chtljs_anim_" + std::to_string(++animationCounter);
}

void GlobalMap::RegisterAnimation(const std::string& id, const std::string& config) {
    animations[id] = config;
}

std::string GlobalMap::GetAnimation(const std::string& id) const {
    auto it = animations.find(id);
    if (it != animations.end()) {
        return it->second;
    }
    return "";
}

std::string GlobalMap::GenerateUniqueFunctionName(const std::string& prefix) {
    return prefix + "_" + std::to_string(++functionCounter);
}

std::string GlobalMap::GenerateUniqueVariableName(const std::string& prefix) {
    return prefix + "_" + std::to_string(++variableCounter);
}

void GlobalMap::Clear() {
    virtualObjects.clear();
    chtljsFunctions.clear();
    selectorCache.clear();
    eventDelegations.clear();
    animations.clear();
    functionCounter = 0;
    variableCounter = 0;
    animationCounter = 0;
}

// VirtualObject实现
VirtualObject::VirtualObject(const std::string& name) : name(name) {}

void VirtualObject::AddProperty(const std::string& key, const std::string& value, bool isFunction) {
    properties[key] = {value, isFunction};
}

std::string VirtualObject::GetProperty(const std::string& key) const {
    auto it = properties.find(key);
    if (it != properties.end()) {
        return it->second.first;
    }
    return "";
}

bool VirtualObject::HasProperty(const std::string& key) const {
    return properties.find(key) != properties.end();
}

bool VirtualObject::IsFunction(const std::string& key) const {
    auto it = properties.find(key);
    if (it != properties.end()) {
        return it->second.second;
    }
    return false;
}

// CHTLJSFunction实现
CHTLJSFunction::CHTLJSFunction(const std::string& name, const std::string& pattern)
    : name(name), pattern(pattern) {
}

void CHTLJSFunction::AddParameter(const std::string& param, bool required, bool ordered) {
    parameters.push_back(param);
    if (required) {
        requiredParams.insert(param);
    }
    if (!ordered) {
        unorderedParams.insert(param);
    }
}

std::string CHTLJSFunction::GenerateJS(const std::unordered_map<std::string, std::string>& args) const {
    // TODO: 根据pattern和参数生成JS代码
    std::stringstream ss;
    ss << name << "({";
    
    bool first = true;
    for (const auto& [key, value] : args) {
        if (!first) ss << ", ";
        ss << key << ": " << value;
        first = false;
    }
    
    ss << "})";
    return ss.str();
}

// EventDelegation实现
EventDelegation::EventDelegation(const std::string& parentSelector)
    : parentSelector(parentSelector) {
}

void EventDelegation::AddTarget(const std::string& targetSelector, const std::string& event, 
                               const std::string& handler) {
    targets[targetSelector][event] = handler;
}

std::string EventDelegation::GenerateJS() const {
    std::stringstream ss;
    
    // 生成事件委托的JS代码
    ss << "(function() {\n";
    ss << "    const parent = document.querySelector('" << parentSelector << "');\n";
    ss << "    if (!parent) return;\n\n";
    
    // 为每个事件类型生成委托处理器
    std::unordered_map<std::string, std::vector<std::pair<std::string, std::string>>> eventMap;
    
    for (const auto& [target, events] : targets) {
        for (const auto& [event, handler] : events) {
            eventMap[event].push_back({target, handler});
        }
    }
    
    for (const auto& [event, handlers] : eventMap) {
        ss << "    parent.addEventListener('" << event << "', function(e) {\n";
        
        for (const auto& [target, handler] : handlers) {
            ss << "        if (e.target.matches('" << target << "')) {\n";
            ss << "            (" << handler << ").call(e.target, e);\n";
            ss << "        }\n";
        }
        
        ss << "    });\n\n";
    }
    
    ss << "})();\n";
    
    return ss.str();
}

} // namespace CHTLJS