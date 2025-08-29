#include "ErrorReporter.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cstring>

#ifdef _WIN32
#include <windows.h>
#include <psapi.h>
#else
#include <unistd.h>
#include <sys/resource.h>
#include <cstdlib>
#endif

namespace CHTL {

// ========== ErrorReporter 实现 ==========

ErrorReporter::ErrorReporter()
    : m_logLevel(ErrorLevel::INFO)
    , m_consoleOutput(true)
    , m_fileLogging(false) {
}

void ErrorReporter::error(ErrorType type, const std::string& message) {
    reportError(type, ErrorLevel::ERROR, message);
}

void ErrorReporter::reportError(ErrorType type, ErrorLevel level, const std::string& message, const ErrorContext& context) {
    DetailedError error(type, level, message);
    error.context = context;
    
    // 如果没有设置上下文，使用当前上下文
    if (error.context.fileName.empty() && !m_currentContext.fileName.empty()) {
        error.context = m_currentContext;
    }
    
    // 生成堆栈跟踪（仅对错误和关键错误）
    if (level >= ErrorLevel::ERROR) {
        error.stackTrace = generateStackTrace();
    }
    
    m_errors.push_back(error);
    updateStatistics(error);
    
    // 输出错误（根据日志级别）
    if (level >= m_logLevel) {
        if (m_consoleOutput) {
            outputToConsole(error);
        }
        
        if (m_fileLogging) {
            outputToFile(error);
        }
    }
}

void ErrorReporter::warning(const std::string& message, const ErrorContext& context) {
    reportError(ErrorType::SYNTAX_WARNING, ErrorLevel::WARNING, message, context);
}

void ErrorReporter::info(const std::string& message, const ErrorContext& context) {
    reportError(ErrorType::INFO, ErrorLevel::INFO, message, context);
}

void ErrorReporter::debug(const std::string& message, const ErrorContext& context) {
    reportError(ErrorType::DEBUG, ErrorLevel::DEBUG, message, context);
}

void ErrorReporter::setContext(const ErrorContext& context) {
    m_currentContext = context;
}

void ErrorReporter::clearErrors() {
    m_errors.clear();
    m_statistics = ErrorStatistics();
}

bool ErrorReporter::hasErrors() const {
    return getErrorCount(ErrorLevel::ERROR) > 0;
}

bool ErrorReporter::hasCriticalErrors() const {
    return getErrorCount(ErrorLevel::CRITICAL) > 0;
}

int ErrorReporter::getErrorCount(ErrorLevel level) const {
    auto it = m_statistics.levelCounts.find(level);
    return (it != m_statistics.levelCounts.end()) ? it->second : 0;
}

std::string ErrorReporter::formatErrorReport(bool includeStackTrace) const {
    std::ostringstream ss;
    
    ss << "=== CHTL Error Report ===\n";
    ss << "Generated: " << std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now().time_since_epoch()).count() << "\n";
    ss << "Total Errors: " << m_statistics.totalErrors << "\n\n";
    
    // 错误统计
    ss << "Error Statistics:\n";
    for (const auto& [level, count] : m_statistics.levelCounts) {
        ss << "  " << getLevelString(level) << ": " << count << "\n";
    }
    ss << "\n";
    
    // 详细错误列表
    ss << "Detailed Error List:\n";
    ss << "-------------------\n";
    
    for (const auto& error : m_errors) {
        ss << formatSingleError(error, includeStackTrace) << "\n";
    }
    
    return ss.str();
}

bool ErrorReporter::exportToFile(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) return false;
    
    file << formatErrorReport(true);
    return true;
}

void ErrorReporter::setLogLevel(ErrorLevel level) {
    m_logLevel = level;
}

void ErrorReporter::setConsoleOutput(bool enabled) {
    m_consoleOutput = enabled;
}

void ErrorReporter::setFileLogging(bool enabled, const std::string& logFile) {
    m_fileLogging = enabled;
    m_logFile = logFile;
    
    if (enabled && !m_logStream) {
        m_logStream = std::make_unique<std::ofstream>(logFile, std::ios::app);
    } else if (!enabled && m_logStream) {
        m_logStream.reset();
    }
}

void ErrorReporter::updateStatistics(const DetailedError& error) {
    m_statistics.totalErrors++;
    m_statistics.errorCounts[error.type]++;
    m_statistics.levelCounts[error.level]++;
}

