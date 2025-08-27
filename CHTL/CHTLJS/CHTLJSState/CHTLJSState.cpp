#include "CHTLJSState.h"
#include <sstream>
#include <algorithm>

namespace CHTLJS {

// StateGuard 实现
StateGuard::StateGuard(CHTLJSState* s, StateType newState)
    : state(s), previousState(s->getCurrentState()) {
    if (state) {
        state->pushState(newState);
    }
}

StateGuard::~StateGuard() {
    if (state) {
        state->popState();
    }
}

StateGuard::StateGuard(StateGuard&& other) noexcept
    : state(other.state), previousState(other.previousState) {
    other.state = nullptr;
}

StateGuard& StateGuard::operator=(StateGuard&& other) noexcept {
    if (this != &other) {
        if (state) {
            state->popState();
        }
        state = other.state;
        previousState = other.previousState;
        other.state = nullptr;
    }
    return *this;
}

// CHTLJSState 实现
CHTLJSState::CHTLJSState() 
    : currentState(StateType::INITIAL), recordHistory(false),
      inSelectorContext(false), inArrowChain(false), selectorDepth(0) {
    stateStack.push(StateContext(StateType::INITIAL));
    initializeTransitionRules();
}

void CHTLJSState::initializeTransitionRules() {
    // 初始状态转换
    transitionRules[StateType::INITIAL] = {
        {StateType::IN_STATEMENT, true},
        {StateType::IN_DECLARATION, true},
        {StateType::IN_FUNCTION, true},
        {StateType::IN_COMMENT, true}
    };
    
    // 语句状态转换
    transitionRules[StateType::IN_STATEMENT] = {
        {StateType::IN_EXPRESSION, true},
        {StateType::IN_BLOCK, true},
        {StateType::IN_DECLARATION, true},
        {StateType::IN_COMMENT, true}
    };
    
    // 表达式状态转换
    transitionRules[StateType::IN_EXPRESSION] = {
        {StateType::IN_OBJECT_LITERAL, true},
        {StateType::IN_ARRAY_LITERAL, true},
        {StateType::IN_SELECTOR, true},
        {StateType::IN_LISTEN_BLOCK, true},
        {StateType::IN_ANIMATE_BLOCK, true},
        {StateType::IN_DELEGATE_BLOCK, true},
        {StateType::IN_ARROW_ACCESS, true},
        {StateType::IN_BIND_EXPRESSION, true},
        {StateType::IN_STRING, true},
        {StateType::IN_COMMENT, true}
    };
    
    // 声明状态转换
    transitionRules[StateType::IN_DECLARATION] = {
        {StateType::IN_EXPRESSION, true},
        {StateType::IN_FUNCTION, true},
        {StateType::IN_VIR_DECLARATION, true},
        {StateType::IN_COMMENT, true}
    };
    
    // vir声明转换
    transitionRules[StateType::IN_VIR_DECLARATION] = {
        {StateType::IN_EXPRESSION, true},
        {StateType::IN_INEVERAWAY_BLOCK, true},
        {StateType::IN_COMMENT, true}
    };
    
    // CHTL JS 特殊块转换
    transitionRules[StateType::IN_LISTEN_BLOCK] = {
        {StateType::IN_OBJECT_LITERAL, true},
        {StateType::IN_COMMENT, true}
    };
    
    transitionRules[StateType::IN_ANIMATE_BLOCK] = {
        {StateType::IN_OBJECT_LITERAL, true},
        {StateType::IN_COMMENT, true}
    };
    
    transitionRules[StateType::IN_DELEGATE_BLOCK] = {
        {StateType::IN_OBJECT_LITERAL, true},
        {StateType::IN_COMMENT, true}
    };
    
    transitionRules[StateType::IN_INEVERAWAY_BLOCK] = {
        {StateType::IN_OBJECT_LITERAL, true},
        {StateType::IN_COMMENT, true}
    };
    
    // 选择器状态只能包含字符串
    transitionRules[StateType::IN_SELECTOR] = {
        {StateType::IN_STRING, true}
    };
    
    // 箭头访问可以链式调用
    transitionRules[StateType::IN_ARROW_ACCESS] = {
        {StateType::IN_EXPRESSION, true},
        {StateType::IN_ARROW_ACCESS, true},
        {StateType::IN_LISTEN_BLOCK, true},
        {StateType::IN_DELEGATE_BLOCK, true}
    };
}

bool CHTLJSState::canTransitionTo(StateType newState) const {
    auto it = transitionRules.find(currentState);
    if (it == transitionRules.end()) {
        return false;
    }
    
    auto transIt = it->second.find(newState);
    return transIt != it->second.end() && transIt->second;
}

void CHTLJSState::pushState(StateType newState, std::shared_ptr<BaseNode> node) {
    if (!canTransitionTo(newState)) {
        throw std::runtime_error("Invalid state transition from " + 
                               stateToString(currentState) + " to " + 
                               stateToString(newState));
    }
    
    StateContext context(newState);
    context.currentNode = node;
    context.depth = stateStack.size();
    
    // 继承特殊上下文信息
    if (!stateStack.empty()) {
        const auto& parentContext = stateStack.top();
        context.inCHTLJSFunction = parentContext.inCHTLJSFunction;
        context.inVirtualContext = parentContext.inVirtualContext;
    }
    
    stateStack.push(context);
    currentState = newState;
    
    if (recordHistory) {
        stateHistory.push_back({newState, "push"});
    }
}

void CHTLJSState::popState() {
    if (stateStack.size() <= 1) {
        throw std::runtime_error("Cannot pop initial state");
    }
    
    StateType poppedState = currentState;
    stateStack.pop();
    
    if (!stateStack.empty()) {
        currentState = stateStack.top().type;
    }
    
    if (recordHistory) {
        stateHistory.push_back({poppedState, "pop"});
    }
}

StateGuard CHTLJSState::enterState(StateType newState) {
    return StateGuard(this, newState);
}

StateContext* CHTLJSState::getCurrentContext() {
    return stateStack.empty() ? nullptr : &stateStack.top();
}

const StateContext* CHTLJSState::getCurrentContext() const {
    return stateStack.empty() ? nullptr : &stateStack.top();
}

std::shared_ptr<BaseNode> CHTLJSState::getCurrentNode() {
    auto* context = getCurrentContext();
    return context ? context->currentNode : nullptr;
}

void CHTLJSState::enterSelector(const std::string& selector) {
    inSelectorContext = true;
    selectorDepth++;
    
    if (auto* context = getCurrentContext()) {
        context->currentSelector = selector;
    }
}

void CHTLJSState::exitSelector() {
    if (selectorDepth > 0) {
        selectorDepth--;
    }
    
    if (selectorDepth == 0) {
        inSelectorContext = false;
    }
    
    if (auto* context = getCurrentContext()) {
        context->currentSelector.clear();
    }
}

std::string CHTLJSState::getCurrentSelector() const {
    const auto* context = getCurrentContext();
    return context ? context->currentSelector : "";
}

void CHTLJSState::enterArrowChain() {
    inArrowChain = true;
}

void CHTLJSState::exitArrowChain() {
    inArrowChain = false;
}

void CHTLJSState::markAsCHTLJSFunction() {
    if (auto* context = getCurrentContext()) {
        context->inCHTLJSFunction = true;
    }
}

bool CHTLJSState::isInCHTLJSFunction() const {
    const auto* context = getCurrentContext();
    return context ? context->inCHTLJSFunction : false;
}

void CHTLJSState::markAsVirtualContext() {
    if (auto* context = getCurrentContext()) {
        context->inVirtualContext = true;
    }
}

bool CHTLJSState::isInVirtualContext() const {
    const auto* context = getCurrentContext();
    return context ? context->inVirtualContext : false;
}

bool CHTLJSState::isInState(StateType state) const {
    return currentState == state;
}

bool CHTLJSState::isInAnyState(std::initializer_list<StateType> states) const {
    return std::any_of(states.begin(), states.end(), 
                      [this](StateType s) { return isInState(s); });
}

int CHTLJSState::getStateDepth(StateType state) const {
    std::stack<StateContext> tempStack = stateStack;
    int depth = 0;
    
    while (!tempStack.empty()) {
        if (tempStack.top().type == state) {
            return depth;
        }
        tempStack.pop();
        depth++;
    }
    
    return -1;
}

bool CHTLJSState::validateCurrentState() const {
    if (stateStack.empty()) {
        return false;
    }
    
    if (currentState != stateStack.top().type) {
        return false;
    }
    
    // 验证选择器深度
    if (inSelectorContext && selectorDepth <= 0) {
        return false;
    }
    
    return true;
}

std::string CHTLJSState::getStateError() const {
    if (stateStack.empty()) {
        return "State stack is empty";
    }
    
    if (currentState != stateStack.top().type) {
        return "Current state mismatch with stack top";
    }
    
    if (inSelectorContext && selectorDepth <= 0) {
        return "Invalid selector depth";
    }
    
    return "";
}

bool CHTLJSState::canUseArrowOperator() const {
    // 箭头操作符可以在表达式中使用
    return isInAnyState({
        StateType::IN_EXPRESSION,
        StateType::IN_ARROW_ACCESS,
        StateType::IN_BIND_EXPRESSION
    });
}

bool CHTLJSState::canUseCHTLJSFeature() const {
    // CHTL JS 特性可以在表达式和声明中使用
    return isInAnyState({
        StateType::IN_EXPRESSION,
        StateType::IN_DECLARATION,
        StateType::IN_VIR_DECLARATION
    });
}

std::string CHTLJSState::getStateStackTrace() const {
    std::stringstream ss;
    std::stack<StateContext> tempStack = stateStack;
    std::vector<StateContext> contexts;
    
    while (!tempStack.empty()) {
        contexts.push_back(tempStack.top());
        tempStack.pop();
    }
    
    ss << "CHTL JS State Stack Trace:\n";
    for (auto it = contexts.rbegin(); it != contexts.rend(); ++it) {
        ss << "  " << std::string(it->depth * 2, ' ') 
           << stateToString(it->type);
        
        if (it->inCHTLJSFunction) {
            ss << " [CHTL JS Function]";
        }
        if (it->inVirtualContext) {
            ss << " [Virtual Context]";
        }
        if (!it->currentSelector.empty()) {
            ss << " [Selector: " << it->currentSelector << "]";
        }
        
        ss << " (depth: " << it->depth << ")\n";
    }
    
    return ss.str();
}

void CHTLJSState::reset() {
    while (!stateStack.empty()) {
        stateStack.pop();
    }
    
    currentState = StateType::INITIAL;
    stateStack.push(StateContext(StateType::INITIAL));
    
    inSelectorContext = false;
    inArrowChain = false;
    selectorDepth = 0;
    
    stateHistory.clear();
}

std::string CHTLJSState::stateToString(StateType state) {
    switch (state) {
        case StateType::INITIAL: return "INITIAL";
        case StateType::IN_STATEMENT: return "IN_STATEMENT";
        case StateType::IN_EXPRESSION: return "IN_EXPRESSION";
        case StateType::IN_DECLARATION: return "IN_DECLARATION";
        case StateType::IN_FUNCTION: return "IN_FUNCTION";
        case StateType::IN_BLOCK: return "IN_BLOCK";
        case StateType::IN_OBJECT_LITERAL: return "IN_OBJECT_LITERAL";
        case StateType::IN_ARRAY_LITERAL: return "IN_ARRAY_LITERAL";
        case StateType::IN_SELECTOR: return "IN_SELECTOR";
        case StateType::IN_LISTEN_BLOCK: return "IN_LISTEN_BLOCK";
        case StateType::IN_ANIMATE_BLOCK: return "IN_ANIMATE_BLOCK";
        case StateType::IN_DELEGATE_BLOCK: return "IN_DELEGATE_BLOCK";
        case StateType::IN_VIR_DECLARATION: return "IN_VIR_DECLARATION";
        case StateType::IN_INEVERAWAY_BLOCK: return "IN_INEVERAWAY_BLOCK";
        case StateType::IN_ARROW_ACCESS: return "IN_ARROW_ACCESS";
        case StateType::IN_BIND_EXPRESSION: return "IN_BIND_EXPRESSION";
        case StateType::IN_COMMENT: return "IN_COMMENT";
        case StateType::IN_STRING: return "IN_STRING";
        default: return "UNKNOWN";
    }
}

} // namespace CHTLJS