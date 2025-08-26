#ifndef CHTL_GLOBAL_MAP_H
#define CHTL_GLOBAL_MAP_H

#include <string>
#include <unordered_map>
#include <memory>
#include <vector>
#include <optional>

namespace chtl {

// 前向声明
class ASTNode;
class TemplateNode;
class CustomNode;
class NamespaceNode;
class ConfigurationNode;

// 符号类型
enum class SymbolType {
    TEMPLATE_STYLE,      // 模板样式组
    TEMPLATE_ELEMENT,    // 模板元素
    TEMPLATE_VAR,        // 模板变量组
    CUSTOM_STYLE,        // 自定义样式组
    CUSTOM_ELEMENT,      // 自定义元素
    CUSTOM_VAR,          // 自定义变量组
    ORIGIN_BLOCK,        // 原始嵌入块
    NAMESPACE,           // 命名空间
    CONFIGURATION,       // 配置组
    IMPORTED_SYMBOL,     // 导入的符号
    CLASS_NAME,          // 自动添加的类名
    ID_NAME              // 自动添加的ID
};

// 符号信息
struct SymbolInfo {
    SymbolType type;
    std::string name;
    std::string fullName;           // 包含命名空间的完整名称
    std::shared_ptr<ASTNode> node;  // 关联的AST节点
    std::string sourceFile;         // 定义所在的源文件
    size_t line;
    size_t column;
    bool isImported = false;        // 是否是导入的符号
    std::string importedFrom;       // 导入来源
    std::string importedAs;         // 导入时的别名
    
    SymbolInfo() : line(0), column(0) {}
};

// 命名空间信息
struct NamespaceInfo {
    std::string name;
    std::string parentNamespace;
    std::unordered_map<std::string, SymbolInfo> symbols;
    std::vector<std::string> childNamespaces;
    bool isDefault = false;  // 是否是默认命名空间（基于文件名）
};

// 导入信息
struct ImportInfo {
    std::string path;
    std::string type;        // @Chtl, @CJmod, @Html等
    std::string symbolName;  // 具体导入的符号名（如果有）
    std::string asName;      // as别名（如果有）
    bool isWildcard = false; // 是否是通配符导入 (*)
    std::string resolvedPath; // 解析后的实际路径
};

// 全局符号表
class CHTLGlobalMap {
public:
    CHTLGlobalMap();
    ~CHTLGlobalMap();
    
    // 符号注册
    bool registerSymbol(const SymbolInfo& symbol);
    bool registerSymbol(const std::string& namespace_, const SymbolInfo& symbol);
    
    // 符号查找
    std::optional<SymbolInfo> lookupSymbol(const std::string& name) const;
    std::optional<SymbolInfo> lookupSymbol(const std::string& namespace_, const std::string& name) const;
    std::vector<SymbolInfo> lookupSymbolsByType(SymbolType type) const;
    
    // 命名空间管理
    bool createNamespace(const std::string& name, const std::string& parent = "");
    bool mergeNamespace(const std::string& name, const NamespaceInfo& info);
    std::optional<NamespaceInfo> getNamespace(const std::string& name) const;
    std::vector<std::string> getAllNamespaces() const;
    void setCurrentNamespace(const std::string& namespace_);
    std::string getCurrentNamespace() const;
    
    // 导入管理
    bool registerImport(const ImportInfo& import);
    std::vector<ImportInfo> getImports() const;
    bool hasCircularDependency(const std::string& path) const;
    void markFileAsProcessed(const std::string& path);
    
    // 配置管理
    bool registerConfiguration(const std::string& name, std::shared_ptr<ConfigurationNode> config);
    std::shared_ptr<ConfigurationNode> getConfiguration(const std::string& name = "") const;
    std::shared_ptr<ConfigurationNode> getActiveConfiguration() const;
    void setActiveConfiguration(const std::string& name);
    
    // 自动化类名/ID管理
    std::string generateUniqueClassName(const std::string& base = "auto");
    std::string generateUniqueId(const std::string& base = "auto");
    bool registerAutoClass(const std::string& className, const std::string& elementPath);
    bool registerAutoId(const std::string& id, const std::string& elementPath);
    std::optional<std::string> getAutoClassForElement(const std::string& elementPath) const;
    std::optional<std::string> getAutoIdForElement(const std::string& elementPath) const;
    
    // 符号冲突检测
    bool hasConflict(const std::string& name, SymbolType type) const;
    std::vector<std::string> detectConflicts() const;
    
    // 作用域管理
    void enterScope(const std::string& scopeName);
    void exitScope();
    std::string getCurrentScope() const;
    
    // 清理和重置
    void clear();
    void clearNamespace(const std::string& namespace_);
    
    // 调试和诊断
    void dumpSymbolTable() const;
    std::string generateSymbolReport() const;
    
private:
    // 内部辅助方法
    std::string buildFullName(const std::string& namespace_, const std::string& name) const;
    bool isValidNamespace(const std::string& namespace_) const;
    std::string resolveSymbolPath(const std::string& name) const;
    
private:
    // 全局符号表
    std::unordered_map<std::string, SymbolInfo> globalSymbols;
    
    // 命名空间表
    std::unordered_map<std::string, NamespaceInfo> namespaces;
    std::string currentNamespace;
    
    // 导入表
    std::vector<ImportInfo> imports;
    std::unordered_map<std::string, bool> processedFiles;
    
    // 配置表
    std::unordered_map<std::string, std::shared_ptr<ConfigurationNode>> configurations;
    std::string activeConfiguration;
    
    // 自动生成的类名和ID
    std::unordered_map<std::string, std::string> elementToAutoClass;
    std::unordered_map<std::string, std::string> elementToAutoId;
    std::unordered_map<std::string, std::string> autoClassToElement;
    std::unordered_map<std::string, std::string> autoIdToElement;
    size_t autoClassCounter;
    size_t autoIdCounter;
    
    // 作用域栈
    std::vector<std::string> scopeStack;
};

// 全局符号表单例访问
class GlobalMapManager {
public:
    static CHTLGlobalMap& getInstance();
    static void reset();
    
private:
    GlobalMapManager() = default;
    static std::unique_ptr<CHTLGlobalMap> instance;
};

} // namespace chtl

#endif // CHTL_GLOBAL_MAP_H