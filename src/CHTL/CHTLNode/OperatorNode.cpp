#include "OperatorNode.h"
#include <algorithm>
#include <sstream>

namespace CHTL {

// ========== OperatorNode 实现 ==========

OperatorNode::OperatorNode(OperatorType operatorType, const std::string& target)
    : BaseNode(CHTLNodeType::USE_NODE, "operator")
    , m_operatorType(operatorType)
    , m_target(target) {
    
    // 根据操作类型设置节点类型
    switch (operatorType) {
        case OperatorType::USE_OP:
            m_nodeType = CHTLNodeType::USE_NODE;
            break;
        case OperatorType::INHERIT_OP:
            m_nodeType = CHTLNodeType::INHERIT_NODE;
            break;
        case OperatorType::DELETE_OP:
            m_nodeType = CHTLNodeType::DELETE_NODE;
            break;
        case OperatorType::INSERT_OP:
            m_nodeType = CHTLNodeType::INSERT_NODE;
            break;
        case OperatorType::EXCEPT_OP:
            m_nodeType = CHTLNodeType::EXCEPT_NODE;
            break;
    }
    
    setName(getOperatorTypeString());
}

OperatorType OperatorNode::getOperatorType() const {
    return m_operatorType;
}

const std::string& OperatorNode::getTarget() const {
    return m_target;
}

void OperatorNode::setTarget(const std::string& target) {
    m_target = target;
}

std::string OperatorNode::getOperatorTypeString() const {
    switch (m_operatorType) {
        case OperatorType::USE_OP: return "use";
        case OperatorType::INHERIT_OP: return "inherit";
        case OperatorType::DELETE_OP: return "delete";
        case OperatorType::INSERT_OP: return "insert";
        case OperatorType::EXCEPT_OP: return "except";
        default: return "unknown";
    }
}

std::shared_ptr<BaseNode> OperatorNode::clone() const {
    auto cloned = std::make_shared<OperatorNode>(m_operatorType, m_target);
    cloned->m_position = m_position;
    cloned->m_access = m_access;
    cloned->m_attributes = m_attributes;
    return cloned;
}

std::string OperatorNode::toString() const {
    std::stringstream ss;
    ss << getOperatorTypeString();
    if (!m_target.empty()) {
        ss << " " << m_target;
    }
    return ss.str();
}

bool OperatorNode::validate(ErrorReporter* errorReporter) const {
    if (!BaseNode::validate(errorReporter)) {
        return false;
    }
    
    return internalValidate(errorReporter);
}

bool OperatorNode::internalValidate(ErrorReporter* errorReporter) const {
    // 基础验证
    if (m_target.empty() && m_operatorType != OperatorType::USE_OP) {
        if (errorReporter) {
            ErrorPosition pos("", m_position.line, m_position.column);
            errorReporter->warning("Operator has empty target", pos);
        }
    }
    
    return true;
}

// ========== UseNode 实现 ==========

UseNode::UseNode(const std::string& target)
    : OperatorNode(OperatorType::USE_OP, target)
    , m_isHtml5(false) {
    
    if (target == "html5") {
        m_isHtml5 = true;
    } else {
        m_configName = target;
    }
}

bool UseNode::isHtml5Mode() const {
    return m_isHtml5;
}

void UseNode::setHtml5Mode(bool html5) {
    m_isHtml5 = html5;
    if (html5) {
        setTarget("html5");
        m_configName.clear();
    }
}

std::string UseNode::getConfigName() const {
    return m_configName;
}

void UseNode::setConfigName(const std::string& configName) {
    m_configName = configName;
    setTarget(configName);
    m_isHtml5 = false;
}

std::shared_ptr<BaseNode> UseNode::clone() const {
    auto cloned = std::make_shared<UseNode>(getTarget());
    cloned->m_position = m_position;
    cloned->m_access = m_access;
    cloned->m_attributes = m_attributes;
    cloned->m_isHtml5 = m_isHtml5;
    cloned->m_configName = m_configName;
    return cloned;
}

// ========== InheritNode 实现 ==========

InheritNode::InheritNode(const std::string& target)
    : OperatorNode(OperatorType::INHERIT_OP, target) {
}

std::string InheritNode::getInheritType() const {
    return m_inheritType;
}

void InheritNode::setInheritType(const std::string& type) {
    m_inheritType = type;
}

std::string InheritNode::getSourceName() const {
    return m_sourceName;
}

void InheritNode::setSourceName(const std::string& sourceName) {
    m_sourceName = sourceName;
    setTarget(m_inheritType + " " + sourceName);
}

std::shared_ptr<BaseNode> InheritNode::clone() const {
    auto cloned = std::make_shared<InheritNode>(getTarget());
    cloned->m_position = m_position;
    cloned->m_access = m_access;
    cloned->m_attributes = m_attributes;
    cloned->m_inheritType = m_inheritType;
    cloned->m_sourceName = m_sourceName;
    return cloned;
}

// ========== DeleteNode 实现 ==========

DeleteNode::DeleteNode(const std::string& target)
    : OperatorNode(OperatorType::DELETE_OP, target)
    , m_deletingInheritance(false) {
    
    if (!target.empty()) {
        addDeleteTarget(target);
    }
}

void DeleteNode::addDeleteTarget(const std::string& target) {
    if (std::find(m_deleteTargets.begin(), m_deleteTargets.end(), target) 
        == m_deleteTargets.end()) {
        m_deleteTargets.push_back(target);
    }
}

const std::vector<std::string>& DeleteNode::getDeleteTargets() const {
    return m_deleteTargets;
}

bool DeleteNode::isDeleting(const std::string& target) const {
    return std::find(m_deleteTargets.begin(), m_deleteTargets.end(), target) 
           != m_deleteTargets.end();
}

void DeleteNode::clearDeleteTargets() {
    m_deleteTargets.clear();
}

bool DeleteNode::isDeletingInheritance() const {
    return m_deletingInheritance;
}

void DeleteNode::setDeletingInheritance(bool deleting) {
    m_deletingInheritance = deleting;
}

std::shared_ptr<BaseNode> DeleteNode::clone() const {
    auto cloned = std::make_shared<DeleteNode>(getTarget());
    cloned->m_position = m_position;
    cloned->m_access = m_access;
    cloned->m_attributes = m_attributes;
    cloned->m_deleteTargets = m_deleteTargets;
    cloned->m_deletingInheritance = m_deletingInheritance;
    return cloned;
}

// ========== InsertNode 实现 ==========

InsertNode::InsertNode(const std::string& target)
    : OperatorNode(OperatorType::INSERT_OP, target)
    , m_insertPosition(InsertPosition::AFTER)
    , m_index(-1)
    , m_hasIndex(false) {
}

InsertPosition InsertNode::getInsertPosition() const {
    return m_insertPosition;
}

void InsertNode::setInsertPosition(InsertPosition position) {
    m_insertPosition = position;
}

std::string InsertNode::getInsertPositionString() const {
    switch (m_insertPosition) {
        case InsertPosition::BEFORE: return "before";
        case InsertPosition::AFTER: return "after";
        case InsertPosition::REPLACE: return "replace";
        case InsertPosition::AT_TOP: return "at top";
        case InsertPosition::AT_BOTTOM: return "at bottom";
        default: return "unknown";
    }
}

std::string InsertNode::getTargetSelector() const {
    return m_targetSelector;
}

void InsertNode::setTargetSelector(const std::string& selector) {
    m_targetSelector = selector;
}

int InsertNode::getIndex() const {
    return m_index;
}

void InsertNode::setIndex(int index) {
    m_index = index;
    m_hasIndex = true;
}

bool InsertNode::hasIndex() const {
    return m_hasIndex;
}

std::shared_ptr<BaseNode> InsertNode::clone() const {
    auto cloned = std::make_shared<InsertNode>(getTarget());
    cloned->m_position = m_position;
    cloned->m_access = m_access;
    cloned->m_attributes = m_attributes;
    cloned->m_insertPosition = m_insertPosition;
    cloned->m_targetSelector = m_targetSelector;
    cloned->m_index = m_index;
    cloned->m_hasIndex = m_hasIndex;
    return cloned;
}

// ========== ExceptNode 实现 ==========

ExceptNode::ExceptNode(const std::string& target)
    : OperatorNode(OperatorType::EXCEPT_OP, target)
    , m_isGlobal(false) {
    
    if (!target.empty()) {
        addConstraintTarget(target);
    }
}

void ExceptNode::addConstraintTarget(const std::string& target) {
    if (std::find(m_constraintTargets.begin(), m_constraintTargets.end(), target) 
        == m_constraintTargets.end()) {
        m_constraintTargets.push_back(target);
    }
}

const std::vector<std::string>& ExceptNode::getConstraintTargets() const {
    return m_constraintTargets;
}

bool ExceptNode::isConstraining(const std::string& target) const {
    return std::find(m_constraintTargets.begin(), m_constraintTargets.end(), target) 
           != m_constraintTargets.end();
}

void ExceptNode::clearConstraintTargets() {
    m_constraintTargets.clear();
}

bool ExceptNode::isGlobalConstraint() const {
    return m_isGlobal;
}

void ExceptNode::setGlobalConstraint(bool global) {
    m_isGlobal = global;
}

std::shared_ptr<BaseNode> ExceptNode::clone() const {
    auto cloned = std::make_shared<ExceptNode>(getTarget());
    cloned->m_position = m_position;
    cloned->m_access = m_access;
    cloned->m_attributes = m_attributes;
    cloned->m_constraintTargets = m_constraintTargets;
    cloned->m_isGlobal = m_isGlobal;
    return cloned;
}

} // namespace CHTL