#include "GlobalMap.h"
#include <algorithm>
#include <queue>
#include <sstream>

namespace CHTL {

void GlobalMap::registerTemplate(const std::string& name, TemplateInfo::TemplateKind kind, 
                                const std::string& file) {
    auto info = std::make_shared<TemplateInfo>(name, kind, file);
    symbols_[name] = info;
}

void GlobalMap::registerCustom(const std::string& name, CustomInfo::CustomKind kind,
                              const std::string& file) {
    auto info = std::make_shared<CustomInfo>(name, kind, file);
    symbols_[name] = info;
}

void GlobalMap::registerOrigin(const std::string& name, const std::string& type,
                              const std::string& file) {
    auto info = std::make_shared<OriginInfo>(name, type, file);
    std::string fullName = type + "_" + name;  // 使用类型前缀避免命名冲突
    symbols_[fullName] = info;
}

void GlobalMap::registerNamespace(const std::string& name, const std::string& file) {
    auto info = std::make_shared<NamespaceInfo>(name, file);
    namespaces_[name] = info;
    
    // 如果包含嵌套命名空间，处理父子关系
    size_t dotPos = name.find_last_of('.');
    if (dotPos != std::string::npos) {
        std::string parentNs = name.substr(0, dotPos);
        std::string childName = name.substr(dotPos + 1);
        
        auto parentIt = namespaces_.find(parentNs);
        if (parentIt != namespaces_.end()) {
            parentIt->second->addChildNamespace(childName);
        }
    }
}

void GlobalMap::registerConfiguration(const std::string& name, const std::string& file) {
    auto info = std::make_shared<ConfigurationInfo>(name, file);
    std::string configName = name.empty() ? "__default_config__" : name;
    symbols_[configName] = info;
}

std::shared_ptr<SymbolInfo> GlobalMap::lookupSymbol(const std::string& name) const {
    // 首先检查别名
    auto aliasIt = aliasMap_.find(name);
    std::string lookupName = (aliasIt != aliasMap_.end()) ? aliasIt->second : name;
    
    auto it = symbols_.find(lookupName);
    if (it != symbols_.end()) {
        return it->second;
    }
    return nullptr;
}

std::shared_ptr<SymbolInfo> GlobalMap::lookupSymbol(const std::string& name, 
                                                    const std::string& namespacePath) const {
    // 在指定命名空间中查找符号
    auto nsIt = namespaces_.find(namespacePath);
    if (nsIt != namespaces_.end()) {
        auto symbolType = nsIt->second->getSymbol(name);
        if (symbolType.has_value()) {
            // 构造完整名称并查找
            std::string fullName = namespacePath + "." + name;
            return lookupSymbol(fullName);
        }
    }
    
    // 如果在命名空间中找不到，尝试全局查找
    return lookupSymbol(name);
}

std::string GlobalMap::getDefaultNamespace(const std::string& file) const {
    auto it = fileNamespaces_.find(file);
    if (it != fileNamespaces_.end()) {
        return it->second;
    }
    
    // 如果没有显式命名空间，使用文件名作为默认命名空间
    size_t lastSlash = file.find_last_of("/\\");
    size_t lastDot = file.find_last_of('.');
    
    if (lastDot != std::string::npos && (lastSlash == std::string::npos || lastDot > lastSlash)) {
        std::string filename = (lastSlash != std::string::npos) 
            ? file.substr(lastSlash + 1, lastDot - lastSlash - 1)
            : file.substr(0, lastDot);
        return filename;
    }
    
    return file;
}

void GlobalMap::setDefaultNamespace(const std::string& file, const std::string& ns) {
    fileNamespaces_[file] = ns;
}

void GlobalMap::addImportedSymbol(const std::string& symbol, const std::string& fromFile,
                                 const std::string& toFile, const std::string& alias) {
    // 添加导入关系
    importGraph_[toFile].insert(fromFile);
    
    // 如果有别名，添加到别名映射
    if (!alias.empty()) {
        aliasMap_[alias] = symbol;
    }
}

bool GlobalMap::hasCircularDependency(const std::string& file1, const std::string& file2) const {
    // 使用BFS检测循环依赖
    std::unordered_set<std::string> visited;
    std::queue<std::string> toVisit;
    
    toVisit.push(file2);
    
    while (!toVisit.empty()) {
        std::string current = toVisit.front();
        toVisit.pop();
        
        if (current == file1) {
            return true;  // 找到循环依赖
        }
        
        if (visited.find(current) != visited.end()) {
            continue;  // 已访问过
        }
        
        visited.insert(current);
        
        // 添加当前文件导入的所有文件
        auto it = importGraph_.find(current);
        if (it != importGraph_.end()) {
            for (const auto& imported : it->second) {
                toVisit.push(imported);
            }
        }
    }
    
    return false;
}

void GlobalMap::clear() {
    symbols_.clear();
    namespaces_.clear();
    fileNamespaces_.clear();
    importGraph_.clear();
    aliasMap_.clear();
}

std::vector<std::string> GlobalMap::getAllSymbols() const {
    std::vector<std::string> result;
    
    for (const auto& [name, info] : symbols_) {
        std::stringstream ss;
        ss << name << " (" << static_cast<int>(info->getType()) << ") from " << info->getSourceFile();
        result.push_back(ss.str());
    }
    
    return result;
}

} // namespace CHTL