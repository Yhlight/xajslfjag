#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <vector>
#include <set>

namespace CHTL {

// 全局符号类型
enum class GlobalSymbolType {
    TEMPLATE_STYLE,
    TEMPLATE_ELEMENT,
    TEMPLATE_VAR,
    CUSTOM_STYLE,
    CUSTOM_ELEMENT,
    CUSTOM_VAR,
    ORIGIN_HTML,
    ORIGIN_STYLE,
    ORIGIN_JAVASCRIPT,
    ORIGIN_CUSTOM,
    NAMESPACE,
    CONFIGURATION,
    IMPORT_INFO
};

// 命名空间项类型
enum class NamespaceItemType {
    CUSTOM_ELEMENT,
    CUSTOM_STYLE,
    CUSTOM_VAR,
    TEMPLATE_ELEMENT,
    TEMPLATE_STYLE,
    TEMPLATE_VAR,
    ORIGIN_HTML,
    ORIGIN_STYLE,
    ORIGIN_JAVASCRIPT,
    ORIGIN_CUSTOM
};

// 命名空间项
struct NamespaceItem {
    std::string name;
    NamespaceItemType type;
    std::string sourceFile;
    size_t line;
    size_t column;
    std::string content;
    
    NamespaceItem(const std::string& name, NamespaceItemType type,
                  const std::string& sourceFile = "", size_t line = 0, size_t column = 0,
                  const std::string& content = "")
        : name(name), type(type), sourceFile(sourceFile), line(line), column(column), content(content) {}
};

// 命名空间
struct Namespace {
    std::string name;
    std::string sourceFile;
    std::vector<std::shared_ptr<NamespaceItem>> items;
    std::vector<std::string> subNamespaces;
    std::unordered_map<std::string, std::shared_ptr<Namespace>> nestedNamespaces;
    
    Namespace(const std::string& name, const std::string& sourceFile = "")
        : name(name), sourceFile(sourceFile) {}
};

// 命名空间冲突
struct NamespaceConflict {
    std::string itemName;
    NamespaceItemType type;
    std::vector<std::string> conflictingNamespaces;
    std::vector<std::string> sourceFiles;
    std::vector<size_t> lineNumbers;
    
    NamespaceConflict(const std::string& itemName, NamespaceItemType type)
        : itemName(itemName), type(type) {}
};

// 配置组
struct ConfigurationGroup {
    std::string name;
    std::unordered_map<std::string, std::string> configItems;
    std::unordered_map<std::string, std::vector<std::string>> nameGroup;
    std::unordered_map<std::string, std::string> originTypes;
    std::string sourceFile;
    size_t line;
    
    ConfigurationGroup(const std::string& name = "", const std::string& sourceFile = "", size_t line = 0)
        : name(name), sourceFile(sourceFile), line(line) {}
};

// 导入信息
struct ImportInfo {
    std::string type;
    std::string path;
    std::string alias;
    std::string sourceFile;
    size_t line;
    bool isWildcard;
    std::vector<std::string> subModules;
    
    ImportInfo(const std::string& type, const std::string& path, const std::string& alias = "",
               const std::string& sourceFile = "", size_t line = 0)
        : type(type), path(path), alias(alias), sourceFile(sourceFile), line(line), isWildcard(false) {}
};

// 全局符号信息
struct GlobalSymbol {
    std::string name;
    GlobalSymbolType type;
    std::string namespace_;
    std::string filePath;
    size_t line;
    size_t column;
    
    GlobalSymbol(const std::string& name, GlobalSymbolType type, 
                 const std::string& namespace_ = "", 
                 const std::string& filePath = "", 
                 size_t line = 0, size_t column = 0)
        : name(name), type(type), namespace_(namespace_), 
          filePath(filePath), line(line), column(column) {}
};

// CHTL全局映射表
class CHTLGlobalMap {
public:
    CHTLGlobalMap();
    ~CHTLGlobalMap() = default;
    
    // 符号管理
    void addSymbol(const GlobalSymbol& symbol);
    std::shared_ptr<GlobalSymbol> findSymbol(const std::string& name, 
                                            GlobalSymbolType type = GlobalSymbolType::TEMPLATE_STYLE,
                                            const std::string& namespace_ = "");
    bool hasSymbol(const std::string& name, 
                  GlobalSymbolType type = GlobalSymbolType::TEMPLATE_STYLE,
                  const std::string& namespace_ = "");
    bool removeSymbol(const std::string& name, 
                     GlobalSymbolType type = GlobalSymbolType::TEMPLATE_STYLE,
                     const std::string& namespace_ = "");
    
    // 命名空间管理
    bool createNamespace(const std::string& name, const std::string& sourceFile = "");
    bool createNestedNamespace(const std::string& parent, const std::string& child, const std::string& sourceFile = "");
    bool addNamespaceItem(const std::string& namespaceName, std::shared_ptr<NamespaceItem> item);
    std::shared_ptr<NamespaceItem> getNamespaceItem(const std::string& namespaceName, const std::string& itemName);
    std::vector<std::shared_ptr<NamespaceItem>> getNamespaceItems(const std::string& namespaceName);
    std::shared_ptr<Namespace> getNamespace(const std::string& name);
    bool mergeNamespaces(const std::string& name);
    std::vector<NamespaceConflict> detectConflicts();
    
    // 配置组管理
    bool addConfigurationGroup(const std::string& name, const ConfigurationGroup& config);
    std::shared_ptr<ConfigurationGroup> getConfigurationGroup(const std::string& name);
    bool updateConfigurationItem(const std::string& groupName, const std::string& key, const std::string& value);
    
    // 导入管理
    void addImport(const ImportInfo& import);
    std::vector<ImportInfo> getImports() const;
    bool checkCircularDependency(const std::string& filePath);
    bool checkDuplicateImport(const std::string& type, const std::string& path);
    
    // 查询功能
    std::vector<std::shared_ptr<GlobalSymbol>> findSymbolsInNamespace(const std::string& namespace_);
    std::vector<std::string> resolveWildcardPath(const std::string& path);
    std::vector<std::string> resolveSubModulePath(const std::string& path);
    
    // 清理功能
    void clear();
    void clearNamespace(const std::string& name);
    
    // 获取信息
    const std::vector<std::shared_ptr<GlobalSymbol>>& getAllSymbols() const { return symbols_; }
    size_t getSymbolCount() const { return symbols_.size(); }
    size_t getNamespaceCount() const { return namespaces_.size(); }
    size_t getConfigurationGroupCount() const { return configurationGroups_.size(); }
    
private:
    std::vector<std::shared_ptr<GlobalSymbol>> symbols_;
    std::unordered_map<std::string, std::shared_ptr<Namespace>> namespaces_;
    std::unordered_map<std::string, ConfigurationGroup> configurationGroups_;
    std::vector<ImportInfo> imports_;
    
    // 创建符号的辅助函数
    std::shared_ptr<GlobalSymbol> createSymbol(const GlobalSymbol& symbol);
    
    // 命名空间路径解析
    std::vector<std::string> parseNamespacePath(const std::string& path);
    std::string normalizeNamespacePath(const std::string& path);
    
    // 冲突检测辅助函数
    bool hasConflict(const std::string& itemName, NamespaceItemType type, const std::string& namespaceName);
};

} // namespace CHTL