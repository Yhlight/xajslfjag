#pragma once

#include <string>
#include <vector>
#include <stack>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <functional>
#include <optional>

namespace CHTL {
namespace Common {

// 状态类型
enum class StateType {
    // 根级状态
    ROOT_STATE,                 // 根状态
    
    // CHTL语法状态
    HTML_ELEMENT,               // HTML元素状态
    ELEMENT_ATTRIBUTES,         // 元素属性状态
    TEXT_NODE,                  // 文本节点状态
    
    // 样式相关状态
    GLOBAL_STYLE_BLOCK,         // 全局样式块
    LOCAL_STYLE_BLOCK,          // 局部样式块
    STYLE_SELECTOR,             // 样式选择器
    STYLE_PROPERTIES,           // 样式属性
    
    // 脚本相关状态
    GLOBAL_SCRIPT_BLOCK,        // 全局脚本块
    LOCAL_SCRIPT_BLOCK,         // 局部脚本块
    CHTL_JS_SYNTAX,             // CHTL JS语法状态
    
    // CHTL JS特有状态
    ENHANCED_SELECTOR,          // 增强选择器 {{selector}}
    VIR_OBJECT,                 // 虚对象 vir
    LISTEN_FUNCTION,            // listen函数
    DELEGATE_FUNCTION,          // delegate函数
    ANIMATE_FUNCTION,           // animate函数
    ARROW_OPERATION,            // -> 操作符
    EVENT_BIND_OPERATION,       // &-> 事件绑定操作符
    
    // 统一化语法状态
    CHTLJS_FUNCTION_CALL,       // CHTL JS函数调用状态
    CHTLJS_KEYVALUE_BLOCK,      // CHTL JS键值对块状态
    CHTLJS_NESTED_SELECTOR,     // CHTL JS嵌套选择器状态
    CHTLJS_MIXED_SYNTAX,        // CHTL JS与原生JS混合语法状态
    
    // 模板和自定义状态
    TEMPLATE_BLOCK,             // 模板块
    CUSTOM_BLOCK,               // 自定义块
    
    // 原始嵌入状态
    ORIGIN_BLOCK,               // 原始嵌入块
    
    // 配置和命名空间状态
    CONFIGURATION_BLOCK,        // 配置块
    NAMESPACE_BLOCK,            // 命名空间块
    IMPORT_STATEMENT,           // 导入语句
    
    // 错误状态
    ERROR_STATE,                // 错误状态
    UNKNOWN_STATE               // 未知状态
};

// 状态转换事件
enum class StateTransitionEvent {
    // 语法块开始/结束
    BLOCK_START,                // 块开始 {
    BLOCK_END,                  // 块结束 }
    
    // 关键字检测
    KEYWORD_DETECTED,           // 关键字检测到
    IDENTIFIER_DETECTED,        // 标识符检测到
    
    // CHTL JS特定事件
    ENHANCED_SELECTOR_START,    // {{ 检测到
    ENHANCED_SELECTOR_END,      // }} 检测到
    ARROW_OPERATOR_DETECTED,    // -> 检测到
    
    // 统一化语法事件
    CHTLJS_FUNCTION_START,      // CHTL JS函数开始 (animate/listen/delegate {)
    CHTLJS_FUNCTION_BLOCK_START, // CHTL JS函数块开始 {
    CHTLJS_FUNCTION_BLOCK_END,  // CHTL JS函数块结束 }
    CSS_BLOCK_IN_CHTLJS,        // CHTL JS中的CSS块 (begin/end/when)
    NESTED_CONTEXT_ENTER,       // 进入嵌套上下文
    NESTED_CONTEXT_EXIT,        // 退出嵌套上下文
    SYNTAX_AMBIGUITY_DETECTED,  // 检测到语法歧义
    
    // 语法元素
    COMMENT_START,              // 注释开始
    COMMENT_END,                // 注释结束
    STRING_LITERAL,             // 字符串字面量
    
    // 错误事件
    SYNTAX_ERROR,               // 语法错误
    UNEXPECTED_TOKEN,           // 意外token
    
    // 特殊事件
    CONTEXT_SWITCH,             // 上下文切换
    FORCE_TRANSITION            // 强制转换
};

// 状态节点信息
struct StateNode {
    StateType type;                             // 状态类型
    std::string state_name;                     // 状态名称
    size_t depth_level;                         // 嵌套深度
    size_t start_position;                      // 开始位置
    size_t end_position;                        // 结束位置（可选）
    std::unordered_map<std::string, std::string> context_data; // 上下文数据
    std::string matched_keyword;                // 匹配的关键字
    bool is_active;                             // 是否活跃
    bool is_error;                              // 是否错误状态
    
    StateNode() : type(StateType::UNKNOWN_STATE), depth_level(0), 
                 start_position(0), end_position(0), is_active(false), is_error(false) {}
    
    StateNode(StateType t, const std::string& name, size_t pos)
        : type(t), state_name(name), depth_level(0), start_position(pos), 
          end_position(0), is_active(true), is_error(false) {}
    
