#pragma once

#include <string>
#include <vector>

namespace CHTL {

class ErrorReporter {
public:
    ErrorReporter();
    ~ErrorReporter();
    
    void error(const std::string& component, const std::string& message);
    void warning(const std::string& component, const std::string& message);
    void info(const std::string& component, const std::string& message);
    
    std::vector<std::string> getErrors() const { return m_errors; }
    std::vector<std::string> getWarnings() const { return m_warnings; }
    void clearErrors();
    
private:
    std::vector<std::string> m_errors;
    std::vector<std::string> m_warnings;
};

}