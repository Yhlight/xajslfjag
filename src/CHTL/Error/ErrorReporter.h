#pragma once
#include "ErrorReport.h"
#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <unordered_map>
#include <fstream>

namespace CHTL {

/**
 * 错误级别枚举
 */
enum class ErrorLevel {
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    CRITICAL
};

/**
 * 错误上下文信息
 */
struct ErrorContext {
    std::string fileName;                           // 文件名
    int lineNumber;                                 // 行号
    int columnNumber;                               // 列号
    std::string functionName;                       // 函数名
    std::string codeSnippet;                        // 代码片段
    std::chrono::system_clock::time_point timestamp; // 时间戳
    
    ErrorContext() : lineNumber(0), columnNumber(0), timestamp(std::chrono::system_clock::now()) {}
};

/**
 * 详细错误信息
 */
struct DetailedError {
    ErrorType type;                                 // 错误类型
    ErrorLevel level;                               // 错误级别
    std::string message;                            // 错误消息
    ErrorContext context;                           // 错误上下文
    std::vector<std::string> suggestions;           // 修复建议
    std::string stackTrace;                         // 堆栈跟踪
    
    DetailedError(ErrorType t, ErrorLevel l, const std::string& msg)
        : type(t), level(l), message(msg) {}
};

/**
 * 错误统计信息
 */
struct ErrorStatistics {
    std::unordered_map<ErrorType, int> errorCounts; // 错误类型计数
    std::unordered_map<ErrorLevel, int> levelCounts; // 错误级别计数
    int totalErrors;                                 // 总错误数
    std::chrono::system_clock::time_point startTime; // 开始时间
    
    ErrorStatistics() : totalErrors(0), startTime(std::chrono::system_clock::now()) {}
};

/**
 * 增强的错误报告器
 */
class ErrorReporter {
public:
    ErrorReporter();
    ~ErrorReporter() = default;
    
    /**
     * 报告错误（兼容原接口）
     */
    void error(ErrorType type, const std::string& message);
    
    /**
     * 详细错误报告
     */
    void reportError(ErrorType type, ErrorLevel level, const std::string& message, const ErrorContext& context = {});
    
    /**
     * 报告警告
     */
    void warning(const std::string& message, const ErrorContext& context = {});
    
    /**
     * 报告信息
     */
    void info(const std::string& message, const ErrorContext& context = {});
    
    /**
     * 报告调试信息
     */
    void debug(const std::string& message, const ErrorContext& context = {});
    
    /**
     * 添加修复建议
     */
    void addSuggestion(const std::string& suggestion);
    
    /**
     * 设置错误上下文
     */
    void setContext(const ErrorContext& context);
    
    /**
     * 获取所有错误
     */
    const std::vector<DetailedError>& getAllErrors() const { return m_errors; }
    
    /**
     * 获取错误统计
     */
    const ErrorStatistics& getStatistics() const { return m_statistics; }
    
    /**
     * 清理错误
     */
    void clearErrors();
    
    /**
     * 是否有错误
     */
    bool hasErrors() const;
    
    /**
     * 是否有关键错误
     */
    bool hasCriticalErrors() const;
    
    /**
     * 获取错误数量
     */
    int getErrorCount(ErrorLevel level = ErrorLevel::ERROR) const;
    
    /**
     * 格式化错误报告
     */
    std::string formatErrorReport(bool includeStackTrace = false) const;
    
    /**
     * 导出错误报告到文件
     */
    bool exportToFile(const std::string& filename) const;
    
    /**
     * 设置日志级别
     */
    void setLogLevel(ErrorLevel level);
    
    /**
     * 启用/禁用控制台输出
     */
    void setConsoleOutput(bool enabled);
    
    /**
     * 启用/禁用文件日志
     */
    void setFileLogging(bool enabled, const std::string& logFile = "chtl_errors.log");

private:
    std::vector<DetailedError> m_errors;            // 错误列表
    ErrorStatistics m_statistics;                   // 错误统计
    ErrorContext m_currentContext;                  // 当前上下文
    ErrorLevel m_logLevel;                          // 日志级别
    bool m_consoleOutput;                           // 控制台输出
    bool m_fileLogging;                             // 文件日志
    std::string m_logFile;                          // 日志文件
    std::unique_ptr<std::ofstream> m_logStream;     // 日志流
    
