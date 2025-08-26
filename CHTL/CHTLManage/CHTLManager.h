#ifndef CHTL_MANAGER_H
#define CHTL_MANAGER_H

#include <memory>
#include <string>
#include "ImportManager.h"
#include "NamespaceManager.h"
#include "ConfigManager.h"
#include "../CHTLLexer/GlobalMap.h"

namespace CHTL {

// CHTL管理器 - 协调各个子管理器
class CHTLManager {
private:
    // 子管理器
    std::unique_ptr<ImportManager> importManager;
    std::unique_ptr<NamespaceManager> namespaceManager;
    std::unique_ptr<ConfigManager> configManager;
    
    // 全局注册表
    std::shared_ptr<GlobalMap> globalMap;
    
    // 当前编译文件
    std::string currentFilePath;
    std::string currentDirectory;
    
    // 辅助方法
    void syncManagersState();
    void updateGlobalMap();
    
public:
    CHTLManager();
    ~CHTLManager() = default;
    
    // 获取子管理器
    ImportManager* getImportManager() { return importManager.get(); }
    NamespaceManager* getNamespaceManager() { return namespaceManager.get(); }
    ConfigManager* getConfigManager() { return configManager.get(); }
    const ImportManager* getImportManager() const { return importManager.get(); }
    const NamespaceManager* getNamespaceManager() const { return namespaceManager.get(); }
    const ConfigManager* getConfigManager() const { return configManager.get(); }
    
    // 获取全局注册表
    std::shared_ptr<GlobalMap> getGlobalMap() { return globalMap; }
    std::shared_ptr<const GlobalMap> getGlobalMap() const { return globalMap; }
    
    // 文件管理
    void setCurrentFile(const std::string& filePath);
    std::string getCurrentFile() const { return currentFilePath; }
    std::string getCurrentDirectory() const { return currentDirectory; }
    
    // 初始化
    void initialize();
    void initializeForFile(const std::string& filePath);
    void reset();
    
    // 配置加载
    void loadConfiguration(const std::string& configName = "");
    void applyConfiguration();
    
    // 导入处理
    void processImport(const ImportRecord& record);
    void resolveAllImports();
    bool checkCircularDependencies();
    
    // 命名空间处理
    void enterNamespace(const std::string& name);
    void exitNamespace();
    std::string getCurrentNamespace() const;
    
    // 符号注册
    void registerSymbol(std::shared_ptr<SymbolInfo> symbol);
    void registerSymbolInNamespace(std::shared_ptr<SymbolInfo> symbol, const std::string& namespacePath);
    
    // 符号查找
    std::shared_ptr<SymbolInfo> findSymbol(const std::string& name) const;
    std::shared_ptr<SymbolInfo> findSymbol(const std::string& name, const std::string& namespacePath) const;
    std::vector<std::shared_ptr<SymbolInfo>> findSymbolsByType(SymbolType type) const;
    
    // 路径解析
    std::string resolvePath(const std::string& path, ImportType type) const;
    std::string resolveModulePath(const std::string& moduleName, bool isCJmod = false) const;
    
    // 验证
    bool validateImports() const;
    bool validateNamespaces() const;
    bool validateConfiguration() const;
    std::vector<std::string> getAllErrors() const;
    
    // 清理
    void clearCurrentFile();
    void clearAll();
    
    // 调试
    void dumpState() const;
    void dumpImports() const;
    void dumpNamespaces() const;
    void dumpConfiguration() const;
    void dumpSymbols() const;
};

} // namespace CHTL

#endif // CHTL_MANAGER_H