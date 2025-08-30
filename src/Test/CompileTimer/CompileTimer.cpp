#include "CompileTimer.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <thread>

namespace CHTL::Test {

// PerformanceMetrics实现
PerformanceMetrics::PerformanceMetrics()
    : elapsedTimeMs(0.0), memoryUsedBytes(0), peakMemoryBytes(0), 
      cpuUsagePercent(0), ioOperations(0) {
}

void PerformanceMetrics::start() {
    startTime = std::chrono::high_resolution_clock::now();
}

void PerformanceMetrics::end() {
    endTime = std::chrono::high_resolution_clock::now();
    elapsedTimeMs = std::chrono::duration<double, std::milli>(endTime - startTime).count();
}

double PerformanceMetrics::getDurationMs() const {
    return elapsedTimeMs;
}

String PerformanceMetrics::toString() const {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(3);
    oss << "Time: " << elapsedTimeMs << "ms, ";
    oss << "Memory: " << memoryUsedBytes << " bytes, ";
    oss << "Peak: " << peakMemoryBytes << " bytes";
    return oss.str();
}

// CompileTimer::TimerData实现
CompileTimer::TimerData::TimerData()
    : accumulatedTime(0.0), isRunning(false), callCount(0) {
}

// CompileTimer实现
CompileTimer::CompileTimer()
    : verbose_(false), autoReport_(false), reportInterval_(0) {
    globalStartTime_ = std::chrono::high_resolution_clock::now();
}

CompileTimer::~CompileTimer() {
    if (autoReport_) {
        std::cout << getTimingReport() << std::endl;
    }
}

void CompileTimer::startTimer(const String& timerName) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    TimerData& timer = timers_[timerName];
    if (!timer.isRunning) {
        updateTimer(timer, true);
        timer.callCount++;
        
        if (verbose_) {
            std::cout << "Timer started: " << timerName << std::endl;
        }
    }
}

void CompileTimer::endTimer(const String& timerName) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = timers_.find(timerName);
    if (it != timers_.end() && it->second.isRunning) {
        updateTimer(it->second, false);
        
        if (verbose_) {
            std::cout << "Timer ended: " << timerName 
                     << " (+" << std::fixed << std::setprecision(3) 
                     << std::chrono::duration<double, std::milli>(
                         it->second.endTime - it->second.startTime).count() 
                     << "ms)" << std::endl;
        }
    }
}

void CompileTimer::resetTimer(const String& timerName) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    timers_.erase(timerName);
}

void CompileTimer::startPhase(CompilePhase phase) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    TimerData& timer = phaseTimers_[phase];
    if (!timer.isRunning) {
        updateTimer(timer, true);
        timer.callCount++;
        
        if (verbose_) {
            std::cout << "Phase started: " << getPhaseDisplayName(phase) << std::endl;
        }
    }
}

void CompileTimer::endPhase(CompilePhase phase) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = phaseTimers_.find(phase);
    if (it != phaseTimers_.end() && it->second.isRunning) {
        updateTimer(it->second, false);
        
        if (verbose_) {
            std::cout << "Phase ended: " << getPhaseDisplayName(phase) 
                     << " (+" << std::fixed << std::setprecision(3) 
                     << std::chrono::duration<double, std::milli>(
                         it->second.endTime - it->second.startTime).count() 
                     << "ms)" << std::endl;
        }
    }
}

void CompileTimer::resetPhase(CompilePhase phase) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    phaseTimers_.erase(phase);
}

// CompileTimer::ScopedTimer实现
CompileTimer::ScopedTimer::ScopedTimer(CompileTimer& timer, const String& name)
    : timer_(timer), timerName_(name), isPhaseTimer_(false) {
    timer_.startTimer(timerName_);
}

CompileTimer::ScopedTimer::ScopedTimer(CompileTimer& timer, CompilePhase phase)
    : timer_(timer), phase_(phase), isPhaseTimer_(true) {
    timer_.startPhase(phase_);
}

CompileTimer::ScopedTimer::~ScopedTimer() {
    if (isPhaseTimer_) {
        timer_.endPhase(phase_);
    } else {
        timer_.endTimer(timerName_);
    }
}

