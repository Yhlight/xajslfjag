#include "CompilationMonitor.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cstdlib>
#include <signal.h>

#ifdef _WIN32
#include <windows.h>
#include <psapi.h>
#else
#include <sys/resource.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#endif

namespace CHTL {
namespace Test {

// ========== CompilationMonitor 实现 ==========

CompilationMonitor::CompilationMonitor(const MonitorConfig& config)
    : m_config(config)
    , m_isMonitoring(false)
    , m_isPaused(false)
    , m_shouldTerminate(false)
    , m_lastHeartbeat(std::chrono::steady_clock::now())
    , m_processedItems(0)
    , m_totalItems(0) {
}

CompilationMonitor::~CompilationMonitor() {
    stopMonitoring();
}

void CompilationMonitor::startMonitoring(const std::string& projectName) {
    if (m_isMonitoring.load()) {
        return; // 已经在监控中
    }
    
    std::lock_guard<std::mutex> lock(m_statsMutex);
    
    // 初始化统计信息
    m_stats = CompilationStats();
    m_stats.startTime = std::chrono::steady_clock::now();
    m_stats.currentPhase = CompilationPhase::UNKNOWN;
    
    // 重置状态
    m_isMonitoring = true;
    m_isPaused = false;
    m_shouldTerminate = false;
    m_lastHeartbeat = std::chrono::steady_clock::now();
    m_processedItems = 0;
    m_totalItems = 0;
    
    // 启动监控线程
    m_monitorThread = std::make_unique<std::thread>(&CompilationMonitor::monitorThreadMain, this);
    m_deadlockThread = std::make_unique<std::thread>(&CompilationMonitor::deadlockThreadMain, this);
    
    logMessage("Compilation monitoring started for project: " + projectName);
}

void CompilationMonitor::stopMonitoring() {
    if (!m_isMonitoring.load()) {
        return; // 没有在监控
    }
    
    m_isMonitoring = false;
    
    // 等待线程结束
    if (m_monitorThread && m_monitorThread->joinable()) {
        m_monitorThread->join();
    }
    if (m_deadlockThread && m_deadlockThread->joinable()) {
        m_deadlockThread->join();
    }
    
    std::lock_guard<std::mutex> lock(m_statsMutex);
    m_stats.endTime = std::chrono::steady_clock::now();
    m_stats.isComplete = true;
    
    logMessage("Compilation monitoring stopped");
    
    // 生成最终报告
    if (m_config.enableDetailedLogging) {
        logMessage("Final Report:\n" + generateReport());
    }
}

void CompilationMonitor::pauseMonitoring() {
    m_isPaused = true;
    logMessage("Compilation monitoring paused");
}

void CompilationMonitor::resumeMonitoring() {
    m_isPaused = false;
    logMessage("Compilation monitoring resumed");
}

bool CompilationMonitor::isMonitoring() const {
    return m_isMonitoring.load();
}

void CompilationMonitor::terminateCompilation() {
    m_shouldTerminate = true;
    m_stats.wasTerminated = true;
    
    logMessage("Compilation termination requested");
    
    if (m_config.enableAutoTermination) {
        // 发送终止信号
        raise(SIGTERM);
    }
}

void CompilationMonitor::setCurrentPhase(CompilationPhase phase) {
    std::lock_guard<std::mutex> lock(m_statsMutex);
    
    CompilationPhase oldPhase = m_stats.currentPhase;
    
    // 记录上一个阶段的结束时间
    if (oldPhase != CompilationPhase::UNKNOWN) {
        recordPhaseEnd(oldPhase);
    }
    
    // 开始新阶段
    m_stats.currentPhase = phase;
    recordPhaseStart(phase);
    
    // 调用回调
    if (m_phaseChangeCallback) {
        m_phaseChangeCallback(oldPhase, phase);
    }
    
    logMessage("Phase changed from " + getPhaseNameString(oldPhase) + " to " + getPhaseNameString(phase));
}

CompilationPhase CompilationMonitor::getCurrentPhase() const {
    std::lock_guard<std::mutex> lock(m_statsMutex);
    return m_stats.currentPhase;
}

void CompilationMonitor::recordPhaseStart(CompilationPhase phase) {
    m_phaseStartTimes[phase] = std::chrono::steady_clock::now();
}

void CompilationMonitor::recordPhaseEnd(CompilationPhase phase) {
    auto it = m_phaseStartTimes.find(phase);
    if (it != m_phaseStartTimes.end()) {
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now() - it->second);
        m_phaseDurations[phase] = duration;
    }
}

void CompilationMonitor::updateProgress(size_t processedItems, size_t totalItems) {
    m_processedItems = processedItems;
    m_totalItems = totalItems;
    
    if (m_progressCallback && totalItems > 0) {
        double progress = static_cast<double>(processedItems) / totalItems;
        m_progressCallback(progress);
    }
}

void CompilationMonitor::addTokenCount(size_t count) {
    std::lock_guard<std::mutex> lock(m_statsMutex);
    m_stats.totalTokens += count;
}

void CompilationMonitor::addNodeCount(size_t count) {
    std::lock_guard<std::mutex> lock(m_statsMutex);
    m_stats.totalNodes += count;
}

void CompilationMonitor::addErrorCount(size_t count) {
    std::lock_guard<std::mutex> lock(m_statsMutex);
    m_stats.errorCount += count;
}

void CompilationMonitor::addWarningCount(size_t count) {
    std::lock_guard<std::mutex> lock(m_statsMutex);
    m_stats.warningCount += count;
}

ResourceUsage CompilationMonitor::getCurrentResourceUsage() const {
    return getSystemResourceUsage();
}

ResourceUsage CompilationMonitor::getMaxResourceUsage() const {
    std::lock_guard<std::mutex> lock(m_statsMutex);
    return m_stats.maxResourceUsage;
}

bool CompilationMonitor::checkResourceLimits() const {
    ResourceUsage current = getCurrentResourceUsage();
    
    if (current.memoryUsageMB > m_config.maxMemoryUsageMB) {
        return false;
    }
    
    if (current.cpuUsagePercent > m_config.maxCpuUsagePercent) {
        return false;
    }
    
    return true;
}

void CompilationMonitor::heartbeat() {
    m_lastHeartbeat = std::chrono::steady_clock::now();
}

bool CompilationMonitor::checkForDeadlock() const {
    auto now = std::chrono::steady_clock::now();
    auto lastHeartbeat = m_lastHeartbeat.load();
    auto timeSinceHeartbeat = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastHeartbeat);
    
