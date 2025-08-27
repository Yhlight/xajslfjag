#pragma once

#include <string>
#include <vector>
#include <fstream>

namespace CHTL {
namespace FileSystem {

/// 检查文件是否存在
bool FileExists(const std::string& filePath);

/// 检查目录是否存在
bool DirectoryExists(const std::string& dirPath);

/// 创建目录
bool CreateDirectory(const std::string& dirPath);

/// 获取文件大小
size_t GetFileSize(const std::string& filePath);

/// 读取文件内容
std::string ReadTextFile(const std::string& filePath);

/// 写入文件内容
bool WriteTextFile(const std::string& filePath, const std::string& content);

/// 获取文件扩展名
std::string GetFileExtension(const std::string& filePath);

/// 获取文件名（不含路径）
std::string GetFileName(const std::string& filePath);

/// 获取目录路径
std::string GetDirectoryPath(const std::string& filePath);

/// 连接路径
std::string JoinPath(const std::string& path1, const std::string& path2);

/// 规范化路径
std::string NormalizePath(const std::string& path);

/// 获取当前工作目录
std::string GetCurrentDirectory();

/// 列出目录中的文件
std::vector<std::string> ListFiles(const std::string& dirPath, const std::string& extension = "");

} // namespace FileSystem
} // namespace CHTL