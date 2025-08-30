#pragma once

#include "../../Util/Common.h"
#include <chrono>
#include <memory>
#include <vector>
#include <map>
#include <functional>
#include <thread>
#include <atomic>
#include <mutex>

namespace CHTL::Test {

// 编译阶段枚举
enum class CompilePhase {
    LEXICAL_ANALYSIS,
    SYNTAX_PARSING,
    AST_BUILDING,
    SEMANTIC_ANALYSIS,
    CODE_GENERATION,
    OPTIMIZATION,
    OUTPUT_WRITING,
    TOTAL_COMPILATION
};

// 性能度量结构
struct PerformanceMetrics {
    std::chrono::high_resolution_clock::time_point startTime;
    std::chrono::high_resolution_clock::time_point endTime;
    double elapsedTimeMs;
    size_t memoryUsedBytes;
    size_t peakMemoryBytes;
    size_t cpuUsagePercent;
    size_t ioOperations;
    
    PerformanceMetrics();
    void start();
    void end();
    double getDurationMs() const;
    String toString() const;
};

// 编译时间监控器
class CompileTimer {
public:
    CompileTimer();
    ~CompileTimer();
    
    // 基本计时功能
    void startTimer(const String& timerName);
    void endTimer(const String& timerName);
    void resetTimer(const String& timerName);
    
    // 编译阶段计时
    void startPhase(CompilePhase phase);
    void endPhase(CompilePhase phase);
    void resetPhase(CompilePhase phase);
    
    // 自动计时器（RAII）
    class ScopedTimer {
    public:
        ScopedTimer(CompileTimer& timer, const String& name);
        ScopedTimer(CompileTimer& timer, CompilePhase phase);
        ~ScopedTimer();
        
    private:
        CompileTimer& timer_;
        String timerName_;
        CompilePhase phase_;
        bool isPhaseTimer_;
    };
    
    std::unique_ptr<ScopedTimer> createScopedTimer(const String& name);
    std::unique_ptr<ScopedTimer> createScopedPhaseTimer(CompilePhase phase);
    
    // 获取计时结果
    double getElapsedTime(const String& timerName) const;
    double getPhaseTime(CompilePhase phase) const;
    double getTotalTime() const;
    
    // 统计信息
    std::map<String, double> getAllTimers() const;
    std::map<CompilePhase, double> getAllPhases() const;
    PerformanceMetrics getOverallMetrics() const;
    
    // 性能分析
    String getTimingReport() const;
    String getPhaseReport() const;
    String getDetailedReport() const;
    
    // 比较分析
    void saveBaseline(const String& name);
    String compareWithBaseline(const String& baselineName) const;
    void clearBaselines();
    
    // 导出功能
    void exportToCSV(const String& filePath) const;
    void exportToJSON(const String& filePath) const;
    void exportToXML(const String& filePath) const;
    
    // 配置选项
    void setVerbose(bool verbose);
    void setAutoReport(bool autoReport);
    void setReportInterval(int intervalSeconds);
    
    // 重置功能
    void resetAllTimers();
    void resetAllPhases();
    void reset();
    
    static String getPhaseDisplayName(CompilePhase phase);

private:
    struct TimerData {
        std::chrono::high_resolution_clock::time_point startTime;
        std::chrono::high_resolution_clock::time_point endTime;
        double accumulatedTime;
        bool isRunning;
        int callCount;
        
        TimerData();
    };
    
    mutable std::mutex mutex_;
    std::map<String, TimerData> timers_;
    std::map<CompilePhase, TimerData> phaseTimers_;
    std::map<String, std::map<String, double>> baselines_;
    std::map<String, std::map<CompilePhase, double>> phaseBaselines_;
    
    std::chrono::high_resolution_clock::time_point globalStartTime_;
    bool verbose_;
    bool autoReport_;
    int reportInterval_;
    
    void updateTimer(TimerData& timer, bool start);
    String formatTime(double timeMs) const;
    String generateTimingTable(const std::map<String, double>& timings) const;
    String generatePhaseTable(const std::map<CompilePhase, double>& phases) const;
};

// 内存监控器
class MemoryMonitor {
public:
    MemoryMonitor();
    ~MemoryMonitor();
    
    // 内存监控控制
    void startMonitoring();
    void stopMonitoring();
    void resetMonitoring();
    
    // 内存快照
    struct MemorySnapshot {
        size_t virtualMemory;
        size_t physicalMemory;
        size_t heapMemory;
        size_t stackMemory;
        std::chrono::high_resolution_clock::time_point timestamp;
        String description;
        
        MemorySnapshot();
        String toString() const;
    };
    
    void takeSnapshot(const String& description = "");
    std::vector<MemorySnapshot> getSnapshots() const;
    MemorySnapshot getCurrentMemoryUsage() const;
    size_t getPeakMemoryUsage() const;
    
    // 内存统计
    String getMemoryReport() const;
    void exportMemoryData(const String& filePath) const;
    
    // 内存泄漏检测
    void enableLeakDetection(bool enable = true);
    bool hasMemoryLeaks() const;
    String getLeakReport() const;
    
    // 内存限制检查
    void setMemoryLimit(size_t limitBytes);
    bool isMemoryLimitExceeded() const;
    void setMemoryWarningThreshold(size_t thresholdBytes);
    
    // 回调设置
    using MemoryCallback = std::function<void(const MemorySnapshot&)>;
    void setMemoryWarningCallback(MemoryCallback callback);
    void setMemoryLimitCallback(MemoryCallback callback);

private:
    mutable std::mutex mutex_;
    std::vector<MemorySnapshot> snapshots_;
    size_t peakMemoryUsage_;
    bool isMonitoring_;
    bool leakDetectionEnabled_;
    size_t memoryLimit_;
    size_t warningThreshold_;
    
