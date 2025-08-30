#include "NamespaceManager.h"
#include "../../../Util/StringUtils.h"
#include <iostream>
#include <algorithm>

namespace CHTL {

NamespaceManager::NamespaceManager() {
    // 创建全局命名空间
    globalNamespace = std::make_unique<NamespaceInfo>("");
    currentNamespace = globalNamespace.get();
    namespaceMap[""] = globalNamespace.get();
}

NamespaceInfo* NamespaceManager::createNamespace(const String& name) {
    if (name.empty()) {
        return globalNamespace.get();
    }
    
    // 检查是否已存在
    auto existing = findNamespace(name);
    if (existing) {
        // 如果存在同名命名空间，进行合并
        autoMergeNamespaces(name);
        return existing;
    }
    
    // 解析嵌套路径
    auto pathComponents = Namespace::splitPath(name);
    
    NamespaceInfo* current = globalNamespace.get();
    String currentPath;
    
    for (const auto& component : pathComponents) {
        if (!currentPath.empty()) {
            currentPath += ".";
        }
        currentPath += component;
        
        NamespaceInfo* child = current->findChild(component);
        if (!child) {
            child = current->addChild(component);
            namespaceMap[currentPath] = child;
        }
        current = child;
    }
    
    return current;
}

NamespaceInfo* NamespaceManager::createNestedNamespace(const String& fullName) {
    return createNamespace(fullName);
}

NamespaceInfo* NamespaceManager::findNamespace(const String& name) const {
    if (name.empty()) {
        return globalNamespace.get();
    }
    
    auto it = namespaceMap.find(name);
    if (it != namespaceMap.end()) {
        return it->second;
    }
    
    // 递归查找
    return findNamespaceRecursive(globalNamespace.get(), name);
}

NamespaceInfo* NamespaceManager::findNamespaceRecursive(NamespaceInfo* root, const String& name) const {
    if (root->name == name || root->getFullName() == name) {
        return root;
    }
    
    for (const auto& child : root->children) {
        auto result = findNamespaceRecursive(child.get(), name);
        if (result) {
            return result;
        }
    }
    
    return nullptr;
}

bool NamespaceManager::enterNamespace(const String& name) {
    auto ns = findNamespace(name);
    if (!ns) {
        ns = createNamespace(name);
    }
    
    if (ns) {
        currentNamespace = ns;
        return true;
    }
    
    return false;
}

bool NamespaceManager::exitNamespace() {
    if (currentNamespace && currentNamespace->parent) {
        currentNamespace = currentNamespace->parent;
        return true;
    }
    
    // 回到全局命名空间
    currentNamespace = globalNamespace.get();
    return false;
}

void NamespaceManager::autoMergeNamespaces(const String& name) {
    // 查找所有同名命名空间并合并
    std::vector<NamespaceInfo*> sameNameNamespaces;
    
    // 收集同名命名空间
    std::function<void(NamespaceInfo*)> collectSameName = [&](NamespaceInfo* ns) {
        if (ns->name == name) {
            sameNameNamespaces.push_back(ns);
        }
        for (const auto& child : ns->children) {
            collectSameName(child.get());
        }
    };
    
    collectSameName(globalNamespace.get());
    
    // 合并命名空间
    if (sameNameNamespaces.size() > 1) {
        NamespaceInfo* target = sameNameNamespaces[0];
        for (size_t i = 1; i < sameNameNamespaces.size(); ++i) {
            mergeNamespaces(target, sameNameNamespaces[i]);
        }
    }
}

void NamespaceManager::mergeNamespaces(NamespaceInfo* target, NamespaceInfo* source) {
    if (!target || !source || target == source) {
        return;
    }
    
    // 合并模板
    for (const auto& template_ : source->templates) {
        if (target->templates.find(template_.first) == target->templates.end()) {
            target->templates[template_.first] = template_.second;
        }
    }
    
    // 合并自定义
    for (const auto& custom : source->customs) {
        if (target->customs.find(custom.first) == target->customs.end()) {
            target->customs[custom.first] = custom.second;
        }
    }
    
    // 合并原始嵌入
    for (const auto& origin : source->origins) {
        if (target->origins.find(origin.first) == target->origins.end()) {
            target->origins[origin.first] = origin.second;
        }
    }
    
    // 合并导入
    for (const auto& import : source->imports) {
        if (std::find(target->imports.begin(), target->imports.end(), import) == target->imports.end()) {
            target->imports.push_back(import);
        }
    }
}

bool NamespaceManager::hasConflict(const String& name, const String& itemName, const String& itemType) const {
    auto ns = findNamespace(name);
    if (!ns) {
        return false;
    }
    
    if (itemType == "template") {
        return ns->templates.find(itemName) != ns->templates.end();
    } else if (itemType == "custom") {
        return ns->customs.find(itemName) != ns->customs.end();
    } else if (itemType == "origin") {
        return ns->origins.find(itemName) != ns->origins.end();
    } else if (itemType == "config") {
        return ns->configs.find(itemName) != ns->configs.end();
    }
    
    return false;
}

bool NamespaceManager::registerTemplate(const String& namespaceName, const String& templateName, BaseNode* templateNode) {
    auto ns = findNamespace(namespaceName);
    if (!ns) {
        ns = createNamespace(namespaceName);
    }
    
    if (ns && templateNode) {
        ns->templates[templateName] = templateNode;
        return true;
    }
    
    return false;
}

bool NamespaceManager::registerCustom(const String& namespaceName, const String& customName, BaseNode* customNode) {
    auto ns = findNamespace(namespaceName);
    if (!ns) {
        ns = createNamespace(namespaceName);
    }
    
    if (ns && customNode) {
        ns->customs[customName] = customNode;
        return true;
    }
    
    return false;
}

bool NamespaceManager::registerOrigin(const String& namespaceName, const String& originName, BaseNode* originNode) {
    auto ns = findNamespace(namespaceName);
    if (!ns) {
        ns = createNamespace(namespaceName);
    }
    
    if (ns && originNode) {
        ns->origins[originName] = originNode;
        return true;
    }
    
    return false;
}

BaseNode* NamespaceManager::findTemplate(const String& name) const {
    // 首先在当前命名空间查找
    if (currentNamespace) {
        auto it = currentNamespace->templates.find(name);
        if (it != currentNamespace->templates.end()) {
            return it->second;
        }
    }
    
    // 在全局命名空间查找
    auto it = globalNamespace->templates.find(name);
    if (it != globalNamespace->templates.end()) {
        return it->second;
    }
    
    return nullptr;
}

BaseNode* NamespaceManager::findCustom(const String& name) const {
    // 首先在当前命名空间查找
    if (currentNamespace) {
        auto it = currentNamespace->customs.find(name);
        if (it != currentNamespace->customs.end()) {
            return it->second;
        }
    }
    
    // 在全局命名空间查找
    auto it = globalNamespace->customs.find(name);
    if (it != globalNamespace->customs.end()) {
        return it->second;
    }
    
    return nullptr;
}

BaseNode* NamespaceManager::findTemplateInNamespace(const String& namespaceName, const String& templateName) const {
    auto ns = findNamespace(namespaceName);
    if (ns) {
        auto it = ns->templates.find(templateName);
        if (it != ns->templates.end()) {
            return it->second;
        }
    }
    
    return nullptr;
}

BaseNode* NamespaceManager::findCustomInNamespace(const String& namespaceName, const String& customName) const {
    auto ns = findNamespace(namespaceName);
    if (ns) {
        auto it = ns->customs.find(customName);
        if (it != ns->customs.end()) {
            return it->second;
        }
    }
    
    return nullptr;
}

NamespaceInfo* NamespaceManager::createDefaultNamespace(const String& fileName) {
    if (!enableDefaultNamespace) {
        return globalNamespace.get();
    }
    
    // 从文件名提取命名空间名称
    String namespaceName = fileName;
    
    // 移除路径
    size_t lastSlash = namespaceName.find_last_of("/\\");
    if (lastSlash != String::npos) {
        namespaceName = namespaceName.substr(lastSlash + 1);
    }
    
    // 移除扩展名
    size_t lastDot = namespaceName.find_last_of(".");
    if (lastDot != String::npos) {
        namespaceName = namespaceName.substr(0, lastDot);
    }
    
    auto ns = createNamespace(namespaceName);
    if (ns) {
        ns->isDefault = true;
    }
    
    return ns;
}

StringVector NamespaceManager::getAllNamespaces() const {
    StringVector result;
    collectNamespacesRecursive(globalNamespace.get(), result);
    return result;
}

void NamespaceManager::collectNamespacesRecursive(NamespaceInfo* ns, StringVector& result) const {
    if (ns && !ns->name.empty()) {
        result.push_back(ns->getFullName());
    }
    
    if (ns) {
        for (const auto& child : ns->children) {
            collectNamespacesRecursive(child.get(), result);
        }
    }
}

void NamespaceManager::printNamespaceTree() const {
    std::cout << "=== Namespace Tree ===" << std::endl;
    printNamespaceRecursive(globalNamespace.get(), 0);
}

void NamespaceManager::printNamespaceRecursive(NamespaceInfo* ns, int depth) const {
    if (!ns) return;
    
    String indent(depth * 2, ' ');
    
    if (depth == 0) {
        std::cout << indent << "Global Namespace" << std::endl;
    } else {
        std::cout << indent << "└─ " << ns->name;
        if (ns == currentNamespace) {
            std::cout << " (current)";
        }
        if (ns->isDefault) {
            std::cout << " (default)";
        }
        std::cout << std::endl;
        
        // 显示内容
        if (!ns->templates.empty()) {
            std::cout << indent << "   Templates: " << ns->templates.size() << std::endl;
        }
        if (!ns->customs.empty()) {
            std::cout << indent << "   Customs: " << ns->customs.size() << std::endl;
        }
        if (!ns->origins.empty()) {
            std::cout << indent << "   Origins: " << ns->origins.size() << std::endl;
        }
    }
    
    for (const auto& child : ns->children) {
        printNamespaceRecursive(child.get(), depth + 1);
    }
}

// NamespaceParser实现
StringVector NamespaceParser::parseNamespacePath(const String& fullPath) {
    return Namespace::splitPath(fullPath);
}

bool NamespaceParser::isValidNamespaceName(const String& name) {
    return Namespace::isValidName(name);
}

String NamespaceParser::buildFullPath(const StringVector& path) {
    return Namespace::joinPath(path);
}

std::pair<String, String> NamespaceParser::parseFromClause(const String& clause) {
    // 解析 "from namespace.subnamespace" 语法
    String trimmed = Util::StringUtils::trim(clause);
    
    if (Util::StringUtils::starts_with(trimmed, "from ")) {
        String namespacePart = trimmed.substr(5);
        namespacePart = Util::StringUtils::trim(namespacePart);
        
        size_t dotPos = namespacePart.find_last_of('.');
        if (dotPos != String::npos) {
            String namespaceName = namespacePart.substr(0, dotPos);
            String itemName = namespacePart.substr(dotPos + 1);
            return {namespaceName, itemName};
        } else {
            return {"", namespacePart};
        }
    }
    
    return {"", clause};
}

// Namespace工具函数实现
namespace Namespace {

bool isValidName(const String& name) {
    if (name.empty()) {
        return false;
    }
    
    // 检查是否为保留名称
    if (isReservedName(name)) {
        return false;
    }
    
    // 检查字符有效性
    for (char c : name) {
        if (!std::isalnum(c) && c != '_' && c != '-') {
            return false;
        }
    }
    
    // 不能以数字开头
    return std::isalpha(name[0]) || name[0] == '_';
}

bool isReservedName(const String& name) {
    StringVector reserved = {"global", "default", "system", "chtl", "temp", "tmp"};
    return std::find(reserved.begin(), reserved.end(), name) != reserved.end();
}

StringVector splitPath(const String& path) {
    return Util::StringUtils::split(path, ".");
}

String joinPath(const StringVector& components) {
    return Util::StringUtils::join(components, ".");
}

String normalizePath(const String& path) {
    auto components = splitPath(path);
    StringVector normalized;
    
    for (const auto& component : components) {
        String trimmed = Util::StringUtils::trim(component);
        if (!trimmed.empty() && isValidName(trimmed)) {
            normalized.push_back(trimmed);
        }
    }
    
    return joinPath(normalized);
}

} // namespace Namespace

} // namespace CHTL