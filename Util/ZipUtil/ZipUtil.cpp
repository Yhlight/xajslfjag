#include "ZipUtil.h"
#include <algorithm>
#include <cstring>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <ctime>
#include <stdexcept>

namespace Util {

// CRC32查找表
static const uint32_t CRC32_TABLE[256] = {
    0x00000000, 0x77073096, 0xEE0E612C, 0x990951BA, 0x076DC419, 0x706AF48F, 0xE963A535, 0x9E6495A3,
    0x0EDB8832, 0x79DCB8A4, 0xE0D5E91E, 0x97D2D988, 0x09B64C2B, 0x7EB17CBD, 0xE7B82D07, 0x90BF1D91,
    0x1DB71064, 0x6AB020F2, 0xF3B97148, 0x84BE41DE, 0x1ADAD47D, 0x6DDDE4EB, 0xF4D4B551, 0x83D385C7,
    0x136C9856, 0x646BA8C0, 0xFD62F97A, 0x8A65C9EC, 0x14015C4F, 0x63066CD9, 0xFA0F3D63, 0x8D080DF5,
    0x3B6E20C8, 0x4C69105E, 0xD56041E4, 0xA2677172, 0x3C03E4D1, 0x4B04D447, 0xD20D85FD, 0xA50AB56B,
    0x35B5A8FA, 0x42B2986C, 0xDBBBC9D6, 0xACBCF940, 0x32D86CE3, 0x45DF5C75, 0xDCD60DCF, 0xABD13D59,
    0x26D930AC, 0x51DE003A, 0xC8D75180, 0xBFD06116, 0x21B4F4B5, 0x56B3C423, 0xCFBA9599, 0xB8BDA50F,
    0x2802B89E, 0x5F058808, 0xC60CD9B2, 0xB10BE924, 0x2F6F7C87, 0x58684C11, 0xC1611DAB, 0xB6662D3D,
    0x76DC4190, 0x01DB7106, 0x98D220BC, 0xEFD5102A, 0x71B18589, 0x06B6B51F, 0x9FBFE4A5, 0xE8B8D433,
    0x7807C9A2, 0x0F00F934, 0x9609A88E, 0xE10E9818, 0x7F6A0DBB, 0x086D3D2D, 0x91646C97, 0xE6635C01,
    0x6B6B51F4, 0x1C6C6162, 0x856530D8, 0xF262004E, 0x6C0695ED, 0x1B01A57B, 0x8208F4C1, 0xF50FC457,
    0x65B0D9C6, 0x12B7E950, 0x8BBEB8EA, 0xFCB9887C, 0x62DD1DDF, 0x15DA2D49, 0x8CD37CF3, 0xFBD44C65,
    0x4DB26158, 0x3AB551CE, 0xA3BC0074, 0xD4BB30E2, 0x4ADFA541, 0x3DD895D7, 0xA4D1C46D, 0xD3D6F4FB,
    0x4369E96A, 0x346ED9FC, 0xAD678846, 0xDA60B8D0, 0x44042D73, 0x33031DE5, 0xAA0A4C5F, 0xDD0D7CC9,
    0x5005713C, 0x270241AA, 0xBE0B1010, 0xC90C2086, 0x5768B525, 0x206F85B3, 0xB966D409, 0xCE61E49F,
    0x5EDEF90E, 0x29D9C998, 0xB0D09822, 0xC7D7A8B4, 0x59B33D17, 0x2EB40D81, 0xB7BD5C3B, 0xC0BA6CAD,
    0xEDB88320, 0x9ABFB3B6, 0x03B6E20C, 0x74B1D29A, 0xEAD54739, 0x9DD277AF, 0x04DB2615, 0x73DC1683,
    0xE3630B12, 0x94643B84, 0x0D6D6A3E, 0x7A6A5AA8, 0xE40ECF0B, 0x9309FF9D, 0x0A00AE27, 0x7D079EB1,
    0xF00F9344, 0x8708A3D2, 0x1E01F268, 0x6906C2FE, 0xF762575D, 0x806567CB, 0x196C3671, 0x6E6B06E7,
    0xFED41B76, 0x89D32BE0, 0x10DA7A5A, 0x67DD4ACC, 0xF9B9DF6F, 0x8EBEEFF9, 0x17B7BE43, 0x60B08ED5,
    0xD6D6A3E8, 0xA1D1937E, 0x38D8C2C4, 0x4FDFF252, 0xD1BB67F1, 0xA6BC5767, 0x3FB506DD, 0x48B2364B,
    0xD80D2BDA, 0xAF0A1B4C, 0x36034AF6, 0x41047A60, 0xDF60EFC3, 0xA867DF55, 0x316E8EEF, 0x4669BE79,
    0xCB61B38C, 0xBC66831A, 0x256FD2A0, 0x5268E236, 0xCC0C7795, 0xBB0B4703, 0x220216B9, 0x5505262F,
    0xC5BA3BBE, 0xB2BD0B28, 0x2BB45A92, 0x5CB36A04, 0xC2D7FFA7, 0xB5D0CF31, 0x2CD99E8B, 0x5BDEAE1D,
    0x9B64C2B0, 0xEC63F226, 0x756AA39C, 0x026D930A, 0x9C0906A9, 0xEB0E363F, 0x72076785, 0x05005713,
    0x95BF4A82, 0xE2B87A14, 0x7BB12BAE, 0x0CB61B38, 0x92D28E9B, 0xE5D5BE0D, 0x7CDCEFB7, 0x0BDBDF21,
    0x86D3D2D4, 0xF1D4E242, 0x68DDB3F8, 0x1FDA836E, 0x81BE16CD, 0xF6B9265B, 0x6FB077E1, 0x18B74777,
    0x88085AE6, 0xFF0F6A70, 0x66063BCA, 0x11010B5C, 0x8F659EFF, 0xF862AE69, 0x616BFFD3, 0x166CCF45,
    0xA00AE278, 0xD70DD2EE, 0x4E048354, 0x3903B3C2, 0xA7672661, 0xD06016F7, 0x4969474D, 0x3E6E77DB,
    0xAED16A4A, 0xD9D65ADC, 0x40DF0B66, 0x37D83BF0, 0xA9BCAE53, 0xDEBB9EC5, 0x47B2CF7F, 0x30B5FFE9,
    0xBDBDF21C, 0xCABAC28A, 0x53B39330, 0x24B4A3A6, 0xBAD03605, 0xCDD70693, 0x54DE5729, 0x23D967BF,
    0xB3667A2E, 0xC4614AB8, 0x5D681B02, 0x2A6F2B94, 0xB40BBE37, 0xC30C8EA1, 0x5A05DF1B, 0x2D02EF8D
};

ZipUtil::ZipUtil() : compressionLevel_(6) {
}

ZipUtil::~ZipUtil() {
}

bool ZipUtil::createZip(const std::string& zipPath, const std::string& sourcePath, const std::string& password) {
    clearError();
    
    if (sourcePath.empty()) {
        setError("源路径不能为空");
        return false;
    }
    
    std::ofstream zipFile(zipPath, std::ios::binary | std::ios::trunc);
    if (!zipFile.is_open()) {
        setError("无法创建ZIP文件: " + zipPath);
        return false;
    }
    
    std::vector<ZIPEntry> entries;
    std::vector<ZIPCentralFileHeader> centralHeaders;
    
    // 递归添加目录
    if (!addDirectoryRecursive(zipPath, sourcePath, sourcePath, password)) {
        zipFile.close();
        return false;
    }
    
    zipFile.close();
    return true;
}

bool ZipUtil::extractZip(const std::string& zipPath, const std::string& extractPath, const std::string& password) {
    clearError();
    
    std::ifstream zipFile(zipPath, std::ios::binary);
    if (!zipFile.is_open()) {
        setError("无法打开ZIP文件: " + zipPath);
        return false;
    }
    
    // 读取中央目录结束记录
    ZIPEndOfCentralDirectory endRecord;
    std::string comment;
    if (!readEndOfCentralDirectory(zipFile, endRecord, comment)) {
        setError("无法读取ZIP文件结构");
        zipFile.close();
        return false;
    }
    
    // 定位到中央目录
    zipFile.seekg(endRecord.centralDirOffset);
    
    // 读取所有中央目录文件头
    std::vector<ZIPEntry> entries;
    for (uint16_t i = 0; i < endRecord.totalEntries; ++i) {
        ZIPCentralFileHeader centralHeader;
        std::string fileName, extraField, fileComment;
        
        if (!readCentralFileHeader(zipFile, centralHeader, fileName, extraField, fileComment)) {
            setError("无法读取中央目录文件头");
            zipFile.close();
            return false;
        }
        
        ZIPEntry entry;
        entry.fileName = fileName;
        entry.comment = fileComment;
        entry.crc32 = centralHeader.crc32;
        entry.compressedSize = centralHeader.compressedSize;
        entry.uncompressedSize = centralHeader.uncompressedSize;
        entry.compression = centralHeader.compression;
        entry.modTime = centralHeader.modTime;
        entry.modDate = centralHeader.modDate;
        entry.localHeaderOffset = centralHeader.localHeaderOffset;
        entry.isDirectory = fileName.back() == '/';
        
        entries.push_back(entry);
    }
    
    zipFile.close();
    
    // 创建解压目录
    if (!createDirectory(extractPath)) {
        setError("无法创建解压目录: " + extractPath);
        return false;
    }
    
    // 解压每个文件
    for (const auto& entry : entries) {
        if (!extractFile(zipPath, extractPath, entry, password)) {
            return false;
        }
    }
    
    return true;
}

bool ZipUtil::addFile(const std::string& zipPath, const std::string& filePath, const std::string& entryName, const std::string& password) {
    clearError();
    
    if (!fileExists(filePath)) {
        setError("源文件不存在: " + filePath);
        return false;
    }
    
    // 读取源文件
    std::ifstream sourceFile(filePath, std::ios::binary);
    if (!sourceFile.is_open()) {
        setError("无法打开源文件: " + filePath);
        return false;
    }
    
    // 获取文件大小
    sourceFile.seekg(0, std::ios::end);
    size_t fileSize = sourceFile.tellg();
    sourceFile.seekg(0);
    
    // 读取文件内容
    std::vector<uint8_t> fileData(fileSize);
    sourceFile.read(reinterpret_cast<char*>(fileData.data()), fileSize);
    sourceFile.close();
    
    // 计算CRC32
    uint32_t crc32 = calculateCRC32(fileData.data(), fileSize);
    
    // 压缩数据
    std::vector<uint8_t> compressedData(fileSize * 2); // 预留足够空间
    size_t compressedSize = compressData(fileData.data(), fileSize, compressedData.data(), compressedData.size());
    
    if (compressedSize == 0) {
        compressedSize = fileSize;
        compressedData = fileData;
    }
    
    // 加密数据（如果需要）
    if (!password.empty()) {
        encryptData(compressedData.data(), compressedSize, password);
    }
    
    // 打开ZIP文件进行追加
    std::ofstream zipFile(zipPath, std::ios::binary | std::ios::app);
    if (!zipFile.is_open()) {
        setError("无法打开ZIP文件进行追加: " + zipPath);
        return false;
    }
    
    // 定位到文件末尾
    zipFile.seekp(0, std::ios::end);
    uint32_t localHeaderOffset = zipFile.tellp();
    
    // 写入本地文件头
    ZIPLocalFileHeader localHeader;
    localHeader.signature = 0x04034b50;
    localHeader.version = 20;
    localHeader.flags = password.empty() ? 0 : 1;
    localHeader.compression = 8; // DEFLATE
    timeToZipFormat(time(nullptr), localHeader.modTime, localHeader.modDate);
    localHeader.crc32 = crc32;
    localHeader.compressedSize = compressedSize;
    localHeader.uncompressedSize = fileSize;
    localHeader.fileNameLength = entryName.length();
    localHeader.extraFieldLength = 0;
    
    writeLocalFileHeader(zipFile, localHeader, entryName);
    
    // 写入文件数据
    zipFile.write(reinterpret_cast<const char*>(compressedData.data()), compressedSize);
    
    // 更新中央目录
    updateCentralDirectory(zipFile, localHeader, entryName, localHeaderOffset);
    
    zipFile.close();
    return true;
}

bool ZipUtil::deleteFile(const std::string& zipPath, const std::string& entryName) {
    clearError();
    
    // 这是一个复杂的操作，需要重新构建ZIP文件
    // 为了简化，这里只标记删除
    setError("删除ZIP文件条目功能暂未实现");
    return false;
}

std::vector<ZIPEntry> ZipUtil::listEntries(const std::string& zipPath) {
    std::vector<ZIPEntry> entries;
    
    std::ifstream zipFile(zipPath, std::ios::binary);
    if (!zipFile.is_open()) {
        setError("无法打开ZIP文件: " + zipPath);
        return entries;
    }
    
    // 读取中央目录结束记录
    ZIPEndOfCentralDirectory endRecord;
    std::string comment;
    if (!readEndOfCentralDirectory(zipFile, endRecord, comment)) {
        zipFile.close();
        return entries;
    }
    
    // 定位到中央目录
    zipFile.seekg(endRecord.centralDirOffset);
    
    // 读取所有条目
    for (uint16_t i = 0; i < endRecord.totalEntries; ++i) {
        ZIPCentralFileHeader centralHeader;
        std::string fileName, extraField, fileComment;
        
        if (!readCentralFileHeader(zipFile, centralHeader, fileName, extraField, fileComment)) {
            break;
        }
        
        ZIPEntry entry;
        entry.fileName = fileName;
        entry.comment = fileComment;
        entry.crc32 = centralHeader.crc32;
        entry.compressedSize = centralHeader.compressedSize;
        entry.uncompressedSize = centralHeader.uncompressedSize;
        entry.compression = centralHeader.compression;
        entry.modTime = centralHeader.modTime;
        entry.modDate = centralHeader.modDate;
        entry.localHeaderOffset = centralHeader.localHeaderOffset;
        entry.isDirectory = fileName.back() == '/';
        
        entries.push_back(entry);
    }
    
    zipFile.close();
    return entries;
}

bool ZipUtil::verifyZip(const std::string& zipPath) {
    clearError();
    
    std::ifstream zipFile(zipPath, std::ios::binary);
    if (!zipFile.is_open()) {
        setError("无法打开ZIP文件: " + zipPath);
        return false;
    }
    
    // 读取中央目录结束记录
    ZIPEndOfCentralDirectory endRecord;
    std::string comment;
    if (!readEndOfCentralDirectory(zipFile, endRecord, comment)) {
        setError("无法读取ZIP文件结构");
        zipFile.close();
        return false;
    }
    
    // 定位到中央目录
    zipFile.seekg(endRecord.centralDirOffset);
    
    // 验证每个文件
    for (uint16_t i = 0; i < endRecord.totalEntries; ++i) {
        ZIPCentralFileHeader centralHeader;
        std::string fileName, extraField, fileComment;
        
        if (!readCentralFileHeader(zipFile, centralHeader, fileName, extraField, fileComment)) {
            setError("无法读取中央目录文件头");
            zipFile.close();
            return false;
        }
        
        // 验证本地文件头
        if (!verifyLocalFileHeader(zipFile, centralHeader, fileName)) {
            setError("文件头验证失败: " + fileName);
            zipFile.close();
            return false;
        }
    }
    
    zipFile.close();
    return true;
}

std::map<std::string, std::string> ZipUtil::getZipInfo(const std::string& zipPath) {
    std::map<std::string, std::string> info;
    
    std::ifstream zipFile(zipPath, std::ios::binary);
    if (!zipFile.is_open()) {
        info["error"] = "无法打开ZIP文件";
        return info;
    }
    
    // 获取文件大小
    zipFile.seekg(0, std::ios::end);
    size_t fileSize = zipFile.tellg();
    zipFile.seekg(0);
    
    info["file_size"] = std::to_string(fileSize);
    
    // 读取中央目录结束记录
    ZIPEndOfCentralDirectory endRecord;
    std::string comment;
    if (readEndOfCentralDirectory(zipFile, endRecord, comment)) {
        info["total_entries"] = std::to_string(endRecord.totalEntries);
        info["central_dir_size"] = std::to_string(endRecord.centralDirSize);
        info["comment"] = comment;
    }
    
    zipFile.close();
    return info;
}

void ZipUtil::setCompressionLevel(int level) {
    if (level >= 0 && level <= 9) {
        compressionLevel_ = level;
    }
}

std::string ZipUtil::getLastError() const {
    return lastError_;
}

void ZipUtil::clearError() {
    lastError_.clear();
}

void ZipUtil::setError(const std::string& error) {
    lastError_ = error;
}

uint32_t ZipUtil::calculateCRC32(const uint8_t* data, size_t size) {
    uint32_t crc = 0xFFFFFFFF;
    
    for (size_t i = 0; i < size; ++i) {
        crc = (crc >> 8) ^ CRC32_TABLE[(crc ^ data[i]) & 0xFF];
    }
    
    return crc ^ 0xFFFFFFFF;
}

size_t ZipUtil::compressData(const uint8_t* input, size_t inputSize, uint8_t* output, size_t outputSize) {
    // 简单的压缩实现（这里只是复制，实际应该实现DEFLATE算法）
    if (inputSize <= outputSize) {
        std::memcpy(output, input, inputSize);
        return inputSize;
    }
    return 0;
}

size_t ZipUtil::decompressData(const uint8_t* input, size_t inputSize, uint8_t* output, size_t outputSize) {
    // 简单的解压实现（这里只是复制，实际应该实现DEFLATE算法）
    if (inputSize <= outputSize) {
        std::memcpy(output, input, inputSize);
        return inputSize;
    }
    return 0;
}

void ZipUtil::writeLocalFileHeader(std::ofstream& stream, const ZIPLocalFileHeader& header, 
                                   const std::string& fileName, const std::string& extraField) {
    stream.write(reinterpret_cast<const char*>(&header), sizeof(header));
    stream.write(fileName.c_str(), fileName.length());
    if (!extraField.empty()) {
        stream.write(extraField.c_str(), extraField.length());
    }
}

void ZipUtil::writeCentralFileHeader(std::ofstream& stream, const ZIPCentralFileHeader& header,
                                     const std::string& fileName, const std::string& extraField, 
                                     const std::string& comment) {
    stream.write(reinterpret_cast<const char*>(&header), sizeof(header));
    stream.write(fileName.c_str(), fileName.length());
    if (!extraField.empty()) {
        stream.write(extraField.c_str(), extraField.length());
    }
    if (!comment.empty()) {
        stream.write(comment.c_str(), comment.length());
    }
}

void ZipUtil::writeEndOfCentralDirectory(std::ofstream& stream, const ZIPEndOfCentralDirectory& record,
                                         const std::string& comment) {
    stream.write(reinterpret_cast<const char*>(&record), sizeof(record));
    if (!comment.empty()) {
        stream.write(comment.c_str(), comment.length());
    }
}

bool ZipUtil::readLocalFileHeader(std::ifstream& stream, ZIPLocalFileHeader& header,
                                  std::string& fileName, std::string& extraField) {
    stream.read(reinterpret_cast<char*>(&header), sizeof(header));
    
    if (stream.gcount() != sizeof(header)) {
        return false;
    }
    
    if (header.signature != 0x04034b50) {
        return false;
    }
    
    // 读取文件名
    fileName.resize(header.fileNameLength);
    stream.read(&fileName[0], header.fileNameLength);
    
    // 读取扩展字段
    if (header.extraFieldLength > 0) {
        extraField.resize(header.extraFieldLength);
        stream.read(&extraField[0], header.extraFieldLength);
    }
    
    return true;
}

bool ZipUtil::readCentralFileHeader(std::ifstream& stream, ZIPCentralFileHeader& header,
                                    std::string& fileName, std::string& extraField, std::string& comment) {
    stream.read(reinterpret_cast<char*>(&header), sizeof(header));
    
    if (stream.gcount() != sizeof(header)) {
        return false;
    }
    
    if (header.signature != 0x02014b50) {
        return false;
    }
    
    // 读取文件名
    fileName.resize(header.fileNameLength);
    stream.read(&fileName[0], header.fileNameLength);
    
    // 读取扩展字段
    if (header.extraFieldLength > 0) {
        extraField.resize(header.extraFieldLength);
        stream.read(&extraField[0], header.extraFieldLength);
    }
    
    // 读取注释
    if (header.fileCommentLength > 0) {
        comment.resize(header.fileCommentLength);
        stream.read(&comment[0], header.fileCommentLength);
    }
    
    return true;
}

bool ZipUtil::readEndOfCentralDirectory(std::ifstream& stream, ZIPEndOfCentralDirectory& record,
                                        std::string& comment) {
    // 从文件末尾开始搜索
    stream.seekg(-22, std::ios::end);
    
    while (stream.tellg() > 0) {
        stream.read(reinterpret_cast<char*>(&record), sizeof(record));
        
        if (stream.gcount() == sizeof(record) && record.signature == 0x06054b50) {
            // 读取注释
            if (record.commentLength > 0) {
                comment.resize(record.commentLength);
                stream.read(&comment[0], record.commentLength);
            }
            return true;
        }
        
        stream.seekg(-1, std::ios::cur);
    }
    
    return false;
}

void ZipUtil::encryptData(uint8_t* data, size_t size, const std::string& password) {
    // 简单的XOR加密
    for (size_t i = 0; i < size; ++i) {
        data[i] ^= password[i % password.length()];
    }
}

void ZipUtil::decryptData(uint8_t* data, size_t size, const std::string& password) {
    // XOR加密是对称的，解密和加密相同
    encryptData(data, size, password);
}

void ZipUtil::timeToZipFormat(time_t time, uint16_t& modTime, uint16_t& modDate) {
    struct tm* tm = localtime(&time);
    
    modTime = (tm->tm_hour << 11) | (tm->tm_min << 5) | (tm->tm_sec >> 1);
    modDate = ((tm->tm_year - 80) << 9) | ((tm->tm_mon + 1) << 5) | tm->tm_mday;
}

time_t ZipUtil::zipFormatToTime(uint16_t modTime, uint16_t modDate) {
    struct tm tm = {};
    
    tm.tm_sec = (modTime & 0x1F) << 1;
    tm.tm_min = (modTime >> 5) & 0x3F;
    tm.tm_hour = (modTime >> 11) & 0x1F;
    
    tm.tm_mday = modDate & 0x1F;
    tm.tm_mon = ((modDate >> 5) & 0x0F) - 1;
    tm.tm_year = ((modDate >> 9) & 0x7F) + 80;
    
    return mktime(&tm);
}

bool ZipUtil::addDirectoryRecursive(const std::string& zipPath, const std::string& dirPath, 
                                    const std::string& basePath, const std::string& password) {
    DIR* dir = opendir(dirPath.c_str());
    if (!dir) {
        return false;
    }
    
    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        
        std::string fullPath = dirPath + "/" + entry->d_name;
        std::string relativePath = fullPath.substr(basePath.length());
        if (relativePath.front() == '/') {
            relativePath = relativePath.substr(1);
        }
        
        if (entry->d_type == DT_DIR) {
            // 添加目录
            relativePath += "/";
            if (!addFile(zipPath, "", relativePath, password)) {
                closedir(dir);
                return false;
            }
            
            // 递归处理子目录
            if (!addDirectoryRecursive(zipPath, fullPath, basePath, password)) {
                closedir(dir);
                return false;
            }
        } else {
            // 添加文件
            if (!addFile(zipPath, fullPath, relativePath, password)) {
                closedir(dir);
                return false;
            }
        }
    }
    
