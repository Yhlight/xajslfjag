#include "NamespaceResolver.h"
#include "../CHTLNode/BaseNode.h"
#include <algorithm>
#include <stack>
#include <regex>

namespace CHTL {

// NamespaceInfo实现
bool NamespaceInfo::hasConflict(const String& name) const {
    return conflicts.find(name) != conflicts.end();
}

void NamespaceInfo::addConflict(const String& name, const ConflictInfo& conflict) {
    conflicts[name].push_back(conflict);
}

StringVector NamespaceInfo::getConflicts(const String& name) const {
    auto it = conflicts.find(name);
    if (it != conflicts.end()) {
        StringVector result;
        for (const auto& conflict : it->second) {
            result.push_back(conflict.conflictType + " at " + conflict.location);
        }
        return result;
    }
    return {};
}

// NamespaceResolver实现
NamespaceResolver::NamespaceResolver(const NamespaceConfig& config) 
    : config(config), currentNamespaceDepth(0) {
    // 初始化全局命名空间
    NamespaceInfo globalNs;
    globalNs.name = "__global__";
    globalNs.level = 0;
    globalNs.isGlobal = true;
    namespaces["__global__"] = globalNs;
    
    currentNamespace = "__global__";
}

NamespaceResolutionResult NamespaceResolver::resolveNamespaces(BaseNode* ast) {
    NamespaceResolutionResult result;
    result.success = false;
    
    if (!ast) {
        result.errors.push_back("AST为空");
        return result;
    }
    
    try {
        // 第一遍：收集所有命名空间定义
        collectNamespaceDefinitions(ast, result);
        
        // 第二遍：解析命名空间作用域
        resolveNamespaceScopes(ast, result);
        
        // 第三遍：检测冲突
        detectNamespaceConflicts(result);
        
        // 第四遍：应用解决方案
        applyConflictResolutions(result);
        
        result.success = result.errors.empty();
        
    } catch (const std::exception& e) {
        result.errors.push_back("命名空间解析异常: " + String(e.what()));
    }
    
    return result;
}

void NamespaceResolver::collectNamespaceDefinitions(BaseNode* node, NamespaceResolutionResult& result) {
    if (!node) return;
    
    // 检查是否是命名空间定义节点
    if (node->type == NodeType::NAMESPACE) {
        processNamespaceDefinition(node, result);
    }
    
    // 检查嵌套命名空间
    if (isInNamespaceContext(node)) {
        enterNamespaceContext(node);
    }
    
    // 递归处理子节点
    for (const auto& child : node->children) {
        collectNamespaceDefinitions(child.get(), result);
    }
    
    if (isInNamespaceContext(node)) {
        exitNamespaceContext();
    }
}

void NamespaceResolver::processNamespaceDefinition(BaseNode* namespaceNode, NamespaceResolutionResult& result) {
    String namespaceName = namespaceNode->value;
    
    if (namespaceName.empty()) {
        result.errors.push_back("命名空间名称不能为空");
        return;
    }
    
    // 检查命名空间是否已定义
    String fullName = buildFullNamespaceName(namespaceName);
    
    if (namespaces.find(fullName) != namespaces.end()) {
        result.warnings.push_back("重复定义命名空间: " + fullName);
        return;
    }
    
    // 创建命名空间信息
    NamespaceInfo nsInfo;
    nsInfo.name = namespaceName;
    nsInfo.fullName = fullName;
    nsInfo.level = currentNamespaceDepth;
    nsInfo.parentNamespace = currentNamespace;
    nsInfo.isGlobal = (currentNamespaceDepth == 0);
    nsInfo.definitionNode = namespaceNode;
    
    // 处理命名空间属性
    processNamespaceAttributes(namespaceNode, nsInfo);
    
    // 注册命名空间
    namespaces[fullName] = nsInfo;
    result.resolvedNamespaces.push_back(nsInfo);
    
    // 更新当前命名空间上下文
    namespaceStack.push_back(currentNamespace);
    currentNamespace = fullName;
    currentNamespaceDepth++;
}

void NamespaceResolver::processNamespaceAttributes(BaseNode* namespaceNode, NamespaceInfo& nsInfo) {
    // 处理访问修饰符
    if (namespaceNode->hasAttribute("access")) {
        String access = namespaceNode->getAttribute("access");
        nsInfo.isPublic = (access == "public");
        nsInfo.isPrivate = (access == "private");
    }
    
    // 处理别名
    if (namespaceNode->hasAttribute("alias")) {
        nsInfo.alias = namespaceNode->getAttribute("alias");
    }
    
    // 处理导出列表
    if (namespaceNode->hasAttribute("exports")) {
        String exports = namespaceNode->getAttribute("exports");
        nsInfo.exportedSymbols = splitString(exports, ",");
    }
    
    // 处理导入列表
    if (namespaceNode->hasAttribute("imports")) {
        String imports = namespaceNode->getAttribute("imports");
        nsInfo.importedNamespaces = splitString(imports, ",");
    }
}

void NamespaceResolver::resolveNamespaceScopes(BaseNode* node, NamespaceResolutionResult& result) {
    if (!node) return;
    
    // 建立符号表
    if (isSymbolDefinition(node)) {
        processSymbolDefinition(node, result);
    }
    
    // 处理符号引用
    if (isSymbolReference(node)) {
        processSymbolReference(node, result);
    }
    
    // 处理命名空间使用
    if (node->type == NodeType::USE) {
        processNamespaceUse(node, result);
    }
    
    // 递归处理子节点
    for (const auto& child : node->children) {
        resolveNamespaceScopes(child.get(), result);
    }
}

bool NamespaceResolver::isSymbolDefinition(BaseNode* node) {
    return node->type == NodeType::TEMPLATE ||
           node->type == NodeType::CUSTOM ||
           node->type == NodeType::FUNCTION ||
           node->type == NodeType::VARIABLE;
}

void NamespaceResolver::processSymbolDefinition(BaseNode* node, NamespaceResolutionResult& result) {
    String symbolName = node->value;
    String fullSymbolName = buildFullSymbolName(symbolName);
    
    // 检查当前命名空间中是否已存在同名符号
    NamespaceInfo& currentNs = namespaces[currentNamespace];
    
    if (currentNs.symbols.find(symbolName) != currentNs.symbols.end()) {
        // 记录冲突
        ConflictInfo conflict;
        conflict.conflictType = "Symbol redefinition";
        conflict.location = getNodeLocation(node);
        conflict.suggestedResolution = "Rename symbol or use namespace qualifier";
        
        currentNs.addConflict(symbolName, conflict);
        result.warnings.push_back("符号重定义: " + fullSymbolName);
    }
    
    // 注册符号
    SymbolInfo symbolInfo;
    symbolInfo.name = symbolName;
    symbolInfo.fullName = fullSymbolName;
    symbolInfo.type = nodeTypeToString(node->type);
    symbolInfo.namespace_ = currentNamespace;
    symbolInfo.definitionNode = node;
    symbolInfo.isPublic = isPublicSymbol(node);
    
    currentNs.symbols[symbolName] = symbolInfo;
}

void NamespaceResolver::processSymbolReference(BaseNode* node, NamespaceResolutionResult& result) {
    String symbolName = extractSymbolName(node);
    if (symbolName.empty()) return;
    
    // 尝试解析符号
    SymbolInfo resolvedSymbol = resolveSymbol(symbolName, result);
    
    if (resolvedSymbol.name.empty()) {
        result.errors.push_back("未定义的符号: " + symbolName + " at " + getNodeLocation(node));
    } else {
        // 记录符号使用
        node->setAttribute("resolved_symbol", resolvedSymbol.fullName);
        node->setAttribute("resolved_namespace", resolvedSymbol.namespace_);
    }
}

void NamespaceResolver::processNamespaceUse(BaseNode* useNode, NamespaceResolutionResult& result) {
    String namespaceName = useNode->value;
    
    // 解析命名空间路径
    String resolvedNamespace = resolveNamespacePath(namespaceName);
    
    if (resolvedNamespace.empty()) {
        result.errors.push_back("未找到命名空间: " + namespaceName);
        return;
    }
    
    // 添加到当前命名空间的导入列表
    NamespaceInfo& currentNs = namespaces[currentNamespace];
    currentNs.importedNamespaces.push_back(resolvedNamespace);
    
    // 处理using指令的作用域
    if (useNode->hasAttribute("scope")) {
        String scope = useNode->getAttribute("scope");
        if (scope == "local") {
            // 局部using，只影响当前块
            localUsingDirectives.push_back({resolvedNamespace, getCurrentScopeDepth()});
        }
    }
}

SymbolInfo NamespaceResolver::resolveSymbol(const String& symbolName, NamespaceResolutionResult& result) {
    // 检查是否是限定名称（包含命名空间前缀）
    if (symbolName.find("::") != String::npos) {
        return resolveQualifiedSymbol(symbolName, result);
    }
    
    // 未限定名称解析
    return resolveUnqualifiedSymbol(symbolName, result);
}

SymbolInfo NamespaceResolver::resolveQualifiedSymbol(const String& qualifiedName, NamespaceResolutionResult& result) {
    size_t lastColon = qualifiedName.rfind("::");
    if (lastColon == String::npos) {
        return SymbolInfo(); // 空符号信息
    }
    
    String namespacePart = qualifiedName.substr(0, lastColon);
    String symbolPart = qualifiedName.substr(lastColon + 2);
    
    // 解析命名空间部分
    String resolvedNamespace = resolveNamespacePath(namespacePart);
    if (resolvedNamespace.empty()) {
        return SymbolInfo();
    }
    
    // 在指定命名空间中查找符号
    auto nsIt = namespaces.find(resolvedNamespace);
    if (nsIt != namespaces.end()) {
        auto symbolIt = nsIt->second.symbols.find(symbolPart);
        if (symbolIt != nsIt->second.symbols.end()) {
            return symbolIt->second;
        }
    }
    
    return SymbolInfo();
}

SymbolInfo NamespaceResolver::resolveUnqualifiedSymbol(const String& symbolName, NamespaceResolutionResult& result) {
    // 首先在当前命名空间中查找
    NamespaceInfo& currentNs = namespaces[currentNamespace];
    auto it = currentNs.symbols.find(symbolName);
    if (it != currentNs.symbols.end()) {
        return it->second;
    }
    
    // 在导入的命名空间中查找
    for (const String& importedNs : currentNs.importedNamespaces) {
        auto nsIt = namespaces.find(importedNs);
        if (nsIt != namespaces.end()) {
            auto symbolIt = nsIt->second.symbols.find(symbolName);
            if (symbolIt != nsIt->second.symbols.end() && symbolIt->second.isPublic) {
                return symbolIt->second;
            }
        }
    }
    
    // 在using指令引入的命名空间中查找
    for (const auto& usingDir : localUsingDirectives) {
        if (usingDir.scopeDepth <= getCurrentScopeDepth()) {
            auto nsIt = namespaces.find(usingDir.namespaceName);
            if (nsIt != namespaces.end()) {
                auto symbolIt = nsIt->second.symbols.find(symbolName);
                if (symbolIt != nsIt->second.symbols.end() && symbolIt->second.isPublic) {
                    return symbolIt->second;
                }
            }
        }
    }
    
    // 在父命名空间中查找
    return searchInParentNamespaces(symbolName, currentNamespace);
}

SymbolInfo NamespaceResolver::searchInParentNamespaces(const String& symbolName, const String& startNamespace) {
    String currentNs = startNamespace;
    
    while (!currentNs.empty() && currentNs != "__global__") {
        auto nsIt = namespaces.find(currentNs);
        if (nsIt != namespaces.end()) {
            String parentNs = nsIt->second.parentNamespace;
            if (!parentNs.empty()) {
                auto parentIt = namespaces.find(parentNs);
                if (parentIt != namespaces.end()) {
                    auto symbolIt = parentIt->second.symbols.find(symbolName);
                    if (symbolIt != parentIt->second.symbols.end()) {
                        return symbolIt->second;
                    }
                }
                currentNs = parentNs;
            } else {
                break;
            }
        } else {
            break;
        }
    }
    
    return SymbolInfo();
}

void NamespaceResolver::detectNamespaceConflicts(NamespaceResolutionResult& result) {
    for (auto& nsPair : namespaces) {
        NamespaceInfo& nsInfo = nsPair.second;
        
        // 检测符号冲突
        detectSymbolConflicts(nsInfo, result);
        
        // 检测命名空间名称冲突
        detectNamespaceNameConflicts(nsInfo, result);
        
        // 检测导入冲突
        detectImportConflicts(nsInfo, result);
    }
}

void NamespaceResolver::detectSymbolConflicts(NamespaceInfo& nsInfo, NamespaceResolutionResult& result) {
    StringUnorderedMap symbolCounts;
    
    // 统计符号出现次数
    for (const auto& symbolPair : nsInfo.symbols) {
        const String& symbolName = symbolPair.first;
        symbolCounts[symbolName]++;
    }
    
    // 检查重复符号
    for (const auto& countPair : symbolCounts) {
        if (countPair.second > 1) {
            ConflictInfo conflict;
            conflict.conflictType = "Symbol name conflict";
            conflict.location = nsInfo.name;
            conflict.suggestedResolution = "Use different names or full qualification";
            
            nsInfo.addConflict(countPair.first, conflict);
            result.warnings.push_back("符号名称冲突: " + countPair.first + " in " + nsInfo.name);
        }
    }
}

void NamespaceResolver::detectNamespaceNameConflicts(NamespaceInfo& nsInfo, NamespaceResolutionResult& result) {
    // 检查命名空间名称是否与符号名称冲突
    for (const auto& otherNsPair : namespaces) {
        if (otherNsPair.first != nsInfo.fullName) {
            const NamespaceInfo& otherNs = otherNsPair.second;
            
            if (nsInfo.name == otherNs.name && 
                nsInfo.parentNamespace == otherNs.parentNamespace) {
                ConflictInfo conflict;
                conflict.conflictType = "Namespace name conflict";
                conflict.location = nsInfo.fullName;
                conflict.suggestedResolution = "Use different namespace names";
                
                nsInfo.addConflict(nsInfo.name, conflict);
                result.errors.push_back("命名空间名称冲突: " + nsInfo.name);
            }
        }
    }
}

void NamespaceResolver::detectImportConflicts(NamespaceInfo& nsInfo, NamespaceResolutionResult& result) {
    StringUnorderedSet importedSymbols;
    
    for (const String& importedNs : nsInfo.importedNamespaces) {
        auto nsIt = namespaces.find(importedNs);
        if (nsIt != namespaces.end()) {
            for (const auto& symbolPair : nsIt->second.symbols) {
                const String& symbolName = symbolPair.first;
                
                if (importedSymbols.find(symbolName) != importedSymbols.end()) {
                    ConflictInfo conflict;
                    conflict.conflictType = "Import conflict";
                    conflict.location = nsInfo.name;
                    conflict.suggestedResolution = "Use explicit qualification or aliases";
                    
                    nsInfo.addConflict(symbolName, conflict);
                    result.warnings.push_back("导入冲突: " + symbolName + " in " + nsInfo.name);
                }
                
                importedSymbols.insert(symbolName);
            }
        }
    }
}

void NamespaceResolver::applyConflictResolutions(NamespaceResolutionResult& result) {
    for (auto& nsPair : namespaces) {
        NamespaceInfo& nsInfo = nsPair.second;
        
        for (const auto& conflictPair : nsInfo.conflicts) {
            const String& conflictName = conflictPair.first;
            const auto& conflicts = conflictPair.second;
            
            for (const ConflictInfo& conflict : conflicts) {
                if (config.autoResolveConflicts) {
                    applyAutoResolution(nsInfo, conflictName, conflict, result);
                }
            }
        }
    }
}

void NamespaceResolver::applyAutoResolution(NamespaceInfo& nsInfo, const String& conflictName, 
                                          const ConflictInfo& conflict, NamespaceResolutionResult& result) {
    if (conflict.conflictType == "Symbol redefinition") {
        // 自动重命名符号
        String newName = generateUniqueName(conflictName, nsInfo);
        
        auto symbolIt = nsInfo.symbols.find(conflictName);
        if (symbolIt != nsInfo.symbols.end()) {
            SymbolInfo symbolInfo = symbolIt->second;
            nsInfo.symbols.erase(symbolIt);
            
            symbolInfo.name = newName;
            symbolInfo.fullName = buildFullSymbolName(newName);
            nsInfo.symbols[newName] = symbolInfo;
            
            result.warnings.push_back("自动重命名符号: " + conflictName + " -> " + newName);
        }
    }
}

String NamespaceResolver::generateUniqueName(const String& baseName, const NamespaceInfo& nsInfo) {
    String uniqueName = baseName;
    int counter = 1;
    
    while (nsInfo.symbols.find(uniqueName) != nsInfo.symbols.end()) {
        uniqueName = baseName + "_" + std::to_string(counter);
        counter++;
    }
    
    return uniqueName;
}

// 辅助方法实现
String NamespaceResolver::buildFullNamespaceName(const String& namespaceName) {
    if (currentNamespace == "__global__") {
        return namespaceName;
    }
    return currentNamespace + "::" + namespaceName;
}

String NamespaceResolver::buildFullSymbolName(const String& symbolName) {
    if (currentNamespace == "__global__") {
        return symbolName;
    }
    return currentNamespace + "::" + symbolName;
}

String NamespaceResolver::resolveNamespacePath(const String& namespacePath) {
    // 如果是绝对路径（以::开头）
    if (namespacePath.length() > 2 && namespacePath.substr(0, 2) == "::") {
        String absolutePath = namespacePath.substr(2);
        if (namespaces.find(absolutePath) != namespaces.end()) {
            return absolutePath;
        }
    }
    
    // 相对路径解析
    String currentPath = currentNamespace;
    while (!currentPath.empty()) {
        String candidatePath = (currentPath == "__global__") ? 
                               namespacePath : 
                               currentPath + "::" + namespacePath;
        
        if (namespaces.find(candidatePath) != namespaces.end()) {
            return candidatePath;
        }
        
        // 向上查找父命名空间
        auto nsIt = namespaces.find(currentPath);
        if (nsIt != namespaces.end() && !nsIt->second.parentNamespace.empty()) {
            currentPath = nsIt->second.parentNamespace;
        } else {
            break;
        }
    }
    
    return "";
}

bool NamespaceResolver::isInNamespaceContext(BaseNode* node) {
    return node && node->type == NodeType::NAMESPACE;
}

void NamespaceResolver::enterNamespaceContext(BaseNode* namespaceNode) {
    namespaceStack.push_back(currentNamespace);
    currentNamespace = buildFullNamespaceName(namespaceNode->value);
    currentNamespaceDepth++;
}

void NamespaceResolver::exitNamespaceContext() {
    if (!namespaceStack.empty()) {
        currentNamespace = namespaceStack.back();
        namespaceStack.pop_back();
        currentNamespaceDepth--;
    }
}

bool NamespaceResolver::isSymbolReference(BaseNode* node) {
    return node && (node->type == NodeType::IDENTIFIER || 
                   node->type == NodeType::CALL);
}

String NamespaceResolver::extractSymbolName(BaseNode* node) {
    if (!node) return "";
    
    switch (node->type) {
        case NodeType::IDENTIFIER:
            return node->value;
        case NodeType::CALL:
            return node->getAttribute("function_name");
        default:
            return "";
    }
}

bool NamespaceResolver::isPublicSymbol(BaseNode* node) {
    if (!node) return false;
    
    String access = node->getAttribute("access");
    return access.empty() || access == "public";
}

String NamespaceResolver::getNodeLocation(BaseNode* node) {
    if (!node) return "unknown";
    
    return "line:" + std::to_string(node->position.line) + 
           ", col:" + std::to_string(node->position.column);
}

int NamespaceResolver::getCurrentScopeDepth() {
    return currentNamespaceDepth;
}

StringVector NamespaceResolver::splitString(const String& str, const String& delimiter) {
    StringVector result;
    size_t start = 0;
    size_t end = str.find(delimiter);
    
    while (end != String::npos) {
        String token = str.substr(start, end - start);
        if (!token.empty()) {
            result.push_back(token);
        }
        start = end + delimiter.length();
        end = str.find(delimiter, start);
    }
    
    String lastToken = str.substr(start);
    if (!lastToken.empty()) {
        result.push_back(lastToken);
    }
    
    return result;
}

String NamespaceResolver::nodeTypeToString(NodeType type) {
    switch (type) {
        case NodeType::TEMPLATE: return "Template";
        case NodeType::CUSTOM: return "Custom";
        case NodeType::FUNCTION: return "Function";
        case NodeType::VARIABLE: return "Variable";
        default: return "Unknown";
    }
}

void NamespaceResolver::setConfig(const NamespaceConfig& newConfig) {
    config = newConfig;
}

NamespaceConfig NamespaceResolver::getConfig() const {
    return config;
}

StringUnorderedMap NamespaceResolver::getAllNamespaces() const {
    StringUnorderedMap result;
    for (const auto& nsPair : namespaces) {
        result[nsPair.first] = nsPair.second.name;
    }
    return result;
}

NamespaceInfo NamespaceResolver::getNamespaceInfo(const String& namespaceName) const {
    auto it = namespaces.find(namespaceName);
    if (it != namespaces.end()) {
        return it->second;
    }
    return NamespaceInfo();
}

void NamespaceResolver::clearNamespaces() {
    namespaces.clear();
    namespaceStack.clear();
    localUsingDirectives.clear();
    currentNamespace = "__global__";
    currentNamespaceDepth = 0;
    
    // 重新初始化全局命名空间
    NamespaceInfo globalNs;
    globalNs.name = "__global__";
    globalNs.level = 0;
    globalNs.isGlobal = true;
    namespaces["__global__"] = globalNs;
}

} // namespace CHTL