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
    virtual bool exists(const CHTL::String& path) const;
    virtual bool isFile(const CHTL::String& path) const;
    virtual bool isDirectory(const CHTL::String& path) const;
    virtual bool isReadable(const CHTL::String& path) const;
    virtual bool isWritable(const CHTL::String& path) const;
    
    // 文件信息
    virtual size_t getFileSize(const CHTL::String& path) const;
    virtual std::time_t getLastModified(const CHTL::String& path) const;
    virtual CHTL::String getAbsolutePath(const CHTL::String& path) const;
    virtual CHTL::String getFileName(const CHTL::String& path) const;
    virtual CHTL::String getDirectoryName(const CHTL::String& path) const;
    virtual CHTL::String getFileExtension(const CHTL::String& path) const;
    
    // 目录操作
    virtual bool createDirectory(const CHTL::String& path) const;
    virtual bool createDirectories(const CHTL::String& path) const;
    virtual bool removeDirectory(const CHTL::String& path) const;
    virtual CHTL::StringVector listDirectory(const CHTL::String& path) const;
    virtual CHTL::StringVector listFiles(const CHTL::String& path, const CHTL::String& pattern = "*") const;
    
    // 文件操作
    virtual bool copyFile(const CHTL::String& source, const CHTL::String& destination) const;
    virtual bool moveFile(const CHTL::String& source, const CHTL::String& destination) const;
    virtual bool removeFile(const CHTL::String& path) const;
    virtual bool createFile(const CHTL::String& path) const;
    
    // 权限操作
    virtual bool setReadOnly(const CHTL::String& path, bool readOnly = true) const;
    virtual bool setExecutable(const CHTL::String& path, bool executable = true) const;
    
    // 路径工具
    static CHTL::String joinPath(const CHTL::String& path1, const CHTL::String& path2);
    static CHTL::String joinPath(const CHTL::StringVector& paths);
    static CHTL::String normalizePath(const CHTL::String& path);
    static CHTL::String getRelativePath(const CHTL::String& from, const CHTL::String& to);
    static CHTL::StringVector splitPath(const CHTL::String& path);
    
    // 临时文件
    virtual CHTL::String createTempFile(const CHTL::String& prefix = "chtl_") const;
    virtual CHTL::String createTempDirectory(const CHTL::String& prefix = "chtl_") const;
    
    // 监控功能
    virtual bool startWatching(const CHTL::String& path) const;
    virtual bool stopWatching(const CHTL::String& path) const;
    virtual bool isWatching(const CHTL::String& path) const;
    
protected:
    // 平台特定的实现辅助
    virtual CHTL::String getPlatformPath(const CHTL::String& path) const;
    virtual bool platformSpecificOperation(const CHTL::String& operation, const CHTL::String& path) const;
    
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
    bool exists(const CHTL::String& path) const override;
    bool isFile(const CHTL::String& path) const override;
    bool isDirectory(const CHTL::String& path) const override;
    bool isReadable(const CHTL::String& path) const override;
    bool isWritable(const CHTL::String& path) const override;
    
    size_t getFileSize(const CHTL::String& path) const override;
    std::time_t getLastModified(const CHTL::String& path) const override;
    CHTL::String getAbsolutePath(const CHTL::String& path) const override;
    
    bool createDirectory(const CHTL::String& path) const override;
    bool createDirectories(const CHTL::String& path) const override;
    bool removeDirectory(const CHTL::String& path) const override;
    CHTL::StringVector listDirectory(const CHTL::String& path) const override;
    CHTL::StringVector listFiles(const CHTL::String& path, const CHTL::String& pattern = "*") const override;
    
    bool copyFile(const CHTL::String& source, const CHTL::String& destination) const override;
    bool moveFile(const CHTL::String& source, const CHTL::String& destination) const override;
    bool removeFile(const CHTL::String& path) const override;
    bool createFile(const CHTL::String& path) const override;
    
    CHTL::String createTempFile(const CHTL::String& prefix = "chtl_") const override;
    CHTL::String createTempDirectory(const CHTL::String& prefix = "chtl_") const override;

private:
    void initializePlatform();
    bool isWindows() const;
    bool isUnix() const;
    CHTL::String getPlatformPath(const CHTL::String& path) const override;
    
#ifdef _WIN32
    CHTL::String winPath(const CHTL::String& path) const;
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