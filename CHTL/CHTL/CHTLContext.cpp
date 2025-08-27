#include "CHTLContext.h"
#include <sstream>

namespace CHTL {

CHTLContext::CHTLContext()
    : inLocalStyle(false), inLocalScript(false),
      autoClassAdded(false), autoIdAdded(false) {
}

CHTLContext::~CHTLContext() = default;

void CHTLContext::EnterScope(const std::string& scopeType) {
    scopeStack.push(scopeType);
}

void CHTLContext::ExitScope() {
    if (!scopeStack.empty()) {
        scopeStack.pop();
    }
}

std::string CHTLContext::GetCurrentScope() const {
    if (!scopeStack.empty()) {
        return scopeStack.top();
    }
    return "";
}

bool CHTLContext::IsInScope(const std::string& scopeType) const {
    std::stack<std::string> temp = scopeStack;
    while (!temp.empty()) {
        if (temp.top() == scopeType) {
            return true;
        }
        temp.pop();
    }
    return false;
}

void CHTLContext::PushElement(const std::string& elementName) {
    elementStack.push(elementName);
}

void CHTLContext::PopElement() {
    if (!elementStack.empty()) {
        elementStack.pop();
    }
}

std::string CHTLContext::GetCurrentElement() const {
    if (!elementStack.empty()) {
        return elementStack.top();
    }
    return "";
}

std::vector<std::string> CHTLContext::GetElementPath() const {
    std::vector<std::string> path;
    std::stack<std::string> temp = elementStack;
    
    while (!temp.empty()) {
        path.insert(path.begin(), temp.top());
        temp.pop();
    }
    
    return path;
}

void CHTLContext::SetActiveConfig(std::shared_ptr<ConfigNode> config) {
    activeConfig = config;
}

std::shared_ptr<ConfigNode> CHTLContext::GetActiveConfig() const {
    return activeConfig;
}

void CHTLContext::SetVariable(const std::string& name, const std::string& value) {
    variables[name] = value;
}

std::string CHTLContext::GetVariable(const std::string& name) const {
    auto it = variables.find(name);
    if (it != variables.end()) {
        return it->second;
    }
    return "";
}

bool CHTLContext::HasVariable(const std::string& name) const {
    return variables.find(name) != variables.end();
}

void CHTLContext::ClearVariables() {
    variables.clear();
}

std::string CHTLContext::GetContextInfo() const {
    std::stringstream ss;
    
    ss << "文件: " << currentFile << "\n";
    ss << "当前作用域: " << GetCurrentScope() << "\n";
    ss << "当前元素: " << GetCurrentElement() << "\n";
    ss << "局部样式: " << (inLocalStyle ? "是" : "否") << "\n";
    ss << "局部脚本: " << (inLocalScript ? "是" : "否") << "\n";
    
    if (!variables.empty()) {
        ss << "变量:\n";
        for (const auto& [name, value] : variables) {
            ss << "  " << name << " = " << value << "\n";
        }
    }
    
    return ss.str();
}

} // namespace CHTL