    void setContextData(const std::string& key, const std::string& value) {
        context_data[key] = value;
    }
    
    std::string getContextData(const std::string& key, const std::string& default_val = "") const {
        auto it = context_data.find(key);
        return it != context_data.end() ? it->second : default_val;
    }
    
    bool hasContextData(const std::string& key) const {
        return context_data.find(key) != context_data.end();
    }
    
    void close(size_t pos) {
        end_position = pos;
        is_active = false;
    }
};

// 状态转换规则
struct TransitionRule {
    StateType from_state;                       // 源状态
    StateType to_state;                         // 目标状态
    StateTransitionEvent trigger_event;         // 触发事件
    std::string condition_pattern;              // 条件模式（正则表达式）
    std::function<bool(const StateNode&, const std::string&)> validator; // 验证器
    int priority;                               // 优先级
    std::string description;                    // 描述
    
    TransitionRule() : from_state(StateType::UNKNOWN_STATE), 
                      to_state(StateType::UNKNOWN_STATE),
                      trigger_event(StateTransitionEvent::FORCE_TRANSITION),
                      priority(0) {}
    
    TransitionRule(StateType from, StateType to, StateTransitionEvent event, 
                   const std::string& pattern = "", int prio = 0)
        : from_state(from), to_state(to), trigger_event(event), 
          condition_pattern(pattern), priority(prio) {}
    
    bool isValid(const StateNode& current_state, const std::string& input) const {
        if (current_state.type != from_state) return false;
        if (!condition_pattern.empty() && !matchesPattern(input, condition_pattern)) return false;
        if (validator && !validator(current_state, input)) return false;
        return true;
    }
    
private:
    bool matchesPattern(const std::string& input, const std::string& pattern) const;
};

// 状态机事件处理器
class StateEventHandler {
public:
    virtual ~StateEventHandler() = default;
    virtual void onStateEnter(const StateNode& state) {}
    virtual void onStateExit(const StateNode& state) {}
    virtual void onStateTransition(const StateNode& from, const StateNode& to) {}
    virtual void onStateError(const StateNode& state, const std::string& error) {}
};

// RAII状态守卫
class StateGuard {
public:
    StateGuard(class StateMachine* machine, StateType state, const std::string& name, size_t pos);
    ~StateGuard();
    
    // 禁止复制，允许移动
    StateGuard(const StateGuard&) = delete;
    StateGuard& operator=(const StateGuard&) = delete;
    StateGuard(StateGuard&& other) noexcept;
    StateGuard& operator=(StateGuard&& other) noexcept;
    
    // 手动释放（提前退出状态）
    void release();
    
    // 获取状态信息
    const StateNode& getState() const { return state_node; }
    bool isValid() const { return machine != nullptr && !released; }
    
    // 设置上下文数据
    void setContextData(const std::string& key, const std::string& value);
    std::string getContextData(const std::string& key, const std::string& default_val = "") const;

private:
    class StateMachine* machine;
    StateNode state_node;
    bool released;
};

// 状态机核心类
class StateMachine {
public:
    StateMachine();
    ~StateMachine();
    
    // 初始化和配置
    void initializeStandardRules();
    void initializeCHTLRules();
    void initializeCHTLJSRules();
    void reset();
    
    // 状态转换
    
    // 手动状态转换
    bool transitionTo(StateType new_state, const std::string& trigger_input = "", 
                     size_t position = 0);
    
    // 自动状态检测和转换
    bool autoTransition(const std::string& input, size_t position);
    
    // 基于token的状态转换
    bool transitionOnToken(const std::string& token, size_t position);
    
    // 状态栈管理
    
    // 推入新状态（RAII管理）
    std::unique_ptr<StateGuard> pushState(StateType state, const std::string& name, size_t position);
    
    // 弹出当前状态
    bool popState(size_t end_position = 0);
    
    // 查询当前状态
    StateType getCurrentState() const;
    const StateNode& getCurrentStateNode() const;
    size_t getStateDepth() const { return state_stack.size(); }
    
    // 状态历史
    std::vector<StateNode> getStateHistory() const { return state_history; }
    StateNode getLastState() const;
    
    // 规则管理
    
    // 添加转换规则
    void addTransitionRule(const TransitionRule& rule);
    
    // 移除转换规则
    void removeTransitionRule(StateType from, StateType to, StateTransitionEvent event);
    
    // 获取可能的转换
    std::vector<StateType> getPossibleTransitions(StateType from_state, 
                                                  StateTransitionEvent event) const;
    
    // 上下文感知功能
    
    // 检查是否在特定语法上下文中
    bool isInCHTLJSContext() const;
    bool isInStyleContext() const;
    bool isInScriptContext() const;
    bool isInTemplateContext() const;
    
    // 检查当前上下文是否允许特定语法
    bool canUseCHTLJSSyntax() const;
    bool canUseEnhancedSelectors() const;
    bool canUseArrowOperator() const;
    
    // 获取当前语法约束
    std::unordered_set<std::string> getCurrentSyntaxConstraints() const;
    
