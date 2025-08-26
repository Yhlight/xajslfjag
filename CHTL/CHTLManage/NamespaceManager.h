#ifndef CHTL_NAMESPACE_MANAGER_H
#define CHTL_NAMESPACE_MANAGER_H

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <memory>

namespace CHTL {

// 前向声明
struct SymbolInfo;

// 命名空间信息
struct NamespaceInfo {
    std::string name;                       // 命名空间名称
    std::string fullPath;                   // 完整路径（如 space.room.area）
    std::string parentPath;                 // 父命名空间路径
    std::string sourcePath;                 // 定义所在文件
    size_t line;                            // 定义所在行
    size_t column;                          // 定义所在列
    bool isNested;                          // 是否为嵌套命名空间
    bool isImplicit;                        // 是否为隐式命名空间（文件名）
    std::unordered_set<std::string> children; // 子命名空间
    
    NamespaceInfo() : line(0), column(0), isNested(false), isImplicit(false) {}
};

// 命名空间符号表项
struct NamespaceSymbol {
    std::shared_ptr<SymbolInfo> symbol;     // 符号信息
    std::string namespacePath;              // 所属命名空间
    bool isExported;                        // 是否导出
    bool isImported;                        // 是否从其他命名空间导入
    std::string importedFrom;               // 导入来源
    
    NamespaceSymbol() : isExported(true), isImported(false) {}
};

// 命名空间管理器
class NamespaceManager {
private:
    // 命名空间表：完整路径 -> 命名空间信息
    std::unordered_map<std::string, NamespaceInfo> namespaceTable;
    
    // 命名空间符号表：命名空间路径 -> 符号列表
    std::unordered_map<std::string, std::vector<NamespaceSymbol>> namespaceSymbols;
    
    // 文件默认命名空间：文件路径 -> 默认命名空间
    std::unordered_map<std::string, std::string> fileDefaultNamespaces;
    
    // 命名空间别名：别名 -> 实际命名空间路径
    std::unordered_map<std::string, std::string> namespaceAliases;
    
    // 当前活动命名空间栈
    std::vector<std::string> namespaceStack;
    
    // 是否禁用默认命名空间
    bool disableDefaultNamespace;
    
    // 辅助方法
    std::string buildFullPath(const std::string& parent, const std::string& name) const;
    std::vector<std::string> splitNamespacePath(const std::string& path) const;
    void createParentNamespaces(const std::string& fullPath, const std::string& sourcePath);
    
public:
    NamespaceManager();
    ~NamespaceManager() = default;
    
    // 命名空间创建和管理
    void createNamespace(const std::string& name, const std::string& sourcePath, size_t line = 0, size_t column = 0);
    void createNestedNamespace(const std::string& path, const std::string& sourcePath, size_t line = 0, size_t column = 0);
    bool namespaceExists(const std::string& path) const;
    NamespaceInfo getNamespace(const std::string& path) const;
    
    // 命名空间栈管理
    void enterNamespace(const std::string& name);
    void exitNamespace();
    std::string getCurrentNamespace() const;
    std::vector<std::string> getNamespaceStack() const;
    void resetNamespaceStack();
    
    // 默认命名空间
    void setFileDefaultNamespace(const std::string& filePath, const std::string& namespaceName);
    std::string getFileDefaultNamespace(const std::string& filePath) const;
    void setDisableDefaultNamespace(bool disable) { disableDefaultNamespace = disable; }
    bool isDefaultNamespaceDisabled() const { return disableDefaultNamespace; }
    
    // 命名空间别名
    void addNamespaceAlias(const std::string& alias, const std::string& actualPath);
    std::string resolveNamespaceAlias(const std::string& alias) const;
    bool isNamespaceAlias(const std::string& name) const;
    
    // 符号管理
    void addSymbolToNamespace(const std::string& namespacePath, std::shared_ptr<SymbolInfo> symbol);
    void addImportedSymbol(const std::string& namespacePath, std::shared_ptr<SymbolInfo> symbol, const std::string& importedFrom);
    std::vector<NamespaceSymbol> getNamespaceSymbols(const std::string& namespacePath) const;
    std::shared_ptr<SymbolInfo> findSymbolInNamespace(const std::string& symbolName, const std::string& namespacePath) const;
    
    // 命名空间合并
    void mergeNamespaces(const std::string& targetPath, const std::string& sourcePath);
    bool canMergeNamespaces(const std::string& path1, const std::string& path2) const;
    std::vector<std::string> detectMergeConflicts(const std::string& path1, const std::string& path2) const;
    
    // 完整限定名
    std::string getFullyQualifiedName(const std::string& name) const;
    std::string getFullyQualifiedName(const std::string& name, const std::string& namespacePath) const;
    bool isFullyQualified(const std::string& name) const;
    
    // 命名空间解析
    std::string resolveNamespacePath(const std::string& path) const;
    std::vector<std::string> resolveNamespaceImports(const std::string& namespacePath) const;
    std::vector<std::string> getChildNamespaces(const std::string& parentPath) const;
    std::vector<std::string> getAllNamespaces() const;
    
    // 符号查找
    std::shared_ptr<SymbolInfo> lookupSymbol(const std::string& name) const;
    std::shared_ptr<SymbolInfo> lookupSymbol(const std::string& name, const std::string& startNamespace) const;
    std::vector<std::shared_ptr<SymbolInfo>> findAllSymbols(const std::string& name) const;
    
    // 清理
    void clearNamespace(const std::string& namespacePath);
    void clearFileNamespaces(const std::string& filePath);
    void clearAll();
    
    // 验证
    bool validateNamespacePath(const std::string& path) const;
    bool hasNamespaceConflict(const std::string& name) const;
    std::vector<std::string> getConflictingNamespaces(const std::string& name) const;
    
    // 调试
    void dumpNamespaceTable() const;
    void dumpNamespaceSymbols() const;
    void dumpNamespaceTree() const;
    void dumpCurrentScope() const;
};

} // namespace CHTL

#endif // CHTL_NAMESPACE_MANAGER_H