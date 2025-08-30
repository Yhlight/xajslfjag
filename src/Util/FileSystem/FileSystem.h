#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <memory>
#include <chrono>
#include <functional>

namespace CHTL {
namespace Util {

/**
 * 文件类型枚举
 */
enum class FileType {
    REGULAR_FILE,       // 普通文件
    DIRECTORY,          // 目录
    SYMBOLIC_LINK,      // 符号链接
    UNKNOWN             // 未知类型
};

/**
 * 文件权限枚举
 */
enum class FilePermission {
    READ = 0x01,        // 读权限
    WRITE = 0x02,       // 写权限
    EXECUTE = 0x04,     // 执行权限
    ALL = 0x07          // 所有权限
};

/**
 * 文件信息结构
 */
struct FileInfo {
    std::string path;                   // 文件路径
    std::string name;                   // 文件名
    std::string extension;              // 文件扩展名
    FileType type;                      // 文件类型
    size_t size;                        // 文件大小（字节）
    std::chrono::system_clock::time_point lastModified; // 最后修改时间
    std::chrono::system_clock::time_point lastAccessed; // 最后访问时间
    int permissions;                    // 文件权限
    
    FileInfo() : type(FileType::UNKNOWN), size(0), permissions(0) {}
};

/**
 * 路径操作工具类
 */
class Path {
public:
    /**
     * 规范化路径
     */
    static std::string normalize(const std::string& path);
    
    /**
     * 连接路径
     */
    static std::string join(const std::string& path1, const std::string& path2);
    
    /**
     * 连接多个路径
     */
    static std::string join(const std::vector<std::string>& paths);
    
    /**
     * 获取父目录
     */
    static std::string getParent(const std::string& path);
    
    /**
     * 获取文件名
     */
    static std::string getFileName(const std::string& path);
    
    /**
     * 获取文件名（不含扩展名）
     */
    static std::string getBaseName(const std::string& path);
    
    /**
     * 获取文件扩展名
     */
    static std::string getExtension(const std::string& path);
    
    /**
     * 检查路径是否为绝对路径
     */
    static bool isAbsolute(const std::string& path);
    
    /**
     * 检查路径是否为相对路径
     */
    static bool isRelative(const std::string& path);
    
    /**
     * 转换为绝对路径
     */
    static std::string toAbsolute(const std::string& path);
    
    /**
     * 转换为相对路径
     */
    static std::string toRelative(const std::string& path, const std::string& base = ".");
    
    /**
     * 获取当前工作目录
     */
    static std::string getCurrentDirectory();
    
    /**
     * 获取系统临时目录
     */
    static std::string getTempDirectory();
    
    /**
     * 获取用户主目录
     */
    static std::string getHomeDirectory();

private:
    /**
     * 路径分隔符
     */
    static const char SEPARATOR;
    
    /**
     * 分割路径
     */
    static std::vector<std::string> splitPath(const std::string& path);
};

/**
 * 文件系统操作类
 */
class FileSystem {
public:
    /**
     * 检查文件是否存在
     */
    static bool exists(const std::string& path);
    
    /**
     * 检查是否为文件
     */
    static bool isFile(const std::string& path);
    
    /**
     * 检查是否为目录
     */
    static bool isDirectory(const std::string& path);
    
    /**
     * 检查是否为符号链接
     */
    static bool isSymbolicLink(const std::string& path);
    
    /**
     * 获取文件信息
     */
    static FileInfo getFileInfo(const std::string& path);
    
    /**
     * 获取文件大小
     */
    static size_t getFileSize(const std::string& path);
    
    /**
     * 获取最后修改时间
     */
    static std::chrono::system_clock::time_point getLastModified(const std::string& path);
    
    /**
     * 创建目录
     */
    static bool createDirectory(const std::string& path, bool recursive = false);
    
    /**
     * 创建文件
     */
    static bool createFile(const std::string& path);
    
    /**
     * 删除文件
     */
    static bool deleteFile(const std::string& path);
    
    /**
     * 删除目录
     */
    static bool deleteDirectory(const std::string& path, bool recursive = false);
    
    /**
     * 复制文件
     */
    static bool copyFile(const std::string& source, const std::string& destination);
    
    /**
     * 复制目录
     */
    static bool copyDirectory(const std::string& source, const std::string& destination);
    
    /**
     * 移动文件或目录
     */
    static bool move(const std::string& source, const std::string& destination);
    
    /**
     * 重命名文件或目录
     */
    static bool rename(const std::string& oldPath, const std::string& newPath);
    
    /**
     * 列出目录内容
     */
    static std::vector<std::string> listDirectory(const std::string& path);
    
    /**
     * 列出目录内容（详细信息）
     */
    static std::vector<FileInfo> listDirectoryDetailed(const std::string& path);
    
    /**
     * 递归列出目录内容
     */
    static std::vector<std::string> listDirectoryRecursive(const std::string& path);
    
    /**
     * 查找文件
     */
    static std::vector<std::string> findFiles(const std::string& directory, 
                                            const std::string& pattern = "*");
    
    /**
     * 查找文件（使用谓词）
     */
    static std::vector<std::string> findFiles(const std::string& directory,
                                            std::function<bool(const FileInfo&)> predicate);
    
