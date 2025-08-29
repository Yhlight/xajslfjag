#pragma once
#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <sstream>

namespace CHTL {

/**
 * 错误等级枚举
 */
enum class ErrorLevel {
    INFO,           // 信息
    WARNING,        // 警告
    ERROR,          // 错误
    FATAL           // 致命错误
};

/**
 * 错误类型枚举
 */
enum class ErrorType {
    // 词法错误
    LEXICAL_ERROR,              // 词法分析错误
    UNTERMINATED_STRING,        // 未终止的字符串
    UNTERMINATED_COMMENT,       // 未终止的注释
    INVALID_CHARACTER,          // 无效字符
    INVALID_ESCAPE_SEQUENCE,    // 无效转义序列
    MALFORMED_TOKEN,            // 格式错误的token
    
    // 语法错误
    SYNTAX_ERROR,               // 语法错误
    UNEXPECTED_TOKEN,           // 意外的token
    MISSING_TOKEN,              // 缺失的token
    INVALID_EXPRESSION,         // 无效表达式
    
    // 语义错误
    SEMANTIC_ERROR,             // 语义错误
    UNDEFINED_SYMBOL,           // 未定义的符号
    DUPLICATE_SYMBOL,           // 重复定义的符号
    TYPE_MISMATCH,              // 类型不匹配
    INVALID_OPERATION,          // 无效操作
    
    // 状态错误
    STATE_ERROR,                // 状态错误
    INVALID_STATE_TRANSITION,   // 无效状态转换
    CONTEXT_ERROR,              // 上下文错误
    
    // 导入错误
    IMPORT_ERROR,               // 导入错误
    FILE_NOT_FOUND,             // 文件未找到
    CIRCULAR_DEPENDENCY,        // 循环依赖
    MODULE_ERROR,               // 模块错误
    
    // 模板系统错误
    TEMPLATE_ERROR,             // 模板错误
    TEMPLATE_WARNING,           // 模板警告
    TEMPLATE_NOT_FOUND,         // 模板未找到
    TEMPLATE_INSTANTIATION_ERROR, // 模板实例化错误
    CIRCULAR_INHERITANCE,       // 循环继承
    INVALID_INHERITANCE,        // 无效继承
    SPECIALIZATION_ERROR,       // 特例化错误
    VARIABLE_NOT_FOUND,         // 变量未找到
    
    // 约束系统错误
    CONSTRAINT_VIOLATION,       // 约束违反
    CONSTRAINT_ERROR,           // 约束错误
    
    // 其他错误
    INTERNAL_ERROR,             // 内部错误
    UNKNOWN_ERROR               // 未知错误
};

/**
 * 错误位置信息
 */
struct ErrorPosition {
    std::string filename;       // 文件名
    size_t line;               // 行号(从1开始)
    size_t column;             // 列号(从1开始)
    size_t offset;             // 偏移量
    size_t length;             // 错误长度
    
    ErrorPosition(const std::string& file = "", 
                  size_t l = 1, 
                  size_t c = 1, 
                  size_t o = 0, 
                  size_t len = 1)
        : filename(file), line(l), column(c), offset(o), length(len) {}
    
    std::string toString() const;
};

/**
 * 错误报告条目
 */
class ErrorEntry {
public:
    ErrorLevel level;           // 错误等级
    ErrorType type;             // 错误类型
    std::string message;        // 错误消息
    ErrorPosition position;     // 错误位置
    std::string context;        // 上下文信息
    std::vector<std::string> suggestions; // 建议修复方案
    
    ErrorEntry(ErrorLevel lvl, 
               ErrorType tp, 
               const std::string& msg,
               const ErrorPosition& pos = ErrorPosition(),
               const std::string& ctx = "")
        : level(lvl), type(tp), message(msg), position(pos), context(ctx) {}
    
    // 添加建议
    void addSuggestion(const std::string& suggestion);
    
    // 获取错误等级字符串
    std::string getLevelString() const;
    
    // 获取错误类型字符串
    std::string getTypeString() const;
    
    // 格式化输出
    std::string format() const;
    
    // 详细格式化输出
    std::string formatDetailed() const;
};

/**
 * 错误统计信息
 */
struct ErrorStatistics {
    size_t infoCount;
    size_t warningCount;
    size_t errorCount;
    size_t fatalCount;
    
    ErrorStatistics() : infoCount(0), warningCount(0), errorCount(0), fatalCount(0) {}
    
    size_t totalCount() const { return infoCount + warningCount + errorCount + fatalCount; }
    bool hasErrors() const { return errorCount > 0 || fatalCount > 0; }
    bool hasFatalErrors() const { return fatalCount > 0; }
};

/**
 * 错误报告器
 * 统一的错误处理和报告系统
 */
class ErrorReporter {
public:
    /**
     * 构造函数
     */
    ErrorReporter();
    
