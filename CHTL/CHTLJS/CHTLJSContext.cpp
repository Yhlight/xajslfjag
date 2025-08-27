#include "CHTLJSContext.h"
#include <sstream>

namespace CHTLJS {

// Scope实现
Scope::Scope(ScopeType type, const std::string& name)
    : type(type), name(name) {
}

void Scope::DeclareVariable(const std::string& name, const std::string& type, 
                           int line, int column, bool isVirtual) {
    VariableInfo info;
    info.name = name;
    info.type = type;
    info.isUsed = false;
    info.isVirtual = isVirtual;
    info.declareLine = line;
    info.declareColumn = column;
    
    variables[name] = info;
}

bool Scope::HasVariable(const std::string& name) const {
    return variables.find(name) != variables.end();
}

VariableInfo* Scope::GetVariable(const std::string& name) {
    auto it = variables.find(name);
    if (it != variables.end()) {
        return &it->second;
    }
    return nullptr;
}

// CHTLJSContext实现
CHTLJSContext::CHTLJSContext() : enhancedSelectorDepth(0) {
    // 创建全局作用域
    PushScope(ScopeType::GLOBAL, "global");
}

CHTLJSContext::~CHTLJSContext() = default;

void CHTLJSContext::PushScope(ScopeType type, const std::string& name) {
    auto scope = std::make_shared<Scope>(type, name);
    scopeStack.push(scope);
}

void CHTLJSContext::PopScope() {
    if (scopeStack.size() > 1) { // 保留全局作用域
        scopeStack.pop();
    }
}

std::shared_ptr<Scope> CHTLJSContext::GetCurrentScope() {
    if (!scopeStack.empty()) {
        return scopeStack.top();
    }
    return nullptr;
}

std::shared_ptr<Scope> CHTLJSContext::GetGlobalScope() {
    if (!scopeStack.empty()) {
        std::stack<std::shared_ptr<Scope>> temp = scopeStack;
        std::shared_ptr<Scope> global;
        
        while (!temp.empty()) {
            global = temp.top();
            temp.pop();
        }
        
        return global;
    }
    return nullptr;
}

void CHTLJSContext::DeclareVariable(const std::string& name, const std::string& type, 
                                   int line, int column, bool isVirtual) {
    auto currentScope = GetCurrentScope();
    if (currentScope) {
        // 检查当前作用域是否已声明
        if (currentScope->HasVariable(name)) {
            AddError("变量 '" + name + "' 已在当前作用域中声明", line, column);
            return;
        }
        
        // 对于let和const，检查是否在块作用域中已声明
        if ((type == "let" || type == "const") && currentScope->GetType() == ScopeType::BLOCK) {
            // 检查外层作用域
            std::stack<std::shared_ptr<Scope>> temp = scopeStack;
            temp.pop(); // 跳过当前作用域
            
            while (!temp.empty()) {
                auto scope = temp.top();
                if (scope->GetType() == ScopeType::FUNCTION || 
                    scope->GetType() == ScopeType::GLOBAL) {
                    break;
                }
                
                if (scope->HasVariable(name)) {
                    AddError("变量 '" + name + "' 已在外层块作用域中声明", line, column);
                    return;
                }
                
                temp.pop();
            }
        }
        
        currentScope->DeclareVariable(name, type, line, column, isVirtual);
    }
}

bool CHTLJSContext::IsVariableDeclared(const std::string& name) {
    return FindVariable(name) != nullptr;
}

VariableInfo* CHTLJSContext::FindVariable(const std::string& name) {
    std::stack<std::shared_ptr<Scope>> temp = scopeStack;
    
    while (!temp.empty()) {
        auto scope = temp.top();
        auto var = scope->GetVariable(name);
        if (var) {
            return var;
        }
        temp.pop();
    }
    
    return nullptr;
}

void CHTLJSContext::EnterFunction(const std::string& name) {
    functionStack.push(name);
    PushScope(ScopeType::FUNCTION, name);
}

void CHTLJSContext::ExitFunction() {
    if (!functionStack.empty()) {
        functionStack.pop();
        PopScope();
    }
}

std::string CHTLJSContext::GetCurrentFunction() const {
    if (!functionStack.empty()) {
        return functionStack.top();
    }
    return "";
}

bool CHTLJSContext::IsInFunction() const {
    return !functionStack.empty();
}

void CHTLJSContext::EnterCHTLJSFunction(const std::string& name) {
    chtljsFunctionStack.push(name);
    PushScope(ScopeType::CHTLJS_FUNC, name);
}

void CHTLJSContext::ExitCHTLJSFunction() {
    if (!chtljsFunctionStack.empty()) {
        chtljsFunctionStack.pop();
        PopScope();
    }
}

bool CHTLJSContext::IsInCHTLJSFunction() const {
    return !chtljsFunctionStack.empty();
}

std::string CHTLJSContext::GetCurrentCHTLJSFunction() const {
    if (!chtljsFunctionStack.empty()) {
        return chtljsFunctionStack.top();
    }
    return "";
}

void CHTLJSContext::EnterEnhancedSelector() {
    enhancedSelectorDepth++;
}

void CHTLJSContext::ExitEnhancedSelector() {
    if (enhancedSelectorDepth > 0) {
        enhancedSelectorDepth--;
    }
}

bool CHTLJSContext::IsInEnhancedSelector() const {
    return enhancedSelectorDepth > 0;
}

void CHTLJSContext::AddError(const std::string& message, int line, int column) {
    std::stringstream ss;
    ss << "[" << line << ":" << column << "] 错误: " << message;
    errors.push_back(ss.str());
}

void CHTLJSContext::AddWarning(const std::string& message, int line, int column) {
    std::stringstream ss;
    ss << "[" << line << ":" << column << "] 警告: " << message;
    warnings.push_back(ss.str());
}

std::string CHTLJSContext::GetContextPath() const {
    std::stringstream ss;
    std::stack<std::shared_ptr<Scope>> temp = scopeStack;
    std::vector<std::string> path;
    
    while (!temp.empty()) {
        auto scope = temp.top();
        path.push_back(scope->GetName());
        temp.pop();
    }
    
    // 反转路径
    for (auto it = path.rbegin(); it != path.rend(); ++it) {
        if (it != path.rbegin()) ss << ".";
        ss << *it;
    }
    
    return ss.str();
}

} // namespace CHTLJS