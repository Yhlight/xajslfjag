#pragma once

#include "../../Util/Common.h"
#include "../CHTLState/State.h"

namespace CHTL {

// 上下文类型
enum class ContextType {
    GLOBAL,              // 全局上下文
    ELEMENT,             // 元素上下文
    TEMPLATE,            // 模板上下文
    CUSTOM,              // 自定义上下文
    STYLE,               // 样式上下文
    SCRIPT,              // 脚本上下文
    IMPORT,              // 导入上下文
    NAMESPACE,           // 命名空间上下文
    CONFIGURATION,       // 配置上下文
    ORIGIN,              // 原始嵌入上下文
    VARIABLE_SCOPE,      // 变量作用域上下文
    LOCAL_SCOPE          // 局部作用域上下文
};

// 作用域信息
struct Scope {
    String name;
    ContextType type;
    StringUnorderedMap variables;
    StringUnorderedMap templates;
    StringUnorderedMap customs;
    StringUnorderedMap origins;
    StringVector imports;
    size_t depth;
    
    Scope(const String& n, ContextType t, size_t d = 0) 
        : name(n), type(t), depth(d) {}
};

// 变量信息
struct Variable {
    String name;
    String value;
    String type;  // 变量类型：style, element, var
    bool isTemplate;
    bool isCustom;
    StringUnorderedMap properties;
    
    Variable(const String& n, const String& v, const String& t = "var")
        : name(n), value(v), type(t), isTemplate(false), isCustom(false) {}
};

// 上下文管理器
class Context {
public:
    Context();
    ~Context() = default;
    
    // 作用域管理
    void pushScope(const String& name, ContextType type);
    void popScope();
    Scope* getCurrentScope();
    const Scope* getCurrentScope() const;
    size_t getScopeDepth() const { return scopes.size(); }
    
    // 变量管理
    void defineVariable(const String& name, const Variable& var);
    Variable* lookupVariable(const String& name);
    const Variable* lookupVariable(const String& name) const;
    bool hasVariable(const String& name) const;
    void removeVariable(const String& name);
    
    // 模板管理
    void defineTemplate(const String& name, const String& type, const String& content);
    String lookupTemplate(const String& name, const String& type = "") const;
    bool hasTemplate(const String& name, const String& type = "") const;
    void removeTemplate(const String& name, const String& type = "");
    StringVector getTemplateNames(const String& type = "") const;
    
    // 自定义管理
    void defineCustom(const String& name, const String& type, const String& content);
    String lookupCustom(const String& name, const String& type = "") const;
    bool hasCustom(const String& name, const String& type = "") const;
    void removeCustom(const String& name, const String& type = "");
    StringVector getCustomNames(const String& type = "") const;
    
    // 原始嵌入管理
    void defineOrigin(const String& name, const String& type, const String& content);
    String lookupOrigin(const String& name, const String& type = "") const;
    bool hasOrigin(const String& name, const String& type = "") const;
    void removeOrigin(const String& name, const String& type = "");
    
    // 导入管理
    void addImport(const String& importPath);
    bool hasImport(const String& importPath) const;
    StringVector getImports() const;
    void clearImports();
    
    // 命名空间管理
    void enterNamespace(const String& name);
    void exitNamespace();
    String getCurrentNamespace() const;
    String getFullyQualifiedName(const String& name) const;
    StringVector getNamespaceStack() const { return namespaceStack; }
    
    // 配置管理
    void setConfigValue(const String& key, const String& value);
    String getConfigValue(const String& key, const String& defaultValue = "") const;
    bool hasConfigValue(const String& key) const;
    StringUnorderedMap getAllConfig() const { return config; }
    
    // 选择器自动化配置
    bool isStyleAutoAddClassDisabled() const;
    bool isStyleAutoAddIdDisabled() const;
    bool isScriptAutoAddClassDisabled() const;
    bool isScriptAutoAddIdDisabled() const;
    bool isDefaultNamespaceDisabled() const;
    
    // 上下文查询
    ContextType getCurrentContextType() const;
    bool isInTemplateContext() const;
    bool isInCustomContext() const;
    bool isInStyleContext() const;
    bool isInScriptContext() const;
    bool isInNamespaceContext() const;
    bool isInGlobalScope() const { return scopes.size() <= 1; }
    
