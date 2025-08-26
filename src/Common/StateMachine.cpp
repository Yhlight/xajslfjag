#include "StateMachine.h"
#include <algorithm>
#include <regex>
#include <iostream>

namespace CHTL {
namespace Common {

// StateGuard实现
StateGuard::StateGuard(StateMachine* machine, StateType state, const std::string& name, size_t pos)
    : machine(machine), released(false) {
    if (machine) {
        state_node = StateNode(state, name, pos);
        machine->state_stack.push(state_node);
        machine->notifyStateEnter(state_node);
    }
}

StateGuard::~StateGuard() {
    if (!released && machine) {
        machine->popState();
    }
}

StateGuard::StateGuard(StateGuard&& other) noexcept 
    : machine(other.machine), state_node(std::move(other.state_node)), released(other.released) {
    other.machine = nullptr;
    other.released = true;
}

StateGuard& StateGuard::operator=(StateGuard&& other) noexcept {
    if (this != &other) {
        if (!released && machine) {
            machine->popState();
        }
        machine = other.machine;
        state_node = std::move(other.state_node);
        released = other.released;
        other.machine = nullptr;
        other.released = true;
    }
    return *this;
}

void StateGuard::release() {
    if (!released && machine) {
        machine->popState();
        released = true;
    }
}

void StateGuard::setContextData(const std::string& key, const std::string& value) {
    if (!released) {
        state_node.setContextData(key, value);
    }
}

std::string StateGuard::getContextData(const std::string& key, const std::string& default_val) const {
    return state_node.getContextData(key, default_val);
}

// StateMachine实现
StateMachine::StateMachine() 
    : strict_mode(false), debug_mode(false), enable_auto_recovery(true) {
    initializeStandardRules();
}

StateMachine::~StateMachine() = default;

void StateMachine::initializeStandardRules() {
    initializeCHTLRules();
    initializeCHTLJSRules();
    setupNewCHTLJSSyntaxTransitions(); // 新语法支持
}

void StateMachine::initializeCHTLRules() {
    setupCHTLTransitions();
}

void StateMachine::initializeCHTLJSRules() {
    setupCHTLJSTransitions();
}

void StateMachine::setupNewCHTLJSSyntaxTransitions() {
    // 新语法：listen { } 转换规则
    addTransitionRule(TransitionRule(
        StateType::LOCAL_SCRIPT_BLOCK,
        StateType::LISTEN_FUNCTION,
        StateTransitionEvent::CHTLJS_FUNCTION_START,
        R"(\{\{[^}]+\}\}\s*->\s*listen\s*\{)", // {{selector}}->listen {
        10 // 高优先级
    ));
    
    // 新语法：delegate { } 转换规则
    addTransitionRule(TransitionRule(
        StateType::LOCAL_SCRIPT_BLOCK,
        StateType::DELEGATE_FUNCTION,
        StateTransitionEvent::CHTLJS_FUNCTION_START,
        R"(\{\{[^}]+\}\}\s*->\s*delegate\s*\{)", // {{selector}}->delegate {
        10 // 高优先级
    ));
    
    // 新语法：animate { } 转换规则
    addTransitionRule(TransitionRule(
        StateType::LOCAL_SCRIPT_BLOCK,
        StateType::ANIMATE_FUNCTION,
        StateTransitionEvent::CHTLJS_FUNCTION_START,
        R"(\banimate\s*\{)", // animate {
        10 // 高优先级
    ));
    
    // 新语法：vir对象声明
    addTransitionRule(TransitionRule(
        StateType::LOCAL_SCRIPT_BLOCK,
        StateType::VIR_OBJECT,
        StateTransitionEvent::KEYWORD_DETECTED,
        R"(\bvir\s+\w+\s*=\s*(listen|iNeverAway)\s*\{)", // vir name = function {
        10 // 高优先级
    ));
    
    // CHTL JS函数块结束转换
    addTransitionRule(TransitionRule(
        StateType::LISTEN_FUNCTION,
        StateType::LOCAL_SCRIPT_BLOCK,
        StateTransitionEvent::CHTLJS_FUNCTION_BLOCK_END,
        R"(\}\s*;?)", // }; 或 }
        5
    ));
    
    addTransitionRule(TransitionRule(
        StateType::DELEGATE_FUNCTION,
        StateType::LOCAL_SCRIPT_BLOCK,
        StateTransitionEvent::CHTLJS_FUNCTION_BLOCK_END,
        R"(\}\s*;?)", // }; 或 }
        5
    ));
    
    addTransitionRule(TransitionRule(
        StateType::ANIMATE_FUNCTION,
        StateType::LOCAL_SCRIPT_BLOCK,
        StateTransitionEvent::CHTLJS_FUNCTION_BLOCK_END,
        R"(\}\s*;?)", // }; 或 }
        5
    ));
    
    // CSS块在animate中的转换
    addTransitionRule(TransitionRule(
        StateType::ANIMATE_FUNCTION,
        StateType::LOCAL_STYLE_BLOCK,
        StateTransitionEvent::CSS_BLOCK_IN_CHTLJS,
        R"(\b(begin|end)\s*:\s*\{)", // begin: { 或 end: {
        8
    ));
    
    // when关键帧数组转换
    addTransitionRule(TransitionRule(
        StateType::ANIMATE_FUNCTION,
        StateType::CHTLJS_KEYVALUE_BLOCK,
        StateTransitionEvent::KEYWORD_DETECTED,
        R"(\bwhen\s*:\s*\[)", // when: [
        8
    ));
}

void StateMachine::setupCHTLTransitions() {
    // 基本CHTL转换规则
    addTransitionRule(TransitionRule(
        StateType::ROOT_STATE,
        StateType::HTML_ELEMENT,
        StateTransitionEvent::KEYWORD_DETECTED,
        R"(\b(html|head|body|div|span|p|h[1-6]|ul|ol|li|a|img|input|button|form|table|tr|td|th)\s*\{)",
        5
    ));
    
    // 样式块转换
    addTransitionRule(TransitionRule(
        StateType::HTML_ELEMENT,
        StateType::LOCAL_STYLE_BLOCK,
        StateTransitionEvent::KEYWORD_DETECTED,
        R"(\bstyle\s*\{)",
        7
    ));
    
    // 脚本块转换
    addTransitionRule(TransitionRule(
        StateType::HTML_ELEMENT,
        StateType::LOCAL_SCRIPT_BLOCK,
        StateTransitionEvent::KEYWORD_DETECTED,
        R"(\bscript\s*\{)",
        7
    ));
}

void StateMachine::setupCHTLJSTransitions() {
    // 增强选择器转换
    addTransitionRule(TransitionRule(
        StateType::LOCAL_SCRIPT_BLOCK,
        StateType::ENHANCED_SELECTOR,
        StateTransitionEvent::ENHANCED_SELECTOR_START,
        R"(\{\{)",
        9
    ));
    
    // 箭头操作符转换
    addTransitionRule(TransitionRule(
        StateType::ENHANCED_SELECTOR,
        StateType::ARROW_OPERATION,
        StateTransitionEvent::ARROW_OPERATOR_DETECTED,
        R"(\}\}\s*->)",
        9
    ));
}

bool StateMachine::isNewSyntaxFunction(const std::string& input) const {
    // 检测新语法模式
    static const std::vector<std::regex> new_syntax_patterns = {
        std::regex(R"(\{\{[^}]+\}\}\s*->\s*listen\s*\{)"),      // {{}}->listen {
        std::regex(R"(\{\{[^}]+\}\}\s*->\s*delegate\s*\{)"),    // {{}}->delegate {
        std::regex(R"(\banimate\s*\{)"),                        // animate {
        std::regex(R"(\bvir\s+\w+\s*=\s*(listen|iNeverAway)\s*\{)") // vir obj = func {
    };
    
    for (const auto& pattern : new_syntax_patterns) {
        if (std::regex_search(input, pattern)) {
            return true;
        }
    }
    return false;
}

StateType StateMachine::detectNewSyntaxState(const std::string& function_name) const {
    if (function_name == "listen") return StateType::LISTEN_FUNCTION;
    if (function_name == "delegate") return StateType::DELEGATE_FUNCTION;
    if (function_name == "animate") return StateType::ANIMATE_FUNCTION;
    if (function_name == "iNeverAway") return StateType::VIR_OBJECT;
    return StateType::UNKNOWN_STATE;
}

void StateMachine::reset() {
    while (!state_stack.empty()) {
        state_stack.pop();
    }
    state_history.clear();
    errors.clear();
    warnings.clear();
    
    // 推入根状态
    state_stack.push(StateNode(StateType::ROOT_STATE, "root", 0));
}

bool StateMachine::transitionTo(StateType new_state, const std::string& trigger_input, size_t position) {
    if (state_stack.empty()) {
        reportError("Cannot transition: state stack is empty", position);
        return false;
    }
    
    StateNode current = state_stack.top();
    
    // 验证转换的有效性
    if (!isValidTransition(current.type, new_state)) {
        if (strict_mode) {
            reportError("Invalid state transition from " + getStateName(current.type) + 
                       " to " + getStateName(new_state), position);
            return false;
        }
    }
    
    // 执行转换
    StateNode new_node(new_state, getStateName(new_state), position);
    state_stack.push(new_node);
    state_history.push_back(new_node);
    
    notifyStateTransition(current, new_node);
    notifyStateEnter(new_node);
    
    return true;
}

bool StateMachine::autoTransition(const std::string& input, size_t position) {
    if (state_stack.empty()) {
        reset();
    }
    
    StateType current_state = getCurrentState();
    
    // 检测新语法函数
    if (isNewSyntaxFunction(input)) {
        std::regex listen_pattern(R"(\{\{[^}]+\}\}\s*->\s*listen\s*\{)");
        std::regex delegate_pattern(R"(\{\{[^}]+\}\}\s*->\s*delegate\s*\{)");
        std::regex animate_pattern(R"(\banimate\s*\{)");
        
        if (std::regex_search(input, listen_pattern)) {
            return transitionTo(StateType::LISTEN_FUNCTION, input, position);
        } else if (std::regex_search(input, delegate_pattern)) {
            return transitionTo(StateType::DELEGATE_FUNCTION, input, position);
        } else if (std::regex_search(input, animate_pattern)) {
            return transitionTo(StateType::ANIMATE_FUNCTION, input, position);
        }
    }
    
    // 寻找匹配的转换规则
    auto rule = findMatchingRule(current_state, StateTransitionEvent::KEYWORD_DETECTED, input);
    if (rule) {
        return transitionTo(rule->to_state, input, position);
    }
    
    return false;
}

std::unique_ptr<StateGuard> StateMachine::pushState(StateType state, const std::string& name, size_t position) {
    return std::make_unique<StateGuard>(this, state, name, position);
}

bool StateMachine::popState(size_t end_position) {
    if (state_stack.empty()) {
        reportError("Cannot pop state: stack is empty", end_position);
        return false;
    }
    
    StateNode current = state_stack.top();
    current.close(end_position);
    state_stack.pop();
    
    notifyStateExit(current);
    
    return true;
}

StateType StateMachine::getCurrentState() const {
    if (state_stack.empty()) {
        return StateType::UNKNOWN_STATE;
    }
    return state_stack.top().type;
}

const StateNode& StateMachine::getCurrentStateNode() const {
    static StateNode unknown(StateType::UNKNOWN_STATE, "unknown", 0);
    if (state_stack.empty()) {
        return unknown;
    }
    return state_stack.top();
}

bool StateMachine::isInCHTLJSContext() const {
    StateType current = getCurrentState();
    return current == StateType::LOCAL_SCRIPT_BLOCK ||
           current == StateType::ENHANCED_SELECTOR ||
           current == StateType::LISTEN_FUNCTION ||
           current == StateType::DELEGATE_FUNCTION ||
           current == StateType::ANIMATE_FUNCTION ||
           current == StateType::VIR_OBJECT ||
           current == StateType::ARROW_OPERATION;
}

bool StateMachine::canUseCHTLJSSyntax() const {
    return isInCHTLJSContext();
}

bool StateMachine::canUseEnhancedSelectors() const {
    StateType current = getCurrentState();
    return current == StateType::LOCAL_SCRIPT_BLOCK ||
           current == StateType::LISTEN_FUNCTION ||
           current == StateType::DELEGATE_FUNCTION ||
           current == StateType::ANIMATE_FUNCTION;
}

bool StateMachine::canUseArrowOperator() const {
    StateType current = getCurrentState();
    return current == StateType::ENHANCED_SELECTOR ||
           current == StateType::LOCAL_SCRIPT_BLOCK;
}

void StateMachine::addTransitionRule(const TransitionRule& rule) {
    transition_rules.push_back(rule);
    rules_by_state[rule.from_state].push_back(rule);
}

std::optional<TransitionRule> StateMachine::findMatchingRule(StateType current_state, 
                                                            StateTransitionEvent event, 
                                                            const std::string& input) const {
    auto it = rules_by_state.find(current_state);
    if (it == rules_by_state.end()) {
        return std::nullopt;
    }
    
    // 按优先级排序查找
    std::vector<TransitionRule> sorted_rules = it->second;
    std::sort(sorted_rules.begin(), sorted_rules.end(), 
              [](const TransitionRule& a, const TransitionRule& b) {
                  return a.priority > b.priority;
              });
    
    for (const auto& rule : sorted_rules) {
        if (rule.trigger_event == event && rule.isValid(getCurrentStateNode(), input)) {
            return rule;
        }
    }
    
    return std::nullopt;
}

bool StateMachine::isValidTransition(StateType from, StateType to) const {
    // 实现转换验证逻辑
    return true; // 简化实现
}

void StateMachine::reportError(const std::string& error, size_t position) {
    errors.push_back("Error at position " + std::to_string(position) + ": " + error);
}

void StateMachine::notifyStateEnter(const StateNode& state) {
    for (auto& handler : event_handlers) {
        handler->onStateEnter(state);
    }
}

void StateMachine::notifyStateExit(const StateNode& state) {
    for (auto& handler : event_handlers) {
        handler->onStateExit(state);
    }
}

void StateMachine::notifyStateTransition(const StateNode& from, const StateNode& to) {
    for (auto& handler : event_handlers) {
        handler->onStateTransition(from, to);
    }
}

std::string StateMachine::getStateName(StateType state) const {
    switch (state) {
        case StateType::ROOT_STATE: return "ROOT_STATE";
        case StateType::HTML_ELEMENT: return "HTML_ELEMENT";
        case StateType::LOCAL_STYLE_BLOCK: return "LOCAL_STYLE_BLOCK";
        case StateType::LOCAL_SCRIPT_BLOCK: return "LOCAL_SCRIPT_BLOCK";
        case StateType::ENHANCED_SELECTOR: return "ENHANCED_SELECTOR";
        case StateType::LISTEN_FUNCTION: return "LISTEN_FUNCTION";
        case StateType::DELEGATE_FUNCTION: return "DELEGATE_FUNCTION";
        case StateType::ANIMATE_FUNCTION: return "ANIMATE_FUNCTION";
        case StateType::VIR_OBJECT: return "VIR_OBJECT";
        case StateType::ARROW_OPERATION: return "ARROW_OPERATION";
        default: return "UNKNOWN";
    }
}

// TransitionRule的私有方法实现
bool TransitionRule::matchesPattern(const std::string& input, const std::string& pattern) const {
    try {
        std::regex regex_pattern(pattern, std::regex_constants::icase);
        return std::regex_search(input, regex_pattern);
    } catch (const std::regex_error& e) {
        return false;
    }
}

} // namespace Common
} // namespace CHTL