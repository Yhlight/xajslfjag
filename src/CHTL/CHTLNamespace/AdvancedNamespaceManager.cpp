#include "AdvancedNamespaceManager.h"
#include <algorithm>
#include <sstream>
#include <regex>
#include <iostream>
#include <chrono>
#include <stack>

namespace CHTL {

// ========== AdvancedNamespaceManager 实现 ==========

AdvancedNamespaceManager::AdvancedNamespaceManager() 
    : enableCircularDependencyCheck(true), enableAccessControl(true), 
      enableVersionCheck(true), maxNamespaceDepth(10) {
    initializeDefaultStrategies();
    initializeBuiltinNamespaces();
}

bool AdvancedNamespaceManager::createNamespace(const String& name, NamespaceType type, NamespaceVisibility visibility) {
    if (!validateNamespaceName(name) || hasNamespace(name)) {
        return false;
    }
    
    // 创建基础命名空间
    if (!NamespaceManager::createNamespace(name)) {
        return false;
    }
    
    // 创建高级信息
    createAdvancedNamespaceInfo(name, type, visibility);
    
    triggerNamespaceCreated(name);
    return true;
}

bool AdvancedNamespaceManager::createNestedNamespace(const String& parentName, const String& childName) {
    if (!hasNamespace(parentName) || hasNamespace(childName)) {
        return false;
    }
    
    // 检查深度限制
    size_t parentDepth = getNamespaceDepth(parentName);
    if (parentDepth >= maxNamespaceDepth) {
        return false;
    }
    
    // 创建子命名空间
    if (!createNamespace(childName, NamespaceType::NESTED)) {
        return false;
    }
    
    // 设置父子关系
    if (!setParentNamespace(childName, parentName)) {
        deleteNamespace(childName);
        return false;
    }
    
    return true;
}

bool AdvancedNamespaceManager::createNamespaceHierarchy(const StringVector& hierarchy) {
    if (hierarchy.empty()) {
        return false;
    }
    
    // 创建根命名空间
    String rootName = hierarchy[0];
    if (!hasNamespace(rootName)) {
        if (!createNamespace(rootName, NamespaceType::ROOT)) {
            return false;
        }
    }
    
    // 创建嵌套层次
    String currentParent = rootName;
    for (size_t i = 1; i < hierarchy.size(); ++i) {
        String childName = hierarchy[i];
        if (!hasNamespace(childName)) {
            if (!createNestedNamespace(currentParent, childName)) {
                return false;
            }
        } else {
            // 如果命名空间已存在，检查父子关系
            if (getParentNamespace(childName) != currentParent) {
                if (!setParentNamespace(childName, currentParent)) {
                    return false;
                }
            }
        }
        currentParent = childName;
    }
    
    return true;
}

bool AdvancedNamespaceManager::setParentNamespace(const String& childName, const String& parentName) {
    auto childInfo = getAdvancedNamespaceInfo(childName);
    auto parentInfo = getAdvancedNamespaceInfo(parentName);
    
    if (!childInfo || !parentInfo) {
        return false;
    }
    
    // 检查循环依赖
    if (wouldCreateCircularDependency(parentName, childName)) {
        return false;
    }
    
    // 移除旧的父子关系
    String oldParent = childInfo->parentNamespace;
    if (!oldParent.empty()) {
        auto oldParentInfo = getAdvancedNamespaceInfo(oldParent);
        if (oldParentInfo) {
            auto& children = oldParentInfo->childNamespaces;
            children.erase(std::remove(children.begin(), children.end(), childName), children.end());
        }
    }
    
    // 设置新的父子关系
    childInfo->parentNamespace = parentName;
    childInfo->depth = parentInfo->depth + 1;
    parentInfo->childNamespaces.push_back(childName);
    
    return true;
}

String AdvancedNamespaceManager::getParentNamespace(const String& namespaceName) const {
    auto info = getAdvancedNamespaceInfo(namespaceName);
    return info ? info->parentNamespace : "";
}

StringVector AdvancedNamespaceManager::getChildNamespaces(const String& namespaceName) const {
    auto info = getAdvancedNamespaceInfo(namespaceName);
    return info ? info->childNamespaces : StringVector{};
}

StringVector AdvancedNamespaceManager::getAllAncestors(const String& namespaceName) const {
    StringVector ancestors;
    String current = getParentNamespace(namespaceName);
    
    while (!current.empty()) {
        ancestors.push_back(current);
        current = getParentNamespace(current);
    }
    
    return ancestors;
}

StringVector AdvancedNamespaceManager::getAllDescendants(const String& namespaceName) const {
    StringVector descendants;
    StringVector children = getChildNamespaces(namespaceName);
    
    for (const String& child : children) {
        descendants.push_back(child);
        
        // 递归获取子孙命名空间
        StringVector grandchildren = getAllDescendants(child);
        descendants.insert(descendants.end(), grandchildren.begin(), grandchildren.end());
    }
    
    return descendants;
}

size_t AdvancedNamespaceManager::getNamespaceDepth(const String& namespaceName) const {
    auto info = getAdvancedNamespaceInfo(namespaceName);
    return info ? info->depth : 0;
}

bool AdvancedNamespaceManager::importNamespace(const String& targetNamespace, const String& sourceNamespace, const String& alias) {
    auto targetInfo = getAdvancedNamespaceInfo(targetNamespace);
    if (!targetInfo || !hasNamespace(sourceNamespace)) {
        return false;
    }
    
    // 检查是否已经导入
    auto& imported = targetInfo->importedNamespaces;
    if (std::find(imported.begin(), imported.end(), sourceNamespace) != imported.end()) {
        return true; // 已经导入
    }
    
    // 添加导入
    imported.push_back(sourceNamespace);
    
    // 添加别名
    if (!alias.empty()) {
        targetInfo->aliases[alias] = sourceNamespace;
    }
    
    return true;
}

bool AdvancedNamespaceManager::addNamespaceDependency(const String& namespaceName, const String& dependency) {
    auto info = getAdvancedNamespaceInfo(namespaceName);
    if (!info || !hasNamespace(dependency)) {
        return false;
    }
    
    // 检查循环依赖
    if (enableCircularDependencyCheck && wouldCreateCircularDependency(namespaceName, dependency)) {
        return false;
    }
    
    // 添加依赖
    auto& deps = info->dependencies;
    if (std::find(deps.begin(), deps.end(), dependency) == deps.end()) {
        deps.push_back(dependency);
    }
    
    return true;
}

StringVector AdvancedNamespaceManager::getNamespaceDependencies(const String& namespaceName) const {
    auto info = getAdvancedNamespaceInfo(namespaceName);
    return info ? info->dependencies : StringVector{};
}

StringVector AdvancedNamespaceManager::getImportedNamespaces(const String& namespaceName) const {
    auto info = getAdvancedNamespaceInfo(namespaceName);
    return info ? info->importedNamespaces : StringVector{};
}

bool AdvancedNamespaceManager::addNamespaceAlias(const String& namespaceName, const String& itemName, const String& alias) {
    auto info = getAdvancedNamespaceInfo(namespaceName);
    if (!info) {
        return false;
    }
    
    info->aliases[alias] = itemName;
    return true;
}

String AdvancedNamespaceManager::resolveAlias(const String& namespaceName, const String& alias) const {
    auto info = getAdvancedNamespaceInfo(namespaceName);
    if (!info) {
        return alias;
    }
    
    auto it = info->aliases.find(alias);
    return it != info->aliases.end() ? it->second : alias;
}

StringVector AdvancedNamespaceManager::getAliases(const String& namespaceName, const String& itemName) const {
    auto info = getAdvancedNamespaceInfo(namespaceName);
    if (!info) {
        return {};
    }
    
    StringVector aliases;
    for (const auto& [alias, target] : info->aliases) {
        if (target == itemName) {
            aliases.push_back(alias);
        }
    }
    
    return aliases;
}

bool AdvancedNamespaceManager::mergeNamespaces(const String& targetNamespace, const String& sourceNamespace) {
    if (!canMergeNamespaces(targetNamespace, sourceNamespace)) {
        return false;
    }
    
    auto targetInfo = getAdvancedNamespaceInfo(targetNamespace);
    auto sourceInfo = getAdvancedNamespaceInfo(sourceNamespace);
    
    if (!targetInfo || !sourceInfo) {
        return false;
    }
    
    // 合并内容
    if (!mergeNamespaceContents(targetNamespace, sourceNamespace)) {
        return false;
    }
    
    // 合并依赖
    for (const String& dep : sourceInfo->dependencies) {
        addNamespaceDependency(targetNamespace, dep);
    }
    
    // 合并导入
    for (const String& imported : sourceInfo->importedNamespaces) {
        importNamespace(targetNamespace, imported);
    }
    
    // 合并别名
    for (const auto& [alias, target] : sourceInfo->aliases) {
        targetInfo->aliases[alias] = target;
    }
    
    // 更新子命名空间的父引用
    for (const String& child : sourceInfo->childNamespaces) {
        setParentNamespace(child, targetNamespace);
    }
    
    // 删除源命名空间
    deleteNamespace(sourceNamespace);
    
    triggerNamespaceMerged(targetNamespace, sourceNamespace);
    return true;
}

bool AdvancedNamespaceManager::canMergeNamespaces(const String& namespace1, const String& namespace2) const {
    auto conflicts = getMergeConflicts(namespace1, namespace2);
    return conflicts.empty();
}

StringVector AdvancedNamespaceManager::getMergeConflicts(const String& namespace1, const String& namespace2) const {
    StringVector conflicts;
    
    auto info1 = getNamespaceInfo(namespace1);
    auto info2 = getNamespaceInfo(namespace2);
    
    if (!info1 || !info2) {
        conflicts.push_back("命名空间不存在");
        return conflicts;
    }
    
    // 检查模板冲突
    for (const auto& [name, node] : info1->templates) {
        if (info2->templates.find(name) != info2->templates.end()) {
            conflicts.push_back("模板冲突: " + name);
        }
    }
    
    // 检查自定义冲突
    for (const auto& [name, node] : info1->customs) {
        if (info2->customs.find(name) != info2->customs.end()) {
            conflicts.push_back("自定义冲突: " + name);
        }
    }
    
    // 检查原始嵌入冲突
    for (const auto& [name, node] : info1->origins) {
        if (info2->origins.find(name) != info2->origins.end()) {
            conflicts.push_back("原始嵌入冲突: " + name);
        }
    }
    
    return conflicts;
}

std::vector<ConflictInfo> AdvancedNamespaceManager::detectAllConflicts() const {
    std::vector<ConflictInfo> allConflicts;
    
    // 检测各种类型的冲突
    auto nameCollisions = detectNameCollisions();
    auto typeMismatches = detectTypeMismatches();
    auto scopeViolations = detectScopeViolations();
    auto circularDeps = detectCircularDependenciesInternal();
    auto accessViolations = detectAccessViolations();
    
    allConflicts.insert(allConflicts.end(), nameCollisions.begin(), nameCollisions.end());
    allConflicts.insert(allConflicts.end(), typeMismatches.begin(), typeMismatches.end());
    allConflicts.insert(allConflicts.end(), scopeViolations.begin(), scopeViolations.end());
    allConflicts.insert(allConflicts.end(), circularDeps.begin(), circularDeps.end());
    allConflicts.insert(allConflicts.end(), accessViolations.begin(), accessViolations.end());
    
    return allConflicts;
}

std::vector<ConflictInfo> AdvancedNamespaceManager::detectNamespaceConflicts(const String& namespaceName) const {
    std::vector<ConflictInfo> conflicts;
    
    auto info = getAdvancedNamespaceInfo(namespaceName);
    if (!info) {
        return conflicts;
    }
    
    // 检查与其他命名空间的冲突
    for (const auto& [otherName, otherInfo] : advancedNamespaces) {
        if (otherName == namespaceName) {
            continue;
        }
        
        // 检查名称冲突
        auto namespaceInfo = getNamespaceInfo(namespaceName);
        auto otherNamespaceInfo = getNamespaceInfo(otherName);
        
        if (namespaceInfo && otherNamespaceInfo) {
            // 检查模板名称冲突
            for (const auto& [templateName, templateNode] : namespaceInfo->templates) {
                if (otherNamespaceInfo->templates.find(templateName) != otherNamespaceInfo->templates.end()) {
                    ConflictInfo conflict;
                    conflict.type = ConflictType::NAME_COLLISION;
                    conflict.namespaceName1 = namespaceName;
                    conflict.namespaceName2 = otherName;
                    conflict.conflictingItem = "模板: " + templateName;
                    conflict.description = "模板名称冲突";
                    conflict.recommendedStrategy = ConflictResolutionStrategy::WARN;
                    conflicts.push_back(conflict);
                }
            }
        }
    }
    
    return conflicts;
}

bool AdvancedNamespaceManager::hasConflicts(const String& namespaceName) const {
    auto conflicts = detectNamespaceConflicts(namespaceName);
    return !conflicts.empty();
}

bool AdvancedNamespaceManager::resolveConflict(const ConflictInfo& conflict, ConflictResolutionStrategy strategy) {
    switch (strategy) {
        case ConflictResolutionStrategy::ERROR:
            // 记录错误但不解决
            triggerConflictDetected(conflict);
            return false;
            
        case ConflictResolutionStrategy::WARN:
            // 记录警告
            std::cout << "警告: " << conflict.description << std::endl;
            return true;
            
        case ConflictResolutionStrategy::OVERRIDE:
            // 覆盖策略 - 删除冲突的第二个项目
            if (conflict.type == ConflictType::NAME_COLLISION) {
                // 实现覆盖逻辑
                return true;
            }
            break;
            
        case ConflictResolutionStrategy::MERGE:
            // 合并策略
            if (conflict.type == ConflictType::NAME_COLLISION) {
                return mergeNamespaces(conflict.namespaceName1, conflict.namespaceName2);
            }
            break;
            
        case ConflictResolutionStrategy::IGNORE:
            // 忽略冲突
            return true;
            
        case ConflictResolutionStrategy::PROMPT:
            // 提示用户选择 (在控制台环境中简化为警告)
            std::cout << "冲突需要用户决策: " << conflict.description << std::endl;
            return true;
    }
    
    return false;
}

void AdvancedNamespaceManager::setConflictResolutionStrategy(ConflictType type, ConflictResolutionStrategy strategy) {
    conflictStrategies[type] = strategy;
}

bool AdvancedNamespaceManager::hasCircularDependencies() const {
    StringUnorderedSet visited;
    StringUnorderedSet recursionStack;
    
    for (const auto& [namespaceName, info] : advancedNamespaces) {
        if (visited.find(namespaceName) == visited.end()) {
            if (hasCircularDependencyDFS(namespaceName, visited, recursionStack)) {
                return true;
            }
        }
    }
    
    return false;
}

StringVector AdvancedNamespaceManager::findCircularDependencies() const {
    StringVector cycles;
    StringUnorderedSet visited;
    StringUnorderedSet recursionStack;
    
    for (const auto& [namespaceName, info] : advancedNamespaces) {
        if (visited.find(namespaceName) == visited.end()) {
            if (hasCircularDependencyDFS(namespaceName, visited, recursionStack)) {
                // 找到循环，构建循环路径
                std::ostringstream cycle;
                bool foundStart = false;
                for (const String& name : recursionStack) {
                    if (name == namespaceName) {
                        foundStart = true;
                    }
                    if (foundStart) {
                        if (cycle.tellp() > 0) cycle << " -> ";
                        cycle << name;
                    }
                }
                cycle << " -> " << namespaceName;
                cycles.push_back(cycle.str());
            }
        }
    }
    
    return cycles;
}

bool AdvancedNamespaceManager::wouldCreateCircularDependency(const String& namespaceName, const String& dependency) const {
    if (!enableCircularDependencyCheck) {
        return false;
    }
    
    // 检查依赖是否已经依赖于namespaceName
    StringUnorderedSet visited;
    StringUnorderedSet recursionStack;
    
    return hasCircularDependencyDFS(dependency, visited, recursionStack);
}

bool AdvancedNamespaceManager::setNamespaceVisibility(const String& namespaceName, NamespaceVisibility visibility) {
    auto info = getAdvancedNamespaceInfo(namespaceName);
    if (!info) {
        return false;
    }
    
    info->visibility = visibility;
    return true;
}

NamespaceVisibility AdvancedNamespaceManager::getNamespaceVisibility(const String& namespaceName) const {
    auto info = getAdvancedNamespaceInfo(namespaceName);
    return info ? info->visibility : NamespaceVisibility::PUBLIC;
}

bool AdvancedNamespaceManager::canAccess(const String& accessorNamespace, const String& targetNamespace) const {
    if (!enableAccessControl) {
        return true;
    }
    
    return isAccessibleFromNamespace(accessorNamespace, targetNamespace);
}

bool AdvancedNamespaceManager::canAccess(const String& accessorNamespace, const String& targetNamespace, const String& itemName) const {
    if (!canAccess(accessorNamespace, targetNamespace)) {
        return false;
    }
    
    // 可以添加更细粒度的项目级访问控制
    return true;
}

String AdvancedNamespaceManager::resolveFullyQualifiedName(const String& namespaceName, const String& itemName) const {
    if (namespaceName.empty()) {
        return itemName;
    }
    
    return namespaceName + "::" + itemName;
}

String AdvancedNamespaceManager::findItemInNamespace(const String& itemName, const String& startNamespace) const {
    String searchNamespace = startNamespace.empty() ? "global" : startNamespace;
    
    // 在当前命名空间中查找
    auto info = getNamespaceInfo(searchNamespace);
    if (info) {
        // 检查模板
        if (info->templates.find(itemName) != info->templates.end()) {
            return searchNamespace;
        }
        
        // 检查自定义
        if (info->customs.find(itemName) != info->customs.end()) {
            return searchNamespace;
        }
        
        // 检查原始嵌入
        if (info->origins.find(itemName) != info->origins.end()) {
            return searchNamespace;
        }
    }
    
    // 在导入的命名空间中查找
    auto advancedInfo = getAdvancedNamespaceInfo(searchNamespace);
    if (advancedInfo) {
        for (const String& imported : advancedInfo->importedNamespaces) {
            String result = findItemInNamespace(itemName, imported);
            if (!result.empty()) {
                return result;
            }
        }
    }
    
    return "";
}

StringVector AdvancedNamespaceManager::findAllMatches(const String& itemName, const String& startNamespace) const {
    StringVector matches;
    
    for (const auto& [namespaceName, info] : namespaces) {
        if (info) {
            bool found = false;
            
            // 检查各种类型的项目
            if (info->templates.find(itemName) != info->templates.end() ||
                info->customs.find(itemName) != info->customs.end() ||
                info->origins.find(itemName) != info->origins.end()) {
                found = true;
            }
            
            if (found) {
                matches.push_back(namespaceName);
            }
        }
    }
    
    return matches;
}

String AdvancedNamespaceManager::resolveNamespaceReference(const String& reference, const String& currentNamespace) const {
    // 处理绝对引用
    if (reference.find("::") == 0) {
        return reference.substr(2);
    }
    
    // 处理相对引用
    if (reference.find(".") != String::npos) {
        StringVector parts = NamespaceUtils::splitNamespacePath(reference);
        String resolved = currentNamespace;
        
        for (const String& part : parts) {
            if (part == "..") {
                resolved = getParentNamespace(resolved);
            } else if (part != ".") {
                resolved = resolved.empty() ? part : resolved + "::" + part;
            }
        }
        
        return resolved;
    }
    
    // 直接命名空间名称
    if (hasNamespace(reference)) {
        return reference;
    }
    
    // 在当前命名空间的别名中查找
    String aliasResolved = resolveAlias(currentNamespace, reference);
    if (aliasResolved != reference && hasNamespace(aliasResolved)) {
        return aliasResolved;
    }
    
    return reference;
}

bool AdvancedNamespaceManager::validateNamespaceName(const String& name) const {
    if (name.empty()) {
        return false;
    }
    
    // 检查字符有效性
    std::regex nameRegex(R"(^[a-zA-Z_][a-zA-Z0-9_]*$)");
    if (!std::regex_match(name, nameRegex)) {
        return false;
    }
    
    // 检查保留字
    static const StringVector reservedNames = {
        "global", "root", "system", "internal", "temp", "anonymous"
    };
    
    return std::find(reservedNames.begin(), reservedNames.end(), name) == reservedNames.end();
}

bool AdvancedNamespaceManager::validateNamespaceHierarchy() const {
    // 检查循环依赖
    if (hasCircularDependencies()) {
        return false;
    }
    
    // 检查孤立的命名空间
    for (const auto& [namespaceName, info] : advancedNamespaces) {
        if (info->type != NamespaceType::ROOT && info->parentNamespace.empty()) {
            // 非根命名空间应该有父命名空间
            if (namespaceName != "global") {
                return false;
            }
        }
    }
    
    return true;
}

StringVector AdvancedNamespaceManager::getValidationErrors() const {
    StringVector errors;
    
    // 检查循环依赖
    if (hasCircularDependencies()) {
        errors.push_back("检测到循环依赖");
    }
    
    // 检查命名空间名称有效性
    for (const auto& [namespaceName, info] : advancedNamespaces) {
        if (!validateNamespaceName(namespaceName)) {
            errors.push_back("无效的命名空间名称: " + namespaceName);
        }
        
        // 检查深度限制
        if (info->depth > maxNamespaceDepth) {
            errors.push_back("命名空间深度超出限制: " + namespaceName + " (深度: " + std::to_string(info->depth) + ")");
        }
    }
    
    return errors;
}

bool AdvancedNamespaceManager::isValidNamespaceStructure() const {
    StringVector errors = getValidationErrors();
    return errors.empty();
}

void AdvancedNamespaceManager::optimizeNamespaceStructure() {
    removeEmptyNamespaces();
    consolidateSimilarNamespaces();
    reorderNamespaceHierarchy();
}

void AdvancedNamespaceManager::compactNamespaces() {
    // 移除没有内容的命名空间
    StringVector toRemove;
    
    for (const auto& [namespaceName, info] : namespaces) {
        if (info && info->templates.empty() && info->customs.empty() && info->origins.empty()) {
            auto advancedInfo = getAdvancedNamespaceInfo(namespaceName);
            if (advancedInfo && advancedInfo->childNamespaces.empty()) {
                toRemove.push_back(namespaceName);
            }
        }
    }
    
    for (const String& name : toRemove) {
        deleteNamespace(name);
    }
}

void AdvancedNamespaceManager::eliminateUnusedNamespaces() {
    // 标记被使用的命名空间
    StringUnorderedSet used;
    
    // 所有有依赖关系的命名空间都被标记为使用
    for (const auto& [namespaceName, info] : advancedNamespaces) {
        if (!info->dependencies.empty() || !info->importedNamespaces.empty()) {
            used.insert(namespaceName);
        }
        
        for (const String& dep : info->dependencies) {
            used.insert(dep);
        }
        
        for (const String& imported : info->importedNamespaces) {
            used.insert(imported);
        }
    }
    
    // 移除未使用的命名空间
    StringVector toRemove;
    for (const auto& [namespaceName, info] : advancedNamespaces) {
        if (used.find(namespaceName) == used.end() && info->type != NamespaceType::ROOT) {
            toRemove.push_back(namespaceName);
        }
    }
    
    for (const String& name : toRemove) {
        deleteNamespace(name);
    }
}

void AdvancedNamespaceManager::optimizeImports() {
    for (const auto& [namespaceName, info] : advancedNamespaces) {
        // 移除重复的导入
        auto& imports = info->importedNamespaces;
        std::sort(imports.begin(), imports.end());
        imports.erase(std::unique(imports.begin(), imports.end()), imports.end());
        
        // 移除不存在的导入
        imports.erase(
            std::remove_if(imports.begin(), imports.end(),
                          [this](const String& imported) { return !hasNamespace(imported); }),
            imports.end()
        );
    }
}

bool AdvancedNamespaceManager::setNamespaceVersion(const String& namespaceName, const String& version) {
    auto info = getAdvancedNamespaceInfo(namespaceName);
    if (!info) {
        return false;
    }
    
    info->version = version;
    return true;
}

String AdvancedNamespaceManager::getNamespaceVersion(const String& namespaceName) const {
    auto info = getAdvancedNamespaceInfo(namespaceName);
    return info ? info->version : "";
}

bool AdvancedNamespaceManager::isVersionCompatible(const String& namespaceName, const String& requiredVersion) const {
    if (!enableVersionCheck) {
        return true;
    }
    
    String currentVersion = getNamespaceVersion(namespaceName);
    if (currentVersion.empty() || requiredVersion.empty()) {
        return true; // 没有版本要求时认为兼容
    }
    
    return compareVersions(currentVersion, requiredVersion) >= 0;
}

std::vector<ConflictInfo> AdvancedNamespaceManager::checkVersionConflicts() const {
    std::vector<ConflictInfo> conflicts;
    
    // 检查依赖的版本兼容性
    for (const auto& [namespaceName, info] : advancedNamespaces) {
        for (const String& dependency : info->dependencies) {
            // 这里可以实现更复杂的版本兼容性检查
            // 简化实现：假设所有依赖都需要相同或更高版本
        }
    }
    
    return conflicts;
}

bool AdvancedNamespaceManager::sealNamespace(const String& namespaceName) {
    auto info = getAdvancedNamespaceInfo(namespaceName);
    if (!info) {
        return false;
    }
    
    info->isSealed = true;
    return true;
}

bool AdvancedNamespaceManager::unsealNamespace(const String& namespaceName) {
    auto info = getAdvancedNamespaceInfo(namespaceName);
    if (!info) {
        return false;
    }
    
    info->isSealed = false;
    return true;
}

bool AdvancedNamespaceManager::isNamespaceSealed(const String& namespaceName) const {
    auto info = getAdvancedNamespaceInfo(namespaceName);
    return info ? info->isSealed : false;
}

bool AdvancedNamespaceManager::markNamespaceAbstract(const String& namespaceName) {
    auto info = getAdvancedNamespaceInfo(namespaceName);
    if (!info) {
        return false;
    }
    
    info->isAbstract = true;
    return true;
}

bool AdvancedNamespaceManager::isNamespaceAbstract(const String& namespaceName) const {
    auto info = getAdvancedNamespaceInfo(namespaceName);
    return info ? info->isAbstract : false;
}

size_t AdvancedNamespaceManager::getTotalNamespaceCount() const {
    return advancedNamespaces.size();
}

size_t AdvancedNamespaceManager::getMaxNamespaceDepth() const {
    size_t maxDepth = 0;
    for (const auto& [namespaceName, info] : advancedNamespaces) {
        maxDepth = std::max(maxDepth, info->depth);
    }
    return maxDepth;
}

StringVector AdvancedNamespaceManager::getNamespacesByType(NamespaceType type) const {
    StringVector result;
    for (const auto& [namespaceName, info] : advancedNamespaces) {
        if (info->type == type) {
            result.push_back(namespaceName);
        }
    }
    return result;
}

StringVector AdvancedNamespaceManager::getNamespacesByVisibility(NamespaceVisibility visibility) const {
    StringVector result;
    for (const auto& [namespaceName, info] : advancedNamespaces) {
        if (info->visibility == visibility) {
            result.push_back(namespaceName);
        }
    }
    return result;
}

std::unordered_map<String, size_t> AdvancedNamespaceManager::getNamespaceStatistics() const {
    std::unordered_map<String, size_t> stats;
    
    stats["total"] = advancedNamespaces.size();
    stats["root"] = getNamespacesByType(NamespaceType::ROOT).size();
    stats["nested"] = getNamespacesByType(NamespaceType::NESTED).size();
    stats["imported"] = getNamespacesByType(NamespaceType::IMPORTED).size();
    stats["public"] = getNamespacesByVisibility(NamespaceVisibility::PUBLIC).size();
    stats["private"] = getNamespacesByVisibility(NamespaceVisibility::PRIVATE).size();
    stats["maxDepth"] = getMaxNamespaceDepth();
    
    size_t totalDependencies = 0;
    size_t totalImports = 0;
    for (const auto& [namespaceName, info] : advancedNamespaces) {
        totalDependencies += info->dependencies.size();
        totalImports += info->importedNamespaces.size();
    }
    stats["totalDependencies"] = totalDependencies;
    stats["totalImports"] = totalImports;
    
    return stats;
}

StringVector AdvancedNamespaceManager::searchNamespaces(const String& pattern) const {
    StringVector matches;
    std::regex searchRegex(pattern);
    
    for (const auto& [namespaceName, info] : advancedNamespaces) {
        if (std::regex_search(namespaceName, searchRegex)) {
            matches.push_back(namespaceName);
        }
    }
    
    return matches;
}

StringVector AdvancedNamespaceManager::filterNamespaces(std::function<bool(const AdvancedNamespaceInfo&)> predicate) const {
    StringVector result;
    
    for (const auto& [namespaceName, info] : advancedNamespaces) {
        if (predicate(*info)) {
            result.push_back(namespaceName);
        }
    }
    
    return result;
}

StringVector AdvancedNamespaceManager::getNamespacesInScope(const String& currentNamespace) const {
    StringVector inScope;
    
    // 当前命名空间
    if (hasNamespace(currentNamespace)) {
        inScope.push_back(currentNamespace);
    }
    
    // 导入的命名空间
    StringVector imported = getImportedNamespaces(currentNamespace);
    inScope.insert(inScope.end(), imported.begin(), imported.end());
    
    // 父命名空间
    StringVector ancestors = getAllAncestors(currentNamespace);
    inScope.insert(inScope.end(), ancestors.begin(), ancestors.end());
    
    // 公共命名空间
    StringVector publicNamespaces = getNamespacesByVisibility(NamespaceVisibility::PUBLIC);
    inScope.insert(inScope.end(), publicNamespaces.begin(), publicNamespaces.end());
    
    // 去重
    std::sort(inScope.begin(), inScope.end());
    inScope.erase(std::unique(inScope.begin(), inScope.end()), inScope.end());
    
    return inScope;
}

void AdvancedNamespaceManager::dumpNamespaceHierarchy() const {
    std::cout << "=== 命名空间层次结构 ===\n";
    
    // 找到所有根命名空间
    StringVector roots = getNamespacesByType(NamespaceType::ROOT);
    
    for (const String& root : roots) {
        printNamespaceTree(root);
    }
    
    std::cout << "\n总计: " << advancedNamespaces.size() << " 个命名空间\n";
}

void AdvancedNamespaceManager::printNamespaceTree(const String& rootNamespace) const {
    std::function<void(const String&, int)> printNode = [&](const String& namespaceName, int depth) {
        String indent(depth * 2, ' ');
        std::cout << indent << namespaceName;
        
        auto info = getAdvancedNamespaceInfo(namespaceName);
        if (info) {
            std::cout << " [" << static_cast<int>(info->type) << "]";
            if (info->isSealed) std::cout << " (sealed)";
            if (info->isAbstract) std::cout << " (abstract)";
        }
        std::cout << "\n";
        
        // 打印子命名空间
        StringVector children = getChildNamespaces(namespaceName);
        for (const String& child : children) {
            printNode(child, depth + 1);
        }
    };
    
    if (rootNamespace.empty()) {
        // 打印所有根命名空间
        StringVector roots = getNamespacesByType(NamespaceType::ROOT);
        for (const String& root : roots) {
            printNode(root, 0);
        }
    } else {
        printNode(rootNamespace, 0);
    }
}

void AdvancedNamespaceManager::generateNamespaceReport() const {
    std::cout << "\n=== 命名空间管理器报告 ===\n";
    
    auto stats = getNamespaceStatistics();
    std::cout << "命名空间总数: " << stats["total"] << "\n";
    std::cout << "根命名空间: " << stats["root"] << "\n";
    std::cout << "嵌套命名空间: " << stats["nested"] << "\n";
    std::cout << "导入命名空间: " << stats["imported"] << "\n";
    std::cout << "公共命名空间: " << stats["public"] << "\n";
    std::cout << "私有命名空间: " << stats["private"] << "\n";
    std::cout << "最大深度: " << stats["maxDepth"] << "\n";
    std::cout << "总依赖数: " << stats["totalDependencies"] << "\n";
    std::cout << "总导入数: " << stats["totalImports"] << "\n";
    
    // 冲突检测
    auto conflicts = detectAllConflicts();
    std::cout << "\n检测到的冲突: " << conflicts.size() << "\n";
    for (const auto& conflict : conflicts) {
        std::cout << "  - " << conflict.description << "\n";
    }
    
    // 循环依赖
    if (hasCircularDependencies()) {
        std::cout << "\n⚠️  检测到循环依赖:\n";
        auto cycles = findCircularDependencies();
        for (const String& cycle : cycles) {
            std::cout << "  " << cycle << "\n";
        }
    } else {
        std::cout << "\n✅ 无循环依赖\n";
    }
}

void AdvancedNamespaceManager::validateIntegrity() const {
    std::cout << "\n=== 命名空间完整性检查 ===\n";
    
    StringVector errors = getValidationErrors();
    if (errors.empty()) {
        std::cout << "✅ 命名空间结构完整性检查通过\n";
    } else {
        std::cout << "❌ 发现 " << errors.size() << " 个完整性问题:\n";
        for (const String& error : errors) {
            std::cout << "  - " << error << "\n";
        }
    }
    
    // 检查层次结构
    if (validateNamespaceHierarchy()) {
        std::cout << "✅ 命名空间层次结构有效\n";
    } else {
        std::cout << "❌ 命名空间层次结构无效\n";
    }
}

// 事件系统实现
void AdvancedNamespaceManager::onNamespaceCreated(NamespaceEventCallback callback) {
    creationCallbacks.push_back(callback);
}

void AdvancedNamespaceManager::onNamespaceDeleted(NamespaceEventCallback callback) {
    deletionCallbacks.push_back(callback);
}

void AdvancedNamespaceManager::onNamespaceMerged(NamespaceEventCallback callback) {
    mergeCallbacks.push_back(callback);
}

void AdvancedNamespaceManager::onConflictDetected(std::function<void(const ConflictInfo&)> callback) {
    conflictCallbacks.push_back(callback);
}

// 内部辅助方法实现
std::shared_ptr<AdvancedNamespaceInfo> AdvancedNamespaceManager::getAdvancedNamespaceInfo(const String& name) const {
    auto it = advancedNamespaces.find(name);
    return it != advancedNamespaces.end() ? it->second : nullptr;
}

void AdvancedNamespaceManager::createAdvancedNamespaceInfo(const String& name, NamespaceType type, NamespaceVisibility visibility) {
    auto info = std::make_shared<AdvancedNamespaceInfo>();
    info->name = name;
    info->type = type;
    info->visibility = visibility;
    info->depth = 0;
    info->lastModified = getCurrentTime();
    
    advancedNamespaces[name] = info;
}

std::vector<ConflictInfo> AdvancedNamespaceManager::detectNameCollisions() const {
    std::vector<ConflictInfo> conflicts;
    
    // 简化的名称冲突检测
    std::unordered_map<String, StringVector> itemLocations;
    
    // 收集所有项目位置
    for (const auto& [namespaceName, namespaceInfo] : namespaces) {
        if (namespaceInfo) {
            for (const auto& [itemName, node] : namespaceInfo->templates) {
                itemLocations[itemName].push_back(namespaceName);
            }
            for (const auto& [itemName, node] : namespaceInfo->customs) {
                itemLocations[itemName].push_back(namespaceName);
            }
        }
    }
    
    // 检测冲突
    for (const auto& [itemName, locations] : itemLocations) {
        if (locations.size() > 1) {
            for (size_t i = 0; i < locations.size(); ++i) {
                for (size_t j = i + 1; j < locations.size(); ++j) {
                    ConflictInfo conflict;
                    conflict.type = ConflictType::NAME_COLLISION;
                    conflict.namespaceName1 = locations[i];
                    conflict.namespaceName2 = locations[j];
                    conflict.conflictingItem = itemName;
                    conflict.description = "项目名称冲突: " + itemName;
                    conflict.recommendedStrategy = ConflictResolutionStrategy::WARN;
                    conflicts.push_back(conflict);
                }
            }
        }
    }
    
    return conflicts;
}

std::vector<ConflictInfo> AdvancedNamespaceManager::detectTypeMismatches() const {
    // 实现类型不匹配检测
    return {};
}

std::vector<ConflictInfo> AdvancedNamespaceManager::detectScopeViolations() const {
    // 实现作用域违反检测
    return {};
}

std::vector<ConflictInfo> AdvancedNamespaceManager::detectCircularDependenciesInternal() const {
    std::vector<ConflictInfo> conflicts;
    
    if (hasCircularDependencies()) {
        StringVector cycles = findCircularDependencies();
        for (const String& cycle : cycles) {
            ConflictInfo conflict;
            conflict.type = ConflictType::CIRCULAR_DEPENDENCY;
            conflict.description = "循环依赖: " + cycle;
            conflict.recommendedStrategy = ConflictResolutionStrategy::ERROR;
            conflicts.push_back(conflict);
        }
    }
    
    return conflicts;
}

std::vector<ConflictInfo> AdvancedNamespaceManager::detectAccessViolations() const {
    // 实现访问违反检测
    return {};
}

bool AdvancedNamespaceManager::hasCircularDependencyDFS(const String& namespaceName, StringUnorderedSet& visited, 
                                                       StringUnorderedSet& recursionStack) const {
    visited.insert(namespaceName);
    recursionStack.insert(namespaceName);
    
    auto info = getAdvancedNamespaceInfo(namespaceName);
    if (info) {
        for (const String& dependency : info->dependencies) {
            if (recursionStack.find(dependency) != recursionStack.end()) {
                return true; // 找到循环
            }
            if (visited.find(dependency) == visited.end()) {
                if (hasCircularDependencyDFS(dependency, visited, recursionStack)) {
                    return true;
                }
            }
        }
    }
    
    recursionStack.erase(namespaceName);
    return false;
}

bool AdvancedNamespaceManager::isAccessibleFromNamespace(const String& accessorNamespace, const String& targetNamespace) const {
    if (accessorNamespace == targetNamespace) {
        return true;
    }
    
    NamespaceVisibility targetVisibility = getNamespaceVisibility(targetNamespace);
    
    switch (targetVisibility) {
        case NamespaceVisibility::PUBLIC:
            return true;
            
        case NamespaceVisibility::PRIVATE:
            return false;
            
        case NamespaceVisibility::PROTECTED:
            return isInSameHierarchy(accessorNamespace, targetNamespace);
            
        case NamespaceVisibility::INTERNAL:
            // 内部命名空间只对同一父命名空间可见
            return getParentNamespace(accessorNamespace) == getParentNamespace(targetNamespace);
    }
    
    return false;
}

bool AdvancedNamespaceManager::isInSameHierarchy(const String& namespace1, const String& namespace2) const {
    StringVector ancestors1 = getAllAncestors(namespace1);
    ancestors1.push_back(namespace1);
    
    StringVector ancestors2 = getAllAncestors(namespace2);
    ancestors2.push_back(namespace2);
    
    // 检查是否有共同祖先
    for (const String& ancestor1 : ancestors1) {
        for (const String& ancestor2 : ancestors2) {
            if (ancestor1 == ancestor2) {
                return true;
            }
        }
    }
    
    return false;
}

void AdvancedNamespaceManager::removeEmptyNamespaces() {
    // 实现移除空命名空间的逻辑
    compactNamespaces();
}

void AdvancedNamespaceManager::consolidateSimilarNamespaces() {
    // 实现合并相似命名空间的逻辑
    // 这里可以实现基于内容相似度的合并策略
}

void AdvancedNamespaceManager::reorderNamespaceHierarchy() {
    // 实现重新排序命名空间层次结构的逻辑
    // 可以按照依赖关系优化顺序
}

int AdvancedNamespaceManager::compareVersions(const String& version1, const String& version2) const {
    std::vector<int> v1, v2;
    
    if (!parseVersion(version1, v1) || !parseVersion(version2, v2)) {
        return 0; // 无法比较
    }
    
    size_t maxSize = std::max(v1.size(), v2.size());
    v1.resize(maxSize, 0);
    v2.resize(maxSize, 0);
    
    for (size_t i = 0; i < maxSize; ++i) {
        if (v1[i] < v2[i]) return -1;
        if (v1[i] > v2[i]) return 1;
    }
    
    return 0;
}

bool AdvancedNamespaceManager::parseVersion(const String& version, std::vector<int>& versionNumbers) const {
    std::regex versionRegex(R"(\d+)");
    std::sregex_iterator iter(version.begin(), version.end(), versionRegex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        versionNumbers.push_back(std::stoi(iter->str()));
    }
    
    return !versionNumbers.empty();
}

void AdvancedNamespaceManager::triggerNamespaceCreated(const String& namespaceName) {
    for (const auto& callback : creationCallbacks) {
        callback(namespaceName, "created");
    }
}

void AdvancedNamespaceManager::triggerNamespaceDeleted(const String& namespaceName) {
    for (const auto& callback : deletionCallbacks) {
        callback(namespaceName, "deleted");
    }
}

void AdvancedNamespaceManager::triggerNamespaceMerged(const String& targetNamespace, const String& sourceNamespace) {
    for (const auto& callback : mergeCallbacks) {
        callback(targetNamespace, sourceNamespace);
    }
}

void AdvancedNamespaceManager::triggerConflictDetected(const ConflictInfo& conflict) {
    for (const auto& callback : conflictCallbacks) {
        callback(conflict);
    }
}

double AdvancedNamespaceManager::getCurrentTime() const {
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    return std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
}

void AdvancedNamespaceManager::initializeDefaultStrategies() {
    conflictStrategies[ConflictType::NAME_COLLISION] = ConflictResolutionStrategy::WARN;
    conflictStrategies[ConflictType::TYPE_MISMATCH] = ConflictResolutionStrategy::ERROR;
    conflictStrategies[ConflictType::SCOPE_VIOLATION] = ConflictResolutionStrategy::ERROR;
    conflictStrategies[ConflictType::CIRCULAR_DEPENDENCY] = ConflictResolutionStrategy::ERROR;
    conflictStrategies[ConflictType::ACCESS_VIOLATION] = ConflictResolutionStrategy::ERROR;
    conflictStrategies[ConflictType::VERSION_CONFLICT] = ConflictResolutionStrategy::WARN;
}

void AdvancedNamespaceManager::initializeBuiltinNamespaces() {
    // 创建内置的全局命名空间
    if (!hasNamespace("global")) {
        createNamespace("global", NamespaceType::ROOT, NamespaceVisibility::PUBLIC);
    }
}

// ========== NamespaceBuilder 实现 ==========

NamespaceBuilder::NamespaceBuilder(const String& namespaceName) {
    namespaceInfo = std::make_shared<AdvancedNamespaceInfo>();
    namespaceInfo->name = namespaceName;
}

NamespaceBuilder& NamespaceBuilder::type(NamespaceType namespaceType) {
    namespaceInfo->type = namespaceType;
    return *this;
}

NamespaceBuilder& NamespaceBuilder::visibility(NamespaceVisibility vis) {
    namespaceInfo->visibility = vis;
    return *this;
}

NamespaceBuilder& NamespaceBuilder::version(const String& ver) {
    namespaceInfo->version = ver;
    return *this;
}

NamespaceBuilder& NamespaceBuilder::author(const String& authorName) {
    namespaceInfo->author = authorName;
    return *this;
}

NamespaceBuilder& NamespaceBuilder::description(const String& desc) {
    namespaceInfo->description = desc;
    return *this;
}

NamespaceBuilder& NamespaceBuilder::parent(const String& parentName) {
    namespaceInfo->parentNamespace = parentName;
    return *this;
}

NamespaceBuilder& NamespaceBuilder::child(const String& childName) {
    namespaceInfo->childNamespaces.push_back(childName);
    return *this;
}

NamespaceBuilder& NamespaceBuilder::children(const StringVector& childNames) {
    namespaceInfo->childNamespaces.insert(
        namespaceInfo->childNamespaces.end(),
        childNames.begin(),
        childNames.end()
    );
    return *this;
}

NamespaceBuilder& NamespaceBuilder::import(const String& importedNamespace, const String& alias) {
    namespaceInfo->importedNamespaces.push_back(importedNamespace);
    if (!alias.empty()) {
        namespaceInfo->aliases[alias] = importedNamespace;
    }
    return *this;
}

NamespaceBuilder& NamespaceBuilder::dependency(const String& dependencyNamespace) {
    namespaceInfo->dependencies.push_back(dependencyNamespace);
    return *this;
}

NamespaceBuilder& NamespaceBuilder::dependencies(const StringVector& dependencyNamespaces) {
    namespaceInfo->dependencies.insert(
        namespaceInfo->dependencies.end(),
        dependencyNamespaces.begin(),
        dependencyNamespaces.end()
    );
    return *this;
}

NamespaceBuilder& NamespaceBuilder::alias(const String& itemName, const String& aliasName) {
    namespaceInfo->aliases[aliasName] = itemName;
    return *this;
}

NamespaceBuilder& NamespaceBuilder::aliases(const std::unordered_map<String, String>& aliasMap) {
    namespaceInfo->aliases.insert(aliasMap.begin(), aliasMap.end());
    return *this;
}

NamespaceBuilder& NamespaceBuilder::sealed(bool isSealed) {
    namespaceInfo->isSealed = isSealed;
    return *this;
}

NamespaceBuilder& NamespaceBuilder::abstract(bool isAbstract) {
    namespaceInfo->isAbstract = isAbstract;
    return *this;
}

bool NamespaceBuilder::build(AdvancedNamespaceManager& manager) {
    if (!validateBuild()) {
        return false;
    }
    
    // 创建命名空间
    if (!manager.createNamespace(namespaceInfo->name, namespaceInfo->type, namespaceInfo->visibility)) {
        return false;
    }
    
    // 设置详细信息
    manager.setNamespaceVersion(namespaceInfo->name, namespaceInfo->version);
    
    // 设置父子关系
    if (!namespaceInfo->parentNamespace.empty()) {
        manager.setParentNamespace(namespaceInfo->name, namespaceInfo->parentNamespace);
    }
    
    // 添加依赖
    for (const String& dep : namespaceInfo->dependencies) {
        manager.addNamespaceDependency(namespaceInfo->name, dep);
    }
    
    // 添加导入
    for (const String& imported : namespaceInfo->importedNamespaces) {
        manager.importNamespace(namespaceInfo->name, imported);
    }
    
    // 添加别名
    for (const auto& [alias, target] : namespaceInfo->aliases) {
        manager.addNamespaceAlias(namespaceInfo->name, target, alias);
    }
    
    // 设置特性
    if (namespaceInfo->isSealed) {
        manager.sealNamespace(namespaceInfo->name);
    }
    if (namespaceInfo->isAbstract) {
        manager.markNamespaceAbstract(namespaceInfo->name);
    }
    
    return true;
}

std::shared_ptr<AdvancedNamespaceInfo> NamespaceBuilder::buildInfo() {
    return validateBuild() ? namespaceInfo : nullptr;
}

String NamespaceBuilder::buildToString() {
    if (!validateBuild()) {
        return "";
    }
    
    std::ostringstream oss;
    oss << "[Namespace] " << namespaceInfo->name << " {\n";
    
    if (!namespaceInfo->description.empty()) {
        oss << "    // " << namespaceInfo->description << "\n";
    }
    
    if (!namespaceInfo->version.empty()) {
        oss << "    // Version: " << namespaceInfo->version << "\n";
    }
    
    // 这里可以添加更多的序列化逻辑
    
    oss << "}";
    return oss.str();
}

bool NamespaceBuilder::validateBuild() const {
    return buildErrors.empty() && !namespaceInfo->name.empty();
}

void NamespaceBuilder::addBuildError(const String& error) {
    buildErrors.push_back(error);
}

// ========== NamespaceQuery 实现 ==========

NamespaceQuery::NamespaceQuery(const AdvancedNamespaceManager& manager) 
    : manager(manager), orderAscending(true), maxResults(SIZE_MAX), skipCount(0) {
    predicate = [this](const AdvancedNamespaceInfo& info) { return defaultPredicate(info); };
}

NamespaceQuery& NamespaceQuery::byName(const String& name) {
    addCondition([name](const AdvancedNamespaceInfo& info) {
        return info.name == name;
    });
    return *this;
}

NamespaceQuery& NamespaceQuery::byType(NamespaceType type) {
    addCondition([type](const AdvancedNamespaceInfo& info) {
        return info.type == type;
    });
    return *this;
}

NamespaceQuery& NamespaceQuery::byVisibility(NamespaceVisibility visibility) {
    addCondition([visibility](const AdvancedNamespaceInfo& info) {
        return info.visibility == visibility;
    });
    return *this;
}

NamespaceQuery& NamespaceQuery::byParent(const String& parentName) {
    addCondition([parentName](const AdvancedNamespaceInfo& info) {
        return info.parentNamespace == parentName;
    });
    return *this;
}

NamespaceQuery& NamespaceQuery::byDepth(size_t depth) {
    addCondition([depth](const AdvancedNamespaceInfo& info) {
        return info.depth == depth;
    });
    return *this;
}

NamespaceQuery& NamespaceQuery::byVersion(const String& version) {
    addCondition([version](const AdvancedNamespaceInfo& info) {
        return info.version == version;
    });
    return *this;
}

NamespaceQuery& NamespaceQuery::sealed(bool isSealed) {
    addCondition([isSealed](const AdvancedNamespaceInfo& info) {
        return info.isSealed == isSealed;
    });
    return *this;
}

NamespaceQuery& NamespaceQuery::abstract(bool isAbstract) {
    addCondition([isAbstract](const AdvancedNamespaceInfo& info) {
        return info.isAbstract == isAbstract;
    });
    return *this;
}

StringVector NamespaceQuery::execute() const {
    return manager.filterNamespaces(predicate);
}

size_t NamespaceQuery::count() const {
    return execute().size();
}

bool NamespaceQuery::exists() const {
    return count() > 0;
}

String NamespaceQuery::first() const {
    StringVector results = execute();
    return results.empty() ? "" : results[0];
}

void NamespaceQuery::addCondition(std::function<bool(const AdvancedNamespaceInfo&)> condition) {
    auto oldPredicate = predicate;
    predicate = [oldPredicate, condition](const AdvancedNamespaceInfo& info) {
        return oldPredicate(info) && condition(info);
    };
}

bool NamespaceQuery::defaultPredicate(const AdvancedNamespaceInfo& info) const {
    return true; // 默认接受所有命名空间
}

// ========== NamespaceUtils 实现 ==========

namespace NamespaceUtils {

std::pair<String, String> parseQualifiedName(const String& qualifiedName) {
    size_t pos = qualifiedName.rfind("::");
    if (pos != String::npos) {
        return {qualifiedName.substr(0, pos), qualifiedName.substr(pos + 2)};
    }
    return {"", qualifiedName};
}

String joinNamespacePath(const StringVector& pathComponents) {
    if (pathComponents.empty()) {
        return "";
    }
    
    std::ostringstream oss;
    for (size_t i = 0; i < pathComponents.size(); ++i) {
        if (i > 0) oss << "::";
        oss << pathComponents[i];
    }
    return oss.str();
}

StringVector splitNamespacePath(const String& namespacePath) {
    StringVector components;
    std::istringstream iss(namespacePath);
    String component;
    
    while (std::getline(iss, component, ':')) {
        if (!component.empty()) {
            components.push_back(component);
        }
    }
    
    return components;
}

String getNamespaceFromPath(const String& namespacePath) {
    auto [namespaceName, itemName] = parseQualifiedName(namespacePath);
    return namespaceName;
}

String getItemFromPath(const String& namespacePath) {
    auto [namespaceName, itemName] = parseQualifiedName(namespacePath);
    return itemName;
}

bool matchesPattern(const String& namespaceName, const String& pattern) {
    std::regex patternRegex(pattern);
    return std::regex_match(namespaceName, patternRegex);
}

StringVector expandWildcard(const StringVector& allNamespaces, const String& wildcardPattern) {
    StringVector matches;
    
    for (const String& namespaceName : allNamespaces) {
        if (matchesPattern(namespaceName, wildcardPattern)) {
            matches.push_back(namespaceName);
        }
    }
    
    return matches;
}

String normalizeNamespaceName(const String& name) {
    String normalized = name;
    
    // 移除前后空白
    normalized.erase(0, normalized.find_first_not_of(" \t"));
    normalized.erase(normalized.find_last_not_of(" \t") + 1);
    
    // 替换无效字符为下划线
    std::regex invalidChars(R"([^a-zA-Z0-9_])");
    normalized = std::regex_replace(normalized, invalidChars, "_");
    
    // 确保以字母或下划线开头
    if (!normalized.empty() && std::isdigit(normalized[0])) {
        normalized = "_" + normalized;
    }
    
    return normalized;
}

bool isValidNamespaceName(const String& name) {
    return normalizeNamespaceName(name) == name && !name.empty();
}

String sanitizeNamespaceName(const String& name) {
    return normalizeNamespaceName(name);
}

String generateUniqueNamespaceName(const StringVector& existingNames, const String& baseName) {
    if (std::find(existingNames.begin(), existingNames.end(), baseName) == existingNames.end()) {
        return baseName;
    }
    
    int counter = 1;
    String uniqueName;
    
    do {
        uniqueName = baseName + "_" + std::to_string(counter);
        counter++;
    } while (std::find(existingNames.begin(), existingNames.end(), uniqueName) != existingNames.end());
    
    return uniqueName;
}

String suggestAlternativeName(const StringVector& existingNames, const String& conflictingName) {
    StringVector suggestions = {
        conflictingName + "_alt",
        conflictingName + "_new",
        conflictingName + "_v2",
        "alt_" + conflictingName,
        "new_" + conflictingName
    };
    
    for (const String& suggestion : suggestions) {
        if (std::find(existingNames.begin(), existingNames.end(), suggestion) == existingNames.end()) {
            return suggestion;
        }
    }
    
    return generateUniqueNamespaceName(existingNames, conflictingName);
}

std::unordered_map<String, size_t> analyzeNamespaceUsage(const AdvancedNamespaceManager& manager) {
    std::unordered_map<String, size_t> usage;
    
    // 统计每个命名空间被导入的次数
    StringVector allNamespaces = manager.getAllNamespaces();
    for (const String& namespaceName : allNamespaces) {
        usage[namespaceName] = 0;
        
        for (const String& otherNamespace : allNamespaces) {
            StringVector imported = manager.getImportedNamespaces(otherNamespace);
            if (std::find(imported.begin(), imported.end(), namespaceName) != imported.end()) {
                usage[namespaceName]++;
            }
        }
    }
    
    return usage;
}

StringVector findUnusedNamespaces(const AdvancedNamespaceManager& manager) {
    auto usage = analyzeNamespaceUsage(manager);
    StringVector unused;
    
    for (const auto& [namespaceName, useCount] : usage) {
        if (useCount == 0) {
            unused.push_back(namespaceName);
        }
    }
    
    return unused;
}

StringVector findOrphanedNamespaces(const AdvancedNamespaceManager& manager) {
    StringVector orphaned;
    StringVector allNamespaces = manager.getAllNamespaces();
    
    for (const String& namespaceName : allNamespaces) {
        String parent = manager.getParentNamespace(namespaceName);
        StringVector children = manager.getChildNamespaces(namespaceName);
        StringVector dependencies = manager.getNamespaceDependencies(namespaceName);
        StringVector imported = manager.getImportedNamespaces(namespaceName);
        
        // 如果没有父命名空间、子命名空间、依赖或导入，则认为是孤立的
        if (parent.empty() && children.empty() && dependencies.empty() && imported.empty()) {
            orphaned.push_back(namespaceName);
        }
    }
    
    return orphaned;
}

} // namespace NamespaceUtils

} // namespace CHTL