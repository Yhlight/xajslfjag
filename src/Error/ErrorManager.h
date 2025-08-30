#pragma once

#include "../Util/Common.h"
#include <exception>
#include <memory>
#include <vector>
#include <map>
#include <functional>

namespace CHTL::Error {

// 错误级别枚举
enum class ErrorLevel {
    DEBUG_LEVEL,  // 避免与系统DEBUG宏冲突
    INFO,
    WARNING,
    ERROR,
    CRITICAL,
    FATAL
};

// 错误代码枚举
enum class ErrorCode {
    // 通用错误 1000-1999
    UNKNOWN_ERROR = 1000,
    INVALID_PARAMETER = 1001,
    NULL_POINTER = 1002,
    OUT_OF_MEMORY = 1003,
    
    // 文件系统错误 2000-2999
    FILE_NOT_FOUND = 2000,
    FILE_ACCESS_DENIED = 2001,
    FILE_READ_ERROR = 2002,
    FILE_WRITE_ERROR = 2003,
    DIRECTORY_NOT_FOUND = 2004,
    INVALID_PATH = 2005,
    
    // 词法分析错误 3000-3999
    LEXER_INVALID_TOKEN = 3000,
    LEXER_UNEXPECTED_CHARACTER = 3001,
    LEXER_UNTERMINATED_STRING = 3002,
    LEXER_INVALID_NUMBER = 3003,
    
    // 语法分析错误 4000-4999
    PARSER_SYNTAX_ERROR = 4000,
    PARSER_UNEXPECTED_TOKEN = 4001,
    PARSER_MISSING_SEMICOLON = 4002,
    PARSER_UNMATCHED_BRACE = 4003,
    PARSER_INVALID_EXPRESSION = 4004,
    
    // CHTL特定错误 5000-5999
    CHTL_INVALID_TEMPLATE = 5000,
    CHTL_CIRCULAR_INHERITANCE = 5001,
    CHTL_UNDEFINED_CUSTOM = 5002,
    CHTL_INVALID_SELECTOR = 5003,
    CHTL_CONSTRAINT_VIOLATION = 5004,
    
    // CHTL JS特定错误 6000-6999
    CHTLJS_INVALID_SYNTAX = 6000,
    CHTLJS_MODULE_NOT_FOUND = 6001,
    CHTLJS_CIRCULAR_DEPENDENCY = 6002,
    CHTLJS_CJMOD_ERROR = 6003,
    
    // 编译器错误 7000-7999
    COMPILER_INTERNAL_ERROR = 7000,
    COMPILER_GENERATION_FAILED = 7001,
    COMPILER_OPTIMIZATION_FAILED = 7002,
    
    // IO流错误 8000-8999
    IO_STREAM_ERROR = 8000,
    IO_BUFFER_OVERFLOW = 8001,
    IO_ASYNC_ERROR = 8002,
    IO_MAPPING_FAILED = 8003
};

// 错误信息结构体
struct ErrorInfo {
    ErrorCode code;
    ErrorLevel level;
    String message;
    String file;
    int line;
    String function;
    std::time_t timestamp;
    StringUnorderedMap context;
    
    ErrorInfo(ErrorCode c, ErrorLevel l, const String& msg, 
              const String& f = "", int ln = 0, const String& func = "");
    
    String toString() const;
    String toJSON() const;
};

// CHTL异常基类
class CHTLException : public std::exception {
public:
    explicit CHTLException(const ErrorInfo& errorInfo);
    explicit CHTLException(ErrorCode code, const String& message, 
                          const String& file = "", int line = 0, 
                          const String& function = "");
    
    const char* what() const noexcept override;
    const ErrorInfo& getErrorInfo() const noexcept;
    ErrorCode getErrorCode() const noexcept;
    ErrorLevel getErrorLevel() const noexcept;
    
protected:
    ErrorInfo errorInfo_;
    mutable String whatMessage_;
};

// 特定类型的异常类
class LexerException : public CHTLException {
public:
    LexerException(ErrorCode code, const String& message, 
                   const String& token = "", int position = -1);
};

class ParserException : public CHTLException {
public:
    ParserException(ErrorCode code, const String& message, 
                    const String& expectedToken = "", const String& actualToken = "");
};

class CompilerException : public CHTLException {
public:
    CompilerException(ErrorCode code, const String& message, 
                     const String& stage = "");
};

class IOStreamException : public CHTLException {
public:
    IOStreamException(ErrorCode code, const String& message, 
                     const String& filePath = "");
};

// 错误管理器
class ErrorManager {
public:
    static ErrorManager& getInstance();
    
    // 错误报告
    void reportError(const ErrorInfo& errorInfo);
    void reportError(ErrorCode code, ErrorLevel level, const String& message,
                    const String& file = "", int line = 0, const String& function = "");
    
