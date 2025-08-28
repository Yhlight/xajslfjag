#ifndef ZIPUTIL_H
#define ZIPUTIL_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <fstream>
#include <cstdint>

namespace Util {

/**
 * @brief ZIP文件头结构
 * 符合ZIP文件格式规范
 */
struct ZIPLocalFileHeader {
    uint32_t signature;           // 本地文件头签名 (0x04034b50)
    uint16_t version;             // 版本
    uint16_t flags;               // 通用位标志
    uint16_t compression;         // 压缩方法
    uint16_t modTime;             // 最后修改时间
    uint16_t modDate;             // 最后修改日期
    uint32_t crc32;               // CRC32校验
    uint32_t compressedSize;      // 压缩后大小
    uint32_t uncompressedSize;    // 未压缩大小
    uint16_t fileNameLength;      // 文件名长度
    uint16_t extraFieldLength;    // 扩展字段长度
};

/**
 * @brief ZIP中央目录文件头结构
 */
struct ZIPCentralFileHeader {
    uint32_t signature;           // 中央目录文件头签名 (0x02014b50)
    uint16_t version;             // 版本
    uint16_t minVersion;          // 最低版本
    uint16_t flags;               // 通用位标志
    uint16_t compression;         // 压缩方法
    uint16_t modTime;             // 最后修改时间
    uint16_t modDate;             // 最后修改日期
    uint32_t crc32;               // CRC32校验
    uint32_t compressedSize;      // 压缩后大小
    uint32_t uncompressedSize;    // 未压缩大小
    uint16_t fileNameLength;      // 文件名长度
    uint16_t extraFieldLength;    // 扩展字段长度
    uint16_t fileCommentLength;   // 文件注释长度
    uint16_t diskNumber;          // 磁盘编号
    uint16_t internalAttributes;  // 内部属性
    uint32_t externalAttributes;  // 外部属性
    uint32_t localHeaderOffset;   // 本地文件头偏移
};

/**
 * @brief ZIP中央目录结束记录结构
 */
struct ZIPEndOfCentralDirectory {
    uint32_t signature;           // 中央目录结束签名 (0x06054b50)
    uint16_t diskNumber;          // 当前磁盘编号
    uint16_t startDisk;           // 中央目录开始磁盘编号
    uint16_t entriesOnDisk;       // 当前磁盘上的条目数
    uint16_t totalEntries;        // 中央目录中的总条目数
    uint32_t centralDirSize;      // 中央目录大小
    uint32_t centralDirOffset;    // 中央目录相对于文件开始的偏移
    uint16_t commentLength;       // 注释长度
};

/**
 * @brief ZIP文件条目信息
 */
struct ZIPEntry {
    std::string fileName;         // 文件名
    std::string comment;          // 注释
    uint32_t crc32;               // CRC32校验
    uint32_t compressedSize;      // 压缩后大小
    uint32_t uncompressedSize;    // 未压缩大小
    uint16_t compression;         // 压缩方法
    uint16_t modTime;             // 最后修改时间
    uint16_t modDate;             // 最后修改日期
    uint32_t localHeaderOffset;   // 本地文件头偏移
    bool isDirectory;             // 是否为目录
};

/**
 * @brief ZIP工具类
 * 提供ZIP文件的创建、读取、解压等功能
 * 完全手写实现，不依赖外部库
 */
class ZipUtil {
public:
    /**
     * @brief 压缩方法枚举
     */
    enum class CompressionMethod {
        STORED = 0,               // 不压缩
        DEFLATE = 8               // DEFLATE压缩
    };

    /**
     * @brief 构造函数
     */
    ZipUtil();
    
    /**
     * @brief 析构函数
     */
    ~ZipUtil();

    /**
     * @brief 创建ZIP文件
     * @param zipPath ZIP文件路径
     * @param sourcePath 源文件/目录路径
     * @param password 密码（可选）
     * @return 是否成功创建
     */
    bool createZip(const std::string& zipPath, const std::string& sourcePath, const std::string& password = "");
    
