#pragma once

#include <string>
#include <vector>
#include <unordered_map>

namespace CHTL {
namespace Core {

/**
 * ModuleUsageRules - 模块使用规则
 * 严格按照语法文档实现模块导入和使用规则
 */
class ModuleUsageRules {
public:
    /**
     * 导入语法类型（严格按照语法文档）
     */
    enum class ImportSyntaxType {
        CHTL_IMPORT,        // [Import] @Chtl from 模块名 - 导入CMOD
        CJMOD_IMPORT        // [Import] @CJmod from 模块名 - 导入CJMOD
    };
    
    /**
     * 模块搜索路径规则（严格按照语法文档）
     */
    struct SearchPathRule {
        std::string official_module_path;      // 官方模块路径（源码编译后生成的module文件夹）
        std::string current_module_path;       // 当前目录的module文件夹
        std::string current_directory;         // 当前目录
        
        SearchPathRule() = default;
        SearchPathRule(const std::string& official, const std::string& current_module, const std::string& current)
            : official_module_path(official), current_module_path(current_module), current_directory(current) {}
    };
    
    /**
     * 验证CHTL导入规则
     * 严格按照语法文档：
     * 1. 名称(不带后缀)：官方模块 -> 当前目录module文件夹 -> 当前目录，cmod优先
     * 2. 具体名称(带后缀)：按路径搜索指定文件
     * 3. 具体路径(含文件信息)：直接按路径查找
     * 4. 具体路径(不含文件信息)：触发报错
     * @param import_path 导入路径
     * @param search_rule 搜索规则
     * @return 解析后的文件路径，失败返回空
     */
    static std::string resolveCHTLImport(const std::string& import_path, const SearchPathRule& search_rule);
    
    /**
     * 验证CJMOD导入规则
     * 严格按照语法文档：
     * 1. 名称(不带后缀)：官方模块 -> 当前目录module文件夹 -> 当前目录，仅匹配cjmod文件
     * 2. 具体名称(带后缀)：按路径搜索指定文件
     * 3. 具体路径(含文件信息)：直接按路径查找
     * 4. 具体路径(不含文件信息)：触发报错
     * @param import_path 导入路径
     * @param search_rule 搜索规则
     * @return 解析后的文件路径，失败返回空
     */
    static std::string resolveCJMODImport(const std::string& import_path, const SearchPathRule& search_rule);
    
    /**
     * 解析混合模块导入
     * 严格按照语法文档：CHTL不会对此进行统一处理，我们不推荐使用@Chtl同时管理CMOD和CJMOD
     * @param module_name 模块名称
     * @param import_type 导入类型
     * @param search_rule 搜索规则
     * @return 解析结果
     */
    static std::string resolveMixedModuleImport(const std::string& module_name, 
                                              ImportSyntaxType import_type,
                                              const SearchPathRule& search_rule);
    
    /**
     * 验证模块名称格式
     * @param module_name 模块名称
     * @return 是否为有效格式
     */
    static bool validateModuleName(const std::string& module_name);
    
    /**
     * 检查是否为子模块导入
     * 支持：Chtholly.Space 或 Chtholly/Space
     * @param import_path 导入路径
     * @return 是否为子模块导入
     */
    static bool isSubModuleImport(const std::string& import_path);
    
    /**
     * 解析子模块路径
     * @param submodule_path 子模块路径（如 Chtholly.Space）
     * @return [父模块名, 子模块名]
     */
    static std::pair<std::string, std::string> parseSubModulePath(const std::string& submodule_path);
    
    /**
     * 检查是否为通配符导入
     * 支持：Chtholly.* (导入所有子模块)
     * @param import_path 导入路径
     * @return 是否为通配符导入
     */
    static bool isWildcardImport(const std::string& import_path);
    