    /**
     * 更新统计信息
     */
    void updateStatistics(const DetailedError& error);
    
    /**
     * 输出到控制台
     */
    void outputToConsole(const DetailedError& error) const;
    
    /**
     * 输出到文件
     */
    void outputToFile(const DetailedError& error);
    
    /**
     * 格式化单个错误
     */
    std::string formatSingleError(const DetailedError& error, bool includeStackTrace = false) const;
    
    /**
     * 获取错误级别字符串
     */
    std::string getLevelString(ErrorLevel level) const;
    
    /**
     * 获取错误类型字符串
     */
    std::string getTypeString(ErrorType type) const;
    
    /**
     * 生成堆栈跟踪
     */
    std::string generateStackTrace() const;
};

/**
 * 性能监控器
 */
class PerformanceMonitor {
public:
    PerformanceMonitor();
    ~PerformanceMonitor() = default;
    
    /**
     * 开始计时
     */
    void startTimer(const std::string& name);
    
    /**
     * 结束计时
     */
    void endTimer(const std::string& name);
    
    /**
     * 记录内存使用
     */
    void recordMemoryUsage(const std::string& label);
    
    /**
     * 记录自定义指标
     */
    void recordMetric(const std::string& name, double value);
    
    /**
     * 获取计时结果
     */
    double getTimerResult(const std::string& name) const;
    
    /**
     * 获取内存使用
     */
    size_t getMemoryUsage(const std::string& label) const;
    
    /**
     * 获取性能报告
     */
    std::string getPerformanceReport() const;
    
    /**
     * 清理性能数据
     */
    void clearMetrics();
    
    /**
     * 导出性能数据
     */
    bool exportToFile(const std::string& filename) const;

private:
    struct TimerData {
        std::chrono::high_resolution_clock::time_point startTime;
        std::chrono::high_resolution_clock::time_point endTime;
        bool isRunning;
        
        TimerData() : isRunning(false) {}
    };
    
    std::unordered_map<std::string, TimerData> m_timers;           // 计时器
    std::unordered_map<std::string, size_t> m_memoryUsage;        // 内存使用
    std::unordered_map<std::string, double> m_customMetrics;      // 自定义指标
    
    /**
     * 获取当前内存使用量
     */
    size_t getCurrentMemoryUsage() const;
    
    /**
     * 格式化时间
     */
    std::string formatDuration(double milliseconds) const;
    
    /**
     * 格式化内存大小
     */
    std::string formatMemorySize(size_t bytes) const;
};

/**
 * 内存管理器
 */
class MemoryManager {
public:
    MemoryManager();
    ~MemoryManager() = default;
    
    /**
     * 记录内存分配
     */
    void recordAllocation(void* ptr, size_t size, const std::string& category = "");
    
    /**
     * 记录内存释放
     */
    void recordDeallocation(void* ptr);
    
    /**
     * 获取总分配内存
     */
    size_t getTotalAllocated() const;
    
    /**
     * 获取当前使用内存
     */
    size_t getCurrentUsage() const;
    
    /**
     * 获取峰值内存使用
     */
    size_t getPeakUsage() const;
    
    /**
     * 检查内存泄漏
     */
    std::vector<std::string> checkMemoryLeaks() const;
    
    /**
     * 获取内存报告
     */
    std::string getMemoryReport() const;
    
    /**
     * 清理内存统计
     */
    void clearStatistics();

private:
    struct AllocationInfo {
        size_t size;
        std::string category;
        std::chrono::system_clock::time_point timestamp;
        
        AllocationInfo(size_t s, const std::string& cat)
            : size(s), category(cat), timestamp(std::chrono::system_clock::now()) {}
    };
    
    std::unordered_map<void*, AllocationInfo> m_allocations;  // 分配记录
    size_t m_totalAllocated;                                  // 总分配量
    size_t m_currentUsage;                                    // 当前使用量
    size_t m_peakUsage;                                       // 峰值使用量
    std::unordered_map<std::string, size_t> m_categoryUsage; // 分类使用量
    