void ErrorReporter::outputToConsole(const DetailedError& error) const {
    std::string colorCode;
    
    // 根据错误级别设置颜色（ANSI颜色代码）
    switch (error.level) {
        case ErrorLevel::DEBUG:
            colorCode = "\033[36m";  // 青色
            break;
        case ErrorLevel::INFO:
            colorCode = "\033[32m";  // 绿色
            break;
        case ErrorLevel::WARNING:
            colorCode = "\033[33m";  // 黄色
            break;
        case ErrorLevel::ERROR:
            colorCode = "\033[31m";  // 红色
            break;
        case ErrorLevel::CRITICAL:
            colorCode = "\033[35m";  // 紫色
            break;
    }
    
    std::cerr << colorCode << "[" << getLevelString(error.level) << "] " 
              << error.message << "\033[0m";  // 重置颜色
    
    if (!error.context.fileName.empty()) {
        std::cerr << " (" << error.context.fileName;
        if (error.context.lineNumber > 0) {
            std::cerr << ":" << error.context.lineNumber;
            if (error.context.columnNumber > 0) {
                std::cerr << ":" << error.context.columnNumber;
            }
        }
        std::cerr << ")";
    }
    
    std::cerr << std::endl;
}

void ErrorReporter::outputToFile(const DetailedError& error) {
    if (!m_logStream || !m_logStream->is_open()) return;
    
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    
    *m_logStream << "[" << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S") << "] "
                 << "[" << getLevelString(error.level) << "] "
                 << error.message;
    
    if (!error.context.fileName.empty()) {
        *m_logStream << " (" << error.context.fileName;
        if (error.context.lineNumber > 0) {
            *m_logStream << ":" << error.context.lineNumber;
            if (error.context.columnNumber > 0) {
                *m_logStream << ":" << error.context.columnNumber;
            }
        }
        *m_logStream << ")";
    }
    
    *m_logStream << std::endl;
    m_logStream->flush();
}

std::string ErrorReporter::formatSingleError(const DetailedError& error, bool includeStackTrace) const {
    std::ostringstream ss;
    
    ss << "[" << getLevelString(error.level) << "] " << error.message;
    
    if (!error.context.fileName.empty()) {
        ss << "\n  File: " << error.context.fileName;
        if (error.context.lineNumber > 0) {
            ss << ":" << error.context.lineNumber;
            if (error.context.columnNumber > 0) {
                ss << ":" << error.context.columnNumber;
            }
        }
    }
    
    if (!error.context.functionName.empty()) {
        ss << "\n  Function: " << error.context.functionName;
    }
    
    if (!error.context.codeSnippet.empty()) {
        ss << "\n  Code: " << error.context.codeSnippet;
    }
    
    if (!error.suggestions.empty()) {
        ss << "\n  Suggestions:";
        for (const auto& suggestion : error.suggestions) {
            ss << "\n    - " << suggestion;
        }
    }
    
    if (includeStackTrace && !error.stackTrace.empty()) {
        ss << "\n  Stack Trace:\n" << error.stackTrace;
    }
    
    return ss.str();
}

std::string ErrorReporter::getLevelString(ErrorLevel level) const {
    switch (level) {
        case ErrorLevel::DEBUG: return "DEBUG";
        case ErrorLevel::INFO: return "INFO";
        case ErrorLevel::WARNING: return "WARNING";
        case ErrorLevel::ERROR: return "ERROR";
        case ErrorLevel::CRITICAL: return "CRITICAL";
        default: return "UNKNOWN";
    }
}

std::string ErrorReporter::getTypeString(ErrorType type) const {
    switch (type) {
        case ErrorType::SYNTAX_ERROR: return "SYNTAX_ERROR";
        case ErrorType::SEMANTIC_ERROR: return "SEMANTIC_ERROR";
        case ErrorType::RUNTIME_ERROR: return "RUNTIME_ERROR";
        case ErrorType::IO_ERROR: return "IO_ERROR";
        case ErrorType::MEMORY_ERROR: return "MEMORY_ERROR";
        case ErrorType::SYNTAX_WARNING: return "SYNTAX_WARNING";
        case ErrorType::INFO: return "INFO";
        case ErrorType::DEBUG: return "DEBUG";
        default: return "UNKNOWN";
    }
}

std::string ErrorReporter::generateStackTrace() const {
    // 简化的堆栈跟踪实现
    // 在实际项目中，可以使用专门的库如 libbacktrace 或 boost::stacktrace
    std::ostringstream ss;
    ss << "  Stack trace not available (simplified implementation)";
    return ss.str();
}

// ========== PerformanceMonitor 实现 ==========

PerformanceMonitor::PerformanceMonitor() {
}

void PerformanceMonitor::startTimer(const std::string& name) {
    auto& timer = m_timers[name];
    timer.startTime = std::chrono::high_resolution_clock::now();
    timer.isRunning = true;
}

void PerformanceMonitor::endTimer(const std::string& name) {
    auto it = m_timers.find(name);
    if (it != m_timers.end() && it->second.isRunning) {
        it->second.endTime = std::chrono::high_resolution_clock::now();
        it->second.isRunning = false;
    }
}

