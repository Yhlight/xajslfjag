#ifndef CHTL_STATE_H
#define CHTL_STATE_H

#include "Core/Common.h"
#include <stack>
#include <functional>

namespace CHTL {

// CHTL编译器状态
enum class CompileState {
    // 基础状态
    Initial,                // 初始状态
    Normal,                 // 普通状态
    
    // 元素相关状态
    InElement,              // 在元素内部
    InAttribute,            // 在属性定义中
    InText,                 // 在text块中
    
    // 样式相关状态
    InLocalStyle,           // 在局部style块中
    InGlobalStyle,          // 在全局style块中
    InStyleSelector,        // 在样式选择器中
    InStyleProperty,        // 在样式属性中
    
    // 脚本相关状态
    InLocalScript,          // 在局部script块中
    InGlobalScript,         // 在全局script块中
    
    // 模板相关状态
    InTemplate,             // 在[Template]块中
    InTemplateStyle,        // 在模板样式组中
    InTemplateElement,      // 在模板元素中
    InTemplateVariable,     // 在模板变量组中
    
    // 自定义相关状态
    InCustom,               // 在[Custom]块中
    InCustomStyle,          // 在自定义样式组中
    InCustomElement,        // 在自定义元素中
    InCustomVariable,       // 在自定义变量组中
    
    // 原始嵌入状态
    InOrigin,               // 在[Origin]块中
    InOriginHtml,           // 在原始HTML中
    InOriginStyle,          // 在原始CSS中
    InOriginJavaScript,     // 在原始JavaScript中
    InOriginCustom,         // 在自定义原始类型中
    
    // 配置相关状态
    InConfiguration,        // 在[Configuration]块中
    InConfigName,           // 在Name配置块中
    InConfigOriginType,     // 在OriginType配置块中
    
    // 导入相关状态
    InImport,               // 在[Import]语句中
    
    // 命名空间状态
    InNamespace,            // 在[Namespace]块中
    
    // 特殊状态
    InComment,              // 在注释中
    InString,               // 在字符串中
    Error                   // 错误状态
};

// 状态转换规则
struct StateTransition {
    CompileState from;      // 源状态
    CompileState to;        // 目标状态
    std::function<bool()> condition;  // 转换条件
    std::function<void()> action;     // 转换动作
};

// 状态机
class StateMachine {
private:
    CompileState currentState_;
    std::stack<CompileState> stateStack_;
    Vector<StateTransition> transitions_;
    HashMap<CompileState, std::function<void()>> enterActions_;
    HashMap<CompileState, std::function<void()>> exitActions_;
    HashMap<CompileState, String> stateNames_;
    
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
    
    // 状态检查助手
    bool IsInElement() const;
    bool IsInStyle() const;
    bool IsInScript() const;
    bool IsInTemplate() const;
    bool IsInCustom() const;
    bool IsInOrigin() const;
    bool IsInConfiguration() const;
    
    // 重置状态机
    void Reset();
    
    // 获取状态栈深度
    size_t GetStackDepth() const { return stateStack_.size(); }
    
    // 调试输出
    String DumpStateStack() const;
    
private:
    // 初始化状态名称
    void InitializeStateNames();
    
    // 初始化默认转换规则
    void InitializeDefaultTransitions();
    
    // 执行状态转换
    void ExecuteTransition(CompileState from, CompileState to);
};

// 状态上下文（用于保存状态相关数据）
class StateContext {
private:
    HashMap<String, std::any> data_;
    CompileState associatedState_;
    
public:
    explicit StateContext(CompileState state) : associatedState_(state) {}
    
    // 设置数据
    template<typename T>
    void Set(const String& key, const T& value) {
        data_[key] = value;
    }
    
    // 获取数据
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
    
    // 检查是否存在数据
    bool Has(const String& key) const {
        return data_.find(key) != data_.end();
    }
    
    // 删除数据
    void Remove(const String& key) {
        data_.erase(key);
    }
    
    // 清空数据
    void Clear() {
        data_.clear();
    }
    
    // 获取关联的状态
    CompileState GetAssociatedState() const { return associatedState_; }
};

// 状态管理器（管理状态机和状态上下文）
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

} // namespace CHTL

#endif // CHTL_STATE_H