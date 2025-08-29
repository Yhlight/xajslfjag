#pragma once

#include "../Util/Common.h"
#include <memory>
#include <vector>
#include <map>
#include <functional>

namespace CHTL::Module {

// 模块类型枚举
enum class ModuleType {
    CHTL_MODULE,     // .chtl模块
    CMOD_MODULE,     // .cmod包模块
    CJMOD_MODULE,    // .cjmod扩展模块
    JS_MODULE,       // .js模块
    CSS_MODULE,      // .css模块
    BINARY_MODULE,   // 二进制模块
    PLUGIN_MODULE    // 插件模块
};

// 模块状态枚举
enum class ModuleStatus {
    NOT_LOADED,
    LOADING,
    LOADED,
    ERROR,
    UNLOADING,
    UNLOADED
};

// 模块依赖信息
struct ModuleDependency {
    String moduleName;
    String version;
    bool isOptional;
    bool isDevDependency;
    String source; // 来源：local, remote, builtin
    
    ModuleDependency(const String& name, const String& ver = "", 
                    bool optional = false, bool dev = false);
};

// 模块元数据
struct ModuleMetadata {
    String name;
    String version;
    String description;
    String author;
    String license;
    ModuleType type;
    String entryPoint;
    std::vector<ModuleDependency> dependencies;
    StringUnorderedMap exports;
    StringUnorderedMap config;
    String sourceFile;
    std::time_t lastModified;
    
    ModuleMetadata();
    String toString() const;
    String toJSON() const;
};

// 模块接口基类
class IModule {
public:
    virtual ~IModule() = default;
    
    // 模块生命周期
    virtual bool initialize() = 0;
    virtual bool load() = 0;
    virtual bool unload() = 0;
    virtual bool reload() = 0;
    
    // 模块信息
    virtual const ModuleMetadata& getMetadata() const = 0;
    virtual ModuleStatus getStatus() const = 0;
    virtual String getLastError() const = 0;
    
    // 模块导出
    virtual bool hasExport(const String& name) const = 0;
    virtual String getExport(const String& name) const = 0;
    virtual StringUnorderedMap getAllExports() const = 0;
    
    // 模块配置
    virtual void setConfig(const String& key, const String& value) = 0;
    virtual String getConfig(const String& key) const = 0;
    virtual StringUnorderedMap getAllConfig() const = 0;
};

// 标准模块实现
class StandardModule : public IModule {
public:
    explicit StandardModule(const ModuleMetadata& metadata);
    virtual ~StandardModule();
    
    // 实现IModule接口
    bool initialize() override;
    bool load() override;
    bool unload() override;
    bool reload() override;
    
    const ModuleMetadata& getMetadata() const override;
    ModuleStatus getStatus() const override;
    String getLastError() const override;
    
    bool hasExport(const String& name) const override;
    String getExport(const String& name) const override;
    StringUnorderedMap getAllExports() const override;
    
    void setConfig(const String& key, const String& value) override;
    String getConfig(const String& key) const override;
    StringUnorderedMap getAllConfig() const override;
    
    // 扩展功能
    void addExport(const String& name, const String& value);
    void removeExport(const String& name);
    void setStatus(ModuleStatus status);
    void setError(const String& error);
    
protected:
    ModuleMetadata metadata_;
    ModuleStatus status_;
    String lastError_;
    StringUnorderedMap exports_;
    StringUnorderedMap config_;
    
    // 虚拟方法供子类重写
    virtual bool doInitialize();
    virtual bool doLoad();
    virtual bool doUnload();
    virtual void onStatusChanged(ModuleStatus oldStatus, ModuleStatus newStatus);
};

// 模块加载器接口
class IModuleLoader {
public:
    virtual ~IModuleLoader() = default;
    
    virtual bool canLoad(const String& modulePath) const = 0;
    virtual std::unique_ptr<IModule> loadModule(const String& modulePath) = 0;
    virtual bool validateModule(const String& modulePath) const = 0;
    virtual ModuleMetadata extractMetadata(const String& modulePath) const = 0;
};

// CHTL模块加载器
class CHTLModuleLoader : public IModuleLoader {
public:
    bool canLoad(const String& modulePath) const override;
    std::unique_ptr<IModule> loadModule(const String& modulePath) override;
    bool validateModule(const String& modulePath) const override;
    ModuleMetadata extractMetadata(const String& modulePath) const override;
    
private:
    bool parseCHTLModule(const String& content, ModuleMetadata& metadata) const;
};

// CMOD模块加载器
class CMODModuleLoader : public IModuleLoader {
public:
    bool canLoad(const String& modulePath) const override;
    std::unique_ptr<IModule> loadModule(const String& modulePath) override;
    bool validateModule(const String& modulePath) const override;
    ModuleMetadata extractMetadata(const String& modulePath) const override;
    
private:
    bool unpackCMOD(const String& cmodPath, String& tempDir) const;
    bool parseCMODInfo(const String& infoContent, ModuleMetadata& metadata) const;
};

// 模块管理器
class ModuleManager {
public:
    static ModuleManager& getInstance();
    
    // 模块加载器管理
    void registerLoader(ModuleType type, std::unique_ptr<IModuleLoader> loader);
    void unregisterLoader(ModuleType type);
    IModuleLoader* getLoader(ModuleType type);
    
    // 模块管理
    bool loadModule(const String& modulePath, const String& alias = "");
    bool unloadModule(const String& moduleName);
    bool reloadModule(const String& moduleName);
    
    // 模块查询
    IModule* getModule(const String& moduleName);
    const IModule* getModule(const String& moduleName) const;
    bool hasModule(const String& moduleName) const;
    StringVector getLoadedModules() const;
    size_t getModuleCount() const;
    
    // 依赖管理
    bool resolveDependencies(const String& moduleName);
    bool checkDependencies(const String& moduleName) const;
    StringVector getMissingDependencies(const String& moduleName) const;
    std::vector<ModuleDependency> getAllDependencies(const String& moduleName) const;
    
    // 模块搜索
    void addSearchPath(const String& path);
    void removeSearchPath(const String& path);
    StringVector getSearchPaths() const;
    String findModule(const String& moduleName) const;
    StringVector findAllModules(const String& pattern = "*") const;
    
    // 模块缓存
    void enableCache(bool enable = true);
    bool isCacheEnabled() const;
    void clearCache();
    void refreshCache();
    
    // 模块验证
    bool validateModule(const String& modulePath) const;
    bool validateAllModules() const;
    StringVector getInvalidModules() const;
    
    // 模块配置
    void setGlobalConfig(const String& key, const String& value);
    String getGlobalConfig(const String& key) const;
    void setModuleConfig(const String& moduleName, const String& key, const String& value);
    
    // 模块事件
    using ModuleEventCallback = std::function<void(const String& moduleName, ModuleStatus status)>;
    void setModuleEventCallback(ModuleEventCallback callback);
    
    // 模块信息
    String getModuleReport() const;
    String getModuleStatistics() const;
    std::vector<ModuleMetadata> getAllModuleMetadata() const;
    
    // 清理
    void unloadAllModules();
    void reset();

private:
    ModuleManager() = default;
    ~ModuleManager() = default;
    
    std::map<ModuleType, std::unique_ptr<IModuleLoader>> loaders_;
    std::map<String, std::unique_ptr<IModule>> modules_;
    StringVector searchPaths_;
    StringUnorderedMap globalConfig_;
    bool cacheEnabled_;
    ModuleEventCallback eventCallback_;
    
    // 缓存
    std::map<String, ModuleMetadata> metadataCache_;
    std::map<String, std::time_t> cacheTimestamps_;
    
    ModuleType detectModuleType(const String& modulePath) const;
    bool loadDependency(const ModuleDependency& dependency);
    void notifyModuleEvent(const String& moduleName, ModuleStatus status);
    bool isCacheValid(const String& modulePath) const;
    void updateCache(const String& modulePath, const ModuleMetadata& metadata);
};

// 模块工厂
class ModuleFactory {
public:
    template<typename ModuleClass>
    static void registerModuleType(const String& typeName);
    
    static std::unique_ptr<IModule> createModule(const String& typeName, const ModuleMetadata& metadata);
    static StringVector getRegisteredTypes();
    
private:
    using ModuleCreator = std::function<std::unique_ptr<IModule>(const ModuleMetadata&)>;
    static std::map<String, ModuleCreator> creators_;
};

// 模块守卫 (RAII)
class ModuleGuard {
public:
    explicit ModuleGuard(const String& modulePath);
    ~ModuleGuard();
    
    IModule* getModule();
    bool isLoaded() const;
    
private:
    String moduleName_;
    bool shouldUnload_;
};

// 便捷宏
#define CHTL_REGISTER_MODULE_TYPE(ModuleClass, typeName) \
    CHTL::Module::ModuleFactory::registerModuleType<ModuleClass>(typeName)

#define CHTL_MODULE_GUARD(modulePath) \
    CHTL::Module::ModuleGuard _module_guard(modulePath)

#define CHTL_LOAD_MODULE(modulePath) \
    CHTL::Module::ModuleManager::getInstance().loadModule(modulePath)

#define CHTL_GET_MODULE(moduleName) \
    CHTL::Module::ModuleManager::getInstance().getModule(moduleName)

} // namespace CHTL::Module