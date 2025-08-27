#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <stack>

namespace CHTLJS {

// 作用域类型
enum class ScopeType {
    GLOBAL,        // 全局作用域
    FUNCTION,      // 函数作用域
    BLOCK,         // 块作用域
    OBJECT,        // 对象作用域
    CHTLJS_FUNC    // CHTL JS函数作用域
};

// 变量信息
struct VariableInfo {
    std::string name;
    std::string type;     // var, let, const, vir
    bool isUsed;
    bool isVirtual;       // 是否是虚对象
    int declareLine;
    int declareColumn;
};

// 作用域
class Scope {
public:
    Scope(ScopeType type, const std::string& name = "");
    
    // 变量管理
    void DeclareVariable(const std::string& name, const std::string& type, 
                        int line, int column, bool isVirtual = false);
    bool HasVariable(const std::string& name) const;
    VariableInfo* GetVariable(const std::string& name);
    
    // 获取所有变量
    std::unordered_map<std::string, VariableInfo>& GetVariables() { return variables; }
    
    // 作用域信息
    ScopeType GetType() const { return type; }
    std::string GetName() const { return name; }
    
private:
    ScopeType type;
    std::string name;
    std::unordered_map<std::string, VariableInfo> variables;
};

// CHTL JS上下文
class CHTLJSContext {
public:
    CHTLJSContext();
    ~CHTLJSContext();
    
    // 作用域管理
    void PushScope(ScopeType type, const std::string& name = "");
    void PopScope();
    std::shared_ptr<Scope> GetCurrentScope();
    std::shared_ptr<Scope> GetGlobalScope();
    
    // 变量声明和查找
    void DeclareVariable(const std::string& name, const std::string& type, 
                        int line, int column, bool isVirtual = false);
    bool IsVariableDeclared(const std::string& name);
    VariableInfo* FindVariable(const std::string& name);
    
    // 函数上下文
    void EnterFunction(const std::string& name);
    void ExitFunction();
    std::string GetCurrentFunction() const;
    bool IsInFunction() const;
    
    // CHTL JS函数上下文
    void EnterCHTLJSFunction(const std::string& name);
    void ExitCHTLJSFunction();
    bool IsInCHTLJSFunction() const;
    std::string GetCurrentCHTLJSFunction() const;
    
    // 增强选择器上下文
    void EnterEnhancedSelector();
    void ExitEnhancedSelector();
    bool IsInEnhancedSelector() const;
    int GetEnhancedSelectorDepth() const { return enhancedSelectorDepth; }
    
    // 错误和警告
    void AddError(const std::string& message, int line, int column);
    void AddWarning(const std::string& message, int line, int column);
    std::vector<std::string> GetErrors() const { return errors; }
    std::vector<std::string> GetWarnings() const { return warnings; }
    bool HasErrors() const { return !errors.empty(); }
    
    // 上下文信息
    std::string GetContextPath() const;
    
private:
    std::stack<std::shared_ptr<Scope>> scopeStack;
    std::stack<std::string> functionStack;
    std::stack<std::string> chtljsFunctionStack;
    int enhancedSelectorDepth;
    
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
};

} // namespace CHTLJS