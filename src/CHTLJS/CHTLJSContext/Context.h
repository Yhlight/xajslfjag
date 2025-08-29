#pragma once

#include "../../Util/Common.h"
#include "../CHTLJSNode/CHTLJSNodes.h"
#include <memory>
#include <stack>
#include <map>
#include <vector>

namespace CHTLJS {

// CHTL JS上下文类型
enum class ContextType {
    GLOBAL,
    MODULE,
    FUNCTION,
    BLOCK,
    LISTENER,
    ANIMATION,
    DELEGATION,
    VIRTUAL_OBJECT,
    CJMOD_EXTENSION
};

// 作用域信息
struct ScopeInfo {
    ContextType type;
    String name;
    StringUnorderedMap variables;
    StringUnorderedMap functions;
    StringVector moduleImports;
    std::unique_ptr<BaseNode> associatedNode;
    
    ScopeInfo(ContextType t, const String& n);
};

// CHTL JS编译上下文
class Context {
public:
    Context();
    ~Context();
    
    // 作用域管理
    void pushScope(ContextType type, const String& name = "");
    void popScope();
    ScopeInfo* getCurrentScope();
    const ScopeInfo* getCurrentScope() const;
    size_t getScopeDepth() const;
    
    // 变量管理
    void declareVariable(const String& name, const String& type = "", const String& value = "");
    bool isVariableDeclared(const String& name) const;
    String getVariableType(const String& name) const;
    String getVariableValue(const String& name) const;
    void setVariableValue(const String& name, const String& value);
    
    // 函数管理
    void declareFunction(const String& name, const StringVector& parameters, const String& returnType = "");
    bool isFunctionDeclared(const String& name) const;
    StringVector getFunctionParameters(const String& name) const;
    String getFunctionReturnType(const String& name) const;
    
    // 模块管理
    void addModuleImport(const String& moduleName, const String& alias = "");
    bool isModuleImported(const String& moduleName) const;
    StringVector getImportedModules() const;
    String getModuleAlias(const String& moduleName) const;
    
    // CHTL JS特定功能
    void enterCHTLJSBlock(const String& selector);
    void exitCHTLJSBlock();
    bool isInCHTLJSBlock() const;
    String getCurrentSelector() const;
    
    // 选择器管理
    void addSelector(const String& selector);
    bool isSelectorUsed(const String& selector) const;
    StringVector getUsedSelectors() const;
    void validateSelector(const String& selector);
    
    // 事件管理
    void addEventListener(const String& event, const String& handler);
    bool isEventHandled(const String& event) const;
    StringVector getHandledEvents() const;
    String getEventHandler(const String& event) const;
    
    // 动画管理
    void addAnimation(const String& name, const StringUnorderedMap& properties);
    bool isAnimationDefined(const String& name) const;
    StringUnorderedMap getAnimationProperties(const String& name) const;
    
    // 虚拟对象管理
    void declareVirtualObject(const String& name, const StringUnorderedMap& properties);
    bool isVirtualObjectDeclared(const String& name) const;
    StringUnorderedMap getVirtualObjectProperties(const String& name) const;
    
    // CJMOD扩展支持
    void registerCJMODExtension(const String& keyword, const String& handler);
    bool isCJMODKeywordRegistered(const String& keyword) const;
    String getCJMODHandler(const String& keyword) const;
    StringVector getRegisteredCJMODKeywords() const;
    
    // 错误和警告管理
    void addError(const String& message, const Position& pos = Position());
    void addWarning(const String& message, const Position& pos = Position());
    bool hasErrors() const;
    bool hasWarnings() const;
    StringVector getErrors() const;
    StringVector getWarnings() const;
    void clearErrors();
    void clearWarnings();
    
    // 代码生成辅助
    void addGeneratedCode(const String& code);
    String getGeneratedCode() const;
    void clearGeneratedCode();
    
    // 依赖分析
    void addDependency(const String& dependency);
    StringVector getDependencies() const;
    bool hasDependency(const String& dependency) const;
    void resolveDependencies();
    
    // 优化提示
    void addOptimizationHint(const String& hint);
    StringVector getOptimizationHints() const;
    
    // 调试信息
    void setDebugMode(bool enabled);
    bool isDebugMode() const;
    void addDebugInfo(const String& info);
    StringVector getDebugInfo() const;
    
    // 上下文状态
    void save();
    void restore();
    void reset();
    
    // 作用域查找
    ScopeInfo* findScopeByName(const String& name);
    ScopeInfo* findScopeByType(ContextType type);
    std::vector<ScopeInfo*> getAllScopes();
    
