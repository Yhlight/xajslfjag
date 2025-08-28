#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <filesystem>

namespace CHTLJS {

// CJMOD文件类型
enum class CJMODFileType {
    SOURCE,     // C++源代码文件
    HEADER,     // C++头文件
    INFO,       // 信息文件
    BINARY      // 编译后的二进制文件
};

// CJMOD模块信息
struct CJMODModuleInfo {
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
    std::vector<std::string> supportedPlatforms;
    
    CJMODModuleInfo() = default;
};

// CJMOD导出项
struct CJMODExportItem {
    std::string type;        // 函数类型或语法扩展
    std::string name;        // 项目名称
    std::string sourceFile;  // 源文件
    size_t line;             // 行号
    std::string signature;   // 函数签名或语法模式
    
    CJMODExportItem(const std::string& type, const std::string& name,
                   const std::string& sourceFile = "", size_t line = 0,
                   const std::string& signature = "")
        : type(type), name(name), sourceFile(sourceFile), line(line), signature(signature) {}
};

// CJMOD模块结构
struct CJMODModule {
    std::string name;
    std::string sourcePath;
    std::string infoPath;
    CJMODModuleInfo info;
    std::vector<CJMODExportItem> exports;
    std::vector<std::string> subModules;
    std::unordered_map<std::string, std::shared_ptr<CJMODModule>> nestedModules;
    std::vector<std::string> cppFiles;
    std::vector<std::string> headerFiles;
    
    CJMODModule(const std::string& name = "") : name(name) {}
};

// CJMOD编译选项
struct CJMODCompileOptions {
    std::string compiler;        // 编译器路径
    std::vector<std::string> flags;  // 编译标志
    std::string outputFormat;    // 输出格式
    bool optimize;               // 是否优化
    bool debug;                  // 是否包含调试信息
    std::vector<std::string> includePaths;  // 包含路径
    std::vector<std::string> libraryPaths;  // 库路径
    std::vector<std::string> libraries;     // 链接的库
    
    CJMODCompileOptions() : optimize(true), debug(false), outputFormat(".cjmod") {}
};

// CJMOD系统管理器
class CJMODSystem {
public:
    CJMODSystem();
    ~CJMODSystem() = default;
    
    // 模块创建
    bool createModule(const std::string& name, const std::string& sourcePath);
    bool createSubModule(const std::string& parentName, const std::string& childName, 
                        const std::string& sourcePath);
    
    // 模块信息管理
    bool setModuleInfo(const std::string& moduleName, const CJMODModuleInfo& info);
    CJMODModuleInfo getModuleInfo(const std::string& moduleName);
    
    // 导出项管理
    bool addExportItem(const std::string& moduleName, const CJMODExportItem& item);
    std::vector<CJMODExportItem> getExportItems(const std::string& moduleName);
    bool removeExportItem(const std::string& moduleName, const std::string& itemName);
    
    // 模块编译
    bool compileModule(const std::string& moduleName, const CJMODCompileOptions& options = CJMODCompileOptions());
    bool compileAllModules(const CJMODCompileOptions& options = CJMODCompileOptions());
    
    // 模块打包
    bool packModule(const std::string& moduleName, const std::string& outputPath);
    bool packAllModules(const std::string& outputPath);
    
    // 模块解包
    bool unpackModule(const std::string& cjmodPath, const std::string& outputPath);
    std::shared_ptr<CJMODModule> loadModule(const std::string& cjmodPath);
    
    // 模块验证
    bool validateModule(const std::string& moduleName);
    std::vector<std::string> getValidationErrors(const std::string& moduleName);
    
    // 模块查询
    std::shared_ptr<CJMODModule> getModule(const std::string& name);
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
    bool installModule(const std::string& cjmodPath, const std::string& installPath);
    bool uninstallModule(const std::string& moduleName);
    
    // 模块更新
    bool updateModule(const std::string& moduleName, const std::string& newCjmodPath);
    bool checkForUpdates(const std::string& moduleName);
    
