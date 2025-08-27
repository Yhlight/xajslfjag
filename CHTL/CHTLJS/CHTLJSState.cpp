#include "CHTLJSState.h"
#include <sstream>
#include <vector>

namespace CHTLJS {

// CHTLJSState实现
CHTLJSState::CHTLJSState() {
    // 初始状态为全局作用域
    stateStack.push(ParseState::GLOBAL);
}

CHTLJSState::~CHTLJSState() = default;

void CHTLJSState::PushState(ParseState state) {
    stateStack.push(state);
}

void CHTLJSState::PopState() {
    if (!stateStack.empty() && stateStack.size() > 1) {
        stateStack.pop();
    }
}

ParseState CHTLJSState::GetCurrentState() const {
    if (!stateStack.empty()) {
        return stateStack.top();
    }
    return ParseState::GLOBAL;
}

bool CHTLJSState::IsInState(ParseState state) const {
    std::stack<ParseState> temp = stateStack;
    while (!temp.empty()) {
        if (temp.top() == state) {
            return true;
        }
        temp.pop();
    }
    return false;
}

bool CHTLJSState::IsInEnhancedSelector() const {
    return IsInState(ParseState::ENHANCED_SELECTOR);
}

bool CHTLJSState::IsInCHTLJSFunction() const {
    return IsInState(ParseState::CHTLJS_FUNCTION);
}

bool CHTLJSState::IsInVirDeclaration() const {
    return IsInState(ParseState::VIR_DECLARATION);
}

bool CHTLJSState::IsInEventBinding() const {
    return IsInState(ParseState::EVENT_BINDING);
}

bool CHTLJSState::IsInFunction() const {
    return IsInState(ParseState::FUNCTION);
}

bool CHTLJSState::IsInGlobalScope() const {
    return GetCurrentState() == ParseState::GLOBAL;
}

void CHTLJSState::SetCurrentCHTLJSFunction(const std::string& name) {
    currentCHTLJSFunction = name;
}

std::string CHTLJSState::GetCurrentCHTLJSFunction() const {
    return currentCHTLJSFunction;
}

void CHTLJSState::EnterVirtualObject(const std::string& name) {
    virtualObjectStack.push(name);
}

void CHTLJSState::ExitVirtualObject() {
    if (!virtualObjectStack.empty()) {
        virtualObjectStack.pop();
    }
}

std::string CHTLJSState::GetCurrentVirtualObject() const {
    if (!virtualObjectStack.empty()) {
        return virtualObjectStack.top();
    }
    return "";
}

bool CHTLJSState::IsInVirtualObject() const {
    return !virtualObjectStack.empty();
}

std::string CHTLJSState::GetStateString() const {
    std::stringstream ss;
    std::stack<ParseState> temp = stateStack;
    std::vector<std::string> states;
    
    while (!temp.empty()) {
        ParseState state = temp.top();
        temp.pop();
        
        switch (state) {
            case ParseState::GLOBAL:
                states.push_back("GLOBAL");
                break;
            case ParseState::FUNCTION:
                states.push_back("FUNCTION");
                break;
            case ParseState::BLOCK:
                states.push_back("BLOCK");
                break;
            case ParseState::OBJECT_LITERAL:
                states.push_back("OBJECT_LITERAL");
                break;
            case ParseState::ARRAY_LITERAL:
                states.push_back("ARRAY_LITERAL");
                break;
            case ParseState::ENHANCED_SELECTOR:
                states.push_back("ENHANCED_SELECTOR");
                break;
            case ParseState::CHTLJS_FUNCTION:
                states.push_back("CHTLJS_FUNCTION(" + currentCHTLJSFunction + ")");
                break;
            case ParseState::VIR_DECLARATION:
                states.push_back("VIR_DECLARATION");
                break;
            case ParseState::EVENT_BINDING:
                states.push_back("EVENT_BINDING");
                break;
        }
    }
    
    // 反转顺序，从底到顶显示
    for (auto it = states.rbegin(); it != states.rend(); ++it) {
        if (it != states.rbegin()) ss << " -> ";
        ss << *it;
    }
    
    return ss.str();
}

// StateGuard实现
StateGuard::StateGuard(std::shared_ptr<CHTLJSState> state, ParseState newState)
    : state(state), active(true) {
    if (state) {
        state->PushState(newState);
    }
}

StateGuard::~StateGuard() {
    if (active && state) {
        state->PopState();
    }
}

StateGuard::StateGuard(StateGuard&& other) noexcept
    : state(std::move(other.state)), active(other.active) {
    other.active = false;
}

StateGuard& StateGuard::operator=(StateGuard&& other) noexcept {
    if (this != &other) {
        if (active && state) {
            state->PopState();
        }
        state = std::move(other.state);
        active = other.active;
        other.active = false;
    }
    return *this;
}

} // namespace CHTLJS