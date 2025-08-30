#include "AdvancedTemplateNode.h"
#include "ElementNode.h"
#include <algorithm>
#include <sstream>

namespace CHTL {

// ========== AdvancedStyleTemplateNode 实现 ==========

AdvancedStyleTemplateNode::AdvancedStyleTemplateNode(const std::string& name)
    : StyleTemplateNode(name) {
}

void AdvancedStyleTemplateNode::addInheritance(const InheritanceInfo& inheritance) {
    m_inheritances.push_back(inheritance);
}

void AdvancedStyleTemplateNode::addInheritance(const std::string& templateName, InheritanceType type) {
    InheritanceInfo info(templateName, "@Style", type);
    m_inheritances.push_back(info);
}

void AdvancedStyleTemplateNode::removeInheritance(const std::string& templateName) {
    auto it = std::remove_if(m_inheritances.begin(), m_inheritances.end(),
                            [&templateName](const InheritanceInfo& info) {
                                return info.templateName == templateName;
                            });
    m_inheritances.erase(it, m_inheritances.end());
}

const std::vector<InheritanceInfo>& AdvancedStyleTemplateNode::getInheritances() const {
    return m_inheritances;
}

void AdvancedStyleTemplateNode::addPropertyWithoutValue(const std::string& propertyName) {
    if (std::find(m_propertiesWithoutValues.begin(), m_propertiesWithoutValues.end(), propertyName) 
        == m_propertiesWithoutValues.end()) {
        m_propertiesWithoutValues.push_back(propertyName);
    }
}

bool AdvancedStyleTemplateNode::isPropertyWithoutValue(const std::string& propertyName) const {
    return std::find(m_propertiesWithoutValues.begin(), m_propertiesWithoutValues.end(), propertyName)
           != m_propertiesWithoutValues.end();
}

const std::vector<std::string>& AdvancedStyleTemplateNode::getPropertiesWithoutValues() const {
    return m_propertiesWithoutValues;
}

void AdvancedStyleTemplateNode::addSpecialization(const SpecializationInfo& specialization) {
    m_specializations.push_back(specialization);
}

void AdvancedStyleTemplateNode::deleteProperty(const std::string& propertyName) {
    SpecializationInfo spec;
    spec.type = SpecializationType::DELETE_PROPERTY;
    spec.target = propertyName;
    m_specializations.push_back(spec);
}

void AdvancedStyleTemplateNode::deleteInheritance(const std::string& templateName) {
    SpecializationInfo spec;
    spec.type = SpecializationType::DELETE_INHERITANCE;
    spec.target = templateName;
    m_specializations.push_back(spec);
}

const std::vector<SpecializationInfo>& AdvancedStyleTemplateNode::getSpecializations() const {
    return m_specializations;
}

NodePtr AdvancedStyleTemplateNode::instantiate(const std::unordered_map<std::string, std::string>& args) const {
    // 创建基础实例
    auto instance = StyleTemplateNode::instantiate(args);
    
    // 应用继承
    for (const auto& inheritance : m_inheritances) {
        // 这里需要模板管理器来解析继承的模板
        // 暂时简化实现
    }
    
    // 应用特例化
    return instantiateWithSpecializations(args, m_specializations);
}

NodePtr AdvancedStyleTemplateNode::instantiateWithSpecializations(
    const std::unordered_map<std::string, std::string>& args,
    const std::vector<SpecializationInfo>& specializations) const {
    
    auto instance = StyleTemplateNode::instantiate(args);
    
    // 应用特例化操作
    for (const auto& spec : specializations) {
        switch (spec.type) {
            case SpecializationType::DELETE_PROPERTY:
                // 删除属性的逻辑
                break;
            case SpecializationType::DELETE_INHERITANCE:
                // 删除继承的逻辑
                break;
            default:
                break;
        }
    }
    
    return instance;
}

std::string AdvancedStyleTemplateNode::toString() const {
    std::stringstream ss;
    ss << "[Template] @Style " << getTemplateName() << " {\n";
    
    // 输出继承信息
    for (const auto& inheritance : m_inheritances) {
        if (inheritance.inheritanceType == InheritanceType::EXPLICIT) {
            ss << "    inherit @Style " << inheritance.templateName << ";\n";
        } else {
            ss << "    @Style " << inheritance.templateName << ";\n";
        }
    }
    
    // 输出属性
    for (const auto& [property, value] : getParameters()) {
        if (isPropertyWithoutValue(property)) {
            ss << "    " << property << ";\n";
        } else {
            ss << "    " << property << ": " << value << ";\n";
        }
    }
    
    // 输出特例化操作
    for (const auto& spec : m_specializations) {
        switch (spec.type) {
            case SpecializationType::DELETE_PROPERTY:
                ss << "    delete " << spec.target << ";\n";
                break;
            case SpecializationType::DELETE_INHERITANCE:
                ss << "    delete @Style " << spec.target << ";\n";
                break;
            default:
                break;
        }
    }
    
    ss << "}";
    return ss.str();
}

NodePtr AdvancedStyleTemplateNode::clone() const {
    auto cloned = std::make_shared<AdvancedStyleTemplateNode>(getTemplateName());
    
    // 复制继承信息
    cloned->m_inheritances = m_inheritances;
    cloned->m_propertiesWithoutValues = m_propertiesWithoutValues;
    cloned->m_specializations = m_specializations;
    
    // 复制基类数据
    // ...
    
    return cloned;
}

// ========== AdvancedElementTemplateNode 实现 ==========

AdvancedElementTemplateNode::AdvancedElementTemplateNode(const std::string& name)
    : ElementTemplateNode(name) {
}

void AdvancedElementTemplateNode::addInheritance(const InheritanceInfo& inheritance) {
    m_inheritances.push_back(inheritance);
}

void AdvancedElementTemplateNode::addInheritance(const std::string& templateName, InheritanceType type) {
    InheritanceInfo info(templateName, "@Element", type);
    m_inheritances.push_back(info);
}

void AdvancedElementTemplateNode::removeInheritance(const std::string& templateName) {
    auto it = std::remove_if(m_inheritances.begin(), m_inheritances.end(),
                            [&templateName](const InheritanceInfo& info) {
                                return info.templateName == templateName;
                            });
    m_inheritances.erase(it, m_inheritances.end());
}

const std::vector<InheritanceInfo>& AdvancedElementTemplateNode::getInheritances() const {
    return m_inheritances;
}

NodePtr AdvancedElementTemplateNode::getChildByIndex(int index) const {
    const auto& children = getChildren();
    if (index >= 0 && index < static_cast<int>(children.size())) {
        return children[index];
    }
    return nullptr;
}

NodePtr AdvancedElementTemplateNode::getElementByTagAndIndex(const std::string& tagName, int index) const {
    auto elements = getElementsByTag(tagName);
    if (index >= 0 && index < static_cast<int>(elements.size())) {
        return elements[index];
    }
    return nullptr;
}

std::vector<NodePtr> AdvancedElementTemplateNode::getElementsByTag(const std::string& tagName) const {
    std::vector<NodePtr> result;
    const auto& children = getChildren();
    
    for (const auto& child : children) {
        if (child->getNodeType() == CHTLNodeType::ELEMENT_NODE) {
            auto element = std::dynamic_pointer_cast<ElementNode>(child);
            if (element && element->getTagName() == tagName) {
                result.push_back(child);
            }
        }
    }
    
    return result;
}

void AdvancedElementTemplateNode::addSpecialization(const SpecializationInfo& specialization) {
    m_specializations.push_back(specialization);
}

void AdvancedElementTemplateNode::insertElement(InsertPosition position, const std::string& selector,
                                               const std::vector<NodePtr>& elements) {
    SpecializationInfo spec;
    spec.type = SpecializationType::INSERT_ELEMENT;
    spec.position = position;
    spec.target = selector;
    spec.content = elements;
    m_specializations.push_back(spec);
}

void AdvancedElementTemplateNode::insertElement(InsertPosition position, int index,
                                               const std::vector<NodePtr>& elements) {
    SpecializationInfo spec;
    spec.type = SpecializationType::INSERT_ELEMENT;
    spec.position = position;
    spec.index = index;
    spec.content = elements;
    m_specializations.push_back(spec);
}

void AdvancedElementTemplateNode::deleteElement(const std::string& selector) {
    SpecializationInfo spec;
    spec.type = SpecializationType::DELETE_PROPERTY; // 复用用于删除元素
    spec.target = selector;
    m_specializations.push_back(spec);
}

void AdvancedElementTemplateNode::deleteElement(int index) {
    SpecializationInfo spec;
    spec.type = SpecializationType::DELETE_PROPERTY; // 复用用于删除元素
    spec.index = index;
    m_specializations.push_back(spec);
}

void AdvancedElementTemplateNode::deleteInheritance(const std::string& templateName) {
    SpecializationInfo spec;
    spec.type = SpecializationType::DELETE_INHERITANCE;
    spec.target = templateName;
    m_specializations.push_back(spec);
}

const std::vector<SpecializationInfo>& AdvancedElementTemplateNode::getSpecializations() const {
    return m_specializations;
}

NodePtr AdvancedElementTemplateNode::instantiate(const std::unordered_map<std::string, std::string>& args) const {
    return instantiateWithSpecializations(args, m_specializations);
}

NodePtr AdvancedElementTemplateNode::instantiateWithSpecializations(
    const std::unordered_map<std::string, std::string>& args,
    const std::vector<SpecializationInfo>& specializations) const {
    
    auto instance = ElementTemplateNode::instantiate(args);
    applySpecializations(instance, specializations);
    return instance;
}

void AdvancedElementTemplateNode::applySpecializations(NodePtr instance,
                                                      const std::vector<SpecializationInfo>& specializations) const {
    for (const auto& spec : specializations) {
        switch (spec.type) {
            case SpecializationType::INSERT_ELEMENT:
                // 插入元素的逻辑
                break;
            case SpecializationType::DELETE_PROPERTY: // 用于删除元素
                // 删除元素的逻辑
                break;
            case SpecializationType::DELETE_INHERITANCE:
                // 删除继承的逻辑
                break;
            default:
                break;
        }
    }
}

std::string AdvancedElementTemplateNode::toString() const {
    std::stringstream ss;
    ss << "[Template] @Element " << getTemplateName() << " {\n";
    
    // 输出继承信息
    for (const auto& inheritance : m_inheritances) {
        if (inheritance.inheritanceType == InheritanceType::EXPLICIT) {
            ss << "    inherit @Element " << inheritance.templateName << ";\n";
        } else {
            ss << "    @Element " << inheritance.templateName << ";\n";
        }
    }
    
    // 输出子元素
    for (const auto& child : getChildren()) {
        ss << "    " << child->toString() << "\n";
    }
    
    ss << "}";
    return ss.str();
}

NodePtr AdvancedElementTemplateNode::clone() const {
    auto cloned = std::make_shared<AdvancedElementTemplateNode>(getTemplateName());
    cloned->m_inheritances = m_inheritances;
    cloned->m_specializations = m_specializations;
    return cloned;
}

// ========== AdvancedVarTemplateNode 实现 ==========

AdvancedVarTemplateNode::AdvancedVarTemplateNode(const std::string& name)
    : VarTemplateNode(name) {
}

void AdvancedVarTemplateNode::addInheritance(const InheritanceInfo& inheritance) {
    m_inheritances.push_back(inheritance);
}

void AdvancedVarTemplateNode::addInheritance(const std::string& templateName, InheritanceType type) {
    InheritanceInfo info(templateName, "@Var", type);
    m_inheritances.push_back(info);
}

void AdvancedVarTemplateNode::removeInheritance(const std::string& templateName) {
    auto it = std::remove_if(m_inheritances.begin(), m_inheritances.end(),
                            [&templateName](const InheritanceInfo& info) {
                                return info.templateName == templateName;
                            });
    m_inheritances.erase(it, m_inheritances.end());
}

const std::vector<InheritanceInfo>& AdvancedVarTemplateNode::getInheritances() const {
    return m_inheritances;
}

void AdvancedVarTemplateNode::setVariable(const std::string& name, const std::string& value) {
    m_variables[name] = value;
}

std::string AdvancedVarTemplateNode::getVariable(const std::string& name) const {
    auto it = m_variables.find(name);
    return (it != m_variables.end()) ? it->second : "";
}

std::string AdvancedVarTemplateNode::getVariable(const std::string& name, const std::string& overrideValue) const {
    return overrideValue.empty() ? getVariable(name) : overrideValue;
}

bool AdvancedVarTemplateNode::hasVariable(const std::string& name) const {
    return m_variables.find(name) != m_variables.end();
}

const std::unordered_map<std::string, std::string>& AdvancedVarTemplateNode::getVariables() const {
    return m_variables;
}

void AdvancedVarTemplateNode::addSpecialization(const SpecializationInfo& specialization) {
    m_specializations.push_back(specialization);
}

void AdvancedVarTemplateNode::specializeVariable(const std::string& varName, const std::string& newValue) {
    SpecializationInfo spec;
    spec.type = SpecializationType::DELETE_PROPERTY; // 复用用于变量特例化
    spec.target = varName;
    spec.value = newValue;
    m_specializations.push_back(spec);
}

const std::vector<SpecializationInfo>& AdvancedVarTemplateNode::getSpecializations() const {
    return m_specializations;
}

NodePtr AdvancedVarTemplateNode::instantiate(const std::unordered_map<std::string, std::string>& args) const {
    return instantiateWithSpecializations(args, m_specializations);
}

NodePtr AdvancedVarTemplateNode::instantiateWithSpecializations(
    const std::unordered_map<std::string, std::string>& args,
    const std::vector<SpecializationInfo>& specializations) const {
    
    auto instance = VarTemplateNode::instantiate(args);
    
    // 应用特例化操作（变量覆盖）
    for (const auto& spec : specializations) {
        if (spec.type == SpecializationType::DELETE_PROPERTY) { // 用于变量特例化
            // 覆盖变量值
        }
    }
    
    return instance;
}

std::string AdvancedVarTemplateNode::toString() const {
    std::stringstream ss;
    ss << "[Template] @Var " << getTemplateName() << " {\n";
    
    // 输出继承信息
    for (const auto& inheritance : m_inheritances) {
        if (inheritance.inheritanceType == InheritanceType::EXPLICIT) {
            ss << "    inherit @Var " << inheritance.templateName << ";\n";
        } else {
            ss << "    @Var " << inheritance.templateName << ";\n";
        }
    }
    
    // 输出变量
    for (const auto& [name, value] : m_variables) {
        ss << "    " << name << ": " << value << ";\n";
    }
    
    ss << "}";
    return ss.str();
}

NodePtr AdvancedVarTemplateNode::clone() const {
    auto cloned = std::make_shared<AdvancedVarTemplateNode>(getTemplateName());
    cloned->m_inheritances = m_inheritances;
    cloned->m_specializations = m_specializations;
    cloned->m_variables = m_variables;
    return cloned;
}

// ========== CustomNode 实现 ==========

CustomNode::CustomNode(CustomType type, const std::string& name)
    : BaseNode(CHTLNodeType::CUSTOM_STYLE_NODE, name), m_customType(type), m_name(name) {
}

CustomType CustomNode::getCustomType() const {
    return m_customType;
}

const std::string& CustomNode::getName() const {
    return m_name;
}

void CustomNode::setName(const std::string& name) {
    m_name = name;
}

std::string CustomNode::toString() const {
    std::string typeStr;
    switch (m_customType) {
        case CustomType::CUSTOM_STYLE: typeStr = "@Style"; break;
        case CustomType::CUSTOM_ELEMENT: typeStr = "@Element"; break;
        case CustomType::CUSTOM_VAR: typeStr = "@Var"; break;
    }
    
    return "[Custom] " + typeStr + " " + m_name;
}

NodePtr CustomNode::clone() const {
    return std::make_shared<CustomNode>(m_customType, m_name);
}

bool CustomNode::validate(ErrorReporter* errorReporter) const {
    (void)errorReporter; // 暂时不使用
    return !m_name.empty();
}

// ========== TemplateUsageNode 实现 ==========

TemplateUsageNode::TemplateUsageNode(const std::string& templateType, const std::string& templateName)
    : BaseNode(CHTLNodeType::TEMPLATE_USAGE_NODE, templateName), m_templateType(templateType), m_templateName(templateName), m_fullyQualified(false) {
}

const std::string& TemplateUsageNode::getTemplateType() const {
    return m_templateType;
}

const std::string& TemplateUsageNode::getTemplateName() const {
    return m_templateName;
}

void TemplateUsageNode::setParameter(const std::string& name, const std::string& value) {
    m_parameters[name] = value;
}

std::string TemplateUsageNode::getParameter(const std::string& name) const {
    auto it = m_parameters.find(name);
    return (it != m_parameters.end()) ? it->second : "";
}

const std::unordered_map<std::string, std::string>& TemplateUsageNode::getParameters() const {
    return m_parameters;
}

void TemplateUsageNode::addSpecialization(const SpecializationInfo& specialization) {
    m_specializations.push_back(specialization);
}

const std::vector<SpecializationInfo>& TemplateUsageNode::getSpecializations() const {
    return m_specializations;
}

void TemplateUsageNode::setFullyQualified(bool fullyQualified) {
    m_fullyQualified = fullyQualified;
}

bool TemplateUsageNode::isFullyQualified() const {
    return m_fullyQualified;
}

std::string TemplateUsageNode::toString() const {
    std::stringstream ss;
    if (m_fullyQualified) {
        ss << "[Template] ";
    }
    ss << m_templateType << " " << m_templateName;
    
    if (!m_parameters.empty()) {
        ss << " {";
        for (const auto& [name, value] : m_parameters) {
            ss << " " << name << ": " << value << ";";
        }
        ss << " }";
    } else {
        ss << ";";
    }
    
    return ss.str();
}

NodePtr TemplateUsageNode::clone() const {
    auto cloned = std::make_shared<TemplateUsageNode>(m_templateType, m_templateName);
    cloned->m_parameters = m_parameters;
    cloned->m_specializations = m_specializations;
    cloned->m_fullyQualified = m_fullyQualified;
    return cloned;
}

bool TemplateUsageNode::validate(ErrorReporter* errorReporter) const {
    (void)errorReporter; // 暂时不使用
    return !m_templateType.empty() && !m_templateName.empty();
}

} // namespace CHTL