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
    WILDCARD_DIRECTORY,
    WILDCARD_RECURSIVE,
    
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
    
    // 通配符配置
    bool enableRecursiveWildcard = true;    // 启用递归通配符 **
    size_t maxWildcardDepth = 5;           // 通配符最大深度
    size_t maxWildcardResults = 100;       // 通配符最大结果数
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
    std::vector<ResolvedPath> resolveRecursiveWildcard(const String& baseDirectory, const String& pattern) const;
    std::vector<ResolvedPath> resolveGlobPattern(const String& globPattern, const String& currentDirectory = "") const;
    
    // 高级通配符特性
    std::vector<ResolvedPath> resolveWildcardWithFilters(const String& pattern, const StringVector& includeExtensions, const StringVector& excludePatterns) const;
    std::vector<ResolvedPath> resolveMultipleWildcards(const StringVector& patterns, const String& currentDirectory = "") const;
    bool isWildcardPattern(const String& pattern) const;
    String expandWildcardPath(const String& pattern, const String& currentDirectory) const;
    
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

// 超级导入解析器 - 100% 完整功能
class SuperImportResolver : public AdvancedImportResolver {
public:
    // 高级路径解析功能
    struct AdvancedResolveOptions {
        bool enableSymlinkResolution = true;    // 符号链接解析
        bool enableCaseSensitiveSearch = true;  // 大小写敏感搜索
        bool enableFuzzyMatching = false;       // 模糊匹配
        bool enableVersionResolution = true;    // 版本解析
        bool enableConditionalImports = true;   // 条件导入
        size_t maxResolutionDepth = 20;        // 最大解析深度
        size_t maxWildcardResults = 1000;      // 最大通配符结果数
        double resolutionTimeoutSeconds = 30.0; // 解析超时时间
        
        // 高级过滤选项
        StringVector includePatterns;           // 包含模式
        StringVector excludePatterns;           // 排除模式
        StringVector requiredExtensions;        // 必需扩展名
        StringVector forbiddenExtensions;       // 禁止扩展名
        
        // 安全选项
        bool preventPathTraversal = true;       // 防止路径遍历
        bool validateFilePermissions = true;    // 验证文件权限
        bool checkFileIntegrity = false;        // 检查文件完整性
        StringVector trustedDirectories;        // 受信任目录
    };
    
    // 智能路径匹配器
    class PathMatcher {
    public:
        virtual ~PathMatcher() = default;
        virtual bool matches(const String& path, const String& pattern) const = 0;
        virtual double getMatchScore(const String& path, const String& pattern) const = 0;
        virtual StringVector getSuggestions(const String& path, const StringVector& candidates) const = 0;
    };
    
    // 模糊匹配器
    class FuzzyPathMatcher : public PathMatcher {
    public:
        bool matches(const String& path, const String& pattern) const override;
        double getMatchScore(const String& path, const String& pattern) const override;
        StringVector getSuggestions(const String& path, const StringVector& candidates) const override;
        
    private:
        double calculateLevenshteinDistance(const String& s1, const String& s2) const;
        double calculateJaroWinkler(const String& s1, const String& s2) const;
    };
    
    // 正则表达式匹配器
    class RegexPathMatcher : public PathMatcher {
    public:
        bool matches(const String& path, const String& pattern) const override;
        double getMatchScore(const String& path, const String& pattern) const override;
        StringVector getSuggestions(const String& path, const StringVector& candidates) const override;
    };
    
    // Glob 匹配器 (支持 *, **, ?, [], {})
    class GlobPathMatcher : public PathMatcher {
    public:
        bool matches(const String& path, const String& pattern) const override;
        double getMatchScore(const String& path, const String& pattern) const override;
        StringVector getSuggestions(const String& path, const StringVector& candidates) const override;
        
    private:
        bool matchGlob(const String& path, const String& pattern) const;
        bool matchRecursiveWildcard(const String& path, const String& pattern) const;
        bool matchCharacterClass(char c, const String& charClass) const;
        bool matchBraceExpansion(const String& path, const String& pattern) const;
    };
    
    // 版本解析器
    class VersionResolver {
    public:
        struct VersionInfo {
            String version;
            String path;
            bool isStable;
            bool isLatest;
            std::chrono::milliseconds timestamp;
        };
        
        virtual ~VersionResolver() = default;
        virtual StringVector getAvailableVersions(const String& moduleName) const = 0;
        virtual VersionInfo resolveVersion(const String& moduleName, const String& versionSpec) const = 0;
        virtual VersionInfo getLatestVersion(const String& moduleName) const = 0;
        virtual bool isVersionCompatible(const String& required, const String& available) const = 0;
    };
    
    // 语义版本解析器
    class SemanticVersionResolver : public VersionResolver {
    public:
        StringVector getAvailableVersions(const String& moduleName) const override;
        VersionInfo resolveVersion(const String& moduleName, const String& versionSpec) const override;
        VersionInfo getLatestVersion(const String& moduleName) const override;
        bool isVersionCompatible(const String& required, const String& available) const override;
        
