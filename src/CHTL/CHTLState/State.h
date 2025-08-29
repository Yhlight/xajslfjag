#ifndef CHTL_STATE_H
#define CHTL_STATE_H

#include <string>
#include <memory>
#include <stack>
#include <unordered_map>
#include <optional>

namespace CHTL {

// 编译器状态类型
enum class StateType {
    // 顶层状态
    GLOBAL,                 // 全局状态
    IN_NAMESPACE,           // 在命名空间内
    IN_CONFIGURATION,       // 在配置组内
    
    // 定义状态
    IN_TEMPLATE,            // 在模板定义内
    IN_CUSTOM,              // 在自定义内
    IN_ORIGIN,              // 在原始嵌入内
    IN_INFO,                // 在信息块内
    IN_EXPORT,              // 在导出块内
    
    // 元素状态
    IN_ELEMENT,             // 在元素内
    IN_STYLE_BLOCK,         // 在样式块内（局部或全局）
    IN_SCRIPT_BLOCK,        // 在脚本块内（局部或全局）
    IN_TEXT_BLOCK,          // 在文本块内
    
    // 样式相关状态
    IN_CLASS_SELECTOR,      // 在类选择器内
    IN_ID_SELECTOR,         // 在ID选择器内
    IN_PSEUDO_SELECTOR,     // 在伪类/伪元素选择器内
    IN_STYLE_PROPERTY,      // 在样式属性定义内
    
    // 特殊操作状态
    IN_DELETE_OPERATION,    // 在delete操作内
    IN_INSERT_OPERATION,    // 在insert操作内
    IN_INHERIT_OPERATION,   // 在inherit操作内
    IN_EXCEPT_CONSTRAINT,   // 在except约束内
    
    // 导入状态
    IN_IMPORT,              // 在导入语句内
    IN_USE,                 // 在use语句内
    
    // 属性状态
    IN_ATTRIBUTE,           // 在属性定义内
    IN_ATTRIBUTE_VALUE,     // 在属性值内
    
    // 字符串状态
    IN_STRING_LITERAL,      // 在字符串字面量内
    IN_COMMENT              // 在注释内
};

// 状态上下文信息
struct StateContext {
    StateType type;
    std::string name;           // 状态名称（如元素名、选择器名等）
    size_t startLine;
    size_t startColumn;
    std::unordered_map<std::string, std::string> properties;  // 额外属性
    
    StateContext(StateType t, const std::string& n = "", 
                 size_t line = 0, size_t col = 0)
        : type(t), name(n), startLine(line), startColumn(col) {}
    
    void setProperty(const std::string& key, const std::string& value) {
        properties[key] = value;
    }
    
    std::optional<std::string> getProperty(const std::string& key) const {
        auto it = properties.find(key);
        if (it != properties.end()) {
            return it->second;
        }
        return std::nullopt;
    }
};

// RAII状态管理器
class StateManager {
public:
    StateManager();
    ~StateManager() = default;
    
    // 进入新状态
    void pushState(StateType type, const std::string& name = "", 
                   size_t line = 0, size_t col = 0);
    
    // 退出当前状态
    void popState();
    
    // 获取当前状态
    StateType getCurrentState() const;
    
    // 获取当前状态上下文
    const StateContext* getCurrentContext() const;
    
    // 检查是否在特定状态
    bool isInState(StateType type) const;
    
    // 检查是否在任一指定状态
    bool isInAnyState(std::initializer_list<StateType> types) const;
    
    // 获取特定类型的最近状态上下文
    const StateContext* findNearestState(StateType type) const;
    
    // 获取状态栈深度
    size_t getStackDepth() const { return stateStack_.size(); }
    
    // 清空状态栈
    void reset();
    
    // 验证状态转换是否合法
    bool canTransitionTo(StateType newState) const;
    
    // 获取当前允许的操作
    std::vector<StateType> getAllowedTransitions() const;
    
    // 状态属性管理
    void setCurrentStateProperty(const std::string& key, const std::string& value);
    std::optional<std::string> getCurrentStateProperty(const std::string& key) const;
    
    // 获取状态路径（用于调试）
    std::string getStatePath() const;

private:
    std::stack<StateContext> stateStack_;
    
    // 状态转换规则
    static const std::unordered_map<StateType, std::vector<StateType>> transitionRules_;
    
    // 初始化状态转换规则
    static std::unordered_map<StateType, std::vector<StateType>> initTransitionRules();
};

// RAII状态守卫
class StateGuard {
public:
    StateGuard(StateManager& manager, StateType type, const std::string& name = "",
               size_t line = 0, size_t col = 0)
        : manager_(manager) {
        manager_.pushState(type, name, line, col);
    }
    
    ~StateGuard() {
        manager_.popState();
    }
    
    // 禁止拷贝
    StateGuard(const StateGuard&) = delete;
    StateGuard& operator=(const StateGuard&) = delete;
    
    // 允许移动
    StateGuard(StateGuard&& other) noexcept : manager_(other.manager_), moved_(false) {
        other.moved_ = true;
    }

private:
    StateManager& manager_;
    bool moved_ = false;
};

// 状态异常
class StateException : public std::exception {
public:
    explicit StateException(const std::string& message) : msg_(message) {}
    const char* what() const noexcept override { return msg_.c_str(); }
    
private:
    std::string msg_;
};

} // namespace CHTL

#endif // CHTL_STATE_H