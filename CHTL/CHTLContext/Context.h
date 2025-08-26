#ifndef CHTL_CONTEXT_H
#define CHTL_CONTEXT_H

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <stack>
#include <memory>

namespace CHTL {

// 前向声明
class GlobalMap;
class StateManager;

// 上下文类型枚举
enum class ContextType {
    GLOBAL,                 // 全局上下文
    NAMESPACE,              // 命名空间上下文
    TEMPLATE,               // 模板上下文
    CUSTOM,                 // 自定义上下文
    ELEMENT,                // 元素上下文
    STYLE,                  // 样式上下文
    SCRIPT,                 // 脚本上下文
    CONFIGURATION,          // 配置上下文
    IMPORT,                 // 导入上下文
    CONSTRAINT              // 约束上下文
};

// 作用域类型
enum class ScopeType {
    GLOBAL_SCOPE,           // 全局作用域
    NAMESPACE_SCOPE,        // 命名空间作用域
    TEMPLATE_SCOPE,         // 模板作用域
    CUSTOM_SCOPE,           // 自定义作用域
    ELEMENT_SCOPE,          // 元素作用域
    LOCAL_SCOPE             // 局部作用域
};

// 上下文信息结构
struct ContextInfo {
    ContextType type;
    ScopeType scope;
    std::string name;
    std::string namespace_name;
    std::unordered_map<std::string, std::string> properties;
    std::unordered_set<std::string> constraints;
    size_t depth;
    
    ContextInfo(ContextType t = ContextType::GLOBAL, 
                ScopeType s = ScopeType::GLOBAL_SCOPE,
                const std::string& n = "",
                const std::string& ns = "")
        : type(t), scope(s), name(n), namespace_name(ns), depth(0) {}
};

// 上下文管理器类 - RAII管理
class ContextManager {
private:
    std::stack<ContextInfo> context_stack;
    GlobalMap* global_map;
    StateManager* state_manager;
    
    // 当前上下文缓存
    ContextInfo current_context;
    
    // 约束管理
    std::unordered_map<std::string, std::unordered_set<std::string>> global_constraints;
    std::unordered_map<std::string, std::unordered_set<std::string>> local_constraints;
    
    // 引用管理
    std::unordered_map<std::string, std::string> variable_references;
    std::unordered_map<std::string, std::string> template_references;
    std::unordered_map<std::string, std::string> custom_references;
    
    // 选择器自动化状态
    struct SelectorState {
        bool auto_add_class = true;
        bool auto_add_id = true;
        std::vector<std::string> classes;
        std::vector<std::string> ids;
        std::string current_class;
        std::string current_id;
    } selector_state;
    
    // 错误处理
    std::vector<std::string> errors;
    
public:
    ContextManager(GlobalMap* globalMap = nullptr, StateManager* stateManager = nullptr);
    ~ContextManager() = default;
    
    // 上下文管理
    void pushContext(ContextType type, const std::string& name = "", ScopeType scope = ScopeType::LOCAL_SCOPE);
    bool popContext();
    const ContextInfo& getCurrentContext() const;
    const ContextInfo& getPreviousContext() const;
    
    // 上下文查询
    bool isInContext(ContextType type) const;
    bool isInScope(ScopeType scope) const;
    bool isInNamespace(const std::string& namespace_name) const;
    
    // 命名空间管理
    void enterNamespace(const std::string& namespace_name);
    void exitNamespace();
    std::string getCurrentNamespace() const;
    std::vector<std::string> getNamespaceHierarchy() const;
    
    // 模板和自定义管理
    void enterTemplate(const std::string& template_name);
    void exitTemplate();
    void enterCustom(const std::string& custom_name);
    void exitCustom();
    
    std::string getCurrentTemplate() const;
    std::string getCurrentCustom() const;
    
    // 元素上下文管理
    void enterElement(const std::string& element_name);
    void exitElement();
    std::string getCurrentElement() const;
    
    // 样式和脚本上下文
    void enterStyle();
    void exitStyle();
    void enterScript();
    void exitScript();
    
    bool isInStyleContext() const;
    bool isInScriptContext() const;
    
    // 约束管理
    void addConstraint(const std::string& constraint_type, const std::string& target = "");
    void removeConstraint(const std::string& constraint_type, const std::string& target = "");
    bool hasConstraint(const std::string& constraint_type, const std::string& target = "") const;
    bool isAllowed(const std::string& operation, const std::string& target = "") const;
    
    // 引用管理
    void addVariableReference(const std::string& var_name, const std::string& resolved_name);
    void addTemplateReference(const std::string& template_name, const std::string& resolved_name);
    void addCustomReference(const std::string& custom_name, const std::string& resolved_name);
    
