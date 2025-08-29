#ifndef ERROR_REPORT_H
#define ERROR_REPORT_H

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <chrono>
#include <sstream>
#include <unordered_map>

namespace CHTL {

// 错误级别
enum class ErrorLevel {
    DEBUG,      // 调试信息
    INFO,       // 信息
    WARNING,    // 警告
    ERROR,      // 错误
    FATAL       // 致命错误
};

// 错误类型
enum class ErrorType {
    SYNTAX_ERROR,         // 语法错误
    LEXICAL_ERROR,       // 词法错误
    SEMANTIC_ERROR,      // 语义错误
    TYPE_ERROR,          // 类型错误
    REFERENCE_ERROR,     // 引用错误
    IMPORT_ERROR,        // 导入错误
    NAMESPACE_ERROR,     // 命名空间错误
    CONSTRAINT_ERROR,    // 约束错误
    RUNTIME_ERROR,       // 运行时错误
    IO_ERROR,            // IO错误
    MEMORY_ERROR,        // 内存错误
    TIMEOUT_ERROR,       // 超时错误
    INTERNAL_ERROR       // 内部错误
};

// 错误位置信息
struct ErrorLocation {
    std::string filename;
    size_t line = 0;
    size_t column = 0;
    size_t offset = 0;
    size_t length = 0;
    
    std::string toString() const;
};

// 错误上下文
struct ErrorContext {
    std::string phase;              // 编译阶段（lexing, parsing, generating等）
    std::string component;          // 组件名称
    std::string function;           // 函数名称
    std::unordered_map<std::string, std::string> metadata;  // 额外元数据
};

// 错误信息
struct ErrorInfo {
    ErrorLevel level;
    ErrorType type;
    std::string code;               // 错误代码（如E0001）
    std::string message;            // 错误消息
    std::string detail;             // 详细说明
    ErrorLocation location;         // 错误位置
    ErrorContext context;           // 错误上下文
    std::chrono::system_clock::time_point timestamp;  // 时间戳
    std::vector<std::string> suggestions;  // 修复建议
    std::string stackTrace;         // 堆栈跟踪
};

// 错误报告器接口
class IErrorReporter {
public:
    virtual ~IErrorReporter() = default;
    virtual void report(const ErrorInfo& error) = 0;
    virtual void flush() = 0;
};

// 控制台错误报告器
class ConsoleErrorReporter : public IErrorReporter {
public:
    ConsoleErrorReporter(bool useColors = true);
    void report(const ErrorInfo& error) override;
    void flush() override;
    
    void setVerbose(bool verbose) { verbose_ = verbose; }
    void setShowStackTrace(bool show) { showStackTrace_ = show; }
    
private:
    bool useColors_;
    bool verbose_ = false;
    bool showStackTrace_ = false;
    
    std::string formatError(const ErrorInfo& error);
    std::string colorize(const std::string& text, const std::string& color);
    std::string getColorForLevel(ErrorLevel level);
};

// 文件错误报告器
class FileErrorReporter : public IErrorReporter {
public:
    explicit FileErrorReporter(const std::string& filename);
    ~FileErrorReporter();
    
    void report(const ErrorInfo& error) override;
    void flush() override;
    
private:
    std::string filename_;
    std::unique_ptr<std::ofstream> file_;
};

// JSON错误报告器
class JSONErrorReporter : public IErrorReporter {
public:
    JSONErrorReporter();
    void report(const ErrorInfo& error) override;
    void flush() override;
    
    std::string getJSON() const { return buffer_.str(); }
    
private:
    std::stringstream buffer_;
    bool firstError_ = true;
};

// 错误收集器
class ErrorCollector : public IErrorReporter {
public:
    void report(const ErrorInfo& error) override;
    void flush() override {}
    
    const std::vector<ErrorInfo>& getErrors() const { return errors_; }
    std::vector<ErrorInfo> getErrorsByLevel(ErrorLevel level) const;
    std::vector<ErrorInfo> getErrorsByType(ErrorType type) const;
    
    size_t getErrorCount() const;
    size_t getWarningCount() const;
    bool hasErrors() const { return getErrorCount() > 0; }
    bool hasFatalErrors() const;
    
    void clear() { errors_.clear(); }
    
private:
    std::vector<ErrorInfo> errors_;
};

// 错误报告管理器（单例）
class ErrorReport {
public:
    static ErrorReport& getInstance() {
        static ErrorReport instance;
        return instance;
    }
    
    // 添加错误报告器
    void addReporter(std::shared_ptr<IErrorReporter> reporter);
    void removeAllReporters();
    
    // 报告错误
    void report(ErrorLevel level, ErrorType type, const std::string& message);
    void report(const ErrorInfo& error);
    
    // 便捷方法
    void debug(const std::string& message);
    void info(const std::string& message);
    void warning(const std::string& message);
    void error(const std::string& message);
    void fatal(const std::string& message);
    
