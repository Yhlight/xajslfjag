#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <functional>
#include <chrono>
#include "FileSystem.h"
#include "ZipLibrary.h"

namespace CHTL {
namespace Core {

/**
 * ModuleType - 模块类型
 */
enum class ModuleType {
    CMOD,           // CHTL模块（包含.chtl文件）
    CJMOD,          // CHTL JS扩展模块（包含C++源码）
    MIXED           // 混合模块
};

/**
 * ModuleInfo - 模块信息
 */
struct ModuleInfo {
    std::string name;                    // 模块名称
    std::string version;                 // 版本号
    std::string description;             // 描述
    std::string author;                  // 作者
    std::string license;                 // 许可证
    std::string dependencies;            // 依赖项
    std::string category;                // 分类
    std::string min_chtl_version;        // 最小CHTL版本
    std::string max_chtl_version;        // 最大CHTL版本
    ModuleType type;                     // 模块类型
    std::chrono::system_clock::time_point created_time;  // 创建时间
    std::chrono::system_clock::time_point modified_time; // 修改时间
    
    ModuleInfo() : type(ModuleType::CMOD) {}
};

/**
 * ModuleExport - 模块导出项
 */
struct ModuleExport {
    enum class ExportType {
        CUSTOM_STYLE,      // [Custom] @Style
        CUSTOM_ELEMENT,    // [Custom] @Element
        CUSTOM_VAR,        // [Custom] @Var
        TEMPLATE_STYLE,    // [Template] @Style
        TEMPLATE_ELEMENT,  // [Template] @Element
        TEMPLATE_VAR,      // [Template] @Var
        ORIGIN_HTML,       // [Origin] @Html
        ORIGIN_STYLE,      // [Origin] @Style
        ORIGIN_JAVASCRIPT, // [Origin] @Javascript
        ORIGIN_CUSTOM,     // [Origin] @CustomType
        CONFIGURATION      // [Configuration] @Config
    };
    
    ExportType type;
    std::string name;
    std::string description;
    
    ModuleExport() : type(ExportType::CUSTOM_STYLE) {}
    ModuleExport(ExportType t, const std::string& n, const std::string& desc = "")
        : type(t), name(n), description(desc) {}
};

/**
 * ModuleValidationResult - 模块验证结果
 */
struct ModuleValidationResult {
    bool is_valid;                         // 是否有效
    std::vector<std::string> errors;       // 错误列表
    std::vector<std::string> warnings;     // 警告列表
    std::vector<std::string> missing_files; // 缺失文件
    ModuleInfo detected_info;              // 检测到的模块信息
    std::vector<ModuleExport> exports;     // 导出项
    
    ModuleValidationResult() : is_valid(false) {}
};

/**
 * CMODPackager - CMOD打包器
 */
class CMODPackager {
public:
    CMODPackager();
    ~CMODPackager();
    
    /**
     * 打包CMOD文件夹
     * @param source_directory 源目录路径
     * @param output_file 输出.cmod文件路径
     * @param compression_level 压缩级别
     * @return 是否成功
     */
    bool packageCMOD(const std::string& source_directory, 
                     const std::string& output_file,
                     CompressionLevel compression_level = CompressionLevel::DEFAULT_COMPRESSION);
    
    /**
     * 解包CMOD文件
     * @param cmod_file .cmod文件路径
     * @param output_directory 输出目录
     * @return 是否成功
     */
    bool unpackageCMOD(const std::string& cmod_file, const std::string& output_directory);
    
    /**
     * 验证CMOD文件夹结构
     * @param directory 目录路径
     * @return 验证结果
     */
    ModuleValidationResult validateCMODStructure(const std::string& directory);
    
    /**
     * 验证CMOD文件
     * @param cmod_file .cmod文件路径
     * @return 验证结果
     */
    ModuleValidationResult validateCMODFile(const std::string& cmod_file);
    
