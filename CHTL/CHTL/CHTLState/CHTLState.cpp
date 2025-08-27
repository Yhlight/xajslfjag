#include "CHTLState.hpp"
#include <iostream>
#include <algorithm>

namespace CHTL {

CHTLState::CHTLState() {
    currentState.state = ParseState::INITIAL;
    currentState.context = ContextType::GLOBAL;
    currentState.nestingLevel = 0;
    currentState.isInSpecialization = false;
}

CHTLState::~CHTLState() {
    // 自动清理
}

bool CHTLState::isValidTransition(ParseState from, ParseState to) const {
    // 定义有效的状态转换
    switch (from) {
        case ParseState::INITIAL:
            return to == ParseState::IN_ELEMENT || to == ParseState::IN_TEMPLATE ||
                   to == ParseState::IN_CUSTOM || to == ParseState::IN_ORIGIN ||
                   to == ParseState::IN_IMPORT || to == ParseState::IN_CONFIGURATION ||
                   to == ParseState::IN_NAMESPACE || to == ParseState::IN_COMMENT;
                   
        case ParseState::IN_ELEMENT:
            return to == ParseState::IN_STYLE_BLOCK || to == ParseState::IN_SCRIPT_BLOCK ||
                   to == ParseState::IN_TEXT_BLOCK || to == ParseState::IN_ATTRIBUTE ||
                   to == ParseState::IN_ELEMENT || to == ParseState::INITIAL;
                   
        case ParseState::IN_STYLE_BLOCK:
            return to == ParseState::IN_SELECTOR || to == ParseState::IN_PROPERTY ||
                   to == ParseState::IN_ELEMENT || to == ParseState::INITIAL;
                   
        case ParseState::IN_SCRIPT_BLOCK:
            return to == ParseState::IN_ELEMENT || to == ParseState::INITIAL;
            
        case ParseState::IN_TEMPLATE:
        case ParseState::IN_CUSTOM:
            return to == ParseState::IN_ELEMENT || to == ParseState::INITIAL ||
                   to == ParseState::IN_STYLE_BLOCK;
                   
        default:
            return true; // 允许其他转换
    }
}

bool CHTLState::isValidContext(ContextType context, ParseState state) const {
    // 验证上下文和状态的匹配性
    switch (context) {
        case ContextType::LOCAL_STYLE:
            return state == ParseState::IN_STYLE_BLOCK;
        case ContextType::LOCAL_SCRIPT:
            return state == ParseState::IN_SCRIPT_BLOCK;
        case ContextType::TEXT:
            return state == ParseState::IN_TEXT_BLOCK;
        case ContextType::ELEMENT:
            return state == ParseState::IN_ELEMENT;
        default:
            return true;
    }
}

void CHTLState::pushState(ParseState state, ContextType context) {
    if (!isValidTransition(currentState.state, state)) {
        reportError("无效的状态转换: " + std::to_string(static_cast<int>(currentState.state)) + 
                   " -> " + std::to_string(static_cast<int>(state)));
        return;
    }
    
    if (!isValidContext(context, state)) {
        reportError("无效的上下文状态组合");
        return;
    }
    
    stateStack.push(currentState);
    currentState.state = state;
    currentState.context = context;
    currentState.nestingLevel++;
}

void CHTLState::popState() {
    if (stateStack.empty()) {
        reportError("尝试从空状态栈中弹出状态");
        return;
    }
    
    currentState = stateStack.top();
    stateStack.pop();
}

void CHTLState::setState(ParseState state) {
    if (!isValidTransition(currentState.state, state)) {
        reportError("无效的状态转换");
        return;
    }
    currentState.state = state;
}

void CHTLState::setContext(ContextType context) {
    if (!isValidContext(context, currentState.state)) {
        reportError("无效的上下文设置");
        return;
    }
    currentState.context = context;
}

ParseState CHTLState::getCurrentState() const {
    return currentState.state;
}

ContextType CHTLState::getCurrentContext() const {
    return currentState.context;
}

const StateInfo& CHTLState::getCurrentStateInfo() const {
    return currentState;
}

size_t CHTLState::getStackDepth() const {
    return stateStack.size();
}

bool CHTLState::isInState(ParseState state) const {
    return currentState.state == state;
}

bool CHTLState::isInContext(ContextType context) const {
    return currentState.context == context;
}

void CHTLState::setCurrentElement(const std::string& element) {
    currentState.currentElement = element;
}

void CHTLState::setCurrentNamespace(const std::string& namespaceName) {
    currentState.currentNamespace = namespaceName;
}

std::string CHTLState::getCurrentElement() const {
    return currentState.currentElement;
}

std::string CHTLState::getCurrentNamespace() const {
    return currentState.currentNamespace;
}

void CHTLState::setAttribute(const std::string& name, const std::string& value) {
    currentState.attributes[name] = value;
}

std::string CHTLState::getAttribute(const std::string& name) const {
    auto it = currentState.attributes.find(name);
    return (it != currentState.attributes.end()) ? it->second : "";
}

bool CHTLState::hasAttribute(const std::string& name) const {
    return currentState.attributes.find(name) != currentState.attributes.end();
}

void CHTLState::clearAttributes() {
    currentState.attributes.clear();
}

void CHTLState::pushConstraint(const std::string& constraint) {
    currentState.constraintStack.push_back(constraint);
}

void CHTLState::popConstraint() {
    if (!currentState.constraintStack.empty()) {
        currentState.constraintStack.pop_back();
    }
}

bool CHTLState::isConstraintActive(const std::string& constraint) const {
    return std::find(currentState.constraintStack.begin(), 
                    currentState.constraintStack.end(), 
                    constraint) != currentState.constraintStack.end();
}

void CHTLState::enterSpecialization() {
    currentState.isInSpecialization = true;
}

void CHTLState::exitSpecialization() {
    currentState.isInSpecialization = false;
}

bool CHTLState::isInSpecialization() const {
    return currentState.isInSpecialization;
}

void CHTLState::incrementNesting() {
    currentState.nestingLevel++;
}

void CHTLState::decrementNesting() {
    if (currentState.nestingLevel > 0) {
        currentState.nestingLevel--;
    }
}

size_t CHTLState::getNestingLevel() const {
    return currentState.nestingLevel;
}

void CHTLState::setFeatureFlag(const std::string& flag, bool value) {
    featureFlags[flag] = value;
}

bool CHTLState::getFeatureFlag(const std::string& flag) const {
    auto it = featureFlags.find(flag);
    return (it != featureFlags.end()) ? it->second : false;
}

void CHTLState::reportError(const std::string& error) {
    errorStack.push_back(error);
}

std::vector<std::string> CHTLState::getErrors() const {
    return errorStack;
}

void CHTLState::clearErrors() {
    errorStack.clear();
}

bool CHTLState::hasErrors() const {
    return !errorStack.empty();
}

bool CHTLState::canEnterState(ParseState state) const {
    return isValidTransition(currentState.state, state);
}

bool CHTLState::canExitState() const {
    return !stateStack.empty();
}

bool CHTLState::validateCurrentState() const {
    return isValidContext(currentState.context, currentState.state);
}

void CHTLState::dumpState() const {
    std::cout << "=== CHTL State ===\n";
    std::cout << "Current State: " << static_cast<int>(currentState.state) << "\n";
    std::cout << "Current Context: " << static_cast<int>(currentState.context) << "\n";
    std::cout << "Current Element: " << currentState.currentElement << "\n";
    std::cout << "Current Namespace: " << currentState.currentNamespace << "\n";
    std::cout << "Nesting Level: " << currentState.nestingLevel << "\n";
    std::cout << "In Specialization: " << (currentState.isInSpecialization ? "Yes" : "No") << "\n";
    std::cout << "Stack Depth: " << stateStack.size() << "\n";
    std::cout << "Constraints: ";
    for (const auto& constraint : currentState.constraintStack) {
        std::cout << constraint << " ";
    }
    std::cout << "\n";
    std::cout << "Attributes: ";
    for (const auto& attr : currentState.attributes) {
        std::cout << attr.first << "=" << attr.second << " ";
    }
    std::cout << "\n";
    if (!errorStack.empty()) {
        std::cout << "Errors:\n";
        for (const auto& error : errorStack) {
            std::cout << "  - " << error << "\n";
        }
    }
    std::cout << "==================\n";
}

std::string CHTLState::getStateDescription() const {
    return "State: " + std::to_string(static_cast<int>(currentState.state)) +
           ", Context: " + std::to_string(static_cast<int>(currentState.context)) +
           ", Element: " + currentState.currentElement +
           ", Namespace: " + currentState.currentNamespace;
}

void CHTLState::reset() {
    while (!stateStack.empty()) {
        stateStack.pop();
    }
    currentState = StateInfo();
    featureFlags.clear();
    errorStack.clear();
}

// StateGuard 实现
StateGuard::StateGuard(CHTLState& state, ParseState newState, ContextType newContext)
    : state(state), shouldPop(true) {
    state.pushState(newState, newContext);
}

StateGuard::~StateGuard() {
    if (shouldPop) {
        state.popState();
    }
}

StateGuard::StateGuard(StateGuard&& other) noexcept
    : state(other.state), shouldPop(other.shouldPop) {
    other.shouldPop = false;
}

StateGuard& StateGuard::operator=(StateGuard&& other) noexcept {
    if (this != &other) {
        if (shouldPop) {
            state.popState();
        }
        shouldPop = other.shouldPop;
        other.shouldPop = false;
    }
    return *this;
}

void StateGuard::release() {
    shouldPop = false;
}

// ContextManager 实现
ContextManager::ContextManager(CHTLState& state, ContextType context)
    : state(state) {
    savedElement = state.getCurrentElement();
    savedNamespace = state.getCurrentNamespace();
    state.setContext(context);
}

ContextManager::~ContextManager() {
    state.setCurrentElement(savedElement);
    state.setCurrentNamespace(savedNamespace);
}

void ContextManager::setElement(const std::string& element) {
    state.setCurrentElement(element);
}

void ContextManager::setNamespace(const std::string& namespaceName) {
    state.setCurrentNamespace(namespaceName);
}

}