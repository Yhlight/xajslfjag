#include "TemplateManager.h"
#include <algorithm>
#include <sstream>
#include <unordered_set>

namespace CHTL {

// ========== TemplateManager 实现 ==========

TemplateManager::TemplateManager() : m_errorReporter(nullptr) {
    // 初始化统计信息
    m_statistics["templates_registered"] = 0;
    m_statistics["templates_instantiated"] = 0;
    m_statistics["inheritance_resolved"] = 0;
    m_statistics["specializations_applied"] = 0;
}

bool TemplateManager::registerStyleTemplate(const std::string& name, 
                                           std::shared_ptr<AdvancedStyleTemplateNode> templateNode,
                                           bool isCustom) {
    TemplateRegistration registration(name, "@Style", isCustom ? "Custom" : "Template", templateNode);
    return registerTemplate(registration);
}

bool TemplateManager::registerElementTemplate(const std::string& name,
                                             std::shared_ptr<AdvancedElementTemplateNode> templateNode,
                                             bool isCustom) {
    TemplateRegistration registration(name, "@Element", isCustom ? "Custom" : "Template", templateNode);
    return registerTemplate(registration);
}

bool TemplateManager::registerVarTemplate(const std::string& name,
                                         std::shared_ptr<AdvancedVarTemplateNode> templateNode,
                                         bool isCustom) {
    TemplateRegistration registration(name, "@Var", isCustom ? "Custom" : "Template", templateNode);
    return registerTemplate(registration);
}

bool TemplateManager::registerTemplate(const TemplateRegistration& registration) {
    if (registration.name.empty() || registration.type.empty() || !registration.templateNode) {
        reportError("Invalid template registration");
        return false;
    }
    
    // 检查是否已存在
    if (hasTemplate(registration.name, registration.type)) {
        reportWarning("Template already exists, overwriting: " + registration.name);
    }
    
    // 注册模板
    m_templates[registration.type][registration.name] = registration;
    
    // 更新索引
    updateIndices(m_templates[registration.type][registration.name]);
    
    updateStatistics("templates_registered");
    return true;
}

bool TemplateManager::unregisterTemplate(const std::string& name, const std::string& type, 
                                        const std::string& category) {
    auto typeIt = m_templates.find(type);
    if (typeIt == m_templates.end()) {
        return false;
    }
    
    auto nameIt = typeIt->second.find(name);
    if (nameIt == typeIt->second.end()) {
        return false;
    }
    
    // 如果指定了类别，检查类别是否匹配
    if (!category.empty() && nameIt->second.category != category) {
        return false;
    }
    
    // 移除索引
    removeFromIndices(nameIt->second);
    
    // 移除模板
    typeIt->second.erase(nameIt);
    
    return true;
}

TemplateRegistration* TemplateManager::findTemplate(const std::string& name, const std::string& type) {
    auto typeIt = m_templates.find(type);
    if (typeIt == m_templates.end()) {
        return nullptr;
    }
    
    auto nameIt = typeIt->second.find(name);
    if (nameIt == typeIt->second.end()) {
        return nullptr;
    }
    
    return &nameIt->second;
}

TemplateRegistration* TemplateManager::findTemplate(const std::string& fullyQualifiedName) {
    auto it = m_fullyQualifiedIndex.find(fullyQualifiedName);
    return (it != m_fullyQualifiedIndex.end()) ? it->second : nullptr;
}

bool TemplateManager::hasTemplate(const std::string& name, const std::string& type) {
    return findTemplate(name, type) != nullptr;
}

std::vector<TemplateRegistration> TemplateManager::getAllTemplates() const {
    std::vector<TemplateRegistration> result;
    
    for (const auto& [type, templates] : m_templates) {
        for (const auto& [name, registration] : templates) {
            result.push_back(registration);
        }
    }
    
    return result;
}

std::vector<TemplateRegistration> TemplateManager::getTemplatesByType(const std::string& type) const {
    std::vector<TemplateRegistration> result;
    
    auto it = m_templates.find(type);
    if (it != m_templates.end()) {
        for (const auto& [name, registration] : it->second) {
            result.push_back(registration);
        }
    }
    
    return result;
}

std::vector<TemplateRegistration*> TemplateManager::resolveInheritanceChain(
    const std::string& templateName, const std::string& templateType) {
    
    std::vector<TemplateRegistration*> chain;
    std::unordered_set<std::string> visited;
    
    resolveInheritanceRecursive(templateName, templateType, chain, visited, 0);
    
    updateStatistics("inheritance_resolved");
    return chain;
}

bool TemplateManager::hasCircularInheritance(const std::string& templateName, const std::string& templateType) {
    std::unordered_set<std::string> visited;
    std::vector<TemplateRegistration*> chain;
    
    try {
        resolveInheritanceRecursive(templateName, templateType, chain, visited, 0);
        return false;
    } catch (const std::runtime_error&) {
        return true;
    }
}

NodePtr TemplateManager::instantiateTemplate(const std::string& templateName, const std::string& templateType,
                                            const TemplateContext& context) {
    auto registration = findTemplate(templateName, templateType);
    if (!registration) {
        reportError("Template not found: " + templateType + " " + templateName);
        return nullptr;
    }
    
    // 解析继承链
    auto inheritanceChain = resolveInheritanceChain(templateName, templateType);
    
    // 合并继承的模板
    NodePtr result = mergeInheritedTemplates(inheritanceChain, context);
    
    // 应用特例化操作
    applySpecializations(result, context.specializations);
    
    updateStatistics("templates_instantiated");
    return result;
}

NodePtr TemplateManager::mergeInheritedTemplates(const std::vector<TemplateRegistration*>& inheritanceChain,
                                                const TemplateContext& context) {
    if (inheritanceChain.empty()) {
        return nullptr;
    }
    
    // 根据第一个模板的类型选择合并策略
    const auto& firstTemplate = inheritanceChain[0];
    if (firstTemplate->type == "@Style") {
        return mergeStyleTemplates(inheritanceChain, context);
    } else if (firstTemplate->type == "@Element") {
        return mergeElementTemplates(inheritanceChain, context);
    } else if (firstTemplate->type == "@Var") {
        return mergeVarTemplates(inheritanceChain, context);
    }
    
    return nullptr;
}

NodePtr TemplateManager::mergeStyleTemplates(const std::vector<TemplateRegistration*>& chain,
                                           const TemplateContext& context) {
    // 简化实现：直接返回第一个模板的实例
    if (!chain.empty() && chain[0]->templateNode) {
        auto styleTemplate = std::dynamic_pointer_cast<AdvancedStyleTemplateNode>(chain[0]->templateNode);
        if (styleTemplate) {
            return styleTemplate->instantiate(context.parameters);
        }
    }
    return nullptr;
}

NodePtr TemplateManager::mergeElementTemplates(const std::vector<TemplateRegistration*>& chain,
                                              const TemplateContext& context) {
    // 简化实现：直接返回第一个模板的实例
    if (!chain.empty() && chain[0]->templateNode) {
        auto elementTemplate = std::dynamic_pointer_cast<AdvancedElementTemplateNode>(chain[0]->templateNode);
        if (elementTemplate) {
            return elementTemplate->instantiate(context.parameters);
        }
    }
    return nullptr;
}

NodePtr TemplateManager::mergeVarTemplates(const std::vector<TemplateRegistration*>& chain,
                                         const TemplateContext& context) {
    // 简化实现：直接返回第一个模板的实例
    if (!chain.empty() && chain[0]->templateNode) {
        auto varTemplate = std::dynamic_pointer_cast<AdvancedVarTemplateNode>(chain[0]->templateNode);
        if (varTemplate) {
            return varTemplate->instantiate(context.parameters);
        }
    }
    return nullptr;
}

void TemplateManager::applySpecializations(NodePtr node, const std::vector<SpecializationInfo>& specializations) {
    for (const auto& spec : specializations) {
        switch (spec.type) {
            case SpecializationType::DELETE_PROPERTY:
            case SpecializationType::DELETE_INHERITANCE:
                applyDeleteOperation(node, spec);
                break;
            case SpecializationType::INSERT_ELEMENT:
                applyInsertOperation(node, spec);
                break;
            case SpecializationType::INDEX_ACCESS:
                applyIndexAccess(node, spec);
                break;
            default:
                break;
        }
    }
    
    updateStatistics("specializations_applied");
}

void TemplateManager::applyDeleteOperation(NodePtr node, const SpecializationInfo& operation) {
    // 简化实现：记录删除操作
    (void)node;
    (void)operation;
}

void TemplateManager::applyInsertOperation(NodePtr node, const SpecializationInfo& operation) {
    // 简化实现：记录插入操作
    (void)node;
    (void)operation;
}

NodePtr TemplateManager::applyIndexAccess(NodePtr node, const SpecializationInfo& operation) {
    // 简化实现：返回原节点
    (void)operation;
    return node;
}

std::string TemplateManager::resolveVariableReference(const std::string& varGroupName, 
                                                     const std::string& varName,
                                                     const std::string& overrideValue) {
    if (!overrideValue.empty()) {
        return overrideValue;
    }
    
    // 查找变量组模板
    auto registration = findTemplate(varGroupName, "@Var");
    if (!registration) {
        reportError("Variable group not found: " + varGroupName);
        return "";
    }
    
    auto varTemplate = std::dynamic_pointer_cast<AdvancedVarTemplateNode>(registration->templateNode);
    if (!varTemplate) {
        reportError("Invalid variable template: " + varGroupName);
        return "";
    }
    
    return varTemplate->getVariable(varName);
}

void TemplateManager::setErrorReporter(ErrorReporter* errorReporter) {
    m_errorReporter = errorReporter;
}

std::unordered_map<std::string, size_t> TemplateManager::getStatistics() const {
    return m_statistics;
}

void TemplateManager::clear() {
    m_templates.clear();
    m_fullyQualifiedIndex.clear();
    m_inheritanceCache.clear();
    resetStatistics();
}

void TemplateManager::resetStatistics() {
    for (auto& [key, value] : m_statistics) {
        value = 0;
    }
}

// ========== 内部辅助方法 ==========

std::string TemplateManager::generateTemplateKey(const std::string& name, const std::string& type,
                                                const std::string& category) const {
    return category + "::" + type + "::" + name;
}

void TemplateManager::updateIndices(const TemplateRegistration& registration) {
    m_fullyQualifiedIndex[registration.fullyQualifiedName] = 
        const_cast<TemplateRegistration*>(&registration);
}

void TemplateManager::removeFromIndices(const TemplateRegistration& registration) {
    m_fullyQualifiedIndex.erase(registration.fullyQualifiedName);
}

void TemplateManager::resolveInheritanceRecursive(const std::string& templateName, const std::string& templateType,
                                                 std::vector<TemplateRegistration*>& chain,
                                                 std::unordered_set<std::string>& visited, int depth) {
    if (depth > 10) { // 防止无限递归
        throw std::runtime_error("Maximum inheritance depth exceeded");
    }
    
    std::string key = templateType + "::" + templateName;
    if (visited.find(key) != visited.end()) {
        throw std::runtime_error("Circular inheritance detected");
    }
    
    visited.insert(key);
    
    auto registration = findTemplate(templateName, templateType);
    if (!registration) {
        return;
    }
    
    chain.push_back(registration);
    
    // 简化实现：不处理实际的继承链解析
    // 实际实现中需要解析模板的继承信息
}

void TemplateManager::reportError(const std::string& message) {
    if (m_errorReporter) {
        m_errorReporter->error(ErrorType::TEMPLATE_ERROR, message);
    }
}

void TemplateManager::reportWarning(const std::string& message) {
    if (m_errorReporter) {
        m_errorReporter->warning(message);
    }
}

void TemplateManager::updateStatistics(const std::string& key, size_t increment) {
    m_statistics[key] += increment;
}

// ========== 全局实例 ==========

TemplateManager& getGlobalTemplateManager() {
    static TemplateManager instance;
    return instance;
}

} // namespace CHTL