#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <chrono>

namespace CHTL {
namespace Core {

/**
 * ModuleStructureType - 模块结构类型
 */
enum class ModuleStructureType {
    PURE_CMOD,          // 纯CMOD模块
    PURE_CJMOD,         // 纯CJMOD模块  
    MIXED_MODULE        // CMOD + CJMOD混合模块
};

/**
 * ModuleNamingRule - 模块命名规则验证
 * 严格按照语法文档：模块文件夹、主模块chtl文件、模块信息chtl文件必须同名
 */
class ModuleNamingRule {
public:
    /**
     * 验证CMOD命名规则
     * @param module_directory_name 模块目录名
     * @param main_module_path 主模块路径 
     * @param info_module_path 信息模块路径
     * @return 是否符合同名原则
     */
    static bool validateCMODNaming(const std::string& module_directory_name,
                                   const std::string& main_module_path,
                                   const std::string& info_module_path);
    
    /**
     * 验证CJMOD命名规则
     * @param module_directory_name 模块目录名
     * @param info_module_path 信息模块路径
     * @return 是否符合同名原则
     */
    static bool validateCJMODNaming(const std::string& module_directory_name,
                                    const std::string& info_module_path);
    
    /**
     * 验证子模块命名规则
     * @param submodule_directory_name 子模块目录名
     * @param submodule_main_path 子模块主文件路径
     * @param submodule_info_path 子模块信息文件路径
     * @return 是否符合同名原则
     */
    static bool validateSubModuleNaming(const std::string& submodule_directory_name,
                                       const std::string& submodule_main_path,
                                       const std::string& submodule_info_path);

private:
    static std::string extractNameFromPath(const std::string& path);
    static std::string removeExtension(const std::string& filename);
};

/**
 * CMODStructure - CMOD标准结构
 * 严格按照语法文档：
 * Chtholly/
 *   src/
 *     Chtholly.chtl  (主模块，可选如果有子模块)
 *     Other.chtl
 *   info/
 *     Chtholly.chtl  (模块信息，必须同名)
 */
struct CMODStructure {
    std::string module_name;                    // 模块名称（必须与文件夹同名）
    std::string src_directory;                  // src目录路径
    std::string info_directory;                 // info目录路径
    std::string main_module_file;               // 主模块文件（可选）
    std::string info_module_file;               // 信息模块文件（必须）
    std::vector<std::string> other_chtl_files; // 其他.chtl文件
    std::vector<CMODStructure> sub_modules;     // 子模块
    bool has_main_module;                       // 是否有主模块文件
    
    CMODStructure() : has_main_module(false) {}
    
    /**
     * 验证结构完整性
     * @return 是否符合CMOD标准格式
     */
    bool validateStructure() const;
    
    /**
     * 检查是否需要主模块
     * 在没有子模块的情况下，主模块chtl文件必须存在
     * @return 是否需要主模块
     */
    bool requiresMainModule() const;
};

/**
 * CJMODStructure - CJMOD标准结构
 * 严格按照语法文档：
 * CJmod文件夹/
 *   src/xxx.cpp xxx.h
 *   Box/
 *     src/xxx.cpp xxx.h
 *     info/Box.chtl
 *   Box2/
 *     src/xxx.cpp xxx.h  
 *     info/Box2.chtl
 *   info/CJmod文件夹名称.chtl
 */
struct CJMODStructure {
    std::string module_name;                     // 模块名称（必须与文件夹同名）
    std::string src_directory;                   // src目录路径
    std::string info_directory;                  // info目录路径
    std::string info_module_file;                // 信息模块文件（必须同名）
    std::vector<std::string> cpp_source_files;   // C++源文件
    std::vector<std::string> header_files;       // 头文件
    std::vector<CJMODStructure> sub_modules;     // 子模块
    
    /**
     * 验证结构完整性
     * @return 是否符合CJMOD标准格式
     */
    bool validateStructure() const;
    
