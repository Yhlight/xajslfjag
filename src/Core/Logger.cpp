#include "Core/Logger.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <ctime>

namespace CHTL {

// 静态成员初始化
LogLevel Logger::currentLevel_ = LogLevel::Info;
std::mutex Logger::mutex_;
bool Logger::initialized_ = false;
String Logger::logFile_;
bool Logger::enableColor_ = true;

void Logger::Initialize(LogLevel level, const String& logFile, bool enableColor) {
    std::lock_guard<std::mutex> lock(mutex_);
    currentLevel_ = level;
    logFile_ = logFile;
    enableColor_ = enableColor;
    initialized_ = true;
}

void Logger::SetLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(mutex_);
    currentLevel_ = level;
}

LogLevel Logger::GetLevel() {
    std::lock_guard<std::mutex> lock(mutex_);
    return currentLevel_;
}

String Logger::GetLevelString(LogLevel level) {
    switch (level) {
        case LogLevel::Debug:   return "DEBUG";
        case LogLevel::Info:    return "INFO ";
        case LogLevel::Warning: return "WARN ";
        case LogLevel::Error:   return "ERROR";
        case LogLevel::Fatal:   return "FATAL";
        default:                return "UNKNOWN";
    }
}

String Logger::GetLevelColor(LogLevel level) {
    if (!enableColor_) return "";
    
    switch (level) {
        case LogLevel::Debug:   return "\033[36m";  // Cyan
        case LogLevel::Info:    return "\033[32m";  // Green
        case LogLevel::Warning: return "\033[33m";  // Yellow
        case LogLevel::Error:   return "\033[31m";  // Red
        case LogLevel::Fatal:   return "\033[35m";  // Magenta
        default:                return "";
    }
}

String Logger::GetTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    ss << '.' << std::setfill('0') << std::setw(3) << ms.count();
    return ss.str();
}

void Logger::Output(LogLevel level, const String& message) {
    if (!initialized_ || level < currentLevel_) return;
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    String timestamp = GetTimestamp();
    String levelStr = GetLevelString(level);
    String color = GetLevelColor(level);
    String reset = enableColor_ ? "\033[0m" : "";
    
    String formattedMessage = "[" + timestamp + "] " + color + levelStr + reset + " " + message;
    
    // 输出到控制台
    std::cout << formattedMessage << std::endl;
    
    // 输出到文件（如果指定）
    if (!logFile_.empty()) {
        std::ofstream file(logFile_, std::ios::app);
        if (file.is_open()) {
            file << formattedMessage << std::endl;
        }
    }
    
    // Fatal级别退出程序
    if (level == LogLevel::Fatal) {
        std::exit(1);
    }
}

void Logger::Debug(const String& message) {
    Output(LogLevel::Debug, message);
}

void Logger::Info(const String& message) {
    Output(LogLevel::Info, message);
}

void Logger::Warning(const String& message) {
    Output(LogLevel::Warning, message);
}

void Logger::Error(const String& message) {
    Output(LogLevel::Error, message);
}

void Logger::Fatal(const String& message) {
    Output(LogLevel::Fatal, message);
}

// ScopedLogTimer 实现
ScopedLogTimer::ScopedLogTimer(const String& name) 
    : name_(name), start_(std::chrono::high_resolution_clock::now()) {
    Logger::Debug("开始计时: " + name_);
}

ScopedLogTimer::~ScopedLogTimer() {
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start_);
    Logger::Debug(name_ + " 耗时: " + std::to_string(duration.count()) + "ms");
}

} // namespace CHTL