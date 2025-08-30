#include "ConstraintNode.h"
#include "ElementNode.h"
#include "TemplateNode.h"
#include "AdvancedTemplateNode.h"
#include "OriginNode.h"
#include <sstream>
#include <algorithm>

namespace CHTL {

// ========== ConstraintItem 实现 ==========

bool ConstraintItem::matches(ConstraintTargetType type, const std::string& name, const std::string& subType) const {
    if (targetType != type) {
        return false;
    }
    
    // 对于精确约束，需要匹配名称
    if (!targetName.empty() && targetName != name) {
        return false;
    }
    
    // 对于有子类型的约束，需要匹配子类型
    if (!targetSubType.empty() && targetSubType != subType) {
        return false;
    }
    
    return true;
}

std::string ConstraintItem::toString() const {
    std::stringstream ss;
    
    switch (targetType) {
        case ConstraintTargetType::HTML_ELEMENT:
            ss << targetName;
            break;
        case ConstraintTargetType::TEMPLATE_OBJECT:
            ss << "[Template]";
            if (!targetSubType.empty()) {
                ss << " " << targetSubType;
            }
            if (!targetName.empty()) {
                ss << " " << targetName;
            }
            break;
        case ConstraintTargetType::CUSTOM_OBJECT:
            ss << "[Custom]";
            if (!targetSubType.empty()) {
                ss << " " << targetSubType;
            }
            if (!targetName.empty()) {
                ss << " " << targetName;
            }
            break;
        case ConstraintTargetType::ORIGIN_OBJECT:
            ss << "[Origin]";
            if (!targetSubType.empty()) {
                ss << " " << targetSubType;
            }
            if (!targetName.empty()) {
                ss << " " << targetName;
            }
            break;
        case ConstraintTargetType::HTML_TYPE:
            ss << "@Html";
            break;
        case ConstraintTargetType::TEMPLATE_TYPE:
            ss << "[Template]";
            break;
        case ConstraintTargetType::CUSTOM_TYPE:
            ss << "[Custom]";
            break;
        case ConstraintTargetType::ORIGIN_TYPE:
            ss << "[Origin]";
            break;
        case ConstraintTargetType::TEMPLATE_VAR_TYPE:
            ss << "[Template] @Var";
            break;
        case ConstraintTargetType::CUSTOM_VAR_TYPE:
            ss << "[Custom] @Var";
            break;
        case ConstraintTargetType::TEMPLATE_STYLE_TYPE:
            ss << "[Template] @Style";
            break;
        case ConstraintTargetType::CUSTOM_STYLE_TYPE:
            ss << "[Custom] @Style";
            break;
        case ConstraintTargetType::TEMPLATE_ELEMENT_TYPE:
            ss << "[Template] @Element";
            break;
        case ConstraintTargetType::CUSTOM_ELEMENT_TYPE:
            ss << "[Custom] @Element";
            break;
    }
    
    return ss.str();
}

// ========== ConstraintNode 实现 ==========

ConstraintNode::ConstraintNode(ConstraintType constraintType) 
    : BaseNode(CHTLNodeType::CONSTRAINT_NODE, "except"), 
      m_constraintType(constraintType), 
      m_isGlobal(false) {
}

ConstraintType ConstraintNode::getConstraintType() const {
    return m_constraintType;
}

void ConstraintNode::setConstraintType(ConstraintType type) {
    m_constraintType = type;
}

void ConstraintNode::addConstraintItem(const ConstraintItem& item) {
    // 避免重复添加相同的约束项
    for (const auto& existing : m_constraintItems) {
        if (existing.targetType == item.targetType && 
            existing.targetName == item.targetName && 
            existing.targetSubType == item.targetSubType) {
            return; // 已存在相同的约束项
        }
    }
    
    m_constraintItems.push_back(item);
}

bool ConstraintNode::removeConstraintItem(const ConstraintItem& item) {
    auto it = std::find_if(m_constraintItems.begin(), m_constraintItems.end(),
        [&item](const ConstraintItem& existing) {
            return existing.targetType == item.targetType && 
                   existing.targetName == item.targetName && 
                   existing.targetSubType == item.targetSubType;
        });
    
    if (it != m_constraintItems.end()) {
        m_constraintItems.erase(it);
        return true;
    }
    
    return false;
}

const std::vector<ConstraintItem>& ConstraintNode::getConstraintItems() const {
    return m_constraintItems;
}

bool ConstraintNode::isConstraining(ConstraintTargetType targetType, const std::string& targetName, 
                                   const std::string& targetSubType) const {
    for (const auto& item : m_constraintItems) {
        if (item.matches(targetType, targetName, targetSubType)) {
            return true;
        }
    }
    return false;
}

bool ConstraintNode::isGlobalConstraint() const {
    return m_isGlobal;
}

void ConstraintNode::setAsGlobalConstraint(bool global) {
    m_isGlobal = global;
    if (global) {
        m_constraintType = ConstraintType::GLOBAL_CONSTRAINT;
    }
}

const std::string& ConstraintNode::getConstraintScope() const {
    return m_constraintScope;
}

void ConstraintNode::setConstraintScope(const std::string& scope) {
    m_constraintScope = scope;
}

bool ConstraintNode::validateConstraint() const {
    // 基本验证：至少要有一个约束项
    return !m_constraintItems.empty();
}

ConstraintTargetType ConstraintNode::parseTargetType(const std::string& typeString) const {
    if (typeString == "@Html") {
        return ConstraintTargetType::HTML_TYPE;
    } else if (typeString == "[Template]") {
        return ConstraintTargetType::TEMPLATE_TYPE;
    } else if (typeString == "[Custom]") {
        return ConstraintTargetType::CUSTOM_TYPE;
    } else if (typeString == "[Origin]") {
        return ConstraintTargetType::ORIGIN_TYPE;
    } else if (typeString == "[Template] @Var") {
        return ConstraintTargetType::TEMPLATE_VAR_TYPE;
    } else if (typeString == "[Custom] @Var") {
        return ConstraintTargetType::CUSTOM_VAR_TYPE;
    } else if (typeString == "[Template] @Style") {
        return ConstraintTargetType::TEMPLATE_STYLE_TYPE;
    } else if (typeString == "[Custom] @Style") {
        return ConstraintTargetType::CUSTOM_STYLE_TYPE;
    } else if (typeString == "[Template] @Element") {
        return ConstraintTargetType::TEMPLATE_ELEMENT_TYPE;
    } else if (typeString == "[Custom] @Element") {
        return ConstraintTargetType::CUSTOM_ELEMENT_TYPE;
    } else {
        // 默认为HTML元素
        return ConstraintTargetType::HTML_ELEMENT;
    }
}

std::string ConstraintNode::toString() const {
    std::stringstream ss;
    ss << "except ";
    
    for (size_t i = 0; i < m_constraintItems.size(); ++i) {
        if (i > 0) {
            ss << ", ";
        }
        ss << m_constraintItems[i].toString();
    }
    
    ss << ";";
    return ss.str();
}

NodePtr ConstraintNode::clone() const {
    auto cloned = std::make_shared<ConstraintNode>(m_constraintType);
    cloned->m_constraintItems = m_constraintItems;
    cloned->m_isGlobal = m_isGlobal;
    cloned->m_constraintScope = m_constraintScope;
    return cloned;
}

bool ConstraintNode::validate(ErrorReporter* errorReporter) const {
    (void)errorReporter; // 暂时不使用
    return validateConstraint();
}

// ========== ConstraintManager 实现 ==========

ConstraintManager::ConstraintManager() : m_errorReporter(nullptr) {
}

void ConstraintManager::addConstraint(std::shared_ptr<ConstraintNode> constraint, const std::string& scope) {
    if (!constraint) {
        return;
    }
    
    if (constraint->isGlobalConstraint()) {
        m_globalConstraints.push_back(constraint);
    } else {
        std::string constraintScope = scope.empty() ? constraint->getConstraintScope() : scope;
        m_scopeConstraints[constraintScope].push_back(constraint);
    }
}

bool ConstraintManager::removeConstraint(std::shared_ptr<ConstraintNode> constraint, const std::string& scope) {
    if (!constraint) {
        return false;
    }
    
    if (constraint->isGlobalConstraint()) {
        auto it = std::find(m_globalConstraints.begin(), m_globalConstraints.end(), constraint);
        if (it != m_globalConstraints.end()) {
            m_globalConstraints.erase(it);
            return true;
        }
    } else {
        std::string constraintScope = scope.empty() ? constraint->getConstraintScope() : scope;
        auto scopeIt = m_scopeConstraints.find(constraintScope);
        if (scopeIt != m_scopeConstraints.end()) {
            auto& constraints = scopeIt->second;
            auto it = std::find(constraints.begin(), constraints.end(), constraint);
            if (it != constraints.end()) {
                constraints.erase(it);
                return true;
            }
        }
    }
    
    return false;
}

bool ConstraintManager::isConstrained(ConstraintTargetType targetType, const std::string& targetName, 
                                     const std::string& targetSubType, const std::string& scope) const {
    // 检查全局约束
    if (checkGlobalConstraints(targetType, targetName, targetSubType)) {
        return true;
    }
    
    // 检查作用域约束
    if (checkScopeConstraints(targetType, targetName, targetSubType, scope)) {
        return true;
    }
    
    return false;
}

std::vector<std::shared_ptr<ConstraintNode>> ConstraintManager::getConstraintsInScope(const std::string& scope) const {
    auto it = m_scopeConstraints.find(scope);
    if (it != m_scopeConstraints.end()) {
        return it->second;
    }
    return {};
}

std::vector<std::shared_ptr<ConstraintNode>> ConstraintManager::getGlobalConstraints() const {
    return m_globalConstraints;
}

void ConstraintManager::clearAllConstraints() {
    m_globalConstraints.clear();
    m_scopeConstraints.clear();
}

void ConstraintManager::clearScopeConstraints(const std::string& scope) {
    m_scopeConstraints.erase(scope);
}

bool ConstraintManager::validateNode(std::shared_ptr<BaseNode> node, const std::string& scope) const {
    if (!node) {
        return true;
    }
    
    ConstraintTargetType targetType = inferTargetTypeFromNode(node);
    std::string nodeName = getNodeName(node);
    std::string nodeSubType = getNodeSubType(node);
    
    return !isConstrained(targetType, nodeName, nodeSubType, scope);
}

std::vector<std::shared_ptr<ConstraintNode>> ConstraintManager::getViolatedConstraints(
    ConstraintTargetType targetType, const std::string& targetName, 
    const std::string& targetSubType, const std::string& scope) const {
    
    std::vector<std::shared_ptr<ConstraintNode>> violatedConstraints;
    
    // 检查全局约束
    for (auto& constraint : m_globalConstraints) {
        if (constraint->isConstraining(targetType, targetName, targetSubType)) {
            violatedConstraints.push_back(constraint);
        }
    }
    
    // 检查作用域约束
    auto scopeIt = m_scopeConstraints.find(scope);
    if (scopeIt != m_scopeConstraints.end()) {
        for (auto& constraint : scopeIt->second) {
            if (constraint->isConstraining(targetType, targetName, targetSubType)) {
                violatedConstraints.push_back(constraint);
            }
        }
    }
    
    return violatedConstraints;
}

std::string ConstraintManager::generateConstraintReport() const {
    std::stringstream ss;
    
    ss << "Constraint Report:\n";
    ss << "=================\n\n";
    
    // 全局约束
    ss << "Global Constraints (" << m_globalConstraints.size() << "):\n";
    for (const auto& constraint : m_globalConstraints) {
        ss << "  " << constraint->toString() << "\n";
    }
    ss << "\n";
    
    // 作用域约束
    ss << "Scope Constraints:\n";
    for (const auto& [scope, constraints] : m_scopeConstraints) {
        ss << "  Scope: " << scope << " (" << constraints.size() << " constraints)\n";
        for (const auto& constraint : constraints) {
            ss << "    " << constraint->toString() << "\n";
        }
    }
    
    return ss.str();
}

void ConstraintManager::setErrorReporter(ErrorReporter* errorReporter) {
    m_errorReporter = errorReporter;
}

bool ConstraintManager::checkScopeConstraints(ConstraintTargetType targetType, const std::string& targetName,
                                             const std::string& targetSubType, const std::string& scope) const {
    auto scopeIt = m_scopeConstraints.find(scope);
    if (scopeIt != m_scopeConstraints.end()) {
        for (const auto& constraint : scopeIt->second) {
            if (constraint->isConstraining(targetType, targetName, targetSubType)) {
                return true;
            }
        }
    }
    return false;
}

bool ConstraintManager::checkGlobalConstraints(ConstraintTargetType targetType, const std::string& targetName,
                                              const std::string& targetSubType) const {
    for (const auto& constraint : m_globalConstraints) {
        if (constraint->isConstraining(targetType, targetName, targetSubType)) {
            return true;
        }
    }
    return false;
}

ConstraintTargetType ConstraintManager::inferTargetTypeFromNode(std::shared_ptr<BaseNode> node) const {
    switch (node->getNodeType()) {
        case CHTLNodeType::ELEMENT_NODE:
            return ConstraintTargetType::HTML_ELEMENT;
        case CHTLNodeType::TEMPLATE_STYLE_NODE:
            return ConstraintTargetType::TEMPLATE_OBJECT;
        case CHTLNodeType::TEMPLATE_ELEMENT_NODE:
            return ConstraintTargetType::TEMPLATE_OBJECT;
        case CHTLNodeType::TEMPLATE_VAR_NODE:
            return ConstraintTargetType::TEMPLATE_OBJECT;
        case CHTLNodeType::CUSTOM_STYLE_NODE:
        case CHTLNodeType::CUSTOM_ELEMENT_NODE:
        case CHTLNodeType::CUSTOM_VAR_NODE:
            return ConstraintTargetType::CUSTOM_OBJECT;
        case CHTLNodeType::ORIGIN_HTML_NODE:
        case CHTLNodeType::ORIGIN_STYLE_NODE:
        case CHTLNodeType::ORIGIN_JAVASCRIPT_NODE:
        case CHTLNodeType::ORIGIN_CUSTOM_NODE:
            return ConstraintTargetType::ORIGIN_OBJECT;
        default:
            return ConstraintTargetType::HTML_ELEMENT;
    }
}

std::string ConstraintManager::getNodeName(std::shared_ptr<BaseNode> node) const {
    return node->getName();
}

std::string ConstraintManager::getNodeSubType(std::shared_ptr<BaseNode> node) const {
    switch (node->getNodeType()) {
        case CHTLNodeType::TEMPLATE_STYLE_NODE:
        case CHTLNodeType::CUSTOM_STYLE_NODE:
            return "@Style";
        case CHTLNodeType::TEMPLATE_ELEMENT_NODE:
        case CHTLNodeType::CUSTOM_ELEMENT_NODE:
            return "@Element";
        case CHTLNodeType::TEMPLATE_VAR_NODE:
        case CHTLNodeType::CUSTOM_VAR_NODE:
            return "@Var";
        case CHTLNodeType::ORIGIN_HTML_NODE:
            return "@Html";
        case CHTLNodeType::ORIGIN_STYLE_NODE:
            return "@Style";
        case CHTLNodeType::ORIGIN_JAVASCRIPT_NODE:
            return "@JavaScript";
        case CHTLNodeType::ORIGIN_CUSTOM_NODE:
            // 这里需要从节点获取自定义类型
            return "@Custom";
        default:
            return "";
    }
}

// ========== ConstraintValidator 实现 ==========

ConstraintValidator::ConstraintValidator(ConstraintManager* manager) 
    : m_constraintManager(manager) {
}

bool ConstraintValidator::validateHtmlElement(const std::string& elementName, const std::string& scope) const {
    if (!m_constraintManager) {
        return true;
    }
    
    std::string actualScope = scope.empty() ? m_currentScope : scope;
    return !m_constraintManager->isConstrained(ConstraintTargetType::HTML_ELEMENT, elementName, "", actualScope);
}

bool ConstraintValidator::validateTemplateUsage(const std::string& templateType, const std::string& templateName, 
                                               const std::string& scope) const {
    if (!m_constraintManager) {
        return true;
    }
    
    std::string actualScope = scope.empty() ? m_currentScope : scope;
    
    // 检查具体模板约束
    if (m_constraintManager->isConstrained(ConstraintTargetType::TEMPLATE_OBJECT, templateName, templateType, actualScope)) {
        return false;
    }
    
    // 检查模板类型约束
    ConstraintTargetType typeConstraint = parseTypeString("[Template] " + templateType);
    if (m_constraintManager->isConstrained(typeConstraint, "", "", actualScope)) {
        return false;
    }
    
    // 检查全局模板约束
    if (m_constraintManager->isConstrained(ConstraintTargetType::TEMPLATE_TYPE, "", "", actualScope)) {
        return false;
    }
    
    return true;
}

bool ConstraintValidator::validateCustomUsage(const std::string& customType, const std::string& customName, 
                                             const std::string& scope) const {
    if (!m_constraintManager) {
        return true;
    }
    
    std::string actualScope = scope.empty() ? m_currentScope : scope;
    
    // 检查具体自定义约束
    if (m_constraintManager->isConstrained(ConstraintTargetType::CUSTOM_OBJECT, customName, customType, actualScope)) {
        return false;
    }
    
    // 检查自定义类型约束
    ConstraintTargetType typeConstraint = parseTypeString("[Custom] " + customType);
    if (m_constraintManager->isConstrained(typeConstraint, "", "", actualScope)) {
        return false;
    }
    
    // 检查全局自定义约束
    if (m_constraintManager->isConstrained(ConstraintTargetType::CUSTOM_TYPE, "", "", actualScope)) {
        return false;
    }
    
    return true;
}

bool ConstraintValidator::validateOriginUsage(const std::string& originType, const std::string& originName, 
                                             const std::string& scope) const {
    if (!m_constraintManager) {
        return true;
    }
    
    std::string actualScope = scope.empty() ? m_currentScope : scope;
    
    // 检查具体原始嵌入约束
    if (m_constraintManager->isConstrained(ConstraintTargetType::ORIGIN_OBJECT, originName, originType, actualScope)) {
        return false;
    }
    
    // 检查全局原始嵌入约束
    if (m_constraintManager->isConstrained(ConstraintTargetType::ORIGIN_TYPE, "", "", actualScope)) {
        return false;
    }
    
    return true;
}

bool ConstraintValidator::validateTypeUsage(const std::string& typeString, const std::string& scope) const {
    if (!m_constraintManager) {
        return true;
    }
    
    std::string actualScope = scope.empty() ? m_currentScope : scope;
    ConstraintTargetType targetType = parseTypeString(typeString);
    
    return !m_constraintManager->isConstrained(targetType, "", "", actualScope);
}

void ConstraintValidator::setCurrentScope(const std::string& scope) {
    m_currentScope = scope;
}

const std::string& ConstraintValidator::getCurrentScope() const {
    return m_currentScope;
}

ConstraintTargetType ConstraintValidator::parseTypeString(const std::string& typeString) const {
    if (typeString == "@Html") {
        return ConstraintTargetType::HTML_TYPE;
    } else if (typeString == "[Template]") {
        return ConstraintTargetType::TEMPLATE_TYPE;
    } else if (typeString == "[Custom]") {
        return ConstraintTargetType::CUSTOM_TYPE;
    } else if (typeString == "[Origin]") {
        return ConstraintTargetType::ORIGIN_TYPE;
    } else if (typeString == "[Template] @Var") {
        return ConstraintTargetType::TEMPLATE_VAR_TYPE;
    } else if (typeString == "[Custom] @Var") {
        return ConstraintTargetType::CUSTOM_VAR_TYPE;
    } else if (typeString == "[Template] @Style") {
        return ConstraintTargetType::TEMPLATE_STYLE_TYPE;
    } else if (typeString == "[Custom] @Style") {
        return ConstraintTargetType::CUSTOM_STYLE_TYPE;
    } else if (typeString == "[Template] @Element") {
        return ConstraintTargetType::TEMPLATE_ELEMENT_TYPE;
    } else if (typeString == "[Custom] @Element") {
        return ConstraintTargetType::CUSTOM_ELEMENT_TYPE;
    } else {
        return ConstraintTargetType::HTML_ELEMENT;
    }
}

} // namespace CHTL