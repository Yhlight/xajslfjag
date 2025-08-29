#pragma once

#include "../Util/Common.h"
#include "../Scanner/CHTLUnifiedScanner.h"
#include "../CHTL/CHTLParser/Parser.h"
#include "../CHTL/CHTLGenerator/Generator.h"
#include "../CHTLJS/CHTLJSParser/CHTLJSParser.h"
#include "../ANTLR/ANTLRJavaScriptWrapper.h"
#include "../ANTLR/ANTLRCSSWrapper.h"

namespace CHTL {

// 编译任务类型
enum class CompilationTaskType {
    CHTL_COMPILATION,      // CHTL编译
    CHTLJS_COMPILATION,    // CHTL JS编译
    CSS_COMPILATION,       // CSS编译
    JAVASCRIPT_COMPILATION, // JavaScript编译
    MIXED_COMPILATION      // 混合编译
};

// 编译任务
struct CompilationTask {
    CompilationTaskType type;
    String source;
    String sourcePath;
    CodeFragment fragment;
    StringUnorderedMap parameters;
    size_t priority;
    
    CompilationTask(CompilationTaskType t, const String& src, const String& path = "")
        : type(t), source(src), sourcePath(path), priority(0) {}
};

// 编译结果
struct CompilationResult {
    String html;
    String css;
    String javascript;
    StringUnorderedMap additionalOutputs;
    StringVector errors;
    StringVector warnings;
    double compilationTime;
    size_t inputSize;
    size_t outputSize;
    bool success;
    
    CompilationResult() : compilationTime(0.0), inputSize(0), outputSize(0), success(false) {}
    
    bool hasErrors() const { return !errors.empty(); }
    bool hasWarnings() const { return !warnings.empty(); }
    String getSummary() const;
    void merge(const CompilationResult& other);
};

// 编译器调度器配置
struct DispatcherConfig {
    bool enableParallelCompilation = true;
    size_t maxWorkerThreads = 0;  // 0 = auto detect
    bool enableCaching = true;
    bool enableOptimization = true;
    bool strictMode = false;
    
    // 编译器特定配置
    ParserConfig parserConfig;
    GeneratorConfig generatorConfig;
    ScannerConfig scannerConfig;
    
    // 性能配置
    size_t maxConcurrentTasks = 10;
    size_t taskQueueSize = 100;
    double timeoutSeconds = 30.0;
    
    // 缓存配置
    size_t cacheMaxSize = 1000;
    double cacheTimeoutMinutes = 60.0;
    String cacheDirectory = "cache";
};

// 编译器调度器
class CompilerDispatcher {
public:
    explicit CompilerDispatcher(const DispatcherConfig& config = DispatcherConfig{});
    ~CompilerDispatcher();
    
    // 主编译方法
    CompilationResult compile(const String& source, const String& sourcePath = "");
    CompilationResult compileFile(const String& filePath);
    CompilationResult compileFragment(const CodeFragment& fragment);
    
    // 批量编译
    std::vector<CompilationResult> compileFiles(const StringVector& filePaths);
    std::vector<CompilationResult> compileSources(const StringVector& sources, const StringVector& sourcePaths = {});
    
    // 异步编译
    std::future<CompilationResult> compileAsync(const String& source, const String& sourcePath = "");
    std::future<CompilationResult> compileFileAsync(const String& filePath);
    std::vector<std::future<CompilationResult>> compileFilesAsync(const StringVector& filePaths);
    
    // 编译管道
    void addTask(const CompilationTask& task);
    void addTasks(const std::vector<CompilationTask>& tasks);
    void processQueue();
    void clearQueue();
    size_t getQueueSize() const;
    
    // 配置管理
    void setConfig(const DispatcherConfig& config);
    const DispatcherConfig& getConfig() const { return config; }
    
    // 编译器管理
    void setParser(std::unique_ptr<Parser> parser);
    void setGenerator(std::unique_ptr<Generator> generator);
    void setScanner(std::unique_ptr<CHTLUnifiedScanner> scanner);
    
    // ANTLR编译器管理
    void setANTLRJavaScriptWrapper(std::unique_ptr<ANTLR::ANTLRJavaScriptWrapper> wrapper);
    void setANTLRCSSWrapper(std::unique_ptr<ANTLR::ANTLRCSSWrapper> wrapper);
    bool isANTLREnabled() const;
    
