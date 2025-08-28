#include "NamespaceManager.h"
#include <algorithm>
#include <sstream>
#include <filesystem>

namespace CHTL {

NamespaceManager::NamespaceManager(std::shared_ptr<ConfigurationManager> configManager)
    : configManager_(configManager), defaultNamespaceEnabled_(true) {
    if (configManager_) {
        auto disableDefaultNamespace = configManager_->getConfig("", "DISABLE_DEFAULT_NAMESPACE");
        if (disableDefaultNamespace.type == ConfigValueType::BOOLEAN) {
            defaultNamespaceEnabled_ = !disableDefaultNamespace.boolValue;
        }
    }
}

NamespaceManager::~NamespaceManager() = default;

bool NamespaceManager::createNamespace(const std::string& name, const std::string& sourceFile, bool isDefault) {
    if (!validateNamespaceName(name)) {
        return false;
    }
    
    if (namespaceExists(name)) {
        return false;
    }
    
    auto namespace_ = std::make_shared<Namespace>();
    namespace_->name = name;
    namespace_->sourceFile = sourceFile;
    namespace_->isDefault = isDefault;
    
    namespaces_[name] = namespace_;
    return true;
}

bool NamespaceManager::createNestedNamespace(const std::string& parentName, const std::string& childName, const std::string& sourceFile) {
    if (!namespaceExists(parentName)) {
        return false;
    }
    
    auto parent = namespaces_[parentName];
    std::vector<std::string> pathComponents = {childName};
    return createNestedNamespacePath(parent, pathComponents, sourceFile);
}

bool NamespaceManager::addNamespaceItem(const std::string& namespaceName, const std::shared_ptr<NamespaceItem>& item) {
    if (!namespaceExists(namespaceName)) {
        return false;
    }
    
    auto namespace_ = namespaces_[namespaceName];
    
    // 检查是否已存在同名同类型项
    for (const auto& existingItem : namespace_->items) {
        if (existingItem->name == item->name && existingItem->type == item->type) {
            return false; // 冲突
        }
    }
    
    namespace_->items.push_back(item);
    return true;
}

std::shared_ptr<Namespace> NamespaceManager::getNamespace(const std::string& name) {
    auto it = namespaces_.find(name);
    if (it != namespaces_.end()) {
        return it->second;
    }
    return nullptr;
}

const std::unordered_map<std::string, std::shared_ptr<Namespace>>& NamespaceManager::getAllNamespaces() const {
    return namespaces_;
}

bool NamespaceManager::namespaceExists(const std::string& name) const {
    return namespaces_.find(name) != namespaces_.end();
}

bool NamespaceManager::namespaceItemExists(const std::string& namespaceName, const std::string& itemName, NamespaceItemType itemType) const {
    auto it = namespaces_.find(namespaceName);
    if (it == namespaces_.end()) {
        return false;
    }
    
    for (const auto& item : it->second->items) {
        if (item->name == itemName && item->type == itemType) {
            return true;
        }
    }
    
    return false;
}

bool NamespaceManager::mergeNamespaces(const std::string& name) {
    auto it = namespaces_.find(name);
    if (it == namespaces_.end()) {
        return false;
    }
    
    auto namespace_ = it->second;
    
    // 查找所有同名命名空间
    std::vector<std::shared_ptr<Namespace>> toMerge;
    for (auto& pair : namespaces_) {
        if (pair.first == name && pair.second != namespace_) {
            toMerge.push_back(pair.second);
        }
    }
    
    if (toMerge.empty()) {
        return true; // 没有需要合并的
    }
    
    // 合并所有同名命名空间
    for (auto& mergeTarget : toMerge) {
        // 合并项
        for (const auto& item : mergeTarget->items) {
            bool added = false;
            for (const auto& existingItem : namespace_->items) {
                if (existingItem->name == item->name && existingItem->type == item->type) {
                    added = true;
                    break;
                }
            }
            if (!added) {
                namespace_->items.push_back(item);
            }
        }
        
        // 合并子命名空间
        for (const auto& subNamespace : mergeTarget->subNamespaces) {
            if (std::find(namespace_->subNamespaces.begin(), namespace_->subNamespaces.end(), subNamespace) == namespace_->subNamespaces.end()) {
                namespace_->subNamespaces.push_back(subNamespace);
            }
        }
        
        // 合并嵌套命名空间
        for (const auto& nestedPair : mergeTarget->nestedNamespaces) {
            if (namespace_->nestedNamespaces.find(nestedPair.first) == namespace_->nestedNamespaces.end()) {
                namespace_->nestedNamespaces[nestedPair.first] = nestedPair.second;
            }
        }
        
        // 删除已合并的命名空间
        namespaces_.erase(mergeTarget->name);
    }
    
    return true;
}

std::vector<NamespaceConflict> NamespaceManager::detectConflicts() const {
    std::vector<NamespaceConflict> conflicts;
    
    // 检查所有命名空间之间的冲突
    std::vector<std::string> namespaceNames;
    for (const auto& pair : namespaces_) {
        namespaceNames.push_back(pair.first);
    }
    
    for (size_t i = 0; i < namespaceNames.size(); ++i) {
        for (size_t j = i + 1; j < namespaceNames.size(); ++j) {
            const auto& namespace1 = namespaces_.at(namespaceNames[i]);
            const auto& namespace2 = namespaces_.at(namespaceNames[j]);
            checkNamespaceConflicts(namespace1, namespace2, conflicts);
        }
    }
    
    return conflicts;
}

std::vector<std::string> NamespaceManager::parseNamespacePath(const std::string& path) const {
    std::vector<std::string> components;
    std::stringstream ss(path);
    std::string component;
    
    while (std::getline(ss, component, '.')) {
        if (!component.empty()) {
            components.push_back(component);
        }
    }
    
    return components;
}

std::shared_ptr<NamespaceItem> NamespaceManager::getNamespaceItem(const std::string& namespacePath, const std::string& itemName, NamespaceItemType itemType) const {
    auto namespace_ = findNamespace(namespacePath);
    if (!namespace_) {
        return nullptr;
    }
    
    for (const auto& item : namespace_->items) {
        if (item->name == itemName && item->type == itemType) {
            return item;
        }
    }
    
    return nullptr;
}

std::string NamespaceManager::createDefaultNamespace(const std::string& sourceFile) {
    if (!defaultNamespaceEnabled_) {
        return "";
    }
    
    std::string defaultName = generateDefaultNamespaceName(sourceFile);
    
    // 如果默认命名空间已存在，返回现有名称
    if (namespaceExists(defaultName)) {
        return defaultName;
    }
    
    // 创建新的默认命名空间
    if (createNamespace(defaultName, sourceFile, true)) {
        return defaultName;
    }
    
    return "";
}

void NamespaceManager::setDefaultNamespaceEnabled(bool enabled) {
    defaultNamespaceEnabled_ = enabled;
}

bool NamespaceManager::isDefaultNamespaceEnabled() const {
    return defaultNamespaceEnabled_;
}

void NamespaceManager::clearNamespace(const std::string& name) {
    namespaces_.erase(name);
}

void NamespaceManager::clearAllNamespaces() {
    namespaces_.clear();
}

std::string NamespaceManager::getStatistics() const {
    std::stringstream ss;
    ss << "命名空间统计信息:\n";
    ss << "总命名空间数量: " << namespaces_.size() << "\n";
    ss << "默认命名空间启用: " << (defaultNamespaceEnabled_ ? "是" : "否") << "\n\n";
    
    for (const auto& pair : namespaces_) {
        const auto& namespace_ = pair.second;
        ss << "命名空间: " << namespace_->name << "\n";
        ss << "  源文件: " << namespace_->sourceFile << "\n";
        ss << "  是否为默认: " << (namespace_->isDefault ? "是" : "否") << "\n";
        ss << "  项数量: " << namespace_->items.size() << "\n";
        ss << "  子命名空间数量: " << namespace_->subNamespaces.size() << "\n";
        ss << "  嵌套命名空间数量: " << namespace_->nestedNamespaces.size() << "\n\n";
    }
    
    return ss.str();
}

bool NamespaceManager::validateNamespaceName(const std::string& name) const {
    if (name.empty()) {
        return false;
    }
    
    // 检查是否包含非法字符
    for (char c : name) {
        if (!std::isalnum(c) && c != '_' && c != '-') {
            return false;
        }
    }
    
    return true;
}

std::shared_ptr<Namespace> NamespaceManager::findNamespace(const std::string& path) const {
    auto components = parseNamespacePath(path);
    if (components.empty()) {
        return nullptr;
    }
    
    auto it = namespaces_.find(components[0]);
    if (it == namespaces_.end()) {
        return nullptr;
    }
    
    auto current = it->second;
    
    // 遍历路径组件
    for (size_t i = 1; i < components.size(); ++i) {
        auto nestedIt = current->nestedNamespaces.find(components[i]);
        if (nestedIt == current->nestedNamespaces.end()) {
            return nullptr;
        }
        current = nestedIt->second;
    }
    
    return current;
}

bool NamespaceManager::createNestedNamespacePath(std::shared_ptr<Namespace> parent, const std::vector<std::string>& pathComponents, const std::string& sourceFile) {
    auto current = parent;
    
    for (const auto& component : pathComponents) {
        auto nestedIt = current->nestedNamespaces.find(component);
        if (nestedIt == current->nestedNamespaces.end()) {
            // 创建新的嵌套命名空间
            auto nestedNamespace = std::make_shared<Namespace>();
            nestedNamespace->name = component;
            nestedNamespace->sourceFile = sourceFile;
            nestedNamespace->isDefault = false;
            
            current->nestedNamespaces[component] = nestedNamespace;
            current->subNamespaces.push_back(component);
            current = nestedNamespace;
        } else {
            current = nestedIt->second;
        }
    }
    
    return true;
}

void NamespaceManager::checkNamespaceConflicts(const std::shared_ptr<Namespace>& namespace1, 
                                              const std::shared_ptr<Namespace>& namespace2,
                                              std::vector<NamespaceConflict>& conflicts) const {
    // 不同命名空间中的相同名称项不应该被视为冲突
    // 真正的冲突应该是同一命名空间中的重复项
    // 这里我们只检查同一命名空间内部的冲突
    
    // 检查namespace1内部的冲突
    std::unordered_map<std::string, std::vector<std::shared_ptr<NamespaceItem>>> nameTypeMap;
    for (const auto& item : namespace1->items) {
        std::string key = item->name + "_" + std::to_string(static_cast<int>(item->type));
        nameTypeMap[key].push_back(item);
    }
    
    for (const auto& pair : nameTypeMap) {
        if (pair.second.size() > 1) {
            // 发现同一命名空间中的重复项
            for (size_t i = 0; i < pair.second.size(); ++i) {
                for (size_t j = i + 1; j < pair.second.size(); ++j) {
                    NamespaceConflict conflict;
                    conflict.itemName = pair.second[i]->name;
                    conflict.itemType = pair.second[i]->type;
                    conflict.namespace1 = namespace1->name;
                    conflict.namespace2 = namespace1->name; // 同一命名空间
                    conflict.sourceFile1 = pair.second[i]->sourceFile;
                    conflict.sourceFile2 = pair.second[j]->sourceFile;
                    conflict.line1 = pair.second[i]->lineNumber;
                    conflict.line2 = pair.second[j]->lineNumber;
                    conflicts.push_back(conflict);
                }
            }
        }
    }
    
    // 检查namespace2内部的冲突
    nameTypeMap.clear();
    for (const auto& item : namespace2->items) {
        std::string key = item->name + "_" + std::to_string(static_cast<int>(item->type));
        nameTypeMap[key].push_back(item);
    }
    
    for (const auto& pair : nameTypeMap) {
        if (pair.second.size() > 1) {
            // 发现同一命名空间中的重复项
            for (size_t i = 0; i < pair.second.size(); ++i) {
                for (size_t j = i + 1; j < pair.second.size(); ++j) {
                    NamespaceConflict conflict;
                    conflict.itemName = pair.second[i]->name;
                    conflict.itemType = pair.second[i]->type;
                    conflict.namespace1 = namespace2->name;
                    conflict.namespace2 = namespace2->name; // 同一命名空间
                    conflict.sourceFile1 = pair.second[i]->sourceFile;
                    conflict.sourceFile2 = pair.second[j]->sourceFile;
                    conflict.line1 = pair.second[i]->lineNumber;
                    conflict.line2 = pair.second[j]->lineNumber;
                    conflicts.push_back(conflict);
                }
            }
        }
    }
}

std::string NamespaceManager::generateDefaultNamespaceName(const std::string& filePath) const {
    std::filesystem::path path(filePath);
    std::string fileName = path.stem().string();
    
    // 转换为有效的命名空间名称
    std::string result;
    for (char c : fileName) {
        if (std::isalnum(c) || c == '_' || c == '-') {
            result += c;
        } else {
            result += '_';
        }
    }
    
    // 确保不以数字开头
    if (!result.empty() && std::isdigit(result[0])) {
        result = "ns_" + result;
    }
    
    return result.empty() ? "default" : result;
}

bool NamespaceManager::areItemTypesCompatible(NamespaceItemType type1, NamespaceItemType type2) const {
    // 相同类型总是兼容的
    if (type1 == type2) {
        return true;
    }
    
    // 检查类型兼容性规则
    switch (type1) {
        case NamespaceItemType::CUSTOM_ELEMENT:
        case NamespaceItemType::TEMPLATE_ELEMENT:
            return type2 == NamespaceItemType::CUSTOM_ELEMENT || type2 == NamespaceItemType::TEMPLATE_ELEMENT;
            
        case NamespaceItemType::CUSTOM_STYLE:
        case NamespaceItemType::TEMPLATE_STYLE:
            return type2 == NamespaceItemType::CUSTOM_STYLE || type2 == NamespaceItemType::TEMPLATE_STYLE;
            
        case NamespaceItemType::CUSTOM_VAR:
        case NamespaceItemType::TEMPLATE_VAR:
            return type2 == NamespaceItemType::CUSTOM_VAR || type2 == NamespaceItemType::TEMPLATE_VAR;
            
        case NamespaceItemType::ORIGIN_HTML:
        case NamespaceItemType::ORIGIN_STYLE:
        case NamespaceItemType::ORIGIN_JAVASCRIPT:
        case NamespaceItemType::ORIGIN_CUSTOM:
            return type2 == NamespaceItemType::ORIGIN_HTML || type2 == NamespaceItemType::ORIGIN_STYLE || 
                   type2 == NamespaceItemType::ORIGIN_JAVASCRIPT || type2 == NamespaceItemType::ORIGIN_CUSTOM;
            
        default:
            return false;
    }
}

} // namespace CHTL