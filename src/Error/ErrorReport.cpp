#include "ErrorReport.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <ctime>

namespace CHTL {

// ErrorLocation implementation
std::string ErrorLocation::toString() const {
    std::stringstream ss;
    if (!filename.empty()) {
        ss << filename;
        if (line > 0) {
            ss << ":" << line;
            if (column > 0) {
                ss << ":" << column;
            }
        }
    }
    return ss.str();
}

// ConsoleErrorReporter implementation
ConsoleErrorReporter::ConsoleErrorReporter(bool useColors) : useColors_(useColors) {}

void ConsoleErrorReporter::report(const ErrorInfo& error) {
    std::cerr << formatError(error) << std::endl;
}

void ConsoleErrorReporter::flush() {
    std::cerr.flush();
}

std::string ConsoleErrorReporter::formatError(const ErrorInfo& error) {
    std::stringstream ss;
    
    // 错误位置
    if (!error.location.filename.empty()) {
        ss << error.location.toString() << ": ";
    }
    
    // 错误级别
    std::string levelStr;
    switch (error.level) {
        case ErrorLevel::DEBUG:   levelStr = "debug"; break;
        case ErrorLevel::INFO:    levelStr = "info"; break;
        case ErrorLevel::WARNING: levelStr = "warning"; break;
        case ErrorLevel::ERROR:   levelStr = "error"; break;
        case ErrorLevel::FATAL:   levelStr = "fatal"; break;
    }
    
    if (useColors_) {
        levelStr = colorize(levelStr, getColorForLevel(error.level));
    }
    
    ss << levelStr << " " << error.code << ": " << error.message;
    
    // 详细信息
    if (verbose_ && !error.detail.empty()) {
        ss << "\n  Detail: " << error.detail;
    }
    
    // 上下文信息
    if (verbose_ && !error.context.phase.empty()) {
        ss << "\n  Context: " << error.context.phase;
        if (!error.context.component.empty()) {
            ss << " in " << error.context.component;
        }
    }
    
    // 修复建议
    if (!error.suggestions.empty()) {
        ss << "\n  Suggestions:";
        for (const auto& suggestion : error.suggestions) {
            ss << "\n    - " << suggestion;
        }
    }
    
    // 堆栈跟踪
    if (showStackTrace_ && !error.stackTrace.empty()) {
        ss << "\n  Stack trace:\n" << error.stackTrace;
    }
    
    return ss.str();
}

std::string ConsoleErrorReporter::colorize(const std::string& text, const std::string& color) {
    if (!useColors_) return text;
    return color + text + "\033[0m";
}

std::string ConsoleErrorReporter::getColorForLevel(ErrorLevel level) {
    switch (level) {
        case ErrorLevel::DEBUG:   return "\033[90m";  // 灰色
        case ErrorLevel::INFO:    return "\033[36m";  // 青色
        case ErrorLevel::WARNING: return "\033[33m";  // 黄色
        case ErrorLevel::ERROR:   return "\033[31m";  // 红色
        case ErrorLevel::FATAL:   return "\033[91m";  // 亮红色
        default: return "";
    }
}

// FileErrorReporter implementation
FileErrorReporter::FileErrorReporter(const std::string& filename) 
    : filename_(filename), file_(std::make_unique<std::ofstream>(filename, std::ios::app)) {
    if (!file_->is_open()) {
        throw std::runtime_error("Failed to open error log file: " + filename);
    }
}

FileErrorReporter::~FileErrorReporter() {
    if (file_ && file_->is_open()) {
        file_->close();
    }
}

void FileErrorReporter::report(const ErrorInfo& error) {
    if (!file_ || !file_->is_open()) return;
    
    // 时间戳
    auto time = std::chrono::system_clock::to_time_t(error.timestamp);
    *file_ << "[" << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S") << "] ";
    
    // 级别
    switch (error.level) {
        case ErrorLevel::DEBUG:   *file_ << "[DEBUG] "; break;
        case ErrorLevel::INFO:    *file_ << "[INFO] "; break;
        case ErrorLevel::WARNING: *file_ << "[WARNING] "; break;
        case ErrorLevel::ERROR:   *file_ << "[ERROR] "; break;
        case ErrorLevel::FATAL:   *file_ << "[FATAL] "; break;
    }
    
    // 错误信息
    *file_ << error.code << ": " << error.message;
    
    // 位置
    if (!error.location.filename.empty()) {
        *file_ << " at " << error.location.toString();
    }
    
    *file_ << std::endl;
    
    // 详细信息
    if (!error.detail.empty()) {
        *file_ << "  Detail: " << error.detail << std::endl;
    }
    
