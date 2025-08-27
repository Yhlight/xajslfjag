#ifndef CHTL_CMOD_SYSTEM_H
#define CHTL_CMOD_SYSTEM_H

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

namespace CHTL {

// CMOD信息结构
struct CMODInfo {
    std::string name;
    std::string version;
    std::string description;
    std::string author;
    std::string license;
    std::string dependencies;
    std::string category;
    std::string minCHTLVersion;
    std::string maxCHTLVersion;
};

// CMOD导出项
struct CMODExport {
    enum Type {
        CUSTOM_STYLE,
        CUSTOM_ELEMENT,
        CUSTOM_VAR,
        TEMPLATE_STYLE,
        TEMPLATE_ELEMENT,
        TEMPLATE_VAR,
        ORIGIN_HTML,
        ORIGIN_STYLE,
        ORIGIN_JAVASCRIPT,
        CONFIGURATION_CONFIG
    };
    
    Type type;
    std::vector<std::string> names;
};

// CMOD模块
class CMODModule {
private:
    std::string modulePath;
    std::string moduleName;
    CMODInfo info;
    std::vector<CMODExport> exports;
    std::unordered_map<std::string, std::unique_ptr<CMODModule>> subModules;
    
    // 源文件内容
    std::unordered_map<std::string, std::string> sourceFiles;
    
public:
    CMODModule(const std::string& name, const std::string& path);
    ~CMODModule() = default;
    
    // 信息访问
    const std::string& getName() const { return moduleName; }
    const std::string& getPath() const { return modulePath; }
    const CMODInfo& getInfo() const { return info; }
    
    // 导出管理
    void addExport(const CMODExport& exp);
    const std::vector<CMODExport>& getExports() const { return exports; }
    bool hasExport(const std::string& name, CMODExport::Type type) const;
    
    // 子模块管理
    void addSubModule(std::unique_ptr<CMODModule> subModule);
    CMODModule* getSubModule(const std::string& name);
    const std::unordered_map<std::string, std::unique_ptr<CMODModule>>& getSubModules() const { 
        return subModules; 
    }
    
    // 源文件管理
    void addSourceFile(const std::string& fileName, const std::string& content);
    std::string getSourceFile(const std::string& fileName) const;
    
    // 加载和保存
    bool load();
    bool save();
};

// CMOD系统管理器
class CMODSystem {
private:
    std::unordered_map<std::string, std::unique_ptr<CMODModule>> loadedModules;
    std::vector<std::string> searchPaths;
    
    // 模块缓存
    std::unordered_map<std::string, std::string> moduleCache;
    
    // 辅助方法
    std::string findModule(const std::string& moduleName);
    bool isValidCMODStructure(const std::string& path);
    
public:
    CMODSystem();
    ~CMODSystem() = default;
    
    // 搜索路径管理
    void addSearchPath(const std::string& path);
    const std::vector<std::string>& getSearchPaths() const { return searchPaths; }
    
    // 模块加载
    CMODModule* loadModule(const std::string& moduleName);
    CMODModule* loadModuleFromPath(const std::string& path);
    
    // 模块查询
    CMODModule* getModule(const std::string& moduleName);
    bool hasModule(const std::string& moduleName) const;
    
    // 打包和解包
    bool packModule(const std::string& sourceDir, const std::string& outputPath);
    bool unpackModule(const std::string& cmodPath, const std::string& outputDir);
    
    // 官方模块前缀支持
    bool isOfficialModule(const std::string& moduleName) const;
    std::string resolveOfficialModule(const std::string& moduleName) const;
    
    // 清理
    void clearCache();
    void unloadModule(const std::string& moduleName);
};

} // namespace CHTL

#endif // CHTL_CMOD_SYSTEM_H