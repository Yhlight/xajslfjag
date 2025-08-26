#pragma once

#include <string>
#include <stack>
#include <unordered_map>
#include <memory>

namespace CHTL {
namespace State {

/**
 * CHTLState - CHTL状态机
 * 管理CHTL编译过程中的状态转换
 */
class CHTLState {
public:
    /**
     * 状态类型
     */
    enum class StateType {
        INITIAL,              // 初始状态
        NAMESPACE_DECL,       // 命名空间声明
        IMPORT_DECL,          // 导入声明
        CONFIG_DECL,          // 配置声明
        TEMPLATE_DECL,        // 模板声明
        CUSTOM_DECL,          // 自定义声明
        ELEMENT_PARSING,      // 元素解析
        TEXT_PARSING,         // 文本解析
        STYLE_BLOCK,          // 样式块
        SCRIPT_BLOCK,         // 脚本块
        ORIGIN_BLOCK,         // 原始嵌入块
        COMMENT_PARSING,      // 注释解析
        ERROR_STATE,          // 错误状态
        FINAL                 // 最终状态
    };

private:
    StateType current_state_;
    std::stack<StateType> state_stack_;
    std::unordered_map<std::string, std::string> state_data_;
    
public:
    /**
     * 构造函数
     */
    CHTLState();
    
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
};

} // namespace State
} // namespace CHTL