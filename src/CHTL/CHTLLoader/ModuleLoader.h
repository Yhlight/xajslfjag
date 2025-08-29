#pragma once

#include "../../Util/Common.h"
#include "ImportResolver.h"
#include "../CHTLNode/BaseNode.h"
#include "../CHTLContext/Context.h"

namespace CHTL {

// 模块类型
enum class ModuleType {
    CHTL_SOURCE,        // CHTL源文件
    CMOD_PACKAGE,       // CMOD包
    CJMOD_PACKAGE,      // CJMOD包
    HTML_FILE,          // HTML文件
    CSS_FILE,           // CSS文件
    JAVASCRIPT_FILE,    // JavaScript文件
    CONFIG_FILE,        // 配置文件
    UNKNOWN_MODULE
};

// 模块信息
struct ModuleInfo {
    String name;                    // 模块名称
    String version;                 // 版本
    String description;             // 描述
    String author;                  // 作者
    String license;                 // 许可证
    String minCHTLVersion;          // 最小CHTL版本
    String maxCHTLVersion;          // 最大CHTL版本
    StringVector dependencies;      // 依赖列表
    String category;                // 类别
    StringUnorderedMap metadata;    // 元数据
    StringVector exportedItems;     // 导出项
    
    ModuleInfo() = default;
    
    bool isValid() const;
    String toString() const;
    void fromString(const String& infoString);
};

// 模块内容
struct ModuleContent {
    ModuleType type;
    String rawContent;              // 原始内容
    std::unique_ptr<BaseNode> ast;  // 解析后的AST
    StringUnorderedMap exports;     // 导出内容
    StringVector imports;           // 导入依赖
    ModuleInfo info;                // 模块信息
    String sourcePath;              // 源路径
    double loadTime;                // 加载时间
    bool isLoaded;                  // 是否已加载
    bool isParsed;                  // 是否已解析
    
    ModuleContent() : type(ModuleType::UNKNOWN_MODULE), loadTime(0.0), isLoaded(false), isParsed(false) {}
};

// 模块缓存
class ModuleCache {
public:
    struct CacheEntry {
        ModuleContent content;
        double cacheTime;
        double lastAccess;
        size_t accessCount;
        bool isDirty;
        
        CacheEntry() : cacheTime(0.0), lastAccess(0.0), accessCount(0), isDirty(false) {}
        explicit CacheEntry(const ModuleContent& moduleContent) 
            : content(moduleContent), accessCount(1), isDirty(false) {
            cacheTime = getCurrentTime();
            lastAccess = cacheTime;
        }
    };
    
    explicit ModuleCache(size_t maxSize = 500, double timeoutMinutes = 120.0);
    
    // 缓存操作
    void put(const String& modulePath, const ModuleContent& content);
    bool has(const String& modulePath) const;
    ModuleContent get(const String& modulePath);
    void remove(const String& modulePath);
    void clear();
    
    // 缓存管理
    void invalidate(const String& modulePath);  // 标记为过期
    void refresh(const String& modulePath);     // 刷新缓存
    void cleanup();                             // 清理过期缓存
    
    // 依赖缓存失效
    void invalidateDependents(const String& modulePath);
    void addDependency(const String& dependent, const String& dependency);
    void removeDependency(const String& dependent, const String& dependency);
    
    // 缓存统计
    struct CacheStats {
        size_t totalEntries = 0;
        size_t hits = 0;
        size_t misses = 0;
        double hitRate = 0.0;
        size_t memoryUsage = 0;
        size_t invalidations = 0;
    };
    
    CacheStats getStats() const;
    void resetStats();
    
private:
    size_t maxCacheSize;
    double timeoutMinutes;
    std::unordered_map<String, CacheEntry> cache;
    StringUnorderedMap dependencies;  // dependent -> [dependencies]
    mutable CacheStats stats;
    mutable std::mutex cacheMutex;
    
    void evictOldEntries();
    bool isExpired(const CacheEntry& entry) const;
    static double getCurrentTime();
};

// 模块加载器配置
struct ModuleLoaderConfig {
    bool enableCaching = true;
    bool enableLazyLoading = true;
    bool enableParallelLoading = true;
    bool strictDependencyChecking = true;
    bool allowCircularDependencies = false;
    bool enableVersionChecking = true;
    bool autoResolveConflicts = false;
    
    size_t maxConcurrentLoads = 10;
    double loadTimeoutSeconds = 30.0;
    StringVector trustedSources;
    StringVector blacklistedSources;
    
    // 路径配置
    StringVector modulePaths;
    StringVector searchPaths;
    String cacheDirectory = "module_cache";
    
    // 安全配置
    bool enableSandboxing = false;
    StringVector allowedOperations;
    StringVector deniedOperations;
};

// 高级模块加载器
class AdvancedModuleLoader {
public:
    explicit AdvancedModuleLoader(const ModuleLoaderConfig& config = ModuleLoaderConfig{});
    ~AdvancedModuleLoader();
    
    // 基础加载方法
    ModuleContent loadModule(const String& modulePath);
    ModuleContent loadModuleFromSource(const String& source, const String& moduleName = "");
    std::vector<ModuleContent> loadModules(const StringVector& modulePaths);
    
