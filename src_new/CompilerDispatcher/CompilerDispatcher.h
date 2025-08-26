#pragma once

#include "../Scanner/CHTLUnifiedScanner.h"
#include "../CHTL/CHTLLexer/GlobalMap.h"
#include "../CHTL/CHTLContext/CHTLContext.h"
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <functional>

namespace CHTL {
namespace CompilerDispatcher {

// 前向声明
namespace Compiler {
    class CHTLCompiler;
    class CHTLJSCompiler;
    class CSSCompiler;
    class JavaScriptCompiler;
}

// 编译阶段
enum class CompilationPhase {
    SCANNING,           // 扫描阶段
    LEXICAL_ANALYSIS,   // 词法分析
    SYNTAX_ANALYSIS,    // 语法分析
    SEMANTIC_ANALYSIS,  // 语义分析
    OPTIMIZATION,       // 优化阶段
    CODE_GENERATION,    // 代码生成
    MERGING,           // 结果合并
    FINALIZATION       // 最终化
};

// 编译器类型
enum class CompilerType {
    CHTL,              // CHTL编译器
    CHTL_JS,           // CHTL JS编译器
    CSS,               // CSS编译器（ANTLR）
    JAVASCRIPT,        // JavaScript编译器（ANTLR）
    UNKNOWN
};

// 编译任务
struct CompilationTask {
    std::string task_id;
    Scanner::CodeFragment fragment;
    CompilerType target_compiler;
    CompilationPhase current_phase;
    std::unordered_map<std::string, std::string> metadata;
    std::vector<std::string> dependencies;
    bool is_completed;
    bool has_errors;
    
    CompilationTask() : target_compiler(CompilerType::UNKNOWN), 
                        current_phase(CompilationPhase::SCANNING),
                        is_completed(false), has_errors(false) {}
    
    CompilationTask(const std::string& id, const Scanner::CodeFragment& frag, CompilerType type)
        : task_id(id), fragment(frag), target_compiler(type),
          current_phase(CompilationPhase::SCANNING), is_completed(false), has_errors(false) {}
};

// 编译结果
struct CompilationResult {
    std::string task_id;
    CompilerType compiler_type;
    std::string generated_code;
    std::string source_map;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    std::unordered_map<std::string, std::string> metadata;
    bool success;
    
    CompilationResult() : compiler_type(CompilerType::UNKNOWN), success(false) {}
    
    void addError(const std::string& error) {
        errors.push_back(error);
        success = false;
    }
    
    void addWarning(const std::string& warning) {
        warnings.push_back(warning);
    }
    
    bool hasErrors() const { return !errors.empty(); }
    bool hasWarnings() const { return !warnings.empty(); }
};

// 编译器注册信息
struct CompilerRegistration {
    CompilerType type;
    std::string name;
    std::string version;
    std::function<void*(void)> create_instance;
    std::function<void(void*)> destroy_instance;
    std::function<CompilationResult(void*, const CompilationTask&)> compile;
    bool is_active;
    
    CompilerRegistration() : type(CompilerType::UNKNOWN), is_active(false) {}
};

// 调度统计信息
struct DispatcherStatistics {
    size_t total_tasks;
    size_t completed_tasks;
    size_t failed_tasks;
    size_t active_tasks;
    std::unordered_map<CompilerType, size_t> tasks_by_compiler;
    std::unordered_map<CompilationPhase, size_t> tasks_by_phase;
    double total_compilation_time;
    double average_task_time;
    
    DispatcherStatistics() : total_tasks(0), completed_tasks(0), failed_tasks(0), 
                            active_tasks(0), total_compilation_time(0.0), average_task_time(0.0) {}
    
    double getCompletionRate() const {
        return total_tasks > 0 ? (double)completed_tasks / total_tasks : 0.0;
    }
    
    double getFailureRate() const {
        return total_tasks > 0 ? (double)failed_tasks / total_tasks : 0.0;
    }
};

// 编译器调度器
class CompilerDispatcher {
public:
    CompilerDispatcher();
    ~CompilerDispatcher();
    
    // 初始化和配置
    bool initialize();
    void shutdown();
    bool isInitialized() const { return is_initialized; }
    
    // 编译器管理
    bool registerCompiler(const CompilerRegistration& registration);
    void unregisterCompiler(CompilerType type);
    bool isCompilerRegistered(CompilerType type) const;
    std::vector<CompilerType> getRegisteredCompilers() const;
    
    // 主编译接口
    std::vector<CompilationResult> compile(const std::string& source, 
                                          const std::string& file_path = "");
    
    std::vector<CompilationResult> compileFile(const std::string& file_path);
    
    CompilationResult compileFragment(const Scanner::CodeFragment& fragment, 
                                     CompilerType target_compiler);
    
    // 批量编译
    std::vector<CompilationResult> compileBatch(const std::vector<std::string>& sources);
    
    std::vector<CompilationResult> compileFiles(const std::vector<std::string>& file_paths);
    
    // 任务管理
    std::string submitTask(const CompilationTask& task);
    bool cancelTask(const std::string& task_id);
    CompilationResult getTaskResult(const std::string& task_id);
    std::vector<CompilationTask> getActiveTasks() const;
    
    // 依赖管理
    void addTaskDependency(const std::string& task_id, const std::string& dependency_id);
    std::vector<std::string> getTaskDependencies(const std::string& task_id) const;
    bool hasCircularDependency(const std::string& task_id) const;
    
