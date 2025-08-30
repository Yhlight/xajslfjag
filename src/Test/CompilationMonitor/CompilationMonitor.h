#pragma once
#include <chrono>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <thread>
#include <atomic>
#include <memory>
#include <mutex>

namespace CHTL {
namespace Test {

/**
 * 编译阶段枚举
 */
enum class CompilationPhase {
    LEXICAL_ANALYSIS,       // 词法分析
    SYNTAX_ANALYSIS,        // 语法分析
    SEMANTIC_ANALYSIS,      // 语义分析
    AST_BUILDING,          // AST构建
    OPTIMIZATION,          // 优化
    CODE_GENERATION,       // 代码生成
    LINKING,               // 链接
    UNKNOWN                // 未知阶段
};

/**
 * 资源使用情况
 */
struct ResourceUsage {
    size_t memoryUsageMB;       // 内存使用量（MB）
    double cpuUsagePercent;     // CPU使用率（%）
    size_t heapSizeMB;          // 堆大小（MB）
    size_t stackSizeMB;         // 栈大小（MB）
    size_t fileHandleCount;     // 文件句柄数量
    
    ResourceUsage() 
        : memoryUsageMB(0), cpuUsagePercent(0.0), heapSizeMB(0), 
          stackSizeMB(0), fileHandleCount(0) {}
};

/**
 * 编译统计信息
 */
struct CompilationStats {
    std::chrono::steady_clock::time_point startTime;   // 开始时间
    std::chrono::steady_clock::time_point endTime;     // 结束时间
    CompilationPhase currentPhase;                      // 当前阶段
    ResourceUsage maxResourceUsage;                     // 最大资源使用量
    ResourceUsage currentResourceUsage;                 // 当前资源使用量
    size_t totalTokens;                                // 总Token数量
    size_t totalNodes;                                 // 总节点数量
    size_t errorCount;                                 // 错误数量
    size_t warningCount;                               // 警告数量
    bool isComplete;                                   // 是否完成
    bool wasTerminated;                                // 是否被终止
    
    CompilationStats() 
        : currentPhase(CompilationPhase::UNKNOWN), totalTokens(0), 
          totalNodes(0), errorCount(0), warningCount(0), 
          isComplete(false), wasTerminated(false) {}
};

/**
 * 编译监视器
 * 监控编译时间、内存使用、死循环检测
 */
class CompilationMonitor {
public:
    /**
     * 监控配置
     */
    struct MonitorConfig {
        std::chrono::milliseconds maxCompilationTime;   // 最大编译时间（5分钟）
        size_t maxMemoryUsageMB;                         // 最大内存使用（2GB）
        double maxCpuUsagePercent;                       // 最大CPU使用率
        std::chrono::milliseconds deadlockCheckInterval; // 死锁检查间隔
        std::chrono::milliseconds resourceCheckInterval;  // 资源检查间隔
        bool enableDetailedLogging;                      // 启用详细日志
        bool enableAutoTermination;                      // 启用自动终止
        std::string logFilePath;                         // 日志文件路径
        
        MonitorConfig() : maxCompilationTime(300000), maxMemoryUsageMB(2048), 
                         maxCpuUsagePercent(95.0), deadlockCheckInterval(1000),
                         resourceCheckInterval(500), enableDetailedLogging(true),
                         enableAutoTermination(true), logFilePath("compilation_monitor.log") {}
    };
    
    /**
     * 构造函数
     */
    CompilationMonitor(const MonitorConfig& config = MonitorConfig());
    
    /**
     * 析构函数
     */
    ~CompilationMonitor();
    
    // 禁用拷贝和移动
    CompilationMonitor(const CompilationMonitor&) = delete;
    CompilationMonitor& operator=(const CompilationMonitor&) = delete;
    
    // ========== 监控控制 ==========
    
    /**
     * 开始监控
     */
    void startMonitoring(const std::string& projectName = "");
    
    /**
     * 停止监控
     */
    void stopMonitoring();
    
    /**
     * 暂停监控
     */
    void pauseMonitoring();
    
    /**
     * 恢复监控
     */
    void resumeMonitoring();
    
    /**
     * 检查是否正在监控
     */
    bool isMonitoring() const;
    
    /**
     * 强制终止编译
     */
    void terminateCompilation();
    
    // ========== 阶段管理 ==========
    