std::unique_ptr<CompileTimer::ScopedTimer> CompileTimer::createScopedTimer(const String& name) {
    return std::make_unique<ScopedTimer>(*this, name);
}

std::unique_ptr<CompileTimer::ScopedTimer> CompileTimer::createScopedPhaseTimer(CompilePhase phase) {
    return std::make_unique<ScopedTimer>(*this, phase);
}

double CompileTimer::getElapsedTime(const String& timerName) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = timers_.find(timerName);
    if (it != timers_.end()) {
        return it->second.accumulatedTime;
    }
    
    return 0.0;
}

double CompileTimer::getPhaseTime(CompilePhase phase) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = phaseTimers_.find(phase);
    if (it != phaseTimers_.end()) {
        return it->second.accumulatedTime;
    }
    
    return 0.0;
}

double CompileTimer::getTotalTime() const {
    auto currentTime = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<double, std::milli>(currentTime - globalStartTime_).count();
}

std::map<String, double> CompileTimer::getAllTimers() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::map<String, double> result;
    for (const auto& [name, timer] : timers_) {
        result[name] = timer.accumulatedTime;
    }
    
    return result;
}

std::map<CompilePhase, double> CompileTimer::getAllPhases() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::map<CompilePhase, double> result;
    for (const auto& [phase, timer] : phaseTimers_) {
        result[phase] = timer.accumulatedTime;
    }
    
    return result;
}

PerformanceMetrics CompileTimer::getOverallMetrics() const {
    PerformanceMetrics metrics;
    metrics.elapsedTimeMs = getTotalTime();
    // 其他指标需要实际的系统调用来获取
    return metrics;
}

String CompileTimer::getTimingReport() const {
    std::ostringstream oss;
    
    oss << "\n=== Timing Report ===\n";
    oss << "Total Time: " << formatTime(getTotalTime()) << "\n\n";
    
    auto timers = getAllTimers();
    if (!timers.empty()) {
        oss << "Custom Timers:\n";
        oss << generateTimingTable(timers);
        oss << "\n";
    }
    
    return oss.str();
}

String CompileTimer::getPhaseReport() const {
    std::ostringstream oss;
    
    oss << "\n=== Phase Report ===\n";
    
    auto phases = getAllPhases();
    if (!phases.empty()) {
        oss << generatePhaseTable(phases);
    } else {
        oss << "No phase timing data available.\n";
    }
    
    return oss.str();
}

String CompileTimer::getDetailedReport() const {
    std::ostringstream oss;
    
    oss << getTimingReport();
    oss << getPhaseReport();
    
    return oss.str();
}

void CompileTimer::saveBaseline(const String& name) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    baselines_[name] = getAllTimers();
    phaseBaselines_[name] = getAllPhases();
}

String CompileTimer::compareWithBaseline(const String& baselineName) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto baselineIt = baselines_.find(baselineName);
    if (baselineIt == baselines_.end()) {
        return "Baseline '" + baselineName + "' not found.";
    }
    
    std::ostringstream oss;
    oss << "\n=== Comparison with Baseline: " << baselineName << " ===\n";
    
    auto currentTimers = getAllTimers();
    const auto& baselineTimers = baselineIt->second;
    
    for (const auto& [name, currentTime] : currentTimers) {
        auto baselineTimeIt = baselineTimers.find(name);
        if (baselineTimeIt != baselineTimers.end()) {
            double diff = currentTime - baselineTimeIt->second;
            double percentage = (diff / baselineTimeIt->second) * 100.0;
            
            oss << std::setw(20) << name << ": "
                << formatTime(currentTime) << " vs " << formatTime(baselineTimeIt->second)
                << " (" << std::showpos << std::fixed << std::setprecision(1) 
                << percentage << "%)" << std::noshowpos << "\n";
        }
    }
    
    return oss.str();
}

void CompileTimer::clearBaselines() {
    std::lock_guard<std::mutex> lock(mutex_);
    baselines_.clear();
    phaseBaselines_.clear();
}

void CompileTimer::exportToCSV(const String& filePath) const {
    std::ofstream file(filePath);
    if (!file.is_open()) {
        return;
    }
    
    file << "Timer Name,Time (ms),Call Count\n";
    
    std::lock_guard<std::mutex> lock(mutex_);
    for (const auto& [name, timer] : timers_) {
        file << name << "," << std::fixed << std::setprecision(3) 
             << timer.accumulatedTime << "," << timer.callCount << "\n";
    }
    
    file.close();
}

