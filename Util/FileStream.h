#ifndef FILESTREAM_H
#define FILESTREAM_H

#include <string>
#include <vector>
#include <memory>
#include <fstream>
#include <sstream>
#include <iostream>
#include <filesystem>

namespace Util {

/**
 * @brief 文件流类型枚举
 */
enum class FileStreamType {
    READ,           // 只读
    WRITE,          // 只写
    READ_WRITE,     // 读写
    APPEND,         // 追加
    BINARY,         // 二进制
    TEXT            // 文本
};

/**
 * @brief 文件流状态枚举
 */
enum class FileStreamState {
    GOOD,           // 正常状态
    EOF,            // 文件结束
    ERROR,          // 错误状态
    CLOSED          // 已关闭
};

/**
 * @brief 文件流配置结构
 */
struct FileStreamConfig {
    FileStreamType type;        // 流类型
    bool buffered;              // 是否缓冲
    size_t bufferSize;          // 缓冲区大小
    std::string encoding;       // 编码格式
    bool autoFlush;             // 自动刷新
    bool createIfNotExists;     // 不存在时创建
    bool truncate;              // 截断文件
    
    FileStreamConfig() : type(FileStreamType::READ), buffered(true), bufferSize(4096), 
                         encoding("UTF-8"), autoFlush(false), createIfNotExists(false), truncate(false) {}
};

/**
 * @brief 文件流信息结构
 */
struct FileStreamInfo {
    std::string name;           // 文件名
    FileStreamType type;        // 流类型
    FileStreamState state;      // 流状态
    size_t position;            // 当前位置
    size_t size;                // 文件大小
    std::string path;           // 文件路径
    std::string mode;           // 打开模式
    
    FileStreamInfo() : type(FileStreamType::READ), state(FileStreamState::CLOSED), 
                       position(0), size(0) {}
};

/**
 * @brief 文件流管理器
 * 提供高级文件操作功能
 */
class FileStream {
public:
    /**
     * @brief 构造函数
     */
    FileStream();
    
    /**
     * @brief 析构函数
     */
    ~FileStream();
    
    /**
     * @brief 打开文件
     * @param path 文件路径
     * @param config 流配置
     * @return 是否成功打开
     */
    bool open(const std::string& path, const FileStreamConfig& config = FileStreamConfig());
    
    /**
     * @brief 关闭文件
     * @return 是否成功关闭
     */
    bool close();
    
    /**
     * @brief 读取文本
     * @param buffer 缓冲区
     * @param size 读取大小
     * @return 实际读取的字节数
     */
    size_t readText(char* buffer, size_t size);
    
    /**
     * @brief 读取一行文本
     * @return 读取的文本行
     */
    std::string readLine();
    
    /**
     * @brief 读取所有文本
     * @return 所有文本内容
     */
    std::string readAll();
    
    /**
     * @brief 写入文本
     * @param data 要写入的数据
     * @param size 数据大小
     * @return 实际写入的字节数
     */
    size_t writeText(const char* data, size_t size);
    
    /**
     * @brief 写入字符串
     * @param str 要写入的字符串
     * @return 是否写入成功
     */
    bool writeString(const std::string& str);
    
    /**
     * @brief 写入行
     * @param line 要写入的行
     * @return 是否写入成功
     */
    bool writeLine(const std::string& line);
    
    /**
     * @brief 刷新缓冲区
     * @return 是否刷新成功
     */
    bool flush();
    
    /**
     * @brief 设置文件位置
     * @param offset 偏移量
     * @param whence 起始位置（0=开始，1=当前，2=结束）
     * @return 是否设置成功
     */
    bool seek(long offset, int whence = 0);
    
    /**
     * @brief 获取当前位置
     * @return 当前位置
     */
    size_t tell() const;
    
    /**
     * @brief 检查是否到达文件末尾
     * @return 是否到达文件末尾
     */
    bool isEOF() const;
    
    /**
     * @brief 检查流状态
     * @return 流状态
     */
    FileStreamState getState() const;
    
    /**
     * @brief 获取流信息
     * @return 流信息
     */
    FileStreamInfo getInfo() const;
    
    /**
     * @brief 检查文件是否存在
     * @param path 文件路径
     * @return 是否存在
     */
    static bool fileExists(const std::string& path);
    
    /**
     * @brief 获取文件大小
     * @param path 文件路径
     * @return 文件大小
     */
    static size_t getFileSize(const std::string& path);
    
    /**
     * @brief 创建目录
     * @param path 目录路径
     * @return 是否创建成功
     */
    static bool createDirectory(const std::string& path);
    
    /**
     * @brief 删除文件
     * @param path 文件路径
     * @return 是否删除成功
     */
    static bool deleteFile(const std::string& path);
    
    /**
     * @brief 复制文件
     * @param source 源文件路径
     * @param destination 目标文件路径
     * @return 是否复制成功
     */
    static bool copyFile(const std::string& source, const std::string& destination);
    
    /**
     * @brief 移动文件
     * @param source 源文件路径
     * @param destination 目标文件路径
     * @return 是否移动成功
     */
    static bool moveFile(const std::string& source, const std::string& destination);
    
    /**
     * @brief 列出目录内容
     * @param path 目录路径
     * @return 文件列表
     */
    static std::vector<std::string> listDirectory(const std::string& path);
    
    /**
     * @brief 获取文件扩展名
     * @param path 文件路径
     * @return 文件扩展名
     */
    static std::string getFileExtension(const std::string& path);
    
    /**
     * @brief 获取文件名（不含扩展名）
     * @param path 文件路径
     * @return 文件名
     */
    static std::string getFileName(const std::string& path);
    
    /**
     * @brief 获取目录路径
     * @param path 文件路径
     * @return 目录路径
     */
    static std::string getDirectoryPath(const std::string& path);
    
    /**
     * @brief 获取绝对路径
     * @param path 相对路径
     * @return 绝对路径
     */
    static std::string getAbsolutePath(const std::string& path);
    
    /**
     * @brief 规范化路径
     * @param path 原始路径
     * @return 规范化后的路径
     */
    static std::string normalizePath(const std::string& path);
    
    /**
     * @brief 检查是否为绝对路径
     * @param path 路径
     * @return 是否为绝对路径
     */
    static bool isAbsolutePath(const std::string& path);
    
    /**
     * @brief 检查是否为目录
     * @param path 路径
     * @return 是否为目录
     */
    static bool isDirectory(const std::string& path);
    
    /**
     * @brief 检查是否为文件
     * @param path 路径
     * @return 是否为文件
     */
    static bool isFile(const std::string& path);
    
    /**
     * @brief 获取文件修改时间
     * @param path 文件路径
     * @return 修改时间（Unix时间戳）
     */
    static time_t getFileModificationTime(const std::string& path);
    
    /**
     * @brief 获取文件创建时间
     * @param path 文件路径
     * @return 创建时间（Unix时间戳）
     */
    static time_t getFileCreationTime(const std::string& path);
    
    /**
     * @brief 获取文件权限
     * @param path 文件路径
     * @return 文件权限
     */
    static std::filesystem::perms getFilePermissions(const std::string& path);
    
    /**
     * @brief 设置文件权限
     * @param path 文件路径
     * @param perms 文件权限
     * @return 是否设置成功
     */
    static bool setFilePermissions(const std::string& path, std::filesystem::perms perms);
    
    /**
     * @brief 获取统计信息
     * @return 统计信息字符串
     */
    std::string getStatistics() const;
    
    /**
     * @brief 清除统计信息
     */
    void clearStatistics();

private:
    std::fstream fileStream_;           // 文件流
    FileStreamConfig config_;           // 流配置
    FileStreamInfo info_;               // 流信息
    
    // 统计信息
    size_t totalBytesRead_;
    size_t totalBytesWritten_;
    size_t totalOperations_;
    size_t totalErrors_;
    
    /**
     * @brief 更新统计信息
     * @param type 统计类型
     * @param value 统计值
     */
    void updateStatistics(const std::string& type, size_t value = 1);
    
    /**
     * @brief 检查流状态
     */
    void checkStreamState();
    
    /**
     * @brief 设置错误状态
     * @param error 错误信息
     */
    void setError(const std::string& error);
    
    /**
     * @brief 设置文件打开模式
     * @return 打开模式
     */
    std::ios_base::openmode getOpenMode() const;
};

} // namespace Util

#endif // FILESTREAM_H