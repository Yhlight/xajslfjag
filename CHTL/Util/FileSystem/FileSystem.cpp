#include "FileSystem.h"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <regex>
#include <ctime>

#ifdef _WIN32
    #include <windows.h>
#else
    #include <unistd.h>
#endif

namespace fs = std::filesystem;

namespace CHTL {
namespace Util {

// 文件读写
std::string FileSystem::readFile(const std::string& path) {
    return readTextFile(path);
}

bool FileSystem::writeFile(const std::string& path, const std::string& content) {
    return writeTextFile(path, content);
}

bool FileSystem::appendFile(const std::string& path, const std::string& content) {
    std::ofstream file(path, std::ios::app);
    if (!file.is_open()) {
        return false;
    }
    file << content;
    file.close();
    return true;
}

// 文件/目录操作
bool FileSystem::exists(const std::string& path) {
    return fs::exists(path);
}

bool FileSystem::isFile(const std::string& path) {
    return fs::is_regular_file(path);
}

bool FileSystem::isDirectory(const std::string& path) {
    return fs::is_directory(path);
}

bool FileSystem::createDirectory(const std::string& path) {
    return fs::create_directory(path);
}

bool FileSystem::createDirectories(const std::string& path) {
    return fs::create_directories(path);
}

bool FileSystem::remove(const std::string& path) {
    return fs::remove(path);
}

bool FileSystem::removeAll(const std::string& path) {
    return fs::remove_all(path) > 0;
}

bool FileSystem::copy(const std::string& from, const std::string& to) {
    try {
        fs::copy(from, to, fs::copy_options::recursive | fs::copy_options::overwrite_existing);
        return true;
    } catch (...) {
        return false;
    }
}

bool FileSystem::move(const std::string& from, const std::string& to) {
    try {
        fs::rename(from, to);
        return true;
    } catch (...) {
        return false;
    }
}

// 路径操作
std::string FileSystem::getAbsolutePath(const std::string& path) {
    return fs::absolute(path).string();
}

std::string FileSystem::getRelativePath(const std::string& path, const std::string& base) {
    return fs::relative(path, base).string();
}

std::string FileSystem::getParentPath(const std::string& path) {
    return fs::path(path).parent_path().string();
}

std::string FileSystem::getFileName(const std::string& path) {
    return fs::path(path).filename().string();
}

std::string FileSystem::getFileNameWithoutExtension(const std::string& path) {
    return fs::path(path).stem().string();
}

std::string FileSystem::getExtension(const std::string& path) {
    return fs::path(path).extension().string();
}

std::string FileSystem::joinPath(const std::string& path1, const std::string& path2) {
    return (fs::path(path1) / path2).string();
}

std::string FileSystem::normalizePath(const std::string& path) {
    return fs::path(path).lexically_normal().string();
}

// 文件搜索
std::vector<std::string> FileSystem::listFiles(const std::string& directory) {
    std::vector<std::string> files;
    if (fs::exists(directory) && fs::is_directory(directory)) {
        for (const auto& entry : fs::directory_iterator(directory)) {
            if (fs::is_regular_file(entry.status())) {
                files.push_back(entry.path().string());
            }
        }
    }
    return files;
}

std::vector<std::string> FileSystem::listDirectories(const std::string& directory) {
    std::vector<std::string> dirs;
    if (fs::exists(directory) && fs::is_directory(directory)) {
        for (const auto& entry : fs::directory_iterator(directory)) {
            if (fs::is_directory(entry.status())) {
                dirs.push_back(entry.path().string());
            }
        }
    }
    return dirs;
}

std::vector<std::string> FileSystem::listAll(const std::string& directory) {
    std::vector<std::string> all;
    if (fs::exists(directory) && fs::is_directory(directory)) {
        for (const auto& entry : fs::directory_iterator(directory)) {
            all.push_back(entry.path().string());
        }
    }
    return all;
}

std::vector<std::string> FileSystem::findFiles(const std::string& directory, const std::string& pattern, bool recursive) {
    std::vector<std::string> matches;
    std::regex regex(pattern);
    
    std::function<void(const fs::path&)> search = [&](const fs::path& dir) {
        for (const auto& entry : fs::directory_iterator(dir)) {
            if (fs::is_regular_file(entry.status())) {
                std::string filename = entry.path().filename().string();
                if (std::regex_match(filename, regex)) {
                    matches.push_back(entry.path().string());
                }
            } else if (recursive && fs::is_directory(entry.status())) {
                search(entry.path());
            }
        }
    };
    
    if (fs::exists(directory) && fs::is_directory(directory)) {
        search(directory);
    }
    
    return matches;
}

std::vector<std::string> FileSystem::findFilesByExtension(const std::string& directory, const std::string& extension, bool recursive) {
    std::vector<std::string> matches;
    std::string ext = extension;
    if (!ext.empty() && ext[0] != '.') {
        ext = "." + ext;
    }
    
    std::function<void(const fs::path&)> search = [&](const fs::path& dir) {
        for (const auto& entry : fs::directory_iterator(dir)) {
            if (fs::is_regular_file(entry.status())) {
                if (entry.path().extension() == ext) {
                    matches.push_back(entry.path().string());
                }
            } else if (recursive && fs::is_directory(entry.status())) {
                search(entry.path());
            }
        }
    };
    
    if (fs::exists(directory) && fs::is_directory(directory)) {
        search(directory);
    }
    
    return matches;
}

// 文件类型判断
FileType FileSystem::getFileType(const std::string& path) {
    std::string ext = getExtension(path);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == ".chtl") return FileType::CHTL;
    if (ext == ".html" || ext == ".htm") return FileType::HTML;
    if (ext == ".css") return FileType::CSS;
    if (ext == ".js") return FileType::JS;
    if (ext == ".cmod") return FileType::CMOD;
    if (ext == ".cjmod") return FileType::CJMOD;
    