    /**
     * 更新峰值使用量
     */
    void updatePeakUsage();
};

/**
 * 编译器优化器
 */
class CompilerOptimizer {
public:
    CompilerOptimizer();
    ~CompilerOptimizer() = default;
    
    /**
     * 优化字符串操作
     */
    std::string optimizeString(const std::string& input);
    
    /**
     * 缓存频繁访问的数据
     */
    template<typename K, typename V>
    void cacheData(const K& key, const V& value);
    
    /**
     * 获取缓存数据
     */
    template<typename K, typename V>
    bool getCachedData(const K& key, V& value) const;
    
    /**
     * 预编译正则表达式
     */
    void precompileRegex(const std::string& name, const std::string& pattern);
    
    /**
     * 使用预编译正则表达式
     */
    bool matchPrecompiledRegex(const std::string& name, const std::string& text) const;
    
    /**
     * 批量处理优化
     */
    template<typename T>
    std::vector<T> batchProcess(const std::vector<T>& items, std::function<T(const T&)> processor);
    
    /**
     * 内存池分配
     */
    void* allocateFromPool(size_t size);
    
    /**
     * 清理内存池
     */
    void clearMemoryPool();
    
    /**
     * 获取优化统计
     */
    std::string getOptimizationStats() const;

private:
    std::unordered_map<std::string, std::any> m_cache;        // 通用缓存
    std::unordered_map<std::string, std::regex> m_regexCache; // 正则表达式缓存
    std::vector<std::unique_ptr<char[]>> m_memoryPool;        // 内存池
    size_t m_poolSize;                                        // 池大小
    std::unordered_map<std::string, int> m_optimizationStats; // 优化统计
    
    /**
     * 更新优化统计
     */
    void updateOptimizationStats(const std::string& category);
};

/**
 * 全局错误和性能管理器
 */
class GlobalManager {
public:
    static GlobalManager& getInstance();
    
    /**
     * 获取错误报告器
     */
    ErrorReporter& getErrorReporter() { return *m_errorReporter; }
    
    /**
     * 获取性能监控器
     */
    PerformanceMonitor& getPerformanceMonitor() { return *m_performanceMonitor; }
    
    /**
     * 获取内存管理器
     */
    MemoryManager& getMemoryManager() { return *m_memoryManager; }
    
    /**
     * 获取编译器优化器
     */
    CompilerOptimizer& getCompilerOptimizer() { return *m_compilerOptimizer; }
    
    /**
     * 初始化全局管理器
     */
    void initialize();
    
    /**
     * 清理全局管理器
     */
    void cleanup();
    
    /**
     * 生成综合报告
     */
    std::string generateComprehensiveReport() const;

private:
    GlobalManager();
    ~GlobalManager() = default;
    
    std::unique_ptr<ErrorReporter> m_errorReporter;
    std::unique_ptr<PerformanceMonitor> m_performanceMonitor;
    std::unique_ptr<MemoryManager> m_memoryManager;
    std::unique_ptr<CompilerOptimizer> m_compilerOptimizer;
    
    bool m_initialized;
};

// 便利宏定义
#define CHTL_ERROR(type, message) \
    CHTL::GlobalManager::getInstance().getErrorReporter().error(type, message)

#define CHTL_WARNING(message) \
    CHTL::GlobalManager::getInstance().getErrorReporter().warning(message)

#define CHTL_INFO(message) \
    CHTL::GlobalManager::getInstance().getErrorReporter().info(message)

#define CHTL_DEBUG(message) \
    CHTL::GlobalManager::getInstance().getErrorReporter().debug(message)

#define CHTL_START_TIMER(name) \
    CHTL::GlobalManager::getInstance().getPerformanceMonitor().startTimer(name)

#define CHTL_END_TIMER(name) \
    CHTL::GlobalManager::getInstance().getPerformanceMonitor().endTimer(name)

#define CHTL_RECORD_MEMORY(label) \
    CHTL::GlobalManager::getInstance().getPerformanceMonitor().recordMemoryUsage(label)

} // namespace CHTL