#pragma once

#include "../Util/Common.h"
#include "../CHTLNode/BaseNode.h"
#include "../CHTLNode/ImportNode.h"
#include <regex>
#include <filesystem>

namespace CHTL {

// 通配符模式类型
enum class WildcardPatternType {
    NONE,               // 无通配符
    SIMPLE_WILDCARD,    // 简单通配符 *.chtl
    PATH_WILDCARD,      // 路径通配符 ./components/*.chtl
    RECURSIVE_WILDCARD, // 递归通配符 **/*.chtl
    COMPLEX_WILDCARD    // 复杂通配符 components/**/*.chtl
};

// 路径解析结果
struct PathResolutionResult {
    bool success;
    StringVector resolvedPaths;
    String errorMessage;
    WildcardPatternType patternType;
    
    PathResolutionResult(bool ok = false) : success(ok), patternType(WildcardPatternType::NONE) {}
};

// 带名原始嵌入配置
struct NamedOriginConfig {
    String originType;          // 原始类型 (Vue, Markdown等)
    String aliasName;           // 别名
    String implementation;      // 实现内容
    bool isCustomType;         // 是否为自定义类型
    StringUnorderedMap properties; // 额外属性
    
    NamedOriginConfig(const String& type = "", const String& alias = "")
        : originType(type), aliasName(alias), isCustomType(true) {}
};

// Import增强器
class ImportEnhancer {
private:
    String workingDirectory;
    StringVector searchPaths;
    std::unordered_map<String, NamedOriginConfig> namedOriginConfigs;
    bool enableWildcardResolution;
    bool enableRecursiveSearch;
    size_t maxRecursionDepth;
    
public:
    ImportEnhancer(const String& workDir = ".", bool enableWildcard = true);
    ~ImportEnhancer() = default;
    
    // 路径解析
    PathResolutionResult resolveImportPath(const String& importPath) const;
    PathResolutionResult resolveWildcardPath(const String& wildcardPattern) const;
    PathResolutionResult resolveRecursivePath(const String& recursivePattern) const;
    
    // 通配符模式检测
    WildcardPatternType detectWildcardPattern(const String& path) const;
    bool isWildcardPath(const String& path) const;
    bool isRecursivePattern(const String& path) const;
    bool isValidWildcardPattern(const String& pattern) const;
    
    // 文件系统操作
    StringVector expandWildcard(const String& pattern, const String& baseDir = "") const;
    StringVector findMatchingFiles(const String& pattern, const String& directory) const;
    StringVector searchRecursively(const String& pattern, const String& startDir, size_t currentDepth = 0) const;
    
    // 带名原始嵌入支持
    void registerNamedOrigin(const String& originType, const NamedOriginConfig& config);
    NamedOriginConfig getNamedOriginConfig(const String& aliasName) const;
    bool hasNamedOriginConfig(const String& aliasName) const;
    
    // as语法增强解析
    std::pair<String, String> parseAsClause(const String& asClause) const;
    std::unique_ptr<ImportNode> enhanceImportWithAs(std::unique_ptr<ImportNode> importNode, const String& asClause) const;
    
    // 搜索路径管理
    void addSearchPath(const String& path);
    void removeSearchPath(const String& path);
    StringVector getSearchPaths() const { return searchPaths; }
    void clearSearchPaths() { searchPaths.clear(); }
    
    // 配置
    void setWorkingDirectory(const String& workDir) { workingDirectory = workDir; }
    String getWorkingDirectory() const { return workingDirectory; }
    
    void setEnableWildcardResolution(bool enable) { enableWildcardResolution = enable; }
    bool isWildcardResolutionEnabled() const { return enableWildcardResolution; }
    
    void setEnableRecursiveSearch(bool enable) { enableRecursiveSearch = enable; }
    bool isRecursiveSearchEnabled() const { return enableRecursiveSearch; }
    
    void setMaxRecursionDepth(size_t depth) { maxRecursionDepth = depth; }
    size_t getMaxRecursionDepth() const { return maxRecursionDepth; }
    
    // 增强导入节点处理
    std::unique_ptr<ImportNode> processEnhancedImport(const String& importType, const String& sourcePath, const String& asClause = "") const;
    StringVector processWildcardImports(const String& importType, const String& wildcardPath) const;
    
    // 验证和错误处理
    bool validateImportPath(const String& path) const;
    bool validateNamedOriginConfig(const NamedOriginConfig& config) const;
    StringVector getValidationErrors(const String& path) const;
    
