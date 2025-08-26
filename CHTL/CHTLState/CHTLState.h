#ifndef CHTL_STATE_H
#define CHTL_STATE_H

#include <string>
#include <stack>
#include <memory>
#include <vector>

namespace CHTL {

// 编译器状态类型
enum class StateType {
    // 顶层状态
    GLOBAL,                  // 全局作用域
    NAMESPACE,               // 命名空间内
    
    // 元素状态
    ELEMENT,                 // HTML元素内
    TEXT_NODE,               // text节点内
    
    // 样式状态
    STYLE_GLOBAL,            // 全局style块
    STYLE_LOCAL,             // 局部style块
    STYLE_SELECTOR,          // 选择器内
    STYLE_RULE,              // 样式规则内
    STYLE_GROUP,             // 样式组内
    
    // 脚本状态
    SCRIPT_GLOBAL,           // 全局script块
    SCRIPT_LOCAL,            // 局部script块
    
    // 导入状态
    IMPORT_DECLARATION,      // [Import]声明内
    CONFIGURATION,           // [Configuration]块内
    
    // 特殊状态
    ORIGIN_EMBED,            // [Origin]原始嵌入内
    COMMENT,                 // 注释内
    STRING_LITERAL,          // 字符串字面量内
    
    // 变量和模板状态
    TEMPLATE_VAR,            // 模板变量内
    CUSTOM_VAR,              // 自定义变量内
    VARIABLE_GROUP,          // 变量组内
    
    // CHTL JS状态
    CHTL_JS_EXPRESSION,      // {{}}表达式内
    CHTL_JS_SELECTOR,        // {{selector}}内
    CHTL_JS_EVENT            // 事件处理内
};

// 状态信息
struct StateInfo {
    StateType type;
    std::string name;        // 状态名称（如元素名、选择器名等）
    std::string extra_info;  // 额外信息
    int start_line;
    int start_column;
    int depth;              // 嵌套深度
    
    StateInfo(StateType t, const std::string& n = "", int line = 0, int col = 0)
        : type(t), name(n), start_line(line), start_column(col), depth(0) {}
};

// RAII状态管理器
class StateGuard {
private:
    class CHTLState* state;
    bool released;
    
public:
    StateGuard(CHTLState* s, StateType type, const std::string& name = "");
    ~StateGuard();
    
    // 禁止拷贝
    StateGuard(const StateGuard&) = delete;
    StateGuard& operator=(const StateGuard&) = delete;
    
    // 允许移动
    StateGuard(StateGuard&& other) noexcept;
    StateGuard& operator=(StateGuard&& other) noexcept;
    
    void release() { released = true; }
};

// 状态管理器
class CHTLState {
private:
    std::stack<StateInfo> state_stack;
    std::vector<StateInfo> state_history;  // 状态历史记录
    int current_line;
    int current_column;
    
public:
    CHTLState();
    ~CHTLState() = default;
    
    // 状态操作
    void pushState(StateType type, const std::string& name = "");
    void popState();
    StateType getCurrentState() const;
    StateInfo getCurrentStateInfo() const;
    
    // RAII状态管理
    [[nodiscard]] StateGuard enterState(StateType type, const std::string& name = "");
    
    // 状态查询
    bool isInState(StateType type) const;
    bool isInAnyState(std::initializer_list<StateType> types) const;
    int getStateDepth(StateType type) const;
    std::vector<StateType> getStateStack() const;
    
    // 特殊状态查询
    bool isInGlobalScope() const;
    bool isInNamespace() const;
    bool isInElement() const;
    bool isInStyle() const;
    bool isInLocalStyle() const;
    bool isInGlobalStyle() const;
    bool isInScript() const;
    bool isInLocalScript() const;
    bool isInGlobalScript() const;
    bool isInComment() const;
    bool isInString() const;
    bool isInCHTLJS() const;
    
    // 获取当前元素/命名空间等的名称
    std::string getCurrentElementName() const;
    std::string getCurrentNamespaceName() const;
    std::string getCurrentSelectorName() const;
    
    // 位置更新
    void updatePosition(int line, int column);
    int getCurrentLine() const { return current_line; }
    int getCurrentColumn() const { return current_column; }
    
    // 状态验证
    bool canUseTemplateVar() const;
    bool canUseCustomVar() const;
    bool canUseStyleGroup() const;
    bool canUseDelete() const;
    bool canUseInheritance() const;
    bool canUseCHTLJSSyntax() const;
    bool canUseOriginEmbed() const;
    bool canUseGeneratorComment() const;
    
    // 清理
    void clear();
    
    // 调试
    void dumpStack() const;
    std::string getStateString() const;
    
    friend class StateGuard;
};

} // namespace CHTL

#endif // CHTL_STATE_H