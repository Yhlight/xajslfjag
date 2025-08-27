#include "FileSystem.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <stdexcept>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <limits.h>
#endif

namespace CHTL {

namespace fs = std::filesystem;

std::string FileSystem::ReadFile(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("无法打开文件: " + path);
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void FileSystem::WriteFile(const std::string& path, const std::string& content) {
    // 确保目录存在
    fs::path filePath(path);
    if (filePath.has_parent_path()) {
        fs::create_directories(filePath.parent_path());
    }
    
    std::ofstream file(path, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("无法写入文件: " + path);
    }
    
    file << content;
    file.close();
}

bool FileSystem::FileExists(const std::string& path) {
    return fs::exists(path) && fs::is_regular_file(path);
}

bool FileSystem::DirectoryExists(const std::string& path) {
    return fs::exists(path) && fs::is_directory(path);
}

bool FileSystem::CreateDirectory(const std::string& path) {
    return fs::create_directories(path);
}

std::string FileSystem::GetExtension(const std::string& path) {
    return fs::path(path).extension().string();
}

std::string FileSystem::GetFilename(const std::string& path) {
    return fs::path(path).filename().string();
}

std::string FileSystem::GetStem(const std::string& path) {
    return fs::path(path).stem().string();
}

std::string FileSystem::GetDirectory(const std::string& path) {
    return fs::path(path).parent_path().string();
}

std::string FileSystem::GetAbsolutePath(const std::string& path) {
    return fs::absolute(path).string();
}

std::string FileSystem::NormalizePath(const std::string& path) {
    fs::path p(path);
    p = p.lexically_normal();
    
    // 将路径分隔符统一为/
    std::string result = p.string();
    std::replace(result.begin(), result.end(), '\\', '/');
    return result;
}

std::string FileSystem::JoinPath(const std::string& path1, const std::string& path2) {
    fs::path p1(path1);
    fs::path p2(path2);
    return (p1 / p2).string();
}

std::vector<std::string> FileSystem::ListFiles(const std::string& directory, 
                                               const std::string& extension) {
    std::vector<std::string> files;
    
    if (!DirectoryExists(directory)) {
        return files;
    }
    
    for (const auto& entry : fs::directory_iterator(directory)) {
        if (entry.is_regular_file()) {
            if (extension.empty() || entry.path().extension() == extension) {
                files.push_back(entry.path().string());
            }
        }
    }
    
    return files;
}

std::vector<std::string> FileSystem::ListFilesRecursive(const std::string& directory,
                                                        const std::string& extension) {
    std::vector<std::string> files;
    
    if (!DirectoryExists(directory)) {
        return files;
    }
    
    for (const auto& entry : fs::recursive_directory_iterator(directory)) {
        if (entry.is_regular_file()) {
            if (extension.empty() || entry.path().extension() == extension) {
                files.push_back(entry.path().string());
            }
        }
    }
    
    return files;
}

bool FileSystem::CopyFile(const std::string& source, const std::string& destination) {
    try {
        // 确保目标目录存在
        fs::path destPath(destination);
        if (destPath.has_parent_path()) {
            fs::create_directories(destPath.parent_path());
        }
        
        fs::copy_file(source, destination, fs::copy_options::overwrite_existing);
        return true;
    } catch (...) {
        return false;
    }
}

bool FileSystem::MoveFile(const std::string& source, const std::string& destination) {
    try {
        // 确保目标目录存在
        fs::path destPath(destination);
        if (destPath.has_parent_path()) {
            fs::create_directories(destPath.parent_path());
        }
        
        fs::rename(source, destination);
        return true;
    } catch (...) {
        return false;
    }
}

bool FileSystem::DeleteFile(const std::string& path) {
    try {
        return fs::remove(path);
    } catch (...) {
        return false;
    }
}

size_t FileSystem::GetFileSize(const std::string& path) {
    if (!FileExists(path)) {
        return 0;
    }
    return fs::file_size(path);
}

std::time_t FileSystem::GetModificationTime(const std::string& path) {
    if (!fs::exists(path)) {
        return 0;
    }
    
    auto ftime = fs::last_write_time(path);
    auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
        ftime - fs::file_time_type::clock::now() + std::chrono::system_clock::now()
    );
    return std::chrono::system_clock::to_time_t(sctp);
}

std::string FileSystem::SearchFile(const std::string& filename, 
                                  const std::vector<std::string>& searchPaths) {
    for (const auto& searchPath : searchPaths) {
        std::string fullPath = JoinPath(searchPath, filename);
        if (FileExists(fullPath)) {
            return fullPath;
        }
    }
    return "";
}

std::string FileSystem::GetOfficialModulePath() {
    // 获取编译器可执行文件所在目录
#ifdef _WIN32
    char buffer[MAX_PATH];
    GetModuleFileNameA(NULL, buffer, MAX_PATH);
    fs::path exePath(buffer);
#else
    char buffer[PATH_MAX];
    ssize_t len = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
    if (len != -1) {
        buffer[len] = '\0';
    }
    fs::path exePath(buffer);
#endif
    
    // 官方模块目录在编译器同级目录下的module文件夹
    return (exePath.parent_path() / "module").string();
}

std::string FileSystem::GetLocalModulePath(const std::string& currentPath) {
    fs::path current(currentPath);
    if (current.is_relative()) {
        current = fs::absolute(current);
    }
    
    // 如果是文件，获取其目录
    if (fs::is_regular_file(current)) {
        current = current.parent_path();
    }
    
    return (current / "module").string();
}

std::vector<std::string> FileSystem::GetDefaultModulePaths(const std::string& currentPath) {
    std::vector<std::string> paths;
    
    // 1. 官方模块目录
    paths.push_back(GetOfficialModulePath());
    
    // 2. 当前目录的module文件夹
    paths.push_back(GetLocalModulePath(currentPath));
    
    // 3. 当前目录
    fs::path current(currentPath);
    if (fs::is_regular_file(current)) {
        current = current.parent_path();
    }
    paths.push_back(current.string());
    
    return paths;
}

} // namespace CHTL