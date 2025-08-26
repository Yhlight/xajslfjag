#include "CHTLState.h"
#include <iostream>

namespace CHTL {

CHTLState::CHTLState() {
    state_stack.push(StateInfo(ParsingState::GlobalScope));
}

void CHTLState::pushState(ParsingState state, const std::string& context_name) {
    state_stack.push(StateInfo(state, context_name));
}

void CHTLState::popState() {
    if (state_stack.size() > 1) {
        state_stack.pop();
    }
}

ParsingState CHTLState::getCurrentState() const {
    return state_stack.top().state;
}

const StateInfo& CHTLState::getCurrentStateInfo() const {
    return state_stack.top();
}

bool CHTLState::isInState(ParsingState state) const {
    return getCurrentState() == state;
}

bool CHTLState::isInAnyStyleState() const {
    ParsingState current = getCurrentState();
    return current == ParsingState::InStyleBlock ||
           current == ParsingState::InStyleProperty ||
           current == ParsingState::InStyleSelector ||
           current == ParsingState::InStyleSpecialization;
}

bool CHTLState::isInDefinitionState() const {
    ParsingState current = getCurrentState();
    return current == ParsingState::InTemplateDefinition ||
           current == ParsingState::InCustomDefinition ||
           current == ParsingState::InOriginDefinition ||
           current == ParsingState::InNamespaceDefinition ||
           current == ParsingState::InConfigurationDefinition;
}

bool CHTLState::isInSpecializationState() const {
    ParsingState current = getCurrentState();
    return current == ParsingState::InStyleSpecialization ||
           current == ParsingState::InElementSpecialization ||
           current == ParsingState::InVarSpecialization;
}

void CHTLState::setStateProperty(const std::string& key, const std::string& value) {
    state_stack.top().properties[key] = value;
}

std::string CHTLState::getStateProperty(const std::string& key) const {
    const auto& props = state_stack.top().properties;
    auto it = props.find(key);
    return (it != props.end()) ? it->second : "";
}

void CHTLState::setGlobalProperty(const std::string& key, const std::string& value) {
    global_context[key] = value;
}

std::string CHTLState::getGlobalProperty(const std::string& key) const {
    auto it = global_context.find(key);
    return (it != global_context.end()) ? it->second : "";
}

size_t CHTLState::getDepth() const {
    return state_stack.size();
}

void CHTLState::clear() {
    while (!state_stack.empty()) {
        state_stack.pop();
    }
    state_stack.push(StateInfo(ParsingState::GlobalScope));
    global_context.clear();
}

bool CHTLState::canDefineTemplate() const {
    ParsingState current = getCurrentState();
    return current == ParsingState::GlobalScope ||
           current == ParsingState::InNamespaceDefinition;
}

bool CHTLState::canDefineCustom() const {
    ParsingState current = getCurrentState();
    return current == ParsingState::GlobalScope ||
           current == ParsingState::InNamespaceDefinition;
}

bool CHTLState::canUseTemplate() const {
    ParsingState current = getCurrentState();
    return current == ParsingState::InElementBody ||
           current == ParsingState::InTemplateUsage ||
           current == ParsingState::InCustomDefinition ||
           current == ParsingState::InElementSpecialization;
}

bool CHTLState::canUseStyle() const {
    ParsingState current = getCurrentState();
    return current == ParsingState::InElementBody ||
           current == ParsingState::InStyleBlock;
}

bool CHTLState::canUseScript() const {
    ParsingState current = getCurrentState();
    return current == ParsingState::InElementBody ||
           current == ParsingState::InScriptBlock;
}

bool CHTLState::canAddAttribute() const {
    ParsingState current = getCurrentState();
    return current == ParsingState::InElementBody ||
           current == ParsingState::InAttributeList;
}

} // namespace CHTL