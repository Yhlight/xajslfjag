#include "GlobalMap.h"
#include <algorithm>
#include <sstream>

namespace CHTL {

std::string GlobalMap::makeKey(const std::string& type, const std::string& name) const {
    return type + ":" + name;
}

// 模板管理
void GlobalMap::registerTemplate(const std::string& type, const std::string& name, 
                                std::shared_ptr<TemplateNode> node) {
    std::string key = makeKey(type, name);
    templates_[key] = node;
}

std::shared_ptr<TemplateNode> GlobalMap::getTemplate(const std::string& type, 
                                                    const std::string& name) const {
    std::string key = makeKey(type, name);
    auto it = templates_.find(key);
    return (it != templates_.end()) ? it->second : nullptr;
}

bool GlobalMap::hasTemplate(const std::string& type, const std::string& name) const {
    std::string key = makeKey(type, name);
    return templates_.find(key) != templates_.end();
}

void GlobalMap::removeTemplate(const std::string& type, const std::string& name) {
    std::string key = makeKey(type, name);
    templates_.erase(key);
}

// 自定义管理
void GlobalMap::registerCustom(const std::string& type, const std::string& name, 
                              std::shared_ptr<CustomNode> node) {
    std::string key = makeKey(type, name);
    customs_[key] = node;
}

std::shared_ptr<CustomNode> GlobalMap::getCustom(const std::string& type, 
                                                const std::string& name) const {
    std::string key = makeKey(type, name);
    auto it = customs_.find(key);
    return (it != customs_.end()) ? it->second : nullptr;
}

bool GlobalMap::hasCustom(const std::string& type, const std::string& name) const {
    std::string key = makeKey(type, name);
    return customs_.find(key) != customs_.end();
}

void GlobalMap::removeCustom(const std::string& type, const std::string& name) {
    std::string key = makeKey(type, name);
    customs_.erase(key);
}

// 命名空间管理
void GlobalMap::registerNamespace(const std::string& name, 
                                 std::shared_ptr<NamespaceNode> node) {
    namespaces_[name] = node;
}

std::shared_ptr<NamespaceNode> GlobalMap::getNamespace(const std::string& name) const {
    auto it = namespaces_.find(name);
    return (it != namespaces_.end()) ? it->second : nullptr;
}

bool GlobalMap::hasNamespace(const std::string& name) const {
    return namespaces_.find(name) != namespaces_.end();
}

void GlobalMap::removeNamespace(const std::string& name) {
    namespaces_.erase(name);
}

// 配置管理
void GlobalMap::registerConfig(const std::string& name, 
                              std::shared_ptr<ConfigNode> node) {
    configs_[name] = node;
    
    // 如果没有活动配置，设置为第一个
    if (activeConfigName_.empty()) {
        activeConfigName_ = name;
    }
}

std::shared_ptr<ConfigNode> GlobalMap::getConfig(const std::string& name) const {
    auto it = configs_.find(name);
    return (it != configs_.end()) ? it->second : nullptr;
}

std::shared_ptr<ConfigNode> GlobalMap::getActiveConfig() const {
    if (activeConfigName_.empty()) {
        return nullptr;
    }
    return getConfig(activeConfigName_);
}

void GlobalMap::setActiveConfig(const std::string& name) {
    if (hasConfig(name)) {
        activeConfigName_ = name;
    }
}

bool GlobalMap::hasConfig(const std::string& name) const {
    return configs_.find(name) != configs_.end();
}

// 原始嵌入管理
void GlobalMap::registerOrigin(const std::string& type, const std::string& name, 
                              const std::string& content) {
    std::string key = makeKey(type, name);
    origins_[key] = content;
}

std::string GlobalMap::getOrigin(const std::string& type, const std::string& name) const {
    std::string key = makeKey(type, name);
    auto it = origins_.find(key);
    return (it != origins_.end()) ? it->second : "";
}

bool GlobalMap::hasOrigin(const std::string& type, const std::string& name) const {
    std::string key = makeKey(type, name);
    return origins_.find(key) != origins_.end();
}

// 导入路径管理
void GlobalMap::addImportPath(const std::string& path) {
    importedPaths_.insert(path);
}

void GlobalMap::removeImportPath(const std::string& path) {
    importedPaths_.erase(path);
}

std::vector<std::string> GlobalMap::getImportPaths() const {
    return std::vector<std::string>(importedPaths_.begin(), importedPaths_.end());
}

bool GlobalMap::isPathImported(const std::string& path) const {
    return importedPaths_.find(path) != importedPaths_.end();
}

// 循环依赖检测
void GlobalMap::pushImportStack(const std::string& path) {
    importStack_.push_back(path);
}

void GlobalMap::popImportStack() {
    if (!importStack_.empty()) {
        importStack_.pop_back();
    }
}

bool GlobalMap::hasCircularDependency(const std::string& path) const {
    return std::find(importStack_.begin(), importStack_.end(), path) != importStack_.end();
}

// HTML标签类名/ID自动分配
std::string GlobalMap::allocateClassName(const std::string& base) {
    std::string name;
    
    if (base.empty()) {
        do {
            std::ostringstream oss;
            oss << "chtl-class-" << ++classCounter_;
            name = oss.str();
        } while (isClassNameUsed(name));
    } else {
        name = base;
        if (isClassNameUsed(name)) {
            size_t counter = 1;
            do {
                std::ostringstream oss;
                oss << base << "-" << counter++;
                name = oss.str();
            } while (isClassNameUsed(name));
        }
    }
    
    usedClassNames_.insert(name);
    return name;
}

std::string GlobalMap::allocateIdName(const std::string& base) {
    std::string name;
    
    if (base.empty()) {
        do {
            std::ostringstream oss;
            oss << "chtl-id-" << ++idCounter_;
            name = oss.str();
        } while (isIdNameUsed(name));
    } else {
        name = base;
        if (isIdNameUsed(name)) {
            size_t counter = 1;
            do {
                std::ostringstream oss;
                oss << base << "-" << counter++;
                name = oss.str();
            } while (isIdNameUsed(name));
        }
    }
    
    usedIdNames_.insert(name);
    return name;
}

bool GlobalMap::isClassNameUsed(const std::string& name) const {
    return usedClassNames_.find(name) != usedClassNames_.end();
}

bool GlobalMap::isIdNameUsed(const std::string& name) const {
    return usedIdNames_.find(name) != usedIdNames_.end();
}

// 类/ID选择器管理
void GlobalMap::registerClassSelector(const std::string& name) {
    classSelectors_.insert(name);
    usedClassNames_.insert(name);
}

void GlobalMap::registerIdSelector(const std::string& name) {
    idSelectors_.insert(name);
    usedIdNames_.insert(name);
}

std::unordered_set<std::string> GlobalMap::getClassSelectors() const {
    return classSelectors_;
}

std::unordered_set<std::string> GlobalMap::getIdSelectors() const {
    return idSelectors_;
}

// 清理方法
void GlobalMap::clear() {
    clearTemplates();
    clearCustoms();
    clearNamespaces();
    clearConfigs();
    origins_.clear();
    importedPaths_.clear();
    importStack_.clear();
    usedClassNames_.clear();
    usedIdNames_.clear();
    classSelectors_.clear();
    idSelectors_.clear();
    classCounter_ = 0;
    idCounter_ = 0;
}

void GlobalMap::clearTemplates() {
    templates_.clear();
}

void GlobalMap::clearCustoms() {
    customs_.clear();
}

void GlobalMap::clearNamespaces() {
    namespaces_.clear();
}

void GlobalMap::clearConfigs() {
    configs_.clear();
    activeConfigName_.clear();
}

} // namespace CHTL