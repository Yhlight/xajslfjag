#ifndef ZIPUTIL_H
#define ZIPUTIL_H

#include <string>
#include <vector>
#include <memory>
#include <fstream>
#include <map>

namespace Util {

/**
 * @brief ZIP文件头结构
 */
struct ZipHeader {
    uint32_t signature;         // 本地文件头签名 (0x04034b50)
    uint16_t version;           // 版本
    uint16_t flags;             // 通用位标志
    uint16_t compression;       // 压缩方法
    uint16_t modTime;           // 文件最后修改时间
    uint16_t modDate;           // 文件最后修改日期
    uint32_t crc32;             // CRC-32
    uint32_t compressedSize;    // 压缩后大小
    uint32_t uncompressedSize;  // 未压缩大小
    uint16_t fileNameLength;    // 文件名长度
    uint16_t extraFieldLength;  // 扩展字段长度
    
    ZipHeader() : signature(0), version(0), flags(0), compression(0), modTime(0), modDate(0),
                  crc32(0), compressedSize(0), uncompressedSize(0), fileNameLength(0), extraFieldLength(0) {}
};

/**
 * @brief ZIP中央目录文件头结构
 */
struct ZipCentralHeader {
    uint32_t signature;         // 中央文件头签名 (0x02014b50)
    uint16_t version;           // 版本
    uint16_t minVersion;        // 最低版本
    uint16_t flags;             // 通用位标志
    uint16_t compression;       // 压缩方法
    uint16_t modTime;           // 文件最后修改时间
    uint16_t modDate;           // 文件最后修改日期
    uint32_t crc32;             // CRC-32
    uint32_t compressedSize;    // 压缩后大小
    uint32_t uncompressedSize;  // 未压缩大小
    uint16_t fileNameLength;    // 文件名长度
    uint16_t extraFieldLength;  // 扩展字段长度
    uint16_t fileCommentLength; // 文件注释长度
    uint16_t diskNumber;        // 磁盘编号
    uint16_t internalAttrs;     // 内部文件属性
    uint32_t externalAttrs;     // 外部文件属性
    uint32_t localHeaderOffset; // 本地文件头的相对偏移量
    
    ZipCentralHeader() : signature(0), version(0), minVersion(0), flags(0), compression(0), modTime(0), modDate(0),
                         crc32(0), compressedSize(0), uncompressedSize(0), fileNameLength(0), extraFieldLength(0),
                         fileCommentLength(0), diskNumber(0), internalAttrs(0), externalAttrs(0), localHeaderOffset(0) {}
};

/**
 * @brief ZIP文件条目信息
 */
struct ZipEntry {
    std::string fileName;       // 文件名
    uint32_t compressedSize;    // 压缩后大小
    uint32_t uncompressedSize;  // 未压缩大小
    uint32_t crc32;             // CRC-32
    uint16_t compression;       // 压缩方法
    uint32_t offset;            // 文件在ZIP中的偏移量
    std::vector<uint8_t> data;  // 文件数据
    
    ZipEntry() : compressedSize(0), uncompressedSize(0), crc32(0), compression(0), offset(0) {}
};

/**
 * @brief ZIP文件结束记录结构
 */
struct ZipEndRecord {
    uint32_t signature;         // 中央目录结束签名 (0x06054b50)
    uint16_t diskNumber;        // 当前磁盘编号
    uint16_t centralDiskNumber; // 中央目录所在磁盘编号
    uint16_t centralEntries;    // 本磁盘上的条目数
    uint16_t totalEntries;      // 中央目录中的条目总数
    uint32_t centralSize;       // 中央目录大小
    uint32_t centralOffset;     // 中央目录相对于文件起始的偏移量
    uint16_t commentLength;     // ZIP文件注释长度
    
    ZipEndRecord() : signature(0), diskNumber(0), centralDiskNumber(0), centralEntries(0), totalEntries(0),
                     centralSize(0), centralOffset(0), commentLength(0) {}
};

/**
 * @brief ZIP工具类
 * 提供ZIP文件的创建、读取、解压等功能（手写实现，不依赖外部库）
 */
class ZipUtil {
public:
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
     * @param entries 要包含的文件条目
     * @return 是否创建成功
     */
    bool createZip(const std::string& zipPath, const std::vector<ZipEntry>& entries);
    
    /**
     * @brief 读取ZIP文件
     * @param zipPath ZIP文件路径
     * @return 是否读取成功
     */
    bool readZip(const std::string& zipPath);
    
    /**
     * @brief 解压ZIP文件
     * @param zipPath ZIP文件路径
     * @param extractPath 解压目标路径
     * @return 是否解压成功
     */
    bool extractZip(const std::string& zipPath, const std::string& extractPath);
    
    /**
     * @brief 添加文件到ZIP
     * @param filePath 要添加的文件路径
     * @param zipPath ZIP文件路径
     * @return 是否添加成功
     */
    bool addFileToZip(const std::string& filePath, const std::string& zipPath);
    
    /**
     * @brief 从ZIP中删除文件
     * @param fileName 要删除的文件名
     * @param zipPath ZIP文件路径
     * @return 是否删除成功
     */
    bool removeFileFromZip(const std::string& fileName, const std::string& zipPath);
    