    // ANTLR编译方法
    CompilationResult compileJavaScriptWithANTLR(const String& jsCode);
    CompilationResult compileCSSWithANTLR(const String& cssCode);
    bool validateJavaScriptSyntax(const String& jsCode);
    bool validateCSSSyntax(const String& cssCode);
    
    // 缓存管理
    void enableCache(bool enable) { cacheEnabled = enable; }
    bool isCacheEnabled() const { return cacheEnabled; }
    void clearCache();
    void setCacheDirectory(const String& directory);
    
    // 统计信息
    struct CompilationStats {
        size_t totalCompilations = 0;
        size_t successfulCompilations = 0;
        size_t failedCompilations = 0;
        size_t cacheHits = 0;
        size_t cacheMisses = 0;
        double totalTime = 0.0;
        double averageTime = 0.0;
        size_t totalInputSize = 0;
        size_t totalOutputSize = 0;
        double compressionRatio = 0.0;
    };
    
    CompilationStats getStats() const { return stats; }
    void resetStats() { stats = CompilationStats{}; }
    
    // 错误处理
    bool hasErrors() const { return !errors.empty(); }
    StringVector getErrors() const { return errors; }
    void clearErrors() { errors.clear(); }
    
    // 调试支持
    void enableDebugMode(bool enable) { debugMode = enable; }
    bool isDebugMode() const { return debugMode; }
    void dumpState() const;
    
private:
    DispatcherConfig config;
    std::unique_ptr<CHTLUnifiedScanner> scanner;
    std::unique_ptr<Parser> chtlParser;
    std::unique_ptr<Generator> generator;
    std::queue<CompilationTask> taskQueue;
    std::mutex queueMutex;
    std::condition_variable queueCondition;
    std::vector<std::thread> workerThreads;
    std::atomic<bool> shouldStop;
    bool cacheEnabled;
    StringVector errors;
    CompilationStats stats;
    bool debugMode;
    
    // ANTLR集成组件
    std::unique_ptr<ANTLR::ANTLRJavaScriptWrapper> antlrJSWrapper;
    std::unique_ptr<ANTLR::ANTLRCSSWrapper> antlrCSSWrapper;
    
    // 编译核心逻辑
    CompilationResult compileInternal(const String& source, const String& sourcePath);
    FragmentResult compileFragment(const ScanResult& fragment);
    FragmentResult compileCHTLFragment(const ScanResult& fragment);
    FragmentResult compileCHTLJSFragment(const ScanResult& fragment);
    FragmentResult compilePureJSFragment(const ScanResult& fragment);
    FragmentResult compileCSSFragment(const ScanResult& fragment);
    FragmentResult compileJSFragment(const ScanResult& fragment);
    FragmentResult compileHTMLFragment(const ScanResult& fragment);
    
    // 片段分析和调度
    std::vector<CodeFragment> analyzeSource(const String& source);
    CompilationTaskType determineTaskType(const CodeFragment& fragment);
    std::vector<CompilationTask> createTasks(const std::vector<CodeFragment>& fragments);
    void scheduleTasks(const std::vector<CompilationTask>& tasks);
    
    // 并行处理
    void startWorkerThreads();
    void stopWorkerThreads();
    void workerThreadFunction();
    CompilationResult processTask(const CompilationTask& task);
    
    // 结果合并
    CompilationResult mergeResults(const std::vector<CompilationResult>& results);
    void optimizeResult(CompilationResult& result);
    void validateResult(const CompilationResult& result);
    
    // 缓存系统
    class CompilationCache {
    public:
        explicit CompilationCache(const String& cacheDir, size_t maxSize, double timeoutMinutes);
        
        String getCacheKey(const String& source, const DispatcherConfig& config);
        bool has(const String& key);
        CompilationResult get(const String& key);
        void put(const String& key, const CompilationResult& result);
        void clear();
        void cleanup();
        
        struct CacheStats {
            size_t hits = 0;
            size_t misses = 0;
            size_t entries = 0;
            size_t totalSize = 0;
        };
        
        CacheStats getStats() const { return cacheStats; }
        
    private:
        String cacheDirectory;
        size_t maxCacheSize;
        double timeoutMinutes;
        CacheStats cacheStats;
        std::mutex cacheMutex;
        
