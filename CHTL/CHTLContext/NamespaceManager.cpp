#include "NamespaceManager.h"
#include <algorithm>
#include <sstream>
#include <stdexcept>

namespace CHTL {

NamespaceManager::NamespaceManager(std::shared_ptr<ConfigurationManager> configManager)
    : configManager_(configManager) {
    // 检查是否禁用默认命名空间
    if (configManager_) {
        auto disableDefaultNamespace = configManager_->getConfig("", "DISABLE_DEFAULT_NAMESPACE");
        if (disableDefaultNamespace.type == ConfigItemType::BOOLEAN) {
            // 如果配置禁用默认命名空间，则不创建
            if (disableDefaultNamespace.get<bool>()) {
                return;
            }
        }
    }
    
    // 创建默认命名空间
    createNamespace("", "");
}

bool NamespaceManager::createNamespace(const std::string& name, const std::string& sourceFile) {
    if (!isValidNamespaceName(name)) {
        return false;
    }
    
    if (hasNamespace(name)) {
        return false; // 命名空间已存在
    }
    
    auto namespace_ = std::make_shared<Namespace>(name, sourceFile);
    namespaces_[name] = namespace_;
    
    // 更新统计
    statistics_.totalNamespaces++;
    
    return true;
}

bool NamespaceManager::createNestedNamespace(const std::string& parentName, const std::string& childName, const std::string& sourceFile) {
    if (!hasNamespace(parentName)) {
        return false;
    }
    
    if (!hasNamespace(childName)) {
        if (!createNamespace(childName, sourceFile)) {
            return false;
        }
    }
    
    auto parent = getNamespace(parentName);
    if (parent) {
        parent->subNamespaces.push_back(childName);
        parent->nestedNamespaces[childName] = getNamespace(childName);
        statistics_.nestedNamespaces++;
    }
    
    return true;
}

bool NamespaceManager::addNamespaceItem(const std::string& namespaceName, std::shared_ptr<NamespaceItem> item) {
    if (!hasNamespace(namespaceName)) {
        return false;
    }
    
    if (!isValidNamespaceItem(item)) {
        return false;
    }
    
    auto namespace_ = getNamespace(namespaceName);
    if (namespace_) {
        namespace_->items.push_back(item);
        statistics_.totalItems++;
        return true;
    }
    
    return false;
}

std::shared_ptr<NamespaceItem> NamespaceManager::getNamespaceItem(const std::string& namespaceName, const std::string& itemName) {
    auto namespace_ = getNamespace(namespaceName);
    if (!namespace_) {
        return nullptr;
    }
    
    for (const auto& item : namespace_->items) {
        if (item->name == itemName) {
            return item;
        }
    }
    
    return nullptr;
}

std::vector<std::shared_ptr<NamespaceItem>> NamespaceManager::getNamespaceItems(const std::string& namespaceName) {
    auto namespace_ = getNamespace(namespaceName);
    if (!namespace_) {
        return {};
    }
    
    return namespace_->items;
}

bool NamespaceManager::removeNamespaceItem(const std::string& namespaceName, const std::string& itemName) {
    auto namespace_ = getNamespace(namespaceName);
    if (!namespace_) {
        return false;
    }
    
    auto it = std::find_if(namespace_->items.begin(), namespace_->items.end(),
                           [&itemName](const std::shared_ptr<NamespaceItem>& item) {
                               return item->name == itemName;
                           });
    
    if (it != namespace_->items.end()) {
        namespace_->items.erase(it);
        statistics_.totalItems--;
        return true;
    }
    
    return false;
}

std::shared_ptr<Namespace> NamespaceManager::getNamespace(const std::string& name) const {
    auto it = namespaces_.find(name);
    if (it != namespaces_.end()) {
        return it->second;
    }
    return nullptr;
}

std::vector<std::string> NamespaceManager::getNamespaceNames() const {
    std::vector<std::string> names;
    for (const auto& [name, _] : namespaces_) {
        names.push_back(name);
    }
    return names;
}

bool NamespaceManager::hasNamespace(const std::string& name) const {
    return namespaces_.count(name) > 0;
}

bool NamespaceManager::mergeNamespaces(const std::string& name) {
    auto namespace_ = getNamespace(name);
    if (!namespace_) {
        return false;
    }
    
    // 查找同名命名空间进行合并
    std::vector<std::string> toMerge;
    for (const auto& [nsName, ns] : namespaces_) {
        if (nsName != name && nsName.find(name + ".") == 0) {
            toMerge.push_back(nsName);
        }
    }
    
    for (const auto& nsName : toMerge) {
        if (!mergeNestedNamespaces(name, nsName)) {
            return false;
        }
    }
    
    statistics_.mergedNamespaces++;
    return true;
}

bool NamespaceManager::mergeNestedNamespaces(const std::string& parent, const std::string& child) {
    auto parentNs = getNamespace(parent);
    auto childNs = getNamespace(child);
    
    if (!parentNs || !childNs) {
        return false;
    }
    
    // 合并项目
    mergeItems(parentNs->items, childNs->items);
    
    // 合并子命名空间
    mergeSubNamespaces(parentNs->subNamespaces, childNs->subNamespaces);
    
    // 合并嵌套命名空间
    mergeNestedNamespaces(parentNs->nestedNamespaces, childNs->nestedNamespaces);
    
    // 删除子命名空间
    namespaces_.erase(child);
    statistics_.totalNamespaces--;
    
    return true;
}

std::vector<NamespaceConflict> NamespaceManager::detectConflicts() {
    std::vector<NamespaceConflict> conflicts;
    
    for (const auto& [nsName, ns] : namespaces_) {
        for (const auto& item : ns->items) {
            if (hasConflict(item->name, item->type, nsName)) {
                addConflict(item->name, item->type, nsName, item->sourceFile, item->line);
            }
        }
    }
    
    statistics_.totalConflicts = conflicts.size();
    return conflicts;
}

bool NamespaceManager::hasConflicts() const {
    return statistics_.totalConflicts > 0;
}

std::vector<NamespaceConflict> NamespaceManager::getConflictsForNamespace(const std::string& namespaceName) {
    // 简化实现：返回空列表
    return {};
}

std::vector<std::string> NamespaceManager::parseNamespacePath(const std::string& path) {
    return splitNamespacePath(path);
}

std::string NamespaceManager::normalizeNamespacePath(const std::string& path) {
    auto parts = splitNamespacePath(path);
    return joinNamespacePath(parts);
}

std::string NamespaceManager::resolveNamespacePath(const std::string& path) {
    if (isAbsoluteNamespacePath(path)) {
        return path;
    }
    
    if (defaultNamespace_.empty()) {
        return path;
    }
    
    return defaultNamespace_ + "." + path;
}

void NamespaceManager::setDefaultNamespace(const std::string& name) {
    defaultNamespace_ = name;
}

bool NamespaceManager::isDefaultNamespaceEnabled() const {
    return !isDefaultNamespaceDisabled();
}

bool NamespaceManager::inheritNamespace(const std::string& child, const std::string& parent) {
    if (child == parent) {
        return false;
    }
    
    if (!hasNamespace(child) || !hasNamespace(parent)) {
        return false;
    }
    
    inheritanceMap_[child] = parent;
    return true;
}

std::vector<std::string> NamespaceManager::getInheritanceChain(const std::string& namespaceName) {
    std::vector<std::string> chain;
    std::string current = namespaceName;
    
    while (!current.empty() && inheritanceMap_.count(current) > 0) {
        chain.push_back(current);
        current = inheritanceMap_.at(current);
    }
    
    if (!current.empty()) {
        chain.push_back(current);
    }
    
    return chain;
}

std::string NamespaceManager::exportNamespace(const std::string& name) const {
    auto namespace_ = getNamespace(name);
    if (!namespace_) {
        return "";
    }
    
    std::ostringstream oss;
    oss << "[Namespace] " << name << "\n";
    oss << "Source: " << namespace_->sourceFile << "\n";
    oss << "Items: " << namespace_->items.size() << "\n";
    oss << "Sub-namespaces: " << namespace_->subNamespaces.size() << "\n";
    
    return oss.str();
}

bool NamespaceManager::importNamespace(const std::string& exportData, const std::string& name) {
    // 简化实现：总是返回成功
    return true;
}

void NamespaceManager::clearNamespace(const std::string& name) {
    auto it = namespaces_.find(name);
    if (it != namespaces_.end()) {
        statistics_.totalItems -= it->second->items.size();
        namespaces_.erase(it);
        statistics_.totalNamespaces--;
    }
}

void NamespaceManager::clearAllNamespaces() {
    namespaces_.clear();
    statistics_.totalNamespaces = 0;
    statistics_.totalItems = 0;
}

bool NamespaceManager::validateNamespace(const std::string& name) {
    return isValidNamespaceName(name) && hasNamespace(name);
}

std::vector<std::string> NamespaceManager::getValidationErrors(const std::string& name) {
    std::vector<std::string> errors;
    
    if (!isValidNamespaceName(name)) {
        errors.push_back("Invalid namespace name: " + name);
    }
    
    if (!hasNamespace(name)) {
        errors.push_back("Namespace not found: " + name);
    }
    
    return errors;
}

void NamespaceManager::clearStatistics() {
    statistics_ = NamespaceStatistics();
}

std::string NamespaceManager::getDebugInfo() const {
    std::ostringstream oss;
    oss << "NamespaceManager Debug Info:\n";
    oss << "Total namespaces: " << statistics_.totalNamespaces << "\n";
    oss << "Total items: " << statistics_.totalItems << "\n";
    oss << "Total conflicts: " << statistics_.totalConflicts << "\n";
    oss << "Merged namespaces: " << statistics_.mergedNamespaces << "\n";
    oss << "Nested namespaces: " << statistics_.nestedNamespaces << "\n";
    oss << "Default namespace: " << (defaultNamespace_.empty() ? "(none)" : defaultNamespace_) << "\n";
    
    return oss.str();
}

std::string NamespaceManager::getNamespaceInfo(const std::string& name) const {
    auto namespace_ = getNamespace(name);
    if (!namespace_) {
        return "Namespace not found: " + name;
    }
    
    std::ostringstream oss;
    oss << "Namespace: " << name << "\n";
    oss << "Source file: " << (namespace_->sourceFile.empty() ? "(none)" : namespace_->sourceFile) << "\n";
    oss << "Items: " << namespace_->items.size() << "\n";
    oss << "Sub-namespaces: " << namespace_->subNamespaces.size() << "\n";
    oss << "Nested namespaces: " << namespace_->nestedNamespaces.size() << "\n";
    
    return oss.str();
}

void NamespaceManager::reset() {
    clearAllNamespaces();
    defaultNamespace_.clear();
    inheritanceMap_.clear();
    clearStatistics();
    
    // 重新创建默认命名空间
    if (!isDefaultNamespaceDisabled()) {
        createNamespace("", "");
    }
}

bool NamespaceManager::isValidNamespaceName(const std::string& name) const {
    if (name.empty()) {
        return true; // 允许空名称（默认命名空间）
    }
    
    // 检查是否包含非法字符
    for (char c : name) {
        if (!std::isalnum(c) && c != '_' && c != '.' && c != '-') {
            return false;
        }
    }
    
    return true;
}

bool NamespaceManager::isValidNamespaceItem(const std::shared_ptr<NamespaceItem>& item) const {
    if (!item) {
        return false;
    }
    
    if (item->name.empty()) {
        return false;
    }
    
    return true;
}

std::string NamespaceManager::generateNamespaceKey(const std::string& prefix, const std::string& suffix) const {
    if (prefix.empty() && suffix.empty()) {
        return "";
    }
    
    if (prefix.empty()) {
        return suffix;
    }
    
    if (suffix.empty()) {
        return prefix;
    }
    
    return prefix + "_" + suffix;
}

std::vector<std::string> NamespaceManager::splitNamespacePath(const std::string& path) {
    std::vector<std::string> parts;
    std::istringstream iss(path);
    std::string part;
    
    while (std::getline(iss, part, '.')) {
        if (!part.empty()) {
            parts.push_back(part);
        }
    }
    
    return parts;
}

std::string NamespaceManager::joinNamespacePath(const std::vector<std::string>& parts) {
    if (parts.empty()) {
        return "";
    }
    
    std::ostringstream oss;
    for (size_t i = 0; i < parts.size(); ++i) {
        if (i > 0) {
            oss << ".";
        }
        oss << parts[i];
    }
    
    return oss.str();
}

bool NamespaceManager::isAbsoluteNamespacePath(const std::string& path) const {
    return !path.empty() && path[0] == '/';
}

bool NamespaceManager::hasConflict(const std::string& itemName, NamespaceItemType type, const std::string& namespaceName) {
    // 简化实现：总是返回 false
    return false;
}

void NamespaceManager::addConflict(const std::string& itemName, NamespaceItemType type, 
                                  const std::string& namespaceName, const std::string& sourceFile, size_t line) {
    // 简化实现：不执行任何操作
}

void NamespaceManager::mergeItems(std::vector<std::shared_ptr<NamespaceItem>>& target, 
                                 const std::vector<std::shared_ptr<NamespaceItem>>& source) {
    target.insert(target.end(), source.begin(), source.end());
}

void NamespaceManager::mergeSubNamespaces(std::vector<std::string>& target, 
                                         const std::vector<std::string>& source) {
    for (const auto& ns : source) {
        if (std::find(target.begin(), target.end(), ns) == target.end()) {
            target.push_back(ns);
        }
    }
}

void NamespaceManager::mergeNestedNamespaces(std::unordered_map<std::string, std::shared_ptr<Namespace>>& target,
                                            const std::unordered_map<std::string, std::shared_ptr<Namespace>>& source) {
    for (const auto& [name, ns] : source) {
        target[name] = ns;
    }
}

void NamespaceManager::buildInheritanceMap() {
    // 简化实现：不执行任何操作
}

bool NamespaceManager::hasCircularInheritance(const std::string& namespaceName, std::unordered_set<std::string>& visited) {
    // 简化实现：总是返回 false
    return false;
}

void NamespaceManager::updateStatistics(const std::string& type, size_t value) {
    if (type == "namespace") {
        statistics_.totalNamespaces += value;
    } else if (type == "item") {
        statistics_.totalItems += value;
    } else if (type == "conflict") {
        statistics_.totalConflicts += value;
    } else if (type == "merge") {
        statistics_.mergedNamespaces += value;
    } else if (type == "nested") {
        statistics_.nestedNamespaces += value;
    }
}

bool NamespaceManager::isDefaultNamespaceDisabled() const {
    if (!configManager_) {
        return false;
    }
    
    auto config = configManager_->getConfig("", "DISABLE_DEFAULT_NAMESPACE");
    if (config.type == ConfigItemType::BOOLEAN) {
        return config.get<bool>();
    }
    
    return false;
}

std::string NamespaceManager::getDefaultNamespaceFromConfig() const {
    if (!configManager_) {
        return "";
    }
    
    auto config = configManager_->getConfig("", "DEFAULT_NAMESPACE");
    if (config.type == ConfigItemType::STRING) {
        return config.get<std::string>();
    }
    
    return "";
}

} // namespace CHTL