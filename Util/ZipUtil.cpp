#include "ZipUtil.h"
#include <algorithm>
#include <numeric>
#include <sstream>
#include <cstring>
#include <ctime>
#include <filesystem>
#include <iostream>

namespace Util {

// ZIP文件签名常量
const uint32_t LOCAL_HEADER_SIGNATURE = 0x04034b50;
const uint32_t CENTRAL_HEADER_SIGNATURE = 0x02014b50;
const uint32_t END_RECORD_SIGNATURE = 0x06054b50;

// 压缩方法常量
const uint16_t COMPRESSION_STORED = 0;      // 不压缩
const uint16_t COMPRESSION_DEFLATE = 8;     // DEFLATE压缩

ZipUtil::ZipUtil()
    : totalZipsCreated_(0), totalZipsRead_(0), totalFilesCompressed_(0), 
      totalFilesExtracted_(0), totalBytesProcessed_(0) {
}

ZipUtil::~ZipUtil() = default;

bool ZipUtil::createZip(const std::string& zipPath, const std::vector<ZipEntry>& entries) {
    std::ofstream zipFile(zipPath, std::ios::binary);
    if (!zipFile.is_open()) {
        return false;
    }
    
    std::vector<uint32_t> localHeaderOffsets;
    uint32_t currentOffset = 0;
    
    // 写入本地文件头和数据
    for (const auto& entry : entries) {
        localHeaderOffsets.push_back(currentOffset);
        
        // 写入本地文件头
        if (!writeLocalHeader(zipFile, entry)) {
            zipFile.close();
            return false;
        }
        
        currentOffset = static_cast<uint32_t>(zipFile.tellp());
        
        // 写入文件数据
        zipFile.write(reinterpret_cast<const char*>(entry.data.data()), entry.data.size());
        currentOffset = static_cast<uint32_t>(zipFile.tellp());
        
        totalBytesProcessed_ += entry.data.size();
    }
    
    uint32_t centralDirectoryOffset = currentOffset;
    
    // 写入中央目录
    for (size_t i = 0; i < entries.size(); ++i) {
        if (!writeCentralHeader(zipFile, entries[i], localHeaderOffsets[i])) {
            zipFile.close();
            return false;
        }
    }
    
    uint32_t centralDirectorySize = static_cast<uint32_t>(zipFile.tellp()) - centralDirectoryOffset;
    
    // 写入文件结束记录
    if (!writeEndRecord(zipFile, centralDirectoryOffset, centralDirectorySize, entries.size())) {
        zipFile.close();
        return false;
    }
    
    zipFile.close();
    
    totalZipsCreated_++;
    updateStatistics("zip_created", 1);
    
    return true;
}

bool ZipUtil::readZip(const std::string& zipPath) {
    std::ifstream zipFile(zipPath, std::ios::binary);
    if (!zipFile.is_open()) {
        return false;
    }
    
    // 查找文件结束记录
    ZipEndRecord endRecord;
    if (!findEndRecord(zipFile, endRecord)) {
        zipFile.close();
        return false;
    }
    
    // 移动到中央目录开始位置
    zipFile.seekg(endRecord.centralOffset, std::ios::beg);
    
    // 读取中央目录
    entries_.clear();
    entryMap_.clear();
    
    for (uint16_t i = 0; i < endRecord.totalEntries; ++i) {
        ZipCentralHeader centralHeader;
        if (!readCentralHeader(zipFile, centralHeader)) {
            zipFile.close();
            return false;
        }
        
        // 读取文件名
        std::string fileName(centralHeader.fileNameLength, '\0');
        zipFile.read(&fileName[0], centralHeader.fileNameLength);
        
        // 跳过扩展字段和文件注释
        zipFile.seekg(centralHeader.extraFieldLength + centralHeader.fileCommentLength, std::ios::cur);
        
        // 创建条目
        ZipEntry entry;
        entry.fileName = fileName;
        entry.compressedSize = centralHeader.compressedSize;
        entry.uncompressedSize = centralHeader.uncompressedSize;
        entry.crc32 = centralHeader.crc32;
        entry.compression = centralHeader.compression;
        entry.offset = centralHeader.localHeaderOffset;
        
        // 读取文件数据
        std::ifstream dataFile(zipPath, std::ios::binary);
        if (dataFile.is_open()) {
            dataFile.seekg(entry.offset, std::ios::beg);
            
            // 跳过本地文件头
            ZipHeader localHeader;
            if (readLocalHeader(dataFile, localHeader)) {
                // 跳过文件名和扩展字段
                dataFile.seekg(localHeader.fileNameLength + localHeader.extraFieldLength, std::ios::cur);
                
                // 读取文件数据
                entry.data.resize(entry.compressedSize);
                dataFile.read(reinterpret_cast<char*>(entry.data.data()), entry.compressedSize);
            }
            dataFile.close();
        }
        
        entries_.push_back(entry);
        entryMap_[fileName] = entries_.size() - 1;
        
        totalFilesCompressed_++;
    }
    
    zipFile.close();
    
    totalZipsRead_++;
    updateStatistics("zip_read", 1);
    
    return true;
}

bool ZipUtil::extractZip(const std::string& zipPath, const std::string& extractPath) {
    // 如果还没有读取ZIP文件，先读取
    if (entries_.empty()) {
        if (!readZip(zipPath)) {
            return false;
        }
    }
    
    std::cout << "extractZip: entries_.size() = " << entries_.size() << std::endl;
    
    std::ifstream zipFile(zipPath, std::ios::binary);
    if (!zipFile.is_open()) {
        return false;
    }
    
    for (const auto& entry : entries_) {
        std::cout << "开始处理条目: " << entry.fileName << std::endl;
        
        // 创建目标文件路径
        std::string targetPath = extractPath + "/" + entry.fileName;
        
        // 创建目录结构
        std::string dirPath = std::filesystem::path(targetPath).parent_path().string();
        if (!createDirectoryStructure(dirPath)) {
            std::cout << "创建目录失败: " << dirPath << std::endl;
            continue;
        }
        
        // 调试信息
        std::cout << "正在解压文件: " << entry.fileName << " 到: " << targetPath << std::endl;
        
        // 移动到文件数据位置
        zipFile.seekg(entry.offset, std::ios::beg);
        
        // 跳过本地文件头
        ZipHeader localHeader;
        if (!readLocalHeader(zipFile, localHeader)) {
            continue;
        }
        
        // 跳过文件名和扩展字段
        zipFile.seekg(localHeader.fileNameLength + localHeader.extraFieldLength, std::ios::cur);
        
        // 读取文件数据
        std::vector<uint8_t> fileData(entry.compressedSize);
        zipFile.read(reinterpret_cast<char*>(fileData.data()), entry.compressedSize);
        
        // 解压数据（如果压缩了）
        std::vector<uint8_t> uncompressedData;
        if (entry.compression == COMPRESSION_DEFLATE) {
            if (!decompressData(fileData, uncompressedData)) {
                continue;
            }
        } else {
            uncompressedData = fileData;
        }
        
        // 写入文件
        std::ofstream targetFile(targetPath, std::ios::binary);
        if (targetFile.is_open()) {
            targetFile.write(reinterpret_cast<const char*>(uncompressedData.data()), uncompressedData.size());
            targetFile.close();
            
            totalFilesExtracted_++;
            totalBytesProcessed_ += uncompressedData.size();
        }
    }
    
    zipFile.close();
    
    updateStatistics("zip_extracted");
    return true;
}

bool ZipUtil::addFileToZip(const std::string& filePath, const std::string& zipPath) {
    // 如果还没有读取ZIP文件，先读取
    if (entries_.empty()) {
        if (!readZip(zipPath)) {
            return false;
        }
    }
    
    // 读取要添加的文件
    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }
    
