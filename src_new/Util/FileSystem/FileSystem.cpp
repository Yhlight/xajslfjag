#include "FileSystem.h"
#include <fstream>
#include <filesystem>
#include <sstream>

namespace CHTL {
namespace Util {

bool FileSystem::fileExists(const std::string& path) {
    return std::filesystem::exists(path) && std::filesystem::is_regular_file(path);
}

bool FileSystem::directoryExists(const std::string& path) {
    return std::filesystem::exists(path) && std::filesystem::is_directory(path);
}

bool FileSystem::createDirectory(const std::string& path) {
    try {
        return std::filesystem::create_directories(path);
    } catch (...) {
        return false;
    }
}

std::string FileSystem::readFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        return "";
    }
    
    std::ostringstream oss;
    oss << file.rdbuf();
    return oss.str();
}

bool FileSystem::writeFile(const std::string& path, const std::string& content) {
    std::ofstream file(path);
    if (!file.is_open()) {
        return false;
    }
    
    file << content;
    return true;
}

std::vector<std::string> FileSystem::listFiles(const std::string& directory, const std::string& extension) {
    std::vector<std::string> files;
    
    try {
        for (const auto& entry : std::filesystem::directory_iterator(directory)) {
            if (entry.is_regular_file()) {
                std::string filename = entry.path().filename().string();
                if (extension.empty() || getFileExtension(filename) == extension) {
                    files.push_back(filename);
                }
            }
        }
    } catch (...) {
        // 目录不存在或无法访问
    }
    
    return files;
}

std::string FileSystem::getFileExtension(const std::string& path) {
    size_t dotPos = path.find_last_of('.');
    if (dotPos == std::string::npos) {
        return "";
    }
    return path.substr(dotPos + 1);
}

std::string FileSystem::getFileName(const std::string& path) {
    size_t slashPos = path.find_last_of("/\\");
    if (slashPos == std::string::npos) {
        return path;
    }
    return path.substr(slashPos + 1);
}

std::string FileSystem::getDirectoryPath(const std::string& path) {
    size_t slashPos = path.find_last_of("/\\");
    if (slashPos == std::string::npos) {
        return ".";
    }
    return path.substr(0, slashPos);
}

std::string FileSystem::combinePath(const std::string& dir, const std::string& file) {
    if (dir.empty()) return file;
    if (file.empty()) return dir;
    
    std::string result = dir;
    if (result.back() != '/' && result.back() != '\\') {
        result += "/";
    }
    result += file;
    
    return normalizePath(result);
}

std::string FileSystem::normalizePath(const std::string& path) {
    std::string result = path;
    
    // 简单的路径规范化
    // 将反斜杠替换为正斜杠
    for (char& c : result) {
        if (c == '\\') {
            c = '/';
        }
    }
    
    return result;
}

} // namespace Util
} // namespace CHTL