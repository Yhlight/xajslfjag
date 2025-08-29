#ifndef UTIL_ZIPUTIL_H
#define UTIL_ZIPUTIL_H

#include <string>
#include <vector>
#include <memory>
#include <functional>

namespace CHTL {

// ZIP压缩级别
enum class CompressionLevel {
    STORE = 0,      // 不压缩
    FASTEST = 1,    // 最快压缩
    FAST = 3,       // 快速压缩
    NORMAL = 6,     // 正常压缩
    GOOD = 7,       // 良好压缩
    BEST = 9        // 最佳压缩
};

// ZIP文件信息
struct ZIPFileInfo {
    std::string filename;
    size_t uncompressedSize;
    size_t compressedSize;
    std::time_t modificationTime;
    bool isDirectory;
    int compressionMethod;
};

// ZIP工具类
class ZIPUtil {
public:
    ZIPUtil();
    ~ZIPUtil();
    
    // 设置压缩级别
    void setCompressionLevel(int level);
    
    // 创建ZIP文件
    bool createArchive(const std::string& zipPath);
    
    // 添加文件到ZIP
    bool addFile(const std::string& filePath, const std::string& entryName = "");
    
    // 从内存添加数据到ZIP
    bool addFromMemory(const std::string& entryName, const std::string& data);
    
    // 添加目录到ZIP
    bool addDirectory(const std::string& dirPath, const std::string& entryPrefix = "");
    
    // 完成ZIP创建
    bool finalize();
    
    // 解压整个ZIP文件
    bool extractArchive(const std::string& zipPath, const std::string& destDir);
    
    // 解压单个文件
    bool extractFile(const std::string& zipPath, const std::string& entryName, const std::string& destPath);
    
    // 列出ZIP内容
    std::vector<ZIPFileInfo> listFiles(const std::string& zipPath);
    
    // 检查文件是否存在于ZIP中
    bool fileExists(const std::string& zipPath, const std::string& entryName);
    
    // 读取ZIP中的文件到内存
    std::string readFileToString(const std::string& zipPath, const std::string& entryName);
    
    // 设置进度回调
    void setProgressCallback(std::function<void(size_t current, size_t total)> callback);
    
    // 获取错误信息
    const std::string& getLastError() const { return lastError_; }

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
    std::string lastError_;
    
    // 辅助方法
    bool ensureDirectoryExists(const std::string& path);
    std::string normalizeEntryName(const std::string& name);
};

// ZIP读取器（用于流式读取）
class ZIPReader {
public:
    ZIPReader();
    ~ZIPReader();
    
    // 打开ZIP文件
    bool open(const std::string& zipPath);
    
    // 关闭ZIP文件
    void close();
    
    // 获取文件列表
    std::vector<ZIPFileInfo> getFileList();
    
    // 定位到指定文件
    bool locateFile(const std::string& entryName);
    
    // 读取当前文件
    std::vector<uint8_t> readCurrentFile();
    
    // 读取部分数据
    size_t read(void* buffer, size_t size);
    
    // 是否已打开
    bool isOpen() const;
    
    // 获取当前文件信息
    ZIPFileInfo getCurrentFileInfo();

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

// ZIP写入器（用于流式写入）
class ZIPWriter {
public:
    ZIPWriter();
    ~ZIPWriter();
    
    // 创建ZIP文件
    bool create(const std::string& zipPath);
    
    // 开始新文件
    bool beginFile(const std::string& entryName, int compressionLevel = 6);
    
    // 写入数据
    bool write(const void* data, size_t size);
    
    // 结束当前文件
    bool endFile();
    
    // 关闭ZIP文件
    bool close();
    
    // 是否已打开
    bool isOpen() const;
    
    // 设置注释
    void setComment(const std::string& comment);

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace CHTL

#endif // UTIL_ZIPUTIL_H