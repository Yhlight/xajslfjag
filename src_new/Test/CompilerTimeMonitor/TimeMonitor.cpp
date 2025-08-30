#include "TimeMonitor.h"
#include <iostream>
#include <sstream>
#include <iomanip>

namespace CHTL {
namespace Test {

CompilerTimeMonitor::CompilerTimeMonitor() {
}

void CompilerTimeMonitor::startMonitoring(const std::string& compilerName) {
    auto& stats = m_compilerStats[compilerName];
    stats.startTime = std::chrono::high_resolution_clock::now();
    stats.isRunning = true;
    stats.isAbnormal = false;
    stats.memoryUsage = detectMemoryUsage(compilerName);
}

void CompilerTimeMonitor::stopMonitoring(const std::string& compilerName) {
    auto it = m_compilerStats.find(compilerName);
    if (it != m_compilerStats.end()) {
        it->second.endTime = std::chrono::high_resolution_clock::now();
        it->second.isRunning = false;
        
        // 检测异常行为
        detectAbnormalBehavior(compilerName);
    }
}

double CompilerTimeMonitor::getCompilationTime(const std::string& compilerName) const {
    auto it = m_compilerStats.find(compilerName);
    if (it != m_compilerStats.end() && !it->second.isRunning) {
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
            it->second.endTime - it->second.startTime
        );
        return duration.count() / 1000.0; // 返回毫秒
    }
    return 0.0;
}

size_t CompilerTimeMonitor::getMemoryUsage(const std::string& compilerName) const {
    auto it = m_compilerStats.find(compilerName);
    if (it != m_compilerStats.end()) {
        return it->second.memoryUsage;
    }
    return 0;
}

bool CompilerTimeMonitor::isCompilerAbnormal(const std::string& compilerName) const {
    auto it = m_compilerStats.find(compilerName);
    if (it != m_compilerStats.end()) {
        return it->second.isAbnormal;
    }
    return false;
}

bool CompilerTimeMonitor::killAbnormalCompiler(const std::string& compilerName) {
    auto it = m_compilerStats.find(compilerName);
    if (it != m_compilerStats.end() && it->second.isAbnormal) {
        // 在实际实现中，这里应该终止编译器进程
        // 简化实现：标记为已停止
        it->second.isRunning = false;
        it->second.endTime = std::chrono::high_resolution_clock::now();
        
        std::cout << "警告: 编译器 " << compilerName << " 异常，已终止\n";
        return true;
    }
    return false;
}

std::string CompilerTimeMonitor::generateMonitorReport() const {
    std::ostringstream report;
    
    report << "编译器监控报告\n";
    report << "===============\n";
    report << "监控的编译器数量: " << m_compilerStats.size() << "\n\n";
    
    for (const auto& pair : m_compilerStats) {
        const std::string& name = pair.first;
        const CompilerStats& stats = pair.second;
        
        report << "编译器: " << name << "\n";
        report << "  状态: " << (stats.isRunning ? "运行中" : "已停止") << "\n";
        report << "  异常: " << (stats.isAbnormal ? "是" : "否") << "\n";
        
        if (!stats.isRunning) {
            double time = getCompilationTime(name);
            report << "  编译时间: " << std::fixed << std::setprecision(2) << time << " ms\n";
        }
        
        report << "  内存使用: " << stats.memoryUsage << " KB\n";
        report << "\n";
    }
    
    return report.str();
}

void CompilerTimeMonitor::reset() {
    m_compilerStats.clear();
}

size_t CompilerTimeMonitor::detectMemoryUsage(const std::string& compilerName) {
    // 简化的内存检测实现
    // 在实际实现中，这里应该查询进程的内存使用情况
    return 1024; // 返回1MB作为示例
}

void CompilerTimeMonitor::detectAbnormalBehavior(const std::string& compilerName) {
    auto it = m_compilerStats.find(compilerName);
    if (it != m_compilerStats.end()) {
        // 检查编译时间是否过长
        double time = getCompilationTime(compilerName);
        if (time > 10000.0) { // 超过10秒
            it->second.isAbnormal = true;
        }
        
        // 检查内存使用是否过高
        if (it->second.memoryUsage > 100 * 1024) { // 超过100MB
            it->second.isAbnormal = true;
        }
    }
}

CompilerMonitorGuard::CompilerMonitorGuard(CompilerTimeMonitor& monitor, const std::string& compilerName)
    : m_monitor(monitor), m_compilerName(compilerName) {
    m_monitor.startMonitoring(m_compilerName);
}

CompilerMonitorGuard::~CompilerMonitorGuard() {
    m_monitor.stopMonitoring(m_compilerName);
}

} // namespace Test
} // namespace CHTL