    /**
     * 检查C++源码完整性
     * @return 是否有必要的源码文件
     */
    bool validateCppSources() const;
};

/**
 * MixedModuleStructure - CMOD + CJMOD混合模块结构
 * 严格按照语法文档：
 * 模块名称/
 *   CMOD/  (或Cmod, cmod)
 *     Box/
 *       src/Box.chtl, Other.chtl
 *       info/Box.chtl
 *   CJMOD/ (或CJmod, cjmod)
 *     Box/
 *       src/xxx.cpp xxx.h
 *       info/Box.chtl
 */
struct MixedModuleStructure {
    std::string module_name;           // 模块名称
    std::string cmod_directory;        // CMOD目录（CMOD/Cmod/cmod）
    std::string cjmod_directory;       // CJMOD目录（CJMOD/CJmod/cjmod）
    CMODStructure cmod_structure;      // CMOD结构
    CJMODStructure cjmod_structure;    // CJMOD结构
    
    /**
     * 验证混合模块结构
     * @return 是否符合混合模块标准格式
     */
    bool validateStructure() const;
    
    /**
     * 检查目录命名规范
     * @return 是否使用了正确的CMOD/CJMOD目录名
     */
    bool validateDirectoryNaming() const;
};

/**
 * ModuleInfoParser - 模块信息解析器
 * 严格按照语法文档解析[Info]块，不处理[Export]块（系统生成）
 */
class ModuleInfoParser {
public:
    /**
     * [Info]块结构（严格按照语法文档）
     */
    struct InfoBlock {
        std::string name;              // name = "chtholly"
        std::string version;           // version = "1.0.0"
        std::string description;       // description = "珂朵莉主题模块 - 世界上最幸福的女孩"
        std::string author;            // author = "CHTL Team"
        std::string license;           // license = "MIT"
        std::string dependencies;      // dependencies = ""
        std::string category;          // category = "theme"
        std::string minCHTLVersion;    // minCHTLVersion = "1.0.0"
        std::string maxCHTLVersion;    // maxCHTLVersion = "2.0.0"
        
        /**
         * 验证必填字段
         * @return 是否包含所有必填字段
         */
        bool validateRequiredFields() const;
    };
    
    /**
     * 解析info文件中的[Info]块
     * @param info_file_path info文件路径
     * @return 解析出的Info信息
     */
    static InfoBlock parseInfoBlock(const std::string& info_file_path);
    
    /**
     * 验证Info块格式
     * @param content 文件内容
     * @return 是否符合[Info]块格式
     */
    static bool validateInfoBlockFormat(const std::string& content);
    
    /**
     * 生成Info块内容
     * @param info Info信息
     * @return 格式化的[Info]块内容
     */
    static std::string generateInfoBlock(const InfoBlock& info);

private:
    static std::string extractFieldValue(const std::string& content, const std::string& field_name);
    static std::string formatFieldValue(const std::string& field_name, const std::string& value);
};

/**
 * ExportTableGenerator - [Export]表生成器
 * 重要：[Export]表由系统生成，开发者不需要写！
 * 严格按照语法文档：外部查询表，优化性能用，无实际作用，不需要写这个，此由系统生成
 */
class ExportTableGenerator {
public:
    /**
     * Export项类型（严格按照语法文档）
     */
    enum class ExportItemType {
        CUSTOM_STYLE,        // [Custom] @Style
        CUSTOM_ELEMENT,      // [Custom] @Element  
        CUSTOM_VAR,          // [Custom] @Var
        TEMPLATE_STYLE,      // [Template] @Style
        TEMPLATE_ELEMENT,    // [Template] @Element
        TEMPLATE_VAR,        // [Template] @Var
        ORIGIN_HTML,         // [Origin] @Html
        ORIGIN_STYLE,        // [Origin] @Style
        ORIGIN_JAVASCRIPT,   // [Origin] @Javascript
        ORIGIN_VUE,          // [Origin] @Vue (自定义类型的原始嵌入)
        CONFIGURATION        // [Configuration] @Config
    };
    
    /**
     * Export项
     */
    struct ExportItem {
        ExportItemType type;
        std::string name;
        std::string full_declaration;
        
        ExportItem() : type(ExportItemType::CUSTOM_STYLE) {}
        ExportItem(ExportItemType t, const std::string& n, const std::string& decl = "")
            : type(t), name(n), full_declaration(decl) {}
    };
    
    /**
     * 扫描CMOD源码并生成[Export]表
     * @param cmod_src_directory CMOD src目录
     * @return Export项列表
     */
    static std::vector<ExportItem> scanAndGenerateExports(const std::string& cmod_src_directory);
    
