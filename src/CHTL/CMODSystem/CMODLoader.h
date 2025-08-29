#ifndef CHTL_CMOD_LOADER_H
#define CHTL_CMOD_LOADER_H

#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <optional>
#include "CMODPackager.h"

namespace CHTL {

// Forward declarations
class CompileContext;
class ASTNode;

// CMOD加载配置
struct CMODLoadConfig {
    bool autoExtractDependencies = true;   // 自动解压依赖
    bool cacheExtractedModules = true;     // 缓存已解压的模块
    std::string cacheDirectory = ".cmod_cache"; // 缓存目录
    std::string officialModulePath = "";   // 官方模块路径
};

// 模块搜索路径类型
enum class ModuleSearchPath {
    OFFICIAL,    // 官方模块目录
    LOCAL,       // 当前目录的module文件夹
    CURRENT,     // 当前目录
    ABSOLUTE     // 绝对路径
};

// CMOD加载器
class CMODLoader {
public:
    CMODLoader(std::shared_ptr<CompileContext> context);
    ~CMODLoader() = default;
    
    // 设置配置
    void setConfig(const CMODLoadConfig& config) { config_ = config; }
    
    // 加载CMOD文件
    bool loadModule(const std::string& modulePath);
    
    // 加载CHTL文件
    bool loadCHTLFile(const std::string& chtlPath);
    
    // 加载目录中的CHTL文件
    bool loadFromDirectory(const std::string& dir);
    
    // 搜索模块
    std::optional<std::string> findModule(const std::string& moduleName);
    
    // 获取已加载的模块信息
    std::optional<CMODInfo> getLoadedModuleInfo(const std::string& moduleName) const;
    
    // 获取模块导出
    std::optional<CMODExport> getModuleExports(const std::string& moduleName) const;
    
    // 检查是否已加载模块
    bool isModuleLoaded(const std::string& moduleName) const;
    
    // 获取错误信息
    const std::string& getLastError() const { return lastError_; }
    
    // 获取所有已加载的模块名
    std::vector<std::string> getLoadedModules() const;
    
private:
    std::shared_ptr<CompileContext> context_;
    CMODLoadConfig config_;
    std::string lastError_;
    
    // 已加载的模块
    struct LoadedModule {
        CMODInfo info;
        CMODExport exports;
        std::string sourcePath;
        bool isCMOD;  // true for CMOD, false for CHTL
    };
    std::unordered_map<std::string, LoadedModule> loadedModules_;
    
    // 模块缓存路径
    std::unordered_map<std::string, std::string> moduleCache_;
    
    // 内部方法
    bool extractCMOD(const std::string& cmodPath, std::string& extractPath);
    bool loadExtractedModule(const std::string& extractPath, const std::string& moduleName);
    bool processCHTLFile(const std::string& chtlPath);
    bool processModuleDependencies(const CMODInfo& info);
    
    // 路径解析
    std::string resolveModulePath(const std::string& moduleName, ModuleSearchPath searchPath);
    std::vector<std::string> getModuleSearchPaths(const std::string& moduleName);
    bool isOfficialModulePrefix(const std::string& moduleName);
    
    // 清理和验证
    bool cleanupCache();
    bool validateModuleName(const std::string& name);
    
    // 循环依赖检测
    bool checkCircularDependency(const std::string& moduleName);
    std::vector<std::string> currentLoadingChain_;
};

} // namespace CHTL

#endif // CHTL_CMOD_LOADER_H