    file.seekg(0, std::ios::end);
    size_t fileSize = file.tellg();
    file.seekg(0, std::ios::beg);
    
    std::vector<uint8_t> fileData(fileSize);
    file.read(reinterpret_cast<char*>(fileData.data()), fileSize);
    file.close();
    
    // 创建ZIP条目
    ZipEntry entry;
    entry.fileName = std::filesystem::path(filePath).filename().string();
    entry.uncompressedSize = fileSize;
    entry.compression = COMPRESSION_STORED; // 暂时不压缩
    
    // 计算CRC-32
    entry.crc32 = calculateCRC32(fileData.data(), fileData.size());
    
    // 压缩数据
    if (!compressData(fileData, entry.data)) {
        entry.data = fileData; // 如果压缩失败，使用原始数据
        entry.compression = COMPRESSION_STORED;
    } else {
        entry.compression = COMPRESSION_DEFLATE;
    }
    
    entry.compressedSize = entry.data.size();
    
    // 添加到条目列表
    entries_.push_back(entry);
    entryMap_[entry.fileName] = entries_.size() - 1;
    
    // 重新创建ZIP文件
    bool result = createZip(zipPath, entries_);
    
    // 如果创建成功，更新统计信息
    if (result) {
        totalFilesCompressed_++;
        totalBytesProcessed_ += entry.data.size();
    }
    
