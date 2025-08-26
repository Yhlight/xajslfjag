#ifndef CHTL_IMPORT_MANAGER_H
#define CHTL_IMPORT_MANAGER_H

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <memory>

namespace CHTL {

// 导入类型
enum class ImportType {
    CHTL_MODULE,        // @Chtl
    CJMOD_MODULE,       // @CJmod
    HTML_FILE,          // @Html
    STYLE_FILE,         // @Style
    JAVASCRIPT_FILE,    // @JavaScript
    TEMPLATE_ELEMENT,   // [Template] @Element
    TEMPLATE_STYLE,     // [Template] @Style
    TEMPLATE_VAR,       // [Template] @Var
    CUSTOM_ELEMENT,     // [Custom] @Element
    CUSTOM_STYLE,       // [Custom] @Style
    CUSTOM_VAR,         // [Custom] @Var
    ORIGIN_EMBED,       // [Origin]
    CONFIG              // [Configuration] @Config
};

// 导入项信息
struct ImportItem {
    ImportType type;
    std::string itemName;           // 具体的项名（如元素名、样式组名等）
    std::string alias;              // as别名
    bool isWildcard;                // 是否为通配符导入
    std::string specificType;       // 具体类型（如@Style, @Element等）
    
    ImportItem() : type(ImportType::CHTL_MODULE), isWildcard(false) {}
};

// 导入记录
struct ImportRecord {
    std::string sourcePath;         // 导入语句所在文件
    std::string targetPath;         // 被导入的文件路径
    std::string rawPath;            // 原始路径（导入语句中的路径）
    ImportType baseType;            // 基础导入类型
    std::vector<ImportItem> items;  // 导入的具体项
    std::string namespaceName;      // 导入到的命名空间
    size_t line;                    // 导入语句所在行
    size_t column;                  // 导入语句所在列
    bool resolved;                  // 是否已解析
    
    ImportRecord() : baseType(ImportType::CHTL_MODULE), line(0), column(0), resolved(false) {}
};

// 路径解析结果
struct ResolvedPath {
    std::string fullPath;           // 完整路径
    bool found;                     // 是否找到
    bool isOfficialModule;          // 是否为官方模块
    std::string errorMessage;       // 错误信息
    
    ResolvedPath() : found(false), isOfficialModule(false) {}
};

// 导入管理器
class ImportManager {
private:
    // 导入记录表：源文件路径 -> 导入记录列表
    std::unordered_map<std::string, std::vector<ImportRecord>> importTable;
    
    // 依赖图：文件路径 -> 依赖的文件路径集合
    std::unordered_map<std::string, std::unordered_set<std::string>> dependencyGraph;
    
    // 反向依赖图：文件路径 -> 依赖它的文件路径集合
    std::unordered_map<std::string, std::unordered_set<std::string>> reverseDependencyGraph;
    
    // 循环依赖检测栈
    std::unordered_set<std::string> visitStack;
    std::unordered_set<std::string> visited;
    
    // 官方模块前缀
    static const std::string OFFICIAL_MODULE_PREFIX;
    
    // 路径缓存
    mutable std::unordered_map<std::string, ResolvedPath> pathCache;
    
    // 辅助方法
    bool detectCycleUtil(const std::string& node, std::vector<std::string>& cycle);
    void buildDependencyGraph();
    std::string normalizePathPattern(const std::string& pattern) const;
    
public:
    ImportManager();
    ~ImportManager() = default;
    
    // 添加导入记录
    void addImport(const ImportRecord& record);
    void addImport(const std::string& sourcePath, const ImportRecord& record);
    
    // 获取导入记录
    std::vector<ImportRecord> getImports(const std::string& sourcePath) const;
    std::vector<ImportRecord> getImportsByType(const std::string& sourcePath, ImportType type) const;
    ImportRecord* findImport(const std::string& sourcePath, const std::string& targetPath);
    
    // 路径解析
    ResolvedPath resolvePath(const std::string& path, const std::string& currentPath, ImportType type) const;
    ResolvedPath resolveModulePath(const std::string& moduleName, const std::string& currentPath, bool isCJmod = false) const;
    ResolvedPath resolveFilePath(const std::string& filePath, const std::string& currentPath, const std::string& extension) const;
    ResolvedPath resolveWildcardPath(const std::string& pattern, const std::string& currentPath, ImportType type) const;
    
    // 子模块解析
    std::vector<std::string> resolveSubmodules(const std::string& moduleName, const std::string& currentPath) const;
    bool isSubmodule(const std::string& moduleName) const;
    std::pair<std::string, std::string> splitModulePath(const std::string& moduleName) const;
    
    // 官方模块
    bool isOfficialModule(const std::string& moduleName) const;
    std::string stripOfficialPrefix(const std::string& moduleName) const;
    
    // 依赖管理
    void addDependency(const std::string& source, const std::string& target);
    void removeDependency(const std::string& source, const std::string& target);
    std::unordered_set<std::string> getDependencies(const std::string& source) const;
    std::unordered_set<std::string> getDependents(const std::string& target) const;
    
    // 循环依赖检测
    bool hasCircularDependency(const std::string& source, const std::string& target);
    std::vector<std::string> findCircularDependency(const std::string& source);
    bool checkCircularDependency();
    
    // 导入解析
    void resolveImport(ImportRecord& record, const std::string& currentPath);
    void resolveAllImports(const std::string& sourcePath);
    bool isImportResolved(const std::string& sourcePath, const std::string& targetPath) const;
    
    // 清理
    void clearImports(const std::string& sourcePath);
    void clearAll();
    void clearPathCache();
    
    // 验证
    bool validateImport(const ImportRecord& record) const;
    std::vector<std::string> getImportErrors(const std::string& sourcePath) const;
    
    // 调试
    void dumpImportTable() const;
    void dumpDependencyGraph() const;
    void dumpCircularDependencies() const;
};

} // namespace CHTL

#endif // CHTL_IMPORT_MANAGER_H