#include "State.h"
#include <sstream>
#include <algorithm>

namespace CHTL {

// 初始化状态转换规则
const std::unordered_map<StateType, std::vector<StateType>> StateManager::transitionRules_ = 
    StateManager::initTransitionRules();

std::unordered_map<StateType, std::vector<StateType>> StateManager::initTransitionRules() {
    std::unordered_map<StateType, std::vector<StateType>> rules;
    
    // 全局状态可以转换到的状态
    rules[StateType::GLOBAL] = {
        StateType::IN_NAMESPACE,
        StateType::IN_CONFIGURATION,
        StateType::IN_TEMPLATE,
        StateType::IN_CUSTOM,
        StateType::IN_ORIGIN,
        StateType::IN_ELEMENT,
        StateType::IN_IMPORT,
        StateType::IN_USE,
        StateType::IN_COMMENT,
        StateType::IN_STYLE_BLOCK,  // 全局style
        StateType::IN_SCRIPT_BLOCK  // 全局script
    };
    
    // 命名空间内可以包含的状态
    rules[StateType::IN_NAMESPACE] = {
        StateType::IN_NAMESPACE,     // 嵌套命名空间
        StateType::IN_TEMPLATE,
        StateType::IN_CUSTOM,
        StateType::IN_ORIGIN,
        StateType::IN_ELEMENT,
        StateType::IN_EXCEPT_CONSTRAINT,
        StateType::IN_COMMENT
    };
    
    // 配置组内的状态
    rules[StateType::IN_CONFIGURATION] = {
        StateType::IN_ATTRIBUTE,
        StateType::IN_COMMENT
    };
    
    // 模板内的状态
    rules[StateType::IN_TEMPLATE] = {
        StateType::IN_ELEMENT,
        StateType::IN_STYLE_BLOCK,
        StateType::IN_ATTRIBUTE,
        StateType::IN_INHERIT_OPERATION,
        StateType::IN_COMMENT
    };
    
    // 自定义内的状态
    rules[StateType::IN_CUSTOM] = {
        StateType::IN_ELEMENT,
        StateType::IN_STYLE_BLOCK,
        StateType::IN_ATTRIBUTE,
        StateType::IN_INHERIT_OPERATION,
        StateType::IN_DELETE_OPERATION,
        StateType::IN_INSERT_OPERATION,
        StateType::IN_COMMENT
    };
    
    // 元素内的状态
    rules[StateType::IN_ELEMENT] = {
        StateType::IN_ELEMENT,       // 嵌套元素
        StateType::IN_ATTRIBUTE,
        StateType::IN_STYLE_BLOCK,   // 局部样式块
        StateType::IN_SCRIPT_BLOCK,  // 局部脚本块
        StateType::IN_TEXT_BLOCK,
        StateType::IN_EXCEPT_CONSTRAINT,
        StateType::IN_COMMENT
    };
    
    // 样式块内的状态
    rules[StateType::IN_STYLE_BLOCK] = {
        StateType::IN_STYLE_BLOCK,  // Allow recursive style blocks for now
        StateType::IN_CLASS_SELECTOR,
        StateType::IN_ID_SELECTOR,
        StateType::IN_PSEUDO_SELECTOR,
        StateType::IN_STYLE_PROPERTY,
        StateType::IN_INHERIT_OPERATION,
        StateType::IN_DELETE_OPERATION,
        StateType::IN_COMMENT
    };
    
    // 类选择器内的状态
    rules[StateType::IN_CLASS_SELECTOR] = {
        StateType::IN_STYLE_PROPERTY,
        StateType::IN_COMMENT
    };
    
    // ID选择器内的状态
    rules[StateType::IN_ID_SELECTOR] = {
        StateType::IN_STYLE_PROPERTY,
        StateType::IN_COMMENT
    };
    
    // 属性定义状态
    rules[StateType::IN_ATTRIBUTE] = {
        StateType::IN_ATTRIBUTE_VALUE,
        StateType::IN_STRING_LITERAL
    };
    
    return rules;
}

StateManager::StateManager() {
    // 直接添加初始全局状态，不经过转换检查
    stateStack_.emplace(StateType::GLOBAL, "global", 0, 0);
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
    // Cannot return pointer to temporary stack, need to search the original stack
    // This requires a different approach - we'll need to store states differently
    // For now, return nullptr - this method needs redesign
    (void)type; // Suppress unused parameter warning
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

} // namespace CHTL