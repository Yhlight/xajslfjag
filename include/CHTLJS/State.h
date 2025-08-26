#ifndef CHTLJS_STATE_H
#define CHTLJS_STATE_H

#include "Core/Common.h"
#include <stack>
#include <functional>

namespace CHTLJS {

// CHTL JS编译器状态
enum class CompileState {
    // 基础状态
    Initial,                    // 初始状态
    Normal,                     // 普通JavaScript代码
    
    // CHTL JS特有状态
    InEnhancedSelector,         // 在增强选择器 {{}} 中
    InArrowFunction,            // 在箭头操作符 -> 后
    InListen,                   // 在listen语句中
    InDelegate,                 // 在delegate语句中
    InAnimate,                  // 在animate定义中
    InVir,                      // 在vir虚对象定义中
    
    // 动画相关状态
    InAnimateFrom,              // 在from块中
    InAnimateTo,                // 在to块中
    InAnimateBy,                // 在by块中
    InAnimateOptions,           // 在动画选项中
    
    // 事件相关状态
    InEventName,                // 在事件名解析中
    InEventHandler,             // 在事件处理函数中
    InEventOptions,             // 在事件选项中
    
    // JavaScript相关状态
    InFunction,                 // 在函数定义中
    InObject,                   // 在对象字面量中
    InArray,                    // 在数组字面量中
    InString,                   // 在字符串中
    InTemplate,                 // 在模板字符串中
    
    // 键值对状态（支持无序和可选）
    InKeyValuePairs,            // 在键值对定义中
    InOptionalValue,            // 在可选值中
    
    // 表达式状态
    InExpression,               // 在表达式中
    InCondition,                // 在条件表达式中
    InLoop,                     // 在循环中
    
    // 注释状态
    InComment,                  // 在注释中
    
    // 错误状态
    Error                       // 错误状态
};

// 状态转换规则
struct StateTransition {
    CompileState from;          // 源状态
    CompileState to;            // 目标状态
    std::function<bool()> condition;  // 转换条件
    std::function<void()> action;     // 转换动作
};

// CHTL JS状态机
class StateMachine {
private:
    CompileState currentState_;
    std::stack<CompileState> stateStack_;
    Vector<StateTransition> transitions_;
    HashMap<CompileState, std::function<void()>> enterActions_;
    HashMap<CompileState, std::function<void()>> exitActions_;
    HashMap<CompileState, String> stateNames_;
    
    // CHTL JS特有的状态信息
    struct {
        bool inEnhancedSelector = false;
        bool inArrowFunction = false;
        int selectorDepth = 0;      // {{}} 嵌套深度
        int braceDepth = 0;         // {} 嵌套深度
        int parenDepth = 0;         // () 嵌套深度
        int bracketDepth = 0;       // [] 嵌套深度
    } jsState_;
    
public:
    StateMachine();
    ~StateMachine() = default;
    
    // 获取当前状态
    CompileState GetCurrentState() const { return currentState_; }
    
    // 设置当前状态
    void SetState(CompileState state);
    
    // 转换状态
    bool TransitionTo(CompileState newState);
    
    // 压入状态（保存当前状态）
    void PushState(CompileState newState);
    
    // 弹出状态（恢复之前的状态）
    bool PopState();
    
    // 添加状态转换规则
    void AddTransition(const StateTransition& transition);
    void AddTransition(CompileState from, CompileState to, 
                      std::function<bool()> condition = nullptr,
                      std::function<void()> action = nullptr);
    
    // 设置状态进入/退出动作
    void SetEnterAction(CompileState state, std::function<void()> action);
    void SetExitAction(CompileState state, std::function<void()> action);
    
    // 检查是否可以转换到指定状态
    bool CanTransitionTo(CompileState state) const;
    
    // 获取状态名称
    String GetStateName(CompileState state) const;
    String GetCurrentStateName() const;
    
    // CHTL JS特有的状态检查
    bool IsInEnhancedSelector() const { return jsState_.inEnhancedSelector; }
    bool IsInArrowFunction() const { return jsState_.inArrowFunction; }
    bool IsInAnimation() const;
    bool IsInEventHandler() const;
    bool IsInVirtualObject() const;
    
    // 嵌套深度管理
    void EnterEnhancedSelector() { jsState_.selectorDepth++; jsState_.inEnhancedSelector = true; }
    void ExitEnhancedSelector() { jsState_.selectorDepth--; if (jsState_.selectorDepth == 0) jsState_.inEnhancedSelector = false; }
    int GetSelectorDepth() const { return jsState_.selectorDepth; }
    
    void EnterBrace() { jsState_.braceDepth++; }
    void ExitBrace() { jsState_.braceDepth--; }
    int GetBraceDepth() const { return jsState_.braceDepth; }
    
