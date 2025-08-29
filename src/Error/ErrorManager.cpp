#include "ErrorManager.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <algorithm>

namespace CHTL::Error {

// ErrorInfo实现
ErrorInfo::ErrorInfo(ErrorCode c, ErrorLevel l, const String& msg, 
                    const String& f, int ln, const String& func)
    : code(c), level(l), message(msg), file(f), line(ln), function(func) {
    timestamp = std::time(nullptr);
}

String ErrorInfo::toString() const {
    std::ostringstream oss;
    
    // 时间戳
    std::tm* timeinfo = std::localtime(&timestamp);
    oss << "[" << std::put_time(timeinfo, "%Y-%m-%d %H:%M:%S") << "] ";
    
    // 错误级别
    switch (level) {
        case ErrorLevel::DEBUG: oss << "[DEBUG] "; break;
        case ErrorLevel::INFO: oss << "[INFO] "; break;
        case ErrorLevel::WARNING: oss << "[WARNING] "; break;
        case ErrorLevel::ERROR: oss << "[ERROR] "; break;
        case ErrorLevel::CRITICAL: oss << "[CRITICAL] "; break;
        case ErrorLevel::FATAL: oss << "[FATAL] "; break;
    }
    
    // 错误代码
    oss << "(" << static_cast<int>(code) << ") ";
    
    // 消息
    oss << message;
    
    // 位置信息
    if (!file.empty() && line > 0) {
        oss << " [" << file << ":" << line;
        if (!function.empty()) {
            oss << " in " << function << "()";
        }
        oss << "]";
    }
    
    // 上下文信息
    if (!context.empty()) {
        oss << " {";
        bool first = true;
        for (const auto& [key, value] : context) {
            if (!first) oss << ", ";
            oss << key << ": " << value;
            first = false;
        }
        oss << "}";
    }
    
    return oss.str();
}

String ErrorInfo::toJSON() const {
    std::ostringstream oss;
    oss << "{\n";
    oss << "  \"code\": " << static_cast<int>(code) << ",\n";
    oss << "  \"level\": \"";
    
    switch (level) {
        case ErrorLevel::DEBUG: oss << "DEBUG"; break;
        case ErrorLevel::INFO: oss << "INFO"; break;
        case ErrorLevel::WARNING: oss << "WARNING"; break;
        case ErrorLevel::ERROR: oss << "ERROR"; break;
        case ErrorLevel::CRITICAL: oss << "CRITICAL"; break;
        case ErrorLevel::FATAL: oss << "FATAL"; break;
    }
    
    oss << "\",\n";
    oss << "  \"message\": \"" << message << "\",\n";
    oss << "  \"file\": \"" << file << "\",\n";
    oss << "  \"line\": " << line << ",\n";
    oss << "  \"function\": \"" << function << "\",\n";
    oss << "  \"timestamp\": " << timestamp;
    
    if (!context.empty()) {
        oss << ",\n  \"context\": {\n";
        bool first = true;
        for (const auto& [key, value] : context) {
            if (!first) oss << ",\n";
            oss << "    \"" << key << "\": \"" << value << "\"";
            first = false;
        }
        oss << "\n  }";
    }
    
    oss << "\n}";
    return oss.str();
}

// CHTLException实现
CHTLException::CHTLException(const ErrorInfo& errorInfo)
    : errorInfo_(errorInfo) {
}

CHTLException::CHTLException(ErrorCode code, const String& message, 
                           const String& file, int line, const String& function)
    : errorInfo_(code, ErrorLevel::ERROR, message, file, line, function) {
}

const char* CHTLException::what() const noexcept {
    if (whatMessage_.empty()) {
        whatMessage_ = errorInfo_.toString();
    }
    return whatMessage_.c_str();
}

const ErrorInfo& CHTLException::getErrorInfo() const noexcept {
    return errorInfo_;
}

ErrorCode CHTLException::getErrorCode() const noexcept {
    return errorInfo_.code;
}

ErrorLevel CHTLException::getErrorLevel() const noexcept {
    return errorInfo_.level;
}

// 特定异常类实现
LexerException::LexerException(ErrorCode code, const String& message, 
                              const String& token, int position)
    : CHTLException(code, message) {
    if (!token.empty()) {
        errorInfo_.context["token"] = token;
    }
    if (position >= 0) {
        errorInfo_.context["position"] = std::to_string(position);
    }
}

ParserException::ParserException(ErrorCode code, const String& message, 
                                const String& expectedToken, const String& actualToken)
    : CHTLException(code, message) {
    if (!expectedToken.empty()) {
        errorInfo_.context["expected"] = expectedToken;
    }
    if (!actualToken.empty()) {
        errorInfo_.context["actual"] = actualToken;
    }
}

CompilerException::CompilerException(ErrorCode code, const String& message, 
                                   const String& stage)
    : CHTLException(code, message) {
    if (!stage.empty()) {
        errorInfo_.context["stage"] = stage;
    }
}

IOStreamException::IOStreamException(ErrorCode code, const String& message, 
                                   const String& filePath)
    : CHTLException(code, message) {
    if (!filePath.empty()) {
        errorInfo_.context["filePath"] = filePath;
    }
}

// ErrorManager实现
ErrorManager::ErrorManager()
    : errorRecoveryEnabled_(true)
    , consoleOutputEnabled_(true)
    , fileOutputEnabled_(false)
    , maxLogSize_(10 * 1024 * 1024) // 10MB
    , currentLanguage_("en") {
    initializeErrorMessages();
}

ErrorManager& ErrorManager::getInstance() {
    static ErrorManager instance;
    return instance;
}

void ErrorManager::reportError(const ErrorInfo& errorInfo) {
    errors_.push_back(errorInfo);
    
    // 限制错误数量，防止内存溢出
    if (errors_.size() > 10000) {
        errors_.erase(errors_.begin(), errors_.begin() + 1000);
    }
    
    // 输出错误
    if (consoleOutputEnabled_) {
        writeToConsole(errorInfo);
    }
    
    if (fileOutputEnabled_) {
        writeToFile(errorInfo);
    }
    
    // 调用回调函数
    if (globalCallback_) {
        globalCallback_(errorInfo);
    }
    
    auto levelCallbackIt = levelCallbacks_.find(errorInfo.level);
    if (levelCallbackIt != levelCallbacks_.end()) {
        levelCallbackIt->second(errorInfo);
    }
    
    // 尝试错误恢复
    if (errorRecoveryEnabled_ && errorInfo.level >= ErrorLevel::ERROR) {
        attemptErrorRecovery(errorInfo.code);
    }
}

void ErrorManager::reportError(ErrorCode code, ErrorLevel level, const String& message,
                              const String& file, int line, const String& function) {
    ErrorInfo errorInfo(code, level, message, file, line, function);
    reportError(errorInfo);
}

void ErrorManager::reportDebug(const String& message, const String& context) {
    reportError(ErrorCode::UNKNOWN_ERROR, ErrorLevel::DEBUG, message, "", 0, context);
}

void ErrorManager::reportInfo(const String& message, const String& context) {
    reportError(ErrorCode::UNKNOWN_ERROR, ErrorLevel::INFO, message, "", 0, context);
}

void ErrorManager::reportWarning(const String& message, const String& context) {
    reportError(ErrorCode::UNKNOWN_ERROR, ErrorLevel::WARNING, message, "", 0, context);
}

void ErrorManager::reportError(const String& message, const String& context) {
    reportError(ErrorCode::UNKNOWN_ERROR, ErrorLevel::ERROR, message, "", 0, context);
}

void ErrorManager::reportCritical(const String& message, const String& context) {
    reportError(ErrorCode::UNKNOWN_ERROR, ErrorLevel::CRITICAL, message, "", 0, context);
}

void ErrorManager::reportFatal(const String& message, const String& context) {
    reportError(ErrorCode::UNKNOWN_ERROR, ErrorLevel::FATAL, message, "", 0, context);
}

std::vector<ErrorInfo> ErrorManager::getErrors(ErrorLevel minLevel) const {
    std::vector<ErrorInfo> result;
    std::copy_if(errors_.begin(), errors_.end(), std::back_inserter(result),
                [minLevel](const ErrorInfo& error) {
                    return error.level >= minLevel;
                });
    return result;
}

std::vector<ErrorInfo> ErrorManager::getErrorsByCode(ErrorCode code) const {
    std::vector<ErrorInfo> result;
    std::copy_if(errors_.begin(), errors_.end(), std::back_inserter(result),
                [code](const ErrorInfo& error) {
                    return error.code == code;
                });
    return result;
}

std::vector<ErrorInfo> ErrorManager::getRecentErrors(int count) const {
    std::vector<ErrorInfo> result;
    int startIndex = std::max(0, static_cast<int>(errors_.size()) - count);
    
    for (int i = startIndex; i < static_cast<int>(errors_.size()); ++i) {
        result.push_back(errors_[i]);
    }
    
    return result;
}

size_t ErrorManager::getErrorCount(ErrorLevel level) const {
    return std::count_if(errors_.begin(), errors_.end(),
                        [level](const ErrorInfo& error) {
                            return error.level == level;
                        });
}

bool ErrorManager::hasErrors(ErrorLevel minLevel) const {
    return std::any_of(errors_.begin(), errors_.end(),
                      [minLevel](const ErrorInfo& error) {
                          return error.level >= minLevel;
                      });
}

void ErrorManager::clearErrors() {
    errors_.clear();
}

void ErrorManager::clearErrors(ErrorLevel level) {
    errors_.erase(std::remove_if(errors_.begin(), errors_.end(),
                                [level](const ErrorInfo& error) {
                                    return error.level == level;
                                }), errors_.end());
}

void ErrorManager::clearErrorsByCode(ErrorCode code) {
    errors_.erase(std::remove_if(errors_.begin(), errors_.end(),
                                [code](const ErrorInfo& error) {
                                    return error.code == code;
                                }), errors_.end());
}

void ErrorManager::setErrorCallback(ErrorCallback callback) {
    globalCallback_ = callback;
}

void ErrorManager::addErrorListener(ErrorLevel level, ErrorCallback callback) {
    levelCallbacks_[level] = callback;
}

void ErrorManager::removeErrorListener(ErrorLevel level) {
    levelCallbacks_.erase(level);
}

void ErrorManager::enableErrorRecovery(bool enable) {
    errorRecoveryEnabled_ = enable;
}

bool ErrorManager::isErrorRecoveryEnabled() const {
    return errorRecoveryEnabled_;
}

void ErrorManager::addErrorRecoveryStrategy(ErrorCode code, std::function<bool()> strategy) {
    recoveryStrategies_[code] = strategy;
}

void ErrorManager::enableConsoleOutput(bool enable) {
    consoleOutputEnabled_ = enable;
}

void ErrorManager::enableFileOutput(const String& filePath) {
    fileOutputEnabled_ = true;
    logFilePath_ = filePath;
}

void ErrorManager::disableFileOutput() {
    fileOutputEnabled_ = false;
    logFilePath_.clear();
}

void ErrorManager::setMaxLogSize(size_t maxSize) {
    maxLogSize_ = maxSize;
}

ErrorManager::ErrorStatistics ErrorManager::getStatistics() const {
    ErrorStatistics stats;
    stats.totalErrors = errors_.size();
    
    if (!errors_.empty()) {
        stats.firstError = errors_.front().timestamp;
        stats.lastError = errors_.back().timestamp;
        
        for (const auto& error : errors_) {
            stats.countByLevel[error.level]++;
            stats.countByCode[error.code]++;
        }
    }
    
    return stats;
}

void ErrorManager::resetStatistics() {
    // 统计信息通过errors_维护，清空errors_即可重置统计
}

void ErrorManager::setLanguage(const String& language) {
    currentLanguage_ = language;
}

String ErrorManager::getLocalizedMessage(ErrorCode code) const {
    auto codeIt = errorMessages_.find(code);
    if (codeIt != errorMessages_.end()) {
        auto langIt = codeIt->second.find(currentLanguage_);
        if (langIt != codeIt->second.end()) {
            return langIt->second;
        }
        
        // 回退到英语
        auto enIt = codeIt->second.find("en");
        if (enIt != codeIt->second.end()) {
            return enIt->second;
        }
    }
    
    return "Unknown error";
}

void ErrorManager::initializeErrorMessages() {
    // 初始化错误消息模板
    errorMessages_[ErrorCode::FILE_NOT_FOUND]["en"] = "File not found";
    errorMessages_[ErrorCode::FILE_NOT_FOUND]["zh"] = "文件未找到";
    
    errorMessages_[ErrorCode::PARSER_SYNTAX_ERROR]["en"] = "Syntax error";
    errorMessages_[ErrorCode::PARSER_SYNTAX_ERROR]["zh"] = "语法错误";
    
    errorMessages_[ErrorCode::CHTL_INVALID_TEMPLATE]["en"] = "Invalid CHTL template";
    errorMessages_[ErrorCode::CHTL_INVALID_TEMPLATE]["zh"] = "无效的CHTL模板";
    
    errorMessages_[ErrorCode::CHTLJS_MODULE_NOT_FOUND]["en"] = "CHTL JS module not found";
    errorMessages_[ErrorCode::CHTLJS_MODULE_NOT_FOUND]["zh"] = "CHTL JS模块未找到";
    
    // 可以继续添加更多错误消息...
}

void ErrorManager::writeToFile(const ErrorInfo& errorInfo) {
    if (logFilePath_.empty()) return;
    
    std::ofstream logFile(logFilePath_, std::ios::app);
    if (logFile.is_open()) {
        logFile << errorInfo.toString() << std::endl;
        logFile.close();
    }
}

void ErrorManager::writeToConsole(const ErrorInfo& errorInfo) {
    std::ostream* output = &std::cout;
    
    // 错误和更严重的级别输出到stderr
    if (errorInfo.level >= ErrorLevel::ERROR) {
        output = &std::cerr;
    }
    
    *output << formatMessage(errorInfo) << std::endl;
}

String ErrorManager::formatMessage(const ErrorInfo& errorInfo) const {
    // 可以在这里添加颜色代码等格式化
    return errorInfo.toString();
}

bool ErrorManager::attemptErrorRecovery(ErrorCode code) {
    auto it = recoveryStrategies_.find(code);
    if (it != recoveryStrategies_.end()) {
        try {
            return it->second();
        } catch (...) {
            reportError(ErrorCode::COMPILER_INTERNAL_ERROR, ErrorLevel::ERROR,
                       "Error recovery strategy failed");
            return false;
        }
    }
    return false;
}

// ErrorScope实现
ErrorScope::ErrorScope(const String& scopeName) 
    : scopeName_(scopeName) {
    initialErrorCount_ = ErrorManager::getInstance().getErrors().size();
}

ErrorScope::~ErrorScope() {
    // 可以在这里添加清理逻辑
}

void ErrorScope::addContext(const String& key, const String& value) {
    context_[key] = value;
}

bool ErrorScope::hasErrors() const {
    return ErrorManager::getInstance().getErrors().size() > initialErrorCount_;
}

std::vector<ErrorInfo> ErrorScope::getScopeErrors() const {
    auto allErrors = ErrorManager::getInstance().getErrors();
    std::vector<ErrorInfo> scopeErrors;
    
    if (allErrors.size() > initialErrorCount_) {
        scopeErrors.assign(allErrors.begin() + initialErrorCount_, allErrors.end());
    }
    
    return scopeErrors;
}

} // namespace CHTL::Error