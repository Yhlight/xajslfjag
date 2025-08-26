#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <memory>
#include <functional>
#include <chrono>
#include <cstdint>

#ifdef _WIN32
    #include <windows.h>
    #include <direct.h>
    #include <io.h>
#else
    #include <unistd.h>
    #include <dirent.h>
    #include <sys/stat.h>
    #include <sys/types.h>
#endif

namespace CHTL {
namespace Core {

/**
 * FileType - 文件类型枚举
 */
enum class FileType {
    REGULAR_FILE,     // 普通文件
    DIRECTORY,        // 目录
    SYMBOLIC_LINK,    // 符号链接
    UNKNOWN          // 未知类型
};

/**
 * FilePermissions - 文件权限
 */
struct FilePermissions {
    bool readable;
    bool writable;
    bool executable;
    
    FilePermissions() : readable(false), writable(false), executable(false) {}
    FilePermissions(bool r, bool w, bool x) : readable(r), writable(w), executable(x) {}
};

/**
 * FileInfo - 文件信息结构体
 */
struct FileInfo {
    std::string name;                              // 文件名
    std::string path;                              // 完整路径
    std::string extension;                         // 文件扩展名
    FileType type;                                 // 文件类型
    uint64_t size;                                 // 文件大小（字节）
    std::chrono::system_clock::time_point created_time;   // 创建时间
    std::chrono::system_clock::time_point modified_time;  // 修改时间
    std::chrono::system_clock::time_point accessed_time;  // 访问时间
    FilePermissions permissions;                   // 文件权限
    
    FileInfo() : type(FileType::UNKNOWN), size(0) {}
};

/**
 * DirectoryEntry - 目录条目
 */
struct DirectoryEntry {
    std::string name;        // 条目名称
    std::string full_path;   // 完整路径
    FileType type;           // 条目类型
    
    DirectoryEntry() : type(FileType::UNKNOWN) {}
    DirectoryEntry(const std::string& n, const std::string& p, FileType t)
        : name(n), full_path(p), type(t) {}
};

/**
 * FileOperationResult - 文件操作结果
 */
struct FileOperationResult {
    bool success;                        // 是否成功
    std::string error_message;           // 错误信息
    std::vector<std::string> warnings;   // 警告信息
    
    FileOperationResult() : success(false) {}
    FileOperationResult(bool s) : success(s) {}
    FileOperationResult(bool s, const std::string& error) : success(s), error_message(error) {}
};

/**
 * FileSystem - 跨平台文件系统操作类
 */
class FileSystem {
public:
    /**
     * 检查文件是否存在
     * @param path 文件路径
     * @return 是否存在
     */
    static bool exists(const std::string& path);
    
    /**
     * 检查是否为文件
     * @param path 路径
     * @return 是否为文件
     */
    static bool isFile(const std::string& path);
    
    /**
     * 检查是否为目录
     * @param path 路径
     * @return 是否为目录
     */
    static bool isDirectory(const std::string& path);
    
    /**
     * 检查是否为符号链接
     * @param path 路径
     * @return 是否为符号链接
     */
    static bool isSymbolicLink(const std::string& path);
    
    /**
     * 获取文件大小
     * @param path 文件路径
     * @return 文件大小（字节），失败返回-1
     */
    static int64_t getFileSize(const std::string& path);
    
    /**
     * 获取文件信息
     * @param path 文件路径
     * @return 文件信息
     */
    static FileInfo getFileInfo(const std::string& path);
    
    /**
     * 创建目录
     * @param path 目录路径
     * @param recursive 是否递归创建
     * @return 操作结果
     */
    static FileOperationResult createDirectory(const std::string& path, bool recursive = true);
    
    /**
     * 删除文件
     * @param path 文件路径
     * @return 操作结果
     */
    static FileOperationResult deleteFile(const std::string& path);
    
    /**
     * 删除目录
     * @param path 目录路径
     * @param recursive 是否递归删除
     * @return 操作结果
     */
    static FileOperationResult deleteDirectory(const std::string& path, bool recursive = false);
    
    /**
     * 复制文件
     * @param source 源文件路径
     * @param destination 目标文件路径
     * @param overwrite 是否覆盖现有文件
     * @return 操作结果
     */
    static FileOperationResult copyFile(const std::string& source, const std::string& destination, bool overwrite = false);
    
    /**
     * 移动文件
     * @param source 源文件路径
     * @param destination 目标文件路径
     * @param overwrite 是否覆盖现有文件
     * @return 操作结果
     */
    static FileOperationResult moveFile(const std::string& source, const std::string& destination, bool overwrite = false);
    