    // 语法错误
    void syntaxError(const std::string& message, const ErrorLocation& location);
    void lexicalError(const std::string& message, const ErrorLocation& location);
    void semanticError(const std::string& message, const ErrorLocation& location);
    
    // 设置当前上下文
    void setContext(const ErrorContext& context) { currentContext_ = context; }
    void pushContext(const std::string& phase, const std::string& component);
    void popContext();
    
    // 错误代码管理
    void registerErrorCode(const std::string& code, const std::string& description);
    std::string getErrorDescription(const std::string& code) const;
    
    // 统计信息
    size_t getTotalErrors() const { return totalErrors_; }
    size_t getTotalWarnings() const { return totalWarnings_; }
    void resetCounters();
    
    // 刷新所有报告器
    void flush();
    
    // 配置
    void setMaxErrors(size_t max) { maxErrors_ = max; }
    void setSuppressWarnings(bool suppress) { suppressWarnings_ = suppress; }
    void setThrowOnFatal(bool throwOnFatal) { throwOnFatal_ = throwOnFatal; }
    
private:
    ErrorReport() = default;
    ErrorReport(const ErrorReport&) = delete;
    ErrorReport& operator=(const ErrorReport&) = delete;
    
    std::vector<std::shared_ptr<IErrorReporter>> reporters_;
    ErrorContext currentContext_;
    std::vector<ErrorContext> contextStack_;
    std::unordered_map<std::string, std::string> errorCodes_;
    
    size_t totalErrors_ = 0;
    size_t totalWarnings_ = 0;
    size_t maxErrors_ = 100;
    bool suppressWarnings_ = false;
    bool throwOnFatal_ = true;
    
    // 生成错误代码
    std::string generateErrorCode(ErrorType type);
    
    // 创建错误信息
    ErrorInfo createErrorInfo(ErrorLevel level, ErrorType type, 
                             const std::string& message,
                             const ErrorLocation& location = {});
};

// 错误构建器（流式API）
class ErrorBuilder {
public:
    ErrorBuilder(ErrorLevel level, ErrorType type);
    
    ErrorBuilder& withMessage(const std::string& message);
    ErrorBuilder& withDetail(const std::string& detail);
    ErrorBuilder& withCode(const std::string& code);
    ErrorBuilder& atLocation(const ErrorLocation& location);
    ErrorBuilder& atLocation(const std::string& file, size_t line, size_t col);
    ErrorBuilder& inContext(const std::string& phase, const std::string& component);
    ErrorBuilder& withMetadata(const std::string& key, const std::string& value);
    ErrorBuilder& withSuggestion(const std::string& suggestion);
    ErrorBuilder& withStackTrace(const std::string& trace);
    
    void report();
    ErrorInfo build() const;
    
private:
    ErrorInfo error_;
};

// 错误处理宏
#define CHTL_ERROR(msg) \
    CHTL::ErrorReport::getInstance().error(msg)

#define CHTL_WARNING(msg) \
    CHTL::ErrorReport::getInstance().warning(msg)

#define CHTL_FATAL(msg) \
    CHTL::ErrorReport::getInstance().fatal(msg)

#define CHTL_SYNTAX_ERROR(msg, loc) \
    CHTL::ErrorReport::getInstance().syntaxError(msg, loc)

#define CHTL_ERROR_AT(msg, file, line, col) \
    CHTL::ErrorBuilder(CHTL::ErrorLevel::ERROR, CHTL::ErrorType::SYNTAX_ERROR) \
        .withMessage(msg) \
        .atLocation(file, line, col) \
        .report()

// 错误恢复策略
class ErrorRecovery {
public:
    enum class Strategy {
        PANIC_MODE,        // 恐慌模式：跳过到下一个同步点
        PHRASE_LEVEL,      // 短语级恢复
        ERROR_PRODUCTION,  // 错误产生式
        GLOBAL_CORRECTION  // 全局纠正
    };
    
    static void setStrategy(Strategy strategy) { strategy_ = strategy; }
    static Strategy getStrategy() { return strategy_; }
    
    // 同步点
    static void addSyncToken(const std::string& token);
    static bool isSyncToken(const std::string& token);
    static void clearSyncTokens();
    
private:
    static Strategy strategy_;
    static std::vector<std::string> syncTokens_;
};

// 错误上下文守卫（RAII）
class ErrorContextGuard {
public:
    ErrorContextGuard(const std::string& phase, const std::string& component) {
        ErrorReport::getInstance().pushContext(phase, component);
    }
    
    ~ErrorContextGuard() {
        ErrorReport::getInstance().popContext();
    }
    
private:
    ErrorContextGuard(const ErrorContextGuard&) = delete;
    ErrorContextGuard& operator=(const ErrorContextGuard&) = delete;
};

#define ERROR_CONTEXT(phase, component) \
    CHTL::ErrorContextGuard _error_context_guard(phase, component)

} // namespace CHTL

#endif // ERROR_REPORT_H