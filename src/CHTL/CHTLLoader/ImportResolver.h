#pragma once

#include "../../Util/Common.h"
#include <mutex>

namespace CHTL {

// 路径类型
enum class PathType {
    RELATIVE,           // 相对路径
    ABSOLUTE,           // 绝对路径
    MODULE_NAME,        // 模块名称
    OFFICIAL_MODULE,    // 官方模块
    WILDCARD,           // 通配符路径
    DIRECTORY,          // 目录路径
    INVALID             // 无效路径
};

// 导入类型详细分类
enum class ImportTypeDetail {
    // 文件导入
    HTML_FILE,
    CSS_FILE,
    JAVASCRIPT_FILE,
    
    // CHTL相关导入
    CHTL_FILE,
    CMOD_FILE,
    CJMOD_FILE,
    
    // 模板和自定义导入
    TEMPLATE_STYLE,
    TEMPLATE_ELEMENT,
    TEMPLATE_VAR,
    CUSTOM_STYLE,
    CUSTOM_ELEMENT,
    CUSTOM_VAR,
    
    // 原始嵌入导入
    ORIGIN_HTML,
    ORIGIN_STYLE,
    ORIGIN_JAVASCRIPT,
    ORIGIN_CUSTOM,
    
    // 配置导入
    CONFIGURATION,
    
    // 批量导入
    ALL_TEMPLATES,
    ALL_CUSTOMS,
    ALL_ORIGINS,
    ALL_CONFIGURATIONS,
    
    // 通配符导入
    WILDCARD_CMOD,
    WILDCARD_CHTL,
    WILDCARD_ALL,
    
    UNKNOWN
};

// 解析路径信息
struct ResolvedPath {
    String originalPath;        // 原始路径
    String resolvedPath;        // 解析后的完整路径
    PathType pathType;          // 路径类型
    ImportTypeDetail importType; // 导入类型
    String fileExtension;       // 文件扩展名
    String fileName;            // 文件名
    String directoryPath;       // 目录路径
    bool exists;                // 文件是否存在
    bool isDirectory;           // 是否是目录
    double fileSize;            // 文件大小（字节）
    double lastModified;        // 最后修改时间
    StringUnorderedMap metadata; // 元数据
    
    ResolvedPath() : pathType(PathType::INVALID), importType(ImportTypeDetail::UNKNOWN),
                    exists(false), isDirectory(false), fileSize(0.0), lastModified(0.0) {}
};

// 导入解析选项
struct ImportResolveOptions {
    StringVector searchPaths;           // 搜索路径列表
    StringVector moduleDirectories;     // 模块目录列表
    StringVector officialModulePaths;   // 官方模块路径列表
    StringUnorderedMap pathAliases;     // 路径别名映射
    
    bool enableWildcards = true;        // 启用通配符
    bool followSymlinks = true;         // 跟随符号链接
    bool cacheResults = true;           // 缓存结果
    bool allowMissingFiles = false;     // 允许缺失文件
    bool strictTypeChecking = true;     // 严格类型检查
    bool enableOfficialModulePrefix = true; // 启用官方模块前缀 chtl::
    
    size_t maxSearchDepth = 10;         // 最大搜索深度
    size_t cacheMaxSize = 1000;         // 缓存最大大小
    double cacheTimeoutMinutes = 60.0;  // 缓存超时时间
    
    // 默认搜索路径
    StringVector getDefaultSearchPaths() const;
    StringVector getDefaultModuleDirectories() const;
    StringVector getDefaultOfficialModulePaths() const;
};

// 循环依赖检测
class CircularDependencyDetector {
public:
    CircularDependencyDetector();
    
    // 依赖关系管理
    void addDependency(const String& from, const String& to);
    void removeDependency(const String& from, const String& to);
    void clearDependencies();
    
    // 循环检测
    bool hasCircularDependency() const;
    bool hasCircularDependency(const String& startPath) const;
    StringVector getCircularDependencyChain(const String& startPath) const;
    std::vector<StringVector> getAllCircularDependencies() const;
    
    // 依赖分析
    StringVector getDependencies(const String& path) const;
    StringVector getDependents(const String& path) const;
    StringVector getTransitiveDependencies(const String& path) const;
    StringVector getTopologicalOrder() const;
    
    // 依赖图信息
    size_t getDependencyCount() const;
    StringVector getAllPaths() const;
    StringUnorderedMap getDependencyGraph() const { return dependencies; }
    
    // 验证
    bool validateDependencyGraph() const;
    StringVector getDependencyErrors() const;
    
private:
    StringUnorderedMap dependencies;  // path -> [dependent_paths]
    
    bool hasCircularDependencyInternal(const String& startPath, std::set<String>& visiting, std::set<String>& visited) const;
    void topologicalSortUtil(const String& path, std::set<String>& visited, std::stack<String>& stack) const;
};

// 导入缓存
class ImportCache {
public:
    struct CacheEntry {
        ResolvedPath resolvedPath;
        double cacheTime;
        double accessCount;
        double lastAccess;
        
