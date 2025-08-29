#include "ImportEnhancer.h"
#include <iostream>
#include <algorithm>
#include <filesystem>

namespace CHTL {

// 全局实例
ImportEnhancer* g_importEnhancer = nullptr;
NamedOriginManager* g_namedOriginManager = nullptr;

// ========== ImportEnhancer 实现 ==========

ImportEnhancer::ImportEnhancer(const String& workDir, bool enableWildcard)
    : workingDirectory(workDir), enableWildcardResolution(enableWildcard), 
      enableRecursiveSearch(true), maxRecursionDepth(10) {
    
    // 添加默认搜索路径
    addSearchPath(".");
    addSearchPath("./src");
    addSearchPath("./components");
    addSearchPath("./modules");
}

PathResolutionResult ImportEnhancer::resolveImportPath(const String& importPath) const {
    PathResolutionResult result;
    
    // 检测通配符模式
    WildcardPatternType patternType = detectWildcardPattern(importPath);
    result.patternType = patternType;
    
    if (patternType == WildcardPatternType::NONE) {
        // 标准路径解析
        String resolved = ImportPathResolver::resolveStandardPath(importPath, workingDirectory);
        if (ImportPathResolver::fileExists(resolved)) {
            result.success = true;
            result.resolvedPaths.push_back(resolved);
        } else {
            result.success = false;
            result.errorMessage = "文件不存在: " + resolved;
        }
    } else {
        // 通配符路径解析
        return resolveWildcardPath(importPath);
    }
    
    return result;
}

PathResolutionResult ImportEnhancer::resolveWildcardPath(const String& wildcardPattern) const {
    PathResolutionResult result;
    
    if (!enableWildcardResolution) {
        result.success = false;
        result.errorMessage = "通配符解析已禁用";
        return result;
    }
    
    WildcardPatternType patternType = detectWildcardPattern(wildcardPattern);
    result.patternType = patternType;
    
    switch (patternType) {
        case WildcardPatternType::SIMPLE_WILDCARD:
            return resolveSimpleWildcard(wildcardPattern);
            
        case WildcardPatternType::PATH_WILDCARD:
        case WildcardPatternType::COMPLEX_WILDCARD:
            return resolveComplexWildcard(wildcardPattern);
            
        case WildcardPatternType::RECURSIVE_WILDCARD:
            return resolveRecursivePath(wildcardPattern);
            
        default:
            result.success = false;
            result.errorMessage = "无效的通配符模式: " + wildcardPattern;
            return result;
    }
}

PathResolutionResult ImportEnhancer::resolveRecursivePath(const String& recursivePattern) const {
    PathResolutionResult result;
    
    if (!enableRecursiveSearch) {
        result.success = false;
        result.errorMessage = "递归搜索已禁用";
        return result;
    }
    
    // 解析 **/*.chtl 模式
    String baseDir = extractDirectoryFromPattern(recursivePattern);
    String filenamePattern = extractFilenamePattern(recursivePattern);
    
    if (baseDir.empty()) {
        baseDir = workingDirectory;
    }
    
    StringVector matchedFiles = searchRecursively(filenamePattern, baseDir, 0);
    
    if (!matchedFiles.empty()) {
        result.success = true;
        result.resolvedPaths = matchedFiles;
    } else {
        result.success = false;
        result.errorMessage = "未找到匹配的文件: " + recursivePattern;
    }
    
    return result;
}

WildcardPatternType ImportEnhancer::detectWildcardPattern(const String& path) const {
    if (path.find("*") == String::npos) {
        return WildcardPatternType::NONE;
    }
    
    if (path.find("**") != String::npos) {
        return WildcardPatternType::RECURSIVE_WILDCARD;
    }
    
    if (path.find("/") != String::npos || path.find("\\") != String::npos) {
        return WildcardPatternType::PATH_WILDCARD;
    }
    
    if (path.find("*") != String::npos) {
        return WildcardPatternType::SIMPLE_WILDCARD;
    }
    
    return WildcardPatternType::COMPLEX_WILDCARD;
}

bool ImportEnhancer::isWildcardPath(const String& path) const {
    return path.find("*") != String::npos;
}

bool ImportEnhancer::isRecursivePattern(const String& path) const {
    return path.find("**") != String::npos;
}

bool ImportEnhancer::isValidWildcardPattern(const String& pattern) const {
    // 基础验证
    if (pattern.empty()) return false;
    
    // 检查是否有有效的文件扩展名
    if (pattern.find("*.") != String::npos) {
        return true;
    }
    
    // 检查路径格式
    return pattern.find("*") != String::npos;
}

StringVector ImportEnhancer::expandWildcard(const String& pattern, const String& baseDir) const {
    StringVector results;
    
    String searchDir = baseDir.empty() ? workingDirectory : baseDir;
    
    try {
        if (std::filesystem::exists(searchDir) && std::filesystem::is_directory(searchDir)) {
            for (const auto& entry : std::filesystem::directory_iterator(searchDir)) {
                if (entry.is_regular_file()) {
                    String filename = entry.path().filename().string();
                    if (matchesWildcardPattern(filename, pattern)) {
                        results.push_back(entry.path().string());
                    }
                }
            }
        }
    } catch (const std::filesystem::filesystem_error&) {
        // 目录访问错误，返回空结果
    }
    
    return results;
}

StringVector ImportEnhancer::findMatchingFiles(const String& pattern, const String& directory) const {
    return expandWildcard(pattern, directory);
}

StringVector ImportEnhancer::searchRecursively(const String& pattern, const String& startDir, size_t currentDepth) const {
    StringVector results;
    
    if (currentDepth >= maxRecursionDepth) {
        return results;
    }
    
    try {
        if (std::filesystem::exists(startDir) && std::filesystem::is_directory(startDir)) {
            for (const auto& entry : std::filesystem::recursive_directory_iterator(startDir)) {
                if (entry.is_regular_file()) {
                    String filename = entry.path().filename().string();
                    if (matchesWildcardPattern(filename, pattern)) {
                        results.push_back(entry.path().string());
                    }
                }
            }
        }
    } catch (const std::filesystem::filesystem_error&) {
        // 目录访问错误，返回空结果
    }
    
    return results;
}

void ImportEnhancer::registerNamedOrigin(const String& originType, const NamedOriginConfig& config) {
    namedOriginConfigs[config.aliasName] = config;
}

NamedOriginConfig ImportEnhancer::getNamedOriginConfig(const String& aliasName) const {
    auto it = namedOriginConfigs.find(aliasName);
    return (it != namedOriginConfigs.end()) ? it->second : NamedOriginConfig();
}

bool ImportEnhancer::hasNamedOriginConfig(const String& aliasName) const {
    return namedOriginConfigs.find(aliasName) != namedOriginConfigs.end();
}

std::pair<String, String> ImportEnhancer::parseAsClause(const String& asClause) const {
    // 解析 "Vue as CustomVue" 或 "@Vue as CustomVue"
    size_t asPos = asClause.find(" as ");
    if (asPos != String::npos) {
        String originType = asClause.substr(0, asPos);
        String aliasName = asClause.substr(asPos + 4);
        
        // 移除首尾空白
        originType.erase(0, originType.find_first_not_of(" \t\r\n"));
        originType.erase(originType.find_last_not_of(" \t\r\n") + 1);
        aliasName.erase(0, aliasName.find_first_not_of(" \t\r\n"));
        aliasName.erase(aliasName.find_last_not_of(" \t\r\n") + 1);
        
        return std::make_pair(originType, aliasName);
    }
    
    return std::make_pair("", "");
}

std::unique_ptr<ImportNode> ImportEnhancer::enhanceImportWithAs(std::unique_ptr<ImportNode> importNode, const String& asClause) const {
    if (!importNode || asClause.empty()) {
        return importNode;
    }
    
    auto [originType, aliasName] = parseAsClause(asClause);
    
    if (!originType.empty() && !aliasName.empty()) {
        importNode->setAlias(aliasName);
        
        // 注册命名原始嵌入配置
        NamedOriginConfig config(originType, aliasName);
        const_cast<ImportEnhancer*>(this)->registerNamedOrigin(originType, config);
        
        // 设置元数据
        importNode->importMetadata["named_origin_type"] = originType;
        importNode->importMetadata["alias_name"] = aliasName;
    }
    
    return importNode;
}

void ImportEnhancer::addSearchPath(const String& path) {
    if (std::find(searchPaths.begin(), searchPaths.end(), path) == searchPaths.end()) {
        searchPaths.push_back(path);
    }
}

void ImportEnhancer::removeSearchPath(const String& path) {
    searchPaths.erase(
        std::remove(searchPaths.begin(), searchPaths.end(), path),
        searchPaths.end()
    );
}

std::unique_ptr<ImportNode> ImportEnhancer::processEnhancedImport(const String& importType, const String& sourcePath, const String& asClause) const {
    // 创建基础导入节点
    auto importNode = std::make_unique<ImportNode>(NodeType::IMPORT, importType, sourcePath);
    
    // 解析路径
    auto pathResult = resolveImportPath(sourcePath);
    if (pathResult.success) {
        importNode->resolvedPath = pathResult.resolvedPaths.empty() ? sourcePath : pathResult.resolvedPaths[0];
        importNode->isResolved = true;
        
        // 如果是通配符导入，设置相关标志
        if (pathResult.patternType != WildcardPatternType::NONE) {
            importNode->setWildcardImport(true);
        }
    }
    
    // 处理as语法
    if (!asClause.empty()) {
        importNode = enhanceImportWithAs(std::move(importNode), asClause);
    }
    
    return importNode;
}

StringVector ImportEnhancer::processWildcardImports(const String& importType, const String& wildcardPath) const {
    auto pathResult = resolveWildcardPath(wildcardPath);
    return pathResult.success ? pathResult.resolvedPaths : StringVector{};
}

bool ImportEnhancer::validateImportPath(const String& path) const {
    if (path.empty()) return false;
    
    // 验证通配符模式
    if (isWildcardPath(path)) {
        return isValidWildcardPattern(path);
    }
    
    // 验证标准路径
    return ImportPathResolver::isValidImportPath(path);
}

bool ImportEnhancer::validateNamedOriginConfig(const NamedOriginConfig& config) const {
    return !config.originType.empty() && !config.aliasName.empty();
}

StringVector ImportEnhancer::getValidationErrors(const String& path) const {
    StringVector errors;
    
    if (!validateImportPath(path)) {
        errors.push_back("无效的导入路径: " + path);
    }
    
    if (isWildcardPath(path) && !enableWildcardResolution) {
        errors.push_back("通配符解析已禁用: " + path);
    }
    
    return errors;
}

void ImportEnhancer::printImportStats() const {
    std::cout << "=== Import Enhancer Statistics ===" << std::endl;
    std::cout << "Working Directory: " << workingDirectory << std::endl;
    std::cout << "Search Paths: " << searchPaths.size() << std::endl;
    for (const auto& path : searchPaths) {
        std::cout << "  " << path << std::endl;
    }
    std::cout << "Named Origin Configs: " << namedOriginConfigs.size() << std::endl;
    std::cout << "Wildcard Resolution: " << (enableWildcardResolution ? "Enabled" : "Disabled") << std::endl;
    std::cout << "Recursive Search: " << (enableRecursiveSearch ? "Enabled" : "Disabled") << std::endl;
    std::cout << "Max Recursion Depth: " << maxRecursionDepth << std::endl;
}

// 静态方法实现
bool ImportEnhancer::isValidFileExtension(const String& extension) {
    static const StringVector validExtensions = {
        ".chtl", ".html", ".css", ".js", ".json", ".md", ".vue", ".jsx", ".ts", ".tsx"
    };
    
    return std::find(validExtensions.begin(), validExtensions.end(), extension) != validExtensions.end();
}

String ImportEnhancer::extractFileExtension(const String& fileName) {
    size_t dotPos = fileName.find_last_of('.');
    return (dotPos != String::npos) ? fileName.substr(dotPos) : "";
}

String ImportEnhancer::normalizePath(const String& path) {
    String normalized = path;
    std::replace(normalized.begin(), normalized.end(), '\\', '/');
    return normalized;
}

String ImportEnhancer::resolvePath(const String& basePath, const String& relativePath) {
    return ImportPathResolver::joinPaths(basePath, relativePath);
}

bool ImportEnhancer::isRelativePath(const String& path) {
    return path.find("./") == 0 || path.find("../") == 0;
}

bool ImportEnhancer::isAbsolutePath(const String& path) {
    return path.front() == '/' || (path.length() > 2 && path[1] == ':');
}

// 私有方法实现
PathResolutionResult ImportEnhancer::resolveSimpleWildcard(const String& pattern) const {
    PathResolutionResult result;
    
    StringVector matchedFiles = expandWildcard(pattern, workingDirectory);
    
    if (!matchedFiles.empty()) {
        result.success = true;
        result.resolvedPaths = matchedFiles;
    } else {
        result.success = false;
        result.errorMessage = "未找到匹配的文件: " + pattern;
    }
    
    return result;
}

PathResolutionResult ImportEnhancer::resolveComplexWildcard(const String& pattern) const {
    PathResolutionResult result;
    
    String baseDir = extractDirectoryFromPattern(pattern);
    String filenamePattern = extractFilenamePattern(pattern);
    
    if (baseDir.empty()) {
        baseDir = workingDirectory;
    }
    
    StringVector matchedFiles = findMatchingFiles(filenamePattern, baseDir);
    
    if (!matchedFiles.empty()) {
        result.success = true;
        result.resolvedPaths = matchedFiles;
    } else {
        result.success = false;
        result.errorMessage = "未找到匹配的文件: " + pattern;
    }
    
    return result;
}

bool ImportEnhancer::matchesWildcardPattern(const String& filename, const String& pattern) const {
    // 简化的通配符匹配实现
    if (pattern == "*") return true;
    
    if (pattern.find("*.") == 0) {
        String extension = pattern.substr(1);
        return filename.length() >= extension.length() &&
               filename.substr(filename.length() - extension.length()) == extension;
    }
    
    // 更复杂的模式匹配可以使用正则表达式
    try {
        String regexPattern = pattern;
        std::replace(regexPattern.begin(), regexPattern.end(), '*', '.');
        regexPattern = ".*" + regexPattern + ".*";
        
        std::regex regex(regexPattern);
        return std::regex_match(filename, regex);
    } catch (const std::regex_error&) {
        return false;
    }
}

String ImportEnhancer::extractDirectoryFromPattern(const String& pattern) const {
    size_t lastSlash = pattern.find_last_of("/\\");
    return (lastSlash != String::npos) ? pattern.substr(0, lastSlash) : "";
}

String ImportEnhancer::extractFilenamePattern(const String& pattern) const {
    size_t lastSlash = pattern.find_last_of("/\\");
    return (lastSlash != String::npos) ? pattern.substr(lastSlash + 1) : pattern;
}

// ========== ImportPathResolver 实现 ==========

String ImportPathResolver::resolveStandardPath(const String& importPath, const String& currentFile) {
    if (isAbsolutePath(importPath)) {
        return getCanonicalPath(importPath);
    }
    
    if (isRelativePath(importPath)) {
        String baseDir = currentFile.empty() ? "." : getDirectoryFromPath(currentFile);
        return getCanonicalPath(joinPaths(baseDir, importPath));
    }
    
    // 尝试在当前目录查找
    return getCanonicalPath(importPath);
}

String ImportPathResolver::resolveRelativePath(const String& relativePath, const String& baseDir) {
    return getCanonicalPath(joinPaths(baseDir, relativePath));
}

String ImportPathResolver::resolveAbsolutePath(const String& absolutePath) {
    return getCanonicalPath(absolutePath);
}

bool ImportPathResolver::isValidImportPath(const String& path) {
    return !path.empty() && path.find_first_of("<>|") == String::npos;
}

bool ImportPathResolver::isValidFileType(const String& path, const String& expectedExtension) {
    String extension = getExtensionFromPath(path);
    return extension == expectedExtension;
}

bool ImportPathResolver::fileExists(const String& path) {
    try {
        return std::filesystem::exists(path) && std::filesystem::is_regular_file(path);
    } catch (const std::filesystem::filesystem_error&) {
        return false;
    }
}

bool ImportPathResolver::directoryExists(const String& path) {
    try {
        return std::filesystem::exists(path) && std::filesystem::is_directory(path);
    } catch (const std::filesystem::filesystem_error&) {
        return false;
    }
}

String ImportPathResolver::getDirectoryFromPath(const String& path) {
    size_t lastSlash = path.find_last_of("/\\");
    return (lastSlash != String::npos) ? path.substr(0, lastSlash) : ".";
}

String ImportPathResolver::getFilenameFromPath(const String& path) {
    size_t lastSlash = path.find_last_of("/\\");
    return (lastSlash != String::npos) ? path.substr(lastSlash + 1) : path;
}

String ImportPathResolver::getExtensionFromPath(const String& path) {
    String filename = getFilenameFromPath(path);
    size_t dotPos = filename.find_last_of('.');
    return (dotPos != String::npos) ? filename.substr(dotPos) : "";
}

String ImportPathResolver::joinPaths(const String& basePath, const String& relativePath) {
    if (basePath.empty()) return relativePath;
    if (relativePath.empty()) return basePath;
    
    String result = basePath;
    if (result.back() != '/' && result.back() != '\\') {
        result += "/";
    }
    
    String cleanRelativePath = relativePath;
    if (cleanRelativePath.front() == '/' || cleanRelativePath.front() == '\\') {
        cleanRelativePath = cleanRelativePath.substr(1);
    }
    
    return result + cleanRelativePath;
}

String ImportPathResolver::getCanonicalPath(const String& path) {
    try {
        return std::filesystem::canonical(path).string();
    } catch (const std::filesystem::filesystem_error&) {
        return path; // 如果无法规范化，返回原路径
    }
}

// ========== NamedOriginManager 实现 ==========

NamedOriginManager::NamedOriginManager() {
    setupDefaultNamedOrigins();
}

void NamedOriginManager::registerNamedOrigin(const String& aliasName, const NamedOriginConfig& config) {
    if (validateNamedOrigin(config)) {
        namedOrigins[aliasName] = config;
        updateAliasMapping(aliasName, config.originType);
    }
}

void NamedOriginManager::unregisterNamedOrigin(const String& aliasName) {
    namedOrigins.erase(aliasName);
    removeAliasMapping(aliasName);
}

NamedOriginConfig NamedOriginManager::getNamedOrigin(const String& aliasName) const {
    auto it = namedOrigins.find(aliasName);
    return (it != namedOrigins.end()) ? it->second : NamedOriginConfig();
}

bool NamedOriginManager::hasNamedOrigin(const String& aliasName) const {
    return namedOrigins.find(aliasName) != namedOrigins.end();
}

String NamedOriginManager::getOriginalType(const String& aliasName) const {
    auto it = aliasToTypeMap.find(aliasName);
    return (it != aliasToTypeMap.end()) ? it->second : "";
}

NamedOriginConfig NamedOriginManager::parseNamedOriginFromAs(const String& originType, const String& asClause) const {
    size_t asPos = asClause.find(" as ");
    if (asPos != String::npos) {
        String aliasName = asClause.substr(asPos + 4);
        aliasName.erase(0, aliasName.find_first_not_of(" \t\r\n"));
        aliasName.erase(aliasName.find_last_not_of(" \t\r\n") + 1);
        
        return NamedOriginConfig(originType, aliasName);
    }
    
    return NamedOriginConfig();
}

std::unique_ptr<ImportNode> NamedOriginManager::createNamedOriginImport(const String& originType, const String& sourcePath, const String& aliasName) const {
    auto importNode = std::make_unique<ImportNode>(NodeType::IMPORT_ORIGIN, originType, sourcePath);
    importNode->setAlias(aliasName);
    
    // 设置元数据
    importNode->importMetadata["named_origin_type"] = originType;
    importNode->importMetadata["alias_name"] = aliasName;
    
    return importNode;
}

bool NamedOriginManager::validateNamedOrigin(const NamedOriginConfig& config) const {
    return isValidOriginType(config.originType) && isValidAliasName(config.aliasName);
}

void NamedOriginManager::setupDefaultNamedOrigins() {
    registerNamedOrigin("CustomVue", createDefaultNamedOrigin("Vue", "CustomVue"));
    registerNamedOrigin("MD", createDefaultNamedOrigin("Markdown", "MD"));
    registerNamedOrigin("CustomReact", createDefaultNamedOrigin("React", "CustomReact"));
}

NamedOriginConfig NamedOriginManager::createDefaultNamedOrigin(const String& originType, const String& aliasName) {
    NamedOriginConfig config(originType, aliasName);
    config.isCustomType = true;
    return config;
}

bool NamedOriginManager::isValidAliasName(const String& aliasName) const {
    return !aliasName.empty() && aliasName.find_first_of(" \t\r\n[]{}()") == String::npos;
}

bool NamedOriginManager::isValidOriginType(const String& originType) const {
    return !originType.empty() && originType.find_first_of(" \t\r\n[]{}()") == String::npos;
}

void NamedOriginManager::updateAliasMapping(const String& aliasName, const String& originType) {
    aliasToTypeMap[aliasName] = originType;
}

void NamedOriginManager::removeAliasMapping(const String& aliasName) {
    aliasToTypeMap.erase(aliasName);
}

// ========== 全局函数实现 ==========

void initializeImportEnhancer(const String& workingDirectory) {
    if (!g_importEnhancer) {
        g_importEnhancer = new ImportEnhancer(workingDirectory);
    }
    
    if (!g_namedOriginManager) {
        g_namedOriginManager = new NamedOriginManager();
    }
}

void shutdownImportEnhancer() {
    delete g_importEnhancer;
    g_importEnhancer = nullptr;
    
    delete g_namedOriginManager;
    g_namedOriginManager = nullptr;
}

ImportEnhancer& getGlobalImportEnhancer() {
    if (!g_importEnhancer) {
        initializeImportEnhancer();
    }
    return *g_importEnhancer;
}

NamedOriginManager& getGlobalNamedOriginManager() {
    if (!g_namedOriginManager) {
        initializeImportEnhancer();
    }
    return *g_namedOriginManager;
}

} // namespace CHTL