    // 统计信息
    struct Statistics {
        size_t totalVariables;
        size_t totalFunctions;
        size_t totalModules;
        size_t totalSelectors;
        size_t totalEvents;
        size_t totalAnimations;
        size_t totalVirtualObjects;
        size_t totalCJMODExtensions;
        size_t totalErrors;
        size_t totalWarnings;
        
        Statistics();
    };
    
    Statistics getStatistics() const;
    String getContextReport() const;

private:
    std::stack<std::unique_ptr<ScopeInfo>> scopeStack_;
    
    // 全局状态
    StringUnorderedMap globalVariables_;
    StringUnorderedMap globalFunctions_;
    StringVector moduleImports_;
    StringVector usedSelectors_;
    StringUnorderedMap eventHandlers_;
    std::map<String, StringUnorderedMap> animations_;
    std::map<String, StringUnorderedMap> virtualObjects_;
    StringUnorderedMap cjmodExtensions_;
    
    // 编译状态
    StringVector errors_;
    StringVector warnings_;
    String generatedCode_;
    StringVector dependencies_;
    StringVector optimizationHints_;
    StringVector debugInfo_;
    bool debugMode_;
    
    // CHTL JS特定状态
    std::stack<String> selectorStack_;
    
    // 保存/恢复状态
    struct SavedState {
        std::stack<std::unique_ptr<ScopeInfo>> scopeStack;
        StringUnorderedMap globalVariables;
        StringUnorderedMap globalFunctions;
        StringVector moduleImports;
        StringVector usedSelectors;
        StringUnorderedMap eventHandlers;
        std::map<String, StringUnorderedMap> animations;
        std::map<String, StringUnorderedMap> virtualObjects;
        StringUnorderedMap cjmodExtensions;
        StringVector errors;
        StringVector warnings;
        String generatedCode;
        StringVector dependencies;
        StringVector optimizationHints;
        std::stack<String> selectorStack;
    };
    
    std::vector<SavedState> savedStates_;
    
    // 辅助方法
    String lookupVariable(const String& name) const;
    String lookupFunction(const String& name) const;
    bool isValidSelector(const String& selector) const;
    bool isValidEventName(const String& event) const;
    void validateVariableName(const String& name) const;
    void validateFunctionName(const String& name) const;
};

// 上下文管理器
class ContextManager {
public:
    static ContextManager& getInstance();
    
    // 上下文创建和销毁
    String createContext(const String& name = "");
    void destroyContext(const String& contextId);
    Context* getContext(const String& contextId);
    Context* getCurrentContext();
    void setCurrentContext(const String& contextId);
    
    // 上下文切换
    void pushContext(const String& contextId);
    void popContext();
    
    // 全局上下文管理
    void setGlobalContext(const String& contextId);
    Context* getGlobalContext();
    
    // 上下文查询
    StringVector getAllContextIds() const;
    size_t getContextCount() const;
    bool hasContext(const String& contextId) const;
    
    // 上下文同步
    void syncContexts();
    void mergeContexts(const String& sourceId, const String& targetId);
    
    // 清理
    void clearAllContexts();
    void clearUnusedContexts();

private:
    ContextManager() = default;
    ~ContextManager() = default;
    
    std::map<String, std::unique_ptr<Context>> contexts_;
    std::stack<String> contextStack_;
    String currentContextId_;
    String globalContextId_;
    
    String generateContextId();
    static size_t nextContextId_;
};

// 上下文守卫 (RAII)
class ContextGuard {
public:
    explicit ContextGuard(ContextType type, const String& name = "");
    explicit ContextGuard(const String& contextId);
    ~ContextGuard();
    
    Context* getContext();
    
private:
    Context* context_;
    bool shouldPopScope_;
    bool shouldPopContext_;
    String contextId_;
};

#define CHTLJS_CONTEXT_SCOPE(type, name) \
    CHTLJS::ContextGuard _ctx_guard(type, name)

#define CHTLJS_FUNCTION_SCOPE(name) \
    CHTLJS_CONTEXT_SCOPE(CHTLJS::ContextType::FUNCTION, name)

#define CHTLJS_MODULE_SCOPE(name) \
    CHTLJS_CONTEXT_SCOPE(CHTLJS::ContextType::MODULE, name)

#define CHTLJS_BLOCK_SCOPE() \
    CHTLJS_CONTEXT_SCOPE(CHTLJS::ContextType::BLOCK, "")

} // namespace CHTLJS