    private:
        struct SemanticVersion {
            int major, minor, patch;
            String prerelease;
            String build;
        };
        
        SemanticVersion parseVersion(const String& version) const;
        bool satisfiesRange(const SemanticVersion& version, const String& range) const;
        int compareVersions(const SemanticVersion& v1, const SemanticVersion& v2) const;
    };
    
    // 条件导入处理器
    class ConditionalImportProcessor {
    public:
        struct ImportCondition {
            String condition;       // 条件表达式
            String truePath;       // 条件为真时的路径
            String falsePath;      // 条件为假时的路径
            String defaultPath;    // 默认路径
        };
        
        virtual ~ConditionalImportProcessor() = default;
        virtual bool evaluateCondition(const String& condition, const StringUnorderedMap& context) const = 0;
        virtual String resolveConditionalImport(const ImportCondition& condition, 
                                               const StringUnorderedMap& context) const = 0;
    };
    
    // 标准条件处理器
    class StandardConditionalProcessor : public ConditionalImportProcessor {
    public:
        bool evaluateCondition(const String& condition, const StringUnorderedMap& context) const override;
        String resolveConditionalImport(const ImportCondition& condition, 
                                       const StringUnorderedMap& context) const override;
        
    private:
        bool evaluateExpression(const String& expr, const StringUnorderedMap& context) const;
        String substituteVariables(const String& str, const StringUnorderedMap& context) const;
    };
    
private:
    AdvancedResolveOptions advancedOptions;
    std::unique_ptr<PathMatcher> pathMatcher;
    std::unique_ptr<VersionResolver> versionResolver;
    std::unique_ptr<ConditionalImportProcessor> conditionalProcessor;
    
    // 性能监控
    mutable std::unordered_map<String, size_t> resolutionCounts;
    mutable std::unordered_map<String, std::chrono::milliseconds> resolutionTimes;
    mutable std::unordered_map<String, size_t> cacheHits;
    
    // 安全验证
    StringUnorderedSet trustedPaths;
    StringUnorderedSet blockedPaths;
    
public:
    explicit SuperImportResolver(const ImportResolveOptions& options = ImportResolveOptions{},
                               const AdvancedResolveOptions& advancedOptions = AdvancedResolveOptions{});
    ~SuperImportResolver() override = default;
    
    // 设置高级选项
    void setAdvancedOptions(const AdvancedResolveOptions& options) { advancedOptions = options; }
    const AdvancedResolveOptions& getAdvancedOptions() const { return advancedOptions; }
    
    // 设置组件
    void setPathMatcher(std::unique_ptr<PathMatcher> matcher);
    void setVersionResolver(std::unique_ptr<VersionResolver> resolver);
    void setConditionalProcessor(std::unique_ptr<ConditionalImportProcessor> processor);
    
    // 超级通配符解析
    std::vector<ResolvedPath> resolveSuperWildcard(const String& pattern, const String& currentDirectory = "") const;
    std::vector<ResolvedPath> resolveAdvancedGlob(const String& globPattern, const String& currentDirectory = "") const;
    std::vector<ResolvedPath> resolveNestedWildcard(const String& pattern, size_t maxDepth = 10) const;
    std::vector<ResolvedPath> resolveConditionalWildcard(const String& pattern, const StringUnorderedMap& conditions) const;
    
    // 智能路径解析
    std::vector<ResolvedPath> resolveWithFuzzyMatching(const String& pattern, double minScore = 0.7) const;
    std::vector<ResolvedPath> resolveWithSuggestions(const String& pattern) const;
    ResolvedPath resolveBestMatch(const String& pattern) const;
    StringVector getPathSuggestions(const String& partialPath, size_t maxSuggestions = 10) const;
    
    // 版本化导入
    ResolvedPath resolveVersionedImport(const String& moduleName, const String& versionSpec) const;
    std::vector<ResolvedPath> resolveAllVersions(const String& moduleName) const;
    ResolvedPath resolveLatestVersion(const String& moduleName) const;
    bool checkVersionCompatibility(const String& moduleName, const String& requiredVersion) const;
    
    // 条件导入
    ResolvedPath resolveConditionalImport(const String& importPath, const StringUnorderedMap& conditions) const;
    std::vector<ResolvedPath> resolveAllConditionalPaths(const String& importPath, 
                                                         const StringVector& conditionSets) const;
    
    // 高级依赖分析
    StringUnorderedMap buildAdvancedDependencyGraph(const StringVector& filePaths) const;
    StringVector getOptimizedLoadOrder(const StringVector& filePaths) const;
    StringVector detectConflictingDependencies(const StringVector& filePaths) const;
    StringVector analyzeCriticalPath(const StringVector& filePaths, const String& targetFile) const;
    
