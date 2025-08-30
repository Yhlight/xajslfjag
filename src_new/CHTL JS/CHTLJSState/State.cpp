#include "State.h"
#include <sstream>

namespace CHTLJS {

bool StateInfo::isCHTLJSSpecificState() const {
    return type == CHTLJSStateType::ENHANCED_SELECTOR ||
           type == CHTLJSStateType::EVENT_LISTENER ||
           type == CHTLJSStateType::EVENT_DELEGATE ||
           type == CHTLJSStateType::ANIMATION_BLOCK ||
           type == CHTLJSStateType::VIRTUAL_OBJECT ||
           type == CHTLJSStateType::EVENT_BINDING ||
           type == CHTLJSStateType::CHTLJS_FUNCTION;
}

bool StateInfo::isEventState() const {
    return type == CHTLJSStateType::EVENT_LISTENER ||
           type == CHTLJSStateType::EVENT_DELEGATE ||
           type == CHTLJSStateType::EVENT_BINDING;
}

std::string StateInfo::toString() const {
    std::ostringstream oss;
    oss << "State(" << static_cast<int>(type) << ", " << name << ", depth=" << depth << ")";
    return oss.str();
}

CHTLJSStateMachine::CHTLJSStateMachine() {
    // 初始化为全局状态
    m_stateStack.push(StateInfo(CHTLJSStateType::GLOBAL, "global", "", 0, true));
}

void CHTLJSStateMachine::enterState(CHTLJSStateType type, const std::string& name, const std::string& context) {
    if (!canEnterState(type)) {
        return; // 无法进入该状态
    }
    
    size_t newDepth = m_stateStack.empty() ? 0 : m_stateStack.top().depth + 1;
    StateInfo newState(type, name, context, newDepth, true);
    
    m_stateStack.push(newState);
}

void CHTLJSStateMachine::exitState() {
    if (m_stateStack.size() > 1) { // 保持至少一个状态
        m_stateStack.pop();
    }
}

StateInfo CHTLJSStateMachine::getCurrentState() const {
    if (m_stateStack.empty()) {
        return StateInfo(CHTLJSStateType::INVALID_STATE);
    }
    return m_stateStack.top();
}

size_t CHTLJSStateMachine::getDepth() const {
    return m_stateStack.size();
}

bool CHTLJSStateMachine::canEnterState(CHTLJSStateType type) const {
    if (m_stateStack.empty()) {
        return type == CHTLJSStateType::GLOBAL;
    }
    
    CHTLJSStateType currentType = m_stateStack.top().type;
    return validateStateTransition(currentType, type);
}

void CHTLJSStateMachine::reset() {
    while (!m_stateStack.empty()) {
        m_stateStack.pop();
    }
    m_stateStack.push(StateInfo(CHTLJSStateType::GLOBAL, "global", "", 0, true));
}

bool CHTLJSStateMachine::validateStateTransition(CHTLJSStateType from, CHTLJSStateType to) const {
    // 简化的状态转换验证
    switch (from) {
        case CHTLJSStateType::GLOBAL:
            return to == CHTLJSStateType::SCRIPT_BLOCK || 
                   to == CHTLJSStateType::MODULE_BLOCK;
                   
        case CHTLJSStateType::SCRIPT_BLOCK:
            return to == CHTLJSStateType::ENHANCED_SELECTOR ||
                   to == CHTLJSStateType::EVENT_LISTENER ||
                   to == CHTLJSStateType::EVENT_DELEGATE ||
                   to == CHTLJSStateType::ANIMATION_BLOCK ||
                   to == CHTLJSStateType::VIRTUAL_OBJECT ||
                   to == CHTLJSStateType::EVENT_BINDING ||
                   to == CHTLJSStateType::CHTLJS_FUNCTION;
                   
        case CHTLJSStateType::MODULE_BLOCK:
            return to == CHTLJSStateType::SCRIPT_BLOCK;
            
        default:
            return true; // 允许大多数转换
    }
}

StateGuard::StateGuard(CHTLJSStateMachine& stateMachine, CHTLJSStateType type, 
                       const std::string& name, const std::string& context)
    : m_stateMachine(stateMachine), m_shouldExit(true) {
    m_stateMachine.enterState(type, name, context);
}

StateGuard::~StateGuard() {
    if (m_shouldExit) {
        m_stateMachine.exitState();
    }
}

} // namespace CHTLJS