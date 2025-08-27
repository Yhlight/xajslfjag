#ifndef CHTL_FILE_SYSTEM_H
#define CHTL_FILE_SYSTEM_H

#include <string>
#include <vector>
#include <fstream>
#include <memory>

namespace CHTL {
namespace Util {

// 文件类型枚举
enum class FileType {
    CHTL,       // .chtl文件
    HTML,       // .html文件
    CSS,        // .css文件
    JS,         // .js文件
    CMOD,       // .cmod文件
    CJMOD,      // .cjmod文件
    UNKNOWN     // 未知类型
};

// 文件信息结构
struct FileInfo {
    std::string path;
    std::string name;
    std::string extension;
    FileType type;
    size_t size;
    bool exists;
    
    FileInfo() : type(FileType::UNKNOWN), size(0), exists(false) {}
};

// 文件系统工具类
class FileSystem {
public:
    // 文件读写
    static std::string readFile(const std::string& path);
    static bool writeFile(const std::string& path, const std::string& content);
    static bool appendFile(const std::string& path, const std::string& content);
    
    // 文件/目录操作
    static bool exists(const std::string& path);
    static bool isFile(const std::string& path);
    static bool isDirectory(const std::string& path);
    static bool createDirectory(const std::string& path);
    static bool createDirectories(const std::string& path);  // 递归创建
    static bool remove(const std::string& path);
    static bool removeAll(const std::string& path);  // 递归删除
    static bool copy(const std::string& from, const std::string& to);
    static bool move(const std::string& from, const std::string& to);
    
    // 路径操作
    static std::string getAbsolutePath(const std::string& path);
    static std::string getRelativePath(const std::string& path, const std::string& base);
    static std::string getParentPath(const std::string& path);
    static std::string getFileName(const std::string& path);
    static std::string getFileNameWithoutExtension(const std::string& path);
    static std::string getExtension(const std::string& path);
    static std::string joinPath(const std::string& path1, const std::string& path2);
    static std::string normalizePath(const std::string& path);
    
    // 文件搜索
    static std::vector<std::string> listFiles(const std::string& directory);
    static std::vector<std::string> listDirectories(const std::string& directory);
    static std::vector<std::string> listAll(const std::string& directory);
    static std::vector<std::string> findFiles(const std::string& directory, const std::string& pattern, bool recursive = false);
    static std::vector<std::string> findFilesByExtension(const std::string& directory, const std::string& extension, bool recursive = false);
    
    // 文件类型判断
    static FileType getFileType(const std::string& path);
    static bool isCHTLFile(const std::string& path);
    static bool isHTMLFile(const std::string& path);
    static bool isCSSFile(const std::string& path);
    static bool isJSFile(const std::string& path);
    static bool isCMODFile(const std::string& path);
    static bool isCJMODFile(const std::string& path);
    
    // 文件信息
    static FileInfo getFileInfo(const std::string& path);
    static size_t getFileSize(const std::string& path);
    static time_t getLastModifiedTime(const std::string& path);
    
    // 模块路径处理
    static std::string getOfficialModulePath();  // 获取官方模块目录
    static std::string getUserModulePath(const std::string& workingDir);  // 获取用户模块目录
    static std::vector<std::string> getModuleSearchPaths(const std::string& workingDir);  // 获取模块搜索路径列表
    static std::string findModule(const std::string& moduleName, const std::string& workingDir);  // 查找模块
    
    // CMOD/CJMOD打包解包
    static bool packCMOD(const std::string& sourceDir, const std::string& outputPath);
    static bool unpackCMOD(const std::string& cmodPath, const std::string& outputDir);
    static bool packCJMOD(const std::string& sourceDir, const std::string& outputPath);
    static bool unpackCJMOD(const std::string& cjmodPath, const std::string& outputDir);
    
private:
    // 内部辅助方法
    static std::string readTextFile(const std::string& path);
    static bool writeTextFile(const std::string& path, const std::string& content);
    static std::vector<std::string> glob(const std::string& pattern);
};

} // namespace Util
} // namespace CHTL

#endif // CHTL_FILE_SYSTEM_H