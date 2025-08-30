#pragma once
#include <string>
#include <vector>

namespace CHTL {
namespace Util {

/**
 * ZIP文件条目
 */
struct ZIPEntry {
    std::string filename;       // 文件名
    std::string content;        // 文件内容
    size_t size;               // 文件大小
    uint32_t crc32;            // CRC32校验和
    
    ZIPEntry(const std::string& name = "", const std::string& data = "")
        : filename(name), content(data), size(data.length()), crc32(0) {}
};

/**
 * 简单ZIP处理器
 * 为CMOD和CJMOD模块提供ZIP打包解包功能
 */
class ZIPUtil {
public:
    /**
     * 创建ZIP文件
     */
    static bool createZIP(const std::vector<ZIPEntry>& entries, const std::string& zipPath);
    
    /**
     * 解压ZIP文件
     */
    static std::vector<ZIPEntry> extractZIP(const std::string& zipPath);
    
    /**
     * 添加文件到ZIP
     */
    static bool addFileToZIP(const std::string& zipPath, const ZIPEntry& entry);
    
    /**
     * 从ZIP中提取单个文件
     */
    static std::string extractFileFromZIP(const std::string& zipPath, const std::string& filename);
    
    /**
     * 列出ZIP中的文件
     */
    static std::vector<std::string> listZIPContents(const std::string& zipPath);
    
    /**
     * 验证ZIP文件
     */
    static bool validateZIP(const std::string& zipPath);

private:
    /**
     * 计算CRC32
     */
    static uint32_t calculateCRC32(const std::string& data);
    
    /**
     * 写入ZIP文件头
     */
    static void writeZIPHeader(std::ostream& out, const ZIPEntry& entry);
    
    /**
     * 读取ZIP文件头
     */
    static ZIPEntry readZIPHeader(std::istream& in);
};

} // namespace Util
} // namespace CHTL