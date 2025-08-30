#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <filesystem>
#include <regex>

namespace CHTL {

enum class ModuleType {
    CHTL,
    CMOD,
    CJMOD,
    HYBRID  // CMOD + CJMOD混合模块
};

enum class DirectoryStructure {
    MIXED,      // 乱序结构：文件混杂
    CLASSIFIED  // 分类结构：CMOD/CJMOD文件夹分类
};

struct ModuleFile {
    std::string name;
    std::string path;
    std::string extension;
    ModuleType type;
};

struct SubModuleInfo {
    std::string name;
    std::string path;
    ModuleType type;
    std::vector<std::string> files;
};

struct ModuleInfo {
    std::string name;
    std::string path;
    std::string version;
    std::string description;
    std::string author;
    std::string license;
    std::string category;
    std::string minCHTLVersion;
    std::string maxCHTLVersion;
    ModuleType type;
    DirectoryStructure structure;
    std::vector<std::string> dependencies;
    std::vector<SubModuleInfo> subModules;
    std::unordered_map<std::string, std::string> exportTable;  // Export查询表
};

class ModuleManager {
public:
    ModuleManager();
    ~ModuleManager();
    
    // 基本模块管理
    bool loadModule(const std::string& modulePath);
    bool unloadModule(const std::string& moduleName);
    ModuleInfo getModuleInfo(const std::string& moduleName);
    std::vector<std::string> getLoadedModules();
    
    // 模块路径解析 - 严格按照语法文档要求
    bool resolveModulePath(const std::string& moduleName, std::string& resolvedPath);
    bool resolveSubModulePath(const std::string& parentModule, const std::string& subModule, std::string& resolvedPath);
    
    // 目录结构支持 - 支持乱序和分类结构
    DirectoryStructure detectDirectoryStructure(const std::string& directoryPath);
    bool scanMixedStructure(const std::string& modulePath, std::vector<ModuleFile>& files);
    bool scanClassifiedStructure(const std::string& modulePath, std::vector<ModuleFile>& files);
    
    // 分类文件夹检测 - 支持CMOD/cmod/Cmod等变体
    bool detectCMODFolder(const std::string& path);
    bool detectCJMODFolder(const std::string& path);
    std::string normalizeFolderName(const std::string& folderName, ModuleType type);
    
    // 官方模块支持 - chtl::前缀
    std::string getOfficialModuleDirectory();
    bool searchInOfficialModules(const std::string& moduleName, std::string& foundPath);
    bool isOfficialModulePrefix(const std::string& moduleName);
    std::string removeOfficialPrefix(const std::string& moduleName);
    
    // 子模块系统 - 嵌套子模块支持
    std::vector<SubModuleInfo> scanSubModules(const std::string& modulePath);
    bool loadSubModule(const std::string& parentModule, const std::string& subModule);
    
    // 批量导入支持 - .*和/*通配符
    std::vector<std::string> resolveBatchImport(const std::string& pattern);
    bool isWildcardPattern(const std::string& pattern);
    std::vector<std::string> expandWildcardPattern(const std::string& pattern);
    
    // 混合模块支持 - CMOD+CJMOD
    bool loadHybridModule(const std::string& modulePath);
    bool separateCMODAndCJMOD(const std::string& hybridPath, std::string& cmodPath, std::string& cjmodPath);
    
    // 模块验证 - 严格按照语法要求
    bool validateModuleStructure(const std::string& modulePath, ModuleType expectedType);
    bool validateSameNameConstraint(const std::string& modulePath);
    bool validateInfoFile(const std::string& infoPath);
    
    // 导入语法支持
    bool resolveImportPath(const std::string& importPath, const std::string& currentDirectory, std::vector<std::string>& resolvedPaths);
    bool resolveHTMLStyleJSImport(const std::string& importPath, const std::string& currentDirectory, const std::string& fileType, std::string& resolvedPath);
    bool resolveCHTLImport(const std::string& importPath, const std::string& currentDirectory, std::vector<std::string>& resolvedPaths);
    bool resolveCJMODImport(const std::string& importPath, const std::string& currentDirectory, std::string& resolvedPath);
    
private:
    std::unordered_map<std::string, ModuleInfo> m_loadedModules;
    std::vector<std::string> m_modulePaths;
    std::string m_officialModuleDirectory;
    
    void reportError(const std::string& message);
    bool parseModuleInfo(const std::string& infoPath, ModuleInfo& info);
    bool parseInfoBlock(const std::string& content, ModuleInfo& info);
    bool parseExportBlock(const std::string& content, std::unordered_map<std::string, std::string>& exportTable);
    
    // 文件系统辅助
    std::vector<std::string> listFilesInDirectory(const std::string& directory, const std::string& extension = "");
    bool fileExists(const std::string& path);
    bool directoryExists(const std::string& path);
    std::string getFileExtension(const std::string& filename);
    std::string getBaseName(const std::string& filename);
    std::string getDirectoryName(const std::string& path);
    
    // 搜索策略实现
    std::vector<std::string> getSearchPaths(const std::string& currentDirectory);
    bool searchInDirectory(const std::string& directory, const std::string& moduleName, ModuleType preferredType, std::string& foundPath);
};

}