    /**
     * 解析通配符导入
     * @param wildcard_path 通配符路径（如 Chtholly.*）
     * @param search_rule 搜索规则
     * @return 匹配的模块列表
     */
    static std::vector<std::string> resolveWildcardImport(const std::string& wildcard_path,
                                                         const SearchPathRule& search_rule);

private:
    // 路径类型检测
    static bool isNameOnly(const std::string& path);                    // 仅名称（不带后缀）
    static bool isNameWithExtension(const std::string& path);           // 具体名称（带后缀）
    static bool isPathWithFileInfo(const std::string& path);            // 具体路径（含文件信息）
    static bool isPathWithoutFileInfo(const std::string& path);         // 具体路径（不含文件信息）
    
    // 文件搜索
    static std::string searchInOfficialModule(const std::string& name, const std::string& official_path, bool cmod_priority = true);
    static std::string searchInCurrentModule(const std::string& name, const std::string& current_module_path, bool cmod_priority = true);
    static std::string searchInCurrentDirectory(const std::string& name, const std::string& current_dir, bool cmod_priority = true);
    
    // CJMOD特殊搜索（仅匹配.cjmod文件）
    static std::string searchCJMODInOfficialModule(const std::string& name, const std::string& official_path);
    static std::string searchCJMODInCurrentModule(const std::string& name, const std::string& current_module_path);
    static std::string searchCJMODInCurrentDirectory(const std::string& name, const std::string& current_dir);
    
    // 子模块搜索
    static std::string searchSubModule(const std::string& parent_name, const std::string& sub_name, 
                                     const SearchPathRule& search_rule, bool is_cjmod = false);
    
    // 通配符匹配
    static std::vector<std::string> findAllSubModules(const std::string& parent_name, 
                                                     const SearchPathRule& search_rule, bool is_cjmod = false);
    
    // 工具方法
    static std::string extractModuleName(const std::string& path);
    static std::string extractFileExtension(const std::string& path);
    static bool hasFileExtension(const std::string& path);
    static std::string joinPath(const std::string& dir, const std::string& file);
    static bool fileExists(const std::string& path);
    static bool directoryExists(const std::string& path);
};

/**
 * ModuleDirectoryStructure - 模块目录结构管理
 * 严格按照语法文档管理各种目录结构
 */
class ModuleDirectoryStructure {
public:
    /**
     * 目录结构类型（严格按照语法文档）
     */
    enum class DirectoryStructureType {
        FLAT_STRUCTURE,        // 常规混杂结构：chtl、cmod、cjmod文件混合
        CATEGORIZED_STRUCTURE  // 分类结构：CMOD + CJMOD两个文件夹分类
    };
    
    /**
     * 检测官方模块目录结构类型
     * @param official_module_path 官方模块路径
     * @return 目录结构类型
     */
    static DirectoryStructureType detectOfficialStructure(const std::string& official_module_path);
    
    /**
     * 检测用户模块目录结构类型
     * @param user_module_path 用户模块路径
     * @return 目录结构类型
     */
    static DirectoryStructureType detectUserStructure(const std::string& user_module_path);
    
    /**
     * 在分类结构中搜索CMOD
     * @param base_path 基础路径
     * @param module_name 模块名称
     * @return CMOD文件路径
     */
    static std::string searchCMODInCategorized(const std::string& base_path, const std::string& module_name);
    
    /**
     * 在分类结构中搜索CJMOD
     * @param base_path 基础路径
     * @param module_name 模块名称
     * @return CJMOD文件路径
     */
    static std::string searchCJMODInCategorized(const std::string& base_path, const std::string& module_name);
    
    /**
     * 在扁平结构中搜索模块
     * @param base_path 基础路径
     * @param module_name 模块名称
     * @param prefer_cmod 是否优先CMOD
     * @return 模块文件路径
     */
    static std::string searchInFlat(const std::string& base_path, const std::string& module_name, bool prefer_cmod = true);
    
