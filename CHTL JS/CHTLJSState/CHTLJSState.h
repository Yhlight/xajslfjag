#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <stack>

namespace CHTLJS {

// CHTL JS编译器状态枚举
enum class CHTLJSCompilerState {
    INITIAL,                    // 初始状态
    PARSING_MODULE,            // 解析模块导入
    PARSING_VIRTUAL_OBJECT,    // 解析虚对象
    PARSING_ENHANCED_SELECTOR, // 解析增强选择器
    PARSING_EVENT_BINDING,     // 解析事件绑定
    PARSING_LISTEN_BLOCK,      // 解析监听块
    PARSING_DELEGATE_BLOCK,    // 解析委托块
    PARSING_ANIMATE_BLOCK,     // 解析动画块
    PARSING_I_NEVER_AWAY,      // 解析iNeverAway
    PARSING_PRINT_MY_LOVE,     // 解析printMylove
    PARSING_SCRIPT_BLOCK,      // 解析脚本块
    PARSING_FUNCTION,          // 解析函数
    PARSING_OBJECT,            // 解析对象
    PARSING_ARRAY,             // 解析数组
    PARSING_COMMENT,           // 解析注释
    ERROR                      // 错误状态
};

// 解析上下文
struct ParseContext {
    std::string currentFunction;
    std::string currentObject;
    std::string currentArray;
    std::string currentModule;
    std::string currentVirtualObject;
    bool inScriptBlock;
    bool inModuleBlock;
    bool inVirtualObjectBlock;
    bool inListenBlock;
    bool inDelegateBlock;
    bool inAnimateBlock;
    bool inINeverAwayBlock;
    bool inPrintMyloveBlock;
    
    ParseContext() : inScriptBlock(false), inModuleBlock(false), 
                     inVirtualObjectBlock(false), inListenBlock(false), 
                     inDelegateBlock(false), inAnimateBlock(false),
                     inINeverAwayBlock(false), inPrintMyloveBlock(false) {}
};

// 状态转换信息
struct StateTransition {
    CHTLJSCompilerState fromState;
    CHTLJSCompilerState toState;
    std::string trigger;
    std::string condition;
    size_t line;
    size_t column;
    
    StateTransition(CHTLJSCompilerState from, CHTLJSCompilerState to, 
                   const std::string& trigger = "", const std::string& condition = "",
                   size_t line = 0, size_t column = 0)
        : fromState(from), toState(to), trigger(trigger), condition(condition), 
          line(line), column(column) {}
};

// CHTL JS状态管理器
class CHTLJSState {
public:
    CHTLJSState();
    ~CHTLJSState() = default;
    
    // 状态管理
    void setState(CHTLJSCompilerState state);
    CHTLJSCompilerState getCurrentState() const { return currentState_; }
    CHTLJSCompilerState getPreviousState() const { return previousState_; }
    
    // 状态转换
    bool transitionTo(CHTLJSCompilerState newState, const std::string& trigger = "");
    bool canTransitionTo(CHTLJSCompilerState newState) const;
    
    // 上下文管理
    void pushContext();
    void popContext();
    ParseContext& getCurrentContext();
    const ParseContext& getCurrentContext() const;
    
    // 函数栈管理
    void pushFunction(const std::string& function);
    void popFunction();
    std::string getCurrentFunction() const;
    std::vector<std::string> getFunctionStack() const;
    
    // 对象栈管理
    void pushObject(const std::string& object);
    void popObject();
    std::string getCurrentObject() const;
    std::vector<std::string> getObjectStack() const;
    
    // 模块管理
    void setCurrentModule(const std::string& module);
    std::string getCurrentModule() const;
    
    // 虚对象管理
    void setCurrentVirtualObject(const std::string& virtualObject);
    std::string getCurrentVirtualObject() const;
    
    // 块状态管理
    void enterScriptBlock();
    void exitScriptBlock();
    void enterModuleBlock();
    void exitModuleBlock();
    void enterVirtualObjectBlock();
    void exitVirtualObjectBlock();
    void enterListenBlock();
    void exitListenBlock();
    void enterDelegateBlock();
    void exitDelegateBlock();
    void enterAnimateBlock();
    void exitAnimateBlock();
    void enterINeverAwayBlock();
    void exitINeverAwayBlock();
    void enterPrintMyloveBlock();
    void exitPrintMyloveBlock();
    
    // 状态查询
    bool isInScriptBlock() const { return currentContext_.inScriptBlock; }
    bool isInModuleBlock() const { return currentContext_.inModuleBlock; }
    bool isInVirtualObjectBlock() const { return currentContext_.inVirtualObjectBlock; }
    bool isInListenBlock() const { return currentContext_.inListenBlock; }
    bool isInDelegateBlock() const { return currentContext_.inDelegateBlock; }
    bool isInAnimateBlock() const { return currentContext_.inAnimateBlock; }
    bool isInINeverAwayBlock() const { return currentContext_.inINeverAwayBlock; }
    bool isInPrintMyloveBlock() const { return currentContext_.inPrintMyloveBlock; }
    
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
    CHTLJSCompilerState currentState_;
    CHTLJSCompilerState previousState_;
    ParseContext currentContext_;
    std::stack<ParseContext> contextStack_;
    std::vector<std::string> functionStack_;
    std::vector<std::string> objectStack_;
    std::vector<StateTransition> stateHistory_;
    std::vector<std::string> errors_;
    
    // 状态转换规则
    bool isValidTransition(CHTLJSCompilerState from, CHTLJSCompilerState to) const;
    void logStateTransition(CHTLJSCompilerState from, CHTLJSCompilerState to, const std::string& trigger);
    
    // 上下文验证
    bool validateContext() const;
    void updateContext();
};

} // namespace CHTLJS