#pragma once

#include "../../Util/Common.h"
#include "../CHTLNode/BaseNode.h"

namespace CHTL {

// CMOD文件结构
enum class CMODFileType {
    CHTL_SOURCE,        // .chtl源文件
    INFO_FILE,          // info/模块名.chtl信息文件
    SUBMODULE,          // 子模块
    UNKNOWN_FILE
};

// CMOD包结构
struct CMODStructure {
    String moduleName;              // 模块名称
    String rootPath;                // 根路径
    String srcPath;                 // src路径
    String infoPath;                // info路径
    
    // 主模块文件
    String mainModuleFile;          // src/模块名.chtl
    String infoFile;                // info/模块名.chtl
    
    // 子模块
    struct SubModule {
        String name;
        String srcPath;             // src/子模块名/src/
        String infoPath;            // src/子模块名/info/
        String mainFile;            // src/子模块名/src/子模块名.chtl
        String infoFile;            // src/子模块名/info/子模块名.chtl
        bool hasMainFile;
        
        SubModule() : hasMainFile(false) {}
    };
    
    std::vector<SubModule> subModules;
    StringVector additionalFiles;   // 其他CHTL文件
    bool isValid;
    bool hasMainModule;
    
    CMODStructure() : isValid(false), hasMainModule(false) {}
    
    bool validate() const;
    String toString() const;
    StringVector getAllFiles() const;
};

// CMOD信息
struct CMODInfo {
    String name;                    // 模块名称
    String version;                 // 版本
    String description;             // 描述
    String author;                  // 作者
    String license;                 // 许可证
    StringVector dependencies;      // 依赖
    String category;                // 类别
    String minCHTLVersion;          // 最小CHTL版本
    String maxCHTLVersion;          // 最大CHTL版本
    
    // 导出表（系统生成）
    struct ExportTable {
        StringVector customStyles;
        StringVector customElements;
        StringVector customVars;
        StringVector templateStyles;
        StringVector templateElements;
        StringVector templateVars;
        StringVector origins;
        StringVector configurations;
        
        bool isEmpty() const;
        String toString() const;
    };
    
    ExportTable exportTable;
    StringUnorderedMap metadata;    // 额外元数据
    
    CMODInfo() = default;
    
    bool isValid() const;
    String generateInfoContent() const;
    void parseFromContent(const String& content);
    String generateExportTable() const;
};

// CMOD包装器
class CMODPackage {
public:
    explicit CMODPackage(const String& packagePath);
    ~CMODPackage() = default;
    
    // 基础信息
    const String& getPackagePath() const { return packagePath; }
    const String& getModuleName() const { return structure.moduleName; }
    const CMODStructure& getStructure() const { return structure; }
    const CMODInfo& getInfo() const { return info; }
    
    // 加载和解析
    bool load();
    bool parse();
    bool isLoaded() const { return loaded; }
    bool isParsed() const { return parsed; }
    
    // 内容访问
    String getMainModuleContent() const;
    String getInfoContent() const;
    String getSubModuleContent(const String& subModuleName) const;
    StringVector getSubModuleNames() const;
    
    // AST访问
    std::unique_ptr<BaseNode> getMainModuleAST() const;
    std::unique_ptr<BaseNode> getSubModuleAST(const String& subModuleName) const;
    
    // 导出内容
    StringUnorderedMap getExports() const;
    StringVector getExportedCustomStyles() const;
    StringVector getExportedCustomElements() const;
    StringVector getExportedCustomVars() const;
    StringVector getExportedTemplateStyles() const;
    StringVector getExportedTemplateElements() const;
    StringVector getExportedTemplateVars() const;
    StringVector getExportedOrigins() const;
    StringVector getExportedConfigurations() const;
    
    // 依赖管理
    StringVector getDependencies() const { return info.dependencies; }
    bool hasDependency(const String& dependency) const;
    
    // 验证
    bool validate() const;
    StringVector getValidationErrors() const;
    
    // 版本兼容性
    bool isVersionCompatible(const String& chtlVersion) const;
    String getCompatibilityMessage(const String& chtlVersion) const;
    
    // 错误处理
    bool hasErrors() const { return !errors.empty(); }
    StringVector getErrors() const { return errors; }
    void clearErrors() { errors.clear(); }
    
private:
    String packagePath;
    CMODStructure structure;
    CMODInfo info;
    StringUnorderedMap moduleContents;   // 文件路径 -> 内容
    StringUnorderedMap moduleASTs;       // 文件路径 -> AST（序列化）
    StringVector errors;
    bool loaded;
    bool parsed;
    