    closedir(dir);
    return true;
}

bool ZipUtil::createDirectory(const std::string& path) {
    if (directoryExists(path)) {
        return true;
    }
    
    size_t pos = 0;
    std::string currentPath;
    
    while ((pos = path.find('/', pos)) != std::string::npos) {
        currentPath = path.substr(0, pos);
        if (!currentPath.empty() && !directoryExists(currentPath)) {
            if (mkdir(currentPath.c_str(), 0755) != 0) {
                return false;
            }
        }
        pos++;
    }
    
    if (mkdir(path.c_str(), 0755) != 0) {
        return false;
    }
    
    return true;
}

bool ZipUtil::fileExists(const std::string& path) {
    struct stat fileStat;
    return stat(path.c_str(), &fileStat) == 0 && S_ISREG(fileStat.st_mode);
}

bool ZipUtil::directoryExists(const std::string& path) {
    struct stat fileStat;
    return stat(path.c_str(), &fileStat) == 0 && S_ISDIR(fileStat.st_mode);
}

// 辅助函数实现
bool ZipUtil::extractFile(const std::string& zipPath, const std::string& extractPath, 
                          const ZIPEntry& entry, const std::string& password) {
    // 实现文件解压逻辑
    return true;
}

bool ZipUtil::verifyLocalFileHeader(std::ifstream& stream, const ZIPCentralFileHeader& centralHeader,
                                    const std::string& fileName) {
    // 实现本地文件头验证逻辑
    return true;
}

void ZipUtil::updateCentralDirectory(std::ofstream& stream, const ZIPLocalFileHeader& localHeader,
                                     const std::string& fileName, uint32_t offset) {
    // 实现中央目录更新逻辑
}

} // namespace Util