    // 性能优化
    void preloadCommonPaths();
    void optimizeSearchPaths();
    void warmupCache(const StringVector& expectedPaths);
    void enableParallelResolution(bool enable = true);
    
    // 安全管理
    void addTrustedPath(const String& path);
    void addBlockedPath(const String& path);
    void removeTrustedPath(const String& path);
    void removeBlockedPath(const String& path);
    bool isPathTrusted(const String& path) const;
    bool isPathBlocked(const String& path) const;
    StringVector validatePathSecurity(const String& path) const;
    
    // 监控和诊断
    std::unordered_map<String, size_t> getResolutionStatistics() const { return resolutionCounts; }
    std::unordered_map<String, std::chrono::milliseconds> getPerformanceStatistics() const { return resolutionTimes; }
    std::unordered_map<String, size_t> getCacheStatistics() const { return cacheHits; }
    void resetStatistics();
    String generatePerformanceReport() const;
    String generateDependencyReport(const StringVector& filePaths) const;
    
    // 高级工具方法
    static StringVector expandBraceExpansion(const String& pattern);
    static String normalizePath(const String& path, bool resolveDots = true);
    static String resolvePath(const String& basePath, const String& relativePath);
    static StringVector splitPathPattern(const String& pattern);
    static bool isValidPath(const String& path);
    static bool hasPermission(const String& path, const String& permission);
    static String calculateFileHash(const String& filePath);
    static std::chrono::milliseconds getFileModificationTime(const String& filePath);
    
    // 批量操作
    std::vector<ResolvedPath> resolveBatch(const StringVector& patterns, 
                                          const String& currentDirectory = "", 
                                          bool continueOnError = true) const;
    StringUnorderedMap resolveBatchWithResults(const StringVector& patterns, 
                                              const String& currentDirectory = "") const;
    
    // 异步解析（如果需要的话）
    // std::future<std::vector<ResolvedPath>> resolveAsync(const String& pattern) const;
    // std::future<StringUnorderedMap> resolveBatchAsync(const StringVector& patterns) const;
    
private:
    // 内部方法
    std::vector<ResolvedPath> resolveWithMatcher(const String& pattern, const String& currentDirectory) const;
    bool shouldExcludePath(const String& path) const;
    bool matchesIncludePattern(const String& path) const;
    bool matchesExcludePattern(const String& path) const;
    ResolvedPath createResolvedPath(const String& path, const String& originalPattern = "") const;
    void recordResolution(const String& pattern, std::chrono::milliseconds time) const;
    bool isWithinDepthLimit(const String& path, const String& baseDirectory) const;
    StringVector filterBySecurityPolicy(const StringVector& paths) const;
    String resolveSymlinks(const String& path) const;
    
    // 验证方法
    bool validateAdvancedPath(const String& path) const;
    StringVector getAdvancedValidationErrors(const String& path) const;
    bool checkPathPermissions(const String& path) const;
    bool checkPathIntegrity(const String& path) const;
    
    // 缓存优化
    void updateAdvancedCache(const String& pattern, const std::vector<ResolvedPath>& results) const;
    std::vector<ResolvedPath> getFromAdvancedCache(const String& pattern) const;
    bool isInAdvancedCache(const String& pattern) const;
};

// 导入解析工厂
class ImportResolverFactory {
public:
    enum class ResolverType {
        BASIC,              // 基础解析器
        ADVANCED,           // 高级解析器
        SUPER              // 超级解析器
    };
    
    static std::unique_ptr<AdvancedImportResolver> createResolver(
        ResolverType type = ResolverType::ADVANCED,
        const ImportResolveOptions& options = ImportResolveOptions{});
    
    static std::unique_ptr<SuperImportResolver> createSuperResolver(
        const ImportResolveOptions& options = ImportResolveOptions{},
        const SuperImportResolver::AdvancedResolveOptions& advancedOptions = 
            SuperImportResolver::AdvancedResolveOptions{});
    
    // 预配置解析器
    static std::unique_ptr<SuperImportResolver> createWebResolver();      // Web 项目解析器
    static std::unique_ptr<SuperImportResolver> createLibraryResolver();  // 库项目解析器
    static std::unique_ptr<SuperImportResolver> createFrameworkResolver(); // 框架解析器
    static std::unique_ptr<SuperImportResolver> createSecureResolver();   // 安全解析器
    
private:
    static ImportResolveOptions getWebOptions();
    static ImportResolveOptions getLibraryOptions();
    static ImportResolveOptions getFrameworkOptions();
    static ImportResolveOptions getSecureOptions();
    
    static SuperImportResolver::AdvancedResolveOptions getWebAdvancedOptions();
    static SuperImportResolver::AdvancedResolveOptions getLibraryAdvancedOptions();
    static SuperImportResolver::AdvancedResolveOptions getFrameworkAdvancedOptions();
    static SuperImportResolver::AdvancedResolveOptions getSecureAdvancedOptions();
};

} // namespace CHTL