#include "ErrorReport.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>

namespace CHTL {

// ========== ErrorPosition 实现 ==========

std::string ErrorPosition::toString() const {
    std::stringstream ss;
    if (!filename.empty()) {
        ss << filename << ":";
    }
    ss << line << ":" << column;
    return ss.str();
}

// ========== ErrorEntry 实现 ==========

void ErrorEntry::addSuggestion(const std::string& suggestion) {
    suggestions.push_back(suggestion);
}

std::string ErrorEntry::getLevelString() const {
    switch (level) {
        case ErrorLevel::INFO:    return "INFO";
        case ErrorLevel::WARNING: return "WARNING";
        case ErrorLevel::ERROR:   return "ERROR";
        case ErrorLevel::FATAL:   return "FATAL";
        default:                  return "UNKNOWN";
    }
}

std::string ErrorEntry::getTypeString() const {
    switch (type) {
        case ErrorType::LEXICAL_ERROR:           return "LEXICAL_ERROR";
        case ErrorType::UNTERMINATED_STRING:     return "UNTERMINATED_STRING";
        case ErrorType::UNTERMINATED_COMMENT:    return "UNTERMINATED_COMMENT";
        case ErrorType::INVALID_CHARACTER:       return "INVALID_CHARACTER";
        case ErrorType::INVALID_ESCAPE_SEQUENCE: return "INVALID_ESCAPE_SEQUENCE";
        case ErrorType::MALFORMED_TOKEN:         return "MALFORMED_TOKEN";
        
        case ErrorType::SYNTAX_ERROR:            return "SYNTAX_ERROR";
        case ErrorType::UNEXPECTED_TOKEN:        return "UNEXPECTED_TOKEN";
        case ErrorType::MISSING_TOKEN:           return "MISSING_TOKEN";
        case ErrorType::INVALID_EXPRESSION:      return "INVALID_EXPRESSION";
        
        case ErrorType::SEMANTIC_ERROR:          return "SEMANTIC_ERROR";
        case ErrorType::UNDEFINED_SYMBOL:        return "UNDEFINED_SYMBOL";
        case ErrorType::DUPLICATE_SYMBOL:        return "DUPLICATE_SYMBOL";
        case ErrorType::TYPE_MISMATCH:           return "TYPE_MISMATCH";
        case ErrorType::INVALID_OPERATION:       return "INVALID_OPERATION";
        
        case ErrorType::STATE_ERROR:             return "STATE_ERROR";
        case ErrorType::INVALID_STATE_TRANSITION: return "INVALID_STATE_TRANSITION";
        case ErrorType::CONTEXT_ERROR:           return "CONTEXT_ERROR";
        
        case ErrorType::IMPORT_ERROR:            return "IMPORT_ERROR";
        case ErrorType::FILE_NOT_FOUND:          return "FILE_NOT_FOUND";
        case ErrorType::CIRCULAR_DEPENDENCY:     return "CIRCULAR_DEPENDENCY";
        case ErrorType::MODULE_ERROR:            return "MODULE_ERROR";
        
        case ErrorType::INTERNAL_ERROR:          return "INTERNAL_ERROR";
        case ErrorType::UNKNOWN_ERROR:           return "UNKNOWN_ERROR";
        default:                                 return "UNKNOWN_TYPE";
    }
}

std::string ErrorEntry::format() const {
    std::stringstream ss;
    ss << position.toString() << ": " << getLevelString() << ": " << message;
    return ss.str();
}

std::string ErrorEntry::formatDetailed() const {
    std::stringstream ss;
    
    // 基本信息
    ss << position.toString() << ": " << getLevelString() << ": " << message << std::endl;
    
    // 错误类型
    ss << "  Type: " << getTypeString() << std::endl;
    
    // 上下文信息
    if (!context.empty()) {
        ss << "  Context: " << context << std::endl;
    }
    
    // 建议
    if (!suggestions.empty()) {
        ss << "  Suggestions:" << std::endl;
        for (const auto& suggestion : suggestions) {
            ss << "    - " << suggestion << std::endl;
        }
    }
    
    return ss.str();
}

// ========== ErrorReporter 实现 ==========

ErrorReporter::ErrorReporter() 
    : m_maxErrors(100)
    , m_immediateOutput(false) {
}

void ErrorReporter::report(ErrorLevel level, 
                          ErrorType type, 
                          const std::string& message,
                          const ErrorPosition& position,
                          const std::string& context) {
    
    if (shouldStopReporting()) {
        return;
    }
    
    ErrorPosition pos = position;
    if (pos.filename.empty() && !m_currentFile.empty()) {
        pos.filename = m_currentFile;
    }
    
    ErrorEntry entry(level, type, message, pos, context);
    internalReport(entry);
}

void ErrorReporter::info(const std::string& message, 
                        const ErrorPosition& position,
                        const std::string& context) {
    report(ErrorLevel::INFO, ErrorType::UNKNOWN_ERROR, message, position, context);
}

void ErrorReporter::warning(const std::string& message, 
                           const ErrorPosition& position,
                           const std::string& context) {
    report(ErrorLevel::WARNING, ErrorType::UNKNOWN_ERROR, message, position, context);
}

void ErrorReporter::error(ErrorType type,
                         const std::string& message, 
                         const ErrorPosition& position,
                         const std::string& context) {
    report(ErrorLevel::ERROR, type, message, position, context);
}

void ErrorReporter::fatal(ErrorType type,
                         const std::string& message, 
                         const ErrorPosition& position,
                         const std::string& context) {
    report(ErrorLevel::FATAL, type, message, position, context);
}

const std::vector<ErrorEntry>& ErrorReporter::getErrors() const {
    return m_errors;
}

std::vector<ErrorEntry> ErrorReporter::getErrorsByLevel(ErrorLevel level) const {
    std::vector<ErrorEntry> result;
    for (const auto& error : m_errors) {
        if (error.level == level) {
            result.push_back(error);
        }
    }
    return result;
}

std::vector<ErrorEntry> ErrorReporter::getErrorsByType(ErrorType type) const {
    std::vector<ErrorEntry> result;
    for (const auto& error : m_errors) {
        if (error.type == type) {
            result.push_back(error);
        }
    }
    return result;
}

ErrorStatistics ErrorReporter::getStatistics() const {
    ErrorStatistics stats;
    for (const auto& error : m_errors) {
        switch (error.level) {
            case ErrorLevel::INFO:    stats.infoCount++; break;
            case ErrorLevel::WARNING: stats.warningCount++; break;
            case ErrorLevel::ERROR:   stats.errorCount++; break;
            case ErrorLevel::FATAL:   stats.fatalCount++; break;
        }
    }
    return stats;
}

bool ErrorReporter::hasErrors() const {
    return getStatistics().hasErrors();
}

bool ErrorReporter::hasFatalErrors() const {
    return getStatistics().hasFatalErrors();
}

size_t ErrorReporter::getErrorCount() const {
    return m_errors.size();
}

void ErrorReporter::setErrorCallback(std::function<void(const ErrorEntry&)> callback) {
    m_callback = callback;
}

void ErrorReporter::setMaxErrors(size_t maxErrors) {
    m_maxErrors = maxErrors;
}

void ErrorReporter::setImmediateOutput(bool immediate) {
    m_immediateOutput = immediate;
}

void ErrorReporter::setCurrentFile(const std::string& filename) {
    m_currentFile = filename;
}

void ErrorReporter::printAll() const {
    printToStream(std::cerr);
}

void ErrorReporter::printSummary() const {
    ErrorStatistics stats = getStatistics();
    
    std::cerr << "\nError Summary:" << std::endl;
    std::cerr << "  Info: " << stats.infoCount << std::endl;
    std::cerr << "  Warnings: " << stats.warningCount << std::endl;
    std::cerr << "  Errors: " << stats.errorCount << std::endl;
    std::cerr << "  Fatal: " << stats.fatalCount << std::endl;
    std::cerr << "  Total: " << stats.totalCount() << std::endl;
}

void ErrorReporter::printToStream(std::ostream& os) const {
    for (const auto& error : m_errors) {
        os << error.format() << std::endl;
    }
}

void ErrorReporter::clear() {
    m_errors.clear();
}

void ErrorReporter::clearLevel(ErrorLevel level) {
    m_errors.erase(
        std::remove_if(m_errors.begin(), m_errors.end(),
                      [level](const ErrorEntry& entry) {
                          return entry.level == level;
                      }),
        m_errors.end()
    );
}

void ErrorReporter::pushState() {
    m_stateStack.push_back(m_errors);
}

void ErrorReporter::popState() {
    if (!m_stateStack.empty()) {
        m_errors = m_stateStack.back();
        m_stateStack.pop_back();
    }
}

// ========== 私有方法实现 ==========

void ErrorReporter::internalReport(const ErrorEntry& entry) {
    m_errors.push_back(entry);
    
    // 立即输出
    if (m_immediateOutput) {
        std::cerr << entry.format() << std::endl;
    }
    
    // 调用回调
    if (m_callback) {
        m_callback(entry);
    }
}

bool ErrorReporter::shouldStopReporting() const {
    return m_errors.size() >= m_maxErrors;
}

// ========== 全局错误报告器 ==========

ErrorReporter& getGlobalErrorReporter() {
    static ErrorReporter globalReporter;
    return globalReporter;
}

} // namespace CHTL