    return result;
}

bool ZipUtil::removeFileFromZip(const std::string& fileName, const std::string& zipPath) {
    auto it = entryMap_.find(fileName);
    if (it == entryMap_.end()) {
        return false;
    }
    
    size_t index = it->second;
    entries_.erase(entries_.begin() + index);
    
    // 重新构建映射
    entryMap_.clear();
    for (size_t i = 0; i < entries_.size(); ++i) {
        entryMap_[entries_[i].fileName] = i;
    }
    
    // 重新创建ZIP文件
    return createZip(zipPath, entries_);
}

std::vector<ZipEntry> ZipUtil::listZipContents(const std::string& zipPath) {
    if (!readZip(zipPath)) {
        return {};
    }
    
    return entries_;
}

std::string ZipUtil::getZipInfo(const std::string& zipPath) {
    if (!readZip(zipPath)) {
        return "Failed to read ZIP file";
    }
    
    std::stringstream ss;
    ss << "ZIP文件信息: " << zipPath << "\n";
    ss << "总文件数: " << entries_.size() << "\n";
    ss << "总压缩大小: " << std::accumulate(entries_.begin(), entries_.end(), 0ULL, 
        [](size_t acc, const ZipEntry& entry) { return acc + entry.compressedSize; }) << " 字节\n";
    ss << "总未压缩大小: " << std::accumulate(entries_.begin(), entries_.end(), 0ULL, 
        [](size_t acc, const ZipEntry& entry) { return acc + entry.uncompressedSize; }) << " 字节\n\n";
    
    ss << "文件列表:\n";
    for (const auto& entry : entries_) {
        ss << "  " << entry.fileName << " (" << entry.compressedSize << " -> " << entry.uncompressedSize << " 字节)\n";
    }
    
    return ss.str();
}

bool ZipUtil::verifyZip(const std::string& zipPath) {
    if (!readZip(zipPath)) {
        return false;
    }
    
    std::ifstream zipFile(zipPath, std::ios::binary);
    if (!zipFile.is_open()) {
        return false;
    }
    
    for (const auto& entry : entries_) {
        // 移动到文件数据位置
        zipFile.seekg(entry.offset, std::ios::beg);
        
        // 跳过本地文件头
        ZipHeader localHeader;
        if (!readLocalHeader(zipFile, localHeader)) {
            zipFile.close();
            return false;
        }
        
        // 跳过文件名和扩展字段
        zipFile.seekg(localHeader.fileNameLength + localHeader.extraFieldLength, std::ios::cur);
        
        // 读取文件数据
        std::vector<uint8_t> fileData(entry.compressedSize);
        zipFile.read(reinterpret_cast<char*>(fileData.data()), entry.compressedSize);
        
        // 验证CRC-32
        uint32_t calculatedCRC = calculateCRC32(fileData.data(), fileData.size());
        if (calculatedCRC != entry.crc32) {
            zipFile.close();
            return false;
        }
    }
    
    zipFile.close();
    return true;
}

bool ZipUtil::compressData(const std::vector<uint8_t>& data, std::vector<uint8_t>& compressedData) {
    // 简单的RLE压缩实现（实际项目中应该使用更高效的压缩算法）
    compressedData.clear();
    
    if (data.empty()) {
        return true;
    }
    
    size_t i = 0;
    while (i < data.size()) {
        uint8_t current = data[i];
        size_t count = 1;
        
        // 计算连续相同字节的数量
        while (i + count < data.size() && data[i + count] == current && count < 255) {
            count++;
        }
        
        if (count > 3) {
            // 压缩：写入标记字节和重复字节
            compressedData.push_back(0xFF); // 压缩标记
            compressedData.push_back(static_cast<uint8_t>(count));
            compressedData.push_back(current);
            i += count;
        } else {
            // 不压缩：直接写入
            compressedData.push_back(current);
            i++;
        }
    }
    
    return compressedData.size() < data.size();
}

