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
    std::string sourcePath;      // 原始定义文件（如果是导入的）
    
    // 符号特定的额外信息
    std::unordered_map<std::string, std::string> metadata;
    
    SymbolInfo() : type(SymbolType::UNKNOWN), line(0), column(0), isImported(false) {}
};

// 全局符号注册表
class GlobalMap {
private:
    // 主符号表：文件路径 -> 符号列表
    std::unordered_map<std::string, std::vector<std::shared_ptr<SymbolInfo>>> symbolTable;
    
    // 符号索引：符号名 -> 符号信息列表（支持重名）
    std::unordered_map<std::string, std::vector<std::shared_ptr<SymbolInfo>>> symbolIndex;
    
    // 类型索引：符号类型 -> 符号信息列表
    std::unordered_map<SymbolType, std::vector<std::shared_ptr<SymbolInfo>>> typeIndex;
    
    // 命名空间索引：命名空间路径 -> 符号信息列表
    std::unordered_map<std::string, std::vector<std::shared_ptr<SymbolInfo>>> namespaceIndex;
    
    // 统计信息
    size_t totalSymbols;
    std::unordered_map<SymbolType, size_t> symbolCounts;
    
    // 辅助方法
    void updateIndices(std::shared_ptr<SymbolInfo> symbol);
    void removeFromIndices(std::shared_ptr<SymbolInfo> symbol);
    
public:
    GlobalMap();
    ~GlobalMap() = default;
    
    // 符号注册
    void registerSymbol(std::shared_ptr<SymbolInfo> symbol);
    void registerSymbol(const std::string& filePath, std::shared_ptr<SymbolInfo> symbol);
    void unregisterSymbol(const std::string& name, const std::string& filePath);
    
    // 符号查询
    std::shared_ptr<SymbolInfo> findSymbol(const std::string& name) const;
    std::shared_ptr<SymbolInfo> findSymbol(const std::string& name, const std::string& namespacePath) const;
    std::vector<std::shared_ptr<SymbolInfo>> findAllSymbols(const std::string& name) const;
    std::vector<std::shared_ptr<SymbolInfo>> findSymbolsByType(SymbolType type) const;
    std::vector<std::shared_ptr<SymbolInfo>> findSymbolsInNamespace(const std::string& namespacePath) const;
    std::vector<std::shared_ptr<SymbolInfo>> findSymbolsInFile(const std::string& filePath) const;
    
    // 符号存在性检查
    bool symbolExists(const std::string& name) const;
    bool symbolExists(const std::string& name, const std::string& namespacePath) const;
    bool symbolExistsInFile(const std::string& name, const std::string& filePath) const;
    
    // 符号冲突检测
    bool hasSymbolConflict(const std::string& name) const;
    bool hasSymbolConflict(const std::string& name, const std::string& namespacePath) const;
    std::vector<std::shared_ptr<SymbolInfo>> getConflictingSymbols(const std::string& name) const;
    
    // 批量操作
    void registerSymbols(const std::string& filePath, const std::vector<std::shared_ptr<SymbolInfo>>& symbols);
    void unregisterSymbolsFromFile(const std::string& filePath);
    void unregisterSymbolsFromNamespace(const std::string& namespacePath);
    
    // 符号更新
    void updateSymbol(std::shared_ptr<SymbolInfo> symbol);
    void updateSymbolNamespace(const std::string& symbolName, const std::string& oldNamespace, const std::string& newNamespace);
    void updateSymbolAlias(const std::string& symbolName, const std::string& namespacePath, const std::string& newAlias);
    
    // 统计信息
    size_t getTotalSymbolCount() const { return totalSymbols; }
    size_t getSymbolCount(SymbolType type) const;
    std::unordered_map<SymbolType, size_t> getSymbolStatistics() const;
    size_t getFileSymbolCount(const std::string& filePath) const;
    size_t getNamespaceSymbolCount(const std::string& namespacePath) const;
    
    // 清理
    void clear();
    void clearFile(const std::string& filePath);
    void clearNamespace(const std::string& namespacePath);
    
    // 验证
    bool validateSymbolTable() const;
    std::vector<std::string> getValidationErrors() const;
    
    // 调试输出
    void dumpSymbolTable() const;
    void dumpSymbolIndex() const;
    void dumpTypeIndex() const;
    void dumpNamespaceIndex() const;
    void dumpStatistics() const;
    
    // 迭代器支持
    using SymbolIterator = std::unordered_map<std::string, std::vector<std::shared_ptr<SymbolInfo>>>::const_iterator;
    SymbolIterator begin() const { return symbolTable.begin(); }
    SymbolIterator end() const { return symbolTable.end(); }
};

} // namespace CHTL

#endif // CHTL_GLOBAL_MAP_H