    /**
     * 析构函数
     */
    ~ErrorReporter() = default;
    
    // 禁用拷贝和移动
    ErrorReporter(const ErrorReporter&) = delete;
    ErrorReporter& operator=(const ErrorReporter&) = delete;
    
    // ========== 错误报告方法 ==========
    
    /**
     * 报告错误
     */
    void report(ErrorLevel level, 
                ErrorType type, 
                const std::string& message,
                const ErrorPosition& position = ErrorPosition(),
                const std::string& context = "");
    
    /**
     * 报告信息
     */
    void info(const std::string& message, 
              const ErrorPosition& position = ErrorPosition(),
              const std::string& context = "");
    
    /**
     * 报告警告
     */
    void warning(const std::string& message, 
                 const ErrorPosition& position = ErrorPosition(),
                 const std::string& context = "");
    
    /**
     * 报告错误
     */
    void error(ErrorType type,
               const std::string& message, 
               const ErrorPosition& position = ErrorPosition(),
               const std::string& context = "");
    
    /**
     * 报告致命错误
     */
    void fatal(ErrorType type,
               const std::string& message, 
               const ErrorPosition& position = ErrorPosition(),
               const std::string& context = "");
    
    // ========== 错误查询方法 ==========
    
    /**
     * 获取所有错误
     */
    const std::vector<ErrorEntry>& getErrors() const;
    
    /**
     * 获取指定等级的错误
     */
    std::vector<ErrorEntry> getErrorsByLevel(ErrorLevel level) const;
    
    /**
     * 获取指定类型的错误
     */
    std::vector<ErrorEntry> getErrorsByType(ErrorType type) const;
    
    /**
     * 获取错误统计
     */
    ErrorStatistics getStatistics() const;
    
    /**
     * 检查是否有错误
     */
    bool hasErrors() const;
    
    /**
     * 检查是否有致命错误
     */
    bool hasFatalErrors() const;
    
    /**
     * 获取错误数量
     */
    size_t getErrorCount() const;
    
    // ========== 错误处理配置 ==========
    
    /**
     * 设置错误回调函数
     */
    void setErrorCallback(std::function<void(const ErrorEntry&)> callback);
    
    /**
     * 设置最大错误数量
     */
    void setMaxErrors(size_t maxErrors);
    
    /**
     * 设置是否立即输出错误
     */
    void setImmediateOutput(bool immediate);
    
    /**
     * 设置当前文件名
     */
    void setCurrentFile(const std::string& filename);
    
    // ========== 错误输出方法 ==========
    
    /**
     * 输出所有错误
     */
    void printAll() const;
    
    /**
     * 输出摘要
     */
    void printSummary() const;
    
    /**
     * 输出到流
     */
    void printToStream(std::ostream& os) const;
    
    // ========== 错误管理方法 ==========
    
    /**
     * 清除所有错误
     */
    void clear();
    
    /**
     * 清除指定等级的错误
     */
    void clearLevel(ErrorLevel level);
    
    /**
     * 备份当前错误状态
     */
    void pushState();
    
    /**
     * 恢复错误状态
     */
    void popState();

private:
    std::vector<ErrorEntry> m_errors;               // 错误列表
    std::function<void(const ErrorEntry&)> m_callback; // 错误回调
    size_t m_maxErrors;                             // 最大错误数量
    bool m_immediateOutput;                         // 是否立即输出
    std::string m_currentFile;                      // 当前文件名
    std::vector<std::vector<ErrorEntry>> m_stateStack; // 状态栈
    
    /**
     * 内部报告方法
     */
    void internalReport(const ErrorEntry& entry);
    
    /**
     * 检查是否达到最大错误数量
     */
    bool shouldStopReporting() const;
};

/**
 * 全局错误报告器
 */
extern ErrorReporter& getGlobalErrorReporter();

/**
 * 便捷宏定义
 */
#define ERROR_REPORTER getGlobalErrorReporter()

#define REPORT_INFO(msg, ...) \
    ERROR_REPORTER.info(msg, ##__VA_ARGS__)

#define REPORT_WARNING(msg, ...) \
    ERROR_REPORTER.warning(msg, ##__VA_ARGS__)

#define REPORT_ERROR(type, msg, ...) \
    ERROR_REPORTER.error(type, msg, ##__VA_ARGS__)

#define REPORT_FATAL(type, msg, ...) \
    ERROR_REPORTER.fatal(type, msg, ##__VA_ARGS__)

} // namespace CHTL