    return FileType::UNKNOWN;
}

bool FileSystem::isCHTLFile(const std::string& path) {
    return getFileType(path) == FileType::CHTL;
}

bool FileSystem::isHTMLFile(const std::string& path) {
    return getFileType(path) == FileType::HTML;
}

bool FileSystem::isCSSFile(const std::string& path) {
    return getFileType(path) == FileType::CSS;
}

bool FileSystem::isJSFile(const std::string& path) {
    return getFileType(path) == FileType::JS;
}

bool FileSystem::isCMODFile(const std::string& path) {
    return getFileType(path) == FileType::CMOD;
}

bool FileSystem::isCJMODFile(const std::string& path) {
    return getFileType(path) == FileType::CJMOD;
}

// 文件信息
FileInfo FileSystem::getFileInfo(const std::string& path) {
    FileInfo info;
    info.path = path;
    info.name = getFileName(path);
    info.extension = getExtension(path);
    info.type = getFileType(path);
    info.exists = exists(path);
    
    if (info.exists && isFile(path)) {
        info.size = getFileSize(path);
    }
    
    return info;
}

size_t FileSystem::getFileSize(const std::string& path) {
    if (exists(path) && isFile(path)) {
        return fs::file_size(path);
    }
    return 0;
}

time_t FileSystem::getLastModifiedTime(const std::string& path) {
    if (exists(path)) {
        auto ftime = fs::last_write_time(path);
        // C++17/20 兼容性处理
        auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(ftime - fs::file_time_type::clock::now() + std::chrono::system_clock::now());
        return std::chrono::system_clock::to_time_t(sctp);
    }
    return 0;
}

// 模块路径处理
std::string FileSystem::getOfficialModulePath() {
    // 获取编译器所在目录的module文件夹
    char buffer[1024];
    #ifdef _WIN32
        GetModuleFileName(NULL, buffer, sizeof(buffer));
    #else
        ssize_t len = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
        if (len != -1) {
            buffer[len] = '\0';
        } else {
            // 如果readlink失败，使用当前工作目录
            getcwd(buffer, sizeof(buffer));
        }
    #endif
    
    fs::path exePath(buffer);
    return (exePath.parent_path() / "module").string();
}

std::string FileSystem::getUserModulePath(const std::string& workingDir) {
    return joinPath(workingDir, "module");
}

std::vector<std::string> FileSystem::getModuleSearchPaths(const std::string& workingDir) {
    std::vector<std::string> paths;
    
    // 1. 官方模块目录
    paths.push_back(getOfficialModulePath());
    
    // 2. 当前目录的module文件夹
    std::string userModulePath = getUserModulePath(workingDir);
    if (exists(userModulePath)) {
        paths.push_back(userModulePath);
    }
    
    // 3. 当前目录
    paths.push_back(workingDir);
    
    return paths;
}

std::string FileSystem::findModule(const std::string& moduleName, const std::string& workingDir) {
    auto searchPaths = getModuleSearchPaths(workingDir);
    
    for (const auto& searchPath : searchPaths) {
        // 优先查找.cmod文件
        std::string cmodPath = joinPath(searchPath, moduleName + ".cmod");
        if (exists(cmodPath)) {
            return cmodPath;
        }
        
        // 其次查找.chtl文件
        std::string chtlPath = joinPath(searchPath, moduleName + ".chtl");
        if (exists(chtlPath)) {
            return chtlPath;
        }
        
        // 查找目录形式的模块
        std::string moduleDirPath = joinPath(searchPath, moduleName);
        if (exists(moduleDirPath) && isDirectory(moduleDirPath)) {
            // 查找主模块文件
            std::string mainChtl = joinPath(joinPath(moduleDirPath, "src"), moduleName + ".chtl");
            if (exists(mainChtl)) {
                return moduleDirPath;
            }
        }
    }
    
    return "";
}

// CMOD/CJMOD打包解包（简化实现，实际需要更复杂的打包格式）
bool FileSystem::packCMOD(const std::string& sourceDir, const std::string& outputPath) {
    // TODO: 实现CMOD打包逻辑
    // 这里需要实现将目录打包成.cmod文件的逻辑
    // 可能需要使用zip库或自定义的打包格式
    (void)sourceDir;
    (void)outputPath;
    return false;
}

bool FileSystem::unpackCMOD(const std::string& cmodPath, const std::string& outputDir) {
    // TODO: 实现CMOD解包逻辑
    (void)cmodPath;
    (void)outputDir;
    return false;
}

bool FileSystem::packCJMOD(const std::string& sourceDir, const std::string& outputPath) {
    // TODO: 实现CJMOD打包逻辑
    (void)sourceDir;
    (void)outputPath;
    return false;
}

bool FileSystem::unpackCJMOD(const std::string& cjmodPath, const std::string& outputDir) {
    // TODO: 实现CJMOD解包逻辑
    (void)cjmodPath;
    (void)outputDir;
    return false;
}

// 内部辅助方法
std::string FileSystem::readTextFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        return "";
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();
    
    return buffer.str();
}

bool FileSystem::writeTextFile(const std::string& path, const std::string& content) {
    // 确保父目录存在
    fs::path filePath(path);
    fs::path parentPath = filePath.parent_path();
    if (!parentPath.empty() && !exists(parentPath.string())) {
        createDirectories(parentPath.string());
    }
    
    std::ofstream file(path);
    if (!file.is_open()) {
        return false;
    }
    
    file << content;
    file.close();
    
    return true;
}

std::vector<std::string> FileSystem::glob(const std::string& pattern) {
    // TODO: 实现glob模式匹配
    (void)pattern;
    return {};
}

} // namespace Util
} // namespace CHTL