    // 内部方法
    bool analyzeStructure();
    bool loadModuleContents();
    bool parseModuleInfo();
    CMODInfo parseInfoFromContent(const String& content) const;
    void generateExportTable();
    String readFile(const String& filePath) const;
    bool fileExists(const String& filePath) const;
    bool directoryExists(const String& dirPath) const;
    StringVector listDirectory(const String& dirPath) const;
    void reportError(const String& message);
};

// CMOD打包器
class CMODPackager {
public:
    struct PackagingOptions {
        bool includeMainModule = true;      // 包含主模块
        bool includeSubModules = true;      // 包含子模块
        bool generateExportTable = true;    // 生成导出表
        bool validateStructure = true;      // 验证结构
        bool compressPackage = true;        // 压缩包
        bool includeSourceMaps = false;     // 包含源映射
        String outputPath = "";             // 输出路径
        String compressionLevel = "normal"; // 压缩级别：none, fast, normal, best
    };
    
    CMODPackager();
    explicit CMODPackager(const PackagingOptions& options);
    
    // 打包方法
    bool packageDirectory(const String& sourceDir, const String& outputPath);
    bool packageFiles(const StringVector& files, const String& outputPath, const String& moduleName);
    
    // 从现有结构打包
    bool packageFromStructure(const CMODStructure& structure, const String& outputPath);
    
    // 验证和预处理
    bool validateSourceDirectory(const String& sourceDir) const;
    CMODStructure analyzeSourceDirectory(const String& sourceDir) const;
    
    // 打包统计
    struct PackagingStats {
        size_t totalFiles = 0;
        size_t totalSize = 0;
        size_t compressedSize = 0;
        double compressionRatio = 0.0;
        double packagingTime = 0.0;
        StringVector includedFiles;
        StringVector excludedFiles;
    };
    
    PackagingStats getLastPackagingStats() const { return lastStats; }
    
    // 配置
    void setOptions(const PackagingOptions& options) { this->options = options; }
    const PackagingOptions& getOptions() const { return options; }
    
    // 错误处理
    bool hasErrors() const { return !errors.empty(); }
    StringVector getErrors() const { return errors; }
    void clearErrors() { errors.clear(); }
    
private:
    PackagingOptions options;
    PackagingStats lastStats;
    StringVector errors;
    
    // 内部打包逻辑
    bool createPackageStructure(const String& tempDir, const CMODStructure& structure);
    bool copySourceFiles(const String& sourceDir, const String& tempDir, const CMODStructure& structure);
    bool generateInfoFile(const String& infoPath, const CMODInfo& info);
    bool compressDirectory(const String& sourceDir, const String& outputPath);
    
    // 文件操作
    bool copyFile(const String& source, const String& destination);
    bool createDirectory(const String& path);
    bool removeDirectory(const String& path);
    
    // 验证
    bool validateModuleStructure(const CMODStructure& structure) const;
    StringVector getStructureValidationErrors(const CMODStructure& structure) const;
    
    // 统计更新
    void updateStats(const String& filePath, bool included);
    void calculateCompressionRatio();
    
    // 错误报告
    void reportError(const String& message);
};

// CMOD解包器
class CMODUnpacker {
public:
    struct UnpackingOptions {
        bool extractToSubfolder = true;     // 解压到子文件夹
        bool preserveStructure = true;      // 保持结构
        bool validateAfterExtraction = true; // 解压后验证
        bool overwriteExisting = false;     // 覆盖已存在文件
        String extractionPath = "";         // 解压路径
    };
    
    CMODUnpacker();
    explicit CMODUnpacker(const UnpackingOptions& options);
    
    // 解包方法
    bool unpackage(const String& cmodPath, const String& outputDir);
    bool unpackageToMemory(const String& cmodPath, StringUnorderedMap& extractedFiles);
    
    // 验证
    bool validateCMODFile(const String& cmodPath) const;
    CMODStructure analyzeCMODFile(const String& cmodPath) const;
    
    // 解包统计
    struct UnpackingStats {
        size_t totalFiles = 0;
        size_t extractedFiles = 0;
        size_t skippedFiles = 0;
        size_t totalSize = 0;
        double unpackingTime = 0.0;
        StringVector extractedPaths;
        StringVector skippedPaths;
    };
    
    UnpackingStats getLastUnpackingStats() const { return lastStats; }
    
    // 配置
    void setOptions(const UnpackingOptions& options) { this->options = options; }
    const UnpackingOptions& getOptions() const { return options; }
    
    // 错误处理
    bool hasErrors() const { return !errors.empty(); }
    StringVector getErrors() const { return errors; }
    void clearErrors() { errors.clear(); }
    
private:
    UnpackingOptions options;
    UnpackingStats lastStats;
    StringVector errors;
    
    // 内部解包逻辑
    bool extractArchive(const String& archivePath, const String& outputDir);
    bool validateExtractedStructure(const String& extractedDir) const;
    
    // 文件操作
    bool ensureDirectoryExists(const String& path);
    bool shouldExtractFile(const String& filePath) const;
    
    // 统计更新
    void updateStats(const String& filePath, bool extracted);
    
    // 错误报告
    void reportError(const String& message);
};

// CMOD管理器
class CMODManager {
public:
    CMODManager();
    ~CMODManager() = default;
    
    // 包管理
    bool installPackage(const String& cmodPath, const String& installDir = "");
    bool uninstallPackage(const String& packageName);
    bool updatePackage(const String& packageName);
    