    /**
     * 设置当前编译阶段
     */
    void setCurrentPhase(CompilationPhase phase);
    
    /**
     * 获取当前编译阶段
     */
    CompilationPhase getCurrentPhase() const;
    
    /**
     * 记录阶段开始
     */
    void recordPhaseStart(CompilationPhase phase);
    
    /**
     * 记录阶段结束
     */
    void recordPhaseEnd(CompilationPhase phase);
    
    // ========== 进度跟踪 ==========
    
    /**
     * 更新处理进度
     */
    void updateProgress(size_t processedItems, size_t totalItems);
    
    /**
     * 增加Token计数
     */
    void addTokenCount(size_t count);
    
    /**
     * 增加节点计数
     */
    void addNodeCount(size_t count);
    
    /**
     * 增加错误计数
     */
    void addErrorCount(size_t count = 1);
    
    /**
     * 增加警告计数
     */
    void addWarningCount(size_t count = 1);
    
    // ========== 资源监控 ==========
    
    /**
     * 获取当前资源使用情况
     */
    ResourceUsage getCurrentResourceUsage() const;
    
    /**
     * 获取最大资源使用情况
     */
    ResourceUsage getMaxResourceUsage() const;
    
    /**
     * 检查资源限制
     */
    bool checkResourceLimits() const;
    
    // ========== 死循环检测 ==========
    
    /**
     * 记录心跳（用于死循环检测）
     */
    void heartbeat();
    
    /**
     * 检查是否可能出现死循环
     */
    bool checkForDeadlock() const;
    
    /**
     * 设置死循环检测回调
     */
    void setDeadlockCallback(std::function<void()> callback);
    
    // ========== 统计信息 ==========
    
    /**
     * 获取编译统计信息
     */
    CompilationStats getStatistics() const;
    
    /**
     * 获取编译持续时间
     */
    std::chrono::milliseconds getElapsedTime() const;
    
    /**
     * 获取阶段持续时间
     */
    std::chrono::milliseconds getPhaseElapsedTime(CompilationPhase phase) const;
    
    /**
     * 生成统计报告
     */
    std::string generateReport() const;
    
    /**
     * 打印实时统计
     */
    void printRealtimeStats() const;
    
    // ========== 回调和事件 ==========
    
    /**
     * 设置资源超限回调
     */
    void setResourceLimitCallback(std::function<void(const ResourceUsage&)> callback);
    
    /**
     * 设置超时回调
     */
    void setTimeoutCallback(std::function<void()> callback);
    
    /**
     * 设置阶段变更回调
     */
    void setPhaseChangeCallback(std::function<void(CompilationPhase, CompilationPhase)> callback);
    
    /**
     * 设置进度更新回调
     */
    void setProgressCallback(std::function<void(double)> callback);

private:
    MonitorConfig m_config;                                 // 监控配置
    std::atomic<bool> m_isMonitoring;                      // 是否正在监控
    std::atomic<bool> m_isPaused;                          // 是否暂停
    std::atomic<bool> m_shouldTerminate;                   // 是否应该终止
    
    mutable std::mutex m_statsMutex;                       // 统计信息互斥锁
    CompilationStats m_stats;                              // 编译统计信息
    
    std::unordered_map<CompilationPhase, std::chrono::steady_clock::time_point> m_phaseStartTimes; // 阶段开始时间
    std::unordered_map<CompilationPhase, std::chrono::milliseconds> m_phaseDurations; // 阶段持续时间
    
    std::atomic<std::chrono::steady_clock::time_point> m_lastHeartbeat; // 上次心跳时间
    std::atomic<size_t> m_processedItems;                  // 已处理项目数
    std::atomic<size_t> m_totalItems;                      // 总项目数
    
    // 监控线程
    std::unique_ptr<std::thread> m_monitorThread;
    std::unique_ptr<std::thread> m_deadlockThread;
    
    // 回调函数
    std::function<void()> m_deadlockCallback;
    std::function<void(const ResourceUsage&)> m_resourceLimitCallback;
    std::function<void()> m_timeoutCallback;
    std::function<void(CompilationPhase, CompilationPhase)> m_phaseChangeCallback;
    std::function<void(double)> m_progressCallback;
    
    /**
     * 监控线程主函数
     */
    void monitorThreadMain();
    
    /**
     * 死锁检测线程主函数
     */
    void deadlockThreadMain();
    