        CacheEntry() : cacheTime(0.0), accessCount(0.0), lastAccess(0.0) {}
        CacheEntry(const ResolvedPath& path) : resolvedPath(path), accessCount(1.0) {
            cacheTime = getCurrentTime();
            lastAccess = cacheTime;
        }
    };
    
    explicit ImportCache(size_t maxSize = 1000, double timeoutMinutes = 60.0);
    
    // 缓存操作
    void put(const String& key, const ResolvedPath& resolvedPath);
    bool has(const String& key) const;
    ResolvedPath get(const String& key);
    void remove(const String& key);
    void clear();
    
    // 缓存管理
    void cleanup();                    // 清理过期缓存
    void resize(size_t newMaxSize);    // 调整缓存大小
    void setTimeout(double timeoutMinutes); // 设置超时时间
    
    // 缓存统计
    struct CacheStats {
        size_t totalEntries = 0;
        size_t hits = 0;
        size_t misses = 0;
        double hitRate = 0.0;
        size_t memoryUsage = 0;
        double averageAccessTime = 0.0;
    };
    
    CacheStats getStats() const;
    void resetStats();
    
    // 缓存策略
    enum class EvictionPolicy {
        LRU,    // Least Recently Used
        LFU,    // Least Frequently Used
        FIFO    // First In First Out
    };
    
    void setEvictionPolicy(EvictionPolicy policy) { evictionPolicy = policy; }
    EvictionPolicy getEvictionPolicy() const { return evictionPolicy; }
    
private:
    size_t maxCacheSize;
    double timeoutMinutes;
    EvictionPolicy evictionPolicy;
    std::unordered_map<String, CacheEntry> cache;
    mutable CacheStats stats;
    mutable std::mutex cacheMutex;
    
    void evictEntries();
    void evictLRU();
    void evictLFU();
    void evictFIFO();
    bool isExpired(const CacheEntry& entry) const;
    String generateCacheKey(const String& path, const ImportResolveOptions& options) const;
    static double getCurrentTime();
};

// 高级导入解析器
class AdvancedImportResolver {
public:
    explicit AdvancedImportResolver(const ImportResolveOptions& options = ImportResolveOptions{});
    ~AdvancedImportResolver() = default;
    
    // 基础路径解析
    ResolvedPath resolvePath(const String& importPath, const String& currentDirectory = "") const;
    std::vector<ResolvedPath> resolveMultiplePaths(const StringVector& importPaths, const String& currentDirectory = "") const;
    
    // 特殊路径解析
    ResolvedPath resolveModulePath(const String& moduleName, const String& currentDirectory = "") const;
    ResolvedPath resolveOfficialModule(const String& moduleName) const;
    ResolvedPath resolveRelativePath(const String& relativePath, const String& currentDirectory) const;
    ResolvedPath resolveAbsolutePath(const String& absolutePath) const;
    
    // 通配符解析
    std::vector<ResolvedPath> resolveWildcard(const String& wildcardPattern, const String& currentDirectory = "") const;
    std::vector<ResolvedPath> resolveDirectoryImports(const String& directoryPath, const String& filePattern = "*") const;
    
    // 类型特定解析
    ResolvedPath resolveTemplateImport(const String& templateType, const String& templateName, const String& sourcePath) const;
    ResolvedPath resolveCustomImport(const String& customType, const String& customName, const String& sourcePath) const;
    ResolvedPath resolveOriginImport(const String& originType, const String& originName, const String& sourcePath) const;
    ResolvedPath resolveConfigImport(const String& configName, const String& sourcePath) const;
    
    // 批量解析
    std::vector<ResolvedPath> resolveAllTemplates(const String& sourcePath) const;
    std::vector<ResolvedPath> resolveAllCustoms(const String& sourcePath) const;
    std::vector<ResolvedPath> resolveAllOrigins(const String& sourcePath) const;
    
    // 依赖分析
    StringVector analyzeDependencies(const String& filePath) const;
    StringUnorderedMap buildDependencyGraph(const StringVector& filePaths) const;
    StringVector getTopologicalOrder(const StringUnorderedMap& dependencyGraph) const;
    bool hasCircularDependencies(const StringVector& filePaths) const;
    
    // 配置管理
    void setOptions(const ImportResolveOptions& options);
    const ImportResolveOptions& getOptions() const { return options; }
    
    void addSearchPath(const String& path);
    void removeSearchPath(const String& path);
    void addModuleDirectory(const String& directory);
    void addOfficialModulePath(const String& path);
    void addPathAlias(const String& alias, const String& path);
    
    // 缓存管理
    void enableCache(bool enable);
    bool isCacheEnabled() const;
    void clearCache();
    ImportCache::CacheStats getCacheStats() const;
    
    // 验证
    bool validateImportPath(const String& importPath) const;
    StringVector getValidationErrors(const String& importPath) const;
    bool validateResolvedPath(const ResolvedPath& resolvedPath) const;
    
