#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <memory>

namespace CHTL {
namespace Common {

/**
 * EnhancedImportManager - 增强导入管理器
 * 严格按照"完善选择器自动化与引用规则.ini"实现
 */
class EnhancedImportManager {
public:
    /**
     * 导入类型（严格按照规则文档）
     */
    enum class ImportType {
        HTML,           // @Html - 导入HTML文件
        STYLE,          // @Style - 导入样式文件
        JAVASCRIPT,     // @JavaScript - 导入JavaScript文件
        CHTL,           // @Chtl - 导入CHTL模块
        CJMOD,          // @CJmod - 导入CJMOD模块
        ORIGIN,         // [Origin] @Type - 原始嵌入导入
        CUSTOM_TYPE     // 自定义类型（如@Vue）
    };
    
    /**
     * 路径类型（严格按照规则文档）
     */
    enum class PathType {
        NAME_ONLY,              // 文件名（不带后缀）
        SPECIFIC_NAME,          // 具体文件名（带后缀）
        SPECIFIC_PATH_WITH_FILE,// 具体路径（含文件信息）
        SPECIFIC_PATH_NO_FILE,  // 具体路径（不含文件信息）- 会触发报错
        FOLDER_PATH             // 文件夹路径 - 会触发报错
    };
    
    /**
     * 模块目录结构类型
     */
    enum class ModuleStructureType {
        MIXED,          // 混杂结构：chtl文件，cmod，cjmod混合
        CATEGORIZED     // 分类结构：CMOD / CJMOD两个文件夹分类
    };
    
    /**
     * 导入信息
     */
    struct ImportInfo {
        ImportType type;
        std::string source_path;        // 源路径
        std::string alias_name;         // as语法的别名
        bool has_alias;                 // 是否有as语法
        PathType path_type;            // 路径类型
        bool is_official_prefix;        // 是否使用官方模块前缀
        std::vector<std::string> wildcard_results; // 通配符导入结果
        
        ImportInfo() : type(ImportType::CHTL), has_alias(false), 
                      path_type(PathType::NAME_ONLY), is_official_prefix(false) {}
    };
    
    /**
     * 导入结果
     */
    struct ImportResult {
        bool success;
        std::string resolved_path;
        std::string error_message;
        ImportInfo import_info;
        bool should_skip;               // 是否应该跳过（没有as语法的@Html等）
        bool create_named_embed;        // 是否创建带名原始嵌入节点
        
        ImportResult() : success(false), should_skip(false), create_named_embed(false) {}
    };

public:
    /**
     * 解析导入语句
     * 支持无修饰字符串：[Import] @Html from index.html
     * @param import_statement 导入语句
     * @return 解析后的导入信息
     */
    static ImportInfo parseImportStatement(const std::string& import_statement);
    
    /**
     * 处理导入请求
     * @param import_info 导入信息
     * @param current_file_directory 当前文件目录
     * @param official_module_directory 官方模块目录
     * @return 导入结果
     */
    static ImportResult processImport(const ImportInfo& import_info,
                                    const std::string& current_file_directory,
                                    const std::string& official_module_directory);
    
    /**
     * 处理@Html, @Style, @JavaScript导入
     * 规则：没有as语法则跳过，有as语法则创建带名原始嵌入节点
     * @param import_info 导入信息
     * @param current_directory 当前目录
     * @return 导入结果
     */
    static ImportResult processFileImport(const ImportInfo& import_info,
                                        const std::string& current_directory);
    
    /**
     * 处理@Chtl导入
     * 路径搜索规则：官方模块目录 → 当前目录module文件夹 → 当前目录，cmod优先
     * @param import_info 导入信息
     * @param current_directory 当前目录
     * @param official_module_directory 官方模块目录
     * @return 导入结果
     */
    static ImportResult processCHTLImport(const ImportInfo& import_info,
                                        const std::string& current_directory,
                                        const std::string& official_module_directory);
    
    /**
     * 处理@CJmod导入
     * 路径搜索规则：官方模块目录 → 当前目录module文件夹 → 当前目录，仅匹配cjmod文件
     * @param import_info 导入信息
     * @param current_directory 当前目录
     * @param official_module_directory 官方模块目录
     * @return 导入结果
     */
    static ImportResult processCJMODImport(const ImportInfo& import_info,
                                         const std::string& current_directory,
                                         const std::string& official_module_directory);
    
    /**
     * 处理通配符导入
     * 支持：具体路径.*、具体路径.*.cmod、具体路径.*.chtl
     * 等价于：具体路径/*、具体路径/*.cmod、具体路径/*.chtl
     * @param wildcard_path 通配符路径
     * @param base_directory 基础目录
     * @return 匹配的文件列表
     */
    static std::vector<std::string> processWildcardImport(const std::string& wildcard_path,
                                                         const std::string& base_directory);
    
