#pragma once
#include <string>
#include <stack>
#include <memory>
#include <functional>

namespace CHTLJS {

/**
 * CHTL JS状态枚举
 * 专门用于CHTL JS语法的状态管理
 */
enum class CHTLJSStateType {
    // 基础状态
    GLOBAL,                     // 全局作用域
    SCRIPT_BLOCK,               // script块作用域
    MODULE_BLOCK,               // module块作用域
    
    // CHTL JS特有状态
    ENHANCED_SELECTOR,          // 增强选择器状态 {{...}}
    EVENT_LISTENER,             // 事件监听器状态
    EVENT_DELEGATE,             // 事件委托状态
    ANIMATION_BLOCK,            // 动画块状态
    VIRTUAL_OBJECT,             // 虚拟对象状态
    EVENT_BINDING,              // 事件绑定状态 &->
    
    // 函数状态
    CHTLJS_FUNCTION,            // CHTL JS函数状态
    FUNCTION_PARAMETERS,        // 函数参数状态
    
    // 错误状态
    INVALID_STATE               // 无效状态
};

/**
 * CHTL JS状态信息类
 */
class StateInfo {
public:
    CHTLJSStateType type;           // 状态类型
    std::string name;               // 状态名称
    std::string context;            // 上下文信息
    size_t depth;                   // 嵌套深度
    bool allowsNesting;             // 是否允许嵌套
    
    StateInfo(CHTLJSStateType t = CHTLJSStateType::GLOBAL, 
              const std::string& n = "",
              const std::string& ctx = "",
              size_t d = 0,
              bool nesting = true)
        : type(t), name(n), context(ctx), depth(d), allowsNesting(nesting) {}
    
    // 检查是否为CHTL JS特有状态
    bool isCHTLJSSpecificState() const;
    
    // 检查是否为事件相关状态
    bool isEventState() const;
    
    // 获取状态的字符串表示
    std::string toString() const;
};

/**
 * CHTL JS状态机
 * 使用RAII模式管理状态转换
 */
class CHTLJSStateMachine {
public:
    CHTLJSStateMachine();
    ~CHTLJSStateMachine() = default;
    
    // 禁用拷贝和移动
    CHTLJSStateMachine(const CHTLJSStateMachine&) = delete;
    CHTLJSStateMachine& operator=(const CHTLJSStateMachine&) = delete;
    
    /**
     * 进入新状态
     */
    void enterState(CHTLJSStateType type, const std::string& name = "", const std::string& context = "");
    
    /**
     * 退出当前状态
     */
    void exitState();
    
    /**
     * 获取当前状态
     */
    StateInfo getCurrentState() const;
    
    /**
     * 获取状态深度
     */
    size_t getDepth() const;
    
    /**
     * 检查是否可以进入指定状态
     */
    bool canEnterState(CHTLJSStateType type) const;
    
    /**
     * 重置状态机
     */
    void reset();

private:
    std::stack<StateInfo> m_stateStack;     // 状态栈
    
    /**
     * 验证状态转换
     */
    bool validateStateTransition(CHTLJSStateType from, CHTLJSStateType to) const;
};

/**
 * RAII状态管理器
 * 自动管理状态的进入和退出
 */
class StateGuard {
public:
    StateGuard(CHTLJSStateMachine& stateMachine, CHTLJSStateType type, 
               const std::string& name = "", const std::string& context = "");
    ~StateGuard();
    
    // 禁用拷贝和移动
    StateGuard(const StateGuard&) = delete;
    StateGuard& operator=(const StateGuard&) = delete;
    StateGuard(StateGuard&&) = delete;
    StateGuard& operator=(StateGuard&&) = delete;

private:
    CHTLJSStateMachine& m_stateMachine;
    bool m_shouldExit;
};

} // namespace CHTLJS