    // 路径工具
    static PathType detectPathType(const String& path);
    static ImportTypeDetail detectImportType(const String& path, const String& importStatement = "");
    static String normalizePathSeparators(const String& path);
    static String getFileExtension(const String& path);
    static String getFileName(const String& path);
    static String getDirectoryPath(const String& path);
    static bool isAbsolutePath(const String& path);
    static bool isWildcardPath(const String& path);
    
private:
    ImportResolveOptions options;
    std::unique_ptr<ImportCache> cache;
    std::unique_ptr<CircularDependencyDetector> dependencyDetector;
    
    // 内部解析方法
    ResolvedPath resolvePathInternal(const String& importPath, const String& currentDirectory) const;
    String searchInDirectories(const String& fileName, const StringVector& directories) const;
    String findFileWithExtensions(const String& basePath, const StringVector& extensions) const;
    std::vector<ResolvedPath> expandWildcard(const String& pattern, const String& directory) const;
    
    // 文件系统操作
    bool fileExists(const String& path) const;
    bool directoryExists(const String& path) const;
    StringVector listDirectory(const String& path) const;
    StringVector getFilesWithExtension(const String& directory, const String& extension) const;
    ResolvedPath createResolvedPath(const String& originalPath, const String& resolvedPath) const;
    
    // 模块搜索
    String searchOfficialModules(const String& moduleName) const;
    String searchUserModules(const String& moduleName, const String& currentDirectory) const;
    String searchInModuleDirectories(const String& moduleName) const;
    
    // 类型检测和验证
    ImportTypeDetail detectFileImportType(const String& filePath) const;
    bool validateImportType(const ResolvedPath& resolvedPath, ImportTypeDetail expectedType) const;
    
    // 错误处理
    void reportError(const String& message) const;
    void reportWarning(const String& message) const;
    
    // 缓存键生成
    String generateCacheKey(const String& importPath, const String& currentDirectory) const;
    
    // 路径规范化
    String normalizePath(const String& path) const;
    String resolveDots(const String& path) const;  // 解析 . 和 ..
    String makeAbsolute(const String& path, const String& basePath) const;
};

// 导入语句解析器
class ImportStatementParser {
public:
    struct ParsedImport {
        String originalStatement;
        ImportTypeDetail importType;
        String sourcePath;
        String aliasName;
        StringVector importItems;
        StringVector exceptItems;
        bool hasAlias;
        bool isWildcard;
        StringUnorderedMap metadata;
        
        ParsedImport() : importType(ImportTypeDetail::UNKNOWN), hasAlias(false), isWildcard(false) {}
    };
    
    ImportStatementParser();
    
    // 解析方法
    ParsedImport parseImportStatement(const String& statement) const;
    std::vector<ParsedImport> parseMultipleStatements(const StringVector& statements) const;
    
    // 语句生成
    String generateImportStatement(const ParsedImport& parsedImport) const;
    StringVector generateImportStatements(const std::vector<ParsedImport>& parsedImports) const;
    
    // 验证
    bool validateImportStatement(const String& statement) const;
    StringVector getParseErrors(const String& statement) const;
    
    // 解析工具
    static bool isValidImportStatement(const String& statement);
    static ImportTypeDetail detectImportTypeFromStatement(const String& statement);
    static String extractSourcePath(const String& statement);
    static String extractAlias(const String& statement);
    static StringVector extractImportItems(const String& statement);
    static StringVector extractExceptItems(const String& statement);
    
private:
    // 解析规则
    struct ImportRule {
        std::regex pattern;
        ImportTypeDetail type;
        StringVector captureGroups;
    };
    
    std::vector<ImportRule> importRules;
    
    void initializeImportRules();
    ParsedImport applyRule(const ImportRule& rule, const String& statement) const;
    String extractCaptureGroup(const std::smatch& match, const String& groupName, const StringVector& captureGroups) const;
    
    // 工具方法
    String trimQuotes(const String& str) const;
    StringVector splitCommaSeparated(const String& str) const;
    bool matchesPattern(const String& statement, const String& pattern) const;
};

// 导入解析器工厂
class ImportResolverFactory {
public:
    static std::unique_ptr<AdvancedImportResolver> createDefaultResolver();
    static std::unique_ptr<AdvancedImportResolver> createStrictResolver();
    static std::unique_ptr<AdvancedImportResolver> createPermissiveResolver();
    static std::unique_ptr<AdvancedImportResolver> createProductionResolver();
    static std::unique_ptr<AdvancedImportResolver> createDevelopmentResolver();
    
    // 选项预设
    static ImportResolveOptions getDefaultOptions();
    static ImportResolveOptions getStrictOptions();
    static ImportResolveOptions getPermissiveOptions();
    static ImportResolveOptions getProductionOptions();
    static ImportResolveOptions getDevelopmentOptions();
};

} // namespace CHTL