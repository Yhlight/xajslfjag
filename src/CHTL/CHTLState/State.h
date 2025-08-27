#pragma once

#include <stack>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace CHTL {

/// CHTL解析状态枚举
enum class ParseState {
    INITIAL,                // 初始状态
    ELEMENT,                // 元素解析状态
    ELEMENT_CONTENT,        // 元素内容状态
    TEXT_CONTENT,           // 文本内容状态
    STYLE_BLOCK,           // 样式块状态
    SCRIPT_BLOCK,          // 脚本块状态
    TEMPLATE_DEFINITION,   // 模板定义状态
    CUSTOM_DEFINITION,     // 自定义定义状态
    IMPORT_STATEMENT,      // 导入语句状态
    NAMESPACE_DEFINITION,  // 命名空间定义状态
    CONFIGURATION_BLOCK,   // 配置块状态
    ORIGIN_BLOCK,          // 原始嵌入块状态
    COMMENT_BLOCK,         // 注释块状态
    ATTRIBUTE_LIST,        // 属性列表状态
    ATTRIBUTE_VALUE,       // 属性值状态
    USE_STATEMENT,         // use语句状态
    EXCEPT_CLAUSE,         // except子句状态
    INHERITANCE_CLAUSE,    // 继承子句状态
    DELETE_CLAUSE,         // 删除子句状态
    INSERT_CLAUSE,         // 插入子句状态
    BRACKET_EXPRESSION,    // 括号表达式状态
    SELECTOR_CONTEXT,      // 选择器上下文状态
    VARIABLE_REFERENCE,    // 变量引用状态
    ERROR_RECOVERY         // 错误恢复状态
};

/// CHTL解析上下文类型
enum class ContextType {
    GLOBAL,                // 全局上下文
    ELEMENT,               // 元素上下文
    TEMPLATE,              // 模板上下文
    CUSTOM,                // 自定义上下文
    NAMESPACE,             // 命名空间上下文
    CONFIGURATION,         // 配置上下文
    STYLE,                 // 样式上下文
    SCRIPT,                // 脚本上下文
    IMPORT,                // 导入上下文
    ORIGIN                 // 原始嵌入上下文
};

/// CHTL状态信息结构
struct StateInfo {
    ParseState state;
    ContextType context;
    std::string currentElement;          // 当前处理的元素
    std::string currentNamespace;        // 当前命名空间
    int braceDepth;                     // 大括号深度
    int bracketDepth;                   // 方括号深度
    int parenDepth;                     // 圆括号深度
    bool inQuotes;                      // 是否在引号内
    bool inComment;                     // 是否在注释内
    size_t lineNumber;                  // 当前行号
    size_t columnNumber;                // 当前列号
    
    StateInfo(ParseState s = ParseState::INITIAL, ContextType c = ContextType::GLOBAL)
        : state(s), context(c), braceDepth(0), bracketDepth(0), parenDepth(0)
        , inQuotes(false), inComment(false), lineNumber(1), columnNumber(1) {}
};

/// CHTL状态管理器类，使用RAII模式自动管理状态
class StateManager {
private:
    std::stack<StateInfo> stateStack;
    std::unordered_map<std::string, std::string> stateData; // 状态相关数据

public:
    StateManager();
    ~StateManager();
    
    /// 获取当前状态
    const StateInfo& GetCurrentState() const;
    
    /// 获取当前解析状态
    ParseState GetParseState() const;
    
    /// 获取当前上下文类型
    ContextType GetContextType() const;
    
    /// 进入新状态（自动保存当前状态）
    void PushState(ParseState newState, ContextType newContext = ContextType::GLOBAL);
    
    /// 退出当前状态（恢复上一个状态）
    void PopState();
    
    /// 更新当前状态信息
    void UpdateState(const StateInfo& info);
    
    /// 设置当前元素
    void SetCurrentElement(const std::string& element);
    
    /// 设置当前命名空间
    void SetCurrentNamespace(const std::string& ns);
    
    /// 增加大括号深度
    void IncreaseBraceDepth();
    
    /// 减少大括号深度
    void DecreaseBraceDepth();
    
    /// 增加方括号深度
    void IncreaseBracketDepth();
    
    /// 减少方括号深度
    void DecreaseBracketDepth();
    
    /// 增加圆括号深度
    void IncreaseParenDepth();
    
    /// 减少圆括号深度
    void DecreaseParenDepth();
    
    /// 设置引号状态
    void SetInQuotes(bool inQuotes);
    
    /// 设置注释状态
    void SetInComment(bool inComment);
    
    /// 更新位置信息
    void UpdatePosition(size_t line, size_t column);
    
    /// 检查状态是否平衡（括号匹配等）
    bool IsBalanced() const;
    
    /// 获取状态栈深度
    size_t GetStackDepth() const;
    
    /// 重置状态管理器
    void Reset();
    
    /// 设置状态数据
    void SetStateData(const std::string& key, const std::string& value);
    
    /// 获取状态数据
    std::string GetStateData(const std::string& key) const;
    
    /// 清除状态数据
    void ClearStateData();
    
    /// 获取状态描述（用于调试）
    std::string GetStateDescription() const;
    
    /// 检查是否可以转换到指定状态
    bool CanTransitionTo(ParseState newState) const;

private:
    /// 验证状态转换的合法性
    bool IsValidTransition(ParseState from, ParseState to) const;
    
    /// 获取状态名称
    std::string GetStateName(ParseState state) const;
    
    /// 获取上下文名称
    std::string GetContextName(ContextType context) const;
};

/// RAII状态保护器，自动管理状态进入和退出
class StateGuard {
private:
    StateManager& manager;
    bool shouldPop;
    
public:
    StateGuard(StateManager& mgr, ParseState state, ContextType context = ContextType::GLOBAL);
    ~StateGuard();
    
    // 禁止拷贝
    StateGuard(const StateGuard&) = delete;
    StateGuard& operator=(const StateGuard&) = delete;
    
    // 允许移动
    StateGuard(StateGuard&& other) noexcept;
    StateGuard& operator=(StateGuard&& other) noexcept;
    
    /// 提前释放状态（不等析构函数）
    void Release();
};

} // namespace CHTL