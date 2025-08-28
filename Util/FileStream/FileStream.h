#ifndef FILESTREAM_H
#define FILESTREAM_H

#include <string>
#include <vector>
#include <memory>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <functional>

namespace Util {

/**
 * @brief 文件流基类
 * 提供通用的文件操作功能，支持多种文件格式和操作模式
 */
class FileStream {
public:
    /**
     * @brief 文件打开模式枚举
     */
    enum class OpenMode {
        READ_ONLY,      // 只读模式
        WRITE_ONLY,     // 只写模式
        READ_WRITE,     // 读写模式
        APPEND,         // 追加模式
        CREATE,         // 创建模式
        TRUNCATE        // 截断模式
    };

    /**
     * @brief 文件类型枚举
     */
    enum class FileType {
        TEXT,           // 文本文件
        BINARY,         // 二进制文件
        AUTO            // 自动检测
    };

    /**
     * @brief 文件状态枚举
     */
    enum class FileStatus {
        CLOSED,         // 已关闭
        OPEN,           // 已打开
        ERROR,          // 错误状态
        EOF_REACHED     // 到达文件末尾
    };

    /**
     * @brief 构造函数
     * @param filePath 文件路径
     * @param mode 打开模式
     * @param type 文件类型
     */
    explicit FileStream(const std::string& filePath = "", 
                       OpenMode mode = OpenMode::READ_ONLY,
                       FileType type = FileType::AUTO);
    
    /**
     * @brief 虚析构函数
     */
    virtual ~FileStream();

    /**
     * @brief 打开文件
     * @param filePath 文件路径（可选，如果构造函数已提供）
     * @return 是否成功打开
     */
    virtual bool open(const std::string& filePath = "");
    
    /**
     * @brief 关闭文件
     */
    virtual void close();
    
    /**
     * @brief 检查文件是否打开
     * @return 是否打开
     */
    virtual bool isOpen() const;
    
    /**
     * @brief 获取文件状态
     * @return 文件状态
     */
    virtual FileStatus getStatus() const;
    
    /**
     * @brief 获取文件路径
     * @return 文件路径
     */
    virtual std::string getFilePath() const;
    
    /**
     * @brief 获取文件大小
     * @return 文件大小（字节）
     */
    virtual size_t getFileSize() const;
    
    /**
     * @brief 获取文件行数
     * @return 文件行数
     */
    virtual size_t getLineCount() const;
    
    /**
     * @brief 检查文件是否存在
     * @return 是否存在
     */
    virtual bool exists() const;
    
    /**
     * @brief 删除文件
     * @return 是否成功删除
     */
    virtual bool remove();
    
    /**
     * @brief 重命名文件
     * @param newPath 新路径
     * @return 是否成功重命名
     */
    virtual bool rename(const std::string& newPath);
    
    /**
     * @brief 复制文件
     * @param targetPath 目标路径
     * @return 是否成功复制
     */
    virtual bool copy(const std::string& targetPath);
    
    /**
     * @brief 移动文件
     * @param targetPath 目标路径
     * @return 是否成功移动
     */
    virtual bool move(const std::string& targetPath);
    
    /**
     * @brief 获取文件扩展名
     * @return 文件扩展名
     */
    virtual std::string getExtension() const;
    
    /**
     * @brief 获取文件名（不含路径）
     * @return 文件名
     */
    virtual std::string getFileName() const;
    
    /**
     * @brief 获取目录路径
     * @return 目录路径
     */
    virtual std::string getDirectory() const;
    
    /**
     * @brief 获取最后修改时间
     * @return 最后修改时间（时间戳）
     */
    virtual time_t getLastModified() const;
    
    /**
     * @brief 设置文件权限
     * @param permissions 权限掩码
     * @return 是否成功设置
     */
    virtual bool setPermissions(int permissions);
    
    /**
     * @brief 获取文件权限
     * @return 权限掩码
     */
    virtual int getPermissions() const;
    
    /**
     * @brief 锁定文件
     * @param exclusive 是否独占锁定
     * @return 是否成功锁定
     */
    virtual bool lock(bool exclusive = false);
    
    /**
     * @brief 解锁文件
     */
    virtual void unlock();
    
    /**
     * @brief 获取错误信息
     * @return 错误信息
     */
    virtual std::string getLastError() const;
    
    /**
     * @brief 清空错误信息
     */
    virtual void clearError();

protected:
    std::string filePath_;              // 文件路径
    OpenMode openMode_;                 // 打开模式
    FileType fileType_;                 // 文件类型
    FileStatus status_;                 // 文件状态
    std::fstream fileStream_;           // 文件流
    std::string lastError_;             // 最后错误信息
    bool isLocked_;                     // 是否锁定
    int lockType_;                      // 锁定类型
    
    /**
     * @brief 设置错误信息
     * @param error 错误信息
     */
    virtual void setError(const std::string& error);
    
    /**
     * @brief 检测文件类型
     */
    virtual void detectFileType();
    