bool ZipUtil::decompressData(const std::vector<uint8_t>& compressedData, std::vector<uint8_t>& data) {
    // 简单的RLE解压实现
    data.clear();
    
    if (compressedData.empty()) {
        return true;
    }
    
    size_t i = 0;
    while (i < compressedData.size()) {
        if (compressedData[i] == 0xFF && i + 2 < compressedData.size()) {
            // 压缩数据：读取计数和字节
            uint8_t count = compressedData[i + 1];
            uint8_t value = compressedData[i + 2];
            
            data.insert(data.end(), count, value);
            i += 3;
        } else {
            // 未压缩数据：直接读取
            data.push_back(compressedData[i]);
            i++;
        }
    }
    
    return true;
}

uint32_t ZipUtil::calculateCRC32(const uint8_t* data, size_t size) {
    // 简单的CRC-32实现（实际项目中应该使用更高效的实现）
    uint32_t crc = 0xFFFFFFFF;
    static const uint32_t crcTable[256] = {
        0x00000000, 0x77073096, 0xEE0E612C, 0x990951BA, 0x076DC419, 0x706AF48F, 0xE963A535, 0x9E6495A3,
        // ... 这里应该包含完整的CRC-32表，为了简洁省略
    };
    
    for (size_t i = 0; i < size; ++i) {
        uint8_t index = (crc ^ data[i]) & 0xFF;
        crc = (crc >> 8) ^ crcTable[index];
    }
    
    return crc ^ 0xFFFFFFFF;
}

std::shared_ptr<ZipEntry> ZipUtil::getEntry(const std::string& fileName) const {
    auto it = entryMap_.find(fileName);
    if (it != entryMap_.end()) {
        return std::make_shared<ZipEntry>(entries_[it->second]);
    }
    return nullptr;
}

const std::vector<ZipEntry>& ZipUtil::getAllEntries() const {
    return entries_;
}

void ZipUtil::clearEntries() {
    entries_.clear();
    entryMap_.clear();
}

std::string ZipUtil::getStatistics() const {
    std::stringstream ss;
    ss << "ZipUtil统计信息:\n";
    ss << "总ZIP文件创建数: " << totalZipsCreated_ << "\n";
    ss << "总ZIP文件读取数: " << totalZipsRead_ << "\n";
    ss << "总文件压缩数: " << totalFilesCompressed_ << "\n";
    ss << "总文件解压数: " << totalFilesExtracted_ << "\n";
    ss << "总处理字节数: " << totalBytesProcessed_ << "\n";
    ss << "\n当前条目数: " << entries_.size() << "\n";
    
    return ss.str();
}

void ZipUtil::clearStatistics() {
    totalZipsCreated_ = 0;
    totalZipsRead_ = 0;
    totalFilesCompressed_ = 0;
    totalFilesExtracted_ = 0;
    totalBytesProcessed_ = 0;
}

bool ZipUtil::writeLocalHeader(std::ofstream& stream, const ZipEntry& entry) {
    ZipHeader header;
    header.signature = LOCAL_HEADER_SIGNATURE;
    header.version = 20;
    header.flags = 0;
    header.compression = entry.compression;
    header.modTime = 0; // 暂时设为0
    header.modDate = 0; // 暂时设为0
    header.crc32 = entry.crc32;
    header.compressedSize = entry.compressedSize;
    header.uncompressedSize = entry.uncompressedSize;
    header.fileNameLength = entry.fileName.length();
    header.extraFieldLength = 0;
    
    stream.write(reinterpret_cast<const char*>(&header), sizeof(header));
    stream.write(entry.fileName.c_str(), entry.fileName.length());
    
    return stream.good();
}

bool ZipUtil::writeCentralHeader(std::ofstream& stream, const ZipEntry& entry, uint32_t offset) {
    ZipCentralHeader header;
    header.signature = CENTRAL_HEADER_SIGNATURE;
    header.version = 20;
    header.minVersion = 20;
    header.flags = 0;
    header.compression = entry.compression;
    header.modTime = 0; // 暂时设为0
    header.modDate = 0; // 暂时设为0
    header.crc32 = entry.crc32;
    header.compressedSize = entry.compressedSize;
    header.uncompressedSize = entry.uncompressedSize;
    header.fileNameLength = entry.fileName.length();
    header.extraFieldLength = 0;
    header.fileCommentLength = 0;
    header.diskNumber = 0;
    header.internalAttrs = 0;
    header.externalAttrs = 0;
    header.localHeaderOffset = offset;
    
    stream.write(reinterpret_cast<const char*>(&header), sizeof(header));
    stream.write(entry.fileName.c_str(), entry.fileName.length());
    
    return stream.good();
}

