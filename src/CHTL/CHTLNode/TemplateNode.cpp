#include "TemplateNode.h"
#include <algorithm>
#include <sstream>

namespace CHTL {

// ========== TemplateNode 实现 ==========

TemplateNode::TemplateNode(TemplateType templateType, const std::string& templateName)
    : BaseNode(CHTLNodeType::TEMPLATE_STYLE_NODE, templateName)
    , m_templateType(templateType)
    , m_templateName(templateName) {
    
    // 根据模板类型设置节点类型
    switch (templateType) {
        case TemplateType::STYLE_TEMPLATE:
            m_nodeType = CHTLNodeType::TEMPLATE_STYLE_NODE;
            break;
        case TemplateType::ELEMENT_TEMPLATE:
            m_nodeType = CHTLNodeType::TEMPLATE_ELEMENT_NODE;
            break;
        case TemplateType::VAR_TEMPLATE:
            m_nodeType = CHTLNodeType::TEMPLATE_VAR_NODE;
            break;
    }
}

TemplateType TemplateNode::getTemplateType() const {
    return m_templateType;
}

const std::string& TemplateNode::getTemplateName() const {
    return m_templateName;
}

void TemplateNode::setTemplateName(const std::string& name) {
    m_templateName = name;
    setName(name);
}

std::string TemplateNode::getFullQualifiedName() const {
    return getTemplateTypeString() + " " + m_templateName;
}

void TemplateNode::addInheritedTemplate(const std::string& templateName) {
    if (std::find(m_inheritedTemplates.begin(), m_inheritedTemplates.end(), templateName) 
        == m_inheritedTemplates.end()) {
        m_inheritedTemplates.push_back(templateName);
    }
}

bool TemplateNode::removeInheritedTemplate(const std::string& templateName) {
    auto it = std::find(m_inheritedTemplates.begin(), m_inheritedTemplates.end(), templateName);
    if (it != m_inheritedTemplates.end()) {
        m_inheritedTemplates.erase(it);
        return true;
    }
    return false;
}

const std::vector<std::string>& TemplateNode::getInheritedTemplates() const {
    return m_inheritedTemplates;
}

bool TemplateNode::inheritsFrom(const std::string& templateName) const {
    return std::find(m_inheritedTemplates.begin(), m_inheritedTemplates.end(), templateName) 
           != m_inheritedTemplates.end();
}

void TemplateNode::setParameter(const std::string& name, const std::string& value) {
    m_parameters[name] = value;
}

std::string TemplateNode::getParameter(const std::string& name) const {
    auto it = m_parameters.find(name);
    return (it != m_parameters.end()) ? it->second : "";
}

bool TemplateNode::hasParameter(const std::string& name) const {
    return m_parameters.find(name) != m_parameters.end();
}

const std::unordered_map<std::string, std::string>& TemplateNode::getParameters() const {
    return m_parameters;
}

bool TemplateNode::removeParameter(const std::string& name) {
    auto it = m_parameters.find(name);
    if (it != m_parameters.end()) {
        m_parameters.erase(it);
        return true;
    }
    return false;
}

NodePtr TemplateNode::instantiate(const std::unordered_map<std::string, std::string>& args) const {
    // 基础实现，子类应重写
    auto instance = clone();
    return instance;
}

bool TemplateNode::canInstantiate() const {
    // 检查是否所有必需参数都已提供
    return true; // 基础实现
}

std::vector<std::string> TemplateNode::getRequiredParameters() const {
    return {}; // 基础实现
}

std::shared_ptr<BaseNode> TemplateNode::clone() const {
    auto cloned = std::make_shared<TemplateNode>(m_templateType, m_templateName);
    cloned->m_position = m_position;
    cloned->m_access = m_access;
    cloned->m_attributes = m_attributes;
    cloned->m_inheritedTemplates = m_inheritedTemplates;
    cloned->m_parameters = m_parameters;
    return cloned;
}

std::shared_ptr<BaseNode> TemplateNode::deepClone() const {
    auto cloned = std::static_pointer_cast<TemplateNode>(clone());
    
    for (const auto& child : m_children) {
        auto clonedChild = child->deepClone();
        cloned->addChild(clonedChild);
    }
    
    return cloned;
}

std::string TemplateNode::toString() const {
    std::stringstream ss;
    ss << "[Template] " << getTemplateTypeString() << " " << m_templateName;
    
    if (!m_inheritedTemplates.empty()) {
        ss << " (inherits: ";
        bool first = true;
        for (const auto& inherited : m_inheritedTemplates) {
            if (!first) ss << ", ";
            ss << inherited;
            first = false;
        }
        ss << ")";
    }
    
    return ss.str();
}

bool TemplateNode::validate(ErrorReporter* errorReporter) const {
    if (!BaseNode::validate(errorReporter)) {
        return false;
    }
    
    return internalValidate(errorReporter);
}

bool TemplateNode::internalValidate(ErrorReporter* errorReporter) const {
    // 验证模板名称
    if (m_templateName.empty()) {
        if (errorReporter) {
            ErrorPosition pos("", m_position.line, m_position.column);
            errorReporter->error(ErrorType::SEMANTIC_ERROR, "Template has empty name", pos);
        }
        return false;
    }
    
    return true;
}

std::string TemplateNode::getTemplateTypeString() const {
    switch (m_templateType) {
        case TemplateType::STYLE_TEMPLATE: return "@Style";
        case TemplateType::ELEMENT_TEMPLATE: return "@Element";
        case TemplateType::VAR_TEMPLATE: return "@Var";
        default: return "@Unknown";
    }
}

// ========== StyleTemplateNode 实现 ==========

StyleTemplateNode::StyleTemplateNode(const std::string& templateName)
    : TemplateNode(TemplateType::STYLE_TEMPLATE, templateName) {
}

void StyleTemplateNode::addCssProperty(const std::string& property, const std::string& value) {
    m_cssProperties[property] = value;
}

std::string StyleTemplateNode::getCssProperty(const std::string& property) const {
    auto it = m_cssProperties.find(property);
    return (it != m_cssProperties.end()) ? it->second : "";
}

bool StyleTemplateNode::hasCssProperty(const std::string& property) const {
    return m_cssProperties.find(property) != m_cssProperties.end();
}

const std::unordered_map<std::string, std::string>& StyleTemplateNode::getCssProperties() const {
    return m_cssProperties;
}

bool StyleTemplateNode::removeCssProperty(const std::string& property) {
    auto it = m_cssProperties.find(property);
    if (it != m_cssProperties.end()) {
        m_cssProperties.erase(it);
        return true;
    }
    return false;
}

NodePtr StyleTemplateNode::instantiate(const std::unordered_map<std::string, std::string>& args) const {
    auto instance = std::static_pointer_cast<StyleTemplateNode>(clone());
    
    // 应用参数替换
    for (const auto& arg : args) {
        if (instance->hasCssProperty(arg.first)) {
            instance->addCssProperty(arg.first, arg.second);
        }
    }
    
    return instance;
}

std::shared_ptr<BaseNode> StyleTemplateNode::clone() const {
    auto cloned = std::make_shared<StyleTemplateNode>(getTemplateName());
    cloned->m_position = m_position;
    cloned->m_access = m_access;
    cloned->m_attributes = m_attributes;
    cloned->m_cssProperties = m_cssProperties;
    return cloned;
}

// ========== ElementTemplateNode 实现 ==========

ElementTemplateNode::ElementTemplateNode(const std::string& templateName)
    : TemplateNode(TemplateType::ELEMENT_TEMPLATE, templateName) {
}

NodePtr ElementTemplateNode::instantiate(const std::unordered_map<std::string, std::string>& args) const {
    return deepClone();
}

std::shared_ptr<BaseNode> ElementTemplateNode::clone() const {
    auto cloned = std::make_shared<ElementTemplateNode>(getTemplateName());
    cloned->m_position = m_position;
    cloned->m_access = m_access;
    cloned->m_attributes = m_attributes;
    return cloned;
}

// ========== VarTemplateNode 实现 ==========

VarTemplateNode::VarTemplateNode(const std::string& templateName)
    : TemplateNode(TemplateType::VAR_TEMPLATE, templateName) {
}

void VarTemplateNode::setVariable(const std::string& name, const std::string& value) {
    m_variables[name] = value;
}

std::string VarTemplateNode::getVariable(const std::string& name) const {
    auto it = m_variables.find(name);
    return (it != m_variables.end()) ? it->second : "";
}

bool VarTemplateNode::hasVariable(const std::string& name) const {
    return m_variables.find(name) != m_variables.end();
}

const std::unordered_map<std::string, std::string>& VarTemplateNode::getVariables() const {
    return m_variables;
}

bool VarTemplateNode::removeVariable(const std::string& name) {
    auto it = m_variables.find(name);
    if (it != m_variables.end()) {
        m_variables.erase(it);
        return true;
    }
    return false;
}

NodePtr VarTemplateNode::instantiate(const std::unordered_map<std::string, std::string>& args) const {
    auto instance = std::static_pointer_cast<VarTemplateNode>(clone());
    
    // 应用参数替换
    for (const auto& arg : args) {
        instance->setVariable(arg.first, arg.second);
    }
    
    return instance;
}

std::shared_ptr<BaseNode> VarTemplateNode::clone() const {
    auto cloned = std::make_shared<VarTemplateNode>(getTemplateName());
    cloned->m_position = m_position;
    cloned->m_access = m_access;
    cloned->m_attributes = m_attributes;
    cloned->m_variables = m_variables;
    return cloned;
}

} // namespace CHTL