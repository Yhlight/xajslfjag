#include "CHTLState.h"
#include <iostream>
#include <algorithm>

namespace CHTL {

// StateGuard 实现
StateGuard::StateGuard(CHTLState* s, StateType type, const std::string& name)
    : state(s), released(false) {
    if (state) {
        state->pushState(type, name);
    }
}

StateGuard::~StateGuard() {
    if (state && !released) {
        state->popState();
    }
}

StateGuard::StateGuard(StateGuard&& other) noexcept
    : state(other.state), released(other.released) {
    other.state = nullptr;
    other.released = true;
}

StateGuard& StateGuard::operator=(StateGuard&& other) noexcept {
    if (this != &other) {
        if (state && !released) {
            state->popState();
        }
        state = other.state;
        released = other.released;
        other.state = nullptr;
        other.released = true;
    }
    return *this;
}

// CHTLState 实现
CHTLState::CHTLState() : current_line(1), current_column(1) {
    // 初始状态为全局
    pushState(StateType::GLOBAL, "global");
}

void CHTLState::pushState(StateType type, const std::string& name) {
    StateInfo info(type, name, current_line, current_column);
    info.depth = static_cast<int>(state_stack.size());
    state_stack.push(info);
    state_history.push_back(info);
}

void CHTLState::popState() {
    if (!state_stack.empty() && state_stack.size() > 1) {  // 保留全局状态
        state_stack.pop();
    }
}

StateType CHTLState::getCurrentState() const {
    return state_stack.empty() ? StateType::GLOBAL : state_stack.top().type;
}

StateInfo CHTLState::getCurrentStateInfo() const {
    return state_stack.empty() ? StateInfo(StateType::GLOBAL) : state_stack.top();
}

StateGuard CHTLState::enterState(StateType type, const std::string& name) {
    return StateGuard(this, type, name);
}

bool CHTLState::isInState(StateType type) const {
    std::stack<StateInfo> temp = state_stack;
    while (!temp.empty()) {
        if (temp.top().type == type) {
            return true;
        }
        temp.pop();
    }
    return false;
}

bool CHTLState::isInAnyState(std::initializer_list<StateType> types) const {
    for (auto type : types) {
        if (isInState(type)) {
            return true;
        }
    }
    return false;
}

int CHTLState::getStateDepth(StateType type) const {
    int depth = 0;
    std::stack<StateInfo> temp = state_stack;
    while (!temp.empty()) {
        if (temp.top().type == type) {
            depth++;
        }
        temp.pop();
    }
    return depth;
}

std::vector<StateType> CHTLState::getStateStack() const {
    std::vector<StateType> result;
    std::stack<StateInfo> temp = state_stack;
    while (!temp.empty()) {
        result.push_back(temp.top().type);
        temp.pop();
    }
    std::reverse(result.begin(), result.end());
    return result;
}

bool CHTLState::isInGlobalScope() const {
    return getCurrentState() == StateType::GLOBAL;
}

bool CHTLState::isInNamespace() const {
    return isInState(StateType::NAMESPACE);
}

bool CHTLState::isInElement() const {
    return isInState(StateType::ELEMENT);
}

bool CHTLState::isInStyle() const {
    return isInAnyState({StateType::STYLE_GLOBAL, StateType::STYLE_LOCAL});
}

bool CHTLState::isInLocalStyle() const {
    return isInState(StateType::STYLE_LOCAL);
}

bool CHTLState::isInGlobalStyle() const {
    return isInState(StateType::STYLE_GLOBAL);
}

bool CHTLState::isInScript() const {
    return isInAnyState({StateType::SCRIPT_GLOBAL, StateType::SCRIPT_LOCAL});
}

bool CHTLState::isInLocalScript() const {
    return isInState(StateType::SCRIPT_LOCAL);
}

bool CHTLState::isInGlobalScript() const {
    return isInState(StateType::SCRIPT_GLOBAL);
}

bool CHTLState::isInComment() const {
    return isInState(StateType::COMMENT);
}

bool CHTLState::isInString() const {
    return isInState(StateType::STRING_LITERAL);
}

bool CHTLState::isInCHTLJS() const {
    return isInAnyState({StateType::CHTL_JS_EXPRESSION, 
                        StateType::CHTL_JS_SELECTOR, 
                        StateType::CHTL_JS_EVENT});
}

std::string CHTLState::getCurrentElementName() const {
    std::stack<StateInfo> temp = state_stack;
    while (!temp.empty()) {
        if (temp.top().type == StateType::ELEMENT) {
            return temp.top().name;
        }
        temp.pop();
    }
    return "";
}

std::string CHTLState::getCurrentNamespaceName() const {
    std::stack<StateInfo> temp = state_stack;
    while (!temp.empty()) {
        if (temp.top().type == StateType::NAMESPACE) {
            return temp.top().name;
        }
        temp.pop();
    }
    return "";
}

std::string CHTLState::getCurrentSelectorName() const {
    std::stack<StateInfo> temp = state_stack;
    while (!temp.empty()) {
        if (temp.top().type == StateType::STYLE_SELECTOR) {
            return temp.top().name;
        }
        temp.pop();
    }
    return "";
}

void CHTLState::updatePosition(int line, int column) {
    current_line = line;
    current_column = column;
}

bool CHTLState::canUseTemplateVar() const {
    // 模板变量可以在style、script和元素内使用
    return isInStyle() || isInScript() || isInElement();
}

bool CHTLState::canUseCustomVar() const {
    // 自定义变量可以在style、script和元素内使用
    return isInStyle() || isInScript() || isInElement();
}

bool CHTLState::canUseStyleGroup() const {
    // 样式组只能在style块内使用
    return isInStyle();
}

bool CHTLState::canUseDelete() const {
    // delete只能在style块内使用
    return isInStyle();
}

bool CHTLState::canUseInheritance() const {
    // 继承只能在style块内使用
    return isInStyle();
}

bool CHTLState::canUseCHTLJSSyntax() const {
    // CHTL JS语法只能在局部script内使用
    return isInLocalScript() || isInElement();
}

bool CHTLState::canUseOriginEmbed() const {
    // [Origin]原始嵌入任意地方都可以使用
    return true;
}

bool CHTLState::canUseGeneratorComment() const {
    // --注释任意地方都可以使用
    return true;
}

void CHTLState::clear() {
    while (!state_stack.empty()) {
        state_stack.pop();
    }
    state_history.clear();
    current_line = 1;
    current_column = 1;
    // 重新初始化全局状态
    pushState(StateType::GLOBAL, "global");
}

void CHTLState::dumpStack() const {
    std::cout << "State Stack (top to bottom):" << std::endl;
    std::stack<StateInfo> temp = state_stack;
    int level = 0;
    while (!temp.empty()) {
        const auto& info = temp.top();
        std::cout << "  [" << level++ << "] " << getStateString() 
                  << " (" << info.name << ") at " 
                  << info.start_line << ":" << info.start_column << std::endl;
        temp.pop();
    }
}

std::string CHTLState::getStateString() const {
    switch (getCurrentState()) {
        case StateType::GLOBAL: return "GLOBAL";
        case StateType::NAMESPACE: return "NAMESPACE";
        case StateType::ELEMENT: return "ELEMENT";
        case StateType::TEXT_NODE: return "TEXT_NODE";
        case StateType::STYLE_GLOBAL: return "STYLE_GLOBAL";
        case StateType::STYLE_LOCAL: return "STYLE_LOCAL";
        case StateType::STYLE_SELECTOR: return "STYLE_SELECTOR";
        case StateType::STYLE_RULE: return "STYLE_RULE";
        case StateType::STYLE_GROUP: return "STYLE_GROUP";
        case StateType::SCRIPT_GLOBAL: return "SCRIPT_GLOBAL";
        case StateType::SCRIPT_LOCAL: return "SCRIPT_LOCAL";
        case StateType::IMPORT_DECLARATION: return "IMPORT_DECLARATION";
        case StateType::CONFIGURATION: return "CONFIGURATION";
        case StateType::ORIGIN_EMBED: return "ORIGIN_EMBED";
        case StateType::COMMENT: return "COMMENT";
        case StateType::STRING_LITERAL: return "STRING_LITERAL";
        case StateType::TEMPLATE_VAR: return "TEMPLATE_VAR";
        case StateType::CUSTOM_VAR: return "CUSTOM_VAR";
        case StateType::VARIABLE_GROUP: return "VARIABLE_GROUP";
        case StateType::CHTL_JS_EXPRESSION: return "CHTL_JS_EXPRESSION";
        case StateType::CHTL_JS_SELECTOR: return "CHTL_JS_SELECTOR";
        case StateType::CHTL_JS_EVENT: return "CHTL_JS_EVENT";
        default: return "UNKNOWN";
    }
}

} // namespace CHTL