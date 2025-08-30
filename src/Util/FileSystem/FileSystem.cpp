#include "FileSystem.h"
#include <fstream>
#include <filesystem>

namespace CHTL {

bool FileSystem::exists(const std::string& path) {
    return std::filesystem::exists(path);
}

bool FileSystem::isFile(const std::string& path) {
    return std::filesystem::is_regular_file(path);
}

bool FileSystem::isDirectory(const std::string& path) {
    return std::filesystem::is_directory(path);
}

std::string FileSystem::readFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        return "";
    }
    
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    
    return content;
}

bool FileSystem::writeFile(const std::string& filePath, const std::string& content) {
    std::ofstream file(filePath);
    if (!file.is_open()) {
        return false;
    }
    
    file << content;
    file.close();
    
    return true;
}

bool FileSystem::createDirectory(const std::string& path) {
    try {
        return std::filesystem::create_directories(path);
    } catch (const std::exception&) {
        return false;
    }
}

std::vector<std::string> FileSystem::listFiles(const std::string& directory) {
    std::vector<std::string> files;
    
    try {
        for (const auto& entry : std::filesystem::directory_iterator(directory)) {
            if (entry.is_regular_file()) {
                files.push_back(entry.path().filename().string());
            }
        }
    } catch (const std::exception&) {
        // 忽略错误
    }
    
    return files;
}

}