    // 编译管理
    bool setCompiler(const std::string& compilerPath);
    std::string getCompiler() const { return compilerPath_; }
    bool testCompiler();
    
    // 语法扩展管理
    bool registerSyntaxExtension(const std::string& moduleName, const std::string& pattern);
    std::vector<std::string> getSyntaxExtensions(const std::string& moduleName);
    bool unregisterSyntaxExtension(const std::string& moduleName, const std::string& pattern);
    
    // 函数扩展管理
    bool registerFunctionExtension(const std::string& moduleName, const std::string& functionName);
    std::vector<std::string> getFunctionExtensions(const std::string& moduleName);
    bool unregisterFunctionExtension(const std::string& moduleName, const std::string& functionName);
    
    // 统计信息
    struct CJMODStatistics {
        size_t totalModules;
        size_t totalSubModules;
        size_t totalExports;
        size_t compiledModules;
        size_t packedModules;
        size_t unpackedModules;
        size_t syntaxExtensions;
        size_t functionExtensions;
        
        CJMODStatistics() : totalModules(0), totalSubModules(0), totalExports(0),
                           compiledModules(0), packedModules(0), unpackedModules(0),
                           syntaxExtensions(0), functionExtensions(0) {}
    };
    
    const CJMODStatistics& getStatistics() const { return statistics_; }
    void clearStatistics();
    
    // 调试信息
    std::string getDebugInfo() const;
    std::string getModuleInfo(const std::string& name) const;
    
    // 重置
    void reset();
    
private:
    std::unordered_map<std::string, std::shared_ptr<CJMODModule>> modules_;
    std::string basePath_;
    std::string compilerPath_;
    CJMODStatistics statistics_;
    
    // 辅助函数
    bool isValidModuleName(const std::string& name) const;
    bool isValidModulePath(const std::string& path) const;
    std::string normalizeModulePath(const std::string& path);
    
    // 文件操作
    bool createDirectoryStructure(const std::string& path);
    bool copyDirectory(const std::string& source, const std::string& destination);
    bool removeDirectory(const std::string& path);
    std::vector<std::string> listFiles(const std::string& directory, const std::string& pattern);
    
    // 编译逻辑
    bool compileModuleFiles(const std::shared_ptr<CJMODModule>& module, 
                           const CJMODCompileOptions& options);
    bool compileCppFile(const std::string& cppFile, const std::string& outputFile,
                        const CJMODCompileOptions& options);
    bool linkModule(const std::shared_ptr<CJMODModule>& module, const std::string& outputPath,
                   const CJMODCompileOptions& options);
    
    // 打包逻辑
    bool packModuleFiles(const std::shared_ptr<CJMODModule>& module, const std::string& outputPath);
    bool createCJMODArchive(const std::string& modulePath, const std::string& outputPath);
    
    // 解包逻辑
    bool extractCJMODArchive(const std::string& cjmodPath, const std::string& outputPath);
    bool parseCJMODInfo(const std::string& infoPath, CJMODModuleInfo& info);
    bool parseCJMODExports(const std::string& exportPath, std::vector<CJMODExportItem>& exports);
    
    // 验证逻辑
    bool validateModuleStructure(const std::shared_ptr<CJMODModule>& module);
    bool validateModuleInfo(const CJMODModuleInfo& info);
    bool validateExportItems(const std::vector<CJMODExportItem>& exports);
    bool validateCppFiles(const std::vector<std::string>& cppFiles);
    
    // 依赖解析
    std::vector<std::string> parseDependencies(const std::string& dependencies);
    bool hasCircularDependency(const std::string& moduleName, std::unordered_set<std::string>& visited);
    
    // 统计更新
    void updateStatistics(const std::string& type, size_t value = 1);
    
    // 配置相关
    std::string getDefaultInstallPath() const;
    std::string getModuleCachePath() const;
    bool isModuleInstalled(const std::string& moduleName) const;
    
    // 编译器相关
    bool findSystemCompiler();
    std::string getDefaultCompiler() const;
    bool validateCompiler(const std::string& compilerPath);
};

} // namespace CHTLJS