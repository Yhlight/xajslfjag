#pragma once

#include <stack>
#include <string>
#include <memory>

namespace CHTLJS {

// CHTL JS解析状态
enum class ParseState {
    GLOBAL,              // 全局作用域
    FUNCTION,            // 函数内部
    BLOCK,               // 块作用域
    OBJECT_LITERAL,      // 对象字面量内部
    ARRAY_LITERAL,       // 数组字面量内部
    ENHANCED_SELECTOR,   // {{}}内部
    CHTLJS_FUNCTION,     // CHTL JS函数内部(listen/delegate/animate)
    VIR_DECLARATION,     // vir声明内部
    EVENT_BINDING,       // &->事件绑定内部
};

// CHTL JS状态管理器 - RAII模式
class CHTLJSState {
public:
    CHTLJSState();
    ~CHTLJSState();
    
    // 状态管理
    void PushState(ParseState state);
    void PopState();
    ParseState GetCurrentState() const;
    bool IsInState(ParseState state) const;
    
    // 特殊状态查询
    bool IsInEnhancedSelector() const;
    bool IsInCHTLJSFunction() const;
    bool IsInVirDeclaration() const;
    bool IsInEventBinding() const;
    bool IsInFunction() const;
    bool IsInGlobalScope() const;
    
    // 获取当前CHTL JS函数名
    void SetCurrentCHTLJSFunction(const std::string& name);
    std::string GetCurrentCHTLJSFunction() const;
    
    // 虚对象管理
    void EnterVirtualObject(const std::string& name);
    void ExitVirtualObject();
    std::string GetCurrentVirtualObject() const;
    bool IsInVirtualObject() const;
    
    // 状态信息
    size_t GetStateDepth() const { return stateStack.size(); }
    std::string GetStateString() const;
    
private:
    std::stack<ParseState> stateStack;
    std::string currentCHTLJSFunction;
    std::stack<std::string> virtualObjectStack;
};

// RAII状态守卫
class StateGuard {
public:
    StateGuard(std::shared_ptr<CHTLJSState> state, ParseState newState);
    ~StateGuard();
    
    // 禁止拷贝
    StateGuard(const StateGuard&) = delete;
    StateGuard& operator=(const StateGuard&) = delete;
    
    // 允许移动
    StateGuard(StateGuard&& other) noexcept;
    StateGuard& operator=(StateGuard&& other) noexcept;
    
private:
    std::shared_ptr<CHTLJSState> state;
    bool active;
};

} // namespace CHTLJS