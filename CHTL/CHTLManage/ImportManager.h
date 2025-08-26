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
    std::string targetPath;         // 被导入的文件路径（已解析）
    std::string rawPath;            // 原始路径（导入语句中的路径）
    ImportType baseType;            // 基础导入类型
    std::vector<ImportItem> items;  // 导入的具体项
    std::string namespaceName;      // 导入到的命名空间
    size_t line;                    // 导入语句所在行
    size_t column;                  // 导入语句所在列
    bool resolved;                  // 是否已解析
    std::string errorMessage;       // 错误信息（如果有）
    
    ImportRecord() : baseType(ImportType::CHTL_MODULE), line(0), column(0), resolved(false) {}
};

// 导入管理器 - 负责导入记录的管理
class ImportManager {
private:
    // 导入记录表：源文件路径 -> 导入记录列表
    std::unordered_map<std::string, std::vector<ImportRecord>> importTable;
    
    // 导入索引：目标文件 -> 源文件列表（反向索引）
    std::unordered_map<std::string, std::unordered_set<std::string>> importIndex;
    
    // 导入别名表：别名 -> 原始名称
    std::unordered_map<std::string, std::string> aliasTable;
    
    // 辅助方法
    void updateIndex(const ImportRecord& record);
    void removeFromIndex(const ImportRecord& record);
    
public:
    ImportManager();
    ~ImportManager() = default;
    
    // 导入记录管理
    void addImportRecord(const ImportRecord& record);
    void updateImportRecord(const std::string& sourcePath, size_t index, const ImportRecord& record);
    void removeImportRecord(const std::string& sourcePath, size_t index);
    
    // 导入查询
    std::vector<ImportRecord> getImports(const std::string& sourcePath) const;
    std::vector<ImportRecord> getImportsByType(const std::string& sourcePath, ImportType type) const;
    ImportRecord* findImport(const std::string& sourcePath, const std::string& targetPath);
    const ImportRecord* findImport(const std::string& sourcePath, const std::string& targetPath) const;
    
    // 导入项查询
    std::vector<ImportItem> getImportedItems(const std::string& sourcePath, const std::string& targetPath) const;
    bool hasImportedItem(const std::string& sourcePath, const std::string& itemName) const;
    ImportItem* findImportedItem(const std::string& sourcePath, const std::string& itemName);
    
    // 别名管理
    void registerAlias(const std::string& alias, const std::string& originalName, const std::string& scope = "");
    std::string resolveAlias(const std::string& alias, const std::string& scope = "") const;
    bool isAlias(const std::string& name, const std::string& scope = "") const;
    std::unordered_map<std::string, std::string> getAliasesInScope(const std::string& scope) const;
    
    // 反向查询
    std::unordered_set<std::string> getImporters(const std::string& targetPath) const;
    bool isImportedBy(const std::string& targetPath, const std::string& sourcePath) const;
    size_t getImporterCount(const std::string& targetPath) const;
    
    // 导入解析状态
    void markAsResolved(const std::string& sourcePath, const std::string& targetPath);
    void markAsError(const std::string& sourcePath, const std::string& targetPath, const std::string& error);
    bool isResolved(const std::string& sourcePath, const std::string& targetPath) const;
    std::vector<ImportRecord> getUnresolvedImports() const;
    std::vector<ImportRecord> getErrorImports() const;
    
    // 批量操作
    void clearImports(const std::string& sourcePath);
    void clearAllImports();
    void removeUnresolvedImports();
    
    // 验证
    bool hasImportCycle(const std::string& sourcePath) const;
    bool validateImports(const std::string& sourcePath) const;
    std::vector<std::string> getImportErrors(const std::string& sourcePath) const;
    
    // 统计
    size_t getTotalImportCount() const;
    size_t getImportCount(const std::string& sourcePath) const;
    std::unordered_map<ImportType, size_t> getImportStatistics() const;
    
    // 调试
    void dumpImportTable() const;
    void dumpImportIndex() const;
    void dumpAliasTable() const;
};

} // namespace CHTL

#endif // CHTL_IMPORT_MANAGER_H