    /**
     * @brief 列出ZIP文件内容
     * @param zipPath ZIP文件路径
     * @return 文件条目列表
     */
    std::vector<ZipEntry> listZipContents(const std::string& zipPath);
    
    /**
     * @brief 获取ZIP文件信息
     * @param zipPath ZIP文件路径
     * @return ZIP文件信息字符串
     */
    std::string getZipInfo(const std::string& zipPath);
    
    /**
     * @brief 验证ZIP文件完整性
     * @param zipPath ZIP文件路径
     * @return 是否完整
     */
    bool verifyZip(const std::string& zipPath);
    
    /**
     * @brief 压缩数据
     * @param data 要压缩的数据
     * @param compressedData 压缩后的数据
     * @return 是否压缩成功
     */
    static bool compressData(const std::vector<uint8_t>& data, std::vector<uint8_t>& compressedData);
    
    /**
     * @brief 解压数据
     * @param compressedData 压缩的数据
     * @param data 解压后的数据
     * @return 是否解压成功
     */
    static bool decompressData(const std::vector<uint8_t>& compressedData, std::vector<uint8_t>& data);
    
    /**
     * @brief 计算CRC-32
     * @param data 数据
     * @param size 数据大小
     * @return CRC-32值
     */
    static uint32_t calculateCRC32(const uint8_t* data, size_t size);
    
    /**
     * @brief 获取ZIP条目
     * @param fileName 文件名
     * @return ZIP条目指针
     */
    std::shared_ptr<ZipEntry> getEntry(const std::string& fileName) const;
    
    /**
     * @brief 获取所有条目
     * @return 条目列表
     */
    const std::vector<ZipEntry>& getAllEntries() const;
    
    /**
     * @brief 清除所有条目
     */
    void clearEntries();
    
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
    std::vector<ZipEntry> entries_;     // ZIP条目列表
    std::map<std::string, size_t> entryMap_; // 文件名到索引的映射
    
    // 统计信息
    size_t totalZipsCreated_;
    size_t totalZipsRead_;
    size_t totalFilesCompressed_;
    size_t totalFilesExtracted_;
    size_t totalBytesProcessed_;
    
    /**
     * @brief 写入ZIP文件头
     * @param stream 输出流
     * @param entry ZIP条目
     * @return 是否写入成功
     */
    bool writeLocalHeader(std::ofstream& stream, const ZipEntry& entry);
    
    /**
     * @brief 写入ZIP中央目录头
     * @param stream 输出流
     * @param entry ZIP条目
     * @param offset 本地文件头偏移量
     * @return 是否写入成功
     */
    bool writeCentralHeader(std::ofstream& stream, const ZipEntry& entry, uint32_t offset);
    
    /**
     * @brief 写入ZIP文件结束记录
     * @param stream 输出流
     * @param centralOffset 中央目录偏移量
     * @param centralSize 中央目录大小
     * @param totalEntries 总条目数
     * @return 是否写入成功
     */
    bool writeEndRecord(std::ofstream& stream, uint32_t centralOffset, uint32_t centralSize, size_t totalEntries);
    
    /**
     * @brief 读取ZIP文件头
     * @param stream 输入流
     * @param header 文件头结构
     * @return 是否读取成功
     */
    bool readLocalHeader(std::ifstream& stream, ZipHeader& header);
    
    /**
     * @brief 读取ZIP中央目录头
     * @param stream 输入流
     * @param header 中央目录头结构
     * @return 是否读取成功
     */
    bool readCentralHeader(std::ifstream& stream, ZipCentralHeader& header);
    
    /**
     * @brief 读取ZIP文件结束记录
     * @param stream 输入流
     * @param endRecord 结束记录结构
     * @return 是否读取成功
     */
    bool readEndRecord(std::ifstream& stream, ZipEndRecord& endRecord);
    
    /**
     * @brief 查找ZIP文件结束记录
     * @param stream 输入流
     * @param endRecord 结束记录结构
     * @return 是否找到
     */
    bool findEndRecord(std::ifstream& stream, ZipEndRecord& endRecord);
    
    /**
     * @brief 更新统计信息
     * @param type 统计类型
     * @param value 统计值
     */
    void updateStatistics(const std::string& type, size_t value = 1);
    
    /**
     * @brief 创建目录结构
     * @param path 路径
     * @return 是否创建成功
     */
    bool createDirectoryStructure(const std::string& path);
    
    /**
     * @brief 获取文件修改时间
     * @param filePath 文件路径
     * @param modTime 修改时间
     * @param modDate 修改日期
     * @return 是否获取成功
     */
    bool getFileModificationTime(const std::string& filePath, uint16_t& modTime, uint16_t& modDate);
    
    /**
     * @brief 从DOS时间转换为标准时间
     * @param dosTime DOS时间
     * @param dosDate DOS日期
     * @return 标准时间字符串
     */
    std::string dosTimeToString(uint16_t dosTime, uint16_t dosDate);
};

} // namespace Util

#endif // ZIPUTIL_H