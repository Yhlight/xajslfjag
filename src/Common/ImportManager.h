#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <memory>

namespace CHTL {
namespace Common {

// Import类型枚举
enum class ImportType {
    // 文件导入
    HTML_FILE,          // @Html from "file.html"
    STYLE_FILE,         // @Style from "file.css"  
    JAVASCRIPT_FILE,    // @JavaScript from "file.js"
    
    // CHTL模块导入
    CHTL_FILE,          // @Chtl from "file.chtl"
    CJMOD_FILE,         // @CJmod from "module"
    OFFICIAL_MODULE,    // chtl::模块名 (官方模块前缀)
    
    // CHTL项目导入
    TEMPLATE_STYLE,     // [Template] @Style from "file.chtl"
    TEMPLATE_ELEMENT,   // [Template] @Element from "file.chtl"
    TEMPLATE_VAR,       // [Template] @Var from "file.chtl"
    
    CUSTOM_STYLE,       // [Custom] @Style from "file.chtl"
    CUSTOM_ELEMENT,     // [Custom] @Element from "file.chtl"
    CUSTOM_VAR,         // [Custom] @Var from "file.chtl"
    
    ORIGIN_ITEM,        // [Origin] @Type from "file.chtl"
    CONFIG_ITEM,        // @Config from "file.chtl"
    
    // 批量导入
    ALL_TEMPLATES,      // [Template] from "file.chtl"
    ALL_CUSTOMS,        // [Custom] from "file.chtl"
    ALL_ORIGINS,        // [Origin] from "file.chtl"
    ALL_TEMPLATE_TYPES, // [Template] @Style/@Element/@Var from "file.chtl"
    ALL_CUSTOM_TYPES,   // [Custom] @Style/@Element/@Var from "file.chtl"
    
    UNKNOWN
};

// 路径解析类型
enum class PathResolutionType {
    NAME_ONLY,          // 名称（不带后缀）
    SPECIFIC_NAME,      // 具体名称（带后缀）
    SPECIFIC_PATH,      // 具体路径（含文件信息）
    DIRECTORY_PATH,     // 目录路径（错误）
    WILDCARD_PATH       // 通配符路径（.*或/*）
};

// Import项目信息
struct ImportItem {
    ImportType type;
    std::string target_name;        // 导入的目标名称
    std::string import_path;        // from后的路径
    std::string alias_name;         // as后的别名（可选）
    std::string resolved_path;      // 解析后的实际路径
    bool has_alias;                 // 是否有别名
    bool is_processed;              // 是否已处理
    std::vector<std::string> dependencies; // 依赖关系
    
    ImportItem() : type(ImportType::UNKNOWN), has_alias(false), is_processed(false) {}
    
    ImportItem(ImportType t, const std::string& target, const std::string& path)
        : type(t), target_name(target), import_path(path), has_alias(false), is_processed(false) {}
    
    void setAlias(const std::string& alias) {
        alias_name = alias;
        has_alias = !alias.empty();
    }
    
    std::string getEffectiveName() const {
        return has_alias ? alias_name : target_name;
    }
    
    bool requiresAlias() const;
    bool shouldSkip() const;
};

// 路径解析结果
struct PathResolution {
    PathResolutionType type;
    std::string original_path;
    std::string resolved_path;
    std::vector<std::string> candidates;   // 候选路径
    bool success;
    std::string error_message;
    
    PathResolution() : type(PathResolutionType::NAME_ONLY), success(false) {}
    
    void addCandidate(const std::string& path) {
        candidates.push_back(path);
    }
    
    void setError(const std::string& error) {
        success = false;
        error_message = error;
    }
};

// Import管理器
class ImportManager {
public:
    ImportManager();
    ~ImportManager();
    
