#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <memory>
#include <unordered_map>
#include <chrono>
#include <cstdint>
#include <functional>

namespace CHTL {
namespace Core {

// 前向声明
class FileSystem;

/**
 * CompressionMethod - 压缩方法枚举
 */
enum class CompressionMethod : uint16_t {
    STORE = 0,      // 不压缩
    DEFLATE = 8     // DEFLATE压缩算法
};

/**
 * CompressionLevel - 压缩级别
 */
enum class CompressionLevel {
    NO_COMPRESSION = 0,
    BEST_SPEED = 1,
    BEST_COMPRESSION = 9,
    DEFAULT_COMPRESSION = 6
};

/**
 * ZipEntryInfo - ZIP条目信息
 */
struct ZipEntryInfo {
    std::string name;                                      // 文件名
    std::string comment;                                   // 注释
    uint32_t compressed_size;                              // 压缩后大小
    uint32_t uncompressed_size;                            // 原始大小
    uint32_t crc32;                                        // CRC32校验和
    CompressionMethod compression_method;                  // 压缩方法
    std::chrono::system_clock::time_point last_modified;   // 最后修改时间
    bool is_directory;                                     // 是否为目录
    bool is_encrypted;                                     // 是否加密
    uint32_t external_attributes;                          // 外部属性
    uint32_t internal_attributes;                          // 内部属性
    
    ZipEntryInfo() 
        : compressed_size(0), uncompressed_size(0), crc32(0)
        , compression_method(CompressionMethod::STORE)
        , is_directory(false), is_encrypted(false)
        , external_attributes(0), internal_attributes(0) {}
};

/**
 * ZipEntry - ZIP条目
 */
class ZipEntry {
public:
    ZipEntry();
    ZipEntry(const std::string& name, const std::vector<uint8_t>& data);
    ZipEntry(const std::string& name, const std::string& text_data);
    
    // 基本信息
    const std::string& getName() const { return info.name; }
    void setName(const std::string& name) { info.name = name; }
    
    const std::string& getComment() const { return info.comment; }
    void setComment(const std::string& comment) { info.comment = comment; }
    
    uint32_t getCompressedSize() const { return info.compressed_size; }
    uint32_t getUncompressedSize() const { return info.uncompressed_size; }
    uint32_t getCrc32() const { return info.crc32; }
    
    CompressionMethod getCompressionMethod() const { return info.compression_method; }
    void setCompressionMethod(CompressionMethod method) { info.compression_method = method; }
    
    bool isDirectory() const { return info.is_directory; }
    void setIsDirectory(bool is_dir) { info.is_directory = is_dir; }
    
    bool isEncrypted() const { return info.is_encrypted; }
    
    const std::chrono::system_clock::time_point& getLastModified() const { return info.last_modified; }
    void setLastModified(const std::chrono::system_clock::time_point& time) { info.last_modified = time; }
    
    // 数据操作
    const std::vector<uint8_t>& getData() const { return data; }
    void setData(const std::vector<uint8_t>& new_data);
    void setData(const std::string& text_data);
    
    std::string getDataAsString() const;
    
    // 压缩/解压
    bool compress(CompressionLevel level = CompressionLevel::DEFAULT_COMPRESSION);
    bool decompress();
    
    // 验证
    bool validateCrc32() const;
    void updateCrc32();
    
    // 获取完整信息
    const ZipEntryInfo& getInfo() const { return info; }

private:
    ZipEntryInfo info;
    std::vector<uint8_t> data;          // 未压缩数据
    std::vector<uint8_t> compressed_data; // 压缩数据
    bool is_compressed;
    
    void updateSize();
    uint32_t calculateCrc32(const std::vector<uint8_t>& data) const;
    
    friend class ZipArchive;
    friend class ZipReader;
    friend class ZipWriter;
};

/**
 * ZipArchive - ZIP压缩包
 */
class ZipArchive {
public:
    ZipArchive();
    explicit ZipArchive(const std::string& filename);
    ~ZipArchive();
    
    // 文件操作
    bool open(const std::string& filename, bool create_if_not_exists = false);
    bool save();
    bool saveAs(const std::string& filename);
    void close();
    
    bool isOpen() const { return is_open; }
    const std::string& getFilename() const { return filename; }
    
    // 条目管理
    bool addEntry(const ZipEntry& entry);
    bool addFile(const std::string& filename, const std::string& entry_name = "");
    bool addDirectory(const std::string& directory_path, const std::string& entry_prefix = "", bool recursive = true);
    bool addData(const std::string& entry_name, const std::vector<uint8_t>& data);
    bool addText(const std::string& entry_name, const std::string& text);
    
    bool removeEntry(const std::string& entry_name);
    bool hasEntry(const std::string& entry_name) const;
    ZipEntry* getEntry(const std::string& entry_name);
    const ZipEntry* getEntry(const std::string& entry_name) const;
    
    // 提取操作
    bool extractEntry(const std::string& entry_name, const std::string& output_path);
    bool extractAll(const std::string& output_directory);
    std::vector<uint8_t> extractEntryData(const std::string& entry_name);
    std::string extractEntryText(const std::string& entry_name);
    