    /**
     * 获取CMOD信息
     * @param cmod_file .cmod文件路径
     * @return 模块信息
     */
    ModuleInfo getCMODInfo(const std::string& cmod_file);
    
    /**
     * 列出CMOD内容
     * @param cmod_file .cmod文件路径
     * @return 文件列表
     */
    std::vector<std::string> listCMODContents(const std::string& cmod_file);
    
    /**
     * 设置进度回调
     * @param callback 进度回调函数
     */
    void setProgressCallback(std::function<void(const std::string&, int)> callback);

private:
    std::unique_ptr<ZipArchive> zip_archive;
    std::function<void(const std::string&, int)> progress_callback;
    
    // 内部方法
    bool validateCMODStructureInternal(const std::string& directory, ModuleValidationResult& result);
    bool parseInfoFile(const std::string& info_file_path, ModuleInfo& info, std::vector<ModuleExport>& exports);
    bool validateMainModule(const std::string& main_module_path);
    std::vector<std::string> findSubModules(const std::string& src_directory);
    bool validateSubModule(const std::string& submodule_path);
    std::string generateExportTable(const std::string& src_directory);
    void reportProgress(const std::string& operation, int percentage);
};

/**
 * CJMODPackager - CJMOD打包器
 */
class CJMODPackager {
public:
    CJMODPackager();
    ~CJMODPackager();
    
    /**
     * 打包CJMOD文件夹
     * @param source_directory 源目录路径
     * @param output_file 输出.cjmod文件路径
     * @param compression_level 压缩级别
     * @return 是否成功
     */
    bool packageCJMOD(const std::string& source_directory, 
                      const std::string& output_file,
                      CompressionLevel compression_level = CompressionLevel::DEFAULT_COMPRESSION);
    
    /**
     * 解包CJMOD文件
     * @param cjmod_file .cjmod文件路径
     * @param output_directory 输出目录
     * @return 是否成功
     */
    bool unpackageCJMOD(const std::string& cjmod_file, const std::string& output_directory);
    
    /**
     * 验证CJMOD文件夹结构
     * @param directory 目录路径
     * @return 验证结果
     */
    ModuleValidationResult validateCJMODStructure(const std::string& directory);
    
    /**
     * 验证CJMOD文件
     * @param cjmod_file .cjmod文件路径
     * @return 验证结果
     */
    ModuleValidationResult validateCJMODFile(const std::string& cjmod_file);
    
    /**
     * 获取CJMOD信息
     * @param cjmod_file .cjmod文件路径
     * @return 模块信息
     */
    ModuleInfo getCJMODInfo(const std::string& cjmod_file);
    
    /**
     * 编译CJMOD
     * @param cjmod_directory CJMOD源码目录
     * @param output_directory 编译输出目录
     * @return 是否成功
     */
    bool compileCJMOD(const std::string& cjmod_directory, const std::string& output_directory);
    
    /**
     * 设置进度回调
     * @param callback 进度回调函数
     */
    void setProgressCallback(std::function<void(const std::string&, int)> callback);

private:
    std::unique_ptr<ZipArchive> zip_archive;
    std::function<void(const std::string&, int)> progress_callback;
    
    // 内部方法
    bool validateCJMODStructureInternal(const std::string& directory, ModuleValidationResult& result);
    bool validateCppSources(const std::string& src_directory);
    bool validateCMakeFile(const std::string& cmake_file);
    bool validateAPICompliance(const std::string& header_file);
    std::vector<std::string> extractCJMODFunctions(const std::string& header_file);
    void reportProgress(const std::string& operation, int percentage);
};

/**
 * ModuleManager - 模块管理器
 */
class ModuleManager {
public:
    ModuleManager();
    ~ModuleManager();
    
    /**
     * 初始化模块管理器
     * @param official_module_path 官方模块路径
     * @param user_module_path 用户模块路径
     * @return 是否成功
     */
    bool initialize(const std::string& official_module_path, const std::string& user_module_path);
    
