#include "ConstraintNode.h"
#include "../../Scanner/CHTLUnifiedScanner.h"
#include <algorithm>
#include <sstream>

namespace CHTL {

// ========== ConstraintNode 实现 ==========

ConstraintNode::ConstraintNode(ConstraintType type, const Position& pos)
    : BaseNode(NodeType::CONSTRAINT, "", pos), constraintType(type) {
}

void ConstraintNode::addTarget(const ConstraintTarget& target) {
    targets.push_back(target);
}

void ConstraintNode::addTarget(ConstraintTargetType targetType, const String& name, const String& customType) {
    targets.emplace_back(targetType, name, customType);
}

bool ConstraintNode::constrains(const ConstraintTarget& target) const {
    return std::find(targets.begin(), targets.end(), target) != targets.end();
}

bool ConstraintNode::constrainsHTMLElement(const String& elementName) const {
    for (const auto& target : targets) {
        if (target.targetType == ConstraintTargetType::HTML_ELEMENT && 
            (target.name.empty() || target.name == elementName)) {
            return true;
        }
    }
    return false;
}

bool ConstraintNode::constrainsCustomElement(const String& elementName) const {
    for (const auto& target : targets) {
        if (target.targetType == ConstraintTargetType::CUSTOM_ELEMENT && 
            (target.name.empty() || target.name == elementName)) {
            return true;
        }
    }
    return false;
}

bool ConstraintNode::constrainsCustomStyle(const String& styleName) const {
    for (const auto& target : targets) {
        if (target.targetType == ConstraintTargetType::CUSTOM_STYLE && 
            (target.name.empty() || target.name == styleName)) {
            return true;
        }
    }
    return false;
}

bool ConstraintNode::constrainsTemplateType() const {
    for (const auto& target : targets) {
        if (target.targetType == ConstraintTargetType::TEMPLATE_TYPE) {
            return true;
        }
    }
    return false;
}

bool ConstraintNode::constrainsCustomType() const {
    for (const auto& target : targets) {
        if (target.targetType == ConstraintTargetType::CUSTOM_TYPE) {
            return true;
        }
    }
    return false;
}

bool ConstraintNode::constrainsOriginType(const String& originType) const {
    for (const auto& target : targets) {
        if (target.targetType == ConstraintTargetType::ORIGIN_CUSTOM && 
            target.customType == originType) {
            return true;
        }
        if (target.targetType == ConstraintTargetType::ORIGIN_HTML ||
            target.targetType == ConstraintTargetType::ORIGIN_STYLE ||
            target.targetType == ConstraintTargetType::ORIGIN_JAVASCRIPT) {
            return true;
        }
    }
    return false;
}

bool ConstraintNode::validateTarget(const BaseNode* node) const {
    if (!node) return true;
    
    for (const auto& target : targets) {
        if (matchesTarget(target, node)) {
            return false; // 找到匹配的约束目标，表示违反约束
        }
    }
    return true; // 没有违反约束
}

bool ConstraintNode::validateInScope(const String& currentScope) const {
    if (constraintType != ConstraintType::GLOBAL) {
        return true; // 非全局约束总是有效
    }
    
    return scope.empty() || scope == currentScope;
}

String ConstraintNode::getConstraintDescription() const {
    std::ostringstream oss;
    oss << "Constraint (";
    
    switch (constraintType) {
        case ConstraintType::PRECISE:
            oss << "precise";
            break;
        case ConstraintType::TYPE:
            oss << "type";
            break;
        case ConstraintType::GLOBAL:
            oss << "global";
            break;
    }
    
    oss << "): ";
    
    for (size_t i = 0; i < targets.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << targetTypeToString(targets[i].targetType);
        if (!targets[i].name.empty()) {
            oss << " " << targets[i].name;
        }
        if (!targets[i].customType.empty()) {
            oss << " (" << targets[i].customType << ")";
        }
    }
    
    if (!scope.empty()) {
        oss << " in scope: " << scope;
    }
    
    return oss.str();
}

StringVector ConstraintNode::getTargetNames() const {
    StringVector names;
    for (const auto& target : targets) {
        if (!target.name.empty()) {
            names.push_back(target.name);
        } else {
            names.push_back(targetTypeToString(target.targetType));
        }
    }
    return names;
}

String ConstraintNode::toString() const {
    return getConstraintDescription();
}

String ConstraintNode::toHTML() const {
    return "<!-- " + toString() + " -->";
}

std::unique_ptr<ConstraintNode> ConstraintNode::createPreciseConstraint(
    const std::vector<ConstraintTarget>& targets, const Position& pos) {
    auto constraint = std::make_unique<ConstraintNode>(ConstraintType::PRECISE, pos);
    constraint->targets = targets;
    return constraint;
}

std::unique_ptr<ConstraintNode> ConstraintNode::createTypeConstraint(
    const std::vector<ConstraintTarget>& targets, const Position& pos) {
    auto constraint = std::make_unique<ConstraintNode>(ConstraintType::TYPE, pos);
    constraint->targets = targets;
    return constraint;
}

std::unique_ptr<ConstraintNode> ConstraintNode::createGlobalConstraint(
    const std::vector<ConstraintTarget>& targets, const String& scope, const Position& pos) {
    auto constraint = std::make_unique<ConstraintNode>(ConstraintType::GLOBAL, pos);
    constraint->targets = targets;
    constraint->scope = scope;
    return constraint;
}

ConstraintTarget ConstraintNode::parseConstraintTarget(const String& targetString) {
    String trimmed = targetString;
    // 移除首尾空白
    trimmed.erase(0, trimmed.find_first_not_of(" \t\r\n"));
    trimmed.erase(trimmed.find_last_not_of(" \t\r\n") + 1);
    
    // 解析不同类型的目标
    if (trimmed.find("[Custom] @Element") == 0) {
        String name = trimmed.substr(17);
        name.erase(0, name.find_first_not_of(" \t"));
        return ConstraintTarget(ConstraintTargetType::CUSTOM_ELEMENT, name);
    }
    else if (trimmed.find("[Custom] @Style") == 0) {
        String name = trimmed.substr(15);
        name.erase(0, name.find_first_not_of(" \t"));
        return ConstraintTarget(ConstraintTargetType::CUSTOM_STYLE, name);
    }
    else if (trimmed.find("[Custom] @Var") == 0) {
        String name = trimmed.substr(13);
        name.erase(0, name.find_first_not_of(" \t"));
        return ConstraintTarget(ConstraintTargetType::CUSTOM_VAR, name);
    }
    else if (trimmed.find("[Template] @Element") == 0) {
        String name = trimmed.substr(19);
        name.erase(0, name.find_first_not_of(" \t"));
        return ConstraintTarget(ConstraintTargetType::TEMPLATE_ELEMENT, name);
    }
    else if (trimmed.find("[Template] @Style") == 0) {
        String name = trimmed.substr(17);
        name.erase(0, name.find_first_not_of(" \t"));
        return ConstraintTarget(ConstraintTargetType::TEMPLATE_STYLE, name);
    }
    else if (trimmed.find("[Template] @Var") == 0) {
        String name = trimmed.substr(15);
        name.erase(0, name.find_first_not_of(" \t"));
        return ConstraintTarget(ConstraintTargetType::TEMPLATE_VAR, name);
    }
    else if (trimmed == "[Custom]") {
        return ConstraintTarget(ConstraintTargetType::CUSTOM_TYPE);
    }
    else if (trimmed == "[Template]") {
        return ConstraintTarget(ConstraintTargetType::TEMPLATE_TYPE);
    }
    else if (trimmed == "@Html") {
        return ConstraintTarget(ConstraintTargetType::ORIGIN_HTML);
    }
    else if (trimmed == "@Style") {
        return ConstraintTarget(ConstraintTargetType::ORIGIN_STYLE);
    }
    else if (trimmed == "@JavaScript") {
        return ConstraintTarget(ConstraintTargetType::ORIGIN_JAVASCRIPT);
    }
    else if (trimmed.find("@") == 0) {
        // 自定义原始嵌入类型
        String customType = trimmed.substr(1);
        return ConstraintTarget(ConstraintTargetType::ORIGIN_CUSTOM, "", customType);
    }
    else {
        // 默认为HTML元素
        return ConstraintTarget(ConstraintTargetType::HTML_ELEMENT, trimmed);
    }
}

std::vector<ConstraintTarget> ConstraintNode::parseConstraintTargets(const String& targetsString) {
    std::vector<ConstraintTarget> targets;
    std::istringstream iss(targetsString);
    String token;
    
    while (std::getline(iss, token, ',')) {
        targets.push_back(parseConstraintTarget(token));
    }
    
    return targets;
}

bool ConstraintNode::matchesTarget(const ConstraintTarget& target, const BaseNode* node) const {
    switch (target.targetType) {
        case ConstraintTargetType::HTML_ELEMENT:
            return node->type == NodeType::ELEMENT && 
                   (target.name.empty() || node->value == target.name);
                   
        case ConstraintTargetType::CUSTOM_ELEMENT:
            return node->type == NodeType::CUSTOM_ELEMENT && 
                   (target.name.empty() || node->value == target.name);
                   
        case ConstraintTargetType::CUSTOM_STYLE:
            return node->type == NodeType::CUSTOM_STYLE && 
                   (target.name.empty() || node->value == target.name);
                   
        case ConstraintTargetType::CUSTOM_VAR:
            return node->type == NodeType::CUSTOM_VAR && 
                   (target.name.empty() || node->value == target.name);
                   
        case ConstraintTargetType::TEMPLATE_ELEMENT:
            return node->type == NodeType::TEMPLATE_ELEMENT && 
                   (target.name.empty() || node->value == target.name);
                   
        case ConstraintTargetType::TEMPLATE_STYLE:
            return node->type == NodeType::TEMPLATE_STYLE && 
                   (target.name.empty() || node->value == target.name);
                   
        case ConstraintTargetType::TEMPLATE_VAR:
            return node->type == NodeType::TEMPLATE_VAR && 
                   (target.name.empty() || node->value == target.name);
                   
        case ConstraintTargetType::ORIGIN_HTML:
            return node->type == NodeType::ORIGIN_HTML;
            
        case ConstraintTargetType::ORIGIN_STYLE:
            return node->type == NodeType::ORIGIN_STYLE;
            
        case ConstraintTargetType::ORIGIN_JAVASCRIPT:
            return node->type == NodeType::ORIGIN_JAVASCRIPT;
            
        case ConstraintTargetType::ORIGIN_CUSTOM:
            return node->type == NodeType::ORIGIN_CUSTOM && 
                   (target.customType.empty() || node->getAttribute("type") == target.customType);
                   
        case ConstraintTargetType::CUSTOM_TYPE:
            return node->type == NodeType::CUSTOM || 
                   node->type == NodeType::CUSTOM_ELEMENT ||
                   node->type == NodeType::CUSTOM_STYLE ||
                   node->type == NodeType::CUSTOM_VAR;
                   
        case ConstraintTargetType::TEMPLATE_TYPE:
            return node->type == NodeType::TEMPLATE || 
                   node->type == NodeType::TEMPLATE_ELEMENT ||
                   node->type == NodeType::TEMPLATE_STYLE ||
                   node->type == NodeType::TEMPLATE_VAR;
                   
        default:
            return false;
    }
}

String ConstraintNode::targetTypeToString(ConstraintTargetType type) const {
    switch (type) {
        case ConstraintTargetType::HTML_ELEMENT: return "HTML_ELEMENT";
        case ConstraintTargetType::CUSTOM_ELEMENT: return "CUSTOM_ELEMENT";
        case ConstraintTargetType::CUSTOM_STYLE: return "CUSTOM_STYLE";
        case ConstraintTargetType::CUSTOM_VAR: return "CUSTOM_VAR";
        case ConstraintTargetType::TEMPLATE_ELEMENT: return "TEMPLATE_ELEMENT";
        case ConstraintTargetType::TEMPLATE_STYLE: return "TEMPLATE_STYLE";
        case ConstraintTargetType::TEMPLATE_VAR: return "TEMPLATE_VAR";
        case ConstraintTargetType::ORIGIN_HTML: return "ORIGIN_HTML";
        case ConstraintTargetType::ORIGIN_STYLE: return "ORIGIN_STYLE";
        case ConstraintTargetType::ORIGIN_JAVASCRIPT: return "ORIGIN_JAVASCRIPT";
        case ConstraintTargetType::ORIGIN_CUSTOM: return "ORIGIN_CUSTOM";
        case ConstraintTargetType::CUSTOM_TYPE: return "CUSTOM_TYPE";
        case ConstraintTargetType::TEMPLATE_TYPE: return "TEMPLATE_TYPE";
        default: return "UNKNOWN";
    }
}

// ========== ConstraintValidator 实现 ==========

bool ConstraintValidator::validateNode(const BaseNode* node, const std::vector<ConstraintNode*>& constraints) {
    if (!node) return true;
    
    for (const auto* constraint : constraints) {
        if (!constraint->validateTarget(node)) {
            return false; // 违反约束
        }
    }
    return true;
}

std::vector<ConstraintNode*> ConstraintValidator::getApplicableConstraints(
    const BaseNode* node, const std::vector<ConstraintNode*>& allConstraints) {
    std::vector<ConstraintNode*> applicable;
    
    for (auto* constraint : allConstraints) {
        if (isInScope(node, constraint)) {
            applicable.push_back(constraint);
        }
    }
    
    return applicable;
}

bool ConstraintValidator::isInScope(const BaseNode* node, const ConstraintNode* constraint) {
    if (!node || !constraint) return false;
    
    // 全局约束需要检查命名空间
    if (constraint->constraintType == ConstraintType::GLOBAL) {
        // 需要实现命名空间作用域检查
        return true; // 简化实现
    }
    
    // 其他约束检查父节点链
    const BaseNode* current = node->parent;
    while (current) {
        if (current->type == NodeType::NAMESPACE) {
            return constraint->scope.empty() || current->value == constraint->scope;
        }
        current = current->parent;
    }
    
    return true; // 默认在作用域内
}

std::vector<ConstraintNode*> ConstraintValidator::collectScopeConstraints(
    const BaseNode* currentNode, const BaseNode* rootNode) {
    std::vector<ConstraintNode*> constraints;
    
    // 遍历AST收集约束节点
    std::function<void(const BaseNode*)> traverse = [&](const BaseNode* node) {
        if (!node) return;
        
        if (node->type == NodeType::CONSTRAINT) {
            constraints.push_back(const_cast<ConstraintNode*>(static_cast<const ConstraintNode*>(node)));
        }
        
        for (const auto& child : node->children) {
            traverse(child.get());
        }
    };
    
    traverse(rootNode);
    return constraints;
}

String ConstraintValidator::generateConstraintViolationMessage(
    const BaseNode* node, const ConstraintNode* violatedConstraint) {
    if (!node || !violatedConstraint) return "";
    
    std::ostringstream oss;
    oss << "Constraint violation: ";
    oss << "Node '" << node->value << "' of type '" << static_cast<int>(node->type) << "' ";
    oss << "violates constraint: " << violatedConstraint->getConstraintDescription();
    
    return oss.str();
}

// ========== ConstraintManager 实现 ==========

void ConstraintManager::addGlobalConstraint(std::unique_ptr<ConstraintNode> constraint) {
    globalConstraints.push_back(std::move(constraint));
}

void ConstraintManager::addScopeConstraint(const String& scope, std::unique_ptr<ConstraintNode> constraint) {
    scopeConstraints[scope].push_back(std::move(constraint));
}

std::vector<ConstraintNode*> ConstraintManager::getGlobalConstraints() const {
    std::vector<ConstraintNode*> constraints;
    for (const auto& constraint : globalConstraints) {
        constraints.push_back(constraint.get());
    }
    return constraints;
}

std::vector<ConstraintNode*> ConstraintManager::getScopeConstraints(const String& scope) const {
    auto it = scopeConstraints.find(scope);
    if (it == scopeConstraints.end()) {
        return {};
    }
    
    std::vector<ConstraintNode*> constraints;
    for (const auto& constraint : it->second) {
        constraints.push_back(constraint.get());
    }
    return constraints;
}

std::vector<ConstraintNode*> ConstraintManager::getAllConstraints() const {
    std::vector<ConstraintNode*> all;
    
    // 添加全局约束
    for (const auto& constraint : globalConstraints) {
        all.push_back(constraint.get());
    }
    
    // 添加作用域约束
    for (const auto& scopePair : scopeConstraints) {
        for (const auto& constraint : scopePair.second) {
            all.push_back(constraint.get());
        }
    }
    
    return all;
}

bool ConstraintManager::validateNode(const BaseNode* node, const String& currentScope) const {
    if (!node) return true;
    
    // 获取适用的约束
    auto globalConstraints = getGlobalConstraints();
    auto scopeConstraints = getScopeConstraints(currentScope);
    
    // 验证全局约束
    if (!ConstraintValidator::validateNode(node, globalConstraints)) {
        return false;
    }
    
    // 验证作用域约束
    if (!ConstraintValidator::validateNode(node, scopeConstraints)) {
        return false;
    }
    
    return true;
}

StringVector ConstraintManager::getViolationMessages(const BaseNode* node, const String& currentScope) const {
    StringVector messages;
    
    if (!node) return messages;
    
    auto allConstraints = getAllConstraints();
    for (auto* constraint : allConstraints) {
        if (!constraint->validateTarget(node) && constraint->validateInScope(currentScope)) {
            messages.push_back(ConstraintValidator::generateConstraintViolationMessage(node, constraint));
        }
    }
    
    return messages;
}

void ConstraintManager::clear() {
    globalConstraints.clear();
    scopeConstraints.clear();
}

void ConstraintManager::clearScope(const String& scope) {
    scopeConstraints.erase(scope);
}

void ConstraintManager::printConstraints() const {
    std::cout << "Global Constraints (" << globalConstraints.size() << "):" << std::endl;
    for (const auto& constraint : globalConstraints) {
        std::cout << "  " << constraint->toString() << std::endl;
    }
    
    std::cout << "Scope Constraints:" << std::endl;
    for (const auto& scopePair : scopeConstraints) {
        std::cout << "  Scope '" << scopePair.first << "' (" << scopePair.second.size() << "):" << std::endl;
        for (const auto& constraint : scopePair.second) {
            std::cout << "    " << constraint->toString() << std::endl;
        }
    }
}

size_t ConstraintManager::getConstraintCount() const {
    size_t count = globalConstraints.size();
    for (const auto& scopePair : scopeConstraints) {
        count += scopePair.second.size();
    }
    return count;
}

} // namespace CHTL