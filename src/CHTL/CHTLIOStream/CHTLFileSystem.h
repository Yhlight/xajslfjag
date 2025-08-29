#ifndef CHTL_FILESYSTEM_H
#define CHTL_FILESYSTEM_H

#include <string>
#include <vector>
#include <optional>
#include <functional>
#include <fstream>
#include <ctime>
#include <memory>

namespace CHTL {

// 文件类型
enum class FileType {
    Regular,      // 普通文件
    Directory,    // 目录
    Symlink,      // 符号链接
    Other        // 其他
};

// 文件信息
struct FileInfo {
    std::string path;
    std::string name;
    FileType type;
    size_t size;
    time_t modificationTime;
    bool isReadable;
    bool isWritable;
    bool isExecutable;
};

// 路径工具类
class PathUtil {
public:
    // 规范化路径
    static std::string normalize(const std::string& path);
    
    // 获取绝对路径
    static std::string absolute(const std::string& path);
    
    // 获取相对路径
    static std::string relative(const std::string& path, const std::string& base);
    
    // 连接路径
    static std::string join(const std::string& path1, const std::string& path2);
    template<typename... Args>
    static std::string join(const std::string& path1, const std::string& path2, Args... args) {
        return join(join(path1, path2), args...);
    }
    
    // 获取父目录
    static std::string parent(const std::string& path);
    
    // 获取文件名
    static std::string filename(const std::string& path);
    
    // 获取基础名（不含扩展名）
    static std::string basename(const std::string& path);
    
    // 获取扩展名
    static std::string extension(const std::string& path);
    
    // 替换扩展名
    static std::string replaceExtension(const std::string& path, const std::string& newExt);
    
    // 是否是绝对路径
    static bool isAbsolute(const std::string& path);
    
    // 是否是相对路径
    static bool isRelative(const std::string& path);
};

// 文件系统工具类
class FileSystem {
public:
    // 检查文件/目录是否存在
    static bool exists(const std::string& path);
    
    // 是否是文件
    static bool isFile(const std::string& path);
    
    // 是否是目录
    static bool isDirectory(const std::string& path);
    
    // 是否是符号链接
    static bool isSymlink(const std::string& path);
    
    // 获取文件信息
    static std::optional<FileInfo> getFileInfo(const std::string& path);
    
    // 获取文件大小
    static size_t getFileSize(const std::string& path);
    
    // 获取修改时间
    static time_t getModificationTime(const std::string& path);
    
    // 创建目录
    static bool createDirectory(const std::string& path);
    
    // 创建目录（包括父目录）
    static bool createDirectories(const std::string& path);
    
    // 删除文件
    static bool removeFile(const std::string& path);
    
    // 删除目录（必须为空）
    static bool removeDirectory(const std::string& path);
    
    // 递归删除目录
    static bool removeAll(const std::string& path);
    
    // 复制文件
    static bool copyFile(const std::string& from, const std::string& to, bool overwrite = true);
    
    // 移动/重命名文件
    static bool moveFile(const std::string& from, const std::string& to);
    
    // 列出目录内容
    static std::vector<FileInfo> listDirectory(const std::string& path);
    
    // 递归列出目录内容
    static std::vector<FileInfo> listDirectoryRecursive(const std::string& path);
    
    // 搜索文件（支持通配符）
    static std::vector<std::string> glob(const std::string& pattern);
    
    // 获取当前工作目录
    static std::string getCurrentDirectory();
    
    // 设置当前工作目录
    static bool setCurrentDirectory(const std::string& path);
    
    // 获取临时目录
    static std::string getTempDirectory();
    
    // 创建临时文件
    static std::string createTempFile(const std::string& prefix = "tmp");
    
    // 创建临时目录
    static std::string createTempDirectory(const std::string& prefix = "tmp");
};

// 文件读写类
class File {
public:
    // 读取整个文件为字符串
    static std::optional<std::string> readToString(const std::string& path);
    
    // 读取文件为字节数组
    static std::optional<std::vector<uint8_t>> readToBytes(const std::string& path);
    
    // 读取文件为行
    static std::optional<std::vector<std::string>> readLines(const std::string& path);
    
    // 写入字符串到文件
    static bool writeString(const std::string& path, const std::string& content);
    
    // 写入字节数组到文件
    static bool writeBytes(const std::string& path, const std::vector<uint8_t>& data);
    
    // 写入行到文件
    static bool writeLines(const std::string& path, const std::vector<std::string>& lines);
    
    // 追加字符串到文件
    static bool appendString(const std::string& path, const std::string& content);
    
    // 追加行到文件
    static bool appendLines(const std::string& path, const std::vector<std::string>& lines);
};

// 文件流包装器
class FileStream {
public:
    FileStream() = default;
    explicit FileStream(const std::string& path, std::ios::openmode mode = std::ios::in);
    ~FileStream();
    
    // 打开文件
    bool open(const std::string& path, std::ios::openmode mode = std::ios::in);
    
    // 关闭文件
    void close();
    
    // 是否打开
    bool isOpen() const;
    
    // 是否到达文件末尾
    bool eof() const;
    
    // 获取文件位置
    std::streampos tell();
    
    // 设置文件位置
    void seek(std::streampos pos);
    void seekFromBegin(std::streamoff offset);
    void seekFromEnd(std::streamoff offset);
    void seekFromCurrent(std::streamoff offset);
    
    // 读取数据
    std::string readAll();
    std::string read(size_t size);
    std::string readLine();
    size_t readBytes(void* buffer, size_t size);
    
    // 写入数据
    bool write(const std::string& data);
    bool writeLine(const std::string& line);
    bool writeBytes(const void* data, size_t size);
    
    // 刷新缓冲区
    void flush();
    
    // 获取文件大小
    size_t size();

private:
    std::unique_ptr<std::fstream> stream_;
    std::string path_;
    std::ios::openmode mode_;
};

// 文件监视器
class FileWatcher {
public:
    using ChangeCallback = std::function<void(const std::string& path, FileType type)>;
    
    FileWatcher();
    ~FileWatcher();
    
    // 添加监视路径
    bool addPath(const std::string& path, bool recursive = false);
    
    // 移除监视路径
    void removePath(const std::string& path);
    
    // 设置回调
    void setCallback(ChangeCallback callback);
    
    // 开始监视
    bool start();
    
    // 停止监视
    void stop();
    
    // 是否正在监视
    bool isWatching() const;

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

// 文件锁
class FileLock {
public:
    explicit FileLock(const std::string& path);
    ~FileLock();
    
    // 尝试加锁
    bool tryLock();
    
    // 加锁（阻塞）
    bool lock();
    
    // 解锁
    void unlock();
    
    // 是否已加锁
    bool isLocked() const;

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace CHTL

#endif // CHTL_FILESYSTEM_H