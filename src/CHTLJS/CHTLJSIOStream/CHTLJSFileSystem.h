#ifndef CHTLJS_FILE_SYSTEM_H
#define CHTLJS_FILE_SYSTEM_H

#include <string>
#include <vector>
#include <optional>
#include <filesystem>

namespace CHTLJS {

namespace CHTLJSFile {
    // 读取文件到字符串
    std::optional<std::string> readToString(const std::string& path);
    
    // 写入字符串到文件
    bool writeString(const std::string& path, const std::string& content);
    
    // 检查文件是否存在
    bool exists(const std::string& path);
}

namespace CHTLJSDirectory {
    // 检查目录是否存在
    bool exists(const std::string& path);
    
    // 创建目录（包括父目录）
    bool create(const std::string& path);
    
    // 列出目录内容
    std::vector<std::string> list(const std::string& path);
}

} // namespace CHTLJS

#endif // CHTLJS_FILE_SYSTEM_H