void CompileTimer::exportToJSON(const String& filePath) const {
    std::ofstream file(filePath);
    if (!file.is_open()) {
        return;
    }
    
    file << "{\n";
    file << "  \"totalTime\": " << std::fixed << std::setprecision(3) << getTotalTime() << ",\n";
    file << "  \"timers\": {\n";
    
    std::lock_guard<std::mutex> lock(mutex_);
    bool first = true;
    for (const auto& [name, timer] : timers_) {
        if (!first) file << ",\n";
        file << "    \"" << name << "\": {\n";
        file << "      \"time\": " << std::fixed << std::setprecision(3) << timer.accumulatedTime << ",\n";
        file << "      \"callCount\": " << timer.callCount << "\n";
        file << "    }";
        first = false;
    }
    
    file << "\n  }\n";
    file << "}\n";
    
    file.close();
}

void CompileTimer::exportToXML(const String& filePath) const {
    std::ofstream file(filePath);
    if (!file.is_open()) {
        return;
    }
    
    file << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    file << "<timingReport>\n";
    file << "  <totalTime>" << std::fixed << std::setprecision(3) << getTotalTime() << "</totalTime>\n";
    file << "  <timers>\n";
    
    std::lock_guard<std::mutex> lock(mutex_);
    for (const auto& [name, timer] : timers_) {
        file << "    <timer name=\"" << name << "\">\n";
        file << "      <time>" << std::fixed << std::setprecision(3) << timer.accumulatedTime << "</time>\n";
        file << "      <callCount>" << timer.callCount << "</callCount>\n";
        file << "    </timer>\n";
    }
    
    file << "  </timers>\n";
    file << "</timingReport>\n";
    
    file.close();
}

void CompileTimer::setVerbose(bool verbose) {
    verbose_ = verbose;
}

void CompileTimer::setAutoReport(bool autoReport) {
    autoReport_ = autoReport;
}

void CompileTimer::setReportInterval(int intervalSeconds) {
    reportInterval_ = intervalSeconds;
}

void CompileTimer::resetAllTimers() {
    std::lock_guard<std::mutex> lock(mutex_);
    timers_.clear();
}

void CompileTimer::resetAllPhases() {
    std::lock_guard<std::mutex> lock(mutex_);
    phaseTimers_.clear();
}

void CompileTimer::reset() {
    std::lock_guard<std::mutex> lock(mutex_);
    timers_.clear();
    phaseTimers_.clear();
    globalStartTime_ = std::chrono::high_resolution_clock::now();
}

String CompileTimer::getPhaseDisplayName(CompilePhase phase) {
    switch (phase) {
        case CompilePhase::LEXICAL_ANALYSIS: return "Lexical Analysis";
        case CompilePhase::SYNTAX_PARSING: return "Syntax Parsing";
        case CompilePhase::AST_BUILDING: return "AST Building";
        case CompilePhase::SEMANTIC_ANALYSIS: return "Semantic Analysis";
        case CompilePhase::CODE_GENERATION: return "Code Generation";
        case CompilePhase::OPTIMIZATION: return "Optimization";
        case CompilePhase::OUTPUT_WRITING: return "Output Writing";
        case CompilePhase::TOTAL_COMPILATION: return "Total Compilation";
        default: return "Unknown Phase";
    }
}

void CompileTimer::updateTimer(TimerData& timer, bool start) {
    if (start) {
        timer.startTime = std::chrono::high_resolution_clock::now();
        timer.isRunning = true;
    } else {
        timer.endTime = std::chrono::high_resolution_clock::now();
        timer.isRunning = false;
        
        double elapsed = std::chrono::duration<double, std::milli>(timer.endTime - timer.startTime).count();
        timer.accumulatedTime += elapsed;
    }
}

String CompileTimer::formatTime(double timeMs) const {
    std::ostringstream oss;
    
    if (timeMs < 1.0) {
        oss << std::fixed << std::setprecision(3) << timeMs << "ms";
    } else if (timeMs < 1000.0) {
        oss << std::fixed << std::setprecision(1) << timeMs << "ms";
    } else {
        double seconds = timeMs / 1000.0;
        oss << std::fixed << std::setprecision(2) << seconds << "s";
    }
    
    return oss.str();
}

