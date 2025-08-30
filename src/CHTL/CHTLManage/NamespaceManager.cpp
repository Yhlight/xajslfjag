#include "NamespaceManager.h"
#include <algorithm>
#include <sstream>
#include <iostream>

namespace CHTL {

CHTLNamespaceManager::CHTLNamespaceManager() 
    : m_defaultNamespaceEnabled(true) {
}

void CHTLNamespaceManager::registerNamespace(std::shared_ptr<NamespaceNode> namespaceNode, const std::string& sourceFile) {
    if (!namespaceNode) {
        return;
    }
    
    std::string namespacePath = namespaceNode->getFullNamespacePath();
    
    // 规范化命名空间路径
    namespacePath = normalizeNamespacePath(namespacePath);
    
    // 检查是否已存在同名命名空间
    auto it = m_namespaces.find(namespacePath);
    if (it != m_namespaces.end()) {
        // 同名命名空间自动合并
        mergeNamespaces();
    } else {
        // 注册新命名空间
        m_namespaces[namespacePath] = namespaceNode;
        
        // 建立层次关系
        auto pathParts = splitNamespacePath(namespacePath);
        if (pathParts.size() > 1) {
            std::string parentPath = buildNamespacePath(std::vector<std::string>(pathParts.begin(), pathParts.end() - 1));
            m_namespaceHierarchy[namespacePath] = parentPath;
            m_childNamespaces[parentPath].push_back(namespacePath);
        }
    }
    
    // 注册命名空间中的符号 (简化实现，后续可以扩展)
    auto children = namespaceNode->getChildren();
    for (auto child : children) {
        NamespaceSymbolInfo symbol;
        symbol.sourceFile = sourceFile;
        symbol.namespacePath = namespacePath;
        symbol.name = child->getName();
        symbol.node = child;
        
        // 根据节点类型设置符号类型
        switch (child->getNodeType()) {
            case CHTLNodeType::TEMPLATE_STYLE_NODE:
                symbol.type = NamespaceSymbolType::TEMPLATE_STYLE;
                break;
            case CHTLNodeType::CUSTOM_STYLE_NODE:
                symbol.type = NamespaceSymbolType::CUSTOM_STYLE;
                break;
            case CHTLNodeType::NAMESPACE_NODE:
                symbol.type = NamespaceSymbolType::NAMESPACE_NODE;
                break;
            default:
                continue; // 跳过不支持的类型
        }
        
        registerSymbol(namespacePath, symbol);
    }
}

void CHTLNamespaceManager::registerSymbol(const std::string& namespacePath, const NamespaceSymbolInfo& symbol) {
    std::string normalizedPath = normalizeNamespacePath(namespacePath);
    
    // 检查冲突
    ConflictType conflict = checkConflict(normalizedPath, symbol);
    if (conflict != ConflictType::NO_CONFLICT) {
        std::cerr << "Warning: Symbol conflict detected for " << symbol.name 
                  << " in namespace " << normalizedPath << std::endl;
        return;
    }
    
    // 添加符号到符号表
    m_symbolTable[normalizedPath].push_back(symbol);
}

std::vector<NamespaceSymbolInfo> CHTLNamespaceManager::findSymbol(const std::string& name, NamespaceSymbolType type, 
                                                     const std::string& currentNamespace) const {
    std::vector<NamespaceSymbolInfo> results;
    
    // 首先在当前命名空间中查找
    if (!currentNamespace.empty()) {
        auto it = m_symbolTable.find(currentNamespace);
        if (it != m_symbolTable.end()) {
            for (const auto& symbol : it->second) {
                if (symbol.name == name && symbol.type == type) {
                    results.push_back(symbol);
                }
            }
        }
    }
    
    // 如果在当前命名空间中没有找到，在全局范围内查找
    if (results.empty()) {
        for (const auto& pair : m_symbolTable) {
            for (const auto& symbol : pair.second) {
                if (symbol.name == name && symbol.type == type) {
                    results.push_back(symbol);
                }
            }
        }
    }
    
    return results;
}

NamespaceSymbolInfo CHTLNamespaceManager::findSymbolByPath(const std::string& fullPath) const {
    // 解析完整路径 例如: "space.room2.Box"
    size_t lastDot = fullPath.find_last_of('.');
    if (lastDot == std::string::npos) {
        // 没有命名空间路径，直接查找符号
        auto symbols = findSymbol(fullPath);
        return symbols.empty() ? NamespaceSymbolInfo() : symbols[0];
    }
    
    std::string namespacePath = fullPath.substr(0, lastDot);
    std::string symbolName = fullPath.substr(lastDot + 1);
    
    namespacePath = normalizeNamespacePath(namespacePath);
    
    auto it = m_symbolTable.find(namespacePath);
    if (it != m_symbolTable.end()) {
        for (const auto& symbol : it->second) {
            if (symbol.name == symbolName) {
                return symbol;
            }
        }
    }
    
    return NamespaceSymbolInfo(); // 未找到
}

NamespaceSymbolInfo CHTLNamespaceManager::resolveNamespaceReference(const std::string& symbolName, NamespaceSymbolType type, 
                                                      const std::string& namespacePath, 
                                                      const std::string& currentNamespace) const {
    std::string normalizedPath = normalizeNamespacePath(namespacePath);
    
    auto it = m_symbolTable.find(normalizedPath);
    if (it != m_symbolTable.end()) {
        for (const auto& symbol : it->second) {
            if (symbol.name == symbolName && symbol.type == type) {
                return symbol;
            }
        }
    }
    
    return NamespaceSymbolInfo(); // 未找到
}

void CHTLNamespaceManager::mergeNamespaces() {
    // 实现同名命名空间的自动合并
    std::unordered_map<std::string, std::vector<std::shared_ptr<NamespaceNode>>> sameNameNamespaces;
    
    // 按名称分组
    for (const auto& pair : m_namespaces) {
        std::string name = pair.second->getNamespaceName();
        sameNameNamespaces[name].push_back(pair.second);
    }
    
    // 合并同名命名空间的符号
    for (const auto& group : sameNameNamespaces) {
        if (group.second.size() > 1) {
            // 存在同名命名空间，需要合并
            std::string primaryPath = group.second[0]->getFullNamespacePath();
            
            for (size_t i = 1; i < group.second.size(); ++i) {
                std::string secondaryPath = group.second[i]->getFullNamespacePath();
                
                // 合并符号表
                auto it = m_symbolTable.find(secondaryPath);
                if (it != m_symbolTable.end()) {
                    for (const auto& symbol : it->second) {
                        registerSymbol(primaryPath, symbol);
                    }
                    m_symbolTable.erase(it);
                }
                
                // 移除重复的命名空间
                m_namespaces.erase(secondaryPath);
            }
        }
    }
}

ConflictType CHTLNamespaceManager::checkConflict(const std::string& namespacePath, const NamespaceSymbolInfo& symbol) const {
    auto it = m_symbolTable.find(namespacePath);
    if (it == m_symbolTable.end()) {
        return ConflictType::NO_CONFLICT;
    }
    
    for (const auto& existingSymbol : it->second) {
        if (existingSymbol.name == symbol.name) {
            if (existingSymbol.type != symbol.type) {
                return ConflictType::TYPE_MISMATCH;
            }
            
            if (!isSymbolCompatible(existingSymbol, symbol)) {
                return ConflictType::MULTIPLE_DEFINITION;
            }
            
            return ConflictType::NAME_CONFLICT;
        }
    }
    
    return ConflictType::NO_CONFLICT;
}

std::string CHTLNamespaceManager::createDefaultFileNamespace(const std::string& filePath) const {
    if (!m_defaultNamespaceEnabled) {
        return "";
    }
    
    // 从文件路径提取文件名作为默认命名空间
    std::filesystem::path path(filePath);
    std::string filename = path.stem().string(); // 不包含扩展名的文件名
    
    return normalizeNamespacePath(filename);
}

void CHTLNamespaceManager::setDefaultNamespaceEnabled(bool enabled) {
    m_defaultNamespaceEnabled = enabled;
}

const std::unordered_map<std::string, std::shared_ptr<NamespaceNode>>& CHTLNamespaceManager::getAllNamespaces() const {
    return m_namespaces;
}

const std::unordered_map<std::string, std::vector<NamespaceSymbolInfo>>& CHTLNamespaceManager::getSymbolTable() const {
    return m_symbolTable;
}

std::vector<NamespaceSymbolInfo> CHTLNamespaceManager::getNamespaceSymbols(const std::string& namespacePath) const {
    std::string normalizedPath = normalizeNamespacePath(namespacePath);
    
    auto it = m_symbolTable.find(normalizedPath);
    if (it != m_symbolTable.end()) {
        return it->second;
    }
    
    return {};
}

std::vector<std::string> CHTLNamespaceManager::getChildNamespaces(const std::string& namespacePath) const {
    std::string normalizedPath = normalizeNamespacePath(namespacePath);
    
    auto it = m_childNamespaces.find(normalizedPath);
    if (it != m_childNamespaces.end()) {
        return it->second;
    }
    
    return {};
}

bool CHTLNamespaceManager::namespaceExists(const std::string& namespacePath) const {
    std::string normalizedPath = normalizeNamespacePath(namespacePath);
    return m_namespaces.find(normalizedPath) != m_namespaces.end();
}

std::vector<std::string> CHTLNamespaceManager::getNamespaceHierarchy(const std::string& namespacePath) const {
    std::vector<std::string> hierarchy;
    std::string currentPath = normalizeNamespacePath(namespacePath);
    
    while (!currentPath.empty()) {
        hierarchy.insert(hierarchy.begin(), currentPath);
        
        auto it = m_namespaceHierarchy.find(currentPath);
        if (it != m_namespaceHierarchy.end()) {
            currentPath = it->second;
        } else {
            break;
        }
    }
    
    return hierarchy;
}

void CHTLNamespaceManager::importNamespaceSymbols(const std::string& fromNamespace, const std::string& toNamespace, 
                                             const std::vector<NamespaceSymbolType>& symbolTypes) {
    std::string normalizedFrom = normalizeNamespacePath(fromNamespace);
    std::string normalizedTo = normalizeNamespacePath(toNamespace);
    
    auto it = m_symbolTable.find(normalizedFrom);
    if (it == m_symbolTable.end()) {
        return;
    }
    
    for (const auto& symbol : it->second) {
        // 如果指定了符号类型过滤器，检查是否匹配
        if (!symbolTypes.empty()) {
            bool typeMatches = std::find(symbolTypes.begin(), symbolTypes.end(), symbol.type) != symbolTypes.end();
            if (!typeMatches) {
                continue;
            }
        }
        
        // 创建导入的符号副本
        NamespaceSymbolInfo importedSymbol = symbol;
        importedSymbol.namespacePath = normalizedTo;
        
        registerSymbol(normalizedTo, importedSymbol);
    }
}

std::string CHTLNamespaceManager::generateConflictReport() const {
    std::stringstream report;
    report << "Namespace Conflict Report\n";
    report << "=========================\n\n";
    
    for (const auto& namespacePair : m_symbolTable) {
        const std::string& namespacePath = namespacePair.first;
        const auto& symbols = namespacePair.second;
        
        std::unordered_map<std::string, std::vector<NamespaceSymbolInfo>> nameGroups;
        for (const auto& symbol : symbols) {
            nameGroups[symbol.name].push_back(symbol);
        }
        
        for (const auto& nameGroup : nameGroups) {
            if (nameGroup.second.size() > 1) {
                report << "Conflict in namespace '" << namespacePath << "':\n";
                report << "  Symbol '" << nameGroup.first << "' has multiple definitions:\n";
                
                for (const auto& symbol : nameGroup.second) {
                    report << "    - Type: " << getNamespaceSymbolTypeString(symbol.type);
                    if (!symbol.sourceFile.empty()) {
                        report << ", Source: " << symbol.sourceFile;
                    }
                    report << "\n";
                }
                report << "\n";
            }
        }
    }
    
    return report.str();
}

std::string CHTLNamespaceManager::generateUsageReport() const {
    std::stringstream report;
    report << "Namespace Usage Report\n";
    report << "======================\n\n";
    
    report << "Total namespaces: " << m_namespaces.size() << "\n";
    report << "Total symbol entries: ";
    
    size_t totalSymbols = 0;
    for (const auto& pair : m_symbolTable) {
        totalSymbols += pair.second.size();
    }
    report << totalSymbols << "\n\n";
    
    for (const auto& namespacePair : m_namespaces) {
        const std::string& path = namespacePair.first;
        const auto& namespaceNode = namespacePair.second;
        
        report << "Namespace: " << path << "\n";
        report << "  Name: " << namespaceNode->getNamespaceName() << "\n";
        
        auto symbols = getNamespaceSymbols(path);
        report << "  Symbols: " << symbols.size() << "\n";
        
        std::unordered_map<NamespaceSymbolType, int> typeCounts;
        for (const auto& symbol : symbols) {
            typeCounts[symbol.type]++;
        }
        
        for (const auto& typeCount : typeCounts) {
            report << "    " << getNamespaceSymbolTypeString(typeCount.first) << ": " << typeCount.second << "\n";
        }
        
        auto children = getChildNamespaces(path);
        if (!children.empty()) {
            report << "  Child namespaces: ";
            for (size_t i = 0; i < children.size(); ++i) {
                if (i > 0) report << ", ";
                report << children[i];
            }
            report << "\n";
        }
        
        report << "\n";
    }
    
    return report.str();
}

// ========== 私有方法实现 ==========

std::string CHTLNamespaceManager::normalizeNamespacePath(const std::string& path) const {
    if (path.empty()) {
        return "";
    }
    
    std::string normalized = path;
    
    // 移除前后空白
    size_t start = normalized.find_first_not_of(" \t\n\r");
    if (start == std::string::npos) {
        return "";
    }
    
    size_t end = normalized.find_last_not_of(" \t\n\r");
    normalized = normalized.substr(start, end - start + 1);
    
    // 将连续的点替换为单个点
    size_t pos = 0;
    while ((pos = normalized.find("..", pos)) != std::string::npos) {
        normalized.replace(pos, 2, ".");
        pos += 1;
    }
    
    // 移除开头和结尾的点
    if (!normalized.empty() && normalized[0] == '.') {
        normalized = normalized.substr(1);
    }
    if (!normalized.empty() && normalized.back() == '.') {
        normalized.pop_back();
    }
    
    return normalized;
}

std::vector<std::string> CHTLNamespaceManager::splitNamespacePath(const std::string& path) const {
    std::vector<std::string> parts;
    std::string normalizedPath = normalizeNamespacePath(path);
    
    if (normalizedPath.empty()) {
        return parts;
    }
    
    std::stringstream ss(normalizedPath);
    std::string part;
    
    while (std::getline(ss, part, '.')) {
        if (!part.empty()) {
            parts.push_back(part);
        }
    }
    
    return parts;
}

std::string CHTLNamespaceManager::buildNamespacePath(const std::vector<std::string>& parts) const {
    if (parts.empty()) {
        return "";
    }
    
    std::stringstream ss;
    for (size_t i = 0; i < parts.size(); ++i) {
        if (i > 0) {
            ss << ".";
        }
        ss << parts[i];
    }
    
    return ss.str();
}

bool CHTLNamespaceManager::isSymbolCompatible(const NamespaceSymbolInfo& existing, const NamespaceSymbolInfo& newSymbol) const {
    // 检查符号是否可以共存（例如，可以合并的模板）
    if (existing.type != newSymbol.type) {
        return false;
    }
    
    // 对于某些类型的符号，允许合并或重定义
    switch (existing.type) {
        case NamespaceSymbolType::TEMPLATE_STYLE:
        case NamespaceSymbolType::TEMPLATE_ELEMENT:
        case NamespaceSymbolType::TEMPLATE_VAR:
            // 模板可以被重定义或继承
            return true;
            
        case NamespaceSymbolType::CUSTOM_STYLE:
        case NamespaceSymbolType::CUSTOM_ELEMENT:
        case NamespaceSymbolType::CUSTOM_VAR:
            // 自定义可以被特例化
            return true;
            
        case NamespaceSymbolType::NAMESPACE_NODE:
            // 命名空间可以合并
            return true;
            
        default:
            return false;
    }
}

NamespaceSymbolInfo CHTLNamespaceManager::mergeNamespaceSymbolInfo(const NamespaceSymbolInfo& existing, const NamespaceSymbolInfo& newSymbol) const {
    // 创建合并后的符号信息
    NamespaceSymbolInfo merged = existing;
    
    // 更新源文件信息
    if (!newSymbol.sourceFile.empty()) {
        if (merged.sourceFile.empty()) {
            merged.sourceFile = newSymbol.sourceFile;
        } else {
            merged.sourceFile += ";" + newSymbol.sourceFile;
        }
    }
    
    return merged;
}

std::string CHTLNamespaceManager::findClosestNamespace(const std::string& currentNamespace, const std::string& targetName) const {
    // 在当前命名空间及其父命名空间中查找最接近的匹配
    std::vector<std::string> hierarchy = getNamespaceHierarchy(currentNamespace);
    
    for (const auto& namespacePath : hierarchy) {
        if (namespaceExists(namespacePath + "." + targetName)) {
            return namespacePath + "." + targetName;
        }
    }
    
    return "";
}

std::string CHTLNamespaceManager::getNamespaceSymbolTypeString(NamespaceSymbolType type) const {
    switch (type) {
        case NamespaceSymbolType::TEMPLATE_STYLE:   return "Template Style";
        case NamespaceSymbolType::TEMPLATE_ELEMENT: return "Template Element";
        case NamespaceSymbolType::TEMPLATE_VAR:     return "Template Variable";
        case NamespaceSymbolType::CUSTOM_STYLE:     return "Custom Style";
        case NamespaceSymbolType::CUSTOM_ELEMENT:   return "Custom Element";
        case NamespaceSymbolType::CUSTOM_VAR:       return "Custom Variable";
        case NamespaceSymbolType::ORIGIN_NODE:      return "Origin Node";
        case NamespaceSymbolType::CONFIG_NODE:      return "Configuration";
        case NamespaceSymbolType::NAMESPACE_NODE:   return "Namespace";
        default:                           return "Unknown";
    }
}

} // namespace CHTL