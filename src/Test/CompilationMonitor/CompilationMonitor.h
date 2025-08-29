#ifndef COMPILATION_MONITOR_H
#define COMPILATION_MONITOR_H

#include <chrono>
#include <memory>
#include <atomic>
#include <thread>
#include <functional>
#include <string>
#include <vector>
#include <map>
#include <mutex>
#include <condition_variable>

namespace CHTL {
namespace Test {

// 编译阶段
enum class CompilationPhase {
    SCANNING,       // 扫描阶段
    LEXING,         // 词法分析
    PARSING,        // 语法分析
    SEMANTIC,       // 语义分析
    OPTIMIZATION,   // 优化阶段
    GENERATION,     // 代码生成
    COMPLETE        // 完成
};

// 资源使用情况
struct ResourceUsage {
    size_t memoryUsed = 0;        // 内存使用（字节）
    size_t peakMemory = 0;        // 峰值内存
    double cpuUsage = 0.0;        // CPU使用率
    size_t threadCount = 0;       // 线程数
    size_t fileHandles = 0;       // 文件句柄数
};

// 编译统计
struct CompilationStats {
    std::chrono::milliseconds totalTime{0};
    std::map<CompilationPhase, std::chrono::milliseconds> phaseTimes;
    ResourceUsage resourceUsage;
    size_t linesProcessed = 0;
    size_t tokensGenerated = 0;
    size_t nodesCreated = 0;
    size_t errorsFound = 0;
    size_t warningsFound = 0;
};

// 编译监视器
class CompilationMonitor {
public:
    using TimeoutCallback = std::function<void()>;
    using ProgressCallback = std::function<void(CompilationPhase, double progress)>;
    using ResourceCallback = std::function<void(const ResourceUsage&)>;
    
    CompilationMonitor();
    ~CompilationMonitor();
    
    // 开始监视
    void start();
    void stop();
    
    // 设置超时
    void setTimeout(std::chrono::milliseconds timeout);
    void setMemoryLimit(size_t bytes);
    
    // 设置回调
    void setTimeoutCallback(TimeoutCallback callback);
    void setProgressCallback(ProgressCallback callback);
    void setResourceCallback(ResourceCallback callback);
    
    // 更新进度
    void enterPhase(CompilationPhase phase);
    void updateProgress(double progress);  // 0.0 - 1.0
    
    // 更新统计
    void updateLinesProcessed(size_t lines) { stats_.linesProcessed = lines; }
    void updateTokensGenerated(size_t tokens) { stats_.tokensGenerated = tokens; }
    void updateNodesCreated(size_t nodes) { stats_.nodesCreated = nodes; }
    void incrementErrors() { stats_.errorsFound++; }
    void incrementWarnings() { stats_.warningsFound++; }
    
    // 获取统计信息
    CompilationStats getStats() const;
    std::string getStatsReport() const;
    
    // 检查是否应该终止
    bool shouldTerminate() const { return shouldTerminate_; }
    
    // 死循环检测
    void enableDeadlockDetection(bool enable) { detectDeadlock_ = enable; }
    void checkProgress();  // 手动检查进度
    
private:
    std::atomic<bool> running_{false};
    std::atomic<bool> shouldTerminate_{false};
    std::atomic<CompilationPhase> currentPhase_{CompilationPhase::SCANNING};
    std::atomic<double> currentProgress_{0.0};
    
    std::chrono::milliseconds timeout_{300000};  // 默认5分钟超时
    size_t memoryLimit_ = 2ULL * 1024 * 1024 * 1024;  // 默认2GB限制
    
    std::chrono::steady_clock::time_point startTime_;
    std::chrono::steady_clock::time_point phaseStartTime_;
    std::chrono::steady_clock::time_point lastProgressTime_;
    
    CompilationStats stats_;
    mutable std::mutex statsMutex_;
    
    std::unique_ptr<std::thread> monitorThread_;
    std::condition_variable cv_;
    std::mutex cvMutex_;
    
    TimeoutCallback timeoutCallback_;
    ProgressCallback progressCallback_;
    ResourceCallback resourceCallback_;
    
    bool detectDeadlock_ = true;
    std::chrono::milliseconds deadlockTimeout_{30000};  // 30秒无进度视为死锁
    
    // 监视线程函数
    void monitorLoop();
    
    // 资源监视
    ResourceUsage getCurrentResourceUsage();
    size_t getCurrentMemoryUsage();
    double getCurrentCPUUsage();
    