    MemoryCallback warningCallback_;
    MemoryCallback limitCallback_;
    
    std::thread monitoringThread_;
    std::atomic<bool> shouldStopMonitoring_;
    
    void monitoringLoop();
    size_t getCurrentVirtualMemory() const;
    size_t getCurrentPhysicalMemory() const;
    size_t getCurrentHeapMemory() const;
    void checkMemoryLimits(const MemorySnapshot& snapshot);
};

// 无限循环检测器
class InfiniteLoopDetector {
public:
    InfiniteLoopDetector();
    ~InfiniteLoopDetector();
    
    // 检测配置
    void setTimeout(int timeoutSeconds);
    void setMaxIterations(size_t maxIterations);
    void setCheckInterval(int intervalMs);
    
    // 循环检测点
    void registerLoopPoint(const String& pointName);
    void hitLoopPoint(const String& pointName);
    void unregisterLoopPoint(const String& pointName);
    
    // 自动检测器（RAII）
    class LoopGuard {
    public:
        LoopGuard(InfiniteLoopDetector& detector, const String& pointName);
        ~LoopGuard();
        
        void hit(); // 手动触发检测点
        
    private:
        InfiniteLoopDetector& detector_;
        String pointName_;
    };
    
    std::unique_ptr<LoopGuard> createLoopGuard(const String& pointName);
    
    // 检测控制
    void startDetection();
    void stopDetection();
    void resetDetection();
    
    // 检测状态
    bool isInfiniteLoopDetected() const;
    String getLoopDetectionReport() const;
    StringVector getSuspiciousLoopPoints() const;
    
    // 回调设置
    using LoopDetectedCallback = std::function<void(const String& pointName, size_t iterations)>;
    void setLoopDetectedCallback(LoopDetectedCallback callback);
    
    // 紧急停止
    void triggerEmergencyStop();
    bool isEmergencyStopTriggered() const;

private:
    struct LoopPoint {
        String name;
        size_t hitCount;
        std::chrono::high_resolution_clock::time_point firstHit;
        std::chrono::high_resolution_clock::time_point lastHit;
        bool isSuspicious;
        
        LoopPoint() : hitCount(0), isSuspicious(false) {}
        LoopPoint(const String& n);
    };
    
    mutable std::mutex mutex_;
    std::map<String, LoopPoint> loopPoints_;
    
    int timeoutSeconds_;
    size_t maxIterations_;
    int checkInterval_;
    bool isDetecting_;
    std::atomic<bool> emergencyStop_;
    
    LoopDetectedCallback loopCallback_;
    
    std::thread detectionThread_;
    std::atomic<bool> shouldStopDetection_;
    
    void detectionLoop();
    void checkLoopPoints();
    bool isLoopSuspicious(const LoopPoint& point) const;
};

// 综合性能监控器
class PerformanceMonitor {
public:
    PerformanceMonitor();
    ~PerformanceMonitor();
    
    // 组件访问
    CompileTimer& getTimer();
    MemoryMonitor& getMemoryMonitor();
    InfiniteLoopDetector& getLoopDetector();
    
    // 全局监控控制
    void startMonitoring();
    void stopMonitoring();
    void resetMonitoring();
    
    // 综合报告
    String getComprehensiveReport() const;
    String getPerformanceSummary() const;
    
    // 导出功能
    void exportFullReport(const String& filePath) const;
    void exportPerformanceData(const String& directory) const;
    
    // 性能告警
    struct PerformanceAlert {
        enum Type {
            SLOW_COMPILATION,
            HIGH_MEMORY_USAGE,
            POTENTIAL_INFINITE_LOOP,
            PERFORMANCE_DEGRADATION
        };
        
        Type type;
        String message;
        std::chrono::high_resolution_clock::time_point timestamp;
        String details;
        
        PerformanceAlert(Type t, const String& msg, const String& det = "");
        String toString() const;
    };
    
    std::vector<PerformanceAlert> getAlerts() const;
    void clearAlerts();
    
    using AlertCallback = std::function<void(const PerformanceAlert&)>;
    void setAlertCallback(AlertCallback callback);
    
    // 性能建议
    StringVector getPerformanceRecommendations() const;
    
private:
    std::unique_ptr<CompileTimer> timer_;
    std::unique_ptr<MemoryMonitor> memoryMonitor_;
    std::unique_ptr<InfiniteLoopDetector> loopDetector_;
    
    mutable std::mutex alertMutex_;
    std::vector<PerformanceAlert> alerts_;
    AlertCallback alertCallback_;
    
    void addAlert(const PerformanceAlert& alert);
    void analyzePerformance();
};

// 便捷宏
#define CHTL_TIMER_SCOPE(name) \
    auto _timer_guard = CHTL::Test::PerformanceMonitor::getInstance().getTimer().createScopedTimer(name)

#define CHTL_PHASE_TIMER(phase) \
    auto _phase_guard = CHTL::Test::PerformanceMonitor::getInstance().getTimer().createScopedPhaseTimer(phase)

#define CHTL_LOOP_GUARD(name) \
    auto _loop_guard = CHTL::Test::PerformanceMonitor::getInstance().getLoopDetector().createLoopGuard(name)

#define CHTL_MEMORY_SNAPSHOT(desc) \
    CHTL::Test::PerformanceMonitor::getInstance().getMemoryMonitor().takeSnapshot(desc)

} // namespace CHTL::Test