    // 验证语法在当前上下文中的有效性
    bool validateSyntax(const std::string& syntax) const;
    
    // 事件处理
    
    // 注册事件处理器
    void registerEventHandler(std::shared_ptr<StateEventHandler> handler);
    
    // 触发状态事件
    void triggerStateEvent(StateTransitionEvent event, const std::string& data = "");
    
    // 错误处理
    
    // 报告状态错误
    void reportError(const std::string& error, size_t position = 0);
    
    // 获取错误信息
    std::vector<std::string> getErrors() const { return errors; }
    void clearErrors() { errors.clear(); }
    
    // 尝试错误恢复
    bool attemptErrorRecovery(const std::string& input, size_t position);
    
    // 调试和诊断
    
    // 获取状态机状态
    std::string getStateMachineStatus() const;
    
    // 转储状态栈
    void dumpStateStack() const;
    
    // 生成状态转换图
    std::string generateTransitionDiagram() const;
    
    // 验证状态机完整性
    bool validateStateMachine() const;
    
    // 配置选项
    void setStrictMode(bool strict) { strict_mode = strict; }
    void setDebugMode(bool debug) { debug_mode = debug; }
    void setAutoRecovery(bool auto_recovery) { enable_auto_recovery = auto_recovery; }

private:
    // 状态栈和历史
    std::stack<StateNode> state_stack;
    std::vector<StateNode> state_history;
    
    // 转换规则
    std::vector<TransitionRule> transition_rules;
    std::unordered_map<StateType, std::vector<TransitionRule>> rules_by_state;
    
    // 事件处理器
    std::vector<std::shared_ptr<StateEventHandler>> event_handlers;
    
    // 错误信息
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    // 配置选项
    bool strict_mode;
    bool debug_mode;
    bool enable_auto_recovery;
    
    // 内部方法
    
    // 规则匹配
    std::optional<TransitionRule> findMatchingRule(StateType current_state, 
                                                   StateTransitionEvent event, 
                                                   const std::string& input) const;
    
    // 状态验证
    bool isValidTransition(StateType from, StateType to) const;
    bool isStateStackConsistent() const;
    
    // 事件通知
    void notifyStateEnter(const StateNode& state);
    void notifyStateExit(const StateNode& state);
    void notifyStateTransition(const StateNode& from, const StateNode& to);
    void notifyStateError(const StateNode& state, const std::string& error);
    
    // 工具方法
    std::string getStateName(StateType state) const;
    std::string getEventName(StateTransitionEvent event) const;
    
    // 初始化辅助
    void setupCHTLTransitions();
    void setupCHTLJSTransitions();
    void setupStyleTransitions();
    void setupScriptTransitions();
    
    friend class StateGuard;
};

// 上下文协助器
class ContextAssistant {
public:
    ContextAssistant(StateMachine* state_machine);
    ~ContextAssistant();
    
    // 上下文分析
    
    // 分析当前语法上下文
    std::string analyzeCurrentContext() const;
    
    // 检测语法冲突
    std::vector<std::string> detectSyntaxConflicts(const std::string& input) const;
    
    // 推荐语法修正
    std::vector<std::string> suggestSyntaxCorrections(const std::string& input) const;
    
    // 智能语法提示
    
    // 获取当前上下文可用的语法
    std::vector<std::string> getAvailableSyntax() const;
    
    // 获取关键字补全建议
    std::vector<std::string> getKeywordCompletions(const std::string& partial_keyword) const;
    
    // 验证语法片段
    bool validateSyntaxFragment(const std::string& fragment) const;
    
    // 上下文切换辅助
    
    // 预测下一个可能的状态
    std::vector<StateType> predictNextStates(const std::string& look_ahead) const;
    
    // 自动插入必需的语法元素
    std::string autoInsertRequiredSyntax(const std::string& incomplete_syntax) const;
    
    // 检查语法块的完整性
    bool isSyntaxBlockComplete(const std::string& block) const;

private:
    StateMachine* state_machine;
    
    // 语法模式缓存
    std::unordered_map<StateType, std::vector<std::string>> syntax_patterns;
    std::unordered_map<std::string, std::vector<std::string>> keyword_completions;
    
    // 初始化语法模式
    void initializeSyntaxPatterns();
    void initializeKeywordCompletions();
};

// 状态机工具类
class StateMachineUtils {
public:
    // 创建预配置的状态机
    static std::unique_ptr<StateMachine> createCHTLStateMachine();
    static std::unique_ptr<StateMachine> createCHTLJSStateMachine();
    
    // 状态机合并
    static std::unique_ptr<StateMachine> mergeStateMachines(
        const StateMachine& chtl_machine, const StateMachine& chtljs_machine);
    
    // 状态机验证
    static bool validateTransitionRules(const std::vector<TransitionRule>& rules);
    
    // 性能优化
    static void optimizeStateMachine(StateMachine& machine);
    
    // 调试工具
    static std::string generateStateDiagram(const StateMachine& machine);
    static void exportStateMachine(const StateMachine& machine, const std::string& filename);
};

} // namespace Common
} // namespace CHTL