    /**
     * 更新资源使用情况
     */
    void updateResourceUsage();
    
    /**
     * 检查超时
     */
    bool checkTimeout() const;
    
    /**
     * 记录日志
     */
    void logMessage(const std::string& message) const;
    
    /**
     * 获取系统资源使用情况
     */
    ResourceUsage getSystemResourceUsage() const;
    
    /**
     * 获取阶段名称
     */
    std::string getPhaseNameString(CompilationPhase phase) const;
    
    /**
     * 格式化时间
     */
    std::string formatDuration(std::chrono::milliseconds duration) const;
    
    /**
     * 格式化内存大小
     */
    std::string formatMemorySize(size_t sizeInMB) const;
};

/**
 * 编译监视器的RAII包装器
 */
class ScopedCompilationMonitor {
public:
    /**
     * 构造函数 - 自动开始监控
     */
    ScopedCompilationMonitor(CompilationMonitor& monitor, const std::string& projectName = "");
    
    /**
     * 析构函数 - 自动停止监控
     */
    ~ScopedCompilationMonitor();
    
    // 禁用拷贝和移动
    ScopedCompilationMonitor(const ScopedCompilationMonitor&) = delete;
    ScopedCompilationMonitor& operator=(const ScopedCompilationMonitor&) = delete;
    
    /**
     * 获取监控器引用
     */
    CompilationMonitor& getMonitor();

private:
    CompilationMonitor& m_monitor;
};

/**
 * 编译监视器工厂
 */
class CompilationMonitorFactory {
public:
    /**
     * 创建标准监视器
     */
    static std::unique_ptr<CompilationMonitor> createStandardMonitor();
    
    /**
     * 创建调试监视器
     */
    static std::unique_ptr<CompilationMonitor> createDebugMonitor();
    
    /**
     * 创建生产环境监视器
     */
    static std::unique_ptr<CompilationMonitor> createProductionMonitor();
    
    /**
     * 创建高性能监视器
     */
    static std::unique_ptr<CompilationMonitor> createHighPerformanceMonitor();
    
    /**
     * 创建自定义监视器
     */
    static std::unique_ptr<CompilationMonitor> createCustomMonitor(const CompilationMonitor::MonitorConfig& config);
};

/**
 * 全局编译监视器管理器
 */
class GlobalMonitorManager {
public:
    /**
     * 获取全局实例
     */
    static GlobalMonitorManager& getInstance();
    
    /**
     * 设置全局监视器
     */
    void setGlobalMonitor(std::unique_ptr<CompilationMonitor> monitor);
    
    /**
     * 获取全局监视器
     */
    CompilationMonitor* getGlobalMonitor();
    
    /**
     * 为CHTL项目绑定监视器
     */
    void bindToCHTLProject();

private:
    std::unique_ptr<CompilationMonitor> m_globalMonitor;
    std::mutex m_mutex;
    
    GlobalMonitorManager() = default;
};

} // namespace Test
} // namespace CHTL

/**
 * 便捷宏定义
 */
#define CHTL_MONITOR_START(name) \
    do { \
        auto* monitor = CHTL::Test::GlobalMonitorManager::getInstance().getGlobalMonitor(); \
        if (monitor) monitor->startMonitoring(name); \
    } while(0)

#define CHTL_MONITOR_PHASE(phase) \
    do { \
        auto* monitor = CHTL::Test::GlobalMonitorManager::getInstance().getGlobalMonitor(); \
        if (monitor) monitor->setCurrentPhase(phase); \
    } while(0)

#define CHTL_MONITOR_HEARTBEAT() \
    do { \
        auto* monitor = CHTL::Test::GlobalMonitorManager::getInstance().getGlobalMonitor(); \
        if (monitor) monitor->heartbeat(); \
    } while(0)

#define CHTL_MONITOR_PROGRESS(processed, total) \
    do { \
        auto* monitor = CHTL::Test::GlobalMonitorManager::getInstance().getGlobalMonitor(); \
        if (monitor) monitor->updateProgress(processed, total); \
    } while(0)

#define CHTL_MONITOR_STOP() \
    do { \
        auto* monitor = CHTL::Test::GlobalMonitorManager::getInstance().getGlobalMonitor(); \
        if (monitor) monitor->stopMonitoring(); \
    } while(0)