    // 包查询
    bool isPackageInstalled(const String& packageName) const;
    StringVector getInstalledPackages() const;
    CMODPackage getInstalledPackage(const String& packageName) const;
    
    // 依赖管理
    StringVector getPackageDependencies(const String& packageName) const;
    StringVector resolveDependencies(const String& packageName) const;
    bool hasCircularDependencies(const String& packageName) const;
    
    // 版本管理
    StringVector getAvailableVersions(const String& packageName) const;
    String getInstalledVersion(const String& packageName) const;
    bool isVersionCompatible(const String& packageName, const String& version) const;
    
    // 搜索和发现
    StringVector searchPackages(const String& query) const;
    StringVector searchByCategory(const String& category) const;
    StringVector searchByAuthor(const String& author) const;
    
    // 仓库管理
    void addRepository(const String& repositoryUrl);
    void removeRepository(const String& repositoryUrl);
    StringVector getRepositories() const;
    void refreshRepositories();
    
    // 缓存管理
    void clearCache();
    void updateCache();
    String getCacheDirectory() const;
    
    // 配置
    struct ManagerConfig {
        String installDirectory = "modules";
        String cacheDirectory = "cache";
        StringVector repositories;
        bool autoResolveDependencies = true;
        bool allowPrerelease = false;
        bool verifySignatures = true;
        size_t maxConcurrentDownloads = 3;
        double downloadTimeoutSeconds = 300.0;
    };
    
    void setConfig(const ManagerConfig& config) { this->config = config; }
    const ManagerConfig& getConfig() const { return config; }
    
    // 统计信息
    struct ManagerStats {
        size_t installedPackages = 0;
        size_t availablePackages = 0;
        size_t totalInstallSize = 0;
        size_t cacheSize = 0;
        StringVector recentInstalls;
        StringVector recentUpdates;
    };
    
    ManagerStats getStats() const;
    
    // 错误处理
    bool hasErrors() const { return !errors.empty(); }
    StringVector getErrors() const { return errors; }
    void clearErrors() { errors.clear(); }
    
private:
    ManagerConfig config;
    StringUnorderedMap installedPackages;  // name -> install_path
    StringUnorderedMap packageVersions;    // name -> version
    StringUnorderedMap packageInfo;        // name -> info_json
    StringVector errors;
    
    // 内部方法
    bool downloadPackage(const String& packageName, const String& version, const String& outputPath);
    bool verifyPackage(const String& packagePath) const;
    String resolvePackageUrl(const String& packageName, const String& version) const;
    
    // 依赖解析
    StringVector resolveDependenciesRecursive(const String& packageName, std::set<String>& resolved) const;
    bool checkCircularDependency(const String& packageName, std::set<String>& visiting) const;
    
    // 文件系统操作
    bool createInstallDirectory(const String& packageName);
    bool removeInstallDirectory(const String& packageName);
    
    // 注册表操作
    void registerPackage(const String& packageName, const String& installPath, const CMODInfo& info);
    void unregisterPackage(const String& packageName);
    void saveRegistry();
    void loadRegistry();
    
    // 错误报告
    void reportError(const String& message);
};

// ZIP工具集成
class ZIPUtil {
public:
    // 压缩
    static bool compressDirectory(const String& sourceDir, const String& zipPath, const String& compressionLevel = "normal");
    static bool compressFiles(const StringVector& files, const String& zipPath, const String& compressionLevel = "normal");
    
    // 解压
    static bool extractArchive(const String& zipPath, const String& outputDir);
    static bool extractToMemory(const String& zipPath, StringUnorderedMap& extractedFiles);
    
    // 查询
    static StringVector listArchiveContents(const String& zipPath);
    static bool isValidArchive(const String& zipPath);
    static size_t getArchiveSize(const String& zipPath);
    static size_t getUncompressedSize(const String& zipPath);
    
    // 验证
    static bool verifyArchiveIntegrity(const String& zipPath);
    static String calculateArchiveChecksum(const String& zipPath);
    
private:
    static bool initializeZipLibrary();
    static void cleanupZipLibrary();
};

// CMOD工厂
class CMODFactory {
public:
    static std::unique_ptr<CMODPackage> loadPackage(const String& packagePath);
    static std::unique_ptr<CMODPackager> createPackager();
    static std::unique_ptr<CMODPackager> createPackager(const CMODPackager::PackagingOptions& options);
    static std::unique_ptr<CMODUnpacker> createUnpacker();
    static std::unique_ptr<CMODUnpacker> createUnpacker(const CMODUnpacker::UnpackingOptions& options);
    static std::unique_ptr<CMODManager> createManager(const CMODManager::ManagerConfig& config = {});
    
    // 便捷方法
    static bool quickPackage(const String& sourceDir, const String& outputPath);
    static bool quickUnpackage(const String& cmodPath, const String& outputDir);
    static CMODStructure quickAnalyze(const String& path);
};

} // namespace CHTL