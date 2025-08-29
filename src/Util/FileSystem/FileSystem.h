#pragma once

#include "../Common.h"
#include <string>
#include <vector>
#include <ctime>

namespace Util {

// 文件系统抽象基类 - 为高性能IO流提供基础
class FileSystem {
public:
    FileSystem() = default;
    virtual ~FileSystem() = default;

    // 基础文件操作接口
    virtual bool exists(const String& path) const;
    virtual bool isFile(const String& path) const;
    virtual bool isDirectory(const String& path) const;
    virtual bool isReadable(const String& path) const;
    virtual bool isWritable(const String& path) const;
    
    // 文件信息
    virtual size_t getFileSize(const String& path) const;
    virtual std::time_t getLastModified(const String& path) const;
    virtual String getAbsolutePath(const String& path) const;
    virtual String getFileName(const String& path) const;
    virtual String getDirectoryName(const String& path) const;
    virtual String getFileExtension(const String& path) const;
    
    // 目录操作
    virtual bool createDirectory(const String& path) const;
    virtual bool createDirectories(const String& path) const;
    virtual bool removeDirectory(const String& path) const;
    virtual StringVector listDirectory(const String& path) const;
    virtual StringVector listFiles(const String& path, const String& pattern = "*") const;
    
    // 文件操作
    virtual bool copyFile(const String& source, const String& destination) const;
    virtual bool moveFile(const String& source, const String& destination) const;
    virtual bool removeFile(const String& path) const;
    virtual bool createFile(const String& path) const;
    
    // 权限操作
    virtual bool setReadOnly(const String& path, bool readOnly = true) const;
    virtual bool setExecutable(const String& path, bool executable = true) const;
    
    // 路径工具
    static String joinPath(const String& path1, const String& path2);
    static String joinPath(const StringVector& paths);
    static String normalizePath(const String& path);
    static String getRelativePath(const String& from, const String& to);
    static StringVector splitPath(const String& path);
    
    // 临时文件
    virtual String createTempFile(const String& prefix = "chtl_") const;
    virtual String createTempDirectory(const String& prefix = "chtl_") const;
    
    // 监控功能
    virtual bool startWatching(const String& path) const;
    virtual bool stopWatching(const String& path) const;
    virtual bool isWatching(const String& path) const;
    
protected:
    // 平台特定的实现辅助
    virtual String getPlatformPath(const String& path) const;
    virtual bool platformSpecificOperation(const String& operation, const String& path) const;
    
private:
    // 禁用拷贝构造和赋值
    FileSystem(const FileSystem&) = delete;
    FileSystem& operator=(const FileSystem&) = delete;
};

// 跨平台文件系统实现
class CrossPlatformFileSystem : public FileSystem {
public:
    CrossPlatformFileSystem();
    virtual ~CrossPlatformFileSystem();
    
    // 重写基类方法以提供跨平台实现
    bool exists(const String& path) const override;
    bool isFile(const String& path) const override;
    bool isDirectory(const String& path) const override;
    bool isReadable(const String& path) const override;
    bool isWritable(const String& path) const override;
    
    size_t getFileSize(const String& path) const override;
    std::time_t getLastModified(const String& path) const override;
    String getAbsolutePath(const String& path) const override;
    
    bool createDirectory(const String& path) const override;
    bool createDirectories(const String& path) const override;
    bool removeDirectory(const String& path) const override;
    StringVector listDirectory(const String& path) const override;
    StringVector listFiles(const String& path, const String& pattern = "*") const override;
    
    bool copyFile(const String& source, const String& destination) const override;
    bool moveFile(const String& source, const String& destination) const override;
    bool removeFile(const String& path) const override;
    bool createFile(const String& path) const override;
    
    String createTempFile(const String& prefix = "chtl_") const override;
    String createTempDirectory(const String& prefix = "chtl_") const override;

private:
    void initializePlatform();
    bool isWindows() const;
    bool isUnix() const;
    
#ifdef _WIN32
    String winPath(const String& path) const;
#endif
};

// 文件系统工厂类
class FileSystemFactory {
public:
    static std::unique_ptr<FileSystem> createDefault();
    static std::unique_ptr<FileSystem> createCrossPlatform();
    static std::unique_ptr<FileSystem> createOptimized();
    
private:
    FileSystemFactory() = default;
};

} // namespace Util