void PerformanceMonitor::recordMemoryUsage(const std::string& label) {
    m_memoryUsage[label] = getCurrentMemoryUsage();
}

void PerformanceMonitor::recordMetric(const std::string& name, double value) {
    m_customMetrics[name] = value;
}

double PerformanceMonitor::getTimerResult(const std::string& name) const {
    auto it = m_timers.find(name);
    if (it != m_timers.end() && !it->second.isRunning) {
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
            it->second.endTime - it->second.startTime);
        return duration.count() / 1000.0; // 返回毫秒
    }
    return -1.0;
}

size_t PerformanceMonitor::getMemoryUsage(const std::string& label) const {
    auto it = m_memoryUsage.find(label);
    return (it != m_memoryUsage.end()) ? it->second : 0;
}

std::string PerformanceMonitor::getPerformanceReport() const {
    std::ostringstream ss;
    
    ss << "=== Performance Report ===\n";
    
    // 计时器结果
    ss << "\nTimer Results:\n";
    for (const auto& [name, timer] : m_timers) {
        if (!timer.isRunning) {
            double duration = getTimerResult(name);
            ss << "  " << name << ": " << formatDuration(duration) << "\n";
        }
    }
    
    // 内存使用
    ss << "\nMemory Usage:\n";
    for (const auto& [label, usage] : m_memoryUsage) {
        ss << "  " << label << ": " << formatMemorySize(usage) << "\n";
    }
    
    // 自定义指标
    ss << "\nCustom Metrics:\n";
    for (const auto& [name, value] : m_customMetrics) {
        ss << "  " << name << ": " << value << "\n";
    }
    
    return ss.str();
}

void PerformanceMonitor::clearMetrics() {
    m_timers.clear();
    m_memoryUsage.clear();
    m_customMetrics.clear();
}

bool PerformanceMonitor::exportToFile(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) return false;
    
    file << getPerformanceReport();
    return true;
}

size_t PerformanceMonitor::getCurrentMemoryUsage() const {
#ifdef _WIN32
    PROCESS_MEMORY_COUNTERS pmc;
    if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc))) {
        return pmc.WorkingSetSize;
    }
#else
    struct rusage usage;
    if (getrusage(RUSAGE_SELF, &usage) == 0) {
        return usage.ru_maxrss * 1024; // ru_maxrss is in KB on Linux
    }
#endif
    return 0;
}

std::string PerformanceMonitor::formatDuration(double milliseconds) const {
    if (milliseconds < 1.0) {
        return std::to_string(milliseconds * 1000.0) + " μs";
    } else if (milliseconds < 1000.0) {
        return std::to_string(milliseconds) + " ms";
    } else {
        return std::to_string(milliseconds / 1000.0) + " s";
    }
}

std::string PerformanceMonitor::formatMemorySize(size_t bytes) const {
    const char* units[] = {"B", "KB", "MB", "GB", "TB"};
    int unit = 0;
    double size = static_cast<double>(bytes);
    
    while (size >= 1024.0 && unit < 4) {
        size /= 1024.0;
        unit++;
    }
    
    std::ostringstream ss;
    ss << std::fixed << std::setprecision(2) << size << " " << units[unit];
    return ss.str();
}

// ========== MemoryManager 实现 ==========

MemoryManager::MemoryManager()
    : m_totalAllocated(0), m_currentUsage(0), m_peakUsage(0) {
}

void MemoryManager::recordAllocation(void* ptr, size_t size, const std::string& category) {
    if (ptr) {
        m_allocations[ptr] = AllocationInfo(size, category);
        m_totalAllocated += size;
        m_currentUsage += size;
        m_categoryUsage[category] += size;
        updatePeakUsage();
    }
}

void MemoryManager::recordDeallocation(void* ptr) {
    auto it = m_allocations.find(ptr);
    if (it != m_allocations.end()) {
        m_currentUsage -= it->second.size;
        m_categoryUsage[it->second.category] -= it->second.size;
        m_allocations.erase(it);
    }
}

size_t MemoryManager::getTotalAllocated() const {
    return m_totalAllocated;
}

size_t MemoryManager::getCurrentUsage() const {
    return m_currentUsage;
}

size_t MemoryManager::getPeakUsage() const {
    return m_peakUsage;
}

std::vector<std::string> MemoryManager::checkMemoryLeaks() const {
    std::vector<std::string> leaks;
    
    for (const auto& [ptr, info] : m_allocations) {
        std::ostringstream ss;
        ss << "Memory leak: " << info.size << " bytes in category '" 
           << info.category << "' allocated at " << ptr;
        leaks.push_back(ss.str());
    }
    
    return leaks;
}