    /**
     * @brief 解压ZIP文件
     * @param zipPath ZIP文件路径
     * @param extractPath 解压目标路径
     * @param password 密码（可选）
     * @return 是否成功解压
     */
    bool extractZip(const std::string& zipPath, const std::string& extractPath, const std::string& password = "");
    
    /**
     * @brief 向ZIP文件添加文件
     * @param zipPath ZIP文件路径
     * @param filePath 要添加的文件路径
     * @param entryName ZIP中的条目名称
     * @param password 密码（可选）
     * @return 是否成功添加
     */
    bool addFile(const std::string& zipPath, const std::string& filePath, const std::string& entryName, const std::string& password = "");
    
    /**
     * @brief 从ZIP文件删除文件
     * @param zipPath ZIP文件路径
     * @param entryName 要删除的条目名称
     * @return 是否成功删除
     */
    bool deleteFile(const std::string& zipPath, const std::string& entryName);
    
    /**
     * @brief 列出ZIP文件内容
     * @param zipPath ZIP文件路径
     * @return 条目列表
     */
    std::vector<ZIPEntry> listEntries(const std::string& zipPath);
    
    /**
     * @brief 检查ZIP文件完整性
     * @param zipPath ZIP文件路径
     * @return 是否完整
     */
    bool verifyZip(const std::string& zipPath);
    
    /**
     * @brief 获取ZIP文件信息
     * @param zipPath ZIP文件路径
     * @return ZIP文件信息
     */
    std::map<std::string, std::string> getZipInfo(const std::string& zipPath);
    
    /**
     * @brief 设置压缩级别
     * @param level 压缩级别 (0-9, 0=不压缩, 9=最大压缩)
     */
    void setCompressionLevel(int level);
    
    /**
     * @brief 获取最后错误信息
     * @return 错误信息
     */
    std::string getLastError() const;
    
    /**
     * @brief 清空错误信息
     */
    void clearError();

private:
    int compressionLevel_;        // 压缩级别
    std::string lastError_;       // 最后错误信息
    
    /**
     * @brief 设置错误信息
     * @param error 错误信息
     */
    void setError(const std::string& error);
    
    /**
     * @brief 计算CRC32校验
     * @param data 数据
     * @param size 数据大小
     * @return CRC32值
     */
    uint32_t calculateCRC32(const uint8_t* data, size_t size);
    
    /**
     * @brief 压缩数据
     * @param input 输入数据
     * @param inputSize 输入大小
     * @param output 输出缓冲区
     * @param outputSize 输出缓冲区大小
     * @return 压缩后大小
     */
    size_t compressData(const uint8_t* input, size_t inputSize, uint8_t* output, size_t outputSize);
    
    /**
     * @brief 解压数据
     * @param input 输入数据
     * @param inputSize 输入大小
     * @param output 输出缓冲区
     * @param outputSize 输出缓冲区大小
     * @return 解压后大小
     */
    size_t decompressData(const uint8_t* input, size_t inputSize, uint8_t* output, size_t outputSize);
    
    /**
     * @brief 写入本地文件头
     * @param stream 输出流
     * @param header 文件头
     * @param fileName 文件名
     * @param extraField 扩展字段
     */
    void writeLocalFileHeader(std::ofstream& stream, const ZIPLocalFileHeader& header, 
                             const std::string& fileName, const std::string& extraField = "");
    
    /**
     * @brief 写入中央目录文件头
     * @param stream 输出流
     * @param header 文件头
     * @param fileName 文件名
     * @param extraField 扩展字段
     * @param comment 注释
     */
    void writeCentralFileHeader(std::ofstream& stream, const ZIPCentralFileHeader& header,
                               const std::string& fileName, const std::string& extraField = "", 
                               const std::string& comment = "");
    
    /**
     * @brief 写入中央目录结束记录
     * @param stream 输出流
     * @param record 结束记录
     * @param comment 注释
     */
    void writeEndOfCentralDirectory(std::ofstream& stream, const ZIPEndOfCentralDirectory& record,
                                   const std::string& comment = "");
    
