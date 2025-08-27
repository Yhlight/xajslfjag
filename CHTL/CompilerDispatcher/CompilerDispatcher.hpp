#pragma once
#include "../Scanner/Scanner.hpp"
#include <memory>
#include <unordered_map>
#include <functional>

namespace CHTL {

// 前向声明
class CHTLCompiler;
class CHTLJSCompiler;
class CSSCompiler;
class JavaScriptCompiler;

/**
 * 编译结果结构
 */
struct CompilationResult {
    bool success;
    std::string output;
    std::string errorMessage;
    std::vector<std::string> warnings;
    size_t processingTime; // 毫秒
    
    CompilationResult() : success(false), processingTime(0) {}
    
    void addWarning(const std::string& warning) {
        warnings.push_back(warning);
    }
    
    bool hasWarnings() const {
        return !warnings.empty();
    }
};

/**
 * 编译器接口
 * 所有编译器的基类
 */
class ICompiler {
public:
    virtual ~ICompiler() = default;
    virtual CompilationResult compile(const CodeFragment& fragment) = 0;
    virtual bool canCompile(FragmentType type) const = 0;
    virtual std::string getName() const = 0;
    virtual void configure(const std::unordered_map<std::string, std::string>& config) = 0;
};

/**
 * 编译器调度器
 * 负责将不同类型的代码片段分发给相应的编译器
 * 并协调编译过程，最终合并结果
 */
class CompilerDispatcher {
private:
    std::unordered_map<FragmentType, std::unique_ptr<ICompiler>> compilers;
    std::unique_ptr<CHTLUnifiedScanner> scanner;
    
    // 编译配置
    bool parallelCompilation;
    bool optimizeOutput;
    bool generateSourceMaps;
    bool minifyOutput;
    
    // 结果管理
    std::vector<CompilationResult> compilationResults;
    std::string mergedOutput;
    
    // 统计信息
    struct Statistics {
        size_t totalFragments;
        size_t successfulCompilations;
        size_t failedCompilations;
        size_t totalWarnings;
        size_t totalProcessingTime;
        
        Statistics() : totalFragments(0), successfulCompilations(0), 
                      failedCompilations(0), totalWarnings(0), totalProcessingTime(0) {}
    } stats;

public:
    CompilerDispatcher();
    ~CompilerDispatcher();
    
    // 编译器注册
    void registerCompiler(FragmentType type, std::unique_ptr<ICompiler> compiler);
    void unregisterCompiler(FragmentType type);
    bool hasCompiler(FragmentType type) const;
    ICompiler* getCompiler(FragmentType type) const;
    
    // 主编译流程
    CompilationResult compileSource(const std::string& source);
    CompilationResult compileFragments(const std::vector<CodeFragment>& fragments);
    
    // 配置
    void setParallelCompilation(bool enable);
    void setOptimizeOutput(bool enable);
    void setGenerateSourceMaps(bool enable);
    void setMinifyOutput(bool enable);
    
    bool getParallelCompilation() const;
    bool getOptimizeOutput() const;
    bool getGenerateSourceMaps() const;
    bool getMinifyOutput() const;
    
    // 扫描器配置
    void configureScanner(const std::unordered_map<std::string, std::string>& config);
    CHTLUnifiedScanner* getScanner() const;
    
    // 结果获取
    const std::string& getMergedOutput() const;
    const std::vector<CompilationResult>& getCompilationResults() const;
    const Statistics& getStatistics() const;
    
    // 结果处理
    std::string generateHTML() const;
    std::string generateCSS() const;
    std::string generateJavaScript() const;
    
    // 优化和后处理
    void optimizeOutput();
    void minifyOutput();
    void generateSourceMap();
    
    // 错误处理
    bool hasErrors() const;
    std::vector<std::string> getErrors() const;
    bool hasWarnings() const;
    std::vector<std::string> getWarnings() const;
    
    // 调试和监控
    void dumpStatistics() const;
    void enableDebugMode(bool enable);
    void setVerbose(bool verbose);
    
    // 重置
    void reset();
    void clearResults();

private:
    bool debugMode;
    bool verbose;
    
    // 内部编译方法
    CompilationResult compileFragment(const CodeFragment& fragment);
    void mergeResults();
    void updateStatistics(const CompilationResult& result);
    
    // 并行编译支持
    std::vector<CompilationResult> compileFragmentsParallel(const std::vector<CodeFragment>& fragments);
    std::vector<CompilationResult> compileFragmentsSequential(const std::vector<CodeFragment>& fragments);
    
    // 结果合并
    void mergeHTML();
    void mergeCSS();
    void mergeJavaScript();
    
    // 优化方法
    std::string optimizeHTML(const std::string& html) const;
    std::string optimizeCSS(const std::string& css) const;
    std::string optimizeJavaScript(const std::string& js) const;
    
    // 初始化
    void initializeDefaultCompilers();
    void configureDefaultSettings();
    
    // 工具方法
    void reportError(const std::string& error);
    void reportWarning(const std::string& warning);
    void logMessage(const std::string& message) const;
};

/**
 * 编译器工厂
 * 创建和配置各种编译器实例
 */
class CompilerFactory {
public:
    // 创建编译器
    static std::unique_ptr<ICompiler> createCHTLCompiler();
    static std::unique_ptr<ICompiler> createCHTLJSCompiler();
    static std::unique_ptr<ICompiler> createCSSCompiler();
    static std::unique_ptr<ICompiler> createJavaScriptCompiler();
    
    // 创建调度器
    static std::unique_ptr<CompilerDispatcher> createDefaultDispatcher();
    static std::unique_ptr<CompilerDispatcher> createOptimizedDispatcher();
    static std::unique_ptr<CompilerDispatcher> createDebugDispatcher();
    
    // 配置编译器
    static void configureForProduction(CompilerDispatcher& dispatcher);
    static void configureForDevelopment(CompilerDispatcher& dispatcher);
    static void configureForDebugging(CompilerDispatcher& dispatcher);
};

/**
 * 编译任务
 * 表示一个编译任务，支持异步编译
 */
class CompilationTask {
private:
    std::string source;
    std::unique_ptr<CompilerDispatcher> dispatcher;
    std::function<void(const CompilationResult&)> callback;
    bool isAsync;
    
public:
    CompilationTask(const std::string& source, 
                   std::unique_ptr<CompilerDispatcher> dispatcher,
                   bool async = false);
    
    // 任务执行
    CompilationResult execute();
    void executeAsync(std::function<void(const CompilationResult&)> callback);
    
    // 任务配置
    void setCallback(std::function<void(const CompilationResult&)> callback);
    void setAsync(bool async);
    
    // 状态查询
    bool isAsyncTask() const;
    const std::string& getSource() const;
};

}