#pragma once

#include <string>
#include <vector>
#include <memory>
#include "Location.h"

namespace CHTL {

// 错误级别
enum class ErrorLevel {
    INFO,
    WARNING,
    ERROR,
    FATAL
};

// 错误信息
class Error {
public:
    Error(ErrorLevel level, const std::string& message, const Location& location)
        : level_(level), message_(message), location_(location) {}
    
    ErrorLevel GetLevel() const { return level_; }
    const std::string& GetMessage() const { return message_; }
    const Location& GetLocation() const { return location_; }
    
    std::string ToString() const;
    
private:
    ErrorLevel level_;
    std::string message_;
    Location location_;
};

using ErrorPtr = std::shared_ptr<Error>;

// 错误收集器
class ErrorCollector {
public:
    void AddError(ErrorLevel level, const std::string& message, const Location& location) {
        errors_.push_back(std::make_shared<Error>(level, message, location));
    }
    
    void AddInfo(const std::string& message, const Location& location) {
        AddError(ErrorLevel::INFO, message, location);
    }
    
    void AddWarning(const std::string& message, const Location& location) {
        AddError(ErrorLevel::WARNING, message, location);
    }
    
    void AddError(const std::string& message, const Location& location) {
        AddError(ErrorLevel::ERROR, message, location);
    }
    
    void AddFatal(const std::string& message, const Location& location) {
        AddError(ErrorLevel::FATAL, message, location);
    }
    
    bool HasErrors() const {
        for (const auto& error : errors_) {
            if (error->GetLevel() >= ErrorLevel::ERROR) {
                return true;
            }
        }
        return false;
    }
    
    bool HasFatalErrors() const {
        for (const auto& error : errors_) {
            if (error->GetLevel() == ErrorLevel::FATAL) {
                return true;
            }
        }
        return false;
    }
    
    const std::vector<ErrorPtr>& GetErrors() const { return errors_; }
    
    void Clear() { errors_.clear(); }
    
    void PrintErrors() const;
    
private:
    std::vector<ErrorPtr> errors_;
};

} // namespace CHTL