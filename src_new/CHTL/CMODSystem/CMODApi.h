#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

namespace CHTL {

struct CMODInfo {
    std::string name;
    std::string version;
    std::string description;
    std::string author;
    std::string license;
    std::string category;
    std::string minCHTLVersion;
    std::string maxCHTLVersion;
    std::vector<std::string> dependencies;
};

struct CMODExportItem {
    std::string category;     // Custom, Template, Origin, Configuration
    std::string type;         // Style, Element, Var, Html, JavaScript等
    std::vector<std::string> items;
};

struct CMODExportTable {
    std::vector<CMODExportItem> exports;
    std::unordered_map<std::string, std::string> itemTypeMap;  // item名称 -> 完整类型
};

class CMODModule {
public:
    CMODModule() = default;
    ~CMODModule() = default;
    
    // 基本信息
    std::string getName() const { return m_info.name; }
    std::string getVersion() const { return m_info.version; }
    std::string getDescription() const { return m_info.description; }
    CMODInfo getInfo() const { return m_info; }
    
    // Export查询表
    CMODExportTable getExportTable() const { return m_exportTable; }
    bool hasExport(const std::string& itemName) const;
    std::string getExportType(const std::string& itemName) const;
    std::vector<std::string> getExportsByType(const std::string& category, const std::string& type) const;
    
    // 子模块支持
    std::vector<std::string> getSubModules() const { return m_subModules; }
    bool hasSubModule(const std::string& subModuleName) const;
    std::shared_ptr<CMODModule> getSubModule(const std::string& subModuleName) const;
    
    // 源码管理
    std::string getMainSourcePath() const { return m_mainSourcePath; }
    std::vector<std::string> getSourceFiles() const { return m_sourceFiles; }
    std::string getSourceContent(const std::string& fileName) const;
    
    // 模块加载
    bool loadFromPath(const std::string& modulePath);
    bool validateStructure() const;
    
private:
    CMODInfo m_info;
    CMODExportTable m_exportTable;
    std::string m_modulePath;
    std::string m_mainSourcePath;
    std::vector<std::string> m_sourceFiles;
    std::vector<std::string> m_subModules;
    std::unordered_map<std::string, std::shared_ptr<CMODModule>> m_subModuleMap;
    
    bool parseInfoFile(const std::string& infoPath);
    bool parseInfoBlock(const std::string& content);
    bool parseExportBlock(const std::string& content);
    bool loadSourceFiles();
    bool loadSubModules();
    void reportError(const std::string& message) const;
};

class CMODApi {
public:
    CMODApi();
    ~CMODApi();
    
    // 模块管理
    bool loadCMOD(const std::string& modulePath);
    bool unloadCMOD(const std::string& moduleName);
    std::shared_ptr<CMODModule> getCMOD(const std::string& moduleName);
    std::vector<std::string> getLoadedCMODs() const;
    
    // 模块查询
    bool hasCMOD(const std::string& moduleName) const;
    CMODInfo getCMODInfo(const std::string& moduleName) const;
    CMODExportTable getCMODExports(const std::string& moduleName) const;
    
    // 子模块访问
    std::shared_ptr<CMODModule> getSubModule(const std::string& parentModule, const std::string& subModule);
    bool hasSubModule(const std::string& parentModule, const std::string& subModule) const;
    
    // 模块搜索和解析
    bool resolveCMODPath(const std::string& moduleName, std::string& resolvedPath);
    std::vector<std::string> resolveBatchCMODImport(const std::string& pattern);
    
    // 打包和解包
    bool packCMOD(const std::string& sourceDir, const std::string& outputPath);
    bool unpackCMOD(const std::string& cmodPath, const std::string& outputDir);
    
    // 验证
    bool validateCMODStructure(const std::string& modulePath) const;
    bool validateSameNameConstraint(const std::string& modulePath) const;
    
private:
    std::unordered_map<std::string, std::shared_ptr<CMODModule>> m_loadedModules;
    
    void reportError(const std::string& message) const;
    bool fileExists(const std::string& path) const;
    bool directoryExists(const std::string& path) const;
};

}