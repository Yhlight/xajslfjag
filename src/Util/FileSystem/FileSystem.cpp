#include "FileSystem.h"
#include <filesystem>
#include <fstream>
#include <algorithm>
#include <random>
#include <sstream>
#include <iomanip>

#ifdef _WIN32
#include <windows.h>
#include <direct.h>
#else
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#endif

namespace Util {

// FileSystem基类实现
bool FileSystem::exists(const CHTL::String& path) const {
    try {
        return std::filesystem::exists(path);
    } catch (...) {
        return false;
    }
}

bool FileSystem::isFile(const CHTL::String& path) const {
    try {
        return std::filesystem::is_regular_file(path);
    } catch (...) {
        return false;
    }
}

bool FileSystem::isDirectory(const CHTL::String& path) const {
    try {
        return std::filesystem::is_directory(path);
    } catch (...) {
        return false;
    }
}

bool FileSystem::isReadable(const CHTL::String& path) const {
    std::ifstream file(path);
    return file.good();
}

bool FileSystem::isWritable(const CHTL::String& path) const {
    if (exists(path)) {
        std::ofstream file(path, std::ios::app);
        return file.good();
    } else {
        std::ofstream file(path);
        bool writable = file.good();
        file.close();
        if (writable && exists(path)) {
            removeFile(path); // 清理测试文件
        }
        return writable;
    }
}

size_t FileSystem::getFileSize(const CHTL::String& path) const {
    try {
        return std::filesystem::file_size(path);
    } catch (...) {
        return 0;
    }
}

std::time_t FileSystem::getLastModified(const CHTL::String& path) const {
    try {
        auto ftime = std::filesystem::last_write_time(path);
        return std::chrono::duration_cast<std::chrono::seconds>(
            ftime.time_since_epoch()).count();
    } catch (...) {
        return 0;
    }
}

CHTL::String FileSystem::getAbsolutePath(const CHTL::String& path) const {
    try {
        return std::filesystem::absolute(path).string();
    } catch (...) {
        return path;
    }
}

CHTL::String FileSystem::getFileName(const CHTL::String& path) const {
    try {
        return std::filesystem::path(path).filename().string();
    } catch (...) {
        size_t pos = path.find_last_of("/\\");
        return pos != CHTL::String::npos ? path.substr(pos + 1) : path;
    }
}

CHTL::String FileSystem::getDirectoryName(const CHTL::String& path) const {
    try {
        return std::filesystem::path(path).parent_path().string();
    } catch (...) {
        size_t pos = path.find_last_of("/\\");
        return pos != CHTL::String::npos ? path.substr(0, pos) : "";
    }
}

CHTL::String FileSystem::getFileExtension(const CHTL::String& path) const {
    try {
        return std::filesystem::path(path).extension().string();
    } catch (...) {
        size_t pos = path.find_last_of('.');
        return pos != CHTL::String::npos ? path.substr(pos) : "";
    }
}

bool FileSystem::createDirectory(const CHTL::String& path) const {
    try {
        return std::filesystem::create_directory(path);
    } catch (...) {
        return false;
    }
}

bool FileSystem::createDirectories(const CHTL::String& path) const {
    try {
        return std::filesystem::create_directories(path);
    } catch (...) {
        return false;
    }
}

bool FileSystem::removeDirectory(const CHTL::String& path) const {
    try {
        return std::filesystem::remove_all(path) > 0;
    } catch (...) {
        return false;
    }
}

CHTL::StringVector FileSystem::listDirectory(const CHTL::String& path) const {
    CHTL::StringVector items;
    try {
        for (const auto& entry : std::filesystem::directory_iterator(path)) {
            items.push_back(entry.path().filename().string());
        }
    } catch (...) {
        // 返回空列表
    }
    return items;
}

CHTL::StringVector FileSystem::listFiles(const CHTL::String& path, const CHTL::String& pattern) const {
    CHTL::StringVector files;
    try {
        for (const auto& entry : std::filesystem::directory_iterator(path)) {
            if (entry.is_regular_file()) {
                CHTL::String filename = entry.path().filename().string();
                // 简单的通配符匹配
                if (pattern == "*" || filename.find(pattern) != CHTL::String::npos) {
                    files.push_back(filename);
                }
            }
        }
    } catch (...) {
        // 返回空列表
    }
    return files;
}

bool FileSystem::copyFile(const CHTL::String& source, const CHTL::String& destination) const {
    try {
        return std::filesystem::copy_file(source, destination, 
                                        std::filesystem::copy_options::overwrite_existing);
    } catch (...) {
        return false;
    }
}

bool FileSystem::moveFile(const CHTL::String& source, const CHTL::String& destination) const {
    try {
        std::filesystem::rename(source, destination);
        return true;
    } catch (...) {
        return false;
    }
}

bool FileSystem::removeFile(const CHTL::String& path) const {
    try {
        return std::filesystem::remove(path);
    } catch (...) {
        return false;
    }
}

bool FileSystem::createFile(const CHTL::String& path) const {
    std::ofstream file(path);
    return file.good();
}

CHTL::String FileSystem::createTempFile(const CHTL::String& prefix) const {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1000, 9999);
    
