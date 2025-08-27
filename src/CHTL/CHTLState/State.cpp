#include "State.h"
#include <stdexcept>
#include <sstream>

namespace CHTL {

StateManager::StateManager() {
    // 初始化默认状态
    stateStack.push(StateInfo(ParseState::INITIAL, ContextType::GLOBAL));
}

StateManager::~StateManager() {
    // 清理资源
    while (!stateStack.empty()) {
        stateStack.pop();
    }
}

const StateInfo& StateManager::GetCurrentState() const {
    if (stateStack.empty()) {
        throw std::runtime_error("状态栈为空，无法获取当前状态");
    }
    return stateStack.top();
}

ParseState StateManager::GetParseState() const {
    return GetCurrentState().state;
}

ContextType StateManager::GetContextType() const {
    return GetCurrentState().context;
}

void StateManager::PushState(ParseState newState, ContextType newContext) {
    if (!CanTransitionTo(newState)) {
        throw std::runtime_error("非法的状态转换: " + GetStateName(GetParseState()) + " -> " + GetStateName(newState));
    }
    
    StateInfo newStateInfo(newState, newContext);
    
    // 继承当前状态的一些信息
    if (!stateStack.empty()) {
        const StateInfo& current = stateStack.top();
        newStateInfo.currentNamespace = current.currentNamespace;
        newStateInfo.lineNumber = current.lineNumber;
        newStateInfo.columnNumber = current.columnNumber;
        newStateInfo.braceDepth = current.braceDepth;
        newStateInfo.bracketDepth = current.bracketDepth;
        newStateInfo.parenDepth = current.parenDepth;
    }
    
    stateStack.push(newStateInfo);
}

void StateManager::PopState() {
    if (stateStack.size() <= 1) {
        throw std::runtime_error("无法弹出初始状态");
    }
    stateStack.pop();
}

void StateManager::UpdateState(const StateInfo& info) {
    if (stateStack.empty()) {
        throw std::runtime_error("状态栈为空，无法更新状态");
    }
    stateStack.top() = info;
}

void StateManager::SetCurrentElement(const std::string& element) {
    if (!stateStack.empty()) {
        stateStack.top().currentElement = element;
    }
}

void StateManager::SetCurrentNamespace(const std::string& ns) {
    if (!stateStack.empty()) {
        stateStack.top().currentNamespace = ns;
    }
}

void StateManager::IncreaseBraceDepth() {
    if (!stateStack.empty()) {
        stateStack.top().braceDepth++;
    }
}

void StateManager::DecreaseBraceDepth() {
    if (!stateStack.empty()) {
        stateStack.top().braceDepth--;
    }
}

void StateManager::IncreaseBracketDepth() {
    if (!stateStack.empty()) {
        stateStack.top().bracketDepth++;
    }
}

void StateManager::DecreaseBracketDepth() {
    if (!stateStack.empty()) {
        stateStack.top().bracketDepth--;
    }
}

void StateManager::IncreaseParenDepth() {
    if (!stateStack.empty()) {
        stateStack.top().parenDepth++;
    }
}

void StateManager::DecreaseParenDepth() {
    if (!stateStack.empty()) {
        stateStack.top().parenDepth--;
    }
}

void StateManager::SetInQuotes(bool inQuotes) {
    if (!stateStack.empty()) {
        stateStack.top().inQuotes = inQuotes;
    }
}

void StateManager::SetInComment(bool inComment) {
    if (!stateStack.empty()) {
        stateStack.top().inComment = inComment;
    }
}

void StateManager::UpdatePosition(size_t line, size_t column) {
    if (!stateStack.empty()) {
        stateStack.top().lineNumber = line;
        stateStack.top().columnNumber = column;
    }
}

bool StateManager::IsBalanced() const {
    if (stateStack.empty()) {
        return false;
    }
    
    const StateInfo& current = stateStack.top();
    return current.braceDepth == 0 && 
           current.bracketDepth == 0 && 
           current.parenDepth == 0 &&
           !current.inQuotes &&
           !current.inComment;
}

size_t StateManager::GetStackDepth() const {
    return stateStack.size();
}

void StateManager::Reset() {
    while (!stateStack.empty()) {
        stateStack.pop();
    }
    stateStack.push(StateInfo(ParseState::INITIAL, ContextType::GLOBAL));
    stateData.clear();
}

void StateManager::SetStateData(const std::string& key, const std::string& value) {
    stateData[key] = value;
}

std::string StateManager::GetStateData(const std::string& key) const {
    auto it = stateData.find(key);
    return it != stateData.end() ? it->second : "";
}

void StateManager::ClearStateData() {
    stateData.clear();
}

std::string StateManager::GetStateDescription() const {
    if (stateStack.empty()) {
        return "空状态栈";
    }
    
    const StateInfo& current = stateStack.top();
    std::ostringstream oss;
    oss << "状态: " << GetStateName(current.state)
        << ", 上下文: " << GetContextName(current.context)
        << ", 元素: " << current.currentElement
        << ", 命名空间: " << current.currentNamespace
        << ", 位置: (" << current.lineNumber << ":" << current.columnNumber << ")"
        << ", 深度: {" << current.braceDepth << "," << current.bracketDepth << "," << current.parenDepth << "}";
    return oss.str();
}

bool StateManager::CanTransitionTo(ParseState newState) const {
    if (stateStack.empty()) {
        return newState == ParseState::INITIAL;
    }
    
    return IsValidTransition(GetParseState(), newState);
}

bool StateManager::IsValidTransition(ParseState from, ParseState to) const {
    // 实现状态转换验证逻辑
    // 这里可以根据CHTL语法规则定义哪些状态转换是合法的
    
    switch (from) {
        case ParseState::INITIAL:
            return to == ParseState::ELEMENT || 
                   to == ParseState::TEMPLATE_DEFINITION ||
                   to == ParseState::CUSTOM_DEFINITION ||
                   to == ParseState::IMPORT_STATEMENT ||
                   to == ParseState::NAMESPACE_DEFINITION ||
                   to == ParseState::CONFIGURATION_BLOCK ||
                   to == ParseState::USE_STATEMENT ||
                   to == ParseState::COMMENT_BLOCK ||
                   to == ParseState::ERROR_RECOVERY;
                   
        case ParseState::ELEMENT:
            return to == ParseState::ELEMENT_CONTENT ||
                   to == ParseState::ATTRIBUTE_LIST ||
                   to == ParseState::STYLE_BLOCK ||
                   to == ParseState::SCRIPT_BLOCK ||
                   to == ParseState::TEXT_CONTENT ||
                   to == ParseState::COMMENT_BLOCK ||
                   to == ParseState::ORIGIN_BLOCK ||
                   to == ParseState::INITIAL ||
                   to == ParseState::ERROR_RECOVERY;
                   
        case ParseState::ELEMENT_CONTENT:
            return to == ParseState::ELEMENT ||
                   to == ParseState::TEXT_CONTENT ||
                   to == ParseState::STYLE_BLOCK ||
                   to == ParseState::SCRIPT_BLOCK ||
                   to == ParseState::COMMENT_BLOCK ||
                   to == ParseState::ORIGIN_BLOCK ||
                   to == ParseState::INITIAL ||
                   to == ParseState::ERROR_RECOVERY;
                   
        default:
            // 默认允许转换到错误恢复状态和初始状态
            return to == ParseState::ERROR_RECOVERY || to == ParseState::INITIAL;
    }
}

std::string StateManager::GetStateName(ParseState state) const {
    switch (state) {
        case ParseState::INITIAL: return "INITIAL";
        case ParseState::ELEMENT: return "ELEMENT";
        case ParseState::ELEMENT_CONTENT: return "ELEMENT_CONTENT";
        case ParseState::TEXT_CONTENT: return "TEXT_CONTENT";
        case ParseState::STYLE_BLOCK: return "STYLE_BLOCK";
        case ParseState::SCRIPT_BLOCK: return "SCRIPT_BLOCK";
        case ParseState::TEMPLATE_DEFINITION: return "TEMPLATE_DEFINITION";
        case ParseState::CUSTOM_DEFINITION: return "CUSTOM_DEFINITION";
        case ParseState::IMPORT_STATEMENT: return "IMPORT_STATEMENT";
        case ParseState::NAMESPACE_DEFINITION: return "NAMESPACE_DEFINITION";
        case ParseState::CONFIGURATION_BLOCK: return "CONFIGURATION_BLOCK";
        case ParseState::ORIGIN_BLOCK: return "ORIGIN_BLOCK";
        case ParseState::COMMENT_BLOCK: return "COMMENT_BLOCK";
        case ParseState::ATTRIBUTE_LIST: return "ATTRIBUTE_LIST";
        case ParseState::ATTRIBUTE_VALUE: return "ATTRIBUTE_VALUE";
        case ParseState::USE_STATEMENT: return "USE_STATEMENT";
        case ParseState::EXCEPT_CLAUSE: return "EXCEPT_CLAUSE";
        case ParseState::INHERITANCE_CLAUSE: return "INHERITANCE_CLAUSE";
        case ParseState::DELETE_CLAUSE: return "DELETE_CLAUSE";
        case ParseState::INSERT_CLAUSE: return "INSERT_CLAUSE";
        case ParseState::BRACKET_EXPRESSION: return "BRACKET_EXPRESSION";
        case ParseState::SELECTOR_CONTEXT: return "SELECTOR_CONTEXT";
        case ParseState::VARIABLE_REFERENCE: return "VARIABLE_REFERENCE";
        case ParseState::ERROR_RECOVERY: return "ERROR_RECOVERY";
        default: return "UNKNOWN";
    }
}

std::string StateManager::GetContextName(ContextType context) const {
    switch (context) {
        case ContextType::GLOBAL: return "GLOBAL";
        case ContextType::ELEMENT: return "ELEMENT";
        case ContextType::TEMPLATE: return "TEMPLATE";
        case ContextType::CUSTOM: return "CUSTOM";
        case ContextType::NAMESPACE: return "NAMESPACE";
        case ContextType::CONFIGURATION: return "CONFIGURATION";
        case ContextType::STYLE: return "STYLE";
        case ContextType::SCRIPT: return "SCRIPT";
        case ContextType::IMPORT: return "IMPORT";
        case ContextType::ORIGIN: return "ORIGIN";
        default: return "UNKNOWN";
    }
}

// StateGuard实现

StateGuard::StateGuard(StateManager& mgr, ParseState state, ContextType context)
    : manager(mgr), shouldPop(true) {
    manager.PushState(state, context);
}

StateGuard::~StateGuard() {
    if (shouldPop) {
        try {
            manager.PopState();
        } catch (...) {
            // 析构函数中不抛出异常
        }
    }
}

StateGuard::StateGuard(StateGuard&& other) noexcept
    : manager(other.manager), shouldPop(other.shouldPop) {
    other.shouldPop = false;
}

StateGuard& StateGuard::operator=(StateGuard&& other) noexcept {
    if (this != &other) {
        if (shouldPop) {
            try {
                manager.PopState();
            } catch (...) {
                // 忽略异常
            }
        }
        
        manager = other.manager;
        shouldPop = other.shouldPop;
        other.shouldPop = false;
    }
    return *this;
}

void StateGuard::Release() {
    if (shouldPop) {
        manager.PopState();
        shouldPop = false;
    }
}

} // namespace CHTL