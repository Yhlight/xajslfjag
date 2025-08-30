#pragma once
#include <string>
#include <chrono>
#include <unordered_map>
#include <vector>

namespace CHTL {
namespace Test {

/**
 * 编译时间监控器
 * 提供编译时间监控，编译器内存监视，确保编译器异常时杀死编译器
 */
class CompilerTimeMonitor {
public:
    CompilerTimeMonitor();
    ~CompilerTimeMonitor() = default;
    
    /**
     * 开始监控编译器
     */
    void startMonitoring(const std::string& compilerName);
    
    /**
     * 停止监控编译器
     */
    void stopMonitoring(const std::string& compilerName);
    
    /**
     * 获取编译时间
     */
    double getCompilationTime(const std::string& compilerName) const;
    
    /**
     * 获取内存使用情况
     */
    size_t getMemoryUsage(const std::string& compilerName) const;
    
    /**
     * 检查编译器是否异常
     */
    bool isCompilerAbnormal(const std::string& compilerName) const;
    
    /**
     * 杀死异常编译器
     */
    bool killAbnormalCompiler(const std::string& compilerName);
    
    /**
     * 生成监控报告
     */
    std::string generateMonitorReport() const;
    
    /**
     * 重置监控数据
     */
    void reset();

private:
    struct CompilerStats {
        std::chrono::high_resolution_clock::time_point startTime;
        std::chrono::high_resolution_clock::time_point endTime;
        size_t memoryUsage;
        bool isRunning;
        bool isAbnormal;
        
        CompilerStats() : memoryUsage(0), isRunning(false), isAbnormal(false) {}
    };
    
    std::unordered_map<std::string, CompilerStats> m_compilerStats;
    
    /**
     * 检测内存使用
     */
    size_t detectMemoryUsage(const std::string& compilerName);
    
    /**
     * 检测编译器异常
     */
    void detectAbnormalBehavior(const std::string& compilerName);
};

/**
 * RAII编译器监控器
 * 自动开始和停止监控
 */
class CompilerMonitorGuard {
public:
    CompilerMonitorGuard(CompilerTimeMonitor& monitor, const std::string& compilerName);
    ~CompilerMonitorGuard();
    
    // 禁用拷贝和移动
    CompilerMonitorGuard(const CompilerMonitorGuard&) = delete;
    CompilerMonitorGuard& operator=(const CompilerMonitorGuard&) = delete;

private:
    CompilerTimeMonitor& m_monitor;
    std::string m_compilerName;
};

} // namespace Test
} // namespace CHTL