    // 调试和统计
    void printImportStats() const;
    size_t getResolvedPathCount() const;
    StringVector getFailedResolutions() const;
    
    // 静态辅助方法
    static bool isValidFileExtension(const String& extension);
    static String extractFileExtension(const String& fileName);
    static String normalizePath(const String& path);
    static String resolvePath(const String& basePath, const String& relativePath);
    static bool isRelativePath(const String& path);
    static bool isAbsolutePath(const String& path);
    
private:
    // 内部辅助方法
    PathResolutionResult resolveSimpleWildcard(const String& pattern) const;
    PathResolutionResult resolveComplexWildcard(const String& pattern) const;
    StringVector matchFilesInDirectory(const String& directory, const String& pattern) const;
    bool matchesWildcardPattern(const String& filename, const String& pattern) const;
    String extractDirectoryFromPattern(const String& pattern) const;
    String extractFilenamePattern(const String& pattern) const;
    void collectFilesRecursively(const String& directory, const String& pattern, StringVector& results, size_t currentDepth) const;
};

// Import路径解析器
class ImportPathResolver {
public:
    // 标准路径解析
    static String resolveStandardPath(const String& importPath, const String& currentFile = "");
    static String resolveRelativePath(const String& relativePath, const String& baseDir);
    static String resolveAbsolutePath(const String& absolutePath);
    
    // 特殊路径解析
    static StringVector resolveNodeModulesPath(const String& moduleName, const String& startDir = "");
    static String resolveCHTLModulePath(const String& moduleName, const String& startDir = "");
    static String resolveCMODPath(const String& cmodName, const String& startDir = "");
    
    // 路径验证
    static bool isValidImportPath(const String& path);
    static bool isValidFileType(const String& path, const String& expectedExtension);
    static bool fileExists(const String& path);
    static bool directoryExists(const String& path);
    
    // 路径工具
    static String getDirectoryFromPath(const String& path);
    static String getFilenameFromPath(const String& path);
    static String getExtensionFromPath(const String& path);
    static String joinPaths(const String& basePath, const String& relativePath);
    static String getCanonicalPath(const String& path);
    
private:
    static bool isNodeModulesSearchPath(const String& path);
    static StringVector getNodeModulesSearchPaths(const String& startDir);
};

// 带名原始嵌入管理器
class NamedOriginManager {
private:
    std::unordered_map<String, NamedOriginConfig> namedOrigins;
    std::unordered_map<String, String> aliasToTypeMap;  // 别名到类型的映射
    
public:
    NamedOriginManager();
    ~NamedOriginManager() = default;
    
    // 注册和管理
    void registerNamedOrigin(const String& aliasName, const NamedOriginConfig& config);
    void unregisterNamedOrigin(const String& aliasName);
    
    // 查询
    NamedOriginConfig getNamedOrigin(const String& aliasName) const;
    bool hasNamedOrigin(const String& aliasName) const;
    String getOriginalType(const String& aliasName) const;
    
    // 解析as语法
    NamedOriginConfig parseNamedOriginFromAs(const String& originType, const String& asClause) const;
    std::unique_ptr<ImportNode> createNamedOriginImport(const String& originType, const String& sourcePath, const String& aliasName) const;
    
    // 验证
    bool validateNamedOrigin(const NamedOriginConfig& config) const;
    StringVector getValidationErrors(const NamedOriginConfig& config) const;
    
    // 批量操作
    void importNamedOriginsFromConfig(const StringUnorderedMap& config);
    StringUnorderedMap exportNamedOriginsToConfig() const;
    
    // 统计和调试
    size_t getNamedOriginCount() const { return namedOrigins.size(); }
    StringVector getAllAliases() const;
    StringVector getAllOriginTypes() const;
    void printNamedOrigins() const;
    
    // 默认配置
    void setupDefaultNamedOrigins();
    static NamedOriginConfig createDefaultNamedOrigin(const String& originType, const String& aliasName);
    
private:
    bool isValidAliasName(const String& aliasName) const;
    bool isValidOriginType(const String& originType) const;
    void updateAliasMapping(const String& aliasName, const String& originType);
    void removeAliasMapping(const String& aliasName);
};

// 全局Import增强器实例
extern ImportEnhancer* g_importEnhancer;
extern NamedOriginManager* g_namedOriginManager;

// 初始化和关闭函数
void initializeImportEnhancer(const String& workingDirectory = ".");
void shutdownImportEnhancer();

// 便捷访问函数
ImportEnhancer& getGlobalImportEnhancer();
NamedOriginManager& getGlobalNamedOriginManager();

} // namespace CHTL