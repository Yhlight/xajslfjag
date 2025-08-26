#include "GlobalMap.h"
#include <iostream>
#include <algorithm>
#include <filesystem>

namespace CHTL {

const std::string GlobalMap::OFFICIAL_MODULE_PREFIX = "chtl::";

GlobalMap::GlobalMap() : activeConfig("") {}

// 符号管理
void GlobalMap::addSymbol(const std::string& filePath, std::shared_ptr<SymbolInfo> symbol) {
    symbolTable[filePath].push_back(symbol);
    
    // 如果符号有命名空间，也添加到命名空间表
    if (!symbol->namespacePath.empty()) {
        addToNamespace(symbol->namespacePath, symbol);
    }
}

std::vector<std::shared_ptr<SymbolInfo>> GlobalMap::getSymbols(const std::string& filePath) const {
    auto it = symbolTable.find(filePath);
    if (it != symbolTable.end()) {
        return it->second;
    }
    return {};
}

std::shared_ptr<SymbolInfo> GlobalMap::findSymbol(const std::string& name, const std::string& namespacePath) const {
    // 先在指定命名空间中查找
    if (!namespacePath.empty()) {
        auto nsIt = namespaceTable.find(namespacePath);
        if (nsIt != namespaceTable.end()) {
            for (const auto& symbol : nsIt->second) {
                if (symbol->name == name) {
                    return symbol;
                }
            }
        }
    }
    
    // 在全局范围查找
    for (const auto& [path, symbols] : symbolTable) {
        for (const auto& symbol : symbols) {
            if (symbol->name == name && (namespacePath.empty() || symbol->namespacePath == namespacePath)) {
                return symbol;
            }
        }
    }
    
    return nullptr;
}

std::vector<std::shared_ptr<SymbolInfo>> GlobalMap::findSymbolsByType(SymbolType type) const {
    std::vector<std::shared_ptr<SymbolInfo>> result;
    
    for (const auto& [path, symbols] : symbolTable) {
        for (const auto& symbol : symbols) {
            if (symbol->type == type) {
                result.push_back(symbol);
            }
        }
    }
    
    return result;
}

// 命名空间管理
void GlobalMap::addToNamespace(const std::string& namespacePath, std::shared_ptr<SymbolInfo> symbol) {
    namespaceTable[namespacePath].push_back(symbol);
}

std::vector<std::shared_ptr<SymbolInfo>> GlobalMap::getNamespaceSymbols(const std::string& namespacePath) const {
    auto it = namespaceTable.find(namespacePath);
    if (it != namespaceTable.end()) {
        return it->second;
    }
    return {};
}

void GlobalMap::mergeNamespaces(const std::string& namespacePath1, const std::string& namespacePath2) {
    auto symbols2 = getNamespaceSymbols(namespacePath2);
    for (const auto& symbol : symbols2) {
        addToNamespace(namespacePath1, symbol);
    }
}

bool GlobalMap::namespaceExists(const std::string& namespacePath) const {
    return namespaceTable.find(namespacePath) != namespaceTable.end();
}

// 导入管理
void GlobalMap::addImport(const std::string& sourcePath, const ImportInfo& import) {
    importTable[sourcePath].push_back(import);
}

std::vector<ImportInfo> GlobalMap::getImports(const std::string& filePath) const {
    auto it = importTable.find(filePath);
    if (it != importTable.end()) {
        return it->second;
    }
    return {};
}

bool GlobalMap::checkCircularDependency(const std::string& sourcePath, const std::string& targetPath) {
    // 如果目标文件已在导入栈中，说明存在循环依赖
    if (importStack.find(targetPath) != importStack.end()) {
        return true;
    }
    
    // 将当前文件加入导入栈
    importStack.insert(sourcePath);
    
    // 递归检查目标文件的导入
    auto imports = getImports(targetPath);
    for (const auto& import : imports) {
        if (checkCircularDependency(targetPath, import.targetPath)) {
            importStack.erase(sourcePath);
            return true;
        }
    }
    
    // 从导入栈移除
    importStack.erase(sourcePath);
    return false;
}

void GlobalMap::resolveImports(const std::string& filePath) {
    // 解析文件的所有导入，合并符号到当前文件的符号表
    auto imports = getImports(filePath);
    for (const auto& import : imports) {
        auto importedSymbols = getSymbols(import.targetPath);
        for (const auto& symbol : importedSymbols) {
            // 复制符号并标记为导入
            auto importedSymbol = std::make_shared<SymbolInfo>(*symbol);
            importedSymbol->isImported = true;
            if (!import.symbols.empty() && 
                std::find(import.symbols.begin(), import.symbols.end(), symbol->name) == import.symbols.end()) {
                continue;  // 如果指定了导入符号列表，只导入列表中的符号
            }
            addSymbol(filePath, importedSymbol);
        }
    }
}

// 配置管理
void GlobalMap::addConfig(const std::string& name, const ConfigInfo& config) {
    configTable[name] = config;
    if (config.isDefault && activeConfig.empty()) {
        activeConfig = name;
    }
}

ConfigInfo GlobalMap::getConfig(const std::string& name) const {
    auto it = configTable.find(name);
    if (it != configTable.end()) {
        return it->second;
    }
    return ConfigInfo();
}

ConfigInfo GlobalMap::getActiveConfig() const {
    if (!activeConfig.empty()) {
        return getConfig(activeConfig);
    }
    
    // 如果没有活动配置，查找默认配置
    for (const auto& [name, config] : configTable) {
        if (config.isDefault) {
            return config;
        }
    }
    
    return ConfigInfo();
}

void GlobalMap::setActiveConfig(const std::string& name) {
    activeConfig = name;
}

bool GlobalMap::hasDefaultConfig() const {
    for (const auto& [name, config] : configTable) {
        if (config.isDefault) {
            return true;
        }
    }
    return false;
}

// 路径解析
std::string GlobalMap::resolveModulePath(const std::string& moduleName, const std::string& currentPath, bool isCJmod) const {
    namespace fs = std::filesystem;
    
    // 检查是否为官方模块
    bool isOfficial = isOfficialModule(moduleName);
    std::string cleanModuleName = isOfficial ? moduleName.substr(OFFICIAL_MODULE_PREFIX.length()) : moduleName;
    
    // 设置搜索的文件扩展名
    std::vector<std::string> extensions;
    if (isCJmod) {
        extensions = {".cjmod"};
    } else {
        extensions = {".cmod", ".chtl"};
    }
    
    // 搜索路径列表
    std::vector<fs::path> searchPaths;
    
    if (isOfficial) {
        // 只在官方模块目录搜索
        searchPaths.push_back(fs::path("module"));  // 编译器旁的module文件夹
    } else {
        // 按优先级添加搜索路径
        searchPaths.push_back(fs::path("module"));  // 官方模块目录
        searchPaths.push_back(fs::path(currentPath) / "module");  // 当前目录的module文件夹
        searchPaths.push_back(fs::path(currentPath));  // 当前目录
    }
    
    // 在每个搜索路径中查找文件
    for (const auto& searchPath : searchPaths) {
        // 检查是否为具体文件名（带后缀）
        if (moduleName.find('.') != std::string::npos) {
            fs::path fullPath = searchPath / cleanModuleName;
            if (fs::exists(fullPath)) {
                return fullPath.string();
            }
        } else {
            // 不带后缀，按优先级搜索
            for (const auto& ext : extensions) {
                fs::path fullPath = searchPath / (cleanModuleName + ext);
                if (fs::exists(fullPath)) {
                    return fullPath.string();
                }
            }
        }
        
        // 检查分类文件夹结构
        std::string subFolder = isCJmod ? "CJMOD" : "CMOD";
        fs::path categorizedPath = searchPath / subFolder;
        if (fs::exists(categorizedPath)) {
            if (moduleName.find('.') != std::string::npos) {
                fs::path fullPath = categorizedPath / cleanModuleName;
                if (fs::exists(fullPath)) {
                    return fullPath.string();
                }
            } else {
                for (const auto& ext : extensions) {
                    fs::path fullPath = categorizedPath / (cleanModuleName + ext);
                    if (fs::exists(fullPath)) {
                        return fullPath.string();
                    }
                }
            }
        }
    }
    
    return "";  // 未找到
}

std::string GlobalMap::resolveFilePath(const std::string& path, const std::string& currentPath) const {
    namespace fs = std::filesystem;
    
    // 如果是绝对路径，直接返回
    if (fs::path(path).is_absolute()) {
        return path;
    }
    
    // 相对路径，基于当前路径解析
    fs::path fullPath = fs::path(currentPath) / path;
    return fullPath.string();
}

bool GlobalMap::isOfficialModule(const std::string& moduleName) const {
    return moduleName.find(OFFICIAL_MODULE_PREFIX) == 0;
}

// 冲突检测
bool GlobalMap::hasSymbolConflict(const std::string& name, const std::string& namespacePath) const {
    int count = 0;
    
    if (namespacePath.empty()) {
        // 全局范围检查
        for (const auto& [path, symbols] : symbolTable) {
            for (const auto& symbol : symbols) {
                if (symbol->name == name && symbol->namespacePath.empty()) {
                    count++;
                    if (count > 1) return true;
                }
            }
        }
    } else {
        // 特定命名空间检查
        auto symbols = getNamespaceSymbols(namespacePath);
        for (const auto& symbol : symbols) {
            if (symbol->name == name) {
                count++;
                if (count > 1) return true;
            }
        }
    }
    
    return false;
}

std::vector<std::shared_ptr<SymbolInfo>> GlobalMap::getConflictingSymbols(const std::string& name) const {
    std::vector<std::shared_ptr<SymbolInfo>> conflicts;
    
    for (const auto& [path, symbols] : symbolTable) {
        for (const auto& symbol : symbols) {
            if (symbol->name == name) {
                conflicts.push_back(symbol);
            }
        }
    }
    
    return conflicts;
}

// 清理和重置
void GlobalMap::clear() {
    symbolTable.clear();
    namespaceTable.clear();
    importTable.clear();
    configTable.clear();
    activeConfig.clear();
    importStack.clear();
}

void GlobalMap::clearFile(const std::string& filePath) {
    symbolTable.erase(filePath);
    importTable.erase(filePath);
    
    // 从命名空间表中移除该文件的符号
    for (auto& [ns, symbols] : namespaceTable) {
        symbols.erase(
            std::remove_if(symbols.begin(), symbols.end(),
                [&filePath](const std::shared_ptr<SymbolInfo>& symbol) {
                    return symbol->filePath == filePath;
                }),
            symbols.end()
        );
    }
}

// 调试输出
void GlobalMap::dumpSymbolTable() const {
    std::cout << "=== Symbol Table ===" << std::endl;
    for (const auto& [path, symbols] : symbolTable) {
        std::cout << "File: " << path << std::endl;
        for (const auto& symbol : symbols) {
            std::cout << "  - " << symbol->name << " (type: " << static_cast<int>(symbol->type) 
                      << ", namespace: " << symbol->namespacePath << ")" << std::endl;
        }
    }
}

void GlobalMap::dumpNamespaceTable() const {
    std::cout << "=== Namespace Table ===" << std::endl;
    for (const auto& [ns, symbols] : namespaceTable) {
        std::cout << "Namespace: " << ns << std::endl;
        for (const auto& symbol : symbols) {
            std::cout << "  - " << symbol->name << " (type: " << static_cast<int>(symbol->type) << ")" << std::endl;
        }
    }
}

void GlobalMap::dumpImportTable() const {
    std::cout << "=== Import Table ===" << std::endl;
    for (const auto& [path, imports] : importTable) {
        std::cout << "File: " << path << std::endl;
        for (const auto& import : imports) {
            std::cout << "  - from: " << import.targetPath << " (namespace: " << import.namespaceName << ")" << std::endl;
        }
    }
}

} // namespace CHTL