    // 检查限制
    void checkTimeout();
    void checkMemoryLimit();
    void checkDeadlock();
    
    // 终止进程
    void terminateProcess();
};

// 编译计时器（RAII）
class CompilationTimer {
public:
    CompilationTimer(CompilationMonitor& monitor, CompilationPhase phase)
        : monitor_(monitor), phase_(phase) {
        monitor_.enterPhase(phase);
        startTime_ = std::chrono::steady_clock::now();
    }
    
    ~CompilationTimer() {
        auto duration = std::chrono::steady_clock::now() - startTime_;
        // 自动记录阶段时间
    }
    
private:
    CompilationMonitor& monitor_;
    CompilationPhase phase_;
    std::chrono::steady_clock::time_point startTime_;
};

// 进度报告器
class ProgressReporter {
public:
    ProgressReporter(CompilationMonitor& monitor, size_t totalItems)
        : monitor_(monitor), totalItems_(totalItems), processedItems_(0) {}
    
    void update(size_t processedItems) {
        processedItems_ = processedItems;
        double progress = totalItems_ > 0 ? 
            static_cast<double>(processedItems_) / totalItems_ : 0.0;
        monitor_.updateProgress(progress);
    }
    
    void increment(size_t count = 1) {
        update(processedItems_ + count);
    }
    
private:
    CompilationMonitor& monitor_;
    size_t totalItems_;
    size_t processedItems_;
};

// 性能分析器
class PerformanceProfiler {
public:
    struct ProfileData {
        std::string name;
        std::chrono::microseconds totalTime{0};
        size_t callCount = 0;
        std::chrono::microseconds minTime{std::chrono::microseconds::max()};
        std::chrono::microseconds maxTime{0};
    };
    
    void startTiming(const std::string& name);
    void endTiming(const std::string& name);
    
    std::vector<ProfileData> getProfileData() const;
    std::string getReport() const;
    
    void reset();
    
private:
    struct TimingInfo {
        std::chrono::steady_clock::time_point startTime;
        ProfileData data;
    };
    
    mutable std::mutex mutex_;
    std::map<std::string, TimingInfo> timings_;
    std::map<std::string, ProfileData> completed_;
};

// 性能计时器（RAII）
class ScopedTimer {
public:
    ScopedTimer(PerformanceProfiler& profiler, const std::string& name)
        : profiler_(profiler), name_(name) {
        profiler_.startTiming(name);
    }
    
    ~ScopedTimer() {
        profiler_.endTiming(name_);
    }
    
private:
    PerformanceProfiler& profiler_;
    std::string name_;
};

#define PROFILE_SCOPE(profiler, name) \
    CHTL::Test::ScopedTimer _timer_##__LINE__(profiler, name)

// 内存跟踪器
class MemoryTracker {
public:
    struct AllocationInfo {
        size_t size;
        std::string file;
        int line;
        std::chrono::steady_clock::time_point timestamp;
    };
    
    void recordAllocation(void* ptr, size_t size, const char* file, int line);
    void recordDeallocation(void* ptr);
    
    size_t getCurrentUsage() const { return currentUsage_; }
    size_t getPeakUsage() const { return peakUsage_; }
    
    std::vector<AllocationInfo> getActiveAllocations() const;
    std::string getReport() const;
    
    void reset();
    
private:
    mutable std::mutex mutex_;
    std::map<void*, AllocationInfo> allocations_;
    std::atomic<size_t> currentUsage_{0};
    std::atomic<size_t> peakUsage_{0};
};

// 监视器管理器（单例）
class MonitorManager {
public:
    static MonitorManager& getInstance() {
        static MonitorManager instance;
        return instance;
    }
    
    CompilationMonitor& getCompilationMonitor() { return compilationMonitor_; }
    PerformanceProfiler& getPerformanceProfiler() { return performanceProfiler_; }
    MemoryTracker& getMemoryTracker() { return memoryTracker_; }
    
    // 生成完整报告
    std::string generateFullReport() const;
    
    // 保存报告到文件
    void saveReport(const std::string& filename) const;
    
private:
    MonitorManager() = default;
    
    CompilationMonitor compilationMonitor_;
    PerformanceProfiler performanceProfiler_;
    MemoryTracker memoryTracker_;
};

} // namespace Test
} // namespace CHTL

#endif // COMPILATION_MONITOR_H