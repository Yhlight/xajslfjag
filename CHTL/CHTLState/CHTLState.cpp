#include "CHTLState.h"
#include <algorithm>
#include <sstream>
#include <iostream>

namespace CHTL {

CHTLState::CHTLState() {
    stateStack_.push_back(ParseState::INITIAL);
}

void CHTLState::pushState(ParseState state) {
    stateStack_.push_back(state);
}

void CHTLState::popState() {
    if (stateStack_.size() > 1) { // 保持至少一个状态
        stateStack_.pop_back();
    }
}

ParseState CHTLState::getCurrentState() const {
    return stateStack_.empty() ? ParseState::INITIAL : stateStack_.back();
}

ParseState CHTLState::getPreviousState() const {
    if (stateStack_.size() >= 2) {
        return stateStack_[stateStack_.size() - 2];
    }
    return ParseState::INITIAL;
}

size_t CHTLState::getStateDepth() const {
    return stateStack_.size();
}

bool CHTLState::hasState() const {
    return !stateStack_.empty();
}

bool CHTLState::isInState(ParseState state) const {
    return std::find(stateStack_.begin(), stateStack_.end(), state) != stateStack_.end();
}

void CHTLState::addConstraint(const ConstraintRule& rule) {
    constraints_.push_back(rule);
}

void CHTLState::removeConstraint(const std::string& target) {
    constraints_.erase(
        std::remove_if(constraints_.begin(), constraints_.end(),
            [&target](const ConstraintRule& rule) {
                return rule.target == target;
            }),
        constraints_.end()
    );
}

bool CHTLState::hasConstraint(const std::string& target) const {
    return std::any_of(constraints_.begin(), constraints_.end(),
        [&target](const ConstraintRule& rule) {
            return rule.target == target;
        });
}

std::vector<ConstraintRule> CHTLState::getConstraints() const {
    return constraints_;
}

void CHTLState::clearConstraints() {
    constraints_.clear();
}

bool CHTLState::canParseElement(const std::string& elementName) const {
    // 检查元素约束
    for (const auto& rule : constraints_) {
        if (rule.type == ConstraintType::ELEMENT_CONSTRAINT && 
            rule.target == elementName) {
            return false;
        }
        if (rule.type == ConstraintType::TYPE_CONSTRAINT && 
            rule.target == "@Html") {
            return false;
        }
    }
    return true;
}

bool CHTLState::canParseTemplate(const std::string& templateType) const {
    // 检查模板约束
    for (const auto& rule : constraints_) {
        if (rule.type == ConstraintType::TEMPLATE_CONSTRAINT && 
            rule.target == templateType) {
            return false;
        }
        if (rule.type == ConstraintType::TYPE_CONSTRAINT && 
            rule.target == "[Template]") {
            return false;
        }
    }
    return true;
}

bool CHTLState::canParseCustom(const std::string& customType) const {
    // 检查自定义约束
    for (const auto& rule : constraints_) {
        if (rule.type == ConstraintType::CUSTOM_CONSTRAINT && 
            rule.target == customType) {
            return false;
        }
        if (rule.type == ConstraintType::TYPE_CONSTRAINT && 
            rule.target == "[Custom]") {
            return false;
        }
    }
    return true;
}

bool CHTLState::canParseOrigin() const {
    // 原始嵌入在任何地方都可以使用
    return true;
}

void CHTLState::setFlag(const std::string& flag, bool value) {
    flags_[flag] = value;
}

bool CHTLState::getFlag(const std::string& flag) const {
    auto it = flags_.find(flag);
    return (it != flags_.end()) ? it->second : false;
}

void CHTLState::clearFlag(const std::string& flag) {
    flags_.erase(flag);
}

void CHTLState::clearAllFlags() {
    flags_.clear();
}

void CHTLState::addClassSelector(const std::string& selector) {
    if (std::find(classSelectors_.begin(), classSelectors_.end(), selector) == classSelectors_.end()) {
        classSelectors_.push_back(selector);
    }
}

void CHTLState::addIdSelector(const std::string& selector) {
    if (std::find(idSelectors_.begin(), idSelectors_.end(), selector) == idSelectors_.end()) {
        idSelectors_.push_back(selector);
    }
}

std::vector<std::string> CHTLState::getClassSelectors() const {
    return classSelectors_;
}

std::vector<std::string> CHTLState::getIdSelectors() const {
    return idSelectors_;
}

void CHTLState::clearSelectors() {
    classSelectors_.clear();
    idSelectors_.clear();
}

void CHTLState::reset() {
    stateStack_.clear();
    stateStack_.push_back(ParseState::INITIAL);
    
    constraints_.clear();
    flags_.clear();
    classSelectors_.clear();
    idSelectors_.clear();
    
    braceDepth_ = 0;
    bracketDepth_ = 0;
    parenDepth_ = 0;
    
    currentIdentifier_.clear();
    currentType_.clear();
}

std::string CHTLState::getStateInfo() const {
    std::ostringstream oss;
    
    oss << "Current State: " << parseStateToString(getCurrentState()) << "\n";
    oss << "State Depth: " << getStateDepth() << "\n";
    oss << "Brace Depth: " << braceDepth_ << "\n";
    oss << "Bracket Depth: " << bracketDepth_ << "\n";
    oss << "Paren Depth: " << parenDepth_ << "\n";
    
    if (!currentIdentifier_.empty()) {
        oss << "Current Identifier: " << currentIdentifier_ << "\n";
    }
    
    if (!currentType_.empty()) {
        oss << "Current Type: " << currentType_ << "\n";
    }
    
    if (!constraints_.empty()) {
        oss << "Constraints: " << constraints_.size() << "\n";
    }
    
    if (!flags_.empty()) {
        oss << "Flags: ";
        bool first = true;
        for (const auto& flag : flags_) {
            if (!first) oss << ", ";
            oss << flag.first << "=" << (flag.second ? "true" : "false");
            first = false;
        }
        oss << "\n";
    }
    
    return oss.str();
}

void CHTLState::dumpState() const {
    std::cout << "=== CHTL State Dump ===" << std::endl;
    std::cout << getStateInfo() << std::endl;
    std::cout << "======================" << std::endl;
}

std::string parseStateToString(ParseState state) {
    switch (state) {
        case ParseState::INITIAL: return "INITIAL";
        case ParseState::PARSING_ELEMENT: return "PARSING_ELEMENT";
        case ParseState::PARSING_ATTRIBUTES: return "PARSING_ATTRIBUTES";
        case ParseState::PARSING_TEXT_BLOCK: return "PARSING_TEXT_BLOCK";
        case ParseState::PARSING_STYLE_BLOCK: return "PARSING_STYLE_BLOCK";
        case ParseState::PARSING_SCRIPT_BLOCK: return "PARSING_SCRIPT_BLOCK";
        case ParseState::PARSING_TEMPLATE: return "PARSING_TEMPLATE";
        case ParseState::PARSING_CUSTOM: return "PARSING_CUSTOM";
        case ParseState::PARSING_ORIGIN: return "PARSING_ORIGIN";
        case ParseState::PARSING_IMPORT: return "PARSING_IMPORT";
        case ParseState::PARSING_CONFIG: return "PARSING_CONFIG";
        case ParseState::PARSING_NAMESPACE: return "PARSING_NAMESPACE";
        case ParseState::PARSING_USE: return "PARSING_USE";
        case ParseState::PARSING_EXCEPT: return "PARSING_EXCEPT";
        case ParseState::PARSING_COMMENT: return "PARSING_COMMENT";
        case ParseState::ERROR_STATE: return "ERROR_STATE";
        case ParseState::END_STATE: return "END_STATE";
        default: return "UNKNOWN";
    }
}

// StateGuard 实现
StateGuard::StateGuard(CHTLState& state, ParseState newState)
    : state_(state) {
    state_.pushState(newState);
}

StateGuard::~StateGuard() {
    state_.popState();
}

// DepthGuard 实现
DepthGuard::DepthGuard(CHTLState& state, DepthType type)
    : state_(state), type_(type) {
    switch (type_) {
        case DepthType::BRACE:
            state_.incrementBraceDepth();
            break;
        case DepthType::BRACKET:
            state_.incrementBracketDepth();
            break;
        case DepthType::PAREN:
            state_.incrementParenDepth();
            break;
    }
}

DepthGuard::~DepthGuard() {
    switch (type_) {
        case DepthType::BRACE:
            state_.decrementBraceDepth();
            break;
        case DepthType::BRACKET:
            state_.decrementBracketDepth();
            break;
        case DepthType::PAREN:
            state_.decrementParenDepth();
            break;
    }
}

} // namespace CHTL