    // 便捷方法
    void reportDebug(const String& message, const String& context = "");
    void reportInfo(const String& message, const String& context = "");
    void reportWarning(const String& message, const String& context = "");
    void reportError(const String& message, const String& context = "");
    void reportCritical(const String& message, const String& context = "");
    void reportFatal(const String& message, const String& context = "");
    
    // 错误查询
    std::vector<ErrorInfo> getErrors(ErrorLevel minLevel = ErrorLevel::WARNING) const;
    std::vector<ErrorInfo> getErrorsByCode(ErrorCode code) const;
    std::vector<ErrorInfo> getRecentErrors(int count = 10) const;
    size_t getErrorCount(ErrorLevel level) const;
    bool hasErrors(ErrorLevel minLevel = ErrorLevel::ERROR) const;
    
    // 错误处理
    void clearErrors();
    void clearErrors(ErrorLevel level);
    void clearErrorsByCode(ErrorCode code);
    
    // 错误回调
    using ErrorCallback = std::function<void(const ErrorInfo&)>;
    void setErrorCallback(ErrorCallback callback);
    void addErrorListener(ErrorLevel level, ErrorCallback callback);
    void removeErrorListener(ErrorLevel level);
    
    // 错误恢复
    void enableErrorRecovery(bool enable = true);
    bool isErrorRecoveryEnabled() const;
    void addErrorRecoveryStrategy(ErrorCode code, std::function<bool()> strategy);
    
    // 错误输出
    void enableConsoleOutput(bool enable = true);
    void enableFileOutput(const String& filePath);
    void disableFileOutput();
    void setMaxLogSize(size_t maxSize);
    
    // 错误统计
    struct ErrorStatistics {
        std::map<ErrorLevel, size_t> countByLevel;
        std::map<ErrorCode, size_t> countByCode;
        size_t totalErrors;
        std::time_t firstError;
        std::time_t lastError;
    };
    
    ErrorStatistics getStatistics() const;
    void resetStatistics();
    
    // 多语言支持
    void setLanguage(const String& language);
    String getLocalizedMessage(ErrorCode code) const;
    
private:
    ErrorManager();
    ~ErrorManager() = default;
    
    // 禁用拷贝构造和赋值
    ErrorManager(const ErrorManager&) = delete;
    ErrorManager& operator=(const ErrorManager&) = delete;
    
    std::vector<ErrorInfo> errors_;
    ErrorCallback globalCallback_;
    std::map<ErrorLevel, ErrorCallback> levelCallbacks_;
    std::map<ErrorCode, std::function<bool()>> recoveryStrategies_;
    
    bool errorRecoveryEnabled_;
    bool consoleOutputEnabled_;
    bool fileOutputEnabled_;
    String logFilePath_;
    size_t maxLogSize_;
    String currentLanguage_;
    
    // 错误消息模板
    std::map<ErrorCode, std::map<String, String>> errorMessages_;
    
    void initializeErrorMessages();
    void writeToFile(const ErrorInfo& errorInfo);
    void writeToConsole(const ErrorInfo& errorInfo);
    String formatMessage(const ErrorInfo& errorInfo) const;
    bool attemptErrorRecovery(ErrorCode code);
};

// 错误处理宏
#define CHTL_ERROR(code, message) \
    CHTL::Error::ErrorManager::getInstance().reportError( \
        code, CHTL::Error::ErrorLevel::ERROR, message, __FILE__, __LINE__, __FUNCTION__)

#define CHTL_WARNING(message) \
    CHTL::Error::ErrorManager::getInstance().reportWarning(message, __FUNCTION__)

#define CHTL_INFO(message) \
    CHTL::Error::ErrorManager::getInstance().reportInfo(message, __FUNCTION__)

#define CHTL_DEBUG(message) \
    CHTL::Error::ErrorManager::getInstance().reportDebug(message, __FUNCTION__)

#define CHTL_FATAL(code, message) \
    do { \
        CHTL::Error::ErrorManager::getInstance().reportFatal(message, __FUNCTION__); \
        throw CHTL::Error::CHTLException(code, message, __FILE__, __LINE__, __FUNCTION__); \
    } while(0)

#define CHTL_THROW(exceptionType, code, message) \
    throw exceptionType(code, message, __FILE__, __LINE__)

// 错误范围守卫
class ErrorScope {
public:
    explicit ErrorScope(const String& scopeName);
    ~ErrorScope();
    
    void addContext(const String& key, const String& value);
    bool hasErrors() const;
    std::vector<ErrorInfo> getScopeErrors() const;
    
private:
    String scopeName_;
    size_t initialErrorCount_;
    StringUnorderedMap context_;
};

#define CHTL_ERROR_SCOPE(name) CHTL::Error::ErrorScope _errorScope(name)

} // namespace CHTL::Error