    return timeSinceHeartbeat > (m_config.deadlockCheckInterval * 10); // 10倍检查间隔
}

void CompilationMonitor::setDeadlockCallback(std::function<void()> callback) {
    m_deadlockCallback = callback;
}

CompilationStats CompilationMonitor::getStatistics() const {
    std::lock_guard<std::mutex> lock(m_statsMutex);
    return m_stats;
}

std::chrono::milliseconds CompilationMonitor::getElapsedTime() const {
    std::lock_guard<std::mutex> lock(m_statsMutex);
    auto endTime = m_stats.isComplete ? m_stats.endTime : std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(endTime - m_stats.startTime);
}

std::chrono::milliseconds CompilationMonitor::getPhaseElapsedTime(CompilationPhase phase) const {
    auto it = m_phaseDurations.find(phase);
    if (it != m_phaseDurations.end()) {
        return it->second;
    }
    return std::chrono::milliseconds(0);
}

std::string CompilationMonitor::generateReport() const {
    std::stringstream ss;
    
    ss << "=== Compilation Monitor Report ===\n";
    ss << "Total compilation time: " << formatDuration(getElapsedTime()) << "\n";
    
    {
        std::lock_guard<std::mutex> lock(m_statsMutex);
        ss << "Current phase: " << getPhaseNameString(m_stats.currentPhase) << "\n";
        ss << "Total tokens: " << m_stats.totalTokens << "\n";
        ss << "Total nodes: " << m_stats.totalNodes << "\n";
        ss << "Errors: " << m_stats.errorCount << "\n";
        ss << "Warnings: " << m_stats.warningCount << "\n";
        ss << "Max memory usage: " << formatMemorySize(m_stats.maxResourceUsage.memoryUsageMB) << "\n";
        ss << "Was terminated: " << (m_stats.wasTerminated ? "Yes" : "No") << "\n";
    }
    
    if (m_totalItems > 0) {
        double progress = static_cast<double>(m_processedItems.load()) / m_totalItems.load();
        ss << "Progress: " << std::fixed << std::setprecision(1) << (progress * 100) << "%\n";
    }
    
    ss << "\nPhase timings:\n";
    for (const auto& pair : m_phaseDurations) {
        ss << "  " << getPhaseNameString(pair.first) << ": " << formatDuration(pair.second) << "\n";
    }
    
    return ss.str();
}

