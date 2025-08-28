#ifndef CHTLIOSTREAM_H
#define CHTLIOSTREAM_H

#include <string>
#include <vector>
#include <memory>
#include <fstream>
#include <sstream>
#include <iostream>
#include <filesystem>

namespace CHTL {

/**
 * @brief I/O流类型枚举
 */
enum class StreamType {
    INPUT,          // 输入流
    OUTPUT,         // 输出流
    BINARY,         // 二进制流
    TEXT,           // 文本流
    BUFFERED,       // 缓冲流
    UNBUFFERED      // 无缓冲流
};

/**
 * @brief 流状态枚举
 */
enum class StreamState {
    GOOD,           // 正常状态
    EOF,            // 文件结束
    ERROR,          // 错误状态
    CLOSED          // 已关闭
};

/**
 * @brief 流配置结构
 */
struct StreamConfig {
    StreamType type;            // 流类型
    bool buffered;              // 是否缓冲
    size_t bufferSize;          // 缓冲区大小
    std::string encoding;       // 编码格式
    bool autoFlush;             // 自动刷新
    
    StreamConfig() : type(StreamType::TEXT), buffered(true), bufferSize(4096), 
                     encoding("UTF-8"), autoFlush(false) {}
};

/**
 * @brief 流信息结构
 */
struct StreamInfo {
    std::string name;           // 流名称
    StreamType type;            // 流类型
    StreamState state;          // 流状态
    size_t position;            // 当前位置
    size_t size;                // 流大小
    std::string path;           // 文件路径
    
    StreamInfo() : type(StreamType::TEXT), state(StreamState::CLOSED), 
                   position(0), size(0) {}
};

/**
 * @brief CHTL I/O流管理器
 * 负责管理CHTL项目的所有I/O操作
 */
class CHTLIOStream {
public:
    /**
     * @brief 构造函数
     */
    CHTLIOStream();
    
    /**
     * @brief 析构函数
     */
    ~CHTLIOStream();
    
    /**
     * @brief 打开文件流
     * @param path 文件路径
     * @param mode 打开模式
     * @param config 流配置
     * @return 是否成功打开
     */
    bool openFile(const std::string& path, const std::string& mode, const StreamConfig& config = StreamConfig());
    
    /**
     * @brief 关闭文件流
     * @return 是否成功关闭
     */
    bool closeFile();
    
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
    StreamState getState() const;
    
    /**
     * @brief 获取流信息
     * @return 流信息
     */
    StreamInfo getInfo() const;
    
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
    StreamConfig config_;               // 流配置
    StreamInfo info_;                   // 流信息
    
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
};

} // namespace CHTL

#endif // CHTLIOSTREAM_H