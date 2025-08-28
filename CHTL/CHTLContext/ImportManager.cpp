#include "ImportManager.h"
#include <algorithm>
#include <sstream>
#include <stdexcept>
#include <filesystem>

namespace CHTL {

ImportManager::ImportManager(std::shared_ptr<ConfigurationManager> configManager, const std::string& basePath)
    : configManager_(configManager), basePath_(basePath) {
    // 初始化默认搜索路径
    if (!basePath_.empty()) {
        addSearchPath(basePath_);
    }
    
    // 添加当前目录
    addSearchPath(".");
}

void ImportManager::addImport(const ImportInfo& import) {
    if (validateImport(import)) {
        imports_.push_back(import);
        statistics_.totalImports++;
        statistics_.successfulImports++;
    } else {
        statistics_.totalImports++;
        statistics_.failedImports++;
    }
}

std::vector<ImportInfo> ImportManager::getImports() const {
    return imports_;
}

bool ImportManager::removeImport(const ImportInfo& import) {
    auto it = std::find_if(imports_.begin(), imports_.end(),
                           [&import](const ImportInfo& existing) {
                               return existing.type == import.type && 
                                      existing.path == import.path &&
                                      existing.alias == import.alias;
                           });
    
    if (it != imports_.end()) {
        imports_.erase(it);
        return true;
    }
    
    return false;
}

PathResolutionResult ImportManager::resolvePath(const std::string& path, ImportType type) {
    PathResolutionResult result;
    
    if (path.empty()) {
        result.error = "Empty path";
        return result;
    }
    
    // 检查别名
    std::string resolvedPath = resolveAlias(path);
    
    // 检查搜索路径
    for (const auto& searchPath : searchPaths_) {
        std::string fullPath = searchPath + "/" + resolvedPath;
        if (fileExists(fullPath)) {
            result.resolvedPaths.push_back(fullPath);
        }
    }
    
    if (result.resolvedPaths.empty()) {
        result.error = "Path not found: " + path;
    } else {
        result.success = true;
    }
    
    return result;
}

std::vector<std::string> ImportManager::resolveWildcardPath(const std::string& path, ImportType type) {
    if (!isWildcardPattern(path)) {
        return {path};
    }
    
    std::vector<std::string> results;
    
    for (const auto& searchPath : searchPaths_) {
        auto wildcardResults = expandWildcard(path, type);
        for (const auto& result : wildcardResults) {
            std::string fullPath = searchPath + "/" + result;
            if (fileExists(fullPath)) {
                results.push_back(fullPath);
            }
        }
    }
    
    return results;
}

std::vector<std::string> ImportManager::resolveSubModulePath(const std::string& path, ImportType type) {
    return parseSubModulePath(path);
}

bool ImportManager::checkCircularDependency(const std::string& filePath) {
    std::unordered_set<std::string> visited;
    std::unordered_set<std::string> recursionStack;
    return hasCircularDependencyDFS(filePath, visited, recursionStack);
}

std::vector<std::string> ImportManager::getCircularDependencies(const std::string& filePath) {
    // 简化实现：返回空列表
    return {};
}

bool ImportManager::checkDuplicateImport(const std::string& type, const std::string& path) {
    for (const auto& import : imports_) {
        if (static_cast<int>(import.type) == std::stoi(type) && import.path == path) {
            return true;
        }
    }
    return false;
}

std::vector<ImportInfo> ImportManager::getDuplicateImports() const {
    std::vector<ImportInfo> duplicates;
    std::unordered_map<std::string, std::vector<ImportInfo>> typePathMap;
    
    for (const auto& import : imports_) {
        std::string key = std::to_string(static_cast<int>(import.type)) + ":" + import.path;
        typePathMap[key].push_back(import);
    }
    
    for (const auto& [key, importList] : typePathMap) {
        if (importList.size() > 1) {
            duplicates.insert(duplicates.end(), importList.begin(), importList.end());
        }
    }
    
    return duplicates;
}

void ImportManager::addSearchPath(const std::string& path) {
    if (std::find(searchPaths_.begin(), searchPaths_.end(), path) == searchPaths_.end()) {
        searchPaths_.push_back(path);
    }
}

void ImportManager::removeSearchPath(const std::string& path) {
    auto it = std::find(searchPaths_.begin(), searchPaths_.end(), path);
    if (it != searchPaths_.end()) {
        searchPaths_.erase(it);
    }
}

std::vector<std::string> ImportManager::getSearchPaths() const {
    return searchPaths_;
}

std::vector<std::string> ImportManager::searchOfficialModules(const std::string& name, ImportType type) {
    std::string officialPath = getOfficialModulePath();
    if (officialPath.empty()) {
        return {};
    }
    
    return searchInModuleDirectory(officialPath, name, type);
}

std::vector<std::string> ImportManager::searchLocalModules(const std::string& name, ImportType type) {
    std::string localPath = getLocalModulePath();
    if (localPath.empty()) {
        return {};
    }
    
    return searchInModuleDirectory(localPath, name, type);
}

std::vector<std::string> ImportManager::searchCurrentDirectory(const std::string& name, ImportType type) {
    return searchInDirectory(".", name, type);
}

bool ImportManager::isValidFileForType(const std::string& filePath, ImportType type) {
    std::string extension = getExtensionFromPath(filePath);
    auto validExtensions = getValidExtensionsForType(type);
    
    return std::find(validExtensions.begin(), validExtensions.end(), extension) != validExtensions.end();
}

std::vector<std::string> ImportManager::getValidExtensionsForType(ImportType type) {
    switch (type) {
        case ImportType::HTML:
            return {".html", ".htm", ".xhtml"};
        case ImportType::STYLE:
            return {".css", ".scss", ".sass", ".less"};
        case ImportType::JAVASCRIPT:
            return {".js", ".mjs", ".ts", ".jsx", ".tsx"};
        case ImportType::CHTL:
            return {".chtl", ".cht"};
        case ImportType::CJMOD:
            return {".cjs", ".cmod"};
        case ImportType::CONFIG:
            return {".ini", ".conf", ".config", ".json"};
        default:
            return {};
    }
}

std::vector<std::string> ImportManager::expandWildcard(const std::string& pattern, ImportType type) {
    if (!isWildcardPattern(pattern)) {
        return {pattern};
    }
    
    std::vector<std::string> results;
    
    for (const auto& searchPath : searchPaths_) {
        auto files = listFiles(searchPath, pattern);
        results.insert(results.end(), files.begin(), files.end());
    }
    
    return results;
}

bool ImportManager::isWildcardPattern(const std::string& pattern) {
    return pattern.find('*') != std::string::npos || pattern.find('?') != std::string::npos;
}

std::vector<std::string> ImportManager::parseSubModulePath(const std::string& path) {
    std::vector<std::string> components;
    std::istringstream iss(path);
    std::string component;
    
    while (std::getline(iss, component, '.')) {
        if (!component.empty()) {
            components.push_back(component);
        }
    }
    
    return components;
}

std::string ImportManager::normalizeSubModulePath(const std::string& path) {
    auto components = parseSubModulePath(path);
    std::ostringstream oss;
    
    for (size_t i = 0; i < components.size(); ++i) {
        if (i > 0) {
            oss << ".";
        }
        oss << components[i];
    }
    
    return oss.str();
}

void ImportManager::addAlias(const std::string& alias, const std::string& path) {
    aliases_[alias] = path;
}

std::string ImportManager::resolveAlias(const std::string& alias) {
    auto it = aliases_.find(alias);
    if (it != aliases_.end()) {
        return it->second;
    }
    return alias;
}

bool ImportManager::hasAlias(const std::string& alias) const {
    return aliases_.count(alias) > 0;
}

bool ImportManager::validateImport(const ImportInfo& import) {
    if (import.path.empty()) {
        return false;
    }
    
    // 检查文件是否存在
    if (!import.isWildcard) {
        bool found = false;
        for (const auto& searchPath : searchPaths_) {
            std::string fullPath = searchPath + "/" + import.path;
            if (fileExists(fullPath)) {
                found = true;
                break;
            }
        }
        if (!found) {
            return false;
        }
    }
    
    // 检查文件类型是否匹配
    if (!import.isWildcard) {
        return isValidFileForType(import.path, import.type);
    }
    
    return true;
}

std::vector<std::string> ImportManager::getImportErrors(const ImportInfo& import) {
    std::vector<std::string> errors;
    
    if (import.path.empty()) {
        errors.push_back("Empty import path");
    }
    
    if (!import.isWildcard) {
        bool found = false;
        for (const auto& searchPath : searchPaths_) {
            std::string fullPath = searchPath + "/" + import.path;
            if (fileExists(fullPath)) {
                found = true;
                break;
            }
        }
        if (!found) {
            errors.push_back("Import file not found: " + import.path);
        }
        
        if (!isValidFileForType(import.path, import.type)) {
            errors.push_back("Invalid file type for import: " + import.path);
        }
    }
    
    return errors;
}

void ImportManager::clearStatistics() {
    statistics_ = ImportStatistics();
}

std::string ImportManager::getDebugInfo() const {
    std::ostringstream oss;
    oss << "ImportManager Debug Info:\n";
    oss << "Total imports: " << statistics_.totalImports << "\n";
    oss << "Successful imports: " << statistics_.successfulImports << "\n";
    oss << "Failed imports: " << statistics_.failedImports << "\n";
    oss << "Circular dependencies: " << statistics_.circularDependencies << "\n";
    oss << "Duplicate imports: " << statistics_.duplicateImports << "\n";
    oss << "Base path: " << (basePath_.empty() ? "(none)" : basePath_) << "\n";
    oss << "Search paths: " << searchPaths_.size() << "\n";
    oss << "Aliases: " << aliases_.size() << "\n";
    
    return oss.str();
}

std::string ImportManager::getImportInfo() const {
    std::ostringstream oss;
    oss << "Import Information:\n";
    
    for (const auto& import : imports_) {
        oss << "  Type: " << static_cast<int>(import.type) << ", Path: " << import.path;
        if (!import.alias.empty()) {
            oss << ", Alias: " << import.alias;
        }
        oss << "\n";
    }
    
    return oss.str();
}

void ImportManager::reset() {
    imports_.clear();
    searchPaths_.clear();
    aliases_.clear();
    processedFiles_.clear();
    clearStatistics();
    
    // 重新添加默认搜索路径
    if (!basePath_.empty()) {
        addSearchPath(basePath_);
    }
    addSearchPath(".");
}

std::string ImportManager::normalizePath(const std::string& path) {
    std::filesystem::path fsPath(path);
    return fsPath.lexically_normal().string();
}

std::string ImportManager::getDirectoryFromPath(const std::string& path) {
    std::filesystem::path fsPath(path);
    return fsPath.parent_path().string();
}

std::string ImportManager::getFileNameFromPath(const std::string& path) {
    std::filesystem::path fsPath(path);
    return fsPath.filename().string();
}

std::string ImportManager::getExtensionFromPath(const std::string& path) {
    std::filesystem::path fsPath(path);
    return fsPath.extension().string();
}

std::vector<std::string> ImportManager::searchInDirectory(const std::string& directory, const std::string& pattern, ImportType type) {
    if (!isDirectory(directory)) {
        return {};
    }
    
    return listFiles(directory, pattern);
}

std::vector<std::string> ImportManager::searchInModuleDirectory(const std::string& modulePath, const std::string& name, ImportType type) {
    if (!isDirectory(modulePath)) {
        return {};
    }
    
    std::vector<std::string> results;
    auto files = listFiles(modulePath, "*");
    
    for (const auto& file : files) {
        if (file.find(name) != std::string::npos && isValidFileForType(file, type)) {
            results.push_back(file);
        }
    }
    
    return results;
}

void ImportManager::buildDependencyGraph() {
    // 简化实现：不执行任何操作
}

bool ImportManager::hasCircularDependencyDFS(const std::string& file, std::unordered_set<std::string>& visited, 
                                            std::unordered_set<std::string>& recursionStack) {
    if (recursionStack.count(file) > 0) {
        return true;
    }
    
    if (visited.count(file) > 0) {
        return false;
    }
    
    visited.insert(file);
    recursionStack.insert(file);
    
    // 简化实现：总是返回 false
    recursionStack.erase(file);
    return false;
}

bool ImportManager::fileExists(const std::string& path) {
    return std::filesystem::exists(path);
}

bool ImportManager::isDirectory(const std::string& path) {
    return std::filesystem::is_directory(path);
}

std::vector<std::string> ImportManager::listFiles(const std::string& directory, const std::string& pattern) {
    std::vector<std::string> files;
    
    if (!isDirectory(directory)) {
        return files;
    }
    
    try {
        for (const auto& entry : std::filesystem::directory_iterator(directory)) {
            if (entry.is_regular_file()) {
                std::string fileName = entry.path().filename().string();
                // 简化模式匹配：只检查是否包含模式
                if (pattern == "*" || fileName.find(pattern) != std::string::npos) {
                    files.push_back(fileName);
                }
            }
        }
    } catch (const std::exception&) {
        // 忽略错误
    }
    
    return files;
}

void ImportManager::updateStatistics(const std::string& type, size_t value) {
    if (type == "import") {
        statistics_.totalImports += value;
    } else if (type == "success") {
        statistics_.successfulImports += value;
    } else if (type == "failure") {
        statistics_.failedImports += value;
    } else if (type == "circular") {
        statistics_.circularDependencies += value;
    } else if (type == "duplicate") {
        statistics_.duplicateImports += value;
    }
}

std::string ImportManager::getOfficialModulePath() const {
    if (!configManager_) {
        return "";
    }
    
    auto config = configManager_->getConfig("", "OFFICIAL_MODULE_PATH");
    if (config.type == ConfigItemType::STRING) {
        return config.get<std::string>();
    }
    
    return "";
}

std::string ImportManager::getLocalModulePath() const {
    if (!configManager_) {
        return "";
    }
    
    auto config = configManager_->getConfig("", "LOCAL_MODULE_PATH");
    if (config.type == ConfigItemType::STRING) {
        return config.get<std::string>();
    }
    
    return "";
}

bool ImportManager::isOfficialModulePrefix(const std::string& path) const {
    return path.find("@") == 0 || path.find("chtholly.") == 0;
}

} // namespace CHTL