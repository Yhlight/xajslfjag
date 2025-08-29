#include "CHTLJSFileSystem.h"
#include "../../Error/ErrorReport.h"
#include <fstream>
#include <sstream>

namespace CHTLJS {

namespace CHTLJSFile {

std::optional<std::string> readToString(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        CHTL::ErrorBuilder(CHTL::ErrorLevel::ERROR, CHTL::ErrorType::FILE_NOT_FOUND)
            .withMessage("Failed to open file")
            .withDetail("Path: " + path)
            .report();
        return std::nullopt;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

bool writeString(const std::string& path, const std::string& content) {
    std::ofstream file(path);
    if (!file.is_open()) {
        CHTL::ErrorBuilder(CHTL::ErrorLevel::ERROR, CHTL::ErrorType::FILE_NOT_FOUND)
            .withMessage("Failed to create file")
            .withDetail("Path: " + path)
            .report();
        return false;
    }
    
    file << content;
    return file.good();
}

bool exists(const std::string& path) {
    std::ifstream file(path);
    return file.good();
}

} // namespace CHTLJSFile

namespace CHTLJSDirectory {

bool exists(const std::string& path) {
    // Simple check - try to open as directory
    std::filesystem::path p(path);
    return std::filesystem::exists(p) && std::filesystem::is_directory(p);
}

bool create(const std::string& path) {
    std::filesystem::path p(path);
    return std::filesystem::create_directories(p);
}

std::vector<std::string> list(const std::string& path) {
    std::vector<std::string> entries;
    
    try {
        for (const auto& entry : std::filesystem::directory_iterator(path)) {
            entries.push_back(entry.path().filename().string());
        }
    } catch (const std::exception& e) {
        CHTL::ErrorBuilder(CHTL::ErrorLevel::ERROR, CHTL::ErrorType::FILE_NOT_FOUND)
            .withMessage("Failed to list directory")
            .withDetail(e.what())
            .report();
    }
    
    return entries;
}

} // namespace CHTLJSDirectory

} // namespace CHTLJS