    // 重置状态机
    void Reset();
    
    // 获取状态栈深度
    size_t GetStackDepth() const { return stateStack_.size(); }
    
    // 调试输出
    String DumpStateStack() const;
    String DumpJSState() const;
    
private:
    // 初始化状态名称
    void InitializeStateNames();
    
    // 初始化默认转换规则
    void InitializeDefaultTransitions();
    
    // 执行状态转换
    void ExecuteTransition(CompileState from, CompileState to);
};

// 状态上下文（保存CHTL JS特有的状态数据）
class StateContext {
private:
    HashMap<String, std::any> data_;
    CompileState associatedState_;
    
    // CHTL JS特有数据
    struct {
        String currentSelector;     // 当前处理的选择器
        String currentEvent;        // 当前处理的事件
        String currentAnimation;    // 当前处理的动画
        Vector<String> pendingKeys; // 待处理的键（用于无序键值对）
        bool inOptionalValue = false; // 是否在可选值中
    } jsData_;
    
public:
    explicit StateContext(CompileState state) : associatedState_(state) {}
    
    // 通用数据访问
    template<typename T>
    void Set(const String& key, const T& value) {
        data_[key] = value;
    }
    
    template<typename T>
    Optional<T> Get(const String& key) const {
        auto it = data_.find(key);
        if (it != data_.end()) {
            try {
                return std::any_cast<T>(it->second);
            } catch (const std::bad_any_cast&) {
                return std::nullopt;
            }
        }
        return std::nullopt;
    }
    
    // CHTL JS特有数据访问
    void SetCurrentSelector(const String& selector) { jsData_.currentSelector = selector; }
    String GetCurrentSelector() const { return jsData_.currentSelector; }
    
    void SetCurrentEvent(const String& event) { jsData_.currentEvent = event; }
    String GetCurrentEvent() const { return jsData_.currentEvent; }
    
    void SetCurrentAnimation(const String& animation) { jsData_.currentAnimation = animation; }
    String GetCurrentAnimation() const { return jsData_.currentAnimation; }
    
    void AddPendingKey(const String& key) { jsData_.pendingKeys.push_back(key); }
    Vector<String> GetPendingKeys() const { return jsData_.pendingKeys; }
    void ClearPendingKeys() { jsData_.pendingKeys.clear(); }
    
    void SetInOptionalValue(bool value) { jsData_.inOptionalValue = value; }
    bool IsInOptionalValue() const { return jsData_.inOptionalValue; }
    
    // 清空数据
    void Clear() {
        data_.clear();
        jsData_ = {};
    }
    
    // 获取关联的状态
    CompileState GetAssociatedState() const { return associatedState_; }
};

// 状态管理器
class StateManager {
private:
    UniquePtr<StateMachine> stateMachine_;
    HashMap<CompileState, UniquePtr<StateContext>> contexts_;
    
public:
    StateManager();
    ~StateManager() = default;
    
    // 获取状态机
    StateMachine& GetStateMachine() { return *stateMachine_; }
    const StateMachine& GetStateMachine() const { return *stateMachine_; }
    
    // 获取或创建状态上下文
    StateContext& GetOrCreateContext(CompileState state);
    
    // 获取当前状态的上下文
    StateContext& GetCurrentContext();
    
    // 清理指定状态的上下文
    void ClearContext(CompileState state);
    
    // 清理所有上下文
    void ClearAllContexts();
    
    // 重置
    void Reset();
};

// RAII状态守卫
class StateGuard {
private:
    StateMachine& machine_;
    CompileState savedState_;
    bool useStack_;
    
public:
    // 使用栈保存状态
    StateGuard(StateMachine& machine, CompileState newState)
        : machine_(machine), useStack_(true) {
        machine_.PushState(newState);
    }
    
    // 直接保存当前状态
    StateGuard(StateMachine& machine, CompileState newState, bool)
        : machine_(machine), savedState_(machine.GetCurrentState()), useStack_(false) {
        machine_.SetState(newState);
    }
    
    ~StateGuard() {
        if (useStack_) {
            machine_.PopState();
        } else {
            machine_.SetState(savedState_);
        }
    }
};

// 增强选择器守卫（RAII）
class EnhancedSelectorGuard {
private:
    StateMachine& machine_;
    
public:
    explicit EnhancedSelectorGuard(StateMachine& machine) : machine_(machine) {
        machine_.EnterEnhancedSelector();
    }
    
    ~EnhancedSelectorGuard() {
        machine_.ExitEnhancedSelector();
    }
};

} // namespace CHTLJS

#endif // CHTLJS_STATE_H