    /**
     * @brief 设置文件打开模式
     */
    virtual std::ios::openmode getOpenMode() const;
    
    /**
     * @brief 检查文件权限
     * @return 是否有权限
     */
    virtual bool checkPermissions() const;
};

/**
 * @brief 文本文件流
 * 专门用于处理文本文件
 */
class TextFileStream : public FileStream {
public:
    /**
     * @brief 构造函数
     * @param filePath 文件路径
     * @param mode 打开模式
     */
    explicit TextFileStream(const std::string& filePath = "", OpenMode mode = OpenMode::READ_ONLY);
    
    /**
     * @brief 读取所有文本内容
     * @return 文本内容
     */
    virtual std::string readAll();
    
    /**
     * @brief 写入文本内容
     * @param content 文本内容
     * @return 是否成功写入
     */
    virtual bool writeAll(const std::string& content);
    
    /**
     * @brief 读取指定行数
     * @param startLine 起始行号
     * @param endLine 结束行号
     * @return 指定行范围的内容
     */
    virtual std::string readLines(size_t startLine, size_t endLine);
    
    /**
     * @brief 写入指定行
     * @param lineNumber 行号
     * @param content 内容
     * @return 是否成功写入
     */
    virtual bool writeLine(size_t lineNumber, const std::string& content);
    
    /**
     * @brief 插入内容到指定行
     * @param lineNumber 行号
     * @param content 内容
     * @return 是否成功插入
     */
    virtual bool insertLine(size_t lineNumber, const std::string& content);
    
    /**
     * @brief 删除指定行
     * @param lineNumber 行号
     * @return 是否成功删除
     */
    virtual bool deleteLine(size_t lineNumber);
    
    /**
     * @brief 追加内容
     * @param content 内容
     * @return 是否成功追加
     */
    virtual bool append(const std::string& content);
    
    /**
     * @brief 搜索文本
     * @param searchText 搜索文本
     * @param caseSensitive 是否区分大小写
     * @return 匹配的行号列表
     */
    virtual std::vector<size_t> searchText(const std::string& searchText, bool caseSensitive = true);
    
    /**
     * @brief 替换文本
     * @param searchText 搜索文本
     * @param replaceText 替换文本
     * @param caseSensitive 是否区分大小写
     * @return 替换次数
     */
    virtual size_t replaceText(const std::string& searchText, const std::string& replaceText, bool caseSensitive = true);

protected:
    std::vector<std::string> lines_;    // 文件行缓存
    
    /**
     * @brief 加载文件内容到内存
     * @return 是否成功加载
     */
    virtual bool loadFileContent();
    
    /**
     * @brief 保存文件内容到磁盘
     * @return 是否成功保存
     */
    virtual bool saveFileContent();
    
    /**
     * @brief 更新行缓存
     */
    virtual void updateLineCache();
};

/**
 * @brief 二进制文件流
 * 专门用于处理二进制文件
 */
class BinaryFileStream : public FileStream {
public:
    /**
     * @brief 构造函数
     * @param filePath 文件路径
     * @param mode 打开模式
     */
    explicit BinaryFileStream(const std::string& filePath = "", OpenMode mode = OpenMode::READ_ONLY);
    
    /**
     * @brief 读取二进制数据
     * @param buffer 缓冲区
     * @param size 读取大小
     * @return 实际读取的字节数
     */
    virtual size_t read(void* buffer, size_t size);
    
    /**
     * @brief 写入二进制数据
     * @param buffer 缓冲区
     * @param size 写入大小
     * @return 实际写入的字节数
     */
    virtual size_t write(const void* buffer, size_t size);
    
    /**
     * @brief 读取指定位置的数据
     * @param offset 偏移量
     * @param buffer 缓冲区
     * @param size 读取大小
     * @return 实际读取的字节数
     */
    virtual size_t readAt(size_t offset, void* buffer, size_t size);
    
    /**
     * @brief 写入指定位置的数据
     * @param offset 偏移量
     * @param buffer 缓冲区
     * @param size 写入大小
     * @return 实际写入的字节数
     */
    virtual size_t writeAt(size_t offset, const void* buffer, size_t size);
    
    /**
     * @brief 设置文件指针位置
     * @param offset 偏移量
     * @param origin 起始位置
     * @return 是否成功设置
     */
    virtual bool seek(size_t offset, std::ios::seekdir origin = std::ios::beg);
    
    /**
     * @brief 获取当前文件指针位置
     * @return 当前位置
     */
    virtual size_t tell() const;
    
    /**
     * @brief 截断文件
     * @param size 新大小
     * @return 是否成功截断
     */
    virtual bool truncate(size_t size);
    
    /**
     * @brief 刷新缓冲区
     */
    virtual void flush();

protected:
    /**
     * @brief 设置文件打开模式
     */
    virtual std::ios::openmode getOpenMode() const override;
};

} // namespace Util

#endif // FILESTREAM_H