    // 上下文
    if (!error.context.phase.empty()) {
        *file_ << "  Context: " << error.context.phase;
        if (!error.context.component.empty()) {
            *file_ << " in " << error.context.component;
        }
        *file_ << std::endl;
    }
}

void FileErrorReporter::flush() {
    if (file_ && file_->is_open()) {
        file_->flush();
    }
}

// JSONErrorReporter implementation
JSONErrorReporter::JSONErrorReporter() {
    buffer_ << "{\n  \"errors\": [\n";
}

void JSONErrorReporter::report(const ErrorInfo& error) {
    if (!firstError_) {
        buffer_ << ",\n";
    }
    firstError_ = false;
    
    buffer_ << "    {\n";
    buffer_ << "      \"level\": \"";
    switch (error.level) {
        case ErrorLevel::DEBUG:   buffer_ << "debug"; break;
        case ErrorLevel::INFO:    buffer_ << "info"; break;
        case ErrorLevel::WARNING: buffer_ << "warning"; break;
        case ErrorLevel::ERROR:   buffer_ << "error"; break;
        case ErrorLevel::FATAL:   buffer_ << "fatal"; break;
    }
    buffer_ << "\",\n";
    
    buffer_ << "      \"type\": \"";
    switch (error.type) {
        case ErrorType::SYNTAX_ERROR:     buffer_ << "syntax"; break;
        case ErrorType::LEXICAL_ERROR:    buffer_ << "lexical"; break;
        case ErrorType::SEMANTIC_ERROR:   buffer_ << "semantic"; break;
        case ErrorType::TYPE_ERROR:       buffer_ << "type"; break;
        case ErrorType::REFERENCE_ERROR:  buffer_ << "reference"; break;
        case ErrorType::IMPORT_ERROR:     buffer_ << "import"; break;
        case ErrorType::NAMESPACE_ERROR:  buffer_ << "namespace"; break;
        case ErrorType::CONSTRAINT_ERROR: buffer_ << "constraint"; break;
        case ErrorType::RUNTIME_ERROR:    buffer_ << "runtime"; break;
        case ErrorType::IO_ERROR:         buffer_ << "io"; break;
        case ErrorType::FILE_NOT_FOUND:   buffer_ << "file_not_found"; break;
        case ErrorType::MEMORY_ERROR:     buffer_ << "memory"; break;
        case ErrorType::TIMEOUT_ERROR:    buffer_ << "timeout"; break;
        case ErrorType::INTERNAL_ERROR:   buffer_ << "internal"; break;
    }
    buffer_ << "\",\n";
    
    buffer_ << "      \"code\": \"" << error.code << "\",\n";
    buffer_ << "      \"message\": \"" << error.message << "\",\n";
    
    if (!error.detail.empty()) {
        buffer_ << "      \"detail\": \"" << error.detail << "\",\n";
    }
    
    if (!error.location.filename.empty()) {
        buffer_ << "      \"location\": {\n";
        buffer_ << "        \"file\": \"" << error.location.filename << "\",\n";
        buffer_ << "        \"line\": " << error.location.line << ",\n";
        buffer_ << "        \"column\": " << error.location.column << "\n";
        buffer_ << "      },\n";
    }
    
    if (!error.suggestions.empty()) {
        buffer_ << "      \"suggestions\": [";
        for (size_t i = 0; i < error.suggestions.size(); ++i) {
            if (i > 0) buffer_ << ", ";
            buffer_ << "\"" << error.suggestions[i] << "\"";
        }
        buffer_ << "],\n";
    }
    
    // 时间戳
    auto time = std::chrono::system_clock::to_time_t(error.timestamp);
    buffer_ << "      \"timestamp\": \"" 
           << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S") 
           << "\"\n";
    
    buffer_ << "    }";
}

void JSONErrorReporter::flush() {
    buffer_ << "\n  ]\n}\n";
}

// ErrorCollector implementation
void ErrorCollector::report(const ErrorInfo& error) {
    errors_.push_back(error);
}

std::vector<ErrorInfo> ErrorCollector::getErrorsByLevel(ErrorLevel level) const {
    std::vector<ErrorInfo> result;
    std::copy_if(errors_.begin(), errors_.end(), std::back_inserter(result),
                 [level](const ErrorInfo& e) { return e.level == level; });
    return result;
}

std::vector<ErrorInfo> ErrorCollector::getErrorsByType(ErrorType type) const {
    std::vector<ErrorInfo> result;
    std::copy_if(errors_.begin(), errors_.end(), std::back_inserter(result),
                 [type](const ErrorInfo& e) { return e.type == type; });
    return result;
}