    // 基本配置
    void setCurrentDirectory(const std::string& dir) { current_directory = dir; }
    void setOfficialModuleDirectory(const std::string& dir) { official_module_dir = dir; }
    void addModuleSearchPath(const std::string& path);
    void setUseModuleDirStructure(bool use) { use_module_dir_structure = use; }
    
    // Import处理
    
    // 解析Import语句
    ImportItem parseImportStatement(const std::string& statement);
    
    // 官方模块前缀处理 (chtl::)
    bool isOfficialModulePrefix(const std::string& path);
    std::string extractOfficialModuleName(const std::string& path);
    std::string resolveOfficialModulePath(const std::string& module_name);
    
    // 通配符导入处理 (.* 和 /*)
    bool isWildcardImport(const std::string& path);
    std::vector<std::string> expandWildcardPath(const std::string& wildcard_path);
    std::vector<std::string> findMatchingFiles(const std::string& directory, const std::string& pattern);
    
    // 子模块访问处理 (Module.Submodule)
    bool isSubmoduleAccess(const std::string& path);
    std::vector<std::string> parseSubmodulePath(const std::string& path);
    std::string resolveSubmodulePath(const std::string& module_path);
    
    // 添加Import项
    bool addImport(const ImportItem& item);
    
    // 处理Import项（解析路径，检查循环依赖等）
    bool processImport(const std::string& import_id);
    
    // 批量处理所有Import
    bool processAllImports();
    
    // 路径解析
    
    // 解析@Html/@Style/@JavaScript文件路径
    PathResolution resolveFilePath(const std::string& path, ImportType type);
    
    // 解析@Chtl模块路径
    PathResolution resolveChtlPath(const std::string& path);
    
    // 解析@CJmod模块路径  
    PathResolution resolveCJmodPath(const std::string& path);
    
    // 解析通配符路径（.*或/*）
    std::vector<std::string> resolveWildcardPath(const std::string& pattern);
    
    // 验证和检查
    
    // 检查循环依赖
    bool hasCircularDependency(const std::string& import_id) const;
    
    // 检查重复导入
    bool isDuplicateImport(const ImportItem& item) const;
    
    // 检查路径表达方式冲突（同一路径的不同表达方式）
    std::vector<std::string> checkPathExpressionConflicts() const;
    
    // 验证Import项的有效性
    bool validateImport(const ImportItem& item) const;
    
    // as语法处理
    
    // 检查是否需要as语法
    bool requiresAsClause(ImportType type) const;
    
    // 处理带名原始嵌入节点创建
    bool createNamedOriginNode(const ImportItem& item);
    
    // 检查as语法的有效性
    bool isValidAlias(const std::string& alias, ImportType type) const;
    
    // 特殊规则处理
    
    // 处理官方模块前缀（chtl::）
    bool isOfficialModulePrefix(const std::string& path) const;
    std::string resolveOfficialModule(const std::string& module_name);
    
    // 处理子模块导入（使用'.'或'/'分隔符）
    std::string resolveSubModule(const std::string& parent_module, const std::string& sub_module);
    
    // 状态查询
    
    // 获取所有Import项
    std::vector<ImportItem> getAllImports() const;
    
    // 获取特定类型的Import项
    std::vector<ImportItem> getImportsByType(ImportType type) const;
    
    // 获取未处理的Import项
    std::vector<ImportItem> getPendingImports() const;
    
    // 获取Import依赖图
    std::unordered_map<std::string, std::vector<std::string>> getDependencyGraph() const;
    
    // 获取处理顺序
    std::vector<std::string> getProcessingOrder() const;
    
    // 错误处理
    std::vector<std::string> getErrors() const { return errors; }
    std::vector<std::string> getWarnings() const { return warnings; }
    void clearErrors() { errors.clear(); }
    void clearWarnings() { warnings.clear(); }
    
    // 调试和诊断
    void dumpImportState() const;
    void dumpPathResolutions() const;
    void dumpDependencyGraph() const;
    