    /**
     * 生成[Export]块内容
     * @param exports Export项列表
     * @return 格式化的[Export]块内容
     */
    static std::string generateExportBlock(const std::vector<ExportItem>& exports);
    
    /**
     * 将[Export]表追加到info文件
     * @param info_file_path info文件路径
     * @param exports Export项列表
     * @return 是否成功
     */
    static bool appendExportTableToInfoFile(const std::string& info_file_path, 
                                           const std::vector<ExportItem>& exports);

private:
    static std::vector<ExportItem> scanChtlFile(const std::string& chtl_file_path);
    static ExportItemType detectExportType(const std::string& declaration);
    static std::string formatExportDeclaration(ExportItemType type, const std::vector<std::string>& names);
    static std::string exportTypeToString(ExportItemType type);
};

/**
 * SubModuleManager - 子模块管理器
 * 严格按照语法文档处理嵌套结构
 */
class SubModuleManager {
public:
    /**
     * 扫描CMOD子模块
     * @param cmod_src_directory CMOD src目录
     * @return 子模块结构列表
     */
    static std::vector<CMODStructure> scanCMODSubModules(const std::string& cmod_src_directory);
    
    /**
     * 扫描CJMOD子模块
     * @param cjmod_directory CJMOD目录
     * @return 子模块结构列表
     */
    static std::vector<CJMODStructure> scanCJMODSubModules(const std::string& cjmod_directory);
    
    /**
     * 验证子模块结构
     * @param submodule_path 子模块路径
     * @param is_cmod 是否为CMOD子模块
     * @return 是否符合子模块结构
     */
    static bool validateSubModuleStructure(const std::string& submodule_path, bool is_cmod);
    
    /**
     * 构建子模块层次结构
     * @param base_directory 基础目录
     * @param is_cmod 是否为CMOD
     * @return 层次结构映射
     */
    static std::unordered_map<std::string, std::vector<std::string>> buildSubModuleHierarchy(
        const std::string& base_directory, bool is_cmod);

private:
    static bool isValidSubModuleDirectory(const std::string& directory_path, bool is_cmod);
    static CMODStructure parseCMODSubModule(const std::string& submodule_path);
    static CJMODStructure parseCJMODSubModule(const std::string& submodule_path);
};

/**
 * ModuleStructureValidator - 模块结构验证器
 * 统一验证所有类型的模块结构
 */
class ModuleStructureValidator {
public:
    /**
     * 验证结果
     */
    struct ValidationResult {
        bool is_valid;
        ModuleStructureType detected_type;
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
        std::vector<std::string> suggestions;
        
        ValidationResult() : is_valid(false), detected_type(ModuleStructureType::PURE_CMOD) {}
    };
    
    /**
     * 全面验证模块结构
     * @param module_directory 模块目录
     * @return 验证结果
     */
    static ValidationResult validateModuleStructure(const std::string& module_directory);
    
    /**
     * 检测模块类型
     * @param module_directory 模块目录
     * @return 模块结构类型
     */
    static ModuleStructureType detectModuleType(const std::string& module_directory);
    
    /**
     * 验证CMOD结构
     * @param module_directory 模块目录
     * @return 验证结果
     */
    static ValidationResult validateCMODStructure(const std::string& module_directory);
    
    /**
     * 验证CJMOD结构
     * @param module_directory 模块目录
     * @return 验证结果
     */
    static ValidationResult validateCJMODStructure(const std::string& module_directory);
    
    /**
     * 验证混合模块结构
     * @param module_directory 模块目录
     * @return 验证结果
     */
    static ValidationResult validateMixedModuleStructure(const std::string& module_directory);

private:
    static bool checkRequiredDirectories(const std::string& base_path, 
                                        const std::vector<std::string>& required_dirs,
                                        ValidationResult& result);
    static bool checkRequiredFiles(const std::string& base_path,
                                  const std::vector<std::string>& required_files,
                                  ValidationResult& result);
    static void addError(ValidationResult& result, const std::string& error);
    static void addWarning(ValidationResult& result, const std::string& warning);
    static void addSuggestion(ValidationResult& result, const std::string& suggestion);
};

} // namespace Core
} // namespace CHTL