#ifndef CHTL_STATE_H
#define CHTL_STATE_H

#include <string>
#include <vector>
#include <stack>
#include <unordered_map>
#include <memory>

namespace CHTL {

// 解析状态枚举
enum class ParseState {
    GLOBAL,                 // 全局状态
    USE_DECLARATION,        // use声明
    NAMESPACE_BLOCK,        // 命名空间块
    CONFIGURATION_BLOCK,    // 配置块
    TEMPLATE_DEFINITION,    // 模板定义
    CUSTOM_DEFINITION,      // 自定义定义
    ORIGIN_DEFINITION,      // 原始嵌入定义
    IMPORT_STATEMENT,       // 导入语句
    HTML_ELEMENT,           // HTML元素
    ELEMENT_CONTENT,        // 元素内容
    TEXT_BLOCK,             // 文本块
    STYLE_BLOCK,            // 样式块
    SCRIPT_BLOCK,           // 脚本块
    LOCAL_STYLE_BLOCK,      // 局部样式块
    LOCAL_SCRIPT_BLOCK,     // 局部脚本块
    ATTRIBUTE_LIST,         // 属性列表
    STYLE_RULE,             // 样式规则
    TEMPLATE_USAGE,         // 模板使用
    CUSTOM_USAGE,           // 自定义使用
    SELECTOR_CONTEXT,       // 选择器上下文
    VALUE_CONTEXT,          // 值上下文
    SPECIALIZATION_CONTEXT, // 特例化上下文
    INHERITANCE_CONTEXT,    // 继承上下文
    DELETE_CONTEXT,         // 删除上下文
    INSERT_CONTEXT,         // 插入上下文
    CONSTRAINT_CONTEXT,     // 约束上下文
    VARIABLE_REFERENCE,     // 变量引用
    PATH_CONTEXT,           // 路径上下文
    ERROR_STATE             // 错误状态
};

// 状态管理器类 - RAII管理
class StateManager {
private:
    std::stack<ParseState> state_stack;
    std::unordered_map<ParseState, std::string> state_names;
    
    // 状态转换规则
    std::unordered_map<ParseState, std::vector<ParseState>> valid_transitions;
    
    // 当前上下文信息
    std::string current_namespace;
    std::string current_element;
    std::string current_template;
    std::string current_custom;
    
    // 错误处理
    std::vector<std::string> errors;
    
    void initializeStateNames();
    void initializeTransitions();
    
public:
    StateManager();
    ~StateManager() = default;
    
    // 状态管理
    void pushState(ParseState newState);
    bool popState();
    ParseState getCurrentState() const;
    ParseState getPreviousState() const;
    bool isValidTransition(ParseState from, ParseState to) const;
    
    // 上下文管理
    void setCurrentNamespace(const std::string& ns);
    void setCurrentElement(const std::string& element);
    void setCurrentTemplate(const std::string& template_name);
    void setCurrentCustom(const std::string& custom_name);
    
    std::string getCurrentNamespace() const;
    std::string getCurrentElement() const;
    std::string getCurrentTemplate() const;
    std::string getCurrentCustom() const;
    
    // 状态查询
    bool isInGlobalContext() const;
    bool isInElementContext() const;
    bool isInStyleContext() const;
    bool isInScriptContext() const;
    bool isInTemplateContext() const;
    bool isInCustomContext() const;
    bool isInConfigurationContext() const;
    bool isInNamespaceContext() const;
    
    // 嵌套级别检查
    size_t getStateDepth() const;
    bool isNestedState(ParseState state) const;
    
    // 错误处理
    void addError(const std::string& message);
    std::vector<std::string> getErrors() const;
    bool hasErrors() const;
    void clearErrors();
    
    // 状态名称获取
    std::string getStateName(ParseState state) const;
    std::string getCurrentStateName() const;
    
    // 调试支持
    void printStateStack() const;
    void printCurrentContext() const;
    
    // 清理
    void reset();
    void clear();
};

// RAII状态保护类
class StateGuard {
private:
    StateManager* manager;
    ParseState saved_state;
    bool should_restore;
    
public:
    StateGuard(StateManager* mgr, ParseState newState);
    ~StateGuard();
    
    // 禁用拷贝和赋值
    StateGuard(const StateGuard&) = delete;
    StateGuard& operator=(const StateGuard&) = delete;
    
    // 允许移动
    StateGuard(StateGuard&& other) noexcept;
    StateGuard& operator=(StateGuard&& other) noexcept;
    
    // 手动释放保护
    void release();
    
    // 检查是否有效
    bool isValid() const;
};

// 上下文保护类
class ContextGuard {
private:
    StateManager* manager;
    std::string saved_namespace;
    std::string saved_element;
    std::string saved_template;
    std::string saved_custom;
    bool should_restore;
    
public:
    ContextGuard(StateManager* mgr);
    ~ContextGuard();
    
    // 禁用拷贝和赋值
    ContextGuard(const ContextGuard&) = delete;
    ContextGuard& operator=(const ContextGuard&) = delete;
    
    // 允许移动
    ContextGuard(ContextGuard&& other) noexcept;
    ContextGuard& operator=(ContextGuard&& other) noexcept;
    
    // 手动释放保护
    void release();
    
    // 检查是否有效
    bool isValid() const;
};

// 便利宏定义
#define STATE_GUARD(manager, state) \
    StateGuard __state_guard(manager, state)

#define CONTEXT_GUARD(manager) \
    ContextGuard __context_guard(manager)

} // namespace CHTL

#endif // CHTL_STATE_H