#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

namespace CHTL {
namespace CMOD {

/**
 * CMODSystem - CMOD模块系统
 * 管理CHTL模块的加载、解析和使用
 */
class CMODSystem {
public:
    /**
     * 模块信息
     */
    struct ModuleInfo {
        std::string name;
        std::string version;
        std::string description;
        std::string author;
        std::string license;
        std::vector<std::string> dependencies;
        std::string path;
        bool is_loaded;
        
        ModuleInfo() : is_loaded(false) {}
    };

private:
    static std::unordered_map<std::string, ModuleInfo> loaded_modules_;
    static std::vector<std::string> module_search_paths_;
    static bool initialized_;

public:
    /**
     * 初始化CMOD系统
     */
    static void initialize();
    
    /**
     * 清理CMOD系统
     */
    static void cleanup();
    
    /**
     * 添加模块搜索路径
     * @param path 搜索路径
     */
    static void addSearchPath(const std::string& path);
    
    /**
     * 加载模块
     * @param module_name 模块名称
     * @return 是否成功加载
     */
    static bool loadModule(const std::string& module_name);
    
    /**
     * 卸载模块
     * @param module_name 模块名称
     * @return 是否成功卸载
     */
    static bool unloadModule(const std::string& module_name);
    
    /**
     * 检查模块是否已加载
     * @param module_name 模块名称
     * @return 是否已加载
     */
    static bool isModuleLoaded(const std::string& module_name);
    
    /**
     * 获取模块信息
     * @param module_name 模块名称
     * @return 模块信息
     */
    static ModuleInfo getModuleInfo(const std::string& module_name);
    
    /**
     * 获取所有已加载模块
     * @return 模块名称列表
     */
    static std::vector<std::string> getLoadedModules();
    
    /**
     * 搜索模块
     * @param module_name 模块名称
     * @return 模块路径
     */
    static std::string findModule(const std::string& module_name);
    
    /**
     * 解析模块信息
     * @param module_path 模块路径
     * @return 模块信息
     */
    static ModuleInfo parseModuleInfo(const std::string& module_path);
    
    /**
     * 解析模块依赖
     * @param module_info 模块信息
     * @return 依赖列表
     */
    static std::vector<std::string> resolveDependencies(const ModuleInfo& module_info);
    
    /**
     * 检查循环依赖
     * @param module_name 模块名称
     * @param visited 已访问模块
     * @return 是否有循环依赖
     */
    static bool hasCircularDependency(const std::string& module_name,
                                     std::vector<std::string>& visited);

private:
    /**
     * 加载模块文件
     * @param module_path 模块路径
     * @return 是否成功
     */
    static bool loadModuleFile(const std::string& module_path);
    
    /**
     * 解析info文件
     * @param info_path info文件路径
     * @return 模块信息
     */
    static ModuleInfo parseInfoFile(const std::string& info_path);
    
    /**
     * 验证模块结构
     * @param module_path 模块路径
     * @return 是否有效
     */
    static bool validateModuleStructure(const std::string& module_path);
};

} // namespace CMOD
} // namespace CHTL