    // 查询操作
    std::vector<std::string> getEntryNames() const;
    std::vector<ZipEntryInfo> getEntryInfos() const;
    size_t getEntryCount() const { return entries.size(); }
    
    uint64_t getTotalUncompressedSize() const;
    uint64_t getTotalCompressedSize() const;
    double getCompressionRatio() const;
    
    // 验证操作
    bool validateArchive() const;
    std::vector<std::string> getCorruptedEntries() const;
    
    // 注释操作
    const std::string& getComment() const { return comment; }
    void setComment(const std::string& comment) { this->comment = comment; }
    
    // 密码保护
    void setPassword(const std::string& password);
    bool isPasswordProtected() const { return !password.empty(); }
    
    // 进度回调
    using ProgressCallback = std::function<void(const std::string&, size_t, size_t)>;
    void setProgressCallback(ProgressCallback callback) { progress_callback = callback; }

private:
    std::string filename;
    std::unordered_map<std::string, std::unique_ptr<ZipEntry>> entries;
    std::string comment;
    std::string password;
    bool is_open;
    bool is_modified;
    ProgressCallback progress_callback;
    
    // ZIP文件结构常量
    static const uint32_t LOCAL_FILE_HEADER_SIGNATURE = 0x04034b50;
    static const uint32_t CENTRAL_DIRECTORY_HEADER_SIGNATURE = 0x02014b50;
    static const uint32_t END_OF_CENTRAL_DIRECTORY_SIGNATURE = 0x06054b50;
    static const uint32_t DATA_DESCRIPTOR_SIGNATURE = 0x08074b50;
    
    // 内部方法
    bool readFromFile();
    bool writeToFile();
    bool parseLocalFileHeader(std::ifstream& file, ZipEntry& entry);
    bool parseCentralDirectory(std::ifstream& file);
    bool parseEndOfCentralDirectory(std::ifstream& file);
    
    bool writeLocalFileHeader(std::ofstream& file, const ZipEntry& entry, uint32_t& offset);
    bool writeCentralDirectory(std::ofstream& file, const std::vector<uint32_t>& offsets);
    bool writeEndOfCentralDirectory(std::ofstream& file, uint32_t central_dir_offset, uint32_t central_dir_size);
    
    uint16_t dosDateTime(const std::chrono::system_clock::time_point& time) const;
    uint16_t dosDate(const std::chrono::system_clock::time_point& time) const;
    std::chrono::system_clock::time_point fromDosDateTime(uint16_t date, uint16_t time) const;
    
    void reportProgress(const std::string& operation, size_t current, size_t total);
};

/**
 * ZipReader - ZIP文件读取器（只读）
 */
class ZipReader {
public:
    ZipReader();
    explicit ZipReader(const std::string& filename);
    ~ZipReader();
    
    bool open(const std::string& filename);
    void close();
    
    bool isOpen() const { return is_open; }
    
    // 查询操作
    std::vector<std::string> listEntries() const;
    std::vector<ZipEntryInfo> getEntryInfos() const;
    bool hasEntry(const std::string& entry_name) const;
    ZipEntryInfo getEntryInfo(const std::string& entry_name) const;
    
    // 提取操作
    std::vector<uint8_t> extractEntry(const std::string& entry_name);
    std::string extractEntryAsText(const std::string& entry_name);
    bool extractEntryToFile(const std::string& entry_name, const std::string& output_path);
    bool extractAll(const std::string& output_directory);
    
    // 验证操作
    bool validateEntry(const std::string& entry_name) const;
    bool validateArchive() const;
    
    // 搜索操作
    std::vector<std::string> findEntries(const std::string& pattern) const;
    std::vector<std::string> findEntriesInDirectory(const std::string& directory) const;

private:
    std::string filename;
    std::ifstream file;
    std::unordered_map<std::string, ZipEntryInfo> entry_infos;
    std::unordered_map<std::string, uint32_t> entry_offsets;
    bool is_open;
    
    bool scanArchive();
    bool readEntry(const std::string& entry_name, std::vector<uint8_t>& data);
};

/**
 * ZipWriter - ZIP文件写入器（只写）
 */
class ZipWriter {
public:
    ZipWriter();
    explicit ZipWriter(const std::string& filename);
    ~ZipWriter();
    
    bool open(const std::string& filename);
    void close();
    
    bool isOpen() const { return is_open; }
    
    // 添加操作
    bool addFile(const std::string& file_path, const std::string& entry_name = "");
    bool addDirectory(const std::string& directory_path, const std::string& entry_prefix = "", bool recursive = true);
    bool addData(const std::string& entry_name, const std::vector<uint8_t>& data, 
                CompressionLevel level = CompressionLevel::DEFAULT_COMPRESSION);
    bool addText(const std::string& entry_name, const std::string& text,
                CompressionLevel level = CompressionLevel::DEFAULT_COMPRESSION);
    bool addEntry(const ZipEntry& entry);
    