    CHTL::String tempPath;
    
#ifdef _WIN32
    char* tempDir = nullptr;
    size_t len = 0;
    if (_dupenv_s(&tempDir, &len, "TEMP") == 0 && tempDir != nullptr) {
        tempPath = String(tempDir) + "\\" + prefix + std::to_string(dis(gen)) + ".tmp";
        free(tempDir);
    } else {
        tempPath = "C:\\Temp\\" + prefix + std::to_string(dis(gen)) + ".tmp";
    }
#else
    tempPath = "/tmp/" + prefix + std::to_string(dis(gen)) + ".tmp";
#endif
    
    return tempPath;
}

CHTL::String FileSystem::createTempDirectory(const CHTL::String& prefix) const {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1000, 9999);
    
    CHTL::String tempPath;
    
#ifdef _WIN32
    char* tempDir = nullptr;
    size_t len = 0;
    if (_dupenv_s(&tempDir, &len, "TEMP") == 0 && tempDir != nullptr) {
        tempPath = String(tempDir) + "\\" + prefix + std::to_string(dis(gen));
        free(tempDir);
    } else {
        tempPath = "C:\\Temp\\" + prefix + std::to_string(dis(gen));
    }
#else
    tempPath = "/tmp/" + prefix + std::to_string(dis(gen));
#endif
    
    if (createDirectories(tempPath)) {
        return tempPath;
    }
    
    return "";
}

// 静态工具方法
CHTL::String FileSystem::joinPath(const CHTL::String& path1, const CHTL::String& path2) {
    if (path1.empty()) return path2;
    if (path2.empty()) return path1;
    
    char lastChar = path1.back();
    if (lastChar != '/' && lastChar != '\\') {
#ifdef _WIN32
        return path1 + "\\" + path2;
#else
        return path1 + "/" + path2;
#endif
    }
    return path1 + path2;
}

CHTL::String FileSystem::joinPath(const CHTL::StringVector& paths) {
    if (paths.empty()) return "";
    
    CHTL::String result = paths[0];
    for (size_t i = 1; i < paths.size(); ++i) {
        result = joinPath(result, paths[i]);
    }
    return result;
}

CHTL::String FileSystem::normalizePath(const CHTL::String& path) {
    try {
        return std::filesystem::path(path).lexically_normal().string();
    } catch (...) {
        return path;
    }
}

// CrossPlatformFileSystem实现
CrossPlatformFileSystem::CrossPlatformFileSystem() {
    initializePlatform();
}

CrossPlatformFileSystem::~CrossPlatformFileSystem() = default;

void CrossPlatformFileSystem::initializePlatform() {
    // 平台特定的初始化
#ifdef _WIN32
    // Windows特定初始化
#else
    // Unix/Linux特定初始化
#endif
}

bool CrossPlatformFileSystem::isWindows() const {
#ifdef _WIN32
    return true;
#else
    return false;
#endif
}

bool CrossPlatformFileSystem::isUnix() const {
    return !isWindows();
}

