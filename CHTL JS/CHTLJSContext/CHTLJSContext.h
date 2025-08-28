#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include "../CHTLJSState/CHTLJSState.h"

namespace CHTLJS {

// CHTL JS上下文管理器
class CHTLJSContext {
public:
    CHTLJSContext();
    ~CHTLJSContext() = default;
    
    // 状态管理
    void setState(CHTLJSCompilerState state);
    CHTLJSCompilerState getCurrentState() const;
    bool canTransitionTo(CHTLJSCompilerState newState) const;
    bool transitionTo(CHTLJSCompilerState newState, const std::string& trigger = "");
    
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
    bool isInScriptBlock() const;
    bool isInModuleBlock() const;
    bool isInVirtualObjectBlock() const;
    bool isInListenBlock() const;
    bool isInDelegateBlock() const;
    bool isInAnimateBlock() const;
    bool isInINeverAwayBlock() const;
    bool isInPrintMyloveBlock() const;
    
    // 错误处理
    void setError(const std::string& error, size_t line = 0, size_t column = 0);
    bool hasError() const;
    const std::vector<std::string>& getErrors() const;
    void clearErrors();
    
    // 状态历史
    void addStateTransition(const StateTransition& transition);
    const std::vector<StateTransition>& getStateHistory() const;
    
    // 重置上下文
    void reset();
    
    // 调试信息
    std::string getContextInfo() const;
    std::string getStateInfo() const;
    
    // 获取内部组件
    std::shared_ptr<CHTLJSState> getState() const { return state_; }
    
private:
    std::shared_ptr<CHTLJSState> state_;
    
    // 上下文验证
    bool validateContext() const;
    void updateContext();
    
    // 状态转换日志
    void logStateTransition(CHTLJSCompilerState from, CHTLJSCompilerState to, const std::string& trigger);
};

} // namespace CHTLJS