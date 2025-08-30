#include "ZIPUtil.h"
#include <fstream>
#include <sstream>
#include <iostream>

namespace CHTL {
namespace Util {

bool ZIPUtil::createZIP(const std::vector<ZIPEntry>& entries, const std::string& zipPath) {
    std::ofstream zipFile(zipPath, std::ios::binary);
    if (!zipFile.is_open()) {
        return false;
    }
    
    // 简化的ZIP文件创建
    // 写入文件头
    zipFile << "PK\x03\x04"; // ZIP文件签名
    
    for (const auto& entry : entries) {
        writeZIPHeader(zipFile, entry);
        zipFile << entry.content;
    }
    
    // 写入中央目录（简化）
    zipFile << "PK\x01\x02"; // 中央目录签名
    
    // 写入结束记录
    zipFile << "PK\x05\x06"; // 结束记录签名
    
    zipFile.close();
    return true;
}

std::vector<ZIPEntry> ZIPUtil::extractZIP(const std::string& zipPath) {
    std::vector<ZIPEntry> entries;
    
    std::ifstream zipFile(zipPath, std::ios::binary);
    if (!zipFile.is_open()) {
        return entries;
    }
    
    // 简化的ZIP文件解压
    std::string buffer((std::istreambuf_iterator<char>(zipFile)),
                       std::istreambuf_iterator<char>());
    
    // 查找文件条目（简化实现）
    size_t pos = 0;
    while (pos < buffer.length()) {
        if (buffer.substr(pos, 4) == "PK\x03\x04") {
            // 找到文件头
            ZIPEntry entry = readZIPHeader(zipFile);
            if (!entry.filename.empty()) {
                entries.push_back(entry);
            }
            pos += 30; // 跳过文件头
        } else {
            pos++;
        }
    }
    
    zipFile.close();
    return entries;
}

bool ZIPUtil::addFileToZIP(const std::string& zipPath, const ZIPEntry& entry) {
    // 简化实现：重新创建ZIP文件
    std::vector<ZIPEntry> entries = extractZIP(zipPath);
    entries.push_back(entry);
    return createZIP(entries, zipPath);
}

std::string ZIPUtil::extractFileFromZIP(const std::string& zipPath, const std::string& filename) {
    std::vector<ZIPEntry> entries = extractZIP(zipPath);
    
    for (const auto& entry : entries) {
        if (entry.filename == filename) {
            return entry.content;
        }
    }
    
    return "";
}

std::vector<std::string> ZIPUtil::listZIPContents(const std::string& zipPath) {
    std::vector<std::string> filenames;
    std::vector<ZIPEntry> entries = extractZIP(zipPath);
    
    for (const auto& entry : entries) {
        filenames.push_back(entry.filename);
    }
    
    return filenames;
}

bool ZIPUtil::validateZIP(const std::string& zipPath) {
    std::ifstream zipFile(zipPath, std::ios::binary);
    if (!zipFile.is_open()) {
        return false;
    }
    
    // 检查ZIP文件签名
    char signature[4];
    zipFile.read(signature, 4);
    
    bool isValid = (signature[0] == 'P' && signature[1] == 'K' && 
                   signature[2] == '\x03' && signature[3] == '\x04');
    
    zipFile.close();
    return isValid;
}

uint32_t ZIPUtil::calculateCRC32(const std::string& data) {
    // 简化的CRC32计算
    uint32_t crc = 0xFFFFFFFF;
    
    for (char c : data) {
        crc ^= static_cast<uint32_t>(c);
        for (int i = 0; i < 8; ++i) {
            if (crc & 1) {
                crc = (crc >> 1) ^ 0xEDB88320;
            } else {
                crc >>= 1;
            }
        }
    }
    
    return crc ^ 0xFFFFFFFF;
}

void ZIPUtil::writeZIPHeader(std::ostream& out, const ZIPEntry& entry) {
    // 简化的ZIP文件头写入
    out << "PK\x03\x04"; // 文件头签名
    out << "\x14\x00";   // 版本
    out << "\x00\x00";   // 标志
    out << "\x00\x00";   // 压缩方法
    out << "\x00\x00";   // 修改时间
    out << "\x00\x00";   // 修改日期
    
    uint32_t crc = calculateCRC32(entry.content);
    out.write(reinterpret_cast<const char*>(&crc), 4); // CRC32
    
    uint32_t size = static_cast<uint32_t>(entry.content.length());
    out.write(reinterpret_cast<const char*>(&size), 4); // 压缩大小
    out.write(reinterpret_cast<const char*>(&size), 4); // 未压缩大小
    
    uint16_t nameLen = static_cast<uint16_t>(entry.filename.length());
    out.write(reinterpret_cast<const char*>(&nameLen), 2); // 文件名长度
    
    out << "\x00\x00";   // 额外字段长度
    out << entry.filename; // 文件名
}

ZIPEntry ZIPUtil::readZIPHeader(std::istream& in) {
    ZIPEntry entry;
    
    // 简化的ZIP文件头读取
    char signature[4];
    in.read(signature, 4);
    
    if (signature[0] != 'P' || signature[1] != 'K') {
        return entry; // 无效签名
    }
    
    // 跳过版本、标志、压缩方法等
    in.seekg(18, std::ios::cur);
    
    // 读取文件名长度
    uint16_t nameLen;
    in.read(reinterpret_cast<char*>(&nameLen), 2);
    
    // 跳过额外字段长度
    in.seekg(2, std::ios::cur);
    
    // 读取文件名
    if (nameLen > 0) {
        std::vector<char> nameBuffer(nameLen);
        in.read(nameBuffer.data(), nameLen);
        entry.filename = std::string(nameBuffer.begin(), nameBuffer.end());
    }
    
    return entry;
}

} // namespace Util
} // namespace CHTL