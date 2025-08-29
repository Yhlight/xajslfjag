#pragma once

#include "../Common.h"
#include <memory>
#include <functional>

namespace CHTL {
namespace Util {

// ZIP文件条目信息
struct ZIPEntry {
    String path;
    String name;
    size_t size;
    size_t compressedSize;
    bool isDirectory;
    String lastModified;
    uint32_t crc32;
    
    ZIPEntry() : size(0), compressedSize(0), isDirectory(false), crc32(0) {}
};

// ZIP操作结果
struct ZIPResult {
    bool success;
    StringVector errors;
    StringVector warnings;
    String message;
    
    ZIPResult() : success(false) {}
};

// ZIP压缩配置
struct ZIPConfig {
    int compressionLevel = 6;  // 0-9, 0=无压缩, 9=最高压缩
    bool preservePermissions = true;
    bool preserveTimestamps = true;
    bool createDirectories = true;
    bool overwriteExisting = true;
    StringVector excludePatterns;
    StringVector includePatterns;
    String password;  // 可选的密码保护
};

// CMOD特殊配置
struct CMODPackageConfig {
    String moduleName;
    String version;
    String description;
    String author;
    String license;
    String category;
    String minCHTLVersion;
    String maxCHTLVersion;
    StringVector dependencies;
    bool includeSource = true;
    bool includeInfo = true;
    bool validateStructure = true;
};

// ZIP管理器主类
class ZIPManager {
public:
    explicit ZIPManager(const ZIPConfig& config = ZIPConfig{});
    
    // 基础ZIP操作
    ZIPResult createZIP(const String& zipPath, const String& sourcePath);
    ZIPResult extractZIP(const String& zipPath, const String& destPath);
    ZIPResult addToZIP(const String& zipPath, const String& filePath, const String& entryPath = "");
    ZIPResult removeFromZIP(const String& zipPath, const String& entryPath);
    
    // 文件和目录操作
    ZIPResult addFile(const String& zipPath, const String& filePath, const String& entryPath);
    ZIPResult addDirectory(const String& zipPath, const String& dirPath, const String& entryPrefix = "");
    ZIPResult extractFile(const String& zipPath, const String& entryPath, const String& destPath);
    ZIPResult extractDirectory(const String& zipPath, const String& entryPrefix, const String& destPath);
    
    // 信息查询
    std::vector<ZIPEntry> listEntries(const String& zipPath);
    ZIPEntry getEntryInfo(const String& zipPath, const String& entryPath);
    bool hasEntry(const String& zipPath, const String& entryPath);
    size_t getEntryCount(const String& zipPath);
    size_t getTotalSize(const String& zipPath);
    size_t getCompressedSize(const String& zipPath);
    
    // CMOD特殊操作
    ZIPResult packCMOD(const String& cmodPath, const String& sourcePath, const CMODPackageConfig& config);
    ZIPResult unpackCMOD(const String& cmodPath, const String& destPath);
    ZIPResult validateCMOD(const String& cmodPath);
    CMODPackageConfig readCMODInfo(const String& cmodPath);
    
    // 配置管理
    void setConfig(const ZIPConfig& config);
    const ZIPConfig& getConfig() const;
    
    // 错误处理
    const StringVector& getErrors() const;
    const StringVector& getWarnings() const;
    bool hasErrors() const;
    String getErrorSummary() const;
    void clearErrors();
    
    // 进度回调
    using ProgressCallback = std::function<void(const String& operation, int progress, const String& currentFile)>;
    void setProgressCallback(ProgressCallback callback);
    
private:
    ZIPConfig config;
    StringVector errors;
    StringVector warnings;
    ProgressCallback progressCallback;
    
    // 内部ZIP操作
    bool createZIPInternal(const String& zipPath, const String& sourcePath);
    bool extractZIPInternal(const String& zipPath, const String& destPath);
    bool addFileInternal(const String& zipPath, const String& filePath, const String& entryPath);
    bool removeEntryInternal(const String& zipPath, const String& entryPath);
    
    // 文件系统操作
    bool copyFileToZIP(const String& sourcePath, const String& zipPath, const String& entryPath);
    bool extractFileFromZIP(const String& zipPath, const String& entryPath, const String& destPath);
    bool createDirectoryInZIP(const String& zipPath, const String& dirPath);
    
    // CMOD特殊处理
    bool validateCMODStructure(const String& sourcePath);
    CMODPackageConfig parseCMODInfo(const String& infoPath);
    String generateCMODInfo(const CMODPackageConfig& config);
    bool checkCMODDependencies(const CMODPackageConfig& config);
    
    // 验证和检查
    bool isValidZIPPath(const String& path);
    bool isValidEntryPath(const String& entryPath);
    bool shouldIncludeFile(const String& filePath);
    bool shouldExcludeFile(const String& filePath);
    
    // 工具方法
    String calculateCRC32(const String& data);
    String getRelativePath(const String& basePath, const String& fullPath);
    String sanitizeEntryPath(const String& entryPath);
    void reportProgress(const String& operation, int progress, const String& currentFile = "");
    
    // 错误处理
    void addError(const String& message);
    void addWarning(const String& message);
    
    // 平台特定实现
    #ifdef _WIN32
    bool createZIPWindows(const String& zipPath, const String& sourcePath);
    bool extractZIPWindows(const String& zipPath, const String& destPath);
    #else
    bool createZIPUnix(const String& zipPath, const String& sourcePath);
    bool extractZIPUnix(const String& zipPath, const String& destPath);
    #endif
};

// ZIP工厂类
class ZIPManagerFactory {
public:
    static std::unique_ptr<ZIPManager> createManager(const ZIPConfig& config = ZIPConfig{});
    static std::unique_ptr<ZIPManager> createCMODManager();
    static std::unique_ptr<ZIPManager> createFastCompressionManager();
    static std::unique_ptr<ZIPManager> createMaxCompressionManager();
    
    // 配置预设
    static ZIPConfig getDefaultConfig();
    static ZIPConfig getCMODConfig();
    static ZIPConfig getFastCompressionConfig();
    static ZIPConfig getMaxCompressionConfig();
};

// CMOD工具类
class CMODTool {
public:
    // CMOD文件操作
    static ZIPResult createCMODFromDirectory(const String& sourcePath, const String& cmodPath, 
                                           const CMODPackageConfig& config);
    static ZIPResult extractCMODToDirectory(const String& cmodPath, const String& destPath);
    static ZIPResult installCMOD(const String& cmodPath, const String& modulePath);
    static ZIPResult uninstallCMOD(const String& moduleName, const String& modulePath);
    
    // CMOD信息操作
    static CMODPackageConfig readCMODPackageInfo(const String& cmodPath);
    static bool validateCMODPackage(const String& cmodPath);
    static StringVector listCMODContents(const String& cmodPath);
    static String getCMODVersion(const String& cmodPath);
    
    // CMOD依赖管理
    static StringVector resolveDependencies(const StringVector& modules, const String& modulePath);
    static bool checkDependencyConflicts(const StringVector& modules);
    static StringVector getInstalledModules(const String& modulePath);
    
    // CMOD验证
    static bool isValidCMODStructure(const String& sourcePath);
    static bool isCompatibleVersion(const String& cmodVersion, const String& chtlVersion);
    static StringVector validateCMODInfo(const CMODPackageConfig& config);
    
private:
    static String getCMODInfoPath(const String& sourcePath);
    static String getModuleMainFile(const String& sourcePath);
    static bool createCMODInfoFile(const String& path, const CMODPackageConfig& config);
};

} // namespace Util
} // namespace CHTL