String CompileTimer::generateTimingTable(const std::map<String, double>& timings) const {
    std::ostringstream oss;
    
    oss << std::setw(25) << "Timer Name" << " | " 
        << std::setw(12) << "Time" << " | "
        << std::setw(10) << "Percentage" << "\n";
    oss << std::string(50, '-') << "\n";
    
    double totalTime = 0.0;
    for (const auto& [name, time] : timings) {
        totalTime += time;
    }
    
    for (const auto& [name, time] : timings) {
        double percentage = totalTime > 0 ? (time / totalTime) * 100.0 : 0.0;
        
        oss << std::setw(25) << name << " | "
            << std::setw(12) << formatTime(time) << " | "
            << std::setw(9) << std::fixed << std::setprecision(1) << percentage << "%" << "\n";
    }
    
    return oss.str();
}

String CompileTimer::generatePhaseTable(const std::map<CompilePhase, double>& phases) const {
    std::map<String, double> phaseNames;
    for (const auto& [phase, time] : phases) {
        phaseNames[getPhaseDisplayName(phase)] = time;
    }
    
    return generateTimingTable(phaseNames);
}

// MemoryMonitor基础实现
MemoryMonitor::MemoryMonitor()
    : peakMemoryUsage_(0), isMonitoring_(false), leakDetectionEnabled_(false),
      memoryLimit_(0), warningThreshold_(0), shouldStopMonitoring_(false) {
}

MemoryMonitor::~MemoryMonitor() {
    stopMonitoring();
}

MemoryMonitor::MemorySnapshot::MemorySnapshot()
    : virtualMemory(0), physicalMemory(0), heapMemory(0), stackMemory(0) {
    timestamp = std::chrono::high_resolution_clock::now();
}

String MemoryMonitor::MemorySnapshot::toString() const {
    std::ostringstream oss;
    oss << "Virtual: " << virtualMemory << " bytes, ";
    oss << "Physical: " << physicalMemory << " bytes, ";
    oss << "Heap: " << heapMemory << " bytes";
    if (!description.empty()) {
        oss << " (" << description << ")";
    }
    return oss.str();
}

void MemoryMonitor::startMonitoring() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (!isMonitoring_) {
        isMonitoring_ = true;
        shouldStopMonitoring_ = false;
        monitoringThread_ = std::thread(&MemoryMonitor::monitoringLoop, this);
    }
}

void MemoryMonitor::stopMonitoring() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (isMonitoring_) {
        shouldStopMonitoring_ = true;
        isMonitoring_ = false;
        
        if (monitoringThread_.joinable()) {
            monitoringThread_.join();
        }
    }
}

void MemoryMonitor::takeSnapshot(const String& description) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    MemorySnapshot snapshot;
    snapshot.description = description;
    snapshot.virtualMemory = getCurrentVirtualMemory();
    snapshot.physicalMemory = getCurrentPhysicalMemory();
    snapshot.heapMemory = getCurrentHeapMemory();
    
    snapshots_.push_back(snapshot);
    
    if (snapshot.physicalMemory > peakMemoryUsage_) {
        peakMemoryUsage_ = snapshot.physicalMemory;
    }
    
    checkMemoryLimits(snapshot);
}

