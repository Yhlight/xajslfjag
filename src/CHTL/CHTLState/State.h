#pragma once
#include <string>
#include <stack>
#include <memory>
#include <functional>

namespace CHTL {

/**
 * CHTL状态枚举
 * 严格按照CHTL语法文档定义的语法状态
 */
enum class CHTLStateType {
    // 基础状态
    GLOBAL,                     // 全局作用域
    ELEMENT,                    // 元素作用域
    STYLE_BLOCK,                // 样式块作用域
    TEXT_BLOCK,                 // 文本块作用域
    
    // 模板状态
    TEMPLATE_DEFINITION,        // 模板定义
    TEMPLATE_STYLE,             // 样式组模板
    TEMPLATE_ELEMENT,           // 元素模板
    TEMPLATE_VAR,               // 变量组模板
    
    // 自定义状态
    CUSTOM_DEFINITION,          // 自定义定义
    CUSTOM_STYLE,               // 自定义样式组
    CUSTOM_ELEMENT,             // 自定义元素
    CUSTOM_VAR,                 // 自定义变量组
    
    // 导入状态
    IMPORT_STATEMENT,           // 导入语句
    
    // 结构状态
    NAMESPACE_DEFINITION,       // 命名空间定义
    CONFIGURATION_DEFINITION,   // 配置定义
    INFO_DEFINITION,            // 信息定义
    EXPORT_DEFINITION,          // 导出定义
    
    // 原始嵌入状态
    ORIGIN_HTML,                // 原始HTML嵌入
    ORIGIN_STYLE,               // 原始样式嵌入
    ORIGIN_JAVASCRIPT,          // 原始JS嵌入
    ORIGIN_CUSTOM,              // 自定义原始嵌入
    
    // 特殊状态
    ATTRIBUTE_DEFINITION,       // 属性定义
    SELECTOR_STYLE,             // 选择器样式
    INHERIT_OPERATION,          // 继承操作
    DELETE_OPERATION,           // 删除操作
    INSERT_OPERATION,           // 插入操作
    EXCEPT_CONSTRAINT,          // 约束操作
    
    // 错误状态
    INVALID_STATE               // 无效状态
};

/**
 * 状态信息类
 * 存储状态的详细信息
 */
class StateInfo {
public:
    CHTLStateType type;             // 状态类型
    std::string name;               // 状态名称
    std::string context;            // 上下文信息
    size_t depth;                   // 嵌套深度
    bool allowsNesting;             // 是否允许嵌套
    
    StateInfo(CHTLStateType t = CHTLStateType::GLOBAL, 
              const std::string& n = "",
              const std::string& ctx = "",
              size_t d = 0,
              bool nesting = true)
        : type(t), name(n), context(ctx), depth(d), allowsNesting(nesting) {}
    
    // 检查是否为模板相关状态
    bool isTemplateState() const;
    
    // 检查是否为自定义相关状态
    bool isCustomState() const;
    
    // 检查是否为结构定义状态
    bool isStructuralState() const;
    
    // 检查是否为原始嵌入状态
    bool isOriginState() const;
    
    // 检查是否为操作状态
    bool isOperationState() const;
    
    // 获取状态的字符串表示
    std::string toString() const;
};

/**
 * 状态转换规则
 */
class StateTransitionRule {
public:
    CHTLStateType fromState;        // 源状态
    CHTLStateType toState;          // 目标状态
    std::string trigger;            // 触发条件
    std::function<bool()> condition; // 额外条件检查
    
    StateTransitionRule(CHTLStateType from, 
                       CHTLStateType to, 
                       const std::string& trig,
                       std::function<bool()> cond = nullptr)
        : fromState(from), toState(to), trigger(trig), condition(cond) {}
    
    // 检查是否可以进行转换
    bool canTransition() const;
};

/**
 * RAII状态管理器
 * 自动管理状态的进入和退出
 */
class RAIIStateManager {
public:
    /**
     * 构造函数 - 自动进入状态
     */
    RAIIStateManager(class CHTLStateMachine* stateMachine, 
                     CHTLStateType newState,
                     const std::string& context = "");
    
    /**
     * 析构函数 - 自动退出状态
     */
    ~RAIIStateManager();
    
    // 禁用拷贝和移动
    RAIIStateManager(const RAIIStateManager&) = delete;
    RAIIStateManager& operator=(const RAIIStateManager&) = delete;
    RAIIStateManager(RAIIStateManager&&) = delete;
    RAIIStateManager& operator=(RAIIStateManager&&) = delete;
    
    /**
     * 获取当前状态信息
     */
    const StateInfo& getCurrentState() const;
    
    /**
     * 检查RAII管理器是否有效
     */
    bool isValid() const;

private:
    class CHTLStateMachine* m_stateMachine;
    bool m_valid;
};

/**
 * CHTL状态机
 * 管理CHTL语法解析过程中的状态转换
 */
class CHTLStateMachine {
public:
    /**
     * 构造函数
     */
    CHTLStateMachine();
    
    /**
     * 析构函数
     */
    ~CHTLStateMachine() = default;
    
    // 禁用拷贝和移动
    CHTLStateMachine(const CHTLStateMachine&) = delete;
    CHTLStateMachine& operator=(const CHTLStateMachine&) = delete;
    
    /**
     * 获取当前状态
     */
    const StateInfo& getCurrentState() const;
    
    /**
     * 获取当前状态类型
     */
    CHTLStateType getCurrentStateType() const;
    
    /**
     * 获取状态栈深度
     */
    size_t getDepth() const;
    
    /**
     * 进入新状态
     */
    bool enterState(CHTLStateType newState, const std::string& context = "");
    
    /**
     * 退出当前状态
     */
    bool exitState();
    
    /**
     * 检查是否可以进入指定状态
     */
    bool canEnterState(CHTLStateType newState) const;
    
    /**
     * 检查当前是否在指定状态
     */
    bool isInState(CHTLStateType state) const;
    
    /**
     * 检查当前是否在指定状态或其子状态
     */
    bool isInStateOrSubState(CHTLStateType state) const;
    
    /**
     * 重置状态机到初始状态
     */
    void reset();
    
    /**
     * 创建RAII状态管理器
     */
    std::unique_ptr<RAIIStateManager> createRAIIManager(CHTLStateType newState, 
                                                         const std::string& context = "");
    
    /**
     * 设置错误回调
     */
    void setErrorCallback(std::function<void(const std::string&)> callback);
    
    /**
     * 获取状态历史
     */
    std::vector<StateInfo> getStateHistory() const;

private:
    std::stack<StateInfo> m_stateStack;     // 状态栈
    std::vector<StateTransitionRule> m_rules; // 转换规则
    std::function<void(const std::string&)> m_errorCallback; // 错误回调
    std::vector<StateInfo> m_history;       // 状态历史
    
    /**
     * 初始化转换规则
     */
    void initializeTransitionRules();
    
    /**
     * 检查状态转换是否有效
     */
    bool isValidTransition(CHTLStateType from, CHTLStateType to) const;
    
    /**
     * 报告错误
     */
    void reportError(const std::string& message);
    
    /**
     * 记录状态历史
     */
    void recordStateHistory(const StateInfo& state);
    
    friend class RAIIStateManager;
};

/**
 * 便捷宏定义
 */
#define CHTL_STATE_GUARD(stateMachine, state) \
    auto stateGuard = (stateMachine).createRAIIManager(state, __FUNCTION__)

#define CHTL_STATE_GUARD_WITH_CONTEXT(stateMachine, state, context) \
    auto stateGuard = (stateMachine).createRAIIManager(state, context)

} // namespace CHTL