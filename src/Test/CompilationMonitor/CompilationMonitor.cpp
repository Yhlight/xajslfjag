#include "CompilationMonitor.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <csignal>

#ifdef _WIN32
#include <windows.h>
#include <psapi.h>
#else
#include <sys/resource.h>
#include <unistd.h>
#endif

namespace CHTL {
namespace Test {

// CompilationMonitor implementation
CompilationMonitor::CompilationMonitor() {
    startTime_ = std::chrono::steady_clock::now();
    phaseStartTime_ = startTime_;
    lastProgressTime_ = startTime_;
}

CompilationMonitor::~CompilationMonitor() {
    stop();
}

void CompilationMonitor::start() {
    if (running_) return;
    
    running_ = true;
    shouldTerminate_ = false;
    
    monitorThread_ = std::make_unique<std::thread>(&CompilationMonitor::monitorLoop, this);
}

void CompilationMonitor::stop() {
    if (!running_) return;
    
    running_ = false;
    cv_.notify_all();
    
    if (monitorThread_ && monitorThread_->joinable()) {
        monitorThread_->join();
    }
    
    // 计算总时间
    auto endTime = std::chrono::steady_clock::now();
    stats_.totalTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime_);
}

void CompilationMonitor::setTimeout(std::chrono::milliseconds timeout) {
    timeout_ = timeout;
}

void CompilationMonitor::setMemoryLimit(size_t bytes) {
    memoryLimit_ = bytes;
}

void CompilationMonitor::setTimeoutCallback(TimeoutCallback callback) {
    timeoutCallback_ = callback;
}

void CompilationMonitor::setProgressCallback(ProgressCallback callback) {
    progressCallback_ = callback;
}

void CompilationMonitor::setResourceCallback(ResourceCallback callback) {
    resourceCallback_ = callback;
}

void CompilationMonitor::enterPhase(CompilationPhase phase) {
    auto now = std::chrono::steady_clock::now();
    
    // 记录上一个阶段的时间
    if (currentPhase_ != phase) {
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - phaseStartTime_);
        std::lock_guard<std::mutex> lock(statsMutex_);
        stats_.phaseTimes[currentPhase_.load()] += duration;
    }
    
    currentPhase_ = phase;
    phaseStartTime_ = now;
    currentProgress_ = 0.0;
    
    if (progressCallback_) {
        progressCallback_(phase, 0.0);
    }
}

void CompilationMonitor::updateProgress(double progress) {
    currentProgress_ = progress;
    lastProgressTime_ = std::chrono::steady_clock::now();
    
    if (progressCallback_) {
        progressCallback_(currentPhase_, progress);
    }
}

CompilationStats CompilationMonitor::getStats() const {
    std::lock_guard<std::mutex> lock(statsMutex_);
    return stats_;
}

std::string CompilationMonitor::getStatsReport() const {
    auto stats = getStats();
    std::stringstream ss;
    
    ss << "\n=== Compilation Statistics ===\n";
    ss << "Total Time: " << stats.totalTime.count() << " ms\n";
    
    ss << "\nPhase Times:\n";
    for (const auto& [phase, time] : stats.phaseTimes) {
        std::string phaseName;
        switch (phase) {
            case CompilationPhase::SCANNING: phaseName = "Scanning"; break;
            case CompilationPhase::LEXING: phaseName = "Lexing"; break;
            case CompilationPhase::PARSING: phaseName = "Parsing"; break;
            case CompilationPhase::SEMANTIC: phaseName = "Semantic"; break;
            case CompilationPhase::OPTIMIZATION: phaseName = "Optimization"; break;
            case CompilationPhase::GENERATION: phaseName = "Generation"; break;
            case CompilationPhase::COMPLETE: phaseName = "Complete"; break;
        }
        ss << "  " << std::setw(15) << phaseName << ": " << time.count() << " ms\n";
    }
    
    ss << "\nResource Usage:\n";
    ss << "  Memory Used: " << (stats.resourceUsage.memoryUsed / 1024 / 1024) << " MB\n";
    ss << "  Peak Memory: " << (stats.resourceUsage.peakMemory / 1024 / 1024) << " MB\n";
    ss << "  CPU Usage: " << std::fixed << std::setprecision(1) << stats.resourceUsage.cpuUsage << "%\n";
    
    ss << "\nProcessing Statistics:\n";
    ss << "  Lines Processed: " << stats.linesProcessed << "\n";
    ss << "  Tokens Generated: " << stats.tokensGenerated << "\n";
    ss << "  Nodes Created: " << stats.nodesCreated << "\n";
    ss << "  Errors Found: " << stats.errorsFound << "\n";
    ss << "  Warnings Found: " << stats.warningsFound << "\n";
    
    if (stats.totalTime.count() > 0) {
        double linesPerSecond = (stats.linesProcessed * 1000.0) / stats.totalTime.count();
        ss << "\nPerformance:\n";
        ss << "  Lines/Second: " << std::fixed << std::setprecision(0) << linesPerSecond << "\n";
    }
    
    return ss.str();
}

