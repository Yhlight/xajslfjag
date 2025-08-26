#ifndef CHTL_MANAGER_H
#define CHTL_MANAGER_H

#include <memory>
#include <string>
#include "ImportManager.h"
#include "NamespaceManager.h"
#include "ConfigManager.h"
#include "SymbolManager.h"
#include "PathManager.h"
#include "DependencyManager.h"
#include "SelectorManager.h"
#include "InheritanceManager.h"
#include "LocalBlockManager.h"
#include "../CHTLLexer/GlobalMap.h"

namespace CHTL {

// CHTL管理器 - 协调所有细粒度管理器
class CHTLManager {
private:
    // 核心管理器
    std::unique_ptr<SymbolManager> symbolManager;
    std::unique_ptr<PathManager> pathManager;
    std::unique_ptr<ImportManager> importManager;
    std::unique_ptr<NamespaceManager> namespaceManager;
    std::unique_ptr<ConfigManager> configManager;
    std::unique_ptr<DependencyManager> dependencyManager;
    std::unique_ptr<SelectorManager> selectorManager;
    std::unique_ptr<InheritanceManager> inheritanceManager;
    std::unique_ptr<LocalBlockManager> localBlockManager;
    
    // 全局符号注册表
    std::shared_ptr<GlobalMap> globalMap;
    
    // 当前编译状态
    std::string currentFilePath;
    std::string currentDirectory;
    
    // 辅助方法
    void initializeManagers();
    void syncManagersState();
    
public:
    CHTLManager();
    ~CHTLManager() = default;
    
    // 获取管理器
    SymbolManager* getSymbolManager() { return symbolManager.get(); }
    PathManager* getPathManager() { return pathManager.get(); }
    ImportManager* getImportManager() { return importManager.get(); }
    NamespaceManager* getNamespaceManager() { return namespaceManager.get(); }
    ConfigManager* getConfigManager() { return configManager.get(); }
    DependencyManager* getDependencyManager() { return dependencyManager.get(); }
    SelectorManager* getSelectorManager() { return selectorManager.get(); }
    InheritanceManager* getInheritanceManager() { return inheritanceManager.get(); }
    LocalBlockManager* getLocalBlockManager() { return localBlockManager.get(); }
    
    // Const版本
    const SymbolManager* getSymbolManager() const { return symbolManager.get(); }
    const PathManager* getPathManager() const { return pathManager.get(); }
    const ImportManager* getImportManager() const { return importManager.get(); }
    const NamespaceManager* getNamespaceManager() const { return namespaceManager.get(); }
    const ConfigManager* getConfigManager() const { return configManager.get(); }
    const DependencyManager* getDependencyManager() const { return dependencyManager.get(); }
    const SelectorManager* getSelectorManager() const { return selectorManager.get(); }
    const InheritanceManager* getInheritanceManager() const { return inheritanceManager.get(); }
    const LocalBlockManager* getLocalBlockManager() const { return localBlockManager.get(); }
    
    // 全局注册表访问
    std::shared_ptr<GlobalMap> getGlobalMap() { return globalMap; }
    std::shared_ptr<const GlobalMap> getGlobalMap() const { return globalMap; }
    
    // ===== 文件管理 =====
    void setCurrentFile(const std::string& filePath);
    std::string getCurrentFile() const { return currentFilePath; }
    std::string getCurrentDirectory() const { return currentDirectory; }
    
    // ===== 初始化和重置 =====
    void initialize();
    void initializeForFile(const std::string& filePath);
    void reset();
    void clearCurrentFile();
    
    // ===== 配置管理（委托给ConfigManager）=====
    void loadConfiguration(const std::string& configName = "");
    void applyConfiguration();
    bool hasActiveConfiguration() const;
    std::string getConfigOption(const std::string& key) const;
    
    // ===== 路径解析（委托给PathManager）=====
    std::string resolvePath(const std::string& path) const;
    std::string resolveModulePath(const std::string& moduleName, bool isCJmod = false) const;
    std::string resolveImportPath(const std::string& path, ImportType type) const;
    
    // ===== 导入处理（协调多个管理器）=====
    void processImport(const ImportRecord& record);
    void resolveImport(ImportRecord& record);
    bool validateImport(const ImportRecord& record) const;
    
    // ===== 命名空间管理（委托给NamespaceManager）=====
    void enterNamespace(const std::string& name);
    void exitNamespace();
    std::string getCurrentNamespace() const;
    
    // ===== 符号管理（协调SymbolManager和GlobalMap）=====
    std::shared_ptr<SymbolInfo> createSymbol(
        const std::string& name,
        const std::string& type,
        SymbolType symbolType
    );
    void registerSymbol(std::shared_ptr<SymbolInfo> symbol);
    std::shared_ptr<SymbolInfo> findSymbol(const std::string& name) const;
    std::shared_ptr<SymbolInfo> findSymbol(const std::string& name, const std::string& namespacePath) const;
    
    // ===== 依赖管理（委托给DependencyManager）=====
    void addFileDependency(const std::string& source, const std::string& target);
    bool checkCircularDependencies() const;
    std::vector<std::string> getCompilationOrder() const;
    
    // ===== 选择器管理（委托给SelectorManager）=====
    SelectorInfo parseSelector(const std::string& selector) const;
    void configureSelectorRules();
    
    // ===== 继承管理（委托给InheritanceManager）=====
    void addInheritance(const std::string& child, const std::string& parent, const std::string& type);
    InheritanceResolution resolveInheritance(const std::string& node) const;
    
    // ===== 局部块管理（委托给LocalBlockManager）=====
    void enterElement(const std::string& elementName);
    void exitElement();
    void addLocalStyleBlock(const LocalBlockInfo& blockInfo);
    void addLocalScriptBlock(const LocalBlockInfo& blockInfo);
    
    // ===== 综合验证 =====
    bool validateAll() const;
    std::vector<std::string> getAllErrors() const;
    
    // ===== 调试 =====
    void dumpState() const;
    void dumpStatistics() const;
};

} // namespace CHTL

#endif // CHTL_MANAGER_H