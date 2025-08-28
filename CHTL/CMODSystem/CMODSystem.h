#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <filesystem>

namespace CHTL {

// CMOD文件类型
enum class CMODFileType {
    SOURCE,     // 源代码文件
    INFO,       // 信息文件
    EXPORT      // 导出文件
};

// CMOD模块信息
struct CMODModuleInfo {
    std::string name;
    std::string version;
    std::string description;
    std::string author;
    std::string license;
    std::string dependencies;
    std::string category;
    std::string minCHTLVersion;
    std::string maxCHTLVersion;
    std::string sourceFile;
    
    CMODModuleInfo() = default;
};

// CMOD导出项
struct CMODExportItem {
    std::string type;        // @Style, @Element, @Var, @Origin等
    std::string name;        // 项目名称
    std::string sourceFile;  // 源文件
    size_t line;             // 行号
    
    CMODExportItem(const std::string& type, const std::string& name,
                   const std::string& sourceFile = "", size_t line = 0)
        : type(type), name(name), sourceFile(sourceFile), line(line) {}
};

// CMOD模块结构
struct CMODModule {
    std::string name;
    std::string sourcePath;
    std::string infoPath;
    CMODModuleInfo info;
    std::vector<CMODExportItem> exports;
    std::vector<std::string> subModules;
    std::unordered_map<std::string, std::shared_ptr<CMODModule>> nestedModules;
    
    CMODModule(const std::string& name = "") : name(name) {}
};

// CMOD打包选项
struct CMODPackOptions {
    bool includeSourceFiles;
    bool includeInfoFiles;
    bool includeExportFiles;
    bool compressFiles;
    std::string outputFormat;  // .cmod, .zip等
    
    CMODPackOptions() : includeSourceFiles(true), includeInfoFiles(true),
                        includeExportFiles(true), compressFiles(false), outputFormat(".cmod") {}
};

// CMOD系统管理器
class CMODSystem {
public:
    CMODSystem();
    ~CMODSystem() = default;
    
    // 模块创建
    bool createModule(const std::string& name, const std::string& sourcePath);
    bool createSubModule(const std::string& parentName, const std::string& childName, 
                        const std::string& sourcePath);
    
    // 模块信息管理
    bool setModuleInfo(const std::string& moduleName, const CMODModuleInfo& info);
    CMODModuleInfo getModuleInfo(const std::string& moduleName);
    
    // 导出项管理
    bool addExportItem(const std::string& moduleName, const CMODExportItem& item);
    std::vector<CMODExportItem> getExportItems(const std::string& moduleName);
    bool removeExportItem(const std::string& moduleName, const std::string& itemName);
    
    // 模块打包
    bool packModule(const std::string& moduleName, const std::string& outputPath, 
                   const CMODPackOptions& options = CMODPackOptions());
    bool packAllModules(const std::string& outputPath, const CMODPackOptions& options = CMODPackOptions());
    
    // 模块解包
    bool unpackModule(const std::string& cmodPath, const std::string& outputPath);
    std::shared_ptr<CMODModule> loadModule(const std::string& cmodPath);
    
    // 模块验证
    bool validateModule(const std::string& moduleName);
    std::vector<std::string> getValidationErrors(const std::string& moduleName);
    
    // 模块查询
    std::shared_ptr<CMODModule> getModule(const std::string& name);
    std::vector<std::string> getModuleNames() const;
    bool hasModule(const std::string& name) const;
    size_t getModuleCount() const { return modules_.size(); }
    
    // 路径解析
    std::string resolveModulePath(const std::string& moduleName);
    std::vector<std::string> resolveSubModulePath(const std::string& modulePath);
    
    // 依赖管理
    bool checkDependencies(const std::string& moduleName);
    std::vector<std::string> getMissingDependencies(const std::string& moduleName);
    bool resolveDependencies(const std::string& moduleName);
    
    // 模块搜索
    std::vector<std::string> searchModules(const std::string& pattern);
    std::vector<std::string> searchModulesByCategory(const std::string& category);
    std::vector<std::string> searchModulesByAuthor(const std::string& author);
    
    // 模块安装
    bool installModule(const std::string& cmodPath, const std::string& installPath);
    bool uninstallModule(const std::string& moduleName);
    
    // 模块更新
    bool updateModule(const std::string& moduleName, const std::string& newCmodPath);
    bool checkForUpdates(const std::string& moduleName);
    
    // 统计信息
    struct CMODStatistics {
        size_t totalModules;
        size_t totalSubModules;
        size_t totalExports;
        size_t packedModules;
        size_t unpackedModules;
        
        CMODStatistics() : totalModules(0), totalSubModules(0), totalExports(0),
                           packedModules(0), unpackedModules(0) {}
    };
    
    const CMODStatistics& getStatistics() const { return statistics_; }
    void clearStatistics();
    
    // 调试信息
    std::string getDebugInfo() const;
    std::string getModuleInfo(const std::string& name) const;
    
    // 重置
    void reset();
    
private:
    std::unordered_map<std::string, std::shared_ptr<CMODModule>> modules_;
    std::string basePath_;
    CMODStatistics statistics_;
    
    // 辅助函数
    bool isValidModuleName(const std::string& name) const;
    bool isValidModulePath(const std::string& path) const;
    std::string normalizeModulePath(const std::string& path);
    
    // 文件操作
    bool createDirectoryStructure(const std::string& path);
    bool copyDirectory(const std::string& source, const std::string& destination);
    bool removeDirectory(const std::string& path);
    std::vector<std::string> listFiles(const std::string& directory, const std::string& pattern);
    
    // 打包逻辑
    bool packModuleFiles(const std::shared_ptr<CMODModule>& module, const std::string& outputPath,
                        const CMODPackOptions& options);
    bool createCMODArchive(const std::string& modulePath, const std::string& outputPath,
                          const CMODPackOptions& options);
    
    // 解包逻辑
    bool extractCMODArchive(const std::string& cmodPath, const std::string& outputPath);
    bool parseCMODInfo(const std::string& infoPath, CMODModuleInfo& info);
    bool parseCMODExports(const std::string& exportPath, std::vector<CMODExportItem>& exports);
    
    // 验证逻辑
    bool validateModuleStructure(const std::shared_ptr<CMODModule>& module);
    bool validateModuleInfo(const CMODModuleInfo& info);
    bool validateExportItems(const std::vector<CMODExportItem>& exports);
    
    // 依赖解析
    std::vector<std::string> parseDependencies(const std::string& dependencies);
    bool hasCircularDependency(const std::string& moduleName, std::unordered_set<std::string>& visited);
    
    // 统计更新
    void updateStatistics(const std::string& type, size_t value = 1);
    
    // 配置相关
    std::string getDefaultInstallPath() const;
    std::string getModuleCachePath() const;
    bool isModuleInstalled(const std::string& moduleName) const;
};

} // namespace CHTL