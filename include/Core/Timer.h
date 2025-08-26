#ifndef CHTL_CORE_TIMER_H
#define CHTL_CORE_TIMER_H

#include "Core/Common.h"
#include <chrono>

namespace CHTL {

// 编译计时器
class CompileTimer {
private:
    using Clock = std::chrono::high_resolution_clock;
    using TimePoint = Clock::time_point;
    using Duration = std::chrono::duration<double>;
    
    struct PhaseTime {
        TimePoint start;
        TimePoint end;
        Duration duration;
        bool completed = false;
    };
    
public:
    CompileTimer() = default;
    ~CompileTimer() = default;
    
    // 开始计时
    void Start();
    
    // 停止计时
    void Stop();
    
    // 开始阶段计时
    void StartPhase(const String& phaseName);
    
    // 结束阶段计时
    void EndPhase(const String& phaseName);
    
    // 获取总耗时（秒）
    double GetElapsedSeconds() const;
    
    // 获取总耗时（毫秒）
    double GetElapsedMilliseconds() const;
    
    // 获取阶段耗时（秒）
    double GetPhaseSeconds(const String& phaseName) const;
    
    // 获取阶段耗时（毫秒）
    double GetPhaseMilliseconds(const String& phaseName) const;
    
    // 获取格式化的耗时字符串
    String GetElapsedString() const;
    
    // 获取阶段格式化的耗时字符串
    String GetPhaseString(const String& phaseName) const;
    
    // 获取所有阶段的统计信息
    String GetFullReport() const;
    
    // 重置计时器
    void Reset();
    
    // 获取编译状态
    String GetStatus() const;
    
    // 格式化持续时间
    static String FormatDuration(double seconds);
    
private:
    TimePoint startTime_;
    TimePoint endTime_;
    HashMap<String, PhaseTime> phases_;
    bool running_ = false;
};

// RAII阶段计时器
class PhaseTimer {
private:
    CompileTimer* timer_;
    String phaseName_;
    
public:
    PhaseTimer(CompileTimer* timer, const String& phaseName)
        : timer_(timer), phaseName_(phaseName) {
        if (timer_) {
            timer_->StartPhase(phaseName_);
        }
    }
    
    ~PhaseTimer() {
        if (timer_) {
            timer_->EndPhase(phaseName_);
        }
    }
    
    // 禁用拷贝
    PhaseTimer(const PhaseTimer&) = delete;
    PhaseTimer& operator=(const PhaseTimer&) = delete;
    
    // 允许移动
    PhaseTimer(PhaseTimer&& other) noexcept
        : timer_(other.timer_), phaseName_(std::move(other.phaseName_)) {
        other.timer_ = nullptr;
    }
    
    PhaseTimer& operator=(PhaseTimer&& other) noexcept {
        if (this != &other) {
            if (timer_) {
                timer_->EndPhase(phaseName_);
            }
            timer_ = other.timer_;
            phaseName_ = std::move(other.phaseName_);
            other.timer_ = nullptr;
        }
        return *this;
    }
};

// 性能统计器
class PerformanceStats {
private:
    struct Stats {
        size_t count = 0;
        double totalTime = 0.0;
        double minTime = std::numeric_limits<double>::max();
        double maxTime = 0.0;
    };
    
    HashMap<String, Stats> stats_;
    
public:
    // 记录操作耗时
    void Record(const String& operation, double seconds);
    
    // 获取操作统计信息
    String GetStats(const String& operation) const;
    
    // 获取所有统计信息
    String GetAllStats() const;
    
    // 清空统计
    void Clear();
};

// 全局性能统计器
extern PerformanceStats& GlobalPerfStats();

// 便捷宏定义
#define PHASE_TIMER(timer, phase) CHTL::PhaseTimer _phase_timer(timer, phase)
#define PERF_RECORD(op, time) CHTL::GlobalPerfStats().Record(op, time)

} // namespace CHTL

#endif // CHTL_CORE_TIMER_H