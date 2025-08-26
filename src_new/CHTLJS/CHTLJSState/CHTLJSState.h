#pragma once

#include <string>
#include <stack>
#include <unordered_map>
#include <memory>

namespace CHTL {
namespace CHTLJS {
namespace State {

/**
 * CHTLJSState - CHTL JS状态机
 * 管理CHTL JS编译过程中的状态转换
 */
class CHTLJSState {
public:
    /**
     * CHTL JS状态类型
     */
    enum class StateType {
        INITIAL,              // 初始状态
        SCRIPT_CONTEXT,       // script上下文
        ENHANCED_SELECTOR,    // 增强选择器解析
        VIR_DECLARATION,      // vir对象声明
        VIR_ACCESS,           // vir对象访问
        LISTEN_FUNCTION,      // listen函数
        DELEGATE_FUNCTION,    // delegate函数
        ANIMATE_FUNCTION,     // animate函数
        FUNCTION_BLOCK,       // 函数块（新语法）
        CSS_BLOCK,            // CSS块（begin, end等）
        WHEN_ARRAY,           // when数组
        EVENT_HANDLER,        // 事件处理器
        PURE_JAVASCRIPT,      // 纯JavaScript
        ERROR_STATE,          // 错误状态
        FINAL                 // 最终状态
    };

private:
    StateType current_state_;
    std::stack<StateType> state_stack_;
    std::unordered_map<std::string, std::string> state_data_;
    std::string current_function_type_;  // 当前函数类型（listen, delegate, animate）
    
public:
    /**
     * 构造函数
     */
    CHTLJSState();
    
    /**
     * 获取当前状态
     * @return 当前状态
     */
    StateType getCurrentState() const;
    
    /**
     * 转换状态
     * @param new_state 新状态
     * @return 是否成功转换
     */
    bool transitionTo(StateType new_state);
    
    /**
     * 压入状态
     * @param state 状态
     */
    void pushState(StateType state);
    
    /**
     * 弹出状态
     * @return 弹出的状态
     */
    StateType popState();
    
    /**
     * 检查状态转换是否有效
     * @param from 源状态
     * @param to 目标状态
     * @return 是否有效
     */
    bool isValidTransition(StateType from, StateType to) const;
    
    /**
     * 重置状态机
     */
    void reset();
    
    /**
     * 设置当前函数类型
     * @param function_type 函数类型
     */
    void setCurrentFunctionType(const std::string& function_type);
    
    /**
     * 获取当前函数类型
     * @return 函数类型
     */
    const std::string& getCurrentFunctionType() const;
    
    /**
     * 检查是否在CHTL JS函数中
     * @return 是否在函数中
     */
    bool isInCHTLJSFunction() const;
    
    /**
     * 检查是否在vir上下文中
     * @return 是否在vir上下文中
     */
    bool isInVirContext() const;
    
    /**
     * 检查是否在增强选择器上下文中
     * @return 是否在增强选择器上下文中
     */
    bool isInEnhancedSelectorContext() const;
    
    /**
     * 设置状态数据
     * @param key 键
     * @param value 值
     */
    void setStateData(const std::string& key, const std::string& value);
    
    /**
     * 获取状态数据
     * @param key 键
     * @return 值
     */
    std::string getStateData(const std::string& key) const;
    
    /**
     * 状态类型转字符串
     * @param state 状态类型
     * @return 字符串表示
     */
    static std::string stateTypeToString(StateType state);
    
    /**
     * 检查是否为CHTL JS函数状态
     * @param state 状态类型
     * @return 是否为函数状态
     */
    static bool isCHTLJSFunctionState(StateType state);
};

} // namespace State
} // namespace CHTLJS
} // namespace CHTL