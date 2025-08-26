#ifndef CHTL_PATH_MANAGER_H
#define CHTL_PATH_MANAGER_H

#include <string>
#include <vector>
#include <unordered_set>

namespace CHTL {

// 路径类型
enum class PathType {
    ABSOLUTE,           // 绝对路径
    RELATIVE,           // 相对路径
    MODULE_NAME,        // 模块名（不带路径）
    WILDCARD,           // 通配符路径
    SUBMODULE          // 子模块路径（如 Chtholly.Space）
};

// 路径信息
struct PathInfo {
    std::string originalPath;    // 原始路径
    std::string resolvedPath;    // 解析后的路径
    PathType type;              // 路径类型
    bool exists;                // 路径是否存在
    bool isDirectory;           // 是否为目录
    std::string extension;      // 文件扩展名
    std::string errorMessage;   // 错误信息
    
    PathInfo() : type(PathType::RELATIVE), exists(false), isDirectory(false) {}
};

// 路径管理器 - 负责所有路径相关操作
class PathManager {
private:
    // 官方模块目录
    std::string officialModuleDir;
    
    // 搜索路径缓存
    std::vector<std::string> moduleSearchPaths;
    
    // 官方模块前缀
    static const std::string OFFICIAL_MODULE_PREFIX;
    
    // 辅助方法
    PathType detectPathType(const std::string& path) const;
    std::string normalizePathSeparators(const std::string& path) const;
    std::vector<std::string> expandWildcard(const std::string& pattern, const std::string& basePath) const;
    
public:
    PathManager();
    ~PathManager() = default;
    
    // 初始化
    void setOfficialModuleDirectory(const std::string& dir);
    void addModuleSearchPath(const std::string& path);
    void clearSearchPaths();
    
    // 路径类型检测
    bool isAbsolutePath(const std::string& path) const;
    bool isRelativePath(const std::string& path) const;
    bool isModuleName(const std::string& path) const;
    bool isWildcardPath(const std::string& path) const;
    bool isSubmodulePath(const std::string& path) const;
    bool isOfficialModule(const std::string& moduleName) const;
    
    // 路径解析
    PathInfo resolvePath(const std::string& path, const std::string& basePath) const;
    std::string resolveAbsolutePath(const std::string& path) const;
    std::string resolveRelativePath(const std::string& path, const std::string& basePath) const;
    std::string joinPaths(const std::string& base, const std::string& relative) const;
    
    // 模块路径解析
    std::string resolveModulePath(const std::string& moduleName, const std::string& currentPath, const std::vector<std::string>& extensions) const;
    std::vector<std::string> findModuleInSearchPaths(const std::string& moduleName, const std::vector<std::string>& extensions) const;
    std::string resolveOfficialModulePath(const std::string& moduleName, const std::vector<std::string>& extensions) const;
    
    // 子模块解析
    std::pair<std::string, std::string> splitSubmodulePath(const std::string& path) const;
    std::vector<std::string> resolveSubmodules(const std::string& parentModule, const std::string& currentPath) const;
    
    // 通配符处理
    std::vector<std::string> resolveWildcardPath(const std::string& pattern, const std::string& basePath) const;
    std::vector<std::string> findFilesMatchingPattern(const std::string& pattern, const std::string& directory) const;
    
    // 文件系统操作
    bool fileExists(const std::string& path) const;
    bool directoryExists(const std::string& path) const;
    std::string getFileExtension(const std::string& path) const;
    std::string getFileName(const std::string& path) const;
    std::string getFileNameWithoutExtension(const std::string& path) const;
    std::string getDirectory(const std::string& path) const;
    
    // 路径规范化
    std::string normalizePath(const std::string& path) const;
    std::string makeRelativePath(const std::string& path, const std::string& basePath) const;
    std::string stripOfficialPrefix(const std::string& moduleName) const;
    
    // 验证
    bool isValidPath(const std::string& path) const;
    bool isValidModuleName(const std::string& name) const;
    std::string validatePath(const std::string& path) const;
};

} // namespace CHTL

#endif // CHTL_PATH_MANAGER_H