    /**
     * 重命名文件或目录
     * @param old_path 旧路径
     * @param new_path 新路径
     * @return 操作结果
     */
    static FileOperationResult rename(const std::string& old_path, const std::string& new_path);
    
    /**
     * 列出目录内容
     * @param path 目录路径
     * @param recursive 是否递归列出
     * @return 目录条目列表
     */
    static std::vector<DirectoryEntry> listDirectory(const std::string& path, bool recursive = false);
    
    /**
     * 读取文件内容
     * @param path 文件路径
     * @return 文件内容，失败返回空字符串
     */
    static std::string readFile(const std::string& path);
    
    /**
     * 读取二进制文件
     * @param path 文件路径
     * @return 二进制数据
     */
    static std::vector<uint8_t> readBinaryFile(const std::string& path);
    
    /**
     * 写入文件内容
     * @param path 文件路径
     * @param content 内容
     * @param append 是否追加模式
     * @return 操作结果
     */
    static FileOperationResult writeFile(const std::string& path, const std::string& content, bool append = false);
    
    /**
     * 写入二进制文件
     * @param path 文件路径
     * @param data 二进制数据
     * @param append 是否追加模式
     * @return 操作结果
     */
    static FileOperationResult writeBinaryFile(const std::string& path, const std::vector<uint8_t>& data, bool append = false);
    
    /**
     * 获取当前工作目录
     * @return 当前工作目录路径
     */
    static std::string getCurrentDirectory();
    
    /**
     * 设置当前工作目录
     * @param path 目录路径
     * @return 操作结果
     */
    static FileOperationResult setCurrentDirectory(const std::string& path);
    
    /**
     * 获取临时目录
     * @return 临时目录路径
     */
    static std::string getTempDirectory();
    
    /**
     * 获取用户主目录
     * @return 用户主目录路径
     */
    static std::string getHomeDirectory();
    
    /**
     * 获取应用程序目录
     * @return 应用程序目录路径
     */
    static std::string getApplicationDirectory();

private:
    // 内部工具方法
    static std::string normalizePath(const std::string& path);
    static std::vector<std::string> splitPath(const std::string& path);
    static std::string joinPath(const std::vector<std::string>& components);
    static FileType getFileType(const std::string& path);
    static FilePermissions getFilePermissions(const std::string& path);
    static std::chrono::system_clock::time_point getFileTime(const std::string& path, int time_type);
};

/**
 * Path - 路径操作工具类
 */
class Path {
public:
    /**
     * 规范化路径
     * @param path 原始路径
     * @return 规范化后的路径
     */
    static std::string normalize(const std::string& path);
    
    /**
     * 连接路径
     * @param base 基础路径
     * @param relative 相对路径
     * @return 连接后的路径
     */
    static std::string join(const std::string& base, const std::string& relative);
    
    /**
     * 连接多个路径组件
     * @param components 路径组件列表
     * @return 连接后的路径
     */
    static std::string join(const std::vector<std::string>& components);
    
    /**
     * 获取文件名（包含扩展名）
     * @param path 文件路径
     * @return 文件名
     */
    static std::string getFileName(const std::string& path);
    
    /**
     * 获取文件名（不包含扩展名）
     * @param path 文件路径
     * @return 文件名（无扩展名）
     */
    static std::string getFileNameWithoutExtension(const std::string& path);
    
    /**
     * 获取文件扩展名
     * @param path 文件路径
     * @return 文件扩展名（包含点号）
     */
    static std::string getExtension(const std::string& path);
    
    /**
     * 获取父目录
     * @param path 文件路径
     * @return 父目录路径
     */
    static std::string getParentDirectory(const std::string& path);
    
    /**
     * 检查路径是否为绝对路径
     * @param path 路径
     * @return 是否为绝对路径
     */
    static bool isAbsolute(const std::string& path);
    
    /**
     * 检查路径是否为相对路径
     * @param path 路径
     * @return 是否为相对路径
     */
    static bool isRelative(const std::string& path);
    
    /**
     * 转换为绝对路径
     * @param path 相对路径
     * @param base 基础路径（可选，默认使用当前目录）
     * @return 绝对路径
     */
    static std::string toAbsolute(const std::string& path, const std::string& base = "");
    
    /**
     * 转换为相对路径
     * @param path 绝对路径
     * @param base 基础路径
     * @return 相对路径
     */
    static std::string toRelative(const std::string& path, const std::string& base);
    
