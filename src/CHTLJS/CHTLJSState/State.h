#ifndef CHTLJS_STATE_H
#define CHTLJS_STATE_H

#include <string>
#include <memory>
#include <stack>
#include <unordered_map>
#include <optional>

namespace CHTLJS {

// CHTL JS编译器状态类型
enum class StateType {
    // 顶层状态
    GLOBAL,                    // 全局状态
    IN_MODULE_BLOCK,          // 在module块内
    IN_SCRIPT_BLOCK,          // 在script块内（局部或全局）
    
    // CHTL JS特有状态
    IN_ENHANCED_SELECTOR,     // 在增强选择器{{}}内
    IN_LISTEN_BLOCK,          // 在listen块内
    IN_DELEGATE_BLOCK,        // 在delegate块内
    IN_ANIMATE_BLOCK,         // 在animate块内
    IN_INEVERAWAY_BLOCK,      // 在iNeverAway块内
    IN_VIRTUAL_OBJECT,        // 在虚对象定义内
    
    // 动画相关状态
    IN_ANIMATE_BEGIN,         // 在animate的begin块内
    IN_ANIMATE_WHEN,          // 在animate的when块内
    IN_ANIMATE_END,           // 在animate的end块内
    
    // JavaScript状态
    IN_FUNCTION,              // 在函数内
    IN_OBJECT_LITERAL,        // 在对象字面量内
    IN_ARRAY_LITERAL,         // 在数组字面量内
    IN_PROPERTY_DEFINITION,   // 在属性定义内
    
    // 表达式状态
    IN_EXPRESSION,            // 在表达式内
    IN_ARROW_FUNCTION,        // 在箭头函数内
    IN_CALL_EXPRESSION,       // 在函数调用内
    
    // 特殊操作状态
    IN_EVENT_BINDING,         // 在事件绑定&->内
    IN_PROPERTY_ACCESS,       // 在属性访问内（->或.）
    
    // 字符串和注释状态
    IN_STRING_LITERAL,        // 在字符串字面量内
    IN_TEMPLATE_LITERAL,      // 在模板字符串内
    IN_COMMENT               // 在注释内
};

// 状态上下文信息
struct StateContext {
    StateType type;
    std::string name;           // 状态名称
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

// CHTL JS状态管理器
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
    
    // 特殊状态检查
    bool isInCHTLJSContext() const;        // 是否在CHTL JS特有语法上下文中
    bool isInSelectorContext() const;       // 是否在选择器上下文中
    bool isInAnimationContext() const;      // 是否在动画上下文中
    bool canUseVirtualObject() const;       // 是否可以使用虚对象
    
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
        if (!moved_) {
            manager_.popState();
        }
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

} // namespace CHTLJS

#endif // CHTLJS_STATE_H