size_t ErrorCollector::getErrorCount() const {
    return std::count_if(errors_.begin(), errors_.end(),
                        [](const ErrorInfo& e) { 
                            return e.level == ErrorLevel::ERROR || 
                                   e.level == ErrorLevel::FATAL; 
                        });
}

size_t ErrorCollector::getWarningCount() const {
    return std::count_if(errors_.begin(), errors_.end(),
                        [](const ErrorInfo& e) { return e.level == ErrorLevel::WARNING; });
}

bool ErrorCollector::hasFatalErrors() const {
    return std::any_of(errors_.begin(), errors_.end(),
                      [](const ErrorInfo& e) { return e.level == ErrorLevel::FATAL; });
}

// ErrorReport implementation
void ErrorReport::addReporter(std::shared_ptr<IErrorReporter> reporter) {
    reporters_.push_back(reporter);
}

void ErrorReport::removeAllReporters() {
    reporters_.clear();
}

void ErrorReport::report(ErrorLevel level, ErrorType type, const std::string& message) {
    ErrorInfo error = createErrorInfo(level, type, message);
    report(error);
}

void ErrorReport::report(const ErrorInfo& error) {
    // 更新计数器
    switch (error.level) {
        case ErrorLevel::ERROR:
        case ErrorLevel::FATAL:
            totalErrors_++;
            break;
        case ErrorLevel::WARNING:
            if (suppressWarnings_) return;
            totalWarnings_++;
            break;
        default:
            break;
    }
    
    // 检查错误限制
    if (totalErrors_ > maxErrors_) {
        ErrorInfo limitError = createErrorInfo(
            ErrorLevel::FATAL,
            ErrorType::INTERNAL_ERROR,
            "Error limit exceeded (" + std::to_string(maxErrors_) + " errors)"
        );
        for (auto& reporter : reporters_) {
            reporter->report(limitError);
        }
        flush();
        
        if (throwOnFatal_) {
            throw std::runtime_error("Compilation terminated: too many errors");
        }
        return;
    }
    
    // 报告给所有报告器
    for (auto& reporter : reporters_) {
        reporter->report(error);
    }
    
    // 处理致命错误
    if (error.level == ErrorLevel::FATAL && throwOnFatal_) {
        flush();
        throw std::runtime_error("Fatal error: " + error.message);
    }
}

void ErrorReport::debug(const std::string& message) {
    report(ErrorLevel::DEBUG, ErrorType::INTERNAL_ERROR, message);
}

void ErrorReport::info(const std::string& message) {
    report(ErrorLevel::INFO, ErrorType::INTERNAL_ERROR, message);
}

void ErrorReport::warning(const std::string& message) {
    report(ErrorLevel::WARNING, ErrorType::INTERNAL_ERROR, message);
}

void ErrorReport::error(const std::string& message) {
    report(ErrorLevel::ERROR, ErrorType::INTERNAL_ERROR, message);
}

void ErrorReport::fatal(const std::string& message) {
    report(ErrorLevel::FATAL, ErrorType::INTERNAL_ERROR, message);
}

void ErrorReport::syntaxError(const std::string& message, const ErrorLocation& location) {
    ErrorInfo error = createErrorInfo(ErrorLevel::ERROR, ErrorType::SYNTAX_ERROR, message, location);
    report(error);
}

void ErrorReport::lexicalError(const std::string& message, const ErrorLocation& location) {
    ErrorInfo error = createErrorInfo(ErrorLevel::ERROR, ErrorType::LEXICAL_ERROR, message, location);
    report(error);
}

void ErrorReport::semanticError(const std::string& message, const ErrorLocation& location) {
    ErrorInfo error = createErrorInfo(ErrorLevel::ERROR, ErrorType::SEMANTIC_ERROR, message, location);
    report(error);
}

void ErrorReport::pushContext(const std::string& phase, const std::string& component) {
    contextStack_.push_back(currentContext_);
    currentContext_.phase = phase;
    currentContext_.component = component;
}

void ErrorReport::popContext() {
    if (!contextStack_.empty()) {
        currentContext_ = contextStack_.back();
        contextStack_.pop_back();
    }
}

void ErrorReport::registerErrorCode(const std::string& code, const std::string& description) {
    errorCodes_[code] = description;
}

std::string ErrorReport::getErrorDescription(const std::string& code) const {
    auto it = errorCodes_.find(code);
    return it != errorCodes_.end() ? it->second : "";
}

void ErrorReport::resetCounters() {
    totalErrors_ = 0;
    totalWarnings_ = 0;
}

