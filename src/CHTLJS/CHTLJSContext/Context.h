#ifndef CHTLJS_CONTEXT_H
#define CHTLJS_CONTEXT_H

#include <string>
#include <memory>
#include <unordered_map>
#include <stack>
#include <vector>
#include "../CHTLJSState/State.h"
#include "../CHTLJSLexer/GlobalMap.h"

namespace CHTLJS {

// CHTL JS编译上下文
class CompileContext {
public:
    CompileContext(const std::string& sourceFile);
    ~CompileContext() = default;
    
    // 文件信息
    const std::string& getSourceFile() const { return sourceFile_; }
    void setSourceFile(const std::string& file) { sourceFile_ = file; }
    
    // 当前位置信息
    size_t getCurrentLine() const { return currentLine_; }
    size_t getCurrentColumn() const { return currentColumn_; }
    void setPosition(size_t line, size_t col) {
        currentLine_ = line;
        currentColumn_ = col;
    }
    
    // 虚对象管理
    void enterVirtualObject(const std::string& name);
    void exitVirtualObject();
    const std::string& getCurrentVirtualObject() const { return currentVirtualObject_; }
    bool isInVirtualObject() const { return !currentVirtualObject_.empty(); }
    
    // 模块上下文
    void enterModule(const std::string& modulePath);
    void exitModule();
    const std::string& getCurrentModule() const { return currentModule_; }
    void addModuleDependency(const std::string& dependency);
    
    // 选择器上下文
    struct EnhancedSelectorContext {
        std::string selector;
        std::string elementType;     // tag, class, id, compound, ref
        std::string jsCode;          // 生成的JS代码
        bool hasIndex = false;
        size_t index = 0;
    };
    
    void pushSelectorContext(const EnhancedSelectorContext& ctx);
    void popSelectorContext();
    EnhancedSelectorContext* getCurrentSelectorContext();
    
    // 动画上下文
    struct AnimationContext {
        std::string target;
        std::unordered_map<std::string, std::string> properties;
        std::vector<std::unordered_map<std::string, std::string>> whenStates;
    };
    
    void enterAnimation();
    void exitAnimation();
    AnimationContext& getCurrentAnimation() { return animationContext_; }
    
    // CHTL JS函数上下文（listen, delegate, animate等）
    void enterCHTLJSFunction(const std::string& functionName);
    void exitCHTLJSFunction();
    const std::string& getCurrentCHTLJSFunction() const { return currentCHTLJSFunction_; }
    
    // 事件委托上下文
    void registerEventDelegation(const std::string& parentSelector);
    void addDelegationTarget(const std::string& targetSelector, 
                            const std::string& event, 
                            const std::string& handler);
    
    // 错误和警告管理
    void addError(const std::string& message, size_t line = 0, size_t col = 0);
    void addWarning(const std::string& message, size_t line = 0, size_t col = 0);
    const std::vector<std::string>& getErrors() const { return errors_; }
    const std::vector<std::string>& getWarnings() const { return warnings_; }
    bool hasErrors() const { return !errors_.empty(); }
    
    // 局部符号表管理
    void defineLocalSymbol(const std::string& name, const std::string& type);
    bool hasLocalSymbol(const std::string& name) const;
    std::string getLocalSymbolType(const std::string& name) const;
    
    // 作用域管理
    void enterScope(const std::string& scopeName);
    void exitScope();
    const std::string& getCurrentScope() const;
    
    // 获取状态管理器
    StateManager& getStateManager() { return stateManager_; }
    const StateManager& getStateManager() const { return stateManager_; }
    
    // 配置选项
    struct Config {
        bool enableSelectorAutoComplete = true;
        bool enableVirtualObjectOptimization = true;
        bool enableEventDelegationMerging = true;
        bool strictMode = false;
    };
    
    Config& getConfig() { return config_; }
    const Config& getConfig() const { return config_; }

private:
    std::string sourceFile_;
    size_t currentLine_;
    size_t currentColumn_;
    
    // 虚对象栈
    std::string currentVirtualObject_;
    
    // 模块信息
    std::string currentModule_;
    std::vector<std::string> moduleDependencies_;
    
    // 选择器栈
    std::stack<EnhancedSelectorContext> selectorStack_;
    
    // 动画上下文
    AnimationContext animationContext_;
    bool inAnimation_ = false;
    
    // CHTL JS函数栈
    std::stack<std::string> chtljsFunctionStack_;
    std::string currentCHTLJSFunction_;
    
    // 作用域栈
    std::stack<std::string> scopeStack_;
    
    // 局部符号表
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> localSymbols_;
    
    // 错误和警告
    std::vector<std::string> errors_;
    std::vector<std::string> warnings_;
    
    // 状态管理器
    StateManager stateManager_;
    
    // 配置
    Config config_;
};

// 上下文管理器（单例）
class ContextManager {
public:
    static ContextManager& getInstance() {
        static ContextManager instance;
        return instance;
    }
    
    // 创建新的编译上下文
    std::shared_ptr<CompileContext> createContext(const std::string& sourceFile);
    
    // 获取当前活动上下文
    std::shared_ptr<CompileContext> getCurrentContext() const;
    
    // 设置当前活动上下文
    void setCurrentContext(std::shared_ptr<CompileContext> context);
    
    // 获取指定文件的上下文
    std::shared_ptr<CompileContext> getContext(const std::string& file) const;
    
    // 移除上下文
    void removeContext(const std::string& file);
    
    // 清空所有上下文
    void clearAll();

private:
    ContextManager() = default;
    ContextManager(const ContextManager&) = delete;
    ContextManager& operator=(const ContextManager&) = delete;
    
    // 文件到上下文的映射
    std::unordered_map<std::string, std::shared_ptr<CompileContext>> contexts_;
    
    // 当前活动上下文
    std::shared_ptr<CompileContext> currentContext_;
};

// RAII上下文切换守卫
class ContextGuard {
public:
    explicit ContextGuard(std::shared_ptr<CompileContext> newContext)
        : manager_(ContextManager::getInstance()),
          previousContext_(manager_.getCurrentContext()) {
        manager_.setCurrentContext(newContext);
    }
    
    ~ContextGuard() {
        manager_.setCurrentContext(previousContext_);
    }
    
    // 禁止拷贝
    ContextGuard(const ContextGuard&) = delete;
    ContextGuard& operator=(const ContextGuard&) = delete;

private:
    ContextManager& manager_;
    std::shared_ptr<CompileContext> previousContext_;
};

} // namespace CHTLJS

#endif // CHTLJS_CONTEXT_H