    /**
     * 获取可用磁盘空间
     */
    static size_t getAvailableSpace(const std::string& path);
    
    /**
     * 获取总磁盘空间
     */
    static size_t getTotalSpace(const std::string& path);
    
    /**
     * 设置文件权限
     */
    static bool setPermissions(const std::string& path, int permissions);
    
    /**
     * 获取文件权限
     */
    static int getPermissions(const std::string& path);
};

/**
 * 文件读取器
 */
class FileReader {
public:
    /**
     * 构造函数
     */
    explicit FileReader(const std::string& filePath);
    
    /**
     * 析构函数
     */
    ~FileReader();
    
    /**
     * 打开文件
     */
    bool open();
    
    /**
     * 关闭文件
     */
    void close();
    
    /**
     * 检查是否已打开
     */
    bool isOpen() const;
    
    /**
     * 检查是否到达文件末尾
     */
    bool isEOF() const;
    
    /**
     * 读取整个文件内容
     */
    std::string readAll();
    
    /**
     * 读取一行
     */
    std::string readLine();
    
    /**
     * 读取指定字节数
     */
    std::string readBytes(size_t count);
    
    /**
     * 读取字符
     */
    char readChar();
    
    /**
     * 获取当前位置
     */
    size_t getPosition() const;
    
    /**
     * 设置位置
     */
    bool setPosition(size_t position);
    
    /**
     * 获取文件大小
     */
    size_t getSize() const;

private:
    std::string m_filePath;
    std::unique_ptr<std::ifstream> m_stream;
};

/**
 * 文件写入器
 */
class FileWriter {
public:
    /**
     * 写入模式
     */
    enum class WriteMode {
        OVERWRITE,      // 覆盖写入
        APPEND          // 追加写入
    };
    
    /**
     * 构造函数
     */
    explicit FileWriter(const std::string& filePath, WriteMode mode = WriteMode::OVERWRITE);
    
    /**
     * 析构函数
     */
    ~FileWriter();
    
    /**
     * 打开文件
     */
    bool open();
    
    /**
     * 关闭文件
     */
    void close();
    
    /**
     * 检查是否已打开
     */
    bool isOpen() const;
    
    /**
     * 写入字符串
     */
    bool write(const std::string& content);
    
    /**
     * 写入一行
     */
    bool writeLine(const std::string& line);
    
    /**
     * 写入字节
     */
    bool writeBytes(const char* data, size_t size);
    
    /**
     * 写入字符
     */
    bool writeChar(char c);
    
    /**
     * 刷新缓冲区
     */
    void flush();
    
    /**
     * 获取当前位置
     */
    size_t getPosition() const;

private:
    std::string m_filePath;
    WriteMode m_mode;
    std::unique_ptr<std::ofstream> m_stream;
};

/**
 * 目录监视器
 */
class DirectoryWatcher {
public:
    /**
     * 文件系统事件类型
     */
    enum class EventType {
        FILE_CREATED,       // 文件创建
        FILE_MODIFIED,      // 文件修改
        FILE_DELETED,       // 文件删除
        FILE_RENAMED        // 文件重命名
    };
    
    /**
     * 文件系统事件
     */
    struct Event {
        EventType type;
        std::string path;
        std::string oldPath;    // 用于重命名事件
        std::chrono::system_clock::time_point timestamp;
    };
    
    /**
     * 事件回调函数类型
     */
    using EventCallback = std::function<void(const Event&)>;
    
    /**
     * 构造函数
     */
    explicit DirectoryWatcher(const std::string& directoryPath);
    
    /**
     * 析构函数
     */
    ~DirectoryWatcher();
    
    /**
     * 开始监视
     */
    bool startWatching();
    
    /**
     * 停止监视
     */
    void stopWatching();
    
    /**
     * 检查是否正在监视
     */
    bool isWatching() const;
    
    /**
     * 设置事件回调
     */
    void setEventCallback(EventCallback callback);
    
    /**
     * 设置递归监视
     */
    void setRecursive(bool recursive);

private:
    std::string m_directoryPath;
    bool m_isWatching;
    bool m_recursive;
    EventCallback m_callback;
    
    // 平台相关的实现
    void* m_platformData;
    
    /**
     * 监视线程函数
     */
    void watchThread();
};

/**
 * 临时文件管理器
 */
class TemporaryFile {
public:
    /**
     * 构造函数
     */
    TemporaryFile(const std::string& prefix = "tmp", const std::string& suffix = "");
    
    /**
     * 析构函数（自动删除临时文件）
     */
    ~TemporaryFile();
    
    /**
     * 获取临时文件路径
     */
    const std::string& getPath() const;
    
    /**
     * 写入内容
     */
    bool writeContent(const std::string& content);
    
    /**
     * 读取内容
     */
    std::string readContent();
    
    /**
     * 手动删除
     */
    void remove();

private:
    std::string m_path;
    bool m_deleted;
    
    /**
     * 生成唯一文件名
     */
    std::string generateUniqueName(const std::string& prefix, const std::string& suffix);
};

} // namespace Util
} // namespace CHTL