    /**
     * 创建标准目录结构
     * @param base_path 基础路径
     * @param structure_type 结构类型
     * @return 是否成功
     */
    static bool createStandardStructure(const std::string& base_path, DirectoryStructureType structure_type);

private:
    static bool hasCMODDirectory(const std::string& path);
    static bool hasCJMODDirectory(const std::string& path);
    static std::vector<std::string> getCMODDirectoryNames();  // ["CMOD", "Cmod", "cmod"]
    static std::vector<std::string> getCJMODDirectoryNames(); // ["CJMOD", "CJmod", "cjmod"]
    static std::string findFirstExistingDirectory(const std::string& base_path, const std::vector<std::string>& names);
};

/**
 * ModuleImportValidator - 模块导入验证器
 * 验证导入语法的正确性和模块的兼容性
 */
class ModuleImportValidator {
public:
    /**
     * 导入验证结果
     */
    struct ImportValidationResult {
        bool is_valid;                          // 是否有效
        std::string resolved_path;              // 解析后的路径
        std::vector<std::string> errors;        // 错误列表
        std::vector<std::string> warnings;      // 警告列表
        std::vector<std::string> suggestions;   // 建议列表
        
        ImportValidationResult() : is_valid(false) {}
    };
    
    /**
     * 验证CHTL导入语句
     * @param import_statement 导入语句（如 "[Import] @Chtl from Chtholly"）
     * @param search_rule 搜索规则
     * @return 验证结果
     */
    static ImportValidationResult validateCHTLImport(const std::string& import_statement, 
                                                    const SearchPathRule& search_rule);
    
    /**
     * 验证CJMOD导入语句
     * @param import_statement 导入语句（如 "[Import] @CJmod from PrintMyLove"）
     * @param search_rule 搜索规则
     * @return 验证结果
     */
    static ImportValidationResult validateCJMODImport(const std::string& import_statement,
                                                     const SearchPathRule& search_rule);
    
    /**
     * 验证混合模块使用规则
     * 严格按照语法文档：不推荐使用@Chtl同时管理CMOD和CJMOD
     * @param chtl_imports CHTL导入列表
     * @param cjmod_imports CJMOD导入列表
     * @return 验证结果
     */
    static ImportValidationResult validateMixedModuleUsage(const std::vector<std::string>& chtl_imports,
                                                          const std::vector<std::string>& cjmod_imports);
    
    /**
     * 检查循环依赖
     * @param module_name 模块名称
     * @param dependency_chain 依赖链
     * @return 是否存在循环依赖
     */
    static bool checkCircularDependency(const std::string& module_name, 
                                       const std::vector<std::string>& dependency_chain);

private:
    static std::string parseImportStatement(const std::string& statement);
    static bool isValidImportSyntax(const std::string& statement, ImportSyntaxType expected_type);
    static void addError(ImportValidationResult& result, const std::string& error);
    static void addWarning(ImportValidationResult& result, const std::string& warning);
    static void addSuggestion(ImportValidationResult& result, const std::string& suggestion);
};

/**
 * ChthollyModuleUsage - 珂朵莉模块使用示例
 * 严格按照语法文档演示正确的使用方式
 */
class ChthollyModuleUsage {
public:
    /**
     * 生成珂朵莉CMOD使用示例
     * @return CHTL代码示例
     */
    static std::string generateCMODUsageExample();
    
    /**
     * 生成珂朵莉CJMOD使用示例
     * @return CHTL代码示例
     */
    static std::string generateCJMODUsageExample();
    
    /**
     * 生成完整的珂朵莉模块使用示例
     * @return CHTL代码示例
     */
    static std::string generateCompleteUsageExample();
    
    /**
     * 生成模块导入的最佳实践示例
     * @return CHTL代码示例
     */
    static std::string generateBestPracticeExample();

private:
    static std::string generateCMODImportExample();
    static std::string generateCJMODImportExample();
    static std::string generateSubModuleImportExample();
    static std::string generateWildcardImportExample();
};

} // namespace Core
} // namespace CHTL