    // 设置选项
    void setCompressionLevel(CompressionLevel level) { default_compression_level = level; }
    void setComment(const std::string& comment) { archive_comment = comment; }

private:
    std::string filename;
    std::ofstream file;
    std::vector<std::pair<std::string, uint32_t>> written_entries; // name -> offset
    std::vector<ZipEntry> entries_metadata;
    CompressionLevel default_compression_level;
    std::string archive_comment;
    bool is_open;
    
    bool writeEntry(const ZipEntry& entry);
    bool finalize();
};

/**
 * ZipUtils - ZIP工具类
 */
class ZipUtils {
public:
    /**
     * 快速创建ZIP文件
     * @param zip_filename ZIP文件名
     * @param files_to_add 要添加的文件列表
     * @param compression_level 压缩级别
     * @return 是否成功
     */
    static bool createZipFromFiles(const std::string& zip_filename, 
                                  const std::vector<std::string>& files_to_add,
                                  CompressionLevel compression_level = CompressionLevel::DEFAULT_COMPRESSION);
    
    /**
     * 快速创建ZIP文件（从目录）
     * @param zip_filename ZIP文件名
     * @param directory 源目录
     * @param recursive 是否递归包含子目录
     * @param compression_level 压缩级别
     * @return 是否成功
     */
    static bool createZipFromDirectory(const std::string& zip_filename,
                                      const std::string& directory,
                                      bool recursive = true,
                                      CompressionLevel compression_level = CompressionLevel::DEFAULT_COMPRESSION);
    
    /**
     * 快速解压ZIP文件
     * @param zip_filename ZIP文件名
     * @param output_directory 输出目录
     * @return 是否成功
     */
    static bool extractZip(const std::string& zip_filename, const std::string& output_directory);
    
    /**
     * 列出ZIP文件内容
     * @param zip_filename ZIP文件名
     * @return 条目名称列表
     */
    static std::vector<std::string> listZipContents(const std::string& zip_filename);
    
    /**
     * 验证ZIP文件
     * @param zip_filename ZIP文件名
     * @return 是否有效
     */
    static bool validateZip(const std::string& zip_filename);
    
    /**
     * 获取ZIP文件信息
     * @param zip_filename ZIP文件名
     * @return ZIP文件的基本信息
     */
    static std::string getZipInfo(const std::string& zip_filename);
    
    /**
     * 比较两个ZIP文件
     * @param zip1 第一个ZIP文件
     * @param zip2 第二个ZIP文件
     * @return 是否内容相同
     */
    static bool compareZipFiles(const std::string& zip1, const std::string& zip2);
    
    /**
     * 合并多个ZIP文件
     * @param output_zip 输出ZIP文件
     * @param input_zips 输入ZIP文件列表
     * @return 是否成功
     */
    static bool mergeZipFiles(const std::string& output_zip, const std::vector<std::string>& input_zips);
    
    /**
     * 检查ZIP文件是否损坏
     * @param zip_filename ZIP文件名
     * @return 损坏的条目列表，空列表表示没有损坏
     */
    static std::vector<std::string> checkZipIntegrity(const std::string& zip_filename);

private:
    static bool isValidZipFile(const std::string& filename);
    static std::string formatFileSize(uint64_t size);
    static std::string formatCompressionRatio(double ratio);
};

/**
 * DeflateCompressor - DEFLATE压缩器
 */
class DeflateCompressor {
public:
    /**
     * 压缩数据
     * @param input 输入数据
     * @param level 压缩级别
     * @return 压缩后的数据
     */
    static std::vector<uint8_t> compress(const std::vector<uint8_t>& input, CompressionLevel level = CompressionLevel::DEFAULT_COMPRESSION);
    
    /**
     * 解压数据
     * @param input 压缩数据
     * @return 解压后的数据
     */
    static std::vector<uint8_t> decompress(const std::vector<uint8_t>& input);
    
    /**
     * 计算压缩后的大小估计
     * @param input_size 输入大小
     * @param level 压缩级别
     * @return 估计的压缩大小
     */
    static size_t estimateCompressedSize(size_t input_size, CompressionLevel level);

private:
    // 内部压缩算法实现
    struct CompressionState;
    static std::vector<uint8_t> deflateCompress(const std::vector<uint8_t>& input, int level);
    static std::vector<uint8_t> inflateDecompress(const std::vector<uint8_t>& input);
};

/**
 * CRC32Calculator - CRC32校验和计算器
 */
class CRC32Calculator {
public:
    /**
     * 计算数据的CRC32
     * @param data 数据
     * @return CRC32值
     */
    static uint32_t calculate(const std::vector<uint8_t>& data);
    
    /**
     * 计算字符串的CRC32
     * @param text 字符串
     * @return CRC32值
     */
    static uint32_t calculate(const std::string& text);
    
    /**
     * 增量计算CRC32
     * @param previous_crc 之前的CRC32值
     * @param data 新数据
     * @return 更新后的CRC32值
     */
    static uint32_t update(uint32_t previous_crc, const std::vector<uint8_t>& data);

private:
    static const uint32_t CRC32_TABLE[256];
    static void initializeTable();
    static bool table_initialized;
};

} // namespace Core
} // namespace CHTL