void ErrorReport::flush() {
    for (auto& reporter : reporters_) {
        reporter->flush();
    }
}

std::string ErrorReport::generateErrorCode(ErrorType type) {
    static std::unordered_map<ErrorType, std::string> prefixes = {
        {ErrorType::SYNTAX_ERROR,     "E001"},
        {ErrorType::LEXICAL_ERROR,    "E002"},
        {ErrorType::SEMANTIC_ERROR,   "E003"},
        {ErrorType::TYPE_ERROR,       "E004"},
        {ErrorType::REFERENCE_ERROR,  "E005"},
        {ErrorType::IMPORT_ERROR,     "E006"},
        {ErrorType::NAMESPACE_ERROR,  "E007"},
        {ErrorType::CONSTRAINT_ERROR, "E008"},
        {ErrorType::RUNTIME_ERROR,    "E009"},
        {ErrorType::IO_ERROR,         "E010"},
        {ErrorType::MEMORY_ERROR,     "E011"},
        {ErrorType::TIMEOUT_ERROR,    "E012"},
        {ErrorType::INTERNAL_ERROR,   "E999"}
    };
    
    static std::unordered_map<ErrorType, int> counters;
    
    auto it = prefixes.find(type);
    if (it != prefixes.end()) {
        int& counter = counters[type];
        return it->second + std::to_string(++counter);
    }
    
    return "E000";
}

ErrorInfo ErrorReport::createErrorInfo(ErrorLevel level, ErrorType type, 
                                     const std::string& message,
                                     const ErrorLocation& location) {
    ErrorInfo error;
    error.level = level;
    error.type = type;
    error.code = generateErrorCode(type);
    error.message = message;
    error.location = location;
    error.context = currentContext_;
    error.timestamp = std::chrono::system_clock::now();
    
    // 添加错误描述
    std::string desc = getErrorDescription(error.code);
    if (!desc.empty()) {
        error.detail = desc;
    }
    
    return error;
}

// ErrorBuilder implementation
ErrorBuilder::ErrorBuilder(ErrorLevel level, ErrorType type) {
    error_.level = level;
    error_.type = type;
    error_.timestamp = std::chrono::system_clock::now();
}

ErrorBuilder& ErrorBuilder::withMessage(const std::string& message) {
    error_.message = message;
    return *this;
}

ErrorBuilder& ErrorBuilder::withDetail(const std::string& detail) {
    error_.detail = detail;
    return *this;
}

ErrorBuilder& ErrorBuilder::withCode(const std::string& code) {
    error_.code = code;
    return *this;
}

ErrorBuilder& ErrorBuilder::atLocation(const ErrorLocation& location) {
    error_.location = location;
    return *this;
}

ErrorBuilder& ErrorBuilder::atLocation(const std::string& file, size_t line, size_t col) {
    error_.location.filename = file;
    error_.location.line = line;
    error_.location.column = col;
    return *this;
}

ErrorBuilder& ErrorBuilder::inContext(const std::string& phase, const std::string& component) {
    error_.context.phase = phase;
    error_.context.component = component;
    return *this;
}

ErrorBuilder& ErrorBuilder::withMetadata(const std::string& key, const std::string& value) {
    error_.context.metadata[key] = value;
    return *this;
}

ErrorBuilder& ErrorBuilder::withSuggestion(const std::string& suggestion) {
    error_.suggestions.push_back(suggestion);
    return *this;
}

ErrorBuilder& ErrorBuilder::withStackTrace(const std::string& trace) {
    error_.stackTrace = trace;
    return *this;
}

void ErrorBuilder::report() {
    ErrorReport::getInstance().report(build());
}

ErrorInfo ErrorBuilder::build() const {
    ErrorInfo result = error_;
    if (result.code.empty()) {
        result.code = ErrorReport::getInstance().generateErrorCode(result.type);
    }
    return result;
}

// ErrorRecovery static members
ErrorRecovery::Strategy ErrorRecovery::strategy_ = ErrorRecovery::Strategy::PANIC_MODE;
std::vector<std::string> ErrorRecovery::syncTokens_ = {";", "}", "]", ")", "EOF"};

void ErrorRecovery::addSyncToken(const std::string& token) {
    if (std::find(syncTokens_.begin(), syncTokens_.end(), token) == syncTokens_.end()) {
        syncTokens_.push_back(token);
    }
}

bool ErrorRecovery::isSyncToken(const std::string& token) {
    return std::find(syncTokens_.begin(), syncTokens_.end(), token) != syncTokens_.end();
}

void ErrorRecovery::clearSyncTokens() {
    syncTokens_.clear();
}

} // namespace CHTL