    // 清理
    void clear();
    void removeImport(const std::string& import_id);

private:
    // 配置
    std::string current_directory;
    std::string official_module_dir;
    std::vector<std::string> module_search_paths;
    bool use_module_dir_structure;
    
    // Import存储
    std::unordered_map<std::string, ImportItem> imports;
    std::unordered_map<std::string, std::vector<std::string>> dependencies;
    std::unordered_set<std::string> processed_imports;
    
    // 路径缓存
    std::unordered_map<std::string, PathResolution> path_cache;
    std::unordered_map<std::string, std::string> normalized_paths;
    
    // 错误和警告
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    // 计数器
    size_t import_counter;
    
    // 内部方法
    
    // Import语句解析
    ImportType parseImportType(const std::string& type_string) const;
    std::string extractTargetName(const std::string& statement) const;
    std::string extractPath(const std::string& statement) const;
    std::string extractAlias(const std::string& statement) const;
    
    // 路径处理
    PathResolutionType classifyPath(const std::string& path) const;
    std::string normalizePath(const std::string& path) const;
    bool fileExists(const std::string& path) const;
    bool directoryExists(const std::string& path) const;
    
    // 搜索算法
    std::vector<std::string> searchInDirectory(const std::string& dir, const std::string& name, 
                                               const std::vector<std::string>& extensions) const;
    
    std::string findBestMatch(const std::vector<std::string>& candidates, ImportType type) const;
    
    // 模块目录结构处理
    std::vector<std::string> getModuleStructurePaths(const std::string& base_dir) const;
    bool isModuleStructureDir(const std::string& dir) const;
    
    // 依赖分析
    void buildDependencyGraph();
    bool hasCyclicDependencyHelper(const std::string& import_id, 
                                   std::unordered_set<std::string>& visited,
                                   std::unordered_set<std::string>& rec_stack) const;
    
    // 验证
    bool isValidImportPath(const std::string& path) const;
    bool isValidTargetName(const std::string& name, ImportType type) const;
    
    // 冲突检测
    std::string getCanonicalPath(const std::string& path) const;
    bool arePathsEquivalent(const std::string& path1, const std::string& path2) const;
    
    // 错误处理
    void addError(const std::string& message);
    void addWarning(const std::string& message);
    
    // 工具方法
    std::string generateImportId();
    std::string getImportTypeName(ImportType type) const;
    std::vector<std::string> getFileExtensions(ImportType type) const;
    
    // 文件系统操作
    std::vector<std::string> listDirectory(const std::string& dir) const;
    std::string getFileExtension(const std::string& path) const;
    std::string getBaseName(const std::string& path) const;
    std::string joinPath(const std::string& dir, const std::string& file) const;
};

// Import工具类
class ImportUtils {
public:
    // 解析Import语句的组件
    static bool parseImportStatement(const std::string& statement,
                                    std::string& type_part,
                                    std::string& target_part,
                                    std::string& path_part,
                                    std::string& alias_part);
    
    // 检查是否是无修饰字符串
    static bool isUnquotedString(const std::string& str);
    
    // 处理引号（支持无修饰字符串）
    static std::string stripQuotes(const std::string& str);
    
    // 验证路径格式
    static bool isValidPathFormat(const std::string& path);
    
    // 检查是否是通配符路径
    static bool isWildcardPath(const std::string& path);
    
    // 规范化模块名称
    static std::string normalizeModuleName(const std::string& name);
    
    // 获取默认文件扩展名
    static std::vector<std::string> getDefaultExtensions(ImportType type);
    
    // 检查官方模块前缀
    static bool hasOfficialPrefix(const std::string& path);
    static std::string stripOfficialPrefix(const std::string& path);
    
    // 路径工具
    static bool isAbsolutePath(const std::string& path);
    static bool isRelativePath(const std::string& path);
    static std::string makeAbsolute(const std::string& path, const std::string& base_dir);
};

} // namespace Common
} // namespace CHTL