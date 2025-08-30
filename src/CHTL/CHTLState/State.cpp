#include "State.h"
#include <iostream>
#include <algorithm>

namespace CHTL {

StateManager::StateManager() : currentState(CHTLState::IDLE) {
    initializeTransitions();
}

void StateManager::setState(CHTLState newState) {
    if (currentState == newState) return;
    
    CHTLState oldState = currentState;
    currentState = newState;
    updateHistory(oldState, newState);
}

bool StateManager::canTransitionTo(CHTLState targetState, const String& token) const {
    for (const auto& transition : transitions) {
        if (transition.fromState == currentState && transition.toState == targetState) {
            if (transition.triggerToken.empty() || transition.triggerToken == token) {
                if (!transition.condition || transition.condition(token)) {
                    return true;
                }
            }
        }
    }
    return false;
}

bool StateManager::transitionTo(CHTLState targetState, const String& token) {
    if (canTransitionTo(targetState, token)) {
        setState(targetState);
        return true;
    }
    return false;
}

void StateManager::pushState(CHTLState state) {
    stateStack.push(currentState);
    setState(state);
}

CHTLState StateManager::popState() {
    if (stateStack.empty()) {
        return currentState;
    }
    
    CHTLState restoredState = stateStack.top();
    stateStack.pop();
    setState(restoredState);
    return restoredState;
}

StringVector StateManager::getStateHistory() const {
    return stateHistory;
}

void StateManager::clearHistory() {
    stateHistory.clear();
}

bool StateManager::isValidState(CHTLState state) const {
    return state != CHTLState::ERROR_STATE;
}

bool StateManager::isParsingState(CHTLState state) const {
    return state == CHTLState::PARSING_ELEMENT ||
           state == CHTLState::PARSING_TEXT ||
           state == CHTLState::PARSING_ATTRIBUTES ||
           state == CHTLState::PARSING_STYLE_BLOCK ||
           state == CHTLState::PARSING_SCRIPT_BLOCK ||
           state == CHTLState::PARSING_TEMPLATE ||
           state == CHTLState::PARSING_CUSTOM ||
           state == CHTLState::PARSING_ORIGIN ||
           state == CHTLState::PARSING_IMPORT ||
           state == CHTLState::PARSING_CONFIG ||
           state == CHTLState::PARSING_NAMESPACE;
}

bool StateManager::isBlockState(CHTLState state) const {
    return state == CHTLState::PARSING_STYLE_BLOCK ||
           state == CHTLState::PARSING_SCRIPT_BLOCK ||
           state == CHTLState::PARSING_TEMPLATE ||
           state == CHTLState::PARSING_CUSTOM ||
           state == CHTLState::PARSING_ORIGIN ||
           state == CHTLState::PARSING_CONFIG ||
           state == CHTLState::PARSING_NAMESPACE;
}

bool StateManager::isInTemplateContext() const {
    return currentState == CHTLState::PARSING_TEMPLATE ||
           std::find_if(stateHistory.rbegin(), stateHistory.rend(),
                       [](const String& s) { return s == "PARSING_TEMPLATE"; }) != stateHistory.rend();
}

bool StateManager::isInCustomContext() const {
    return currentState == CHTLState::PARSING_CUSTOM ||
           std::find_if(stateHistory.rbegin(), stateHistory.rend(),
                       [](const String& s) { return s == "PARSING_CUSTOM"; }) != stateHistory.rend();
}

bool StateManager::isInStyleContext() const {
    return currentState == CHTLState::PARSING_STYLE_BLOCK;
}

bool StateManager::isInScriptContext() const {
    return currentState == CHTLState::PARSING_SCRIPT_BLOCK;
}

bool StateManager::isInImportContext() const {
    return currentState == CHTLState::PARSING_IMPORT;
}

bool StateManager::isInNamespaceContext() const {
    return currentState == CHTLState::PARSING_NAMESPACE ||
           std::find_if(stateHistory.rbegin(), stateHistory.rend(),
                       [](const String& s) { return s == "PARSING_NAMESPACE"; }) != stateHistory.rend();
}

void StateManager::addTransition(const StateTransition& transition) {
    transitions.push_back(transition);
}

void StateManager::removeTransition(CHTLState from, CHTLState to) {
    transitions.erase(
        std::remove_if(transitions.begin(), transitions.end(),
                      [from, to](const StateTransition& t) {
                          return t.fromState == from && t.toState == to;
                      }),
        transitions.end());
}

std::vector<StateTransition> StateManager::getAvailableTransitions() const {
    std::vector<StateTransition> available;
    for (const auto& transition : transitions) {
        if (transition.fromState == currentState) {
            available.push_back(transition);
        }
    }
    return available;
}

void StateManager::enterErrorState(const String& reason) {
    errorReason = reason;
    setState(CHTLState::ERROR_STATE);
}

bool StateManager::recoverFromError() {
    if (currentState != CHTLState::ERROR_STATE) {
        return true;
    }
    
    if (!stateStack.empty()) {
        popState();
        errorReason.clear();
        return true;
    }
    
    setState(CHTLState::IDLE);
    errorReason.clear();
    return true;
}

String StateManager::stateToString(CHTLState state) const {
    switch (state) {
        case CHTLState::IDLE: return "IDLE";
        case CHTLState::PARSING_ELEMENT: return "PARSING_ELEMENT";
        case CHTLState::PARSING_TEXT: return "PARSING_TEXT";
        case CHTLState::PARSING_ATTRIBUTES: return "PARSING_ATTRIBUTES";
        case CHTLState::PARSING_STYLE_BLOCK: return "PARSING_STYLE_BLOCK";
        case CHTLState::PARSING_SCRIPT_BLOCK: return "PARSING_SCRIPT_BLOCK";
        case CHTLState::PARSING_TEMPLATE: return "PARSING_TEMPLATE";
        case CHTLState::PARSING_CUSTOM: return "PARSING_CUSTOM";
        case CHTLState::PARSING_ORIGIN: return "PARSING_ORIGIN";
        case CHTLState::PARSING_IMPORT: return "PARSING_IMPORT";
        case CHTLState::PARSING_CONFIG: return "PARSING_CONFIG";
        case CHTLState::PARSING_NAMESPACE: return "PARSING_NAMESPACE";
        case CHTLState::PARSING_SELECTOR: return "PARSING_SELECTOR";
        case CHTLState::PARSING_INHERITANCE: return "PARSING_INHERITANCE";
        case CHTLState::PARSING_DELETION: return "PARSING_DELETION";
        case CHTLState::PARSING_INSERTION: return "PARSING_INSERTION";
        case CHTLState::PARSING_COMMENT: return "PARSING_COMMENT";
        case CHTLState::PARSING_STRING: return "PARSING_STRING";
        case CHTLState::PARSING_VARIABLE: return "PARSING_VARIABLE";
        case CHTLState::ERROR_STATE: return "ERROR_STATE";
        default: return "UNKNOWN";
    }
}

String StateManager::getCurrentStateString() const {
    return stateToString(currentState);
}

void StateManager::dumpState() const {
    std::cout << "Current State: " << getCurrentStateString() << std::endl;
    std::cout << "Stack Depth: " << getStackDepth() << std::endl;
    std::cout << "Error: " << (isErrorState() ? errorReason : "None") << std::endl;
    
    if (!stateHistory.empty()) {
        std::cout << "Recent History: ";
        size_t start = stateHistory.size() > 5 ? stateHistory.size() - 5 : 0;
        for (size_t i = start; i < stateHistory.size(); ++i) {
            if (i > start) std::cout << " -> ";
            std::cout << stateHistory[i];
        }
        std::cout << std::endl;
    }
}

void StateManager::initializeTransitions() {
    // 基础状态转换
    addTransition(StateTransition(CHTLState::IDLE, CHTLState::PARSING_ELEMENT, "html_element"));
    addTransition(StateTransition(CHTLState::IDLE, CHTLState::PARSING_TEXT, "text"));
    addTransition(StateTransition(CHTLState::IDLE, CHTLState::PARSING_TEMPLATE, "[Template]"));
    addTransition(StateTransition(CHTLState::IDLE, CHTLState::PARSING_CUSTOM, "[Custom]"));
    addTransition(StateTransition(CHTLState::IDLE, CHTLState::PARSING_ORIGIN, "[Origin]"));
    addTransition(StateTransition(CHTLState::IDLE, CHTLState::PARSING_IMPORT, "[Import]"));
    addTransition(StateTransition(CHTLState::IDLE, CHTLState::PARSING_CONFIG, "[Configuration]"));
    addTransition(StateTransition(CHTLState::IDLE, CHTLState::PARSING_NAMESPACE, "[Namespace]"));
    addTransition(StateTransition(CHTLState::IDLE, CHTLState::PARSING_COMMENT, "//"));
    
    // 元素解析状态转换
    addTransition(StateTransition(CHTLState::PARSING_ELEMENT, CHTLState::PARSING_ATTRIBUTES, "identifier"));
    addTransition(StateTransition(CHTLState::PARSING_ELEMENT, CHTLState::PARSING_STYLE_BLOCK, "style"));
    addTransition(StateTransition(CHTLState::PARSING_ELEMENT, CHTLState::PARSING_SCRIPT_BLOCK, "script"));
    addTransition(StateTransition(CHTLState::PARSING_ELEMENT, CHTLState::PARSING_TEXT, "text"));
    addTransition(StateTransition(CHTLState::PARSING_ELEMENT, CHTLState::PARSING_ELEMENT, "html_element"));
    addTransition(StateTransition(CHTLState::PARSING_ELEMENT, CHTLState::IDLE, "}"));
    
    // 属性解析状态转换
    addTransition(StateTransition(CHTLState::PARSING_ATTRIBUTES, CHTLState::PARSING_ELEMENT, ";"));
    addTransition(StateTransition(CHTLState::PARSING_ATTRIBUTES, CHTLState::PARSING_STRING, "\""));
    addTransition(StateTransition(CHTLState::PARSING_ATTRIBUTES, CHTLState::PARSING_STRING, "'"));
    
    // 样式块状态转换
    addTransition(StateTransition(CHTLState::PARSING_STYLE_BLOCK, CHTLState::PARSING_SELECTOR, "."));
    addTransition(StateTransition(CHTLState::PARSING_STYLE_BLOCK, CHTLState::PARSING_SELECTOR, "#"));
    addTransition(StateTransition(CHTLState::PARSING_STYLE_BLOCK, CHTLState::PARSING_SELECTOR, "&"));
    addTransition(StateTransition(CHTLState::PARSING_STYLE_BLOCK, CHTLState::PARSING_ELEMENT, "}"));
    
    // 脚本块状态转换
    addTransition(StateTransition(CHTLState::PARSING_SCRIPT_BLOCK, CHTLState::PARSING_ELEMENT, "}"));
    
    // 模板状态转换
    addTransition(StateTransition(CHTLState::PARSING_TEMPLATE, CHTLState::PARSING_INHERITANCE, "inherit"));
    addTransition(StateTransition(CHTLState::PARSING_TEMPLATE, CHTLState::PARSING_DELETION, "delete"));
    addTransition(StateTransition(CHTLState::PARSING_TEMPLATE, CHTLState::IDLE, "}"));
    
    // 自定义状态转换
    addTransition(StateTransition(CHTLState::PARSING_CUSTOM, CHTLState::PARSING_INHERITANCE, "inherit"));
    addTransition(StateTransition(CHTLState::PARSING_CUSTOM, CHTLState::PARSING_DELETION, "delete"));
    addTransition(StateTransition(CHTLState::PARSING_CUSTOM, CHTLState::PARSING_INSERTION, "insert"));
    addTransition(StateTransition(CHTLState::PARSING_CUSTOM, CHTLState::IDLE, "}"));
    
    // 错误状态恢复
    addTransition(StateTransition(CHTLState::ERROR_STATE, CHTLState::IDLE, ""));
}

bool StateManager::checkTransitionCondition(const StateTransition& transition, const String& token) const {
    if (transition.condition) {
        return transition.condition(token);
    }
    return true;
}

void StateManager::updateHistory(CHTLState oldState, CHTLState newState) {
    stateHistory.push_back(stateToString(oldState) + "->" + stateToString(newState));
    
    // 限制历史记录大小
    if (stateHistory.size() > 100) {
        stateHistory.erase(stateHistory.begin());
    }
}

// StateGuard实现
StateGuard::StateGuard(StateManager& manager, CHTLState newState)
    : stateManager(manager), savedState(manager.getCurrentState()), shouldRestore(true) {
    stateManager.setState(newState);
}

StateGuard::~StateGuard() {
    if (shouldRestore) {
        restore();
    }
}

void StateGuard::restore() {
    if (shouldRestore) {
        stateManager.setState(savedState);
        shouldRestore = false;
    }
}

void StateGuard::release() {
    shouldRestore = false;
}

// ObservableStateManager实现
void ObservableStateManager::addListener(std::shared_ptr<StateListener> listener) {
    listeners.push_back(listener);
}

void ObservableStateManager::removeListener(std::shared_ptr<StateListener> listener) {
    listeners.erase(
        std::remove_if(listeners.begin(), listeners.end(),
                      [&listener](const std::weak_ptr<StateListener>& weak) {
                          return weak.expired() || weak.lock() == listener;
                      }),
        listeners.end());
}

void ObservableStateManager::setState(CHTLState newState) {
    CHTLState oldState = getCurrentState();
    StateManager::setState(newState);
    notifyStateChanged(oldState, newState);
}

void ObservableStateManager::enterErrorState(const String& reason) {
    StateManager::enterErrorState(reason);
    notifyStateError(reason);
}

void ObservableStateManager::notifyStateChanged(CHTLState oldState, CHTLState newState) {
    cleanupListeners();
    for (auto& weak : listeners) {
        if (auto listener = weak.lock()) {
            listener->onStateChanged(oldState, newState);
        }
    }
}

void ObservableStateManager::notifyStateError(const String& reason) {
    cleanupListeners();
    for (auto& weak : listeners) {
        if (auto listener = weak.lock()) {
            listener->onStateError(reason);
        }
    }
}

void ObservableStateManager::cleanupListeners() {
    listeners.erase(
        std::remove_if(listeners.begin(), listeners.end(),
                      [](const std::weak_ptr<StateListener>& weak) {
                          return weak.expired();
                      }),
        listeners.end());
}

} // namespace CHTL