    /**
     * 处理子模块导入
     * 支持：Chtholly.* (所有子模块)、Chtholly.Space (指定子模块)
     * 支持使用'/'替代'.'：Chtholly/Space
     * @param submodule_path 子模块路径
     * @param base_directory 基础目录
     * @return 子模块文件路径
     */
    static std::vector<std::string> processSubModuleImport(const std::string& submodule_path,
                                                          const std::string& base_directory);
    
    /**
     * 检测模块目录结构类型
     * @param module_directory 模块目录
     * @return 结构类型
     */
    static ModuleStructureType detectModuleStructure(const std::string& module_directory);
    
    /**
     * 在分类结构中搜索文件
     * @param module_directory 模块目录
     * @param filename 文件名
     * @param is_cjmod 是否搜索CJMOD
     * @return 文件路径
     */
    static std::string searchInCategorizedStructure(const std::string& module_directory,
                                                   const std::string& filename,
                                                   bool is_cjmod = false);
    
    /**
     * 在混杂结构中搜索文件
     * @param module_directory 模块目录
     * @param filename 文件名
     * @param prefer_cmod 是否优先CMOD
     * @return 文件路径
     */
    static std::string searchInMixedStructure(const std::string& module_directory,
                                             const std::string& filename,
                                             bool prefer_cmod = true);

private:
    /**
     * 解析导入类型
     * @param type_text 类型文本
     * @return 导入类型
     */
    static ImportType parseImportType(const std::string& type_text);
    
    /**
     * 分析路径类型
     * @param path 路径字符串
     * @return 路径类型
     */
    static PathType analyzePath(const std::string& path);
    
    /**
     * 检查是否为官方模块前缀
     * @param path 路径字符串
     * @return 是否为官方模块前缀
     */
    static bool isOfficialModulePrefix(const std::string& path);
    
    /**
     * 按优先级搜索文件
     * @param directories 目录列表（按优先级排序）
     * @param filename 文件名
     * @param extensions 扩展名列表（按优先级排序）
     * @return 找到的文件路径
     */
    static std::string searchWithPriority(const std::vector<std::string>& directories,
                                         const std::string& filename,
                                         const std::vector<std::string>& extensions);
    
    /**
     * 清理路径
     * @param path 原始路径
     * @return 清理后的路径
     */
    static std::string cleanPath(const std::string& path);
    
    /**
     * 提取文件名和扩展名
     * @param path 路径
     * @return [文件名, 扩展名]
     */
    static std::pair<std::string, std::string> extractFileNameAndExtension(const std::string& path);
    
    /**
     * 检查文件是否存在
     * @param file_path 文件路径
     * @return 是否存在
     */
    static bool fileExists(const std::string& file_path);
    
    /**
     * 检查目录是否存在
     * @param directory_path 目录路径
     * @return 是否存在
     */
    static bool directoryExists(const std::string& directory_path);
    
    /**
     * 列出目录中的文件
     * @param directory_path 目录路径
     * @param pattern 文件模式（可选）
     * @return 文件列表
     */
    static std::vector<std::string> listFiles(const std::string& directory_path,
                                             const std::string& pattern = "");
    
    /**
     * 构建搜索目录列表
     * @param current_directory 当前目录
     * @param official_module_directory 官方模块目录
     * @return 搜索目录列表（按优先级排序）
     */
    static std::vector<std::string> buildSearchDirectories(const std::string& current_directory,
                                                          const std::string& official_module_directory);
};

/**
 * DefaultNamespaceManager - 默认命名空间管理器
 * 处理没有使用命名空间的文件的默认命名空间功能
 */
class DefaultNamespaceManager {
public:
    /**
     * 为文件生成默认命名空间
     * 规则：没有使用命名空间的文件，在被导入时，会默认以文件名作为命名空间
     * @param file_path 文件路径
     * @return 默认命名空间名称
     */
    static std::string generateDefaultNamespace(const std::string& file_path);
    
    /**
     * 检查是否禁用默认命名空间
     * @return 是否禁用
     */
    static bool isDefaultNamespaceDisabled();
    
    /**
     * 设置默认命名空间禁用状态
     * @param disabled 是否禁用
     */
    static void setDefaultNamespaceDisabled(bool disabled);
    
    /**
     * 应用默认命名空间到导入的文件
     * @param file_content 文件内容
     * @param namespace_name 命名空间名称
     * @return 添加了命名空间的文件内容
     */
    static std::string applyDefaultNamespace(const std::string& file_content,
                                           const std::string& namespace_name);

private:
    static bool default_namespace_disabled_;
    
    /**
     * 从文件路径提取文件名
     * @param file_path 文件路径
     * @return 不含扩展名的文件名
     */
    static std::string extractFileName(const std::string& file_path);
    
    /**
     * 检查文件是否已有命名空间
     * @param file_content 文件内容
     * @return 是否已有命名空间
     */
    static bool hasExistingNamespace(const std::string& file_content);
};

} // namespace Common
} // namespace CHTL