void MemoryMonitor::monitoringLoop() {
    while (!shouldStopMonitoring_) {
        takeSnapshot("auto");
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

size_t MemoryMonitor::getCurrentVirtualMemory() const {
    // 这里需要平台特定的实现
    // 暂时返回0
    return 0;
}

size_t MemoryMonitor::getCurrentPhysicalMemory() const {
    // 这里需要平台特定的实现
    // 暂时返回0
    return 0;
}

size_t MemoryMonitor::getCurrentHeapMemory() const {
    // 这里需要平台特定的实现
    // 暂时返回0
    return 0;
}

void MemoryMonitor::checkMemoryLimits(const MemorySnapshot& snapshot) {
    if (warningThreshold_ > 0 && snapshot.physicalMemory > warningThreshold_ && warningCallback_) {
        warningCallback_(snapshot);
    }
    
    if (memoryLimit_ > 0 && snapshot.physicalMemory > memoryLimit_ && limitCallback_) {
        limitCallback_(snapshot);
    }
}

// InfiniteLoopDetector基础实现
InfiniteLoopDetector::InfiniteLoopDetector()
    : timeoutSeconds_(30), maxIterations_(10000), checkInterval_(1000),
      isDetecting_(false), emergencyStop_(false), shouldStopDetection_(false) {
}

InfiniteLoopDetector::~InfiniteLoopDetector() {
    stopDetection();
}

InfiniteLoopDetector::LoopPoint::LoopPoint(const String& n)
    : name(n), hitCount(0), isSuspicious(false) {
    firstHit = std::chrono::high_resolution_clock::now();
    lastHit = firstHit;
}

void InfiniteLoopDetector::setTimeout(int timeoutSeconds) {
    timeoutSeconds_ = timeoutSeconds;
}

void InfiniteLoopDetector::setMaxIterations(size_t maxIterations) {
    maxIterations_ = maxIterations;
}

void InfiniteLoopDetector::setCheckInterval(int intervalMs) {
    checkInterval_ = intervalMs;
}

void InfiniteLoopDetector::registerLoopPoint(const String& pointName) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (loopPoints_.find(pointName) == loopPoints_.end()) {
        loopPoints_[pointName] = LoopPoint(pointName);
    }
}

void InfiniteLoopDetector::hitLoopPoint(const String& pointName) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = loopPoints_.find(pointName);
    if (it != loopPoints_.end()) {
        it->second.hitCount++;
        it->second.lastHit = std::chrono::high_resolution_clock::now();
        
        if (it->second.hitCount > maxIterations_) {
            it->second.isSuspicious = true;
            
            if (loopCallback_) {
                loopCallback_(pointName, it->second.hitCount);
            }
        }
    }
}

void InfiniteLoopDetector::startDetection() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (!isDetecting_) {
        isDetecting_ = true;
        shouldStopDetection_ = false;
        detectionThread_ = std::thread(&InfiniteLoopDetector::detectionLoop, this);
    }
}

void InfiniteLoopDetector::stopDetection() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (isDetecting_) {
        shouldStopDetection_ = true;
        isDetecting_ = false;
        
        if (detectionThread_.joinable()) {
            detectionThread_.join();
        }
    }
}

void InfiniteLoopDetector::detectionLoop() {
    while (!shouldStopDetection_) {
        checkLoopPoints();
        std::this_thread::sleep_for(std::chrono::milliseconds(checkInterval_));
    }
}

void InfiniteLoopDetector::checkLoopPoints() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto currentTime = std::chrono::high_resolution_clock::now();
    
    for (auto& [name, point] : loopPoints_) {
        if (isLoopSuspicious(point)) {
            point.isSuspicious = true;
        }
    }
}

bool InfiniteLoopDetector::isLoopSuspicious(const LoopPoint& point) const {
    auto timeSinceFirst = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::high_resolution_clock::now() - point.firstHit).count();
    
    return point.hitCount > maxIterations_ || timeSinceFirst > timeoutSeconds_;
}

// PerformanceMonitor基础实现
PerformanceMonitor::PerformanceMonitor() {
    timer_ = std::make_unique<CompileTimer>();
    memoryMonitor_ = std::make_unique<MemoryMonitor>();
    loopDetector_ = std::make_unique<InfiniteLoopDetector>();
}

PerformanceMonitor::~PerformanceMonitor() = default;

CompileTimer& PerformanceMonitor::getTimer() {
    return *timer_;
}

MemoryMonitor& PerformanceMonitor::getMemoryMonitor() {
    return *memoryMonitor_;
}

InfiniteLoopDetector& PerformanceMonitor::getLoopDetector() {
    return *loopDetector_;
}

PerformanceMonitor::PerformanceAlert::PerformanceAlert(Type t, const String& msg, const String& det)
    : type(t), message(msg), details(det) {
    timestamp = std::chrono::high_resolution_clock::now();
}

String PerformanceMonitor::PerformanceAlert::toString() const {
    std::ostringstream oss;
    oss << "Alert: " << message;
    if (!details.empty()) {
        oss << " - " << details;
    }
    return oss.str();
}

} // namespace CHTL::Test