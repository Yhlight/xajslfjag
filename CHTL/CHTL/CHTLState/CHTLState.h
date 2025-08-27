#ifndef CHTL_STATE_H
#define CHTL_STATE_H

#include <memory>
#include <stack>
#include <string>
#include <unordered_map>
#include "../CHTLNode/BaseNode.h"

namespace CHTL {

// 状态类型枚举
enum class StateType {
    INITIAL,            // 初始状态
    IN_ELEMENT,         // 在元素内
    IN_TEXT,            // 在文本节点内
    IN_STYLE,           // 在样式块内
    IN_SCRIPT,          // 在脚本块内
    IN_TEMPLATE,        // 在模板定义内
    IN_CUSTOM,          // 在自定义定义内
    IN_ORIGIN,          // 在原始嵌入内
    IN_CONFIG,          // 在配置块内
    IN_NAMESPACE,       // 在命名空间内
    IN_IMPORT,          // 在导入语句内
    IN_OPERATOR,        // 在操作符内
    IN_ATTRIBUTE,       // 在属性定义内
    IN_LITERAL,         // 在字面量内
    IN_COMMENT          // 在注释内
};

// 状态上下文信息
struct StateContext {
    StateType type;
    std::shared_ptr<BaseNode> currentNode;
    std::string contextData;
    int depth;
    
    StateContext(StateType t) : type(t), depth(0) {}
};

// RAII状态守卫
class StateGuard {
private:
    class CHTLState* state;
    StateType previousState;
    
public:
    StateGuard(class CHTLState* s, StateType newState);
    ~StateGuard();
    
    // 禁止拷贝
    StateGuard(const StateGuard&) = delete;
    StateGuard& operator=(const StateGuard&) = delete;
    
    // 允许移动
    StateGuard(StateGuard&& other) noexcept;
    StateGuard& operator=(StateGuard&& other) noexcept;
};

// CHTL状态机
class CHTLState {
private:
    std::stack<StateContext> stateStack;
    StateType currentState;
    
    // 状态转换规则
    std::unordered_map<StateType, std::unordered_map<StateType, bool>> transitionRules;
    
    // 状态历史（用于调试）
    std::vector<std::pair<StateType, std::string>> stateHistory;
    bool recordHistory;
    
    // 初始化转换规则
    void initializeTransitionRules();
    
public:
    CHTLState();
    ~CHTLState() = default;
    
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
    
    // 状态查询
    bool isInState(StateType state) const;
    bool isInAnyState(std::initializer_list<StateType> states) const;
    int getStateDepth(StateType state) const;
    
    // 状态验证
    bool validateCurrentState() const;
    std::string getStateError() const;
    
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

} // namespace CHTL

#endif // CHTL_STATE_H