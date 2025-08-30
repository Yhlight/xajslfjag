#pragma once
#include <string>
#include <vector>

namespace CHTL {
namespace Util {

/**
 * 文件系统工具类
 * 提供文件和目录操作功能
 */
class FileSystem {
public:
    /**
     * 检查文件是否存在
     */
    static bool fileExists(const std::string& path);
    
    /**
     * 检查目录是否存在
     */
    static bool directoryExists(const std::string& path);
    
    /**
     * 创建目录
     */
    static bool createDirectory(const std::string& path);
    
    /**
     * 读取文件内容
     */
    static std::string readFile(const std::string& path);
    
    /**
     * 写入文件内容
     */
    static bool writeFile(const std::string& path, const std::string& content);
    
    /**
     * 获取目录下的所有文件
     */
    static std::vector<std::string> listFiles(const std::string& directory, const std::string& extension = "");
    
    /**
     * 获取文件扩展名
     */
    static std::string getFileExtension(const std::string& path);
    
    /**
     * 获取文件名（不含路径）
     */
    static std::string getFileName(const std::string& path);
    
    /**
     * 获取目录路径
     */
    static std::string getDirectoryPath(const std::string& path);
    
    /**
     * 组合路径
     */
    static std::string combinePath(const std::string& dir, const std::string& file);

private:
    /**
     * 规范化路径
     */
    static std::string normalizePath(const std::string& path);
};

} // namespace Util
} // namespace CHTL