#include "NamespaceManager.h"
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace fs = std::filesystem;

namespace CHTL {

// NamespaceInfo 实现

void NamespaceInfo::addSymbol(const NamespaceSymbol& symbol) {
    symbols_[symbol.name] = symbol;
}

bool NamespaceInfo::hasSymbol(const std::string& name) const {
    return symbols_.find(name) != symbols_.end();
}

std::optional<NamespaceSymbol> NamespaceInfo::getSymbol(const std::string& name) const {
    auto it = symbols_.find(name);
    if (it != symbols_.end()) {
        return it->second;
    }
    return std::nullopt;
}

std::vector<NamespaceSymbol> NamespaceInfo::getAllSymbols() const {
    std::vector<NamespaceSymbol> result;
    for (const auto& [_, symbol] : symbols_) {
        result.push_back(symbol);
    }
    return result;
}

void NamespaceInfo::addChildNamespace(const std::string& child) {
    childNamespaces_.insert(child);
}

bool NamespaceInfo::hasChildNamespace(const std::string& child) const {
    return childNamespaces_.find(child) != childNamespaces_.end();
}

void NamespaceInfo::addConstraint(const std::string& constraint) {
    constraints_.insert(constraint);
}

bool NamespaceInfo::isConstraintViolated(const std::string& item) const {
    // 检查项是否在约束列表中
    return constraints_.find(item) != constraints_.end();
}

void NamespaceInfo::addMergeSource(const std::string& sourceFile) {
    if (std::find(mergeSources_.begin(), mergeSources_.end(), sourceFile) == mergeSources_.end()) {
        mergeSources_.push_back(sourceFile);
    }
}

// NamespaceManager 实现

void NamespaceManager::registerNamespace(const std::string& name, NamespaceType type, 
                                        const std::string& sourceFile) {
    if (!isValidNamespaceName(name)) {
        throw std::invalid_argument("Invalid namespace name: " + name);
    }
    
    auto it = namespaces_.find(name);
    if (it != namespaces_.end()) {
        // 命名空间已存在，需要合并
        it->second->addMergeSource(sourceFile);
        mergeNamespaces(name);
    } else {
        // 创建新命名空间
        auto nsInfo = std::make_shared<NamespaceInfo>(name, type, sourceFile);
        namespaces_[name] = nsInfo;
        
        // 处理嵌套命名空间
        size_t dotPos = name.find_last_of('.');
        if (dotPos != std::string::npos) {
            std::string parentName = name.substr(0, dotPos);
            std::string childName = name.substr(dotPos + 1);
            
            // 确保父命名空间存在
            if (namespaces_.find(parentName) == namespaces_.end()) {
                registerNamespace(parentName, NamespaceType::EXPLICIT, sourceFile);
            }
            
            // 添加子命名空间关系
            auto parent = namespaces_[parentName];
            parent->addChildNamespace(childName);
        }
    }
}

std::shared_ptr<NamespaceInfo> NamespaceManager::getNamespace(const std::string& name) const {
    auto it = namespaces_.find(name);
    if (it != namespaces_.end()) {
        return it->second;
    }
    return nullptr;
}

void NamespaceManager::mergeNamespaces(const std::string& name) {
    auto ns = getNamespace(name);
    if (!ns) return;
    
    // 标记为合并的命名空间
    if (ns->getType() != NamespaceType::MERGED) {
        // 创建新的合并命名空间
        auto mergedNs = std::make_shared<NamespaceInfo>(name, NamespaceType::MERGED, 
                                                        ns->getPrimarySourceFile());
        
        // 复制所有符号
        for (const auto& symbol : ns->getAllSymbols()) {
            mergedNs->addSymbol(symbol);
        }
        
        // 复制子命名空间
        for (const auto& child : ns->getChildNamespaces()) {
            mergedNs->addChildNamespace(child);
        }
        
        // 复制合并来源
        for (const auto& source : ns->getMergeSources()) {
            mergedNs->addMergeSource(source);
        }
        
        namespaces_[name] = mergedNs;
    }
}

void NamespaceManager::addSymbolToNamespace(const std::string& namespaceName, 
                                           const NamespaceSymbol& symbol) {
    auto ns = getNamespace(namespaceName);
    if (ns) {
        ns->addSymbol(symbol);
    }
}

std::optional<NamespaceSymbol> NamespaceManager::resolveSymbol(const std::string& symbolPath) const {
    auto [namespacePath, symbolName] = splitSymbolPath(symbolPath);
    
    if (namespacePath.empty()) {
        // 在所有命名空间中搜索
        for (const auto& [_, ns] : namespaces_) {
            auto symbol = ns->getSymbol(symbolName);
            if (symbol.has_value()) {
                return symbol;
            }
        }
    } else {
        // 在指定命名空间中搜索
        auto ns = getNamespace(namespacePath);
        if (ns) {
            return ns->getSymbol(symbolName);
        }
    }
    
    return std::nullopt;
}

std::vector<NamespaceManager::ConflictInfo> NamespaceManager::checkConflicts() const {
    std::vector<ConflictInfo> conflicts;
    
    // 检查每个命名空间中的符号冲突
    for (const auto& [nsName, ns] : namespaces_) {
        std::unordered_map<std::string, std::vector<NamespaceSymbol>> symbolMap;
        
        // 收集所有符号
        for (const auto& symbol : ns->getAllSymbols()) {
            symbolMap[symbol.name].push_back(symbol);
        }
        
        // 检查冲突
        for (const auto& [symbolName, symbols] : symbolMap) {
            if (symbols.size() > 1) {
                ConflictInfo conflict;
                conflict.namespaceName = nsName;
                conflict.symbolName = symbolName;
                
                // 收集冲突的文件
                std::unordered_set<std::string> files;
                for (const auto& sym : symbols) {
                    files.insert(sym.sourceFile);
                }
                
                conflict.conflictingFiles.assign(files.begin(), files.end());
                conflict.reason = "Symbol '" + symbolName + "' is defined in multiple files";
                
                conflicts.push_back(conflict);
            }
        }
    }
    
    // 检查命名空间本身的冲突（同名命名空间在不同文件中的不兼容定义）
    for (const auto& [nsName, ns] : namespaces_) {
        if (ns->getType() == NamespaceType::MERGED && ns->getMergeSources().size() > 1) {
            // 这里可以添加更复杂的冲突检测逻辑
            // 例如检查合并的命名空间是否有不兼容的约束等
        }
    }
    
    return conflicts;
}

std::string NamespaceManager::getDefaultNamespace(const std::string& filePath) const {
    // 首先检查是否有显式设置的默认命名空间
    auto it = fileToNamespace_.find(filePath);
    if (it != fileToNamespace_.end()) {
        return it->second;
    }
    
    // 否则使用文件名作为默认命名空间
    return extractFilenameAsNamespace(filePath);
}

void NamespaceManager::setDefaultNamespace(const std::string& filePath, 
                                          const std::string& namespaceName) {
    fileToNamespace_[filePath] = namespaceName;
}

bool NamespaceManager::checkConstraintViolation(const std::string& namespaceName, 
                                               const std::string& item) const {
    auto ns = getNamespace(namespaceName);
    if (ns) {
        return ns->isConstraintViolated(item);
    }
    return false;
}

std::vector<std::string> NamespaceManager::getAllNamespaces() const {
    std::vector<std::string> result;
    for (const auto& [name, _] : namespaces_) {
        result.push_back(name);
    }
    return result;
}

void NamespaceManager::clear() {
    namespaces_.clear();
    fileToNamespace_.clear();
}

std::pair<std::string, std::string> NamespaceManager::splitSymbolPath(const std::string& path) const {
    size_t lastDot = path.find_last_of('.');
    if (lastDot != std::string::npos) {
        return {path.substr(0, lastDot), path.substr(lastDot + 1)};
    }
    return {"", path};
}

bool NamespaceManager::isValidNamespaceName(const std::string& name) const {
    if (name.empty()) return false;
    
    // 检查命名空间名称的有效性
    // 只允许字母、数字、下划线和点
    for (char c : name) {
        if (!std::isalnum(c) && c != '_' && c != '.') {
            return false;
        }
    }
    
    // 不能以点开始或结束
    if (name.front() == '.' || name.back() == '.') {
        return false;
    }
    
    // 不能有连续的点
    if (name.find("..") != std::string::npos) {
        return false;
    }
    
    return true;
}

std::string NamespaceManager::extractFilenameAsNamespace(const std::string& filePath) const {
    fs::path path(filePath);
    return path.stem().string();
}

// NamespaceResolver 实现

std::shared_ptr<NamespaceInfo> NamespaceResolver::resolve(const std::string& path) const {
    // 处理嵌套命名空间路径
    std::string currentPath;
    std::shared_ptr<NamespaceInfo> result = nullptr;
    
    std::istringstream ss(path);
    std::string segment;
    
    while (std::getline(ss, segment, '.')) {
        if (!currentPath.empty()) {
            currentPath += ".";
        }
        currentPath += segment;
        
        result = manager_.getNamespace(currentPath);
        if (!result) {
            break;
        }
    }
    
    return result;
}

std::optional<NamespaceSymbol> NamespaceResolver::resolveSymbol(const std::string& symbol, 
                                                               const std::string& fromNamespace) const {
    if (fromNamespace.empty()) {
        return manager_.resolveSymbol(symbol);
    }
    
    // 先在指定命名空间中查找
    auto ns = manager_.getNamespace(fromNamespace);
    if (ns) {
        auto result = ns->getSymbol(symbol);
        if (result.has_value()) {
            return result;
        }
    }
    
    // 如果没找到，尝试在父命名空间中查找
    size_t dotPos = fromNamespace.find_last_of('.');
    if (dotPos != std::string::npos) {
        std::string parentNamespace = fromNamespace.substr(0, dotPos);
        return resolveSymbol(symbol, parentNamespace);
    }
    
    return std::nullopt;
}

std::string NamespaceResolver::buildFullPath(const std::string& namespacePath, 
                                            const std::string& symbol) const {
    if (namespacePath.empty()) {
        return symbol;
    }
    return namespacePath + "." + symbol;
}

} // namespace CHTL