    /**
     * 检查路径是否有效
     * @param path 路径
     * @return 是否有效
     */
    static bool isValid(const std::string& path);
    
    /**
     * 获取路径分隔符
     * @return 当前平台的路径分隔符
     */
    static char getSeparator();
    
    /**
     * 替换路径中的分隔符
     * @param path 原始路径
     * @param separator 新分隔符
     * @return 替换后的路径
     */
    static std::string replaceSeparators(const std::string& path, char separator);

private:
    static const char SEPARATOR;
    static const char ALT_SEPARATOR;
    
    static std::vector<std::string> splitComponents(const std::string& path);
    static std::string cleanPath(const std::string& path);
    static bool isValidPathCharacter(char c);
};

/**
 * FileWatcher - 文件监视器
 */
class FileWatcher {
public:
    enum class WatchEvent {
        FILE_CREATED,
        FILE_MODIFIED,
        FILE_DELETED,
        FILE_RENAMED
    };
    
    using WatchCallback = std::function<void(const std::string&, WatchEvent)>;
    
    FileWatcher();
    ~FileWatcher();
    
    /**
     * 开始监视目录
     * @param directory 要监视的目录
     * @param callback 事件回调函数
     * @param recursive 是否递归监视
     * @return 是否成功开始监视
     */
    bool watchDirectory(const std::string& directory, WatchCallback callback, bool recursive = true);
    
    /**
     * 停止监视目录
     * @param directory 目录路径
     * @return 是否成功停止监视
     */
    bool stopWatching(const std::string& directory);
    
    /**
     * 停止所有监视
     */
    void stopAll();
    
    /**
     * 检查是否正在监视
     * @param directory 目录路径
     * @return 是否正在监视
     */
    bool isWatching(const std::string& directory) const;

private:
    struct WatcherImpl;
    std::unique_ptr<WatcherImpl> impl;
};

/**
 * FileUtils - 文件工具类
 */
class FileUtils {
public:
    /**
     * 计算文件或目录的总大小
     * @param path 路径
     * @return 总大小（字节）
     */
    static uint64_t calculateTotalSize(const std::string& path);
    
    /**
     * 查找文件
     * @param directory 搜索目录
     * @param pattern 文件名模式（支持通配符）
     * @param recursive 是否递归搜索
     * @return 找到的文件路径列表
     */
    static std::vector<std::string> findFiles(const std::string& directory, const std::string& pattern, bool recursive = true);
    
    /**
     * 比较两个文件是否相同
     * @param file1 文件1路径
     * @param file2 文件2路径
     * @return 是否相同
     */
    static bool compareFiles(const std::string& file1, const std::string& file2);
    
    /**
     * 生成唯一的临时文件名
     * @param prefix 文件名前缀
     * @param extension 文件扩展名
     * @return 唯一的临时文件路径
     */
    static std::string generateTempFileName(const std::string& prefix = "tmp", const std::string& extension = ".tmp");
    
    /**
     * 清理空目录
     * @param directory 目录路径
     * @param recursive 是否递归清理
     * @return 清理的目录数量
     */
    static int cleanEmptyDirectories(const std::string& directory, bool recursive = true);
    
    /**
     * 获取文件的MIME类型
     * @param path 文件路径
     * @return MIME类型字符串
     */
    static std::string getMimeType(const std::string& path);
    
    /**
     * 检查文件是否为文本文件
     * @param path 文件路径
     * @return 是否为文本文件
     */
    static bool isTextFile(const std::string& path);
    
    /**
     * 检查文件是否为二进制文件
     * @param path 文件路径
     * @return 是否为二进制文件
     */
    static bool isBinaryFile(const std::string& path);
    
    /**
     * 复制目录
     * @param source 源目录
     * @param destination 目标目录
     * @param overwrite 是否覆盖现有文件
     * @return 操作结果
     */
    static FileOperationResult copyDirectory(const std::string& source, const std::string& destination, bool overwrite = false);
    
    /**
     * 移动目录
     * @param source 源目录
     * @param destination 目标目录
     * @param overwrite 是否覆盖现有文件
     * @return 操作结果
     */
    static FileOperationResult moveDirectory(const std::string& source, const std::string& destination, bool overwrite = false);

private:
    static bool matchPattern(const std::string& filename, const std::string& pattern);
    static bool isTextContent(const std::vector<uint8_t>& data, size_t sample_size = 512);
};

} // namespace Core
} // namespace CHTL