    std::string resolveVariable(const std::string& var_name) const;
    std::string resolveTemplate(const std::string& template_name) const;
    std::string resolveCustom(const std::string& custom_name) const;
    
    // 选择器自动化管理
    void setAutoAddClass(bool enable);
    void setAutoAddId(bool enable);
    void addAutoClass(const std::string& class_name);
    void addAutoId(const std::string& id_name);
    void setCurrentClass(const std::string& class_name);
    void setCurrentId(const std::string& id_name);
    
    bool isAutoAddClassEnabled() const;
    bool isAutoAddIdEnabled() const;
    std::string getCurrentClass() const;
    std::string getCurrentId() const;
    std::vector<std::string> getAutoClasses() const;
    std::vector<std::string> getAutoIds() const;
    
    // 属性管理
    void setContextProperty(const std::string& key, const std::string& value);
    std::string getContextProperty(const std::string& key) const;
    bool hasContextProperty(const std::string& key) const;
    void clearContextProperty(const std::string& key);
    
    // 深度管理
    size_t getContextDepth() const;
    size_t getContextDepth(ContextType type) const;
    
    // 错误处理
    void addError(const std::string& message);
    std::vector<std::string> getErrors() const;
    bool hasErrors() const;
    void clearErrors();
    
    // 验证
    bool validateContext() const;
    bool validateConstraints() const;
    bool validateReferences() const;
    
    // 调试支持
    void printContextStack() const;
    void printCurrentContext() const;
    void printConstraints() const;
    void printReferences() const;
    
    // 清理
    void reset();
    void clear();
    
    // 设置管理器引用
    void setGlobalMap(GlobalMap* globalMap);
    void setStateManager(StateManager* stateManager);
};

// RAII上下文保护类
class ContextGuard {
private:
    ContextManager* manager;
    ContextType saved_type;
    std::string saved_name;
    bool should_restore;
    
public:
    ContextGuard(ContextManager* mgr, ContextType type, const std::string& name = "", ScopeType scope = ScopeType::LOCAL_SCOPE);
    ~ContextGuard();
    
    // 禁用拷贝和赋值
    ContextGuard(const ContextGuard&) = delete;
    ContextGuard& operator=(const ContextGuard&) = delete;
    
    // 允许移动
    ContextGuard(ContextGuard&& other) noexcept;
    ContextGuard& operator=(ContextGuard&& other) noexcept;
    
    // 手动释放保护
    void release();
    
    // 检查是否有效
    bool isValid() const;
};

// 命名空间保护类
class NamespaceGuard {
private:
    ContextManager* manager;
    std::string saved_namespace;
    bool should_restore;
    
public:
    NamespaceGuard(ContextManager* mgr, const std::string& namespace_name);
    ~NamespaceGuard();
    
    // 禁用拷贝和赋值
    NamespaceGuard(const NamespaceGuard&) = delete;
    NamespaceGuard& operator=(const NamespaceGuard&) = delete;
    
    // 允许移动
    NamespaceGuard(NamespaceGuard&& other) noexcept;
    NamespaceGuard& operator=(NamespaceGuard&& other) noexcept;
    
    // 手动释放保护
    void release();
    
    // 检查是否有效
    bool isValid() const;
};

// 约束保护类
class ConstraintGuard {
private:
    ContextManager* manager;
    std::vector<std::pair<std::string, std::string>> saved_constraints;
    bool should_restore;
    
public:
    ConstraintGuard(ContextManager* mgr, const std::string& constraint_type, const std::string& target = "");
    ~ConstraintGuard();
    
    // 禁用拷贝和赋值
    ConstraintGuard(const ConstraintGuard&) = delete;
    ConstraintGuard& operator=(const ConstraintGuard&) = delete;
    
    // 允许移动
    ConstraintGuard(ConstraintGuard&& other) noexcept;
    ConstraintGuard& operator=(ConstraintGuard&& other) noexcept;
    
    // 手动释放保护
    void release();
    
    // 检查是否有效
    bool isValid() const;
};

// 便利宏定义
#define CONTEXT_GUARD(manager, type, name) \
    ContextGuard __context_guard(manager, type, name)

#define NAMESPACE_GUARD(manager, namespace_name) \
    NamespaceGuard __namespace_guard(manager, namespace_name)

#define CONSTRAINT_GUARD(manager, constraint_type, target) \
    ConstraintGuard __constraint_guard(manager, constraint_type, target)

} // namespace CHTL

#endif // CHTL_CONTEXT_H