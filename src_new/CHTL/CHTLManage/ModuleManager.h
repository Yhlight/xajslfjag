#pragma once
#include "../Error/ErrorReport.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <filesystem>

namespace CHTL {

/**
 * 模块类型枚举
 */
enum class ModuleType {
    CMOD,           // CHTL模块
    CJMOD,          // CHTL JS扩展模块
    CHTL_FILE,      // 单个CHTL文件
    UNKNOWN
};

/**
 * 模块信息结构
 */
struct ModuleInfo {
    std::string name;                   // 模块名称
    std::string version;                // 版本
    std::string description;            // 描述
    std::string author;                 // 作者
    std::string license;                // 许可证
    std::string dependencies;           // 依赖
    std::string category;               // 分类
    std::string minCHTLVersion;         // 最小CHTL版本
    std::string maxCHTLVersion;         // 最大CHTL版本
    ModuleType type;                    // 模块类型
    std::string path;                   // 模块路径
    
    ModuleInfo() : type(ModuleType::UNKNOWN) {}
};

/**
 * 模块依赖信息
 */
struct ModuleDependency {
    std::string name;                   // 依赖模块名
    std::string version;                // 版本要求
    bool required;                      // 是否必需
    
    ModuleDependency(const std::string& n = "", const std::string& v = "", bool r = true)
        : name(n), version(v), required(r) {}
};

/**
 * CMOD/CJMOD模块管理器
 * 负责模块的加载、解析、依赖管理等
 */
class ModuleManager {
public:
    /**
     * 构造函数
     */
    ModuleManager();
    
    /**
     * 析构函数
     */
    ~ModuleManager() = default;
    
    // ========== 模块路径管理 ==========
    
    /**
     * 设置官方模块目录
     */
    void setOfficialModuleDirectory(const std::string& directory);
    
    /**
     * 添加模块搜索路径
     */
    void addModuleSearchPath(const std::string& path);
    
    /**
     * 获取所有模块搜索路径
     */
    std::vector<std::string> getModuleSearchPaths() const;
    
    // ========== 模块搜索与加载 ==========
    
    /**
     * 搜索模块
     * @param moduleName 模块名称（支持官方前缀chtl::）
     * @param type 模块类型
     * @return 模块路径，如果未找到返回空字符串
     */
    std::string findModule(const std::string& moduleName, ModuleType type = ModuleType::CMOD);
    
    /**
     * 加载模块信息
     */
    std::shared_ptr<ModuleInfo> loadModuleInfo(const std::string& modulePath);
    
    /**
     * 加载模块
     */
    bool loadModule(const std::string& moduleName, ModuleType type = ModuleType::CMOD);
    
    /**
     * 检查模块是否已加载
     */
    bool isModuleLoaded(const std::string& moduleName) const;
    
    // ========== 依赖管理 ==========
    
    /**
     * 解析模块依赖
     */
    std::vector<ModuleDependency> parseModuleDependencies(const std::string& dependencies);
    
    /**
     * 解决模块依赖
     */
    bool resolveDependencies(const std::string& moduleName);
    
    /**
     * 检查依赖循环
     */
    bool checkCircularDependency(const std::string& moduleName, 
                                std::vector<std::string>& visited);
    
    // ========== 模块打包与解包 ==========
    
    /**
     * 打包模块目录为CMOD文件
     */
    bool packModule(const std::string& moduleDirectory, const std::string& outputPath);
    
    /**
     * 解包CMOD文件到目录
     */
    bool unpackModule(const std::string& cmodPath, const std::string& outputDirectory);
    
    // ========== 官方模块支持 ==========
    
    /**
     * 检查是否为官方模块前缀
     */
    bool isOfficialModulePrefix(const std::string& moduleName);
    
    /**
     * 解析官方模块名称
     */
    std::string parseOfficialModuleName(const std::string& moduleName);
    
    // ========== 工具方法 ==========
    
    /**
     * 获取模块类型从文件扩展名
     */
    ModuleType getModuleTypeFromExtension(const std::string& filePath);
    
    /**
     * 获取已加载的模块列表
     */
    std::vector<std::string> getLoadedModules() const;
    
    /**
     * 生成模块报告
     */
    std::string generateModuleReport() const;
    
    /**
     * 清理所有模块
     */
    void clear();

private:
    std::string m_officialModuleDirectory;                          // 官方模块目录
    std::vector<std::string> m_moduleSearchPaths;                   // 模块搜索路径
    std::unordered_map<std::string, std::shared_ptr<ModuleInfo>> m_loadedModules; // 已加载模块
    std::unordered_map<std::string, std::vector<std::string>> m_dependencyGraph; // 依赖图
    
    /**
     * 搜索模块在指定目录
     */
    std::string searchModuleInDirectory(const std::string& directory, 
                                       const std::string& moduleName, 
                                       ModuleType type);
    
    /**
     * 验证模块格式
     */
    bool validateModuleStructure(const std::string& modulePath);
    
    /**
     * 解析模块信息文件
     */
    std::shared_ptr<ModuleInfo> parseModuleInfoFile(const std::string& infoFilePath);
    
    /**
     * 报告错误
     */
    void reportError(const std::string& message);
};

} // namespace CHTL