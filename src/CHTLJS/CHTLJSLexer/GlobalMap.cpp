#include "GlobalMap.h"

namespace CHTLJS {

void GlobalMap::registerVirtualObject(const std::string& name, const std::string& file) {
    virtualObjects_[name] = std::make_shared<VirtualObjectInfo>(name, file);
}

std::shared_ptr<VirtualObjectInfo> GlobalMap::getVirtualObject(const std::string& name) const {
    auto it = virtualObjects_.find(name);
    if (it != virtualObjects_.end()) {
        return it->second;
    }
    return nullptr;
}

void GlobalMap::registerModule(const std::string& path, bool isCJJS) {
    modules_[path] = std::make_shared<ModuleInfo>(path, isCJJS);
}

std::shared_ptr<ModuleInfo> GlobalMap::getModule(const std::string& path) const {
    auto it = modules_.find(path);
    if (it != modules_.end()) {
        return it->second;
    }
    return nullptr;
}

void GlobalMap::addModuleDependency(const std::string& module, const std::string& dependency) {
    auto moduleInfo = getModule(module);
    if (moduleInfo) {
        moduleInfo->addDependency(dependency);
    }
}

void GlobalMap::registerDelegation(const std::string& parentSelector) {
    if (delegations_.find(parentSelector) == delegations_.end()) {
        delegations_[parentSelector] = std::make_shared<DelegationInfo>(parentSelector);
    }
}

std::shared_ptr<DelegationInfo> GlobalMap::getDelegation(const std::string& parentSelector) const {
    auto it = delegations_.find(parentSelector);
    if (it != delegations_.end()) {
        return it->second;
    }
    return nullptr;
}

void GlobalMap::pushSelectorContext(const std::string& element) {
    SelectorAutoContext ctx;
    ctx.currentElement = element;
    selectorContextStack_.push_back(ctx);
}

void GlobalMap::popSelectorContext() {
    if (!selectorContextStack_.empty()) {
        selectorContextStack_.pop_back();
    }
}

GlobalMap::SelectorAutoContext* GlobalMap::getCurrentSelectorContext() {
    if (!selectorContextStack_.empty()) {
        return &selectorContextStack_.back();
    }
    return nullptr;
}

void GlobalMap::cacheSelector(const std::string& selector, const std::string& jsCode) {
    selectorCache_[selector] = jsCode;
}

std::optional<std::string> GlobalMap::getCachedSelector(const std::string& selector) const {
    auto it = selectorCache_.find(selector);
    if (it != selectorCache_.end()) {
        return it->second;
    }
    return std::nullopt;
}

void GlobalMap::registerCHTLJSFunction(const std::string& name, const std::string& type) {
    chtljsFunctions_.insert(name);
}

bool GlobalMap::isCHTLJSFunction(const std::string& name) const {
    return chtljsFunctions_.find(name) != chtljsFunctions_.end();
}

void GlobalMap::clear() {
    virtualObjects_.clear();
    modules_.clear();
    delegations_.clear();
    selectorContextStack_.clear();
    selectorCache_.clear();
    chtljsFunctions_.clear();
}

} // namespace CHTLJS