    // 调度策略
    void setParallelCompilation(bool enabled) { parallel_compilation = enabled; }
    void setMaxConcurrentTasks(size_t max_tasks) { max_concurrent_tasks = max_tasks; }
    void setPriority(CompilerType compiler, int priority);
    
    // 结果合并
    std::string mergeResults(const std::vector<CompilationResult>& results);
    std::string generateHTML(const std::vector<CompilationResult>& results);
    
    // 错误处理和诊断
    std::vector<std::string> getAllErrors() const;
    std::vector<std::string> getAllWarnings() const;
    void clearErrors();
    void clearWarnings();
    
    // 统计和监控
    DispatcherStatistics getStatistics() const;
    void resetStatistics();
    bool isTaskRunning(const std::string& task_id) const;
    double getCompilerUtilization(CompilerType type) const;
    
    // 调试和日志
    void setDebugMode(bool debug) { debug_mode = debug; }
    void setLogLevel(int level) { log_level = level; }
    void dumpTaskQueue() const;
    void dumpCompilerStates() const;
    
    // 配置
    void setScanner(Scanner::CHTLUnifiedScanner* scanner) { this->scanner = scanner; }
    
private:
    // 状态标志
    bool is_initialized;
    bool debug_mode;
    bool parallel_compilation;
    int log_level;
    size_t max_concurrent_tasks;
    
    // 核心组件
    std::unique_ptr<Scanner::CHTLUnifiedScanner> scanner;
    std::unique_ptr<CHTL::Context::CHTLContext> context;
    
    // 编译器注册表
    std::unordered_map<CompilerType, CompilerRegistration> registered_compilers;
    std::unordered_map<CompilerType, void*> compiler_instances;
    std::unordered_map<CompilerType, int> compiler_priorities;
    
    // 任务管理
    std::unordered_map<std::string, CompilationTask> active_tasks;
    std::unordered_map<std::string, CompilationResult> completed_results;
    std::unordered_map<std::string, std::vector<std::string>> task_dependencies;
    std::vector<std::string> task_queue;
    size_t task_counter;
    
    // 统计信息
    DispatcherStatistics statistics;
    std::unordered_map<CompilerType, double> compiler_usage_times;
    
    // 错误和警告
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    // 内部方法
    
    // 扫描和分析
    std::vector<Scanner::CodeFragment> scanSource(const std::string& source, 
                                                  const std::string& file_path);
    
    std::vector<CompilationTask> createTasks(const std::vector<Scanner::CodeFragment>& fragments);
    
    CompilerType determineCompilerType(const Scanner::CodeFragment& fragment) const;
    
    // 任务调度
    void scheduleTask(const CompilationTask& task);
    void executeTasks();
    std::vector<std::string> getExecutableTaskIds() const;
    bool canExecuteTask(const std::string& task_id) const;
    
    // 编译执行
    CompilationResult executeTask(const CompilationTask& task);
    CompilationResult compileCHTL(const CompilationTask& task);
    CompilationResult compileCHTLJS(const CompilationTask& task);
    CompilationResult compileCSS(const CompilationTask& task);
    CompilationResult compileJavaScript(const CompilationTask& task);
    
    // 依赖解析
    std::vector<std::string> resolveDependencies(const std::string& task_id) const;
    bool hasCyclicDependency(const std::string& task_id, 
                            std::unordered_map<std::string, int>& visited) const;
    
    // 结果处理
    void processResult(const CompilationResult& result);
    void updateStatistics(const CompilationResult& result, double compilation_time);
    
    // 合并逻辑
    std::string mergeHTML(const std::vector<CompilationResult>& results);
    std::string mergeCSS(const std::vector<CompilationResult>& results);
    std::string mergeJavaScript(const std::vector<CompilationResult>& results);
    std::string generateSourceMap(const std::vector<CompilationResult>& results);
    
    // 错误处理
    void addError(const std::string& message);
    void addWarning(const std::string& message);
    void handleCompilerError(CompilerType compiler, const std::string& error);
    
    // 工具方法
    std::string generateTaskId();
    std::string getCompilerTypeName(CompilerType type) const;
    std::string getPhaseName(CompilationPhase phase) const;
    
    // 验证
    bool validateTask(const CompilationTask& task) const;
    bool validateFragment(const Scanner::CodeFragment& fragment) const;
    
    // 调试和日志
    void logMessage(int level, const std::string& message) const;
    void debugLog(const std::string& message) const;
    
    // 性能监控
    void startTimer(const std::string& name);
    double stopTimer(const std::string& name);
    std::unordered_map<std::string, std::chrono::high_resolution_clock::time_point> timers;
    
    // 清理
    void cleanupCompletedTasks();
    void cleanupCompilerInstances();
};

// 辅助函数和工具
class DispatcherUtils {
public:
    // 获取编译器类型名称
    static std::string getCompilerTypeName(CompilerType type);
    
    // 获取编译阶段名称
    static std::string getPhaseName(CompilationPhase phase);
    
    // 创建默认编译器注册
    static std::vector<CompilerRegistration> createDefaultRegistrations();
    
    // 验证编译器兼容性
    static bool isCompilerCompatible(CompilerType type, const Scanner::CodeFragment& fragment);
    
    // 估算编译复杂度
    static size_t estimateCompilationComplexity(const Scanner::CodeFragment& fragment);
    
    // 优化建议
    static std::vector<std::string> getOptimizationSuggestions(const DispatcherStatistics& stats);
    
    // 性能分析
    static std::unordered_map<std::string, double> analyzePerformance(
        const std::vector<CompilationResult>& results);
};

} // namespace CompilerDispatcher
} // namespace CHTL