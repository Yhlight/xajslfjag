#include "ErrorReporter.h"
#include <iostream>

namespace CHTL {

ErrorReporter::ErrorReporter() = default;
ErrorReporter::~ErrorReporter() = default;

void ErrorReporter::error(const std::string& component, const std::string& message) {
    std::string fullMessage = "[" + component + "] " + message;
    m_errors.push_back(fullMessage);
    std::cerr << "❌ " << fullMessage << std::endl;
}

void ErrorReporter::warning(const std::string& component, const std::string& message) {
    std::string fullMessage = "[" + component + "] " + message;
    m_warnings.push_back(fullMessage);
    std::cout << "⚠️ " << fullMessage << std::endl;
}

void ErrorReporter::info(const std::string& component, const std::string& message) {
    std::string fullMessage = "[" + component + "] " + message;
    std::cout << "ℹ️ " << fullMessage << std::endl;
}

void ErrorReporter::clearErrors() {
    m_errors.clear();
    m_warnings.clear();
}

}