        String getCacheFilePath(const String& key);
        bool isValidCacheEntry(const String& filePath);
        void removeOldEntries();
        String generateCacheKey(const String& content);
    };
    
    std::unique_ptr<CompilationCache> cache;
    
    // 错误处理和日志
    void reportError(const String& message);
    void reportWarning(const String& message);
    void logDebug(const String& message) const;
    
    // 性能监控
    void updateStats(const CompilationResult& result);
    double measureTime(std::function<void()> operation);
    
    // 工具方法
    String readFile(const String& filePath);
    bool writeFile(const String& filePath, const String& content);
    String detectSourceType(const String& source);
    bool validateInput(const String& source);
    void initializeComponents();
    void cleanupComponents();
    
    // 纯JS处理方法
    String compilePureJSWithANTLR(const String& jsContent);
    String validateAndCleanJS(const String& jsContent);
};

// 编译器注册表
class CompilerRegistry {
public:
    using ParserFactory = std::function<std::unique_ptr<Parser>(const String&)>;
    using GeneratorFactory = std::function<std::unique_ptr<Generator>()>;
    
    static CompilerRegistry& getInstance();
    
    // 编译器注册
    void registerCHTLParser(const String& name, ParserFactory factory);
    void registerCHTLJSParser(const String& name, ParserFactory factory);
    void registerCSSParser(const String& name, ParserFactory factory);
    void registerJavaScriptParser(const String& name, ParserFactory factory);
    
    void registerGenerator(const String& name, GeneratorFactory factory);
    
    // 编译器获取
    std::unique_ptr<Parser> createParser(const String& type, const String& name, const String& source);
    std::unique_ptr<Generator> createGenerator(const String& name);
    
    // 可用编译器查询
    StringVector getAvailableParsers(const String& type = "");
    StringVector getAvailableGenerators();
    
    // 编译器信息
    bool hasParser(const String& type, const String& name);
    bool hasGenerator(const String& name);
    
private:
    std::unordered_map<String, ParserFactory> chtlParsers;
    std::unordered_map<String, ParserFactory> chtljsParsers;
    std::unordered_map<String, ParserFactory> cssParsers;
    std::unordered_map<String, ParserFactory> jsParsers;
    std::unordered_map<String, GeneratorFactory> generators;
    
    CompilerRegistry() = default;
};

// 编译管道
class CompilationPipeline {
public:
    using ProcessorFunction = std::function<CompilationResult(const CompilationResult&)>;
    
    CompilationPipeline();
    
    // 管道阶段
    void addPreProcessor(ProcessorFunction processor);
    void addPostProcessor(ProcessorFunction processor);
    void addValidator(ProcessorFunction validator);
    void addOptimizer(ProcessorFunction optimizer);
    
    // 执行管道
    CompilationResult execute(const CompilationResult& input);
    
    // 管道配置
    void enableStage(const String& stageName, bool enable);
    bool isStageEnabled(const String& stageName);
    
    // 管道信息
    StringVector getStageNames() const;
    size_t getStageCount() const;
    
private:
    std::vector<ProcessorFunction> preProcessors;
    std::vector<ProcessorFunction> postProcessors;
    std::vector<ProcessorFunction> validators;
    std::vector<ProcessorFunction> optimizers;
    StringUnorderedMap stageSettings;
    
    CompilationResult runProcessors(const std::vector<ProcessorFunction>& processors, const CompilationResult& input);
};

// 调度器工厂
class DispatcherFactory {
public:
    static std::unique_ptr<CompilerDispatcher> createDefaultDispatcher();
    static std::unique_ptr<CompilerDispatcher> createProductionDispatcher();
    static std::unique_ptr<CompilerDispatcher> createDevelopmentDispatcher();
    static std::unique_ptr<CompilerDispatcher> createDebugDispatcher();
    static std::unique_ptr<CompilerDispatcher> createMinimalDispatcher();
    
    // 配置预设
    static DispatcherConfig getDefaultConfig();
    static DispatcherConfig getProductionConfig();
    static DispatcherConfig getDevelopmentConfig();
    static DispatcherConfig getDebugConfig();
    static DispatcherConfig getPerformanceConfig();
};

} // namespace CHTL