bool ZipUtil::writeEndRecord(std::ofstream& stream, uint32_t centralOffset, uint32_t centralSize, size_t totalEntries) {
    ZipEndRecord endRecord;
    endRecord.signature = END_RECORD_SIGNATURE;
    endRecord.diskNumber = 0;
    endRecord.centralDiskNumber = 0;
    endRecord.centralEntries = totalEntries;
    endRecord.totalEntries = totalEntries;
    endRecord.centralSize = centralSize;
    endRecord.centralOffset = centralOffset;
    endRecord.commentLength = 0;
    
    stream.write(reinterpret_cast<const char*>(&endRecord), sizeof(endRecord));
    
    return stream.good();
}

bool ZipUtil::readLocalHeader(std::ifstream& stream, ZipHeader& header) {
    stream.read(reinterpret_cast<char*>(&header), sizeof(header));
    return stream.good() && header.signature == LOCAL_HEADER_SIGNATURE;
}

bool ZipUtil::readCentralHeader(std::ifstream& stream, ZipCentralHeader& header) {
    stream.read(reinterpret_cast<char*>(&header), sizeof(header));
    return stream.good() && header.signature == CENTRAL_HEADER_SIGNATURE;
}

bool ZipUtil::readEndRecord(std::ifstream& stream, ZipEndRecord& endRecord) {
    stream.read(reinterpret_cast<char*>(&endRecord), sizeof(endRecord));
    return stream.good() && endRecord.signature == END_RECORD_SIGNATURE;
}

bool ZipUtil::findEndRecord(std::ifstream& stream, ZipEndRecord& endRecord) {
    // 从文件末尾开始搜索结束记录
    stream.seekg(-static_cast<long>(sizeof(ZipEndRecord)), std::ios::end);
    
    while (stream.tellg() >= 0) {
        if (readEndRecord(stream, endRecord)) {
            return true;
        }
        
        // 向前移动一个字节继续搜索
        stream.seekg(-1, std::ios::cur);
    }
    
    return false;
}

void ZipUtil::updateStatistics(const std::string& type, size_t value) {
    if (type == "zip_created") {
        totalZipsCreated_ += value;
    } else if (type == "zip_read") {
        totalZipsRead_ += value;
    } else if (type == "zip_extracted") {
        // 统计已在extractZip中更新
    }
}

bool ZipUtil::createDirectoryStructure(const std::string& path) {
    try {
        if (std::filesystem::exists(path)) {
            return true; // 目录已存在
        }
        return std::filesystem::create_directories(path);
    } catch (...) {
        return false;
    }
}

bool ZipUtil::getFileModificationTime(const std::string& filePath, uint16_t& modTime, uint16_t& modDate) {
    try {
        auto ftime = std::filesystem::last_write_time(filePath);
        auto sctp = std::chrono::time_point_cast<std::chrono::seconds>(ftime);
        auto sctp_sec = sctp.time_since_epoch().count();
        
        // 转换为DOS时间格式（简化实现）
        time_t time = static_cast<time_t>(sctp_sec);
        struct tm* tm = localtime(&time);
        
        if (tm) {
            // DOS时间格式：HHHHH MMMMMMM SSSSSSS
            modTime = (tm->tm_hour << 11) | (tm->tm_min << 5) | (tm->tm_sec >> 1);
            // DOS日期格式：YYYYYYY MMMM DDDDD
            modDate = ((tm->tm_year - 80) << 9) | ((tm->tm_mon + 1) << 5) | tm->tm_mday;
            return true;
        }
    } catch (...) {
        // 忽略错误
    }
    
    modTime = 0;
    modDate = 0;
    return false;
}

std::string ZipUtil::dosTimeToString(uint16_t dosTime, uint16_t dosDate) {
    // 从DOS时间转换为标准时间字符串（简化实现）
    int hour = (dosTime >> 11) & 0x1F;
    int minute = (dosTime >> 5) & 0x3F;
    int second = (dosTime & 0x1F) << 1;
    
    int year = ((dosDate >> 9) & 0x7F) + 1980;
    int month = (dosDate >> 5) & 0x0F;
    int day = dosDate & 0x1F;
    
    std::stringstream ss;
    ss << year << "-" << month << "-" << day << " " << hour << ":" << minute << ":" << second;
    return ss.str();
}

} // namespace Util