    // 继承支持
    void setInheritanceTarget(const String& target, const String& type);
    String getInheritanceTarget(const String& type = "") const;
    bool hasInheritanceTarget(const String& type = "") const;
    void clearInheritanceTarget(const String& type = "");
    
    // 删除操作支持
    void markForDeletion(const String& target, const String& type);
    bool isMarkedForDeletion(const String& target, const String& type = "") const;
    StringVector getDeletionTargets(const String& type = "") const;
    void clearDeletionMarks();
    
    // 插入操作支持
    struct InsertionPoint {
        String position;  // after, before, replace, at top, at bottom
        String target;
        String content;
        String type;
    };
    
    void addInsertionPoint(const InsertionPoint& point);
    std::vector<InsertionPoint> getInsertionPoints(const String& type = "") const;
    void clearInsertionPoints();
    
    // 类和ID自动生成
    String generateClassName(const String& prefix = "auto");
    String generateIdName(const String& prefix = "auto");
    void addGeneratedClass(const String& className);
    void addGeneratedId(const String& idName);
    StringVector getGeneratedClasses() const { return generatedClasses; }
    StringVector getGeneratedIds() const { return generatedIds; }
    
    // 错误处理
    void reportError(const String& message);
    void reportWarning(const String& message);
    StringVector getErrors() const { return errors; }
    StringVector getWarnings() const { return warnings; }
    bool hasErrors() const { return !errors.empty(); }
    bool hasWarnings() const { return !warnings.empty(); }
    void clearDiagnostics();
    
    // 调试支持
    void dumpContext() const;
    String getContextInfo() const;
    
    // 深拷贝支持
    Context* clone() const;
    void merge(const Context& other);
    
private:
    std::vector<Scope> scopes;
    StringVector namespaceStack;
    StringUnorderedMap config;
    StringUnorderedMap inheritanceTargets;
    StringUnorderedMap deletionMarks;
    std::vector<InsertionPoint> insertionPoints;
    StringVector generatedClasses;
    StringVector generatedIds;
    StringVector errors;
    StringVector warnings;
    size_t classCounter;
    size_t idCounter;
    
    // 内部查找方法
    Variable* findVariableInScopes(const String& name);
    const Variable* findVariableInScopes(const String& name) const;
    String findTemplateInScopes(const String& name, const String& type) const;
    String findCustomInScopes(const String& name, const String& type) const;
    String findOriginInScopes(const String& name, const String& type) const;
    
    // 工具方法
    String makeKey(const String& name, const String& type) const;
    void initializeDefaultConfig();
};

// 上下文守卫（RAII）
class ContextGuard {
public:
    explicit ContextGuard(Context& ctx, const String& scopeName, ContextType type);
    ~ContextGuard();
    
    // 禁止复制和移动
    ContextGuard(const ContextGuard&) = delete;
    ContextGuard& operator=(const ContextGuard&) = delete;
    ContextGuard(ContextGuard&&) = delete;
    ContextGuard& operator=(ContextGuard&&) = delete;
    
    // 手动弹出作用域
    void release();
    
private:
    Context& context;
    bool shouldPop;
};

// 命名空间守卫（RAII）
class NamespaceGuard {
public:
    explicit NamespaceGuard(Context& ctx, const String& namespaceName);
    ~NamespaceGuard();
    
    // 禁止复制和移动
    NamespaceGuard(const NamespaceGuard&) = delete;
    NamespaceGuard& operator=(const NamespaceGuard&) = delete;
    NamespaceGuard(NamespaceGuard&&) = delete;
    NamespaceGuard& operator=(NamespaceGuard&&) = delete;
    
    // 手动退出命名空间
    void release();
    
private:
    Context& context;
    bool shouldExit;
};

// 上下文工厂
class ContextFactory {
public:
    static std::unique_ptr<Context> createGlobalContext();
    static std::unique_ptr<Context> createTemplateContext(const String& templateName);
    static std::unique_ptr<Context> createCustomContext(const String& customName);
    static std::unique_ptr<Context> createStyleContext();
    static std::unique_ptr<Context> createScriptContext();
    static std::unique_ptr<Context> createImportContext();
    static std::unique_ptr<Context> createNamespaceContext(const String& namespaceName);
};

} // namespace CHTL