#include "FileSystem.h"
#include <fstream>
#include <sstream>
#include <filesystem>

namespace fs = std::filesystem;

namespace CHTL {
namespace FileSystem {

bool FileExists(const std::string& filePath) {
    return fs::exists(filePath) && fs::is_regular_file(filePath);
}

bool DirectoryExists(const std::string& dirPath) {
    return fs::exists(dirPath) && fs::is_directory(dirPath);
}

bool CreateDirectory(const std::string& dirPath) {
    try {
        return fs::create_directories(dirPath);
    } catch (...) {
        return false;
    }
}

size_t GetFileSize(const std::string& filePath) {
    try {
        return fs::file_size(filePath);
    } catch (...) {
        return 0;
    }
}

std::string ReadTextFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        return "";
    }
    
    std::ostringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

bool WriteTextFile(const std::string& filePath, const std::string& content) {
    std::ofstream file(filePath);
    if (!file.is_open()) {
        return false;
    }
    
    file << content;
    return true;
}

std::string GetFileExtension(const std::string& filePath) {
    fs::path path(filePath);
    return path.extension().string();
}

std::string GetFileName(const std::string& filePath) {
    fs::path path(filePath);
    return path.filename().string();
}

std::string GetDirectoryPath(const std::string& filePath) {
    fs::path path(filePath);
    return path.parent_path().string();
}

std::string JoinPath(const std::string& path1, const std::string& path2) {
    fs::path p1(path1);
    fs::path p2(path2);
    return (p1 / p2).string();
}

std::string NormalizePath(const std::string& path) {
    try {
        return fs::canonical(path).string();
    } catch (...) {
        return path;
    }
}

std::string GetCurrentDirectory() {
    try {
        return fs::current_path().string();
    } catch (...) {
        return "";
    }
}

std::vector<std::string> ListFiles(const std::string& dirPath, const std::string& extension) {
    std::vector<std::string> files;
    
    try {
        for (const auto& entry : fs::directory_iterator(dirPath)) {
            if (entry.is_regular_file()) {
                std::string filePath = entry.path().string();
                if (extension.empty() || GetFileExtension(filePath) == extension) {
                    files.push_back(filePath);
                }
            }
        }
    } catch (...) {
        // 忽略错误
    }
    
    return files;
}

} // namespace FileSystem
} // namespace CHTL