    // 异步加载
    std::future<ModuleContent> loadModuleAsync(const String& modulePath);
    std::vector<std::future<ModuleContent>> loadModulesAsync(const StringVector& modulePaths);
    
    // 依赖解析和加载
    std::vector<ModuleContent> loadWithDependencies(const String& modulePath);
    StringVector resolveDependencies(const String& modulePath);
    StringVector getLoadOrder(const StringVector& modulePaths);
    
    // 特定类型加载
    ModuleContent loadCHTLModule(const String& path);
    ModuleContent loadCMODPackage(const String& path);
    ModuleContent loadCJMODPackage(const String& path);
    ModuleContent loadConfigModule(const String& path);
    
    // 模块查询
    bool isModuleLoaded(const String& modulePath) const;
    ModuleContent getLoadedModule(const String& modulePath) const;
    StringVector getLoadedModules() const;
    
    // 模块管理
    void unloadModule(const String& modulePath);
    void reloadModule(const String& modulePath);
    void unloadAllModules();
    
    // 依赖管理
    StringVector getModuleDependencies(const String& modulePath) const;
    StringVector getModuleDependents(const String& modulePath) const;
    bool hasCircularDependencies(const String& modulePath) const;
    
    // 版本管理
    struct VersionInfo {
        String version;
        String minRequired;
        String maxSupported;
        bool isCompatible;
        String compatibilityMessage;
    };
    
    VersionInfo checkVersion(const String& modulePath) const;
    bool isVersionCompatible(const String& version, const String& minVersion, const String& maxVersion) const;
    String resolveVersionConflict(const StringVector& conflictingVersions) const;
    
    // 配置管理
    void setConfig(const ModuleLoaderConfig& config);
    const ModuleLoaderConfig& getConfig() const { return config; }
    
    void addModulePath(const String& path);
    void removeModulePath(const String& path);
    void addTrustedSource(const String& source);
    void addBlacklistedSource(const String& source);
    
    // 缓存管理
    void enableCache(bool enable);
    bool isCacheEnabled() const;
    void clearCache();
    ModuleCache::CacheStats getCacheStats() const;
    
    // 安全检查
    bool isTrustedSource(const String& source) const;
    bool isBlacklistedSource(const String& source) const;
    bool validateModuleSecurity(const ModuleContent& content) const;
    
    // 错误处理
    bool hasErrors() const { return !errors.empty(); }
    StringVector getErrors() const { return errors; }
    void clearErrors() { errors.clear(); }
    
    // 统计信息
    struct LoaderStats {
        size_t totalLoads = 0;
        size_t successfulLoads = 0;
        size_t failedLoads = 0;
        size_t cacheHits = 0;
        size_t cacheMisses = 0;
        double totalLoadTime = 0.0;
        double averageLoadTime = 0.0;
        size_t circularDependencies = 0;
        size_t versionConflicts = 0;
    };
    
    LoaderStats getStats() const { return stats; }
    void resetStats() { stats = LoaderStats{}; }
    
private:
    ModuleLoaderConfig config;
    std::unique_ptr<ModuleCache> cache;
    std::unique_ptr<AdvancedImportResolver> importResolver;
    StringUnorderedMap loadedModules;  // path -> module_name
    StringUnorderedMap moduleDependencies;  // module -> [dependencies]
    StringVector errors;
    LoaderStats stats;
    std::mutex loaderMutex;
    
    // 内部加载逻辑
    ModuleContent loadModuleInternal(const String& modulePath);
    ModuleContent parseModuleContent(const String& rawContent, const String& modulePath);
    
    // 类型检测和解析
    ModuleType detectModuleType(const String& path) const;
    ModuleInfo parseModuleInfo(const String& content, ModuleType type) const;
    StringVector extractDependencies(const String& content, ModuleType type) const;
    StringUnorderedMap extractExports(const String& content, ModuleType type) const;
    
    // CMOD/CJMOD处理
    ModuleContent loadCMODInternal(const String& path);
    ModuleContent loadCJMODInternal(const String& path);
    ModuleInfo parseCMODInfo(const String& infoContent) const;
    ModuleInfo parseCJMODInfo(const String& infoContent) const;
    
    // 依赖解析
    StringVector resolveDependenciesRecursive(const String& modulePath, std::set<String>& visited) const;
    bool checkCircularDependency(const String& modulePath, const String& targetDependency, std::set<String>& visiting) const;
    StringVector topologicalSort(const StringUnorderedMap& dependencies) const;
    
    // 版本处理
    bool compareVersions(const String& version1, const String& version2) const;  // -1, 0, 1
    String normalizeVersion(const String& version) const;
    
    // 文件操作
    String readModuleFile(const String& path) const;
    bool isValidModulePath(const String& path) const;
    String resolveModulePath(const String& path) const;
    
    // 并行加载
    class ParallelLoader {
    public:
        explicit ParallelLoader(AdvancedModuleLoader& loader, size_t maxConcurrency);
        
        std::vector<ModuleContent> loadModulesParallel(const StringVector& modulePaths);
        void shutdown();
        