std::string MemoryManager::getMemoryReport() const {
    std::ostringstream ss;
    
    ss << "=== Memory Report ===\n";
    ss << "Total Allocated: " << m_totalAllocated << " bytes\n";
    ss << "Current Usage: " << m_currentUsage << " bytes\n";
    ss << "Peak Usage: " << m_peakUsage << " bytes\n";
    ss << "Active Allocations: " << m_allocations.size() << "\n\n";
    
    ss << "Category Usage:\n";
    for (const auto& [category, usage] : m_categoryUsage) {
        ss << "  " << category << ": " << usage << " bytes\n";
    }
    
    auto leaks = checkMemoryLeaks();
    if (!leaks.empty()) {
        ss << "\nMemory Leaks Detected:\n";
        for (const auto& leak : leaks) {
            ss << "  " << leak << "\n";
        }
    }
    
    return ss.str();
}

void MemoryManager::clearStatistics() {
    m_allocations.clear();
    m_totalAllocated = 0;
    m_currentUsage = 0;
    m_peakUsage = 0;
    m_categoryUsage.clear();
}

void MemoryManager::updatePeakUsage() {
    if (m_currentUsage > m_peakUsage) {
        m_peakUsage = m_currentUsage;
    }
}

// ========== GlobalManager 实现 ==========

GlobalManager& GlobalManager::getInstance() {
    static GlobalManager instance;
    return instance;
}

GlobalManager::GlobalManager() : m_initialized(false) {
}

void GlobalManager::initialize() {
    if (m_initialized) return;
    
    m_errorReporter = std::make_unique<ErrorReporter>();
    m_performanceMonitor = std::make_unique<PerformanceMonitor>();
    m_memoryManager = std::make_unique<MemoryManager>();
    m_compilerOptimizer = std::make_unique<CompilerOptimizer>();
    
    m_initialized = true;
}

void GlobalManager::cleanup() {
    if (!m_initialized) return;
    
    m_errorReporter.reset();
    m_performanceMonitor.reset();
    m_memoryManager.reset();
    m_compilerOptimizer.reset();
    
    m_initialized = false;
}

std::string GlobalManager::generateComprehensiveReport() const {
    if (!m_initialized) return "Global Manager not initialized";
    
    std::ostringstream ss;
    
    ss << "=== CHTL Comprehensive Report ===\n\n";
    ss << m_errorReporter->formatErrorReport() << "\n\n";
    ss << m_performanceMonitor->getPerformanceReport() << "\n\n";
    ss << m_memoryManager->getMemoryReport() << "\n\n";
    
    return ss.str();
}

// ========== CompilerOptimizer 简化实现 ==========

class CompilerOptimizer {
public:
    CompilerOptimizer() : m_poolSize(0) {}
    
    std::string optimizeString(const std::string& input) {
        updateOptimizationStats("string_optimizations");
        
        // 简单的字符串优化：移除多余空白
        std::string optimized = input;
        
        // 移除连续空格
        auto newEnd = std::unique(optimized.begin(), optimized.end(), 
            [](char a, char b) { return a == ' ' && b == ' '; });
        optimized.erase(newEnd, optimized.end());
        
        return optimized;
    }
    
    void precompileRegex(const std::string& name, const std::string& pattern) {
        m_regexCache[name] = std::regex(pattern);
        updateOptimizationStats("regex_precompilations");
    }
    
    bool matchPrecompiledRegex(const std::string& name, const std::string& text) const {
        auto it = m_regexCache.find(name);
        if (it != m_regexCache.end()) {
            return std::regex_search(text, it->second);
        }
        return false;
    }
    
    void* allocateFromPool(size_t size) {
        updateOptimizationStats("pool_allocations");
        
        // 简化的内存池实现
        auto buffer = std::make_unique<char[]>(size);
        void* ptr = buffer.get();
        m_memoryPool.push_back(std::move(buffer));
        m_poolSize += size;
        
        return ptr;
    }
    
    void clearMemoryPool() {
        m_memoryPool.clear();
        m_poolSize = 0;
        updateOptimizationStats("pool_clears");
    }
    
    std::string getOptimizationStats() const {
        std::ostringstream ss;
        
        ss << "=== Compiler Optimization Statistics ===\n";
        for (const auto& [category, count] : m_optimizationStats) {
            ss << category << ": " << count << "\n";
        }
        ss << "Memory Pool Size: " << m_poolSize << " bytes\n";
        
        return ss.str();
    }

private:
    std::unordered_map<std::string, std::any> m_cache;
    std::unordered_map<std::string, std::regex> m_regexCache;
    std::vector<std::unique_ptr<char[]>> m_memoryPool;
    size_t m_poolSize;
    std::unordered_map<std::string, int> m_optimizationStats;
    
    void updateOptimizationStats(const std::string& category) {
        m_optimizationStats[category]++;
    }
};

} // namespace CHTL