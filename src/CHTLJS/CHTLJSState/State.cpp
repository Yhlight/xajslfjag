#include "State.h"
#include <sstream>
#include <algorithm>

namespace CHTLJS {

// 初始化状态转换规则
const std::unordered_map<StateType, std::vector<StateType>> StateManager::transitionRules_ = 
    StateManager::initTransitionRules();

std::unordered_map<StateType, std::vector<StateType>> StateManager::initTransitionRules() {
    std::unordered_map<StateType, std::vector<StateType>> rules;
    
    // 全局状态可以转换到的状态
    rules[StateType::GLOBAL] = {
        StateType::IN_MODULE_BLOCK,
        StateType::IN_SCRIPT_BLOCK,
        StateType::IN_FUNCTION,
        StateType::IN_VIRTUAL_OBJECT,
        StateType::IN_COMMENT,
        StateType::IN_STRING_LITERAL
    };
    
    // script块内可以转换到的状态
    rules[StateType::IN_SCRIPT_BLOCK] = {
        StateType::IN_ENHANCED_SELECTOR,
        StateType::IN_LISTEN_BLOCK,
        StateType::IN_DELEGATE_BLOCK,
        StateType::IN_ANIMATE_BLOCK,
        StateType::IN_INEVERAWAY_BLOCK,
        StateType::IN_VIRTUAL_OBJECT,
        StateType::IN_FUNCTION,
        StateType::IN_OBJECT_LITERAL,
        StateType::IN_ARRAY_LITERAL,
        StateType::IN_EXPRESSION,
        StateType::IN_EVENT_BINDING,
        StateType::IN_STRING_LITERAL,
        StateType::IN_COMMENT
    };
    
    // module块内的状态
    rules[StateType::IN_MODULE_BLOCK] = {
        StateType::IN_PROPERTY_DEFINITION,
        StateType::IN_STRING_LITERAL,
        StateType::IN_COMMENT
    };
    
    // 增强选择器内的状态
    rules[StateType::IN_ENHANCED_SELECTOR] = {
        StateType::IN_PROPERTY_ACCESS,
        StateType::IN_CALL_EXPRESSION,
        StateType::IN_EVENT_BINDING
    };
    
    // listen块内的状态
    rules[StateType::IN_LISTEN_BLOCK] = {
        StateType::IN_PROPERTY_DEFINITION,
        StateType::IN_FUNCTION,
        StateType::IN_ARROW_FUNCTION,
        StateType::IN_STRING_LITERAL,
        StateType::IN_COMMENT
    };
    
    // delegate块内的状态
    rules[StateType::IN_DELEGATE_BLOCK] = {
        StateType::IN_PROPERTY_DEFINITION,
        StateType::IN_ENHANCED_SELECTOR,
        StateType::IN_ARRAY_LITERAL,
        StateType::IN_FUNCTION,
        StateType::IN_ARROW_FUNCTION,
        StateType::IN_STRING_LITERAL,
        StateType::IN_COMMENT
    };
    
    // animate块内的状态
    rules[StateType::IN_ANIMATE_BLOCK] = {
        StateType::IN_PROPERTY_DEFINITION,
        StateType::IN_ANIMATE_BEGIN,
        StateType::IN_ANIMATE_WHEN,
        StateType::IN_ANIMATE_END,
        StateType::IN_ENHANCED_SELECTOR,
        StateType::IN_ARRAY_LITERAL,
        StateType::IN_OBJECT_LITERAL,
        StateType::IN_STRING_LITERAL,
        StateType::IN_COMMENT
    };
    
    // animate子块的状态
    rules[StateType::IN_ANIMATE_BEGIN] = {
        StateType::IN_PROPERTY_DEFINITION,
        StateType::IN_STRING_LITERAL,
        StateType::IN_COMMENT
    };
    
    rules[StateType::IN_ANIMATE_WHEN] = {
        StateType::IN_OBJECT_LITERAL,
        StateType::IN_PROPERTY_DEFINITION,
        StateType::IN_STRING_LITERAL,
        StateType::IN_COMMENT
    };
    
    rules[StateType::IN_ANIMATE_END] = {
        StateType::IN_PROPERTY_DEFINITION,
        StateType::IN_STRING_LITERAL,
        StateType::IN_COMMENT
    };
    
    // iNeverAway块内的状态
    rules[StateType::IN_INEVERAWAY_BLOCK] = {
        StateType::IN_PROPERTY_DEFINITION,
        StateType::IN_FUNCTION,
        StateType::IN_OBJECT_LITERAL,
        StateType::IN_STRING_LITERAL,
        StateType::IN_COMMENT
    };
    
    // 函数内的状态
    rules[StateType::IN_FUNCTION] = {
        StateType::IN_EXPRESSION,
        StateType::IN_ENHANCED_SELECTOR,
        StateType::IN_LISTEN_BLOCK,
        StateType::IN_DELEGATE_BLOCK,
        StateType::IN_ANIMATE_BLOCK,
        StateType::IN_FUNCTION,  // 嵌套函数
        StateType::IN_OBJECT_LITERAL,
        StateType::IN_ARRAY_LITERAL,
        StateType::IN_STRING_LITERAL,
        StateType::IN_COMMENT
    };
    
    // 对象字面量内的状态
    rules[StateType::IN_OBJECT_LITERAL] = {
        StateType::IN_PROPERTY_DEFINITION,
        StateType::IN_FUNCTION,
        StateType::IN_ARROW_FUNCTION,
        StateType::IN_OBJECT_LITERAL,  // 嵌套对象
        StateType::IN_ARRAY_LITERAL,
        StateType::IN_STRING_LITERAL,
        StateType::IN_COMMENT
    };
    
    return rules;
}

StateManager::StateManager() {
    // 初始化为全局状态
    pushState(StateType::GLOBAL, "global");
}

void StateManager::pushState(StateType type, const std::string& name, 
                            size_t line, size_t col) {
    // 验证状态转换是否合法
    if (!canTransitionTo(type)) {
        std::stringstream ss;
        ss << "Invalid state transition from " << static_cast<int>(getCurrentState())
           << " to " << static_cast<int>(type);
        throw StateException(ss.str());
    }
    
    stateStack_.emplace(type, name, line, col);
}

void StateManager::popState() {
    if (stateStack_.size() <= 1) {  // 保留全局状态
        throw StateException("Cannot pop global state");
    }
    stateStack_.pop();
}

StateType StateManager::getCurrentState() const {
    if (stateStack_.empty()) {
        return StateType::GLOBAL;
    }
    return stateStack_.top().type;
}

const StateContext* StateManager::getCurrentContext() const {
    if (stateStack_.empty()) {
        return nullptr;
    }
    return &stateStack_.top();
}

bool StateManager::isInState(StateType type) const {
    std::stack<StateContext> temp = stateStack_;
    while (!temp.empty()) {
        if (temp.top().type == type) {
            return true;
        }
        temp.pop();
    }
    return false;
}

bool StateManager::isInAnyState(std::initializer_list<StateType> types) const {
    for (StateType type : types) {
        if (isInState(type)) {
            return true;
        }
    }
    return false;
}

const StateContext* StateManager::findNearestState(StateType type) const {
    std::stack<StateContext> temp = stateStack_;
    while (!temp.empty()) {
        if (temp.top().type == type) {
            return &temp.top();
        }
        temp.pop();
    }
    return nullptr;
}

void StateManager::reset() {
    // 清空栈
    while (!stateStack_.empty()) {
        stateStack_.pop();
    }
    // 重新初始化为全局状态
    pushState(StateType::GLOBAL, "global");
}

bool StateManager::canTransitionTo(StateType newState) const {
    StateType currentState = getCurrentState();
    
    auto it = transitionRules_.find(currentState);
    if (it == transitionRules_.end()) {
        return false;  // 没有定义转换规则
    }
    
    const auto& allowedStates = it->second;
    return std::find(allowedStates.begin(), allowedStates.end(), newState) != allowedStates.end();
}

std::vector<StateType> StateManager::getAllowedTransitions() const {
    StateType currentState = getCurrentState();
    
    auto it = transitionRules_.find(currentState);
    if (it == transitionRules_.end()) {
        return {};
    }
    
    return it->second;
}

void StateManager::setCurrentStateProperty(const std::string& key, const std::string& value) {
    if (!stateStack_.empty()) {
        stateStack_.top().setProperty(key, value);
    }
}

std::optional<std::string> StateManager::getCurrentStateProperty(const std::string& key) const {
    if (!stateStack_.empty()) {
        return stateStack_.top().getProperty(key);
    }
    return std::nullopt;
}

bool StateManager::isInCHTLJSContext() const {
    return isInAnyState({
        StateType::IN_ENHANCED_SELECTOR,
        StateType::IN_LISTEN_BLOCK,
        StateType::IN_DELEGATE_BLOCK,
        StateType::IN_ANIMATE_BLOCK,
        StateType::IN_INEVERAWAY_BLOCK,
        StateType::IN_VIRTUAL_OBJECT,
        StateType::IN_EVENT_BINDING
    });
}

bool StateManager::isInSelectorContext() const {
    return isInState(StateType::IN_ENHANCED_SELECTOR);
}

bool StateManager::isInAnimationContext() const {
    return isInAnyState({
        StateType::IN_ANIMATE_BLOCK,
        StateType::IN_ANIMATE_BEGIN,
        StateType::IN_ANIMATE_WHEN,
        StateType::IN_ANIMATE_END
    });
}

bool StateManager::canUseVirtualObject() const {
    // 虚对象可以在script块和函数内使用
    return isInAnyState({
        StateType::IN_SCRIPT_BLOCK,
        StateType::IN_FUNCTION,
        StateType::GLOBAL  // 全局作用域
    });
}

std::string StateManager::getStatePath() const {
    std::stringstream ss;
    std::stack<StateContext> temp = stateStack_;
    std::vector<std::string> path;
    
    while (!temp.empty()) {
        const auto& ctx = temp.top();
        std::string stateStr = std::to_string(static_cast<int>(ctx.type));
        if (!ctx.name.empty()) {
            stateStr += "(" + ctx.name + ")";
        }
        path.push_back(stateStr);
        temp.pop();
    }
    
    // 反转路径以正确显示从根到当前的路径
    std::reverse(path.begin(), path.end());
    
    for (size_t i = 0; i < path.size(); ++i) {
        if (i > 0) ss << " -> ";
        ss << path[i];
    }
    
    return ss.str();
}

} // namespace CHTLJS