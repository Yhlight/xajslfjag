#include "State.h"
#include <sstream>
#include <algorithm>

namespace CHTL {

// ========== StateInfo 实现 ==========

bool StateInfo::isTemplateState() const {
    return type == CHTLStateType::TEMPLATE_DEFINITION ||
           type == CHTLStateType::TEMPLATE_STYLE ||
           type == CHTLStateType::TEMPLATE_ELEMENT ||
           type == CHTLStateType::TEMPLATE_VAR;
}

bool StateInfo::isCustomState() const {
    return type == CHTLStateType::CUSTOM_DEFINITION ||
           type == CHTLStateType::CUSTOM_STYLE ||
           type == CHTLStateType::CUSTOM_ELEMENT ||
           type == CHTLStateType::CUSTOM_VAR;
}

bool StateInfo::isStructuralState() const {
    return type == CHTLStateType::NAMESPACE_DEFINITION ||
           type == CHTLStateType::CONFIGURATION_DEFINITION ||
           type == CHTLStateType::INFO_DEFINITION ||
           type == CHTLStateType::EXPORT_DEFINITION;
}

bool StateInfo::isOriginState() const {
    return type == CHTLStateType::ORIGIN_HTML ||
           type == CHTLStateType::ORIGIN_STYLE ||
           type == CHTLStateType::ORIGIN_JAVASCRIPT ||
           type == CHTLStateType::ORIGIN_CUSTOM;
}

bool StateInfo::isOperationState() const {
    return type == CHTLStateType::INHERIT_OPERATION ||
           type == CHTLStateType::DELETE_OPERATION ||
           type == CHTLStateType::INSERT_OPERATION ||
           type == CHTLStateType::EXCEPT_CONSTRAINT;
}

std::string StateInfo::toString() const {
    std::stringstream ss;
    ss << "StateInfo{type:" << static_cast<int>(type)
       << ", name:'" << name << "'"
       << ", context:'" << context << "'"
       << ", depth:" << depth
       << ", allowsNesting:" << allowsNesting << "}";
    return ss.str();
}

// ========== StateTransitionRule 实现 ==========

bool StateTransitionRule::canTransition() const {
    if (condition) {
        return condition();
    }
    return true;
}

// ========== RAIIStateManager 实现 ==========

RAIIStateManager::RAIIStateManager(CHTLStateMachine* stateMachine, 
                                   CHTLStateType newState,
                                   const std::string& context)
    : m_stateMachine(stateMachine), m_valid(false) {
    
    if (m_stateMachine) {
        m_valid = m_stateMachine->enterState(newState, context);
    }
}

RAIIStateManager::~RAIIStateManager() {
    if (m_valid && m_stateMachine) {
        m_stateMachine->exitState();
    }
}

const StateInfo& RAIIStateManager::getCurrentState() const {
    static StateInfo invalidState(CHTLStateType::INVALID_STATE);
    if (m_stateMachine) {
        return m_stateMachine->getCurrentState();
    }
    return invalidState;
}

bool RAIIStateManager::isValid() const {
    return m_valid;
}

// ========== CHTLStateMachine 实现 ==========

CHTLStateMachine::CHTLStateMachine() {
    // 初始化为全局状态
    m_stateStack.push(StateInfo(CHTLStateType::GLOBAL, "global", "root", 0, true));
    initializeTransitionRules();
}

const StateInfo& CHTLStateMachine::getCurrentState() const {
    static StateInfo invalidState(CHTLStateType::INVALID_STATE);
    if (m_stateStack.empty()) {
        return invalidState;
    }
    return m_stateStack.top();
}

CHTLStateType CHTLStateMachine::getCurrentStateType() const {
    return getCurrentState().type;
}

size_t CHTLStateMachine::getDepth() const {
    return m_stateStack.size();
}

bool CHTLStateMachine::enterState(CHTLStateType newState, const std::string& context) {
    if (m_stateStack.empty()) {
        reportError("State stack is empty, cannot enter new state");
        return false;
    }
    
    CHTLStateType currentState = getCurrentStateType();
    
    // 检查状态转换是否有效
    if (!isValidTransition(currentState, newState)) {
        std::stringstream ss;
        ss << "Invalid state transition from " << static_cast<int>(currentState) 
           << " to " << static_cast<int>(newState);
        reportError(ss.str());
        return false;
    }
    
    // 创建新状态信息
    size_t newDepth = m_stateStack.size();
    StateInfo newStateInfo(newState, "", context, newDepth, true);
    
    // 设置状态特定属性
    switch (newState) {
        case CHTLStateType::ORIGIN_HTML:
        case CHTLStateType::ORIGIN_STYLE:
        case CHTLStateType::ORIGIN_JAVASCRIPT:
        case CHTLStateType::ORIGIN_CUSTOM:
            newStateInfo.allowsNesting = false;
            break;
        default:
            break;
    }
    
    // 压入状态栈
    m_stateStack.push(newStateInfo);
    recordStateHistory(newStateInfo);
    
    return true;
}

bool CHTLStateMachine::exitState() {
    if (m_stateStack.size() <= 1) {
        reportError("Cannot exit global state");
        return false;
    }
    
    m_stateStack.pop();
    return true;
}

bool CHTLStateMachine::canEnterState(CHTLStateType newState) const {
    if (m_stateStack.empty()) {
        return newState == CHTLStateType::GLOBAL;
    }
    
    CHTLStateType currentState = getCurrentStateType();
    return isValidTransition(currentState, newState);
}

bool CHTLStateMachine::isInState(CHTLStateType state) const {
    return getCurrentStateType() == state;
}

bool CHTLStateMachine::isInStateOrSubState(CHTLStateType state) const {
    std::stack<StateInfo> tempStack = m_stateStack;
    while (!tempStack.empty()) {
        if (tempStack.top().type == state) {
            return true;
        }
        tempStack.pop();
    }
    return false;
}

void CHTLStateMachine::reset() {
    // 清空状态栈
    while (!m_stateStack.empty()) {
        m_stateStack.pop();
    }
    
    // 重新初始化为全局状态
    m_stateStack.push(StateInfo(CHTLStateType::GLOBAL, "global", "root", 0, true));
    m_history.clear();
}

std::unique_ptr<RAIIStateManager> CHTLStateMachine::createRAIIManager(CHTLStateType newState, 
                                                                       const std::string& context) {
    return std::make_unique<RAIIStateManager>(this, newState, context);
}

void CHTLStateMachine::setErrorCallback(std::function<void(const std::string&)> callback) {
    m_errorCallback = callback;
}

std::vector<StateInfo> CHTLStateMachine::getStateHistory() const {
    return m_history;
}

// ========== 私有方法实现 ==========

void CHTLStateMachine::initializeTransitionRules() {
    // 从全局状态可以进入的状态
    m_rules.emplace_back(CHTLStateType::GLOBAL, CHTLStateType::ELEMENT, "html_element");
    m_rules.emplace_back(CHTLStateType::GLOBAL, CHTLStateType::TEMPLATE_DEFINITION, "[Template]");
    m_rules.emplace_back(CHTLStateType::GLOBAL, CHTLStateType::CUSTOM_DEFINITION, "[Custom]");
    m_rules.emplace_back(CHTLStateType::GLOBAL, CHTLStateType::IMPORT_STATEMENT, "[Import]");
    m_rules.emplace_back(CHTLStateType::GLOBAL, CHTLStateType::NAMESPACE_DEFINITION, "[Namespace]");
    m_rules.emplace_back(CHTLStateType::GLOBAL, CHTLStateType::CONFIGURATION_DEFINITION, "[Configuration]");
    m_rules.emplace_back(CHTLStateType::GLOBAL, CHTLStateType::ORIGIN_HTML, "[Origin] @Html");
    m_rules.emplace_back(CHTLStateType::GLOBAL, CHTLStateType::ORIGIN_STYLE, "[Origin] @Style");
    m_rules.emplace_back(CHTLStateType::GLOBAL, CHTLStateType::ORIGIN_JAVASCRIPT, "[Origin] @JavaScript");
    
    // 从元素状态可以进入的状态
    m_rules.emplace_back(CHTLStateType::ELEMENT, CHTLStateType::ELEMENT, "html_element");
    m_rules.emplace_back(CHTLStateType::ELEMENT, CHTLStateType::STYLE_BLOCK, "style");
    m_rules.emplace_back(CHTLStateType::ELEMENT, CHTLStateType::TEXT_BLOCK, "text");
    m_rules.emplace_back(CHTLStateType::ELEMENT, CHTLStateType::ATTRIBUTE_DEFINITION, "attribute");
    
    // 从样式块状态可以进入的状态
    m_rules.emplace_back(CHTLStateType::STYLE_BLOCK, CHTLStateType::SELECTOR_STYLE, "css_selector");
    
    // 从模板定义状态可以进入的状态
    m_rules.emplace_back(CHTLStateType::TEMPLATE_DEFINITION, CHTLStateType::TEMPLATE_STYLE, "@Style");
    m_rules.emplace_back(CHTLStateType::TEMPLATE_DEFINITION, CHTLStateType::TEMPLATE_ELEMENT, "@Element");
    m_rules.emplace_back(CHTLStateType::TEMPLATE_DEFINITION, CHTLStateType::TEMPLATE_VAR, "@Var");
    
    // 从自定义定义状态可以进入的状态
    m_rules.emplace_back(CHTLStateType::CUSTOM_DEFINITION, CHTLStateType::CUSTOM_STYLE, "@Style");
    m_rules.emplace_back(CHTLStateType::CUSTOM_DEFINITION, CHTLStateType::CUSTOM_ELEMENT, "@Element");
    m_rules.emplace_back(CHTLStateType::CUSTOM_DEFINITION, CHTLStateType::CUSTOM_VAR, "@Var");
    
    // 操作状态转换
    m_rules.emplace_back(CHTLStateType::TEMPLATE_STYLE, CHTLStateType::INHERIT_OPERATION, "inherit");
    m_rules.emplace_back(CHTLStateType::CUSTOM_STYLE, CHTLStateType::INHERIT_OPERATION, "inherit");
    m_rules.emplace_back(CHTLStateType::CUSTOM_STYLE, CHTLStateType::DELETE_OPERATION, "delete");
    m_rules.emplace_back(CHTLStateType::CUSTOM_ELEMENT, CHTLStateType::INSERT_OPERATION, "insert");
    
    // 约束状态转换
    m_rules.emplace_back(CHTLStateType::ELEMENT, CHTLStateType::EXCEPT_CONSTRAINT, "except");
    m_rules.emplace_back(CHTLStateType::NAMESPACE_DEFINITION, CHTLStateType::EXCEPT_CONSTRAINT, "except");
}

bool CHTLStateMachine::isValidTransition(CHTLStateType from, CHTLStateType to) const {
    // 检查是否存在对应的转换规则
    for (const auto& rule : m_rules) {
        if (rule.fromState == from && rule.toState == to) {
            return rule.canTransition();
        }
    }
    
    // 特殊情况：相同状态之间的转换（用于嵌套）
    if (from == to) {
        const StateInfo& currentState = getCurrentState();
        return currentState.allowsNesting;
    }
    
    return false;
}

void CHTLStateMachine::reportError(const std::string& message) {
    if (m_errorCallback) {
        m_errorCallback(message);
    }
}

void CHTLStateMachine::recordStateHistory(const StateInfo& state) {
    m_history.push_back(state);
    
    // 限制历史记录大小，避免内存占用过大
    if (m_history.size() > 1000) {
        m_history.erase(m_history.begin(), m_history.begin() + 100);
    }
}

} // namespace CHTL