    private:
        AdvancedModuleLoader& loader;
        size_t maxConcurrency;
        std::vector<std::thread> workers;
        std::queue<String> taskQueue;
        std::vector<ModuleContent> results;
        std::mutex queueMutex;
        std::condition_variable queueCondition;
        std::atomic<bool> shouldStop;
        
        void workerFunction();
    };
    
    std::unique_ptr<ParallelLoader> parallelLoader;
    
    // 错误处理
    void reportError(const String& message);
    void reportWarning(const String& message);
    
    // 统计更新
    void updateStats(bool successful, double loadTime);
    double measureLoadTime(std::function<ModuleContent()> loadFunction);
};

// 模块注册表
class ModuleRegistry {
public:
    struct RegisteredModule {
        String name;
        String path;
        ModuleInfo info;
        StringVector aliases;
        bool isOfficial;
        bool isActive;
        double registrationTime;
        
        RegisteredModule() : isOfficial(false), isActive(true), registrationTime(0.0) {}
    };
    
    static ModuleRegistry& getInstance();
    
    // 模块注册
    void registerModule(const String& name, const String& path, const ModuleInfo& info, bool isOfficial = false);
    void registerOfficialModule(const String& name, const String& path, const ModuleInfo& info);
    void unregisterModule(const String& name);
    
    // 模块查询
    bool isRegistered(const String& name) const;
    RegisteredModule getRegisteredModule(const String& name) const;
    StringVector getRegisteredModuleNames() const;
    StringVector getOfficialModuleNames() const;
    
    // 别名管理
    void addAlias(const String& moduleName, const String& alias);
    void removeAlias(const String& moduleName, const String& alias);
    String resolveAlias(const String& alias) const;
    
    // 模块搜索
    StringVector searchModules(const String& query) const;
    StringVector searchByCategory(const String& category) const;
    StringVector searchByAuthor(const String& author) const;
    
    // 批量操作
    void registerModulesFromDirectory(const String& directory);
    void refreshModuleRegistry();
    void exportRegistry(const String& filePath) const;
    void importRegistry(const String& filePath);
    
private:
    std::unordered_map<String, RegisteredModule> modules;
    StringUnorderedMap aliases;  // alias -> module_name
    mutable std::mutex registryMutex;
    
    ModuleRegistry() = default;
    
    void scanDirectory(const String& directory);
    ModuleInfo extractModuleInfo(const String& modulePath) const;
};

// 模块依赖解析器
class ModuleDependencyResolver {
public:
    struct DependencyNode {
        String name;
        String version;
        String path;
        StringVector dependencies;
        StringVector dependents;
        bool isResolved;
        bool isOptional;
        
        DependencyNode() : isResolved(false), isOptional(false) {}
    };
    
    ModuleDependencyResolver();
    
    // 依赖图构建
    void addModule(const String& name, const String& version, const StringVector& dependencies);
    void removeModule(const String& name);
    void addDependency(const String& module, const String& dependency);
    void removeDependency(const String& module, const String& dependency);
    
    // 依赖解析
    StringVector resolveDependencies(const String& rootModule);
    StringVector getLoadOrder(const StringVector& modules);
    bool hasCircularDependencies() const;
    StringVector getCircularDependencyChain() const;
    
    // 冲突检测
    struct VersionConflict {
        String moduleName;
        StringVector conflictingVersions;
        StringVector requiredBy;
        String suggestedResolution;
    };
    
    std::vector<VersionConflict> detectVersionConflicts() const;
    StringVector resolveVersionConflicts(const std::vector<VersionConflict>& conflicts);
    
    // 依赖分析
    StringVector getMissingDependencies(const String& module) const;
    StringVector getOptionalDependencies(const String& module) const;
    StringVector getTransitiveDependencies(const String& module) const;
    
    // 图形信息
    StringVector getAllModules() const;
    size_t getDependencyCount() const;
    String generateDependencyGraph() const;  // DOT格式
    
private:
    std::unordered_map<String, DependencyNode> dependencyGraph;
    
    void topologicalSortUtil(const String& module, std::set<String>& visited, std::stack<String>& stack) const;
    bool hasCircularDependencyUtil(const String& module, std::set<String>& visiting, std::set<String>& visited) const;
    int compareVersions(const String& version1, const String& version2) const;
};

// 模块加载器工厂
class ModuleLoaderFactory {
public:
    static std::unique_ptr<AdvancedModuleLoader> createDefaultLoader();
    static std::unique_ptr<AdvancedModuleLoader> createProductionLoader();
    static std::unique_ptr<AdvancedModuleLoader> createDevelopmentLoader();
    static std::unique_ptr<AdvancedModuleLoader> createSecureLoader();
    static std::unique_ptr<AdvancedModuleLoader> createFastLoader();
    
    // 配置预设
    static ModuleLoaderConfig getDefaultConfig();
    static ModuleLoaderConfig getProductionConfig();
    static ModuleLoaderConfig getDevelopmentConfig();
    static ModuleLoaderConfig getSecureConfig();
    static ModuleLoaderConfig getPerformanceConfig();
};

} // namespace CHTL