void CompilationMonitor::printRealtimeStats() const {
    ResourceUsage current = getCurrentResourceUsage();
    auto elapsed = getElapsedTime();
    
    std::cout << "\r[Monitor] " 
              << formatDuration(elapsed) << " | "
              << "Memory: " << formatMemorySize(current.memoryUsageMB) << " | "
              << "CPU: " << std::fixed << std::setprecision(1) << current.cpuUsagePercent << "% | "
              << "Phase: " << getPhaseNameString(getCurrentPhase());
    
    if (m_totalItems > 0) {
        double progress = static_cast<double>(m_processedItems.load()) / m_totalItems.load();
        std::cout << " | Progress: " << std::fixed << std::setprecision(1) << (progress * 100) << "%";
    }
    
    std::cout << std::flush;
}

// ========== 私有方法实现 ==========

void CompilationMonitor::monitorThreadMain() {
    while (m_isMonitoring.load()) {
        if (!m_isPaused.load()) {
            updateResourceUsage();
            
            // 检查资源限制
            if (!checkResourceLimits()) {
                ResourceUsage current = getCurrentResourceUsage();
                if (m_resourceLimitCallback) {
                    m_resourceLimitCallback(current);
                }
                
                if (m_config.enableAutoTermination) {
                    logMessage("Resource limits exceeded, terminating compilation");
                    terminateCompilation();
                    break;
                }
            }
            
            // 检查超时
            if (checkTimeout()) {
                if (m_timeoutCallback) {
                    m_timeoutCallback();
                }
                
                if (m_config.enableAutoTermination) {
                    logMessage("Compilation timeout, terminating compilation");
                    terminateCompilation();
                    break;
                }
            }
            
            // 实时统计显示
            if (m_config.enableDetailedLogging) {
                printRealtimeStats();
            }
        }
        
        std::this_thread::sleep_for(m_config.resourceCheckInterval);
    }
}

void CompilationMonitor::deadlockThreadMain() {
    while (m_isMonitoring.load()) {
        if (!m_isPaused.load() && checkForDeadlock()) {
            logMessage("Potential deadlock detected");
            
            if (m_deadlockCallback) {
                m_deadlockCallback();
            }
            
            if (m_config.enableAutoTermination) {
                logMessage("Deadlock detected, terminating compilation");
                terminateCompilation();
                break;
            }
        }
        
        std::this_thread::sleep_for(m_config.deadlockCheckInterval);
    }
}

void CompilationMonitor::updateResourceUsage() {
    ResourceUsage current = getSystemResourceUsage();
    
    std::lock_guard<std::mutex> lock(m_statsMutex);
    m_stats.currentResourceUsage = current;
    
    // 更新最大使用量
    m_stats.maxResourceUsage.memoryUsageMB = std::max(m_stats.maxResourceUsage.memoryUsageMB, current.memoryUsageMB);
    m_stats.maxResourceUsage.cpuUsagePercent = std::max(m_stats.maxResourceUsage.cpuUsagePercent, current.cpuUsagePercent);
    m_stats.maxResourceUsage.heapSizeMB = std::max(m_stats.maxResourceUsage.heapSizeMB, current.heapSizeMB);
    m_stats.maxResourceUsage.stackSizeMB = std::max(m_stats.maxResourceUsage.stackSizeMB, current.stackSizeMB);
    m_stats.maxResourceUsage.fileHandleCount = std::max(m_stats.maxResourceUsage.fileHandleCount, current.fileHandleCount);
}

