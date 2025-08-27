#pragma once
#include <stack>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace CHTL {

/**
 * CHTL解析状态枚举
 * 表示当前解析器所处的状态
 */
enum class ParseState {
    INITIAL,              // 初始状态
    IN_ELEMENT,           // 在元素内部
    IN_TEMPLATE,          // 在模板内部
    IN_CUSTOM,            // 在自定义内部
    IN_ORIGIN,            // 在原始嵌入内部
    IN_IMPORT,            // 在导入内部
    IN_CONFIGURATION,     // 在配置内部
    IN_NAMESPACE,         // 在命名空间内部
    IN_STYLE_BLOCK,       // 在样式块内部
    IN_SCRIPT_BLOCK,      // 在脚本块内部
    IN_TEXT_BLOCK,        // 在文本块内部
    IN_COMMENT,           // 在注释内部
    IN_ATTRIBUTE,         // 在属性内部
    IN_VALUE,             // 在值内部
    IN_SELECTOR,          // 在选择器内部
    IN_PROPERTY,          // 在属性值内部
    ERROR_STATE           // 错误状态
};

/**
 * CHTL上下文类型
 * 表示当前上下文的类型
 */
enum class ContextType {
    GLOBAL,               // 全局上下文
    ELEMENT,              // 元素上下文
    TEMPLATE_STYLE,       // 样式模板上下文
    TEMPLATE_ELEMENT,     // 元素模板上下文
    TEMPLATE_VAR,         // 变量模板上下文
    CUSTOM_STYLE,         // 自定义样式上下文
    CUSTOM_ELEMENT,       // 自定义元素上下文
    CUSTOM_VAR,           // 自定义变量上下文
    ORIGIN,               // 原始嵌入上下文
    IMPORT,               // 导入上下文
    CONFIGURATION,        // 配置上下文
    NAMESPACE,            // 命名空间上下文
    LOCAL_STYLE,          // 局部样式上下文
    LOCAL_SCRIPT,         // 局部脚本上下文
    TEXT,                 // 文本上下文
    COMMENT               // 注释上下文
};

/**
 * 状态信息结构
 */
struct StateInfo {
    ParseState state;
    ContextType context;
    std::string currentElement;
    std::string currentNamespace;
    std::unordered_map<std::string, std::string> attributes;
    std::vector<std::string> constraintStack;  // 约束栈
    bool isInSpecialization;  // 是否在特例化中
    size_t nestingLevel;      // 嵌套级别
    
    StateInfo() : state(ParseState::INITIAL), context(ContextType::GLOBAL), 
                  isInSpecialization(false), nestingLevel(0) {}
};

/**
 * CHTL状态管理器
 * 使用RAII管理解析状态和上下文
 */
class CHTLState {
private:
    std::stack<StateInfo> stateStack;
    StateInfo currentState;
    std::unordered_map<std::string, bool> featureFlags;
    std::vector<std::string> errorStack;
    
    // 状态验证
    bool isValidTransition(ParseState from, ParseState to) const;
    bool isValidContext(ContextType context, ParseState state) const;
    
    // 约束检查
    bool checkConstraints(const std::string& operation) const;
    void applyConstraints(const std::vector<std::string>& constraints);

public:
    CHTLState();
    ~CHTLState();
    
    // 状态管理
    void pushState(ParseState state, ContextType context = ContextType::GLOBAL);
    void popState();
    void setState(ParseState state);
    void setContext(ContextType context);
    
    // 状态查询
    ParseState getCurrentState() const;
    ContextType getCurrentContext() const;
    const StateInfo& getCurrentStateInfo() const;
    size_t getStackDepth() const;
    bool isInState(ParseState state) const;
    bool isInContext(ContextType context) const;
    
    // 元素和命名空间管理
    void setCurrentElement(const std::string& element);
    void setCurrentNamespace(const std::string& namespaceName);
    std::string getCurrentElement() const;
    std::string getCurrentNamespace() const;
    
    // 属性管理
    void setAttribute(const std::string& name, const std::string& value);
    std::string getAttribute(const std::string& name) const;
    bool hasAttribute(const std::string& name) const;
    void clearAttributes();
    
    // 约束管理
    void pushConstraint(const std::string& constraint);
    void popConstraint();
    bool isConstraintActive(const std::string& constraint) const;
    
    // 特例化管理
    void enterSpecialization();
    void exitSpecialization();
    bool isInSpecialization() const;
    
    // 嵌套级别管理
    void incrementNesting();
    void decrementNesting();
    size_t getNestingLevel() const;
    
    // 特性标志管理
    void setFeatureFlag(const std::string& flag, bool value);
    bool getFeatureFlag(const std::string& flag) const;
    
    // 错误处理
    void reportError(const std::string& error);
    std::vector<std::string> getErrors() const;
    void clearErrors();
    bool hasErrors() const;
    
    // 验证
    bool canEnterState(ParseState state) const;
    bool canExitState() const;
    bool validateCurrentState() const;
    
    // 调试
    void dumpState() const;
    std::string getStateDescription() const;
    void reset();
};

/**
 * RAII状态管理器
 * 自动管理状态进入和退出
 */
class StateGuard {
private:
    CHTLState& state;
    bool shouldPop;

public:
    StateGuard(CHTLState& state, ParseState newState, ContextType newContext = ContextType::GLOBAL);
    ~StateGuard();
    
    // 禁止复制
    StateGuard(const StateGuard&) = delete;
    StateGuard& operator=(const StateGuard&) = delete;
    
    // 允许移动
    StateGuard(StateGuard&& other) noexcept;
    StateGuard& operator=(StateGuard&& other) noexcept;
    
    // 手动控制
    void release();
};

/**
 * 上下文管理器
 * 管理特定类型的上下文切换
 */
class ContextManager {
private:
    CHTLState& state;
    std::string savedElement;
    std::string savedNamespace;

public:
    ContextManager(CHTLState& state, ContextType context);
    ~ContextManager();
    
    void setElement(const std::string& element);
    void setNamespace(const std::string& namespaceName);
    
    // 禁止复制和移动
    ContextManager(const ContextManager&) = delete;
    ContextManager& operator=(const ContextManager&) = delete;
    ContextManager(ContextManager&&) = delete;
    ContextManager& operator=(ContextManager&&) = delete;
};

}