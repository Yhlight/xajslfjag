#ifndef CHTLJS_CONTEXT_H
#define CHTLJS_CONTEXT_H

#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <stack>
#include "../CHTLJSNode/BaseNode.h"
#include "../CHTLJSState/CHTLJSState.h"

namespace CHTLJS {

// 作用域类型
enum class ScopeType {
    GLOBAL,         // 全局作用域
    FUNCTION,       // 函数作用域
    BLOCK,          // 块作用域
    OBJECT,         // 对象作用域
    CHTLJS_FUNCTION // CHTL JS函数作用域
};

// 符号类型
enum class SymbolType {
    VARIABLE,       // 变量
    FUNCTION,       // 函数
    PARAMETER,      // 参数
    SELECTOR,       // 选择器缓存
    VIRTUAL_OBJECT, // 虚对象
    CHTLJS_FUNCTION,// CHTL JS函数
    ANIMATION       // 动画对象
};

// 符号信息
struct Symbol {
    std::string name;
    SymbolType type;
    std::shared_ptr<BaseNode> node;
    ScopeType scope;
    bool isConst;
    bool isVirtual;
    std::string selectorTarget;  // 对于选择器，存储目标
    
    Symbol() : type(SymbolType::VARIABLE), scope(ScopeType::GLOBAL), 
              isConst(false), isVirtual(false) {}
    
    Symbol(const std::string& n, SymbolType t, std::shared_ptr<BaseNode> nd)
        : name(n), type(t), node(nd), scope(ScopeType::GLOBAL), 
          isConst(false), isVirtual(false) {}
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
    class CHTLJSContext* context;
    
public:
    ScopeGuard(CHTLJSContext* ctx, ScopeType type, const std::string& name);
    ~ScopeGuard();
    
    // 禁止拷贝
    ScopeGuard(const ScopeGuard&) = delete;
    ScopeGuard& operator=(const ScopeGuard&) = delete;
    
    // 允许移动
    ScopeGuard(ScopeGuard&& other) noexcept;
    ScopeGuard& operator=(ScopeGuard&& other) noexcept;
};

// CHTL JS上下文管理器
class CHTLJSContext {
private:
    std::shared_ptr<Scope> globalScope;
    std::shared_ptr<Scope> currentScope;
    std::stack<std::shared_ptr<Scope>> scopeStack;
    
    // 状态机引用
    std::unique_ptr<CHTLJSState> state;
    
    // 选择器缓存
    std::unordered_map<std::string, std::string> selectorCache;  // 选择器 -> 生成的变量名
    
    // 虚对象管理
    std::unordered_map<std::string, std::vector<std::string>> virtualObjectMethods;
    
    // CHTL JS函数注册表
    std::unordered_set<std::string> registeredCHTLJSFunctions;
    
    // 动画管理
    std::unordered_map<std::string, std::shared_ptr<BaseNode>> animations;
    
    // 事件委托管理
    struct DelegationInfo {
        std::string parent;
        std::vector<std::string> targets;
        std::unordered_map<std::string, std::string> handlers;
    };
    std::vector<DelegationInfo> delegations;
    
    // 错误和警告
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
public:
    CHTLJSContext();
    ~CHTLJSContext() = default;
    
    // 状态机访问
    CHTLJSState* getState() { return state.get(); }
    const CHTLJSState* getState() const { return state.get(); }
    
    // 作用域管理
    void enterScope(ScopeType type, const std::string& name);
    void exitScope();
    [[nodiscard]] ScopeGuard createScope(ScopeType type, const std::string& name);
    std::shared_ptr<Scope> getCurrentScope() { return currentScope; }
    std::shared_ptr<Scope> getGlobalScope() { return globalScope; }
    
    // 符号表管理
    bool registerSymbol(const std::string& name, SymbolType type, 
                       std::shared_ptr<BaseNode> node, bool isConst = false);
    Symbol* lookupSymbol(const std::string& name);
    bool registerVirtualObject(const std::string& name, const std::vector<std::string>& methods);
    bool isVirtualObject(const std::string& name) const;
    
    // 选择器管理
    std::string registerSelector(const std::string& selector);
    bool hasSelector(const std::string& selector) const;
    std::string getSelectorVariable(const std::string& selector) const;
    
    // CHTL JS函数管理
    void registerCHTLJSFunction(const std::string& name);
    bool isCHTLJSFunction(const std::string& name) const;
    void bindVirtualObject(const std::string& functionName);
    
    // 动画管理
    void registerAnimation(const std::string& name, std::shared_ptr<BaseNode> animNode);
    std::shared_ptr<BaseNode> getAnimation(const std::string& name) const;
    
    // 事件委托管理
    void addDelegation(const DelegationInfo& info);
    const std::vector<DelegationInfo>& getDelegations() const { return delegations; }
    
    // 验证
    bool validateContext() const;
    bool validateArrowUsage(const std::string& object) const;
    bool validateSelectorUsage(const std::string& selector) const;
    bool validateCHTLJSFunctionCall(const std::string& function) const;
    
    // 代码生成辅助
    std::string generateSelectorCode(const std::string& selector) const;
    std::string generateArrowAccessCode(const std::string& object, const std::string& property) const;
    std::string generateEventBindingCode(const std::string& selector, const std::string& event, 
                                        const std::string& handler) const;
    
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

} // namespace CHTLJS

#endif // CHTLJS_CONTEXT_H