bool CompilationMonitor::checkTimeout() const {
    auto elapsed = getElapsedTime();
    return elapsed > m_config.maxCompilationTime;
}

void CompilationMonitor::logMessage(const std::string& message) const {
    if (!m_config.enableDetailedLogging) {
        return;
    }
    
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << "[" << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S") << "] " << message;
    
    // 输出到控制台
    std::cout << "\n" << ss.str() << std::endl;
    
    // 输出到日志文件
    if (!m_config.logFilePath.empty()) {
        std::ofstream logFile(m_config.logFilePath, std::ios::app);
        if (logFile.is_open()) {
            logFile << ss.str() << std::endl;
        }
    }
}

ResourceUsage CompilationMonitor::getSystemResourceUsage() const {
    ResourceUsage usage;
    
#ifdef _WIN32
    // Windows系统资源获取
    PROCESS_MEMORY_COUNTERS pmc;
    if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc))) {
        usage.memoryUsageMB = pmc.WorkingSetSize / (1024 * 1024);
    }
    
    // CPU使用率获取（简化版）
    FILETIME idleTime, kernelTime, userTime;
    if (GetSystemTimes(&idleTime, &kernelTime, &userTime)) {
        // 实际实现需要计算CPU使用率
        usage.cpuUsagePercent = 0.0; // 简化版
    }
#else
    // Linux/Unix系统资源获取
    struct rusage rusage;
    if (getrusage(RUSAGE_SELF, &rusage) == 0) {
        usage.memoryUsageMB = rusage.ru_maxrss / 1024; // Linux下是KB，转换为MB
    }
    
    // 读取/proc/stat获取CPU使用率（简化版）
    usage.cpuUsagePercent = 0.0; // 简化版
#endif
    
    return usage;
}

std::string CompilationMonitor::getPhaseNameString(CompilationPhase phase) const {
    switch (phase) {
        case CompilationPhase::LEXICAL_ANALYSIS: return "Lexical Analysis";
        case CompilationPhase::SYNTAX_ANALYSIS: return "Syntax Analysis";
        case CompilationPhase::SEMANTIC_ANALYSIS: return "Semantic Analysis";
        case CompilationPhase::AST_BUILDING: return "AST Building";
        case CompilationPhase::OPTIMIZATION: return "Optimization";
        case CompilationPhase::CODE_GENERATION: return "Code Generation";
        case CompilationPhase::LINKING: return "Linking";
        case CompilationPhase::UNKNOWN: return "Unknown";
        default: return "Undefined";
    }
}

std::string CompilationMonitor::formatDuration(std::chrono::milliseconds duration) const {
    auto ms = duration.count();
    auto seconds = ms / 1000;
    auto minutes = seconds / 60;
    auto hours = minutes / 60;
    
    ms %= 1000;
    seconds %= 60;
    minutes %= 60;
    
    std::stringstream ss;
    if (hours > 0) {
        ss << hours << "h ";
    }
    if (minutes > 0) {
        ss << minutes << "m ";
    }
    ss << seconds << "." << std::setfill('0') << std::setw(3) << ms << "s";
    
    return ss.str();
}

std::string CompilationMonitor::formatMemorySize(size_t sizeInMB) const {
    if (sizeInMB >= 1024) {
        double sizeInGB = static_cast<double>(sizeInMB) / 1024.0;
        std::stringstream ss;
        ss << std::fixed << std::setprecision(2) << sizeInGB << "GB";
        return ss.str();
    } else {
        return std::to_string(sizeInMB) + "MB";
    }
}

// ========== 其他类的实现（简化版） ==========