bool CrossPlatformFileSystem::exists(const CHTL::String& path) const {
    return FileSystem::exists(getPlatformPath(path));
}

bool CrossPlatformFileSystem::isFile(const CHTL::String& path) const {
    return FileSystem::isFile(getPlatformPath(path));
}

bool CrossPlatformFileSystem::isDirectory(const CHTL::String& path) const {
    return FileSystem::isDirectory(getPlatformPath(path));
}

bool CrossPlatformFileSystem::isReadable(const CHTL::String& path) const {
    return FileSystem::isReadable(getPlatformPath(path));
}

bool CrossPlatformFileSystem::isWritable(const CHTL::String& path) const {
    return FileSystem::isWritable(getPlatformPath(path));
}

size_t CrossPlatformFileSystem::getFileSize(const CHTL::String& path) const {
    return FileSystem::getFileSize(getPlatformPath(path));
}

std::time_t CrossPlatformFileSystem::getLastModified(const CHTL::String& path) const {
    return FileSystem::getLastModified(getPlatformPath(path));
}

CHTL::String CrossPlatformFileSystem::getAbsolutePath(const CHTL::String& path) const {
    return FileSystem::getAbsolutePath(getPlatformPath(path));
}

bool CrossPlatformFileSystem::createDirectory(const CHTL::String& path) const {
    return FileSystem::createDirectory(getPlatformPath(path));
}

bool CrossPlatformFileSystem::createDirectories(const CHTL::String& path) const {
    return FileSystem::createDirectories(getPlatformPath(path));
}

bool CrossPlatformFileSystem::removeDirectory(const CHTL::String& path) const {
    return FileSystem::removeDirectory(getPlatformPath(path));
}

CHTL::StringVector CrossPlatformFileSystem::listDirectory(const CHTL::String& path) const {
    return FileSystem::listDirectory(getPlatformPath(path));
}

CHTL::StringVector CrossPlatformFileSystem::listFiles(const CHTL::String& path, const CHTL::String& pattern) const {
    return FileSystem::listFiles(getPlatformPath(path), pattern);
}

bool CrossPlatformFileSystem::copyFile(const CHTL::String& source, const CHTL::String& destination) const {
    return FileSystem::copyFile(getPlatformPath(source), getPlatformPath(destination));
}

bool CrossPlatformFileSystem::moveFile(const CHTL::String& source, const CHTL::String& destination) const {
    return FileSystem::moveFile(getPlatformPath(source), getPlatformPath(destination));
}

bool CrossPlatformFileSystem::removeFile(const CHTL::String& path) const {
    return FileSystem::removeFile(getPlatformPath(path));
}

bool CrossPlatformFileSystem::createFile(const CHTL::String& path) const {
    return FileSystem::createFile(getPlatformPath(path));
}

CHTL::String CrossPlatformFileSystem::createTempFile(const CHTL::String& prefix) const {
    return FileSystem::createTempFile(prefix);
}

CHTL::String CrossPlatformFileSystem::createTempDirectory(const CHTL::String& prefix) const {
    return FileSystem::createTempDirectory(prefix);
}

CHTL::String CrossPlatformFileSystem::getPlatformPath(const CHTL::String& path) const {
#ifdef _WIN32
    return winPath(path);
#else
    return path;
#endif
}

#ifdef _WIN32
CHTL::String CrossPlatformFileSystem::winPath(const CHTL::String& path) const {
    CHTL::String result = path;
    std::replace(result.begin(), result.end(), '/', '\\');
    return result;
}
#endif

// FileSystemFactory实现
std::unique_ptr<FileSystem> FileSystemFactory::createDefault() {
    return std::make_unique<CrossPlatformFileSystem>();
}

std::unique_ptr<FileSystem> FileSystemFactory::createCrossPlatform() {
    return std::make_unique<CrossPlatformFileSystem>();
}

std::unique_ptr<FileSystem> FileSystemFactory::createOptimized() {
    // 在实际实现中，这里可以返回一个专门优化的文件系统实现
    return std::make_unique<CrossPlatformFileSystem>();
}

} // namespace Util