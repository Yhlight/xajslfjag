#ifndef CHTL_CORE_LOGGER_H
#define CHTL_CORE_LOGGER_H

#include "Core/Common.h"
#include <mutex>

namespace CHTL {

// 日志级别
enum class LogLevel {
    Debug = 0,
    Info = 1,
    Warning = 2,
    Error = 3,
    Fatal = 4
};

// 日志管理器
class Logger {
private:
    static LogLevel currentLevel_;
    static std::mutex mutex_;
    static bool initialized_;
    static String logFile_;
    static bool enableColor_;
    
    // 获取日志级别字符串
    static String GetLevelString(LogLevel level);
    
    // 获取日志级别颜色（ANSI转义序列）
    static String GetLevelColor(LogLevel level);
    
    // 格式化时间戳
    static String GetTimestamp();
    
    // 输出日志
    static void Output(LogLevel level, const String& message);
    
public:
    // 初始化日志系统
    static void Initialize(LogLevel level = LogLevel::Info, 
                          const String& logFile = "",
                          bool enableColor = true);
    
    // 设置日志级别
    static void SetLevel(LogLevel level);
    
    // 获取当前日志级别
    static LogLevel GetLevel();
    
    // 日志输出方法
    static void Debug(const String& message);
    static void Info(const String& message);
    static void Warning(const String& message);
    static void Error(const String& message);
    static void Fatal(const String& message);
    
    // 格式化日志输出
    template<typename... Args>
    static void DebugF(const String& format, Args... args) {
        Debug(FormatString(format, args...));
    }
    
    template<typename... Args>
    static void InfoF(const String& format, Args... args) {
        Info(FormatString(format, args...));
    }
    
    template<typename... Args>
    static void WarningF(const String& format, Args... args) {
        Warning(FormatString(format, args...));
    }
    
    template<typename... Args>
    static void ErrorF(const String& format, Args... args) {
        Error(FormatString(format, args...));
    }
    
    template<typename... Args>
    static void FatalF(const String& format, Args... args) {
        Fatal(FormatString(format, args...));
    }
    
private:
    // 格式化字符串辅助函数
    template<typename... Args>
    static String FormatString(const String& format, Args... args) {
        size_t size = std::snprintf(nullptr, 0, format.c_str(), args...) + 1;
        if (size <= 0) return format;
        
        std::unique_ptr<char[]> buf(new char[size]);
        std::snprintf(buf.get(), size, format.c_str(), args...);
        return String(buf.get(), buf.get() + size - 1);
    }
};

// 作用域日志计时器
class ScopedLogTimer {
private:
    String name_;
    std::chrono::high_resolution_clock::time_point start_;
    
public:
    explicit ScopedLogTimer(const String& name);
    ~ScopedLogTimer();
};

// 便捷宏定义
#define LOG_DEBUG(msg) CHTL::Logger::Debug(msg)
#define LOG_INFO(msg) CHTL::Logger::Info(msg)
#define LOG_WARNING(msg) CHTL::Logger::Warning(msg)
#define LOG_ERROR(msg) CHTL::Logger::Error(msg)
#define LOG_FATAL(msg) CHTL::Logger::Fatal(msg)

#define LOG_SCOPE_TIMER(name) CHTL::ScopedLogTimer _timer(name)

} // namespace CHTL

#endif // CHTL_CORE_LOGGER_H