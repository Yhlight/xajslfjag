#include "Core/Timer.h"
#include <sstream>
#include <iomanip>

namespace CHTL {

void CompileTimer::Start() {
    startTime_ = Clock::now();
    running_ = true;
}

void CompileTimer::Stop() {
    if (running_) {
        endTime_ = Clock::now();
        running_ = false;
    }
}

void CompileTimer::StartPhase(const String& phaseName) {
    phases_[phaseName] = PhaseTime{Clock::now(), TimePoint(), Duration(0), false};
}

void CompileTimer::EndPhase(const String& phaseName) {
    auto it = phases_.find(phaseName);
    if (it != phases_.end() && !it->second.completed) {
        it->second.end = Clock::now();
        it->second.duration = it->second.end - it->second.start;
        it->second.completed = true;
    }
}

double CompileTimer::GetElapsedSeconds() const {
    if (running_) {
        Duration elapsed = Clock::now() - startTime_;
        return elapsed.count();
    } else {
        Duration elapsed = endTime_ - startTime_;
        return elapsed.count();
    }
}

double CompileTimer::GetElapsedMilliseconds() const {
    return GetElapsedSeconds() * 1000.0;
}

double CompileTimer::GetPhaseSeconds(const String& phaseName) const {
    auto it = phases_.find(phaseName);
    if (it != phases_.end() && it->second.completed) {
        return it->second.duration.count();
    }
    return 0.0;
}

double CompileTimer::GetPhaseMilliseconds(const String& phaseName) const {
    return GetPhaseSeconds(phaseName) * 1000.0;
}

String CompileTimer::GetElapsedString() const {
    return FormatDuration(GetElapsedSeconds());
}

String CompileTimer::GetPhaseString(const String& phaseName) const {
    return FormatDuration(GetPhaseSeconds(phaseName));
}

String CompileTimer::GetFullReport() const {
    std::stringstream ss;
    ss << "编译时间统计报告:\n";
    ss << "总耗时: " << GetElapsedString() << "\n";
    ss << "\n各阶段耗时:\n";
    
    for (const auto& [name, phase] : phases_) {
        if (phase.completed) {
            ss << "  " << std::left << std::setw(30) << name 
               << ": " << FormatDuration(phase.duration.count()) 
               << " (" << std::fixed << std::setprecision(1) 
               << (phase.duration.count() / GetElapsedSeconds() * 100.0) << "%)\n";
        }
    }
    
    return ss.str();
}

void CompileTimer::Reset() {
    phases_.clear();
    running_ = false;
}

String CompileTimer::FormatDuration(double seconds) {
    if (seconds < 0.001) {
        return std::to_string(static_cast<int>(seconds * 1000000)) + "μs";
    } else if (seconds < 1.0) {
        return std::to_string(static_cast<int>(seconds * 1000)) + "ms";
    } else if (seconds < 60.0) {
        std::stringstream ss;
        ss << std::fixed << std::setprecision(2) << seconds << "s";
        return ss.str();
    } else {
        int minutes = static_cast<int>(seconds / 60);
        double remainingSeconds = seconds - (minutes * 60);
        std::stringstream ss;
        ss << minutes << "m " << std::fixed << std::setprecision(1) << remainingSeconds << "s";
        return ss.str();
    }
}

// PerformanceStats 实现
void PerformanceStats::Record(const String& operation, double seconds) {
    auto& stat = stats_[operation];
    stat.count++;
    stat.totalTime += seconds;
    stat.minTime = std::min(stat.minTime, seconds);
    stat.maxTime = std::max(stat.maxTime, seconds);
}

String PerformanceStats::GetStats(const String& operation) const {
    auto it = stats_.find(operation);
    if (it == stats_.end()) {
        return operation + ": 无统计数据";
    }
    
    const auto& stat = it->second;
    double avgTime = stat.totalTime / stat.count;
    
    std::stringstream ss;
    ss << operation << ": ";
    ss << "次数=" << stat.count << ", ";
    ss << "总计=" << CompileTimer::FormatDuration(stat.totalTime) << ", ";
    ss << "平均=" << CompileTimer::FormatDuration(avgTime) << ", ";
    ss << "最小=" << CompileTimer::FormatDuration(stat.minTime) << ", ";
    ss << "最大=" << CompileTimer::FormatDuration(stat.maxTime);
    
    return ss.str();
}

String PerformanceStats::GetAllStats() const {
    std::stringstream ss;
    ss << "性能统计:\n";
    
    for (const auto& [operation, stat] : stats_) {
        ss << "  " << GetStats(operation) << "\n";
    }
    
    return ss.str();
}

void PerformanceStats::Clear() {
    stats_.clear();
}

// 全局性能统计器
PerformanceStats& GlobalPerfStats() {
    static PerformanceStats instance;
    return instance;
}

} // namespace CHTL