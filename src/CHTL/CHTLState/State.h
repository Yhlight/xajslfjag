#pragma once

#include "../../Util/Common.h"

namespace CHTL {

// CHTL编译状态枚举
enum class CHTLState {
    IDLE,                    // 空闲状态
    PARSING_ELEMENT,         // 解析HTML元素
    PARSING_TEXT,            // 解析text块
    PARSING_ATTRIBUTES,      // 解析属性
    PARSING_STYLE_BLOCK,     // 解析style块
    PARSING_SCRIPT_BLOCK,    // 解析script块
    PARSING_TEMPLATE,        // 解析模板
    PARSING_CUSTOM,          // 解析自定义
    PARSING_ORIGIN,          // 解析原始嵌入
    PARSING_IMPORT,          // 解析导入
    PARSING_CONFIG,          // 解析配置
    PARSING_NAMESPACE,       // 解析命名空间
    PARSING_SELECTOR,        // 解析CSS选择器
    PARSING_INHERITANCE,     // 解析继承
    PARSING_DELETION,        // 解析删除操作
    PARSING_INSERTION,       // 解析插入操作
    PARSING_COMMENT,         // 解析注释
    PARSING_STRING,          // 解析字符串
    PARSING_VARIABLE,        // 解析变量
    ERROR_STATE              // 错误状态
};

// 状态转换规则
struct StateTransition {
    CHTLState fromState;
    CHTLState toState;
    String triggerToken;
    std::function<bool(const String&)> condition;
    
    StateTransition(CHTLState from, CHTLState to, const String& trigger)
        : fromState(from), toState(to), triggerToken(trigger) {}
    
    StateTransition(CHTLState from, CHTLState to, const String& trigger, 
                   std::function<bool(const String&)> cond)
        : fromState(from), toState(to), triggerToken(trigger), condition(cond) {}
};

// 状态管理器
class StateManager {
public:
    StateManager();
    ~StateManager() = default;
    
    // 状态操作
    CHTLState getCurrentState() const { return currentState; }
    virtual void setState(CHTLState newState);
    bool canTransitionTo(CHTLState targetState, const String& token = "") const;
    bool transitionTo(CHTLState targetState, const String& token = "");
    
    // 状态栈操作
    void pushState(CHTLState state);
    CHTLState popState();
    bool hasStateStack() const { return !stateStack.empty(); }
    size_t getStackDepth() const { return stateStack.size(); }
    
    // 状态历史
    StringVector getStateHistory() const;
    void clearHistory();
    
    // 状态验证
    bool isValidState(CHTLState state) const;
    bool isParsingState(CHTLState state) const;
    bool isBlockState(CHTLState state) const;
    bool isErrorState() const { return currentState == CHTLState::ERROR_STATE; }
    
    // 状态查询
    bool isInTemplateContext() const;
    bool isInCustomContext() const;
    bool isInStyleContext() const;
    bool isInScriptContext() const;
    bool isInImportContext() const;
    bool isInNamespaceContext() const;
    
    // 状态转换规则管理
    void addTransition(const StateTransition& transition);
    void removeTransition(CHTLState from, CHTLState to);
    std::vector<StateTransition> getAvailableTransitions() const;
    
    // 错误处理
    virtual void enterErrorState(const String& reason);
    String getErrorReason() const { return errorReason; }
    bool recoverFromError();
    
    // 调试支持
    String stateToString(CHTLState state) const;
    String getCurrentStateString() const;
    void dumpState() const;
    
private:
    CHTLState currentState;
    std::stack<CHTLState> stateStack;
    StringVector stateHistory;
    std::vector<StateTransition> transitions;
    String errorReason;
    
    void initializeTransitions();
    bool checkTransitionCondition(const StateTransition& transition, const String& token) const;
    void updateHistory(CHTLState oldState, CHTLState newState);
};

// RAII状态保护器
class StateGuard {
public:
    explicit StateGuard(StateManager& manager, CHTLState newState);
    ~StateGuard();
    
    // 禁止复制和移动
    StateGuard(const StateGuard&) = delete;
    StateGuard& operator=(const StateGuard&) = delete;
    StateGuard(StateGuard&&) = delete;
    StateGuard& operator=(StateGuard&&) = delete;
    
    // 手动恢复状态
    void restore();
    
    // 取消自动恢复
    void release();
    
private:
    StateManager& stateManager;
    CHTLState savedState;
    bool shouldRestore;
};

// 状态监听器接口
class StateListener {
public:
    virtual ~StateListener() = default;
    virtual void onStateChanged(CHTLState oldState, CHTLState newState) = 0;
    virtual void onStateError(const String& reason) = 0;
};

// 带监听器的状态管理器
class ObservableStateManager : public StateManager {
public:
    void addListener(std::shared_ptr<StateListener> listener);
    void removeListener(std::shared_ptr<StateListener> listener);
    
    // 重写状态设置方法以触发事件
    void setState(CHTLState newState) override;
    void enterErrorState(const String& reason) override;
    
private:
    std::vector<std::weak_ptr<StateListener>> listeners;
    
    void notifyStateChanged(CHTLState oldState, CHTLState newState);
    void notifyStateError(const String& reason);
    void cleanupListeners();
};

} // namespace CHTL