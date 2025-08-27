#pragma once

#include <string>
#include <vector>
#include <filesystem>

namespace CHTL {

class FileSystem {
public:
    // 读取文件
    static std::string ReadFile(const std::string& path);
    
    // 写入文件
    static void WriteFile(const std::string& path, const std::string& content);
    
    // 检查文件是否存在
    static bool FileExists(const std::string& path);
    
    // 检查目录是否存在
    static bool DirectoryExists(const std::string& path);
    
    // 创建目录
    static bool CreateDirectory(const std::string& path);
    
    // 获取文件扩展名
    static std::string GetExtension(const std::string& path);
    
    // 获取文件名（不含路径）
    static std::string GetFilename(const std::string& path);
    
    // 获取文件名（不含扩展名）
    static std::string GetStem(const std::string& path);
    
    // 获取目录路径
    static std::string GetDirectory(const std::string& path);
    
    // 获取绝对路径
    static std::string GetAbsolutePath(const std::string& path);
    
    // 规范化路径
    static std::string NormalizePath(const std::string& path);
    
    // 连接路径
    static std::string JoinPath(const std::string& path1, const std::string& path2);
    
    // 列出目录中的文件
    static std::vector<std::string> ListFiles(const std::string& directory, 
                                              const std::string& extension = "");
    
    // 递归列出目录中的文件
    static std::vector<std::string> ListFilesRecursive(const std::string& directory,
                                                       const std::string& extension = "");
    
    // 复制文件
    static bool CopyFile(const std::string& source, const std::string& destination);
    
    // 移动文件
    static bool MoveFile(const std::string& source, const std::string& destination);
    
    // 删除文件
    static bool DeleteFile(const std::string& path);
    
    // 获取文件大小
    static size_t GetFileSize(const std::string& path);
    
    // 获取文件修改时间
    static std::time_t GetModificationTime(const std::string& path);
    
    // 搜索文件
    static std::string SearchFile(const std::string& filename, 
                                  const std::vector<std::string>& searchPaths);
    
    // 模块路径处理
    static std::string GetOfficialModulePath();
    static std::string GetLocalModulePath(const std::string& currentPath);
    static std::vector<std::string> GetDefaultModulePaths(const std::string& currentPath);
};

} // namespace CHTL