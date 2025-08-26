#ifndef CHTL_STATE_H
#define CHTL_STATE_H

#include <string>
#include <stack>
#include <memory>
#include <unordered_map>

namespace chtl {

// 编译器状态类型
enum class CompilerState {
    INITIAL,              // 初始状态
    IN_ELEMENT,           // 在元素内部
    IN_ATTRIBUTE,         // 在属性定义中
    IN_TEXT,              // 在文本块中
    IN_STYLE,             // 在样式块中
    IN_SCRIPT,            // 在脚本块中
    IN_TEMPLATE,          // 在模板定义中
    IN_CUSTOM,            // 在自定义定义中
    IN_ORIGIN,            // 在原始嵌入块中
    IN_IMPORT,            // 在导入语句中
    IN_NAMESPACE,         // 在命名空间中
    IN_CONFIGURATION,     // 在配置块中
    IN_COMMENT,           // 在注释中
    IN_STRING,            // 在字符串中
    ERROR                 // 错误状态
};

// 状态上下文信息
struct StateContext {
    CompilerState state;
    std::string name;           // 状态名称（如元素名、模板名等）
    std::string type;           // 状态类型（如@Style、@Element等）
    size_t startLine;
    size_t startColumn;
    int braceDepth = 0;         // 大括号深度
    bool expectingValue = false; // 是否期待值（用于属性）
    
    StateContext(CompilerState s = CompilerState::INITIAL) 
        : state(s), startLine(0), startColumn(0) {}
};

// RAII状态管理器
class StateGuard {
public:
    StateGuard(class CHTLStateManager* manager, CompilerState newState);
    ~StateGuard();
    
    // 禁止复制
    StateGuard(const StateGuard&) = delete;
    StateGuard& operator=(const StateGuard&) = delete;
    
    // 允许移动
    StateGuard(StateGuard&& other) noexcept;
    StateGuard& operator=(StateGuard&& other) noexcept;
    
private:
    class CHTLStateManager* manager;
    bool active;
};

// 状态管理器
class CHTLStateManager {
public:
    CHTLStateManager();
    ~CHTLStateManager();
    
    // 状态操作
    void pushState(CompilerState state, const std::string& name = "");
    void popState();
    CompilerState getCurrentState() const;
    StateContext getCurrentContext() const;
    
    // RAII状态管理
    [[nodiscard]] StateGuard enterState(CompilerState state, const std::string& name = "");
    
    // 状态查询
    bool isInState(CompilerState state) const;
    bool isInAnyState(std::initializer_list<CompilerState> states) const;
    int getStateDepth() const;
    
    // 上下文信息管理
    void setContextName(const std::string& name);
    void setContextType(const std::string& type);
    void incrementBraceDepth();
    void decrementBraceDepth();
    int getBraceDepth() const;
    void setExpectingValue(bool expecting);
    bool isExpectingValue() const;
    
    // 状态验证
    bool canTransition(CompilerState from, CompilerState to) const;
    bool isValidStateSequence() const;
    
    // 错误处理
    void enterErrorState(const std::string& reason);
    bool isInErrorState() const;
    std::string getErrorReason() const;
    
    // 重置
    void reset();
    
    // 调试
    std::string getStateStackTrace() const;
    void dumpStateStack() const;
    
private:
    std::stack<StateContext> stateStack;
    std::string errorReason;
    
    // 状态转换规则
    static const std::unordered_map<CompilerState, std::vector<CompilerState>> validTransitions;
};

// 约束状态管理
enum class ConstraintScope {
    GLOBAL_STYLE,        // 全局样式块
    LOCAL_STYLE,         // 局部样式块
    GLOBAL_SCRIPT,       // 全局脚本块
    LOCAL_SCRIPT,        // 局部脚本块
    ELEMENT,             // 元素内部
    NAMESPACE,           // 命名空间内部
    NONE                 // 无约束
};

// 约束管理器
class ConstraintManager {
public:
    ConstraintManager();
    
    // 约束作用域管理
    void enterConstraintScope(ConstraintScope scope);
    void exitConstraintScope();
    ConstraintScope getCurrentScope() const;
    
    // 约束检查
    bool isAllowed(const std::string& feature) const;
    bool isTemplateVarAllowed() const;
    bool isCustomVarAllowed() const;
    bool isStyleGroupAllowed() const;
    bool isDeleteAllowed() const;
    bool isInheritAllowed() const;
    bool isCHTLSyntaxAllowed() const;
    bool isRawEmbedAllowed() const;
    
    // 特定约束检查
    bool canUseSelector(const std::string& selector) const;
    bool canAutoAddClass() const;
    bool canAutoAddId() const;
    
private:
    std::stack<ConstraintScope> scopeStack;
    
    // 根据作用域获取允许的特性
    std::vector<std::string> getAllowedFeatures(ConstraintScope scope) const;
};

} // namespace chtl

#endif // CHTL_STATE_H