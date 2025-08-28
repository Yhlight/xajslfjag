#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <stack>

namespace CHTL {

// CHTL编译器状态枚举
enum class CHTLCompilerState {
    INITIAL,                    // 初始状态
    PARSING_IMPORT,            // 解析导入语句
    PARSING_USE,               // 解析use语句
    PARSING_CONFIGURATION,     // 解析配置组
    PARSING_NAMESPACE,         // 解析命名空间
    PARSING_TEMPLATE,          // 解析模板
    PARSING_CUSTOM,            // 解析自定义
    PARSING_ORIGIN,            // 解析原始嵌入
    PARSING_ELEMENT,           // 解析元素
    PARSING_ATTRIBUTE,         // 解析属性
    PARSING_STYLE_BLOCK,       // 解析样式块
    PARSING_SCRIPT_BLOCK,      // 解析脚本块
    PARSING_TEXT,              // 解析文本
    PARSING_COMMENT,           // 解析注释
    PARSING_CONSTRAINT,        // 解析约束
    ERROR                      // 错误状态
};

// 解析上下文
struct ParseContext {
    std::string currentElement;
    std::string currentNamespace;
    std::string currentTemplate;
    std::string currentCustom;
    std::string currentOrigin;
    bool inStyleBlock;
    bool inScriptBlock;
    bool inTemplateBlock;
    bool inCustomBlock;
    bool inOriginBlock;
    bool inConstraintBlock;
    
    ParseContext() : inStyleBlock(false), inScriptBlock(false), 
                     inTemplateBlock(false), inCustomBlock(false), 
                     inOriginBlock(false), inConstraintBlock(false) {}
};

// 状态转换信息
struct StateTransition {
    CHTLCompilerState fromState;
    CHTLCompilerState toState;
    std::string trigger;
    std::string condition;
    size_t line;
    size_t column;
    
    StateTransition(CHTLCompilerState from, CHTLCompilerState to, 
                   const std::string& trigger = "", const std::string& condition = "",
                   size_t line = 0, size_t column = 0)
        : fromState(from), toState(to), trigger(trigger), condition(condition), 
          line(line), column(column) {}
};

// CHTL状态管理器
class CHTLState {
public:
    CHTLState();
    ~CHTLState() = default;
    
    // 状态管理
    void setState(CHTLCompilerState state);
    CHTLCompilerState getCurrentState() const { return currentState_; }
    CHTLCompilerState getPreviousState() const { return previousState_; }
    
    // 状态转换
    bool transitionTo(CHTLCompilerState newState, const std::string& trigger = "");
    bool canTransitionTo(CHTLCompilerState newState) const;
    
    // 上下文管理
    void pushContext();
    void popContext();
    ParseContext& getCurrentContext();
    const ParseContext& getCurrentContext() const;
    
    // 元素栈管理
    void pushElement(const std::string& element);
    void popElement();
    std::string getCurrentElement() const;
    std::vector<std::string> getElementStack() const;
    
    // 命名空间管理
    void setCurrentNamespace(const std::string& namespace_);
    std::string getCurrentNamespace() const;
    
    // 模板管理
    void setCurrentTemplate(const std::string& template_);
    std::string getCurrentTemplate() const;
    
    // 自定义管理
    void setCurrentCustom(const std::string& custom);
    std::string getCurrentCustom() const;
    
    // 原始嵌入管理
    void setCurrentOrigin(const std::string& origin);
    std::string getCurrentOrigin() const;
    
    // 块状态管理
    void enterStyleBlock();
    void exitStyleBlock();
    void enterScriptBlock();
    void exitScriptBlock();
    void enterTemplateBlock();
    void exitTemplateBlock();
    void enterCustomBlock();
    void exitCustomBlock();
    void enterOriginBlock();
    void exitOriginBlock();
    void enterConstraintBlock();
    void exitConstraintBlock();
    
    // 状态查询
    bool isInStyleBlock() const { return currentContext_.inStyleBlock; }
    bool isInScriptBlock() const { return currentContext_.inScriptBlock; }
    bool isInTemplateBlock() const { return currentContext_.inTemplateBlock; }
    bool isInCustomBlock() const { return currentContext_.inCustomBlock; }
    bool isInOriginBlock() const { return currentContext_.inOriginBlock; }
    bool isInConstraintBlock() const { return currentContext_.inConstraintBlock; }
    
    // 状态历史
    void addStateTransition(const StateTransition& transition);
    const std::vector<StateTransition>& getStateHistory() const { return stateHistory_; }
    
    // 错误处理
    void setError(const std::string& error, size_t line = 0, size_t column = 0);
    bool hasError() const { return !errors_.empty(); }
    const std::vector<std::string>& getErrors() const { return errors_; }
    void clearErrors();
    
    // 重置状态
    void reset();
    
    // 调试信息
    std::string getStateInfo() const;
    std::string getContextInfo() const;
    
private:
    CHTLCompilerState currentState_;
    CHTLCompilerState previousState_;
    ParseContext currentContext_;
    std::stack<ParseContext> contextStack_;
    std::vector<std::string> elementStack_;
    std::vector<StateTransition> stateHistory_;
    std::vector<std::string> errors_;
    
    // 状态转换规则
    bool isValidTransition(CHTLCompilerState from, CHTLCompilerState to) const;
    void logStateTransition(CHTLCompilerState from, CHTLCompilerState to, const std::string& trigger);
    
    // 上下文验证
    bool validateContext() const;
    void updateContext();
};

} // namespace CHTL