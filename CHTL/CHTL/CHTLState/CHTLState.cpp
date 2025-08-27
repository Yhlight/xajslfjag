#include "CHTLState.h"
#include <sstream>
#include <algorithm>

namespace CHTL {

// StateGuard 实现
StateGuard::StateGuard(CHTLState* s, StateType newState)
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

// CHTLState 实现
CHTLState::CHTLState() : currentState(StateType::INITIAL), recordHistory(false) {
    stateStack.push(StateContext(StateType::INITIAL));
    initializeTransitionRules();
}

void CHTLState::initializeTransitionRules() {
    // 初始状态可以转换到的状态
    transitionRules[StateType::INITIAL] = {
        {StateType::IN_ELEMENT, true},
        {StateType::IN_TEMPLATE, true},
        {StateType::IN_CUSTOM, true},
        {StateType::IN_ORIGIN, true},
        {StateType::IN_CONFIG, true},
        {StateType::IN_NAMESPACE, true},
        {StateType::IN_IMPORT, true},
        {StateType::IN_COMMENT, true},
        {StateType::IN_OPERATOR, true}
    };
    
    // 元素内可以转换到的状态
    transitionRules[StateType::IN_ELEMENT] = {
        {StateType::IN_ELEMENT, true},      // 嵌套元素
        {StateType::IN_TEXT, true},
        {StateType::IN_STYLE, true},
        {StateType::IN_SCRIPT, true},
        {StateType::IN_ATTRIBUTE, true},
        {StateType::IN_COMMENT, true},
        {StateType::IN_OPERATOR, true},
        {StateType::IN_ORIGIN, true}
    };
    
    // 文本节点内的转换
    transitionRules[StateType::IN_TEXT] = {
        {StateType::IN_LITERAL, true},
        {StateType::IN_COMMENT, true}
    };
    
    // 样式块内的转换
    transitionRules[StateType::IN_STYLE] = {
        {StateType::IN_LITERAL, true},
        {StateType::IN_COMMENT, true},
        {StateType::IN_OPERATOR, true},  // 用于@Style等
        {StateType::IN_ORIGIN, true}
    };
    
    // 脚本块内的转换
    transitionRules[StateType::IN_SCRIPT] = {
        {StateType::IN_LITERAL, true},
        {StateType::IN_COMMENT, true},
        {StateType::IN_ORIGIN, true}
    };
    
    // 模板内的转换
    transitionRules[StateType::IN_TEMPLATE] = {
        {StateType::IN_ELEMENT, true},
        {StateType::IN_STYLE, true},
        {StateType::IN_ATTRIBUTE, true},
        {StateType::IN_COMMENT, true},
        {StateType::IN_OPERATOR, true}
    };
    
    // 自定义内的转换
    transitionRules[StateType::IN_CUSTOM] = {
        {StateType::IN_ELEMENT, true},
        {StateType::IN_STYLE, true},
        {StateType::IN_ATTRIBUTE, true},
        {StateType::IN_COMMENT, true},
        {StateType::IN_OPERATOR, true}
    };
    
    // 配置块内的转换
    transitionRules[StateType::IN_CONFIG] = {
        {StateType::IN_ATTRIBUTE, true},
        {StateType::IN_LITERAL, true},
        {StateType::IN_COMMENT, true}
    };
    
    // 命名空间内的转换
    transitionRules[StateType::IN_NAMESPACE] = {
        {StateType::IN_ELEMENT, true},
        {StateType::IN_TEMPLATE, true},
        {StateType::IN_CUSTOM, true},
        {StateType::IN_ORIGIN, true},
        {StateType::IN_NAMESPACE, true},  // 嵌套命名空间
        {StateType::IN_COMMENT, true},
        {StateType::IN_OPERATOR, true}
    };
    
    // 注释状态不能转换到其他状态（必须先退出注释）
    transitionRules[StateType::IN_COMMENT] = {};
    
    // 属性状态的转换
    transitionRules[StateType::IN_ATTRIBUTE] = {
        {StateType::IN_LITERAL, true},
        {StateType::IN_COMMENT, true}
    };
    
    // 操作符状态的转换
    transitionRules[StateType::IN_OPERATOR] = {
        {StateType::IN_ELEMENT, true},
        {StateType::IN_LITERAL, true},
        {StateType::IN_COMMENT, true}
    };
}

bool CHTLState::canTransitionTo(StateType newState) const {
    auto it = transitionRules.find(currentState);
    if (it == transitionRules.end()) {
        return false;
    }
    
    auto transIt = it->second.find(newState);
    return transIt != it->second.end() && transIt->second;
}

void CHTLState::pushState(StateType newState, std::shared_ptr<BaseNode> node) {
    if (!canTransitionTo(newState)) {
        throw std::runtime_error("Invalid state transition from " + 
                               stateToString(currentState) + " to " + 
                               stateToString(newState));
    }
    
    StateContext context(newState);
    context.currentNode = node;
    context.depth = stateStack.size();
    
    stateStack.push(context);
    currentState = newState;
    
    if (recordHistory) {
        stateHistory.push_back({newState, "push"});
    }
}

void CHTLState::popState() {
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

StateGuard CHTLState::enterState(StateType newState) {
    return StateGuard(this, newState);
}

StateContext* CHTLState::getCurrentContext() {
    return stateStack.empty() ? nullptr : &stateStack.top();
}

const StateContext* CHTLState::getCurrentContext() const {
    return stateStack.empty() ? nullptr : &stateStack.top();
}

std::shared_ptr<BaseNode> CHTLState::getCurrentNode() {
    auto* context = getCurrentContext();
    return context ? context->currentNode : nullptr;
}

bool CHTLState::isInState(StateType state) const {
    return currentState == state;
}

bool CHTLState::isInAnyState(std::initializer_list<StateType> states) const {
    return std::any_of(states.begin(), states.end(), 
                      [this](StateType s) { return isInState(s); });
}

int CHTLState::getStateDepth(StateType state) const {
    std::stack<StateContext> tempStack = stateStack;
    int depth = 0;
    
    while (!tempStack.empty()) {
        if (tempStack.top().type == state) {
            return depth;
        }
        tempStack.pop();
        depth++;
    }
    
    return -1;  // 状态不在栈中
}

bool CHTLState::validateCurrentState() const {
    // 检查状态栈是否为空
    if (stateStack.empty()) {
        return false;
    }
    
    // 检查当前状态是否与栈顶一致
    if (currentState != stateStack.top().type) {
        return false;
    }
    
    // 其他验证规则...
    return true;
}

std::string CHTLState::getStateError() const {
    if (stateStack.empty()) {
        return "State stack is empty";
    }
    
    if (currentState != stateStack.top().type) {
        return "Current state mismatch with stack top";
    }
    
    return "";
}

std::string CHTLState::getStateStackTrace() const {
    std::stringstream ss;
    std::stack<StateContext> tempStack = stateStack;
    std::vector<StateContext> contexts;
    
    // 将栈内容反转到vector中
    while (!tempStack.empty()) {
        contexts.push_back(tempStack.top());
        tempStack.pop();
    }
    
    // 从底到顶输出
    ss << "State Stack Trace:\n";
    for (auto it = contexts.rbegin(); it != contexts.rend(); ++it) {
        ss << "  " << std::string(it->depth * 2, ' ') 
           << stateToString(it->type) << " (depth: " << it->depth << ")\n";
    }
    
    return ss.str();
}

void CHTLState::reset() {
    // 清空栈
    while (!stateStack.empty()) {
        stateStack.pop();
    }
    
    // 重置到初始状态
    currentState = StateType::INITIAL;
    stateStack.push(StateContext(StateType::INITIAL));
    
    // 清空历史
    stateHistory.clear();
}

std::string CHTLState::stateToString(StateType state) {
    switch (state) {
        case StateType::INITIAL: return "INITIAL";
        case StateType::IN_ELEMENT: return "IN_ELEMENT";
        case StateType::IN_TEXT: return "IN_TEXT";
        case StateType::IN_STYLE: return "IN_STYLE";
        case StateType::IN_SCRIPT: return "IN_SCRIPT";
        case StateType::IN_TEMPLATE: return "IN_TEMPLATE";
        case StateType::IN_CUSTOM: return "IN_CUSTOM";
        case StateType::IN_ORIGIN: return "IN_ORIGIN";
        case StateType::IN_CONFIG: return "IN_CONFIG";
        case StateType::IN_NAMESPACE: return "IN_NAMESPACE";
        case StateType::IN_IMPORT: return "IN_IMPORT";
        case StateType::IN_OPERATOR: return "IN_OPERATOR";
        case StateType::IN_ATTRIBUTE: return "IN_ATTRIBUTE";
        case StateType::IN_LITERAL: return "IN_LITERAL";
        case StateType::IN_COMMENT: return "IN_COMMENT";
        default: return "UNKNOWN";
    }
}

} // namespace CHTL