#ifndef CHTLJS_STATE_H
#define CHTLJS_STATE_H

#include <memory>
#include <stack>
#include <string>
#include <unordered_map>
#include "../CHTLJSNode/BaseNode.h"

namespace CHTLJS {

// CHTL JS 状态类型
enum class StateType {
    INITIAL,                // 初始状态
    IN_STATEMENT,          // 在语句中
    IN_EXPRESSION,         // 在表达式中
    IN_DECLARATION,        // 在声明中
    IN_FUNCTION,           // 在函数定义中
    IN_BLOCK,              // 在代码块中
    IN_OBJECT_LITERAL,     // 在对象字面量中
    IN_ARRAY_LITERAL,      // 在数组字面量中
    IN_SELECTOR,           // 在{{}}选择器中
    IN_LISTEN_BLOCK,       // 在listen块中
    IN_ANIMATE_BLOCK,      // 在animate块中
    IN_DELEGATE_BLOCK,     // 在delegate块中
    IN_VIR_DECLARATION,    // 在vir声明中
    IN_INEVERAWAY_BLOCK,   // 在iNeverAway块中
    IN_ARROW_ACCESS,       // 在->访问中
    IN_BIND_EXPRESSION,    // 在&->绑定中
    IN_COMMENT,            // 在注释中
    IN_STRING              // 在字符串中
};

// 状态上下文
struct StateContext {
    StateType type;
    std::shared_ptr<BaseNode> currentNode;
    std::string contextData;
    int depth;
    
    // CHTL JS 特殊上下文信息
    bool inCHTLJSFunction;   // 是否在CHTL JS函数中
    bool inVirtualContext;   // 是否在虚对象上下文中
    std::string currentSelector;  // 当前选择器
    
    StateContext(StateType t) 
        : type(t), depth(0), inCHTLJSFunction(false), inVirtualContext(false) {}
};

// RAII状态守卫
class StateGuard {
private:
    class CHTLJSState* state;
    StateType previousState;
    
public:
    StateGuard(class CHTLJSState* s, StateType newState);
    ~StateGuard();
    
    // 禁止拷贝
    StateGuard(const StateGuard&) = delete;
    StateGuard& operator=(const StateGuard&) = delete;
    
    // 允许移动
    StateGuard(StateGuard&& other) noexcept;
    StateGuard& operator=(StateGuard&& other) noexcept;
};

// CHTL JS 状态机
class CHTLJSState {
private:
    std::stack<StateContext> stateStack;
    StateType currentState;
    
    // 状态转换规则
    std::unordered_map<StateType, std::unordered_map<StateType, bool>> transitionRules;
    
    // 状态历史
    std::vector<std::pair<StateType, std::string>> stateHistory;
    bool recordHistory;
    
    // CHTL JS 特殊状态标记
    bool inSelectorContext;
    bool inArrowChain;
    int selectorDepth;
    
    // 初始化转换规则
    void initializeTransitionRules();
    
public:
    CHTLJSState();
    ~CHTLJSState() = default;
    
    // 状态管理
    StateType getCurrentState() const { return currentState; }
    bool canTransitionTo(StateType newState) const;
    void pushState(StateType newState, std::shared_ptr<BaseNode> node = nullptr);
    void popState();
    
    // RAII状态管理
    [[nodiscard]] StateGuard enterState(StateType newState);
    
    // 上下文信息
    StateContext* getCurrentContext();
    const StateContext* getCurrentContext() const;
    std::shared_ptr<BaseNode> getCurrentNode();
    
    // CHTL JS 特殊状态管理
    void enterSelector(const std::string& selector);
    void exitSelector();
    bool isInSelector() const { return inSelectorContext; }
    std::string getCurrentSelector() const;
    
    void enterArrowChain();
    void exitArrowChain();
    bool isInArrowChain() const { return inArrowChain; }
    
    void markAsCHTLJSFunction();
    bool isInCHTLJSFunction() const;
    
    void markAsVirtualContext();
    bool isInVirtualContext() const;
    
    // 状态查询
    bool isInState(StateType state) const;
    bool isInAnyState(std::initializer_list<StateType> states) const;
    int getStateDepth(StateType state) const;
    
    // 状态验证
    bool validateCurrentState() const;
    std::string getStateError() const;
    bool canUseArrowOperator() const;
    bool canUseCHTLJSFeature() const;
    
    // 调试支持
    void enableHistoryRecording(bool enable) { recordHistory = enable; }
    const std::vector<std::pair<StateType, std::string>>& getStateHistory() const { return stateHistory; }
    void clearHistory() { stateHistory.clear(); }
    std::string getStateStackTrace() const;
    
    // 重置
    void reset();
    
    // 状态名称转换
    static std::string stateToString(StateType state);
    
    friend class StateGuard;
};

} // namespace CHTLJS

#endif // CHTLJS_STATE_H