ScopedCompilationMonitor::ScopedCompilationMonitor(CompilationMonitor& monitor, const std::string& projectName)
    : m_monitor(monitor) {
    m_monitor.startMonitoring(projectName);
}

ScopedCompilationMonitor::~ScopedCompilationMonitor() {
    m_monitor.stopMonitoring();
}

CompilationMonitor& ScopedCompilationMonitor::getMonitor() {
    return m_monitor;
}

// ========== CompilationMonitorFactory 实现 ==========

std::unique_ptr<CompilationMonitor> CompilationMonitorFactory::createStandardMonitor() {
    CompilationMonitor::MonitorConfig config;
    config.maxCompilationTime = std::chrono::milliseconds(300000); // 5分钟
    config.maxMemoryUsageMB = 2048; // 2GB
    config.enableDetailedLogging = true;
    config.enableAutoTermination = true;
    return std::make_unique<CompilationMonitor>(config);
}

std::unique_ptr<CompilationMonitor> CompilationMonitorFactory::createDebugMonitor() {
    CompilationMonitor::MonitorConfig config;
    config.maxCompilationTime = std::chrono::milliseconds(600000); // 10分钟
    config.maxMemoryUsageMB = 4096; // 4GB
    config.enableDetailedLogging = true;
    config.enableAutoTermination = false;
    config.resourceCheckInterval = std::chrono::milliseconds(100);
    return std::make_unique<CompilationMonitor>(config);
}

std::unique_ptr<CompilationMonitor> CompilationMonitorFactory::createProductionMonitor() {
    CompilationMonitor::MonitorConfig config;
    config.maxCompilationTime = std::chrono::milliseconds(180000); // 3分钟
    config.maxMemoryUsageMB = 1024; // 1GB
    config.enableDetailedLogging = false;
    config.enableAutoTermination = true;
    return std::make_unique<CompilationMonitor>(config);
}

std::unique_ptr<CompilationMonitor> CompilationMonitorFactory::createHighPerformanceMonitor() {
    CompilationMonitor::MonitorConfig config;
    config.maxCompilationTime = std::chrono::milliseconds(60000); // 1分钟
    config.maxMemoryUsageMB = 512; // 512MB
    config.enableDetailedLogging = false;
    config.enableAutoTermination = true;
    config.resourceCheckInterval = std::chrono::milliseconds(1000);
    return std::make_unique<CompilationMonitor>(config);
}

std::unique_ptr<CompilationMonitor> CompilationMonitorFactory::createCustomMonitor(const CompilationMonitor::MonitorConfig& config) {
    return std::make_unique<CompilationMonitor>(config);
}

// ========== GlobalMonitorManager 实现 ==========

GlobalMonitorManager& GlobalMonitorManager::getInstance() {
    static GlobalMonitorManager instance;
    return instance;
}

void GlobalMonitorManager::setGlobalMonitor(std::unique_ptr<CompilationMonitor> monitor) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_globalMonitor = std::move(monitor);
}

CompilationMonitor* GlobalMonitorManager::getGlobalMonitor() {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_globalMonitor.get();
}

void GlobalMonitorManager::bindToCHTLProject() {
    if (!m_globalMonitor) {
        setGlobalMonitor(CompilationMonitorFactory::createStandardMonitor());
    }
    
    // 设置默认回调
    m_globalMonitor->setDeadlockCallback([]() {
        std::cerr << "\n[CRITICAL] Deadlock detected in CHTL compilation! Terminating..." << std::endl;
    });
    
    m_globalMonitor->setResourceLimitCallback([](const ResourceUsage& usage) {
        std::cerr << "\n[WARNING] Resource limits exceeded: Memory=" << usage.memoryUsageMB 
                  << "MB, CPU=" << usage.cpuUsagePercent << "%" << std::endl;
    });
    
    m_globalMonitor->setTimeoutCallback([]() {
        std::cerr << "\n[CRITICAL] Compilation timeout! Check for infinite loops." << std::endl;
    });
}

} // namespace Test
} // namespace CHTL