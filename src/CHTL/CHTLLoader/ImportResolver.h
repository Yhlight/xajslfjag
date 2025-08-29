#ifndef CHTL_IMPORT_RESOLVER_H
#define CHTL_IMPORT_RESOLVER_H

#include <string>
#include <vector>
#include <optional>
#include <unordered_set>
#include <memory>
#include "../CHTLNode/ImportNode.h"

namespace CHTL {

// 导入解析配置
struct ImportResolverConfig {
    std::string officialModuleDir;      // 官方模块目录
    std::string currentDir;             // 当前文件目录
    bool enableDefaultNamespace = true; // 启用默认命名空间
    bool checkCircularDependency = true; // 检查循环依赖
};

// 文件类型
enum class FileType {
    HTML,
    CSS,
    JAVASCRIPT,
    CHTL,
    CMOD,
    CJMOD,
    UNKNOWN
};

// 解析结果
struct ResolvedImport {
    std::string filePath;           // 解析后的文件路径
    FileType fileType;             // 文件类型
    ImportType importType;         // 导入类型
    std::string namespaceName;     // 命名空间名称（如果适用）
    bool isOfficialModule = false; // 是否为官方模块
};

// 导入解析器
class ImportResolver {
public:
    explicit ImportResolver(const ImportResolverConfig& config);
    ~ImportResolver() = default;
    
    // 解析导入节点
    std::optional<ResolvedImport> resolve(ImportNode* importNode);
    
    // 解析路径
    std::optional<std::string> resolvePath(const std::string& path, FileType expectedType);
    
    // 检查循环依赖
    bool hasCircularDependency(const std::string& fromFile, const std::string& toFile);
    
    // 添加已导入的文件
    void addImportedFile(const std::string& fromFile, const std::string& toFile);
    
    // 获取文件的默认命名空间
    std::string getDefaultNamespace(const std::string& filePath);

private:
    ImportResolverConfig config_;
    std::unordered_map<std::string, std::unordered_set<std::string>> importGraph_;
    
    // 路径解析辅助方法
    std::optional<std::string> resolveInOfficialModules(const std::string& name, FileType type);
    std::optional<std::string> resolveInCurrentModules(const std::string& name, FileType type);
    std::optional<std::string> resolveInCurrentDir(const std::string& name, FileType type);
    std::optional<std::string> resolveAbsolutePath(const std::string& path);
    
    // 搜索文件
    std::optional<std::string> searchFile(const std::string& dir, const std::string& name, 
                                          FileType type, bool checkSubdirs = false);
    
    // 文件类型相关
    FileType detectFileType(const std::string& path);
    std::vector<std::string> getFileExtensions(FileType type);
    bool matchesFileType(const std::string& path, FileType type);
    
    // 路径处理
    std::string normalizePath(const std::string& path);
    std::string joinPath(const std::string& dir, const std::string& file);
    std::string getDirectory(const std::string& path);
    std::string getFilename(const std::string& path);
    std::string getBasename(const std::string& path);
    
    // 模块目录结构处理
    bool hasModuleStructure(const std::string& dir);
    std::string getCMODSubdir(const std::string& dir);
    std::string getCJMODSubdir(const std::string& dir);
    
    // 通配符和批量导入
    std::vector<std::string> resolveWildcard(const std::string& pattern, FileType type);
    std::vector<std::string> findFilesInDir(const std::string& dir, FileType type);
    
    // 官方模块前缀处理
    bool hasOfficialModulePrefix(const std::string& path);
    std::string removeOfficialModulePrefix(const std::string& path);
};

// 文件系统接口（用于测试和跨平台）
class IFileSystem {
public:
    virtual ~IFileSystem() = default;
    
    virtual bool exists(const std::string& path) = 0;
    virtual bool isDirectory(const std::string& path) = 0;
    virtual bool isFile(const std::string& path) = 0;
    virtual std::vector<std::string> listDirectory(const std::string& path) = 0;
    virtual std::string getCurrentDirectory() = 0;
    virtual std::string getAbsolutePath(const std::string& path) = 0;
};

// 默认文件系统实现
class DefaultFileSystem : public IFileSystem {
public:
    bool exists(const std::string& path) override;
    bool isDirectory(const std::string& path) override;
    bool isFile(const std::string& path) override;
    std::vector<std::string> listDirectory(const std::string& path) override;
    std::string getCurrentDirectory() override;
    std::string getAbsolutePath(const std::string& path) override;
};

} // namespace CHTL

#endif // CHTL_IMPORT_RESOLVER_H