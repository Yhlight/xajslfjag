#ifndef CHTL_CONTEXT_H
#define CHTL_CONTEXT_H

#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <stack>
#include "../CHTLNode/BaseNode.h"
#include "../CHTLNode/ConfigNode.h"
#include "../CHTLNode/NamespaceNode.h"
#include "../CHTLState/CHTLState.h"

namespace CHTL {

// 作用域类型
enum class ScopeType {
    GLOBAL,         // 全局作用域
    NAMESPACE,      // 命名空间作用域
    ELEMENT,        // 元素作用域
    TEMPLATE,       // 模板作用域
    CUSTOM,         // 自定义作用域
    LOCAL_STYLE,    // 局部样式作用域
    LOCAL_SCRIPT    // 局部脚本作用域
};

// 符号类型
enum class SymbolType {
    TEMPLATE_STYLE,
    TEMPLATE_ELEMENT,
    TEMPLATE_VAR,
    CUSTOM_STYLE,
    CUSTOM_ELEMENT,
    CUSTOM_VAR,
    ORIGIN_BLOCK,
    CONFIG_VALUE,
    NAMESPACE_NAME,
    ELEMENT_ID,
    ELEMENT_CLASS
};

// 符号信息
struct Symbol {
    std::string name;
    SymbolType type;
    std::shared_ptr<BaseNode> node;
    ScopeType scope;
    std::string namespacePath;  // 命名空间路径（如 "space.room2"）
    
    Symbol() : type(SymbolType::TEMPLATE_STYLE), scope(ScopeType::GLOBAL) {}
    
    Symbol(const std::string& n, SymbolType t, std::shared_ptr<BaseNode> nd)
        : name(n), type(t), node(nd), scope(ScopeType::GLOBAL) {}
};

// 作用域
class Scope {
private:
    ScopeType type;
    std::string name;
    std::unordered_map<std::string, Symbol> symbols;
    std::shared_ptr<Scope> parent;
    std::vector<std::shared_ptr<Scope>> children;
    
public:
    Scope(ScopeType t, const std::string& n, std::shared_ptr<Scope> p = nullptr)
        : type(t), name(n), parent(p) {}
    
    // 符号管理
    bool addSymbol(const Symbol& symbol);
    Symbol* findSymbol(const std::string& name);
    Symbol* findSymbolRecursive(const std::string& name);
    bool hasSymbol(const std::string& name) const;
    
    // 作用域信息
    ScopeType getType() const { return type; }
    const std::string& getName() const { return name; }
    std::shared_ptr<Scope> getParent() { return parent; }
    
    // 子作用域管理
    void addChild(std::shared_ptr<Scope> child);
    std::shared_ptr<Scope> findChild(const std::string& name);
};

// RAII作用域守卫
class ScopeGuard {
private:
    class CHTLContext* context;
    
public:
    ScopeGuard(CHTLContext* ctx, ScopeType type, const std::string& name);
    ~ScopeGuard();
    
    // 禁止拷贝
    ScopeGuard(const ScopeGuard&) = delete;
    ScopeGuard& operator=(const ScopeGuard&) = delete;
    
    // 允许移动
    ScopeGuard(ScopeGuard&& other) noexcept;
    ScopeGuard& operator=(ScopeGuard&& other) noexcept;
};

// CHTL上下文管理器
class CHTLContext {
private:
    std::shared_ptr<Scope> globalScope;
    std::shared_ptr<Scope> currentScope;
    std::stack<std::shared_ptr<Scope>> scopeStack;
    
    // 状态机引用
    std::unique_ptr<CHTLState> state;
    
    // 配置信息
    std::shared_ptr<ConfigNode> activeConfig;
    std::unordered_map<std::string, std::shared_ptr<ConfigNode>> namedConfigs;
    
    // 命名空间管理
    std::unordered_map<std::string, std::shared_ptr<NamespaceNode>> namespaces;
    std::string currentNamespace;
    
    // 导入管理
    std::unordered_map<std::string, std::string> importedFiles;
    std::unordered_set<std::string> importedModules;
    
    // 约束管理
    struct Constraint {
        std::vector<std::string> exceptList;
        std::shared_ptr<BaseNode> scope;
    };
    std::vector<Constraint> constraints;
    
    // 错误和警告
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
public:
    CHTLContext();
    ~CHTLContext() = default;
    
    // 状态机访问
    CHTLState* getState() { return state.get(); }
    const CHTLState* getState() const { return state.get(); }
    
    // 作用域管理
    void enterScope(ScopeType type, const std::string& name);
    void exitScope();
    [[nodiscard]] ScopeGuard createScope(ScopeType type, const std::string& name);
    std::shared_ptr<Scope> getCurrentScope() { return currentScope; }
    std::shared_ptr<Scope> getGlobalScope() { return globalScope; }
    
    // 符号表管理
    bool registerSymbol(const std::string& name, SymbolType type, std::shared_ptr<BaseNode> node);
    Symbol* lookupSymbol(const std::string& name);
    Symbol* lookupSymbolInNamespace(const std::string& name, const std::string& ns);
    
    // 配置管理
    void setActiveConfig(std::shared_ptr<ConfigNode> config);
    std::shared_ptr<ConfigNode> getActiveConfig() { return activeConfig; }
    void registerNamedConfig(const std::string& name, std::shared_ptr<ConfigNode> config);
    std::shared_ptr<ConfigNode> getNamedConfig(const std::string& name);
    
    // 命名空间管理
    void enterNamespace(const std::string& name);
    void exitNamespace();
    std::string getCurrentNamespace() const { return currentNamespace; }
    void registerNamespace(std::shared_ptr<NamespaceNode> ns);
    std::shared_ptr<NamespaceNode> getNamespace(const std::string& path);
    
    // 导入管理
    bool registerImport(const std::string& path, const std::string& resolvedPath);
    bool isImported(const std::string& path) const;
    std::string getResolvedImport(const std::string& path) const;
    void addImportedModule(const std::string& moduleName);
    bool isModuleImported(const std::string& moduleName) const;
    
    // 约束管理
    void addConstraint(const std::vector<std::string>& exceptList, std::shared_ptr<BaseNode> scope);
    bool isAllowed(const std::string& item) const;
    bool isAllowedInScope(const std::string& item, std::shared_ptr<BaseNode> scope) const;
    
    // 验证
    bool validateContext() const;
    bool validateSymbolUsage(const std::string& symbol, StateType currentState) const;
    bool validateStyleBlockContent(const std::vector<std::string>& usedFeatures, bool isLocal) const;
    bool validateScriptBlockContent(const std::vector<std::string>& usedFeatures, bool isLocal) const;
    
    // 错误和警告
    void addError(const std::string& error);
    void addWarning(const std::string& warning);
    const std::vector<std::string>& getErrors() const { return errors; }
    const std::vector<std::string>& getWarnings() const { return warnings; }
    void clearErrors() { errors.clear(); }
    void clearWarnings() { warnings.clear(); }
    
    // 重置
    void reset();
    
    friend class ScopeGuard;
};

} // namespace CHTL

#endif // CHTL_CONTEXT_H