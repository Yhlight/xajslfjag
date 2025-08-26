#ifndef CHTL_GLOBAL_MAP_H
#define CHTL_GLOBAL_MAP_H

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <memory>

namespace CHTL {

// 符号类型
enum class SymbolType {
    ELEMENT_TEMPLATE,
    STYLE_TEMPLATE,
    VAR_TEMPLATE,
    ELEMENT_CUSTOM,
    STYLE_CUSTOM,
    VAR_CUSTOM,
    ORIGIN_EMBED,
    NAMESPACE,
    CONFIG,
    UNKNOWN
};

// 符号信息
struct SymbolInfo {
    std::string name;
    SymbolType type;
    std::string filePath;
    size_t line;
    size_t column;
    std::string namespacePath;  // 命名空间路径，如 "space.room"
    bool isImported;
    std::string importAlias;     // 导入时的别名
    
    SymbolInfo() : type(SymbolType::UNKNOWN), line(0), column(0), isImported(false) {}
};

// 导入信息
struct ImportInfo {
    std::string sourcePath;      // 导入源文件路径
    std::string targetPath;      // 被导入文件路径
    std::string namespaceName;   // 导入的命名空间
    std::vector<std::string> symbols;  // 导入的符号列表
    bool isWildcard;             // 是否通配符导入
    
    ImportInfo() : isWildcard(false) {}
};

// 配置信息
struct ConfigInfo {
    std::string name;
    std::unordered_map<std::string, std::string> options;
    std::unordered_map<std::string, std::vector<std::string>> nameGroup;
    std::unordered_map<std::string, std::string> originTypes;
    bool isDefault;  // 是否为默认配置（无名配置）
    
    ConfigInfo() : isDefault(false) {}
};

// 全局符号表管理器
class GlobalMap {
private:
    // 符号表：文件路径 -> 符号列表
    std::unordered_map<std::string, std::vector<std::shared_ptr<SymbolInfo>>> symbolTable;
    
    // 命名空间表：命名空间路径 -> 符号列表
    std::unordered_map<std::string, std::vector<std::shared_ptr<SymbolInfo>>> namespaceTable;
    
    // 导入表：文件路径 -> 导入信息列表
    std::unordered_map<std::string, std::vector<ImportInfo>> importTable;
    
    // 配置表：配置名 -> 配置信息
    std::unordered_map<std::string, ConfigInfo> configTable;
    
    // 官方模块前缀
    static const std::string OFFICIAL_MODULE_PREFIX;
    
    // 当前活动配置
    std::string activeConfig;
    
    // 循环依赖检测
    std::unordered_set<std::string> importStack;
    
public:
    GlobalMap();
    ~GlobalMap() = default;
    
    // 符号管理
    void addSymbol(const std::string& filePath, std::shared_ptr<SymbolInfo> symbol);
    std::vector<std::shared_ptr<SymbolInfo>> getSymbols(const std::string& filePath) const;
    std::shared_ptr<SymbolInfo> findSymbol(const std::string& name, const std::string& namespacePath = "") const;
    std::vector<std::shared_ptr<SymbolInfo>> findSymbolsByType(SymbolType type) const;
    
    // 命名空间管理
    void addToNamespace(const std::string& namespacePath, std::shared_ptr<SymbolInfo> symbol);
    std::vector<std::shared_ptr<SymbolInfo>> getNamespaceSymbols(const std::string& namespacePath) const;
    void mergeNamespaces(const std::string& namespacePath1, const std::string& namespacePath2);
    bool namespaceExists(const std::string& namespacePath) const;
    
    // 导入管理
    void addImport(const std::string& sourcePath, const ImportInfo& import);
    std::vector<ImportInfo> getImports(const std::string& filePath) const;
    bool checkCircularDependency(const std::string& sourcePath, const std::string& targetPath);
    void resolveImports(const std::string& filePath);
    
    // 配置管理
    void addConfig(const std::string& name, const ConfigInfo& config);
    ConfigInfo getConfig(const std::string& name) const;
    ConfigInfo getActiveConfig() const;
    void setActiveConfig(const std::string& name);
    bool hasDefaultConfig() const;
    
    // 路径解析
    std::string resolveModulePath(const std::string& moduleName, const std::string& currentPath, bool isCJmod = false) const;
    std::string resolveFilePath(const std::string& path, const std::string& currentPath) const;
    bool isOfficialModule(const std::string& moduleName) const;
    
    // 冲突检测
    bool hasSymbolConflict(const std::string& name, const std::string& namespacePath = "") const;
    std::vector<std::shared_ptr<SymbolInfo>> getConflictingSymbols(const std::string& name) const;
    
    // 清理和重置
    void clear();
    void clearFile(const std::string& filePath);
    
    // 调试输出
    void dumpSymbolTable() const;
    void dumpNamespaceTable() const;
    void dumpImportTable() const;
};

} // namespace CHTL

#endif // CHTL_GLOBAL_MAP_H