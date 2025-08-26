#ifndef CHTL_SYMBOL_MANAGER_H
#define CHTL_SYMBOL_MANAGER_H

#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include "../CHTLLexer/GlobalMap.h"

namespace CHTL {

// 符号管理器 - 负责符号的创建、注册和查询
class SymbolManager {
private:
    std::shared_ptr<GlobalMap> symbolRegistry;  // 使用GlobalMap作为底层存储
    
    // 符号工厂方法
    std::shared_ptr<SymbolInfo> createSymbol(
        const std::string& name,
        SymbolType type,
        const std::string& filePath,
        size_t line,
        size_t column
    );
    
public:
    SymbolManager(std::shared_ptr<GlobalMap> registry);
    ~SymbolManager() = default;
    
    // 符号创建
    std::shared_ptr<SymbolInfo> createTemplateSymbol(
        const std::string& name,
        const std::string& type,  // @Style, @Element, @Var
        const std::string& filePath,
        size_t line,
        size_t column
    );
    
    std::shared_ptr<SymbolInfo> createCustomSymbol(
        const std::string& name,
        const std::string& type,  // @Style, @Element, @Var
        const std::string& filePath,
        size_t line,
        size_t column
    );
    
    std::shared_ptr<SymbolInfo> createOriginSymbol(
        const std::string& name,
        const std::string& type,  // @Html, @Style, @JavaScript, 或自定义
        const std::string& filePath,
        size_t line,
        size_t column
    );
    
    std::shared_ptr<SymbolInfo> createNamespaceSymbol(
        const std::string& name,
        const std::string& filePath,
        size_t line,
        size_t column
    );
    
    std::shared_ptr<SymbolInfo> createConfigSymbol(
        const std::string& name,
        const std::string& filePath,
        size_t line,
        size_t column
    );
    
    // 符号注册
    void registerSymbol(std::shared_ptr<SymbolInfo> symbol);
    void registerSymbolWithNamespace(std::shared_ptr<SymbolInfo> symbol, const std::string& namespacePath);
    void registerImportedSymbol(std::shared_ptr<SymbolInfo> symbol, const std::string& sourcePath);
    
    // 符号查询
    std::shared_ptr<SymbolInfo> findSymbol(const std::string& name) const;
    std::shared_ptr<SymbolInfo> findSymbol(const std::string& name, const std::string& namespacePath) const;
    std::vector<std::shared_ptr<SymbolInfo>> findAllSymbols(const std::string& name) const;
    std::vector<std::shared_ptr<SymbolInfo>> findSymbolsByType(SymbolType type) const;
    std::vector<std::shared_ptr<SymbolInfo>> findSymbolsByType(const std::string& type) const;
    
    // 符号验证
    bool symbolExists(const std::string& name) const;
    bool symbolExists(const std::string& name, const std::string& namespacePath) const;
    bool hasConflict(const std::string& name) const;
    bool hasConflict(const std::string& name, const std::string& namespacePath) const;
    
    // 符号更新
    void updateSymbolMetadata(const std::string& name, const std::string& key, const std::string& value);
    void updateSymbolAlias(const std::string& name, const std::string& alias);
    void markSymbolAsImported(const std::string& name, const std::string& sourcePath);
    
    // 批量操作
    void registerSymbols(const std::vector<std::shared_ptr<SymbolInfo>>& symbols);
    void unregisterSymbolsFromFile(const std::string& filePath);
    
    // 统计
    size_t getTotalSymbolCount() const;
    size_t getSymbolCount(SymbolType type) const;
    std::unordered_map<SymbolType, size_t> getSymbolStatistics() const;
};

} // namespace CHTL

#endif // CHTL_SYMBOL_MANAGER_H