void CompilationMonitor::checkProgress() {
    auto now = std::chrono::steady_clock::now();
    auto timeSinceLastProgress = std::chrono::duration_cast<std::chrono::milliseconds>(
        now - lastProgressTime_);
    
    if (detectDeadlock_ && timeSinceLastProgress > deadlockTimeout_) {
        std::cerr << "WARNING: No progress for " << timeSinceLastProgress.count() 
                  << " ms, possible deadlock detected!\n";
        
        if (timeoutCallback_) {
            timeoutCallback_();
        }
        
        terminateProcess();
    }
}

void CompilationMonitor::monitorLoop() {
    while (running_) {
        std::unique_lock<std::mutex> lock(cvMutex_);
        cv_.wait_for(lock, std::chrono::seconds(1), [this] { return !running_; });
        
        if (!running_) break;
        
        // 检查各种限制
        checkTimeout();
        checkMemoryLimit();
        checkDeadlock();
        
        // 更新资源使用情况
        auto usage = getCurrentResourceUsage();
        {
            std::lock_guard<std::mutex> statsLock(statsMutex_);
            stats_.resourceUsage = usage;
        }
        
        if (resourceCallback_) {
            resourceCallback_(usage);
        }
    }
}

ResourceUsage CompilationMonitor::getCurrentResourceUsage() {
    ResourceUsage usage;
    
    usage.memoryUsed = getCurrentMemoryUsage();
    usage.cpuUsage = getCurrentCPUUsage();
    
    // 更新峰值内存
    {
        std::lock_guard<std::mutex> lock(statsMutex_);
        stats_.resourceUsage.peakMemory = std::max(stats_.resourceUsage.peakMemory, usage.memoryUsed);
    }
    
    return usage;
}

size_t CompilationMonitor::getCurrentMemoryUsage() {
#ifdef _WIN32
    PROCESS_MEMORY_COUNTERS pmc;
    if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc))) {
        return pmc.WorkingSetSize;
    }
#else
    struct rusage usage;
    if (getrusage(RUSAGE_SELF, &usage) == 0) {
        return usage.ru_maxrss * 1024;  // Linux返回KB
    }
#endif
    return 0;
}

double CompilationMonitor::getCurrentCPUUsage() {
    // 简化实现，实际应该计算进程CPU使用率
    return 0.0;
}

void CompilationMonitor::checkTimeout() {
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - startTime_);
    
    if (elapsed > timeout_) {
        std::cerr << "ERROR: Compilation timeout exceeded (" << timeout_.count() << " ms)\n";
        
        if (timeoutCallback_) {
            timeoutCallback_();
        }
        
        terminateProcess();
    }
}

void CompilationMonitor::checkMemoryLimit() {
    auto memUsage = getCurrentMemoryUsage();
    
    if (memUsage > memoryLimit_) {
        std::cerr << "ERROR: Memory limit exceeded (" 
                  << (memoryLimit_ / 1024 / 1024) << " MB)\n";
        
        if (timeoutCallback_) {
            timeoutCallback_();
        }
        
        terminateProcess();
    }
}

void CompilationMonitor::checkDeadlock() {
    if (!detectDeadlock_) return;
    
    checkProgress();
}

void CompilationMonitor::terminateProcess() {
    shouldTerminate_ = true;
    
    // 给进程一个机会正常退出
    std::this_thread::sleep_for(std::chrono::seconds(1));
    
    // 如果还在运行，强制终止
    if (running_) {
        std::cerr << "FATAL: Forcefully terminating compilation process\n";
        std::raise(SIGTERM);
    }
}

// PerformanceProfiler implementation
void PerformanceProfiler::startTiming(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    timings_[name] = {
        std::chrono::steady_clock::now(),
        {name, std::chrono::microseconds{0}, 0}
    };
}

void PerformanceProfiler::endTiming(const std::string& name) {
    auto endTime = std::chrono::steady_clock::now();
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = timings_.find(name);
    if (it == timings_.end()) return;
    
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
        endTime - it->second.startTime);
    
    // 更新或创建完成的profile数据
    auto& completed = completed_[name];
    completed.name = name;
    completed.totalTime += duration;
    completed.callCount++;
    completed.minTime = std::min(completed.minTime, duration);
    completed.maxTime = std::max(completed.maxTime, duration);
    
    timings_.erase(it);
}

std::vector<PerformanceProfiler::ProfileData> PerformanceProfiler::getProfileData() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::vector<ProfileData> result;
    for (const auto& [name, data] : completed_) {
        result.push_back(data);
    }
    
    // 按总时间排序
    std::sort(result.begin(), result.end(), 
              [](const ProfileData& a, const ProfileData& b) {
                  return a.totalTime > b.totalTime;
              });
    
    return result;
}

std::string PerformanceProfiler::getReport() const {
    auto data = getProfileData();
    std::stringstream ss;
    
    ss << "\n=== Performance Profile ===\n";
    ss << std::setw(30) << "Function" 
       << std::setw(10) << "Calls"
       << std::setw(12) << "Total (ms)"
       << std::setw(12) << "Avg (μs)"
       << std::setw(12) << "Min (μs)"
       << std::setw(12) << "Max (μs)" << "\n";
    ss << std::string(88, '-') << "\n";
    
    for (const auto& d : data) {
        double avgTime = d.callCount > 0 ? 
            static_cast<double>(d.totalTime.count()) / d.callCount : 0.0;
        
        ss << std::setw(30) << d.name
           << std::setw(10) << d.callCount
           << std::setw(12) << std::fixed << std::setprecision(2) 
           << (d.totalTime.count() / 1000.0)
           << std::setw(12) << std::fixed << std::setprecision(1) << avgTime
           << std::setw(12) << d.minTime.count()
           << std::setw(12) << d.maxTime.count() << "\n";
    }
    
    return ss.str();
}

void PerformanceProfiler::reset() {
    std::lock_guard<std::mutex> lock(mutex_);
    timings_.clear();
    completed_.clear();
}

// MemoryTracker implementation
void MemoryTracker::recordAllocation(void* ptr, size_t size, const char* file, int line) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    allocations_[ptr] = {
        size,
        file ? file : "unknown",
        line,
        std::chrono::steady_clock::now()
    };
    
    currentUsage_ += size;
    if (currentUsage_ > peakUsage_) {
        peakUsage_ = currentUsage_.load();
    }
}

void MemoryTracker::recordDeallocation(void* ptr) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = allocations_.find(ptr);
    if (it != allocations_.end()) {
        currentUsage_ -= it->second.size;
        allocations_.erase(it);
    }
}

std::vector<MemoryTracker::AllocationInfo> MemoryTracker::getActiveAllocations() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::vector<AllocationInfo> result;
    for (const auto& [ptr, info] : allocations_) {
        result.push_back(info);
    }
    
    // 按大小排序
    std::sort(result.begin(), result.end(),
              [](const AllocationInfo& a, const AllocationInfo& b) {
                  return a.size > b.size;
              });
    
    return result;
}

std::string MemoryTracker::getReport() const {
    std::stringstream ss;
    
    ss << "\n=== Memory Report ===\n";
    ss << "Current Usage: " << (currentUsage_ / 1024 / 1024) << " MB\n";
    ss << "Peak Usage: " << (peakUsage_ / 1024 / 1024) << " MB\n";
    
    auto allocations = getActiveAllocations();
    ss << "\nTop Allocations:\n";
    
    size_t count = 0;
    for (const auto& alloc : allocations) {
        if (count++ >= 10) break;  // 只显示前10个
        
        ss << "  " << std::setw(10) << (alloc.size / 1024) << " KB"
           << " at " << alloc.file << ":" << alloc.line << "\n";
    }
    
    return ss.str();
}

void MemoryTracker::reset() {
    std::lock_guard<std::mutex> lock(mutex_);
    allocations_.clear();
    currentUsage_ = 0;
    peakUsage_ = 0;
}

// MonitorManager implementation
std::string MonitorManager::generateFullReport() const {
    std::stringstream ss;
    
    ss << compilationMonitor_.getStatsReport();
    ss << performanceProfiler_.getReport();
    ss << memoryTracker_.getReport();
    
    return ss.str();
}

void MonitorManager::saveReport(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open report file: " << filename << "\n";
        return;
    }
    
    file << generateFullReport();
    file.close();
    
    std::cout << "Report saved to: " << filename << "\n";
}

} // namespace Test
} // namespace CHTL