    /**
     * @brief 读取本地文件头
     * @param stream 输入流
     * @param header 文件头
     * @param fileName 文件名
     * @param extraField 扩展字段
     * @return 是否成功读取
     */
    bool readLocalFileHeader(std::ifstream& stream, ZIPLocalFileHeader& header,
                            std::string& fileName, std::string& extraField);
    
    /**
     * @brief 读取中央目录文件头
     * @param stream 输入流
     * @param header 文件头
     * @param fileName 文件名
     * @param extraField 扩展字段
     * @param comment 注释
     * @return 是否成功读取
     */
    bool readCentralFileHeader(std::ifstream& stream, ZIPCentralFileHeader& header,
                              std::string& fileName, std::string& extraField, std::string& comment);
    
    /**
     * @brief 读取中央目录结束记录
     * @param stream 输入流
     * @param record 结束记录
     * @param comment 注释
     * @return 是否成功读取
     */
    bool readEndOfCentralDirectory(std::ifstream& stream, ZIPEndOfCentralDirectory& record,
                                  std::string& comment);
    
    /**
     * @brief 加密数据
     * @param data 数据
     * @param size 数据大小
     * @param password 密码
     */
    void encryptData(uint8_t* data, size_t size, const std::string& password);
    
    /**
     * @brief 解密数据
     * @param data 数据
     * @param size 数据大小
     * @param password 密码
     */
    void decryptData(uint8_t* data, size_t size, const std::string& password);
    
    /**
     * @brief 转换时间到ZIP格式
     * @param time 时间戳
     * @param modTime 修改时间
     * @param modDate 修改日期
     */
    void timeToZipFormat(time_t time, uint16_t& modTime, uint16_t& modDate);
    
    /**
     * @brief 转换ZIP格式到时间
     * @param modTime 修改时间
     * @param modDate 修改日期
     * @return 时间戳
     */
    time_t zipFormatToTime(uint16_t modTime, uint16_t modDate);
    
    /**
     * @brief 递归添加目录
     * @param zipPath ZIP文件路径
     * @param dirPath 目录路径
     * @param basePath 基础路径
     * @param password 密码
     * @return 是否成功
     */
    bool addDirectoryRecursive(const std::string& zipPath, const std::string& dirPath, 
                              const std::string& basePath, const std::string& password);
    
    /**
     * @brief 创建目录
     * @param path 目录路径
     * @return 是否成功
     */
    bool createDirectory(const std::string& path);
    
    /**
     * @brief 检查文件是否存在
     * @param path 文件路径
     * @return 是否存在
     */
    bool fileExists(const std::string& path);
    
    /**
     * @brief 检查目录是否存在
     * @param path 目录路径
     * @return 是否存在
     */
    bool directoryExists(const std::string& path);
    
    /**
     * @brief 解压文件
     * @param zipPath ZIP文件路径
     * @param extractPath 解压目标路径
     * @param entry ZIP条目
     * @param password 密码
     * @return 是否成功
     */
    bool extractFile(const std::string& zipPath, const std::string& extractPath, 
                     const ZIPEntry& entry, const std::string& password);
    
    /**
     * @brief 验证本地文件头
     * @param stream 输入流
     * @param centralHeader 中央目录文件头
     * @param fileName 文件名
     * @return 是否验证通过
     */
    bool verifyLocalFileHeader(std::ifstream& stream, const ZIPCentralFileHeader& centralHeader,
                              const std::string& fileName);
    
    /**
     * @brief 更新中央目录
     * @param stream 文件流
     * @param localHeader 本地文件头
     * @param fileName 文件名
     * @param offset 偏移量
     */
    void updateCentralDirectory(std::ofstream& stream, const ZIPLocalFileHeader& localHeader,
                               const std::string& fileName, uint32_t offset);
};

} // namespace Util

#endif // ZIPUTIL_H