    /**
     * 安装模块
     * @param module_file 模块文件路径（.cmod或.cjmod）
     * @param install_to_official 是否安装到官方模块目录
     * @return 是否成功
     */
    bool installModule(const std::string& module_file, bool install_to_official = false);
    
    /**
     * 卸载模块
     * @param module_name 模块名称
     * @param from_official 是否从官方模块目录卸载
     * @return 是否成功
     */
    bool uninstallModule(const std::string& module_name, bool from_official = false);
    
    /**
     * 列出已安装的模块
     * @param include_official 是否包含官方模块
     * @return 模块信息列表
     */
    std::vector<ModuleInfo> listInstalledModules(bool include_official = true);
    
    /**
     * 查找模块
     * @param module_name 模块名称
     * @param search_official 是否搜索官方模块
     * @return 模块文件路径，未找到返回空字符串
     */
    std::string findModule(const std::string& module_name, bool search_official = true);
    
    /**
     * 检查模块依赖
     * @param module_name 模块名称
     * @return 依赖的模块列表
     */
    std::vector<std::string> checkModuleDependencies(const std::string& module_name);
    
    /**
     * 验证模块兼容性
     * @param module_name 模块名称
     * @param chtl_version CHTL版本
     * @return 是否兼容
     */
    bool validateModuleCompatibility(const std::string& module_name, const std::string& chtl_version);
    
    /**
     * 获取模块信息
     * @param module_name 模块名称
     * @return 模块信息
     */
    ModuleInfo getModuleInfo(const std::string& module_name);
    
    /**
     * 更新模块索引
     * @return 是否成功
     */
    bool updateModuleIndex();
    
    /**
     * 清理无效模块
     * @return 清理的模块数量
     */
    int cleanupInvalidModules();

private:
    std::string official_module_path;
    std::string user_module_path;
    std::unordered_map<std::string, ModuleInfo> module_index;
    std::unique_ptr<CMODPackager> cmod_packager;
    std::unique_ptr<CJMODPackager> cjmod_packager;
    
    // 内部方法
    bool scanModuleDirectory(const std::string& directory, bool is_official);
    ModuleInfo loadModuleInfo(const std::string& module_file);
    bool checkDependencyLoop(const std::string& module_name, std::vector<std::string>& visited);
    std::string resolveModulePath(const std::string& module_name, bool search_official);
    bool copyModuleFile(const std::string& source, const std::string& destination);
    bool deleteModuleFile(const std::string& module_path);
};

/**
 * ModuleSearcher - 模块搜索器
 */
class ModuleSearcher {
public:
    /**
     * 搜索策略
     */
    enum class SearchStrategy {
        EXACT_MATCH,        // 精确匹配
        PREFIX_MATCH,       // 前缀匹配
        CONTAINS_MATCH,     // 包含匹配
        FUZZY_MATCH         // 模糊匹配
    };
    
    /**
     * 搜索模块
     * @param query 搜索查询
     * @param directory 搜索目录
     * @param strategy 搜索策略
     * @param include_subdirs 是否包含子目录
     * @return 匹配的模块列表
     */
    static std::vector<std::string> searchModules(const std::string& query,
                                                  const std::string& directory,
                                                  SearchStrategy strategy = SearchStrategy::CONTAINS_MATCH,
                                                  bool include_subdirs = true);
    
    /**
     * 搜索子模块
     * @param parent_module 父模块名
     * @param directory 搜索目录
     * @return 子模块列表
     */
    static std::vector<std::string> searchSubModules(const std::string& parent_module, const std::string& directory);
    
    /**
     * 通配符搜索
     * @param pattern 通配符模式（支持*和?）
     * @param directory 搜索目录
     * @return 匹配的模块列表
     */
    static std::vector<std::string> wildcardSearch(const std::string& pattern, const std::string& directory);
    
    /**
     * 按类型搜索
     * @param type 模块类型
     * @param directory 搜索目录
     * @return 匹配的模块列表
     */
    static std::vector<std::string> searchByType(ModuleType type, const std::string& directory);

private:
    static bool matchesStrategy(const std::string& name, const std::string& query, SearchStrategy strategy);
    static bool matchesWildcard(const std::string& name, const std::string& pattern);
    static ModuleType detectModuleType(const std::string& module_file);
};

/**
 * ModuleUtils - 模块工具类
 */
class ModuleUtils {
public:
    /**
     * 生成模块信息文件
     * @param module_directory 模块目录
     * @param info 模块信息
     * @return 是否成功
     */
    static bool generateInfoFile(const std::string& module_directory, const ModuleInfo& info);
    
    /**
     * 解析版本号
     * @param version 版本字符串
     * @return 版本号组件 [major, minor, patch]
     */
    static std::vector<int> parseVersion(const std::string& version);
    
    /**
     * 比较版本号
     * @param version1 版本1
     * @param version2 版本2
     * @return -1:v1<v2, 0:v1==v2, 1:v1>v2
     */
    static int compareVersions(const std::string& version1, const std::string& version2);
    
    /**
     * 检查版本兼容性
     * @param required_version 需求版本
     * @param current_version 当前版本
     * @param min_version 最小版本
     * @param max_version 最大版本
     * @return 是否兼容
     */
    static bool isVersionCompatible(const std::string& required_version,
                                   const std::string& current_version,
                                   const std::string& min_version = "",
                                   const std::string& max_version = "");
    
    /**
     * 计算模块哈希值
     * @param module_file 模块文件
     * @return 哈希值
     */
    static std::string calculateModuleHash(const std::string& module_file);
    
    /**
     * 生成模块清单
     * @param module_file 模块文件
     * @return 清单内容
     */
    static std::string generateModuleManifest(const std::string& module_file);
    
    /**
     * 验证模块签名
     * @param module_file 模块文件
     * @param signature 签名
     * @return 是否有效
     */
    static bool validateModuleSignature(const std::string& module_file, const std::string& signature);
    
    /**
     * 创建模块模板
     * @param module_name 模块名称
     * @param type 模块类型
     * @param output_directory 输出目录
     * @return 是否成功
     */
    static bool createModuleTemplate(const std::string& module_name, ModuleType type, const std::string& output_directory);

private:
    static std::string formatModuleInfo(const ModuleInfo& info);
    static std::string generateExportSection(const std::vector<ModuleExport>& exports);
    static bool createCMODTemplate(const std::string& module_name, const std::string& output_directory);
    static bool createCJMODTemplate(const std::string& module_name, const std::string& output_directory);
};

/**
 * ModuleDependencyResolver - 模块依赖解析器
 */
class ModuleDependencyResolver {
public:
    /**
     * 解析依赖关系
     * @param module_name 模块名称
     * @param module_manager 模块管理器
     * @return 依赖的模块列表（按加载顺序）
     */
    static std::vector<std::string> resolveDependencies(const std::string& module_name, ModuleManager& module_manager);
    
    /**
     * 检查循环依赖
     * @param module_name 模块名称
     * @param module_manager 模块管理器
     * @return 循环依赖链，无循环返回空
     */
    static std::vector<std::string> checkCircularDependencies(const std::string& module_name, ModuleManager& module_manager);
    
    /**
     * 生成依赖图
     * @param modules 模块列表
     * @param module_manager 模块管理器
     * @return 依赖图（DOT格式）
     */
    static std::string generateDependencyGraph(const std::vector<std::string>& modules, ModuleManager& module_manager);

private:
    static void resolveDependenciesRecursive(const std::string& module_name,
                                           ModuleManager& module_manager,
                                           std::vector<std::string>& resolved,
                                           std::vector<std::string>& visiting);
    
    static bool checkCircularDependenciesRecursive(const std::string& module_name,
                                                  ModuleManager& module_manager,
                                                  std::vector<std::string>& path,
                                                  std::unordered_map<std::string, bool>& visited);
};

} // namespace Core
} // namespace CHTL