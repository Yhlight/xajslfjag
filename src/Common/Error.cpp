#include "Error.h"
#include <iostream>
#include <sstream>

namespace CHTL {

std::string Error::ToString() const {
    std::stringstream ss;
    
    // 错误级别
    switch (level_) {
        case ErrorLevel::INFO:
            ss << "[INFO] ";
            break;
        case ErrorLevel::WARNING:
            ss << "[WARNING] ";
            break;
        case ErrorLevel::ERROR:
            ss << "[ERROR] ";
            break;
        case ErrorLevel::FATAL:
            ss << "[FATAL] ";
            break;
    }
    
    // 位置信息
    ss << location_.ToString() << ": ";
    
    // 错误消息
    ss << message_;
    
    return ss.str();
}

void ErrorCollector::PrintErrors() const {
    for (const auto& error : errors_) {
        std::cerr << error->ToString() << std::endl;
    }
}

} // namespace CHTL