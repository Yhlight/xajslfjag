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
    if (chain.empty()) {
        return nullptr;
    }
    
    // 创建结果样式模板
    auto resultTemplate = std::make_shared<AdvancedStyleTemplateNode>("merged_template");
    
    // 从基类到派生类的顺序合并属性（继承链是反向的）
    for (auto it = chain.rbegin(); it != chain.rend(); ++it) {
        auto registration = *it;
        if (!registration->templateNode) continue;
        
        auto styleTemplate = std::dynamic_pointer_cast<AdvancedStyleTemplateNode>(registration->templateNode);
        if (!styleTemplate) continue;
        
        // 合并CSS属性
        auto params = styleTemplate->getParameters();
        for (const auto& [property, value] : params) {
            // 后面的模板覆盖前面的属性（继承覆盖）
            resultTemplate->setParameter(property, value);
        }
        
        // 合并无值属性（仅适用于Custom模板）
        for (const auto& propName : styleTemplate->getPropertiesWithoutValues()) {
            resultTemplate->addPropertyWithoutValue(propName);
        }
        
        // 应用特例化操作
        for (const auto& spec : styleTemplate->getSpecializations()) {
            switch (spec.type) {
                case SpecializationType::DELETE_PROPERTY:
                    // 删除指定属性
                    resultTemplate->setParameter(spec.target, ""); // 标记为删除
                    break;
                case SpecializationType::DELETE_INHERITANCE:
                    // 删除继承（在继承链中处理）
                    break;
                default:
                    break;
            }
        }
    }
    
    // 应用上下文特例化操作
    for (const auto& spec : context.specializations) {
        applySpecializationToStyleTemplate(resultTemplate.get(), spec);
    }
    
    return resultTemplate;
}

void TemplateManager::applySpecializationToStyleTemplate(AdvancedStyleTemplateNode* styleTemplate, 
                                                        const SpecializationInfo& spec) {
    switch (spec.type) {
        case SpecializationType::DELETE_PROPERTY:
            styleTemplate->setParameter(spec.target, ""); // 标记删除
            break;
        case SpecializationType::DELETE_INHERITANCE:
            // 删除特定继承
            styleTemplate->removeInheritance(spec.target);
            break;
        default:
            break;
    }
}

NodePtr TemplateManager::mergeElementTemplates(const std::vector<TemplateRegistration*>& chain,
                                              const TemplateContext& context) {
    if (chain.empty()) {
        return nullptr;
    }
    
    // 创建结果元素模板
    auto resultTemplate = std::make_shared<AdvancedElementTemplateNode>("merged_element_template");
    
    // 从基类到派生类的顺序合并元素（继承链是反向的）
    for (auto it = chain.rbegin(); it != chain.rend(); ++it) {
        auto registration = *it;
        if (!registration->templateNode) continue;
        
        auto elementTemplate = std::dynamic_pointer_cast<AdvancedElementTemplateNode>(registration->templateNode);
        if (!elementTemplate) continue;
        
        // 合并子元素
        for (const auto& child : elementTemplate->getChildren()) {
            resultTemplate->addChild(child->clone());
        }
        
        // 应用特例化操作
        for (const auto& spec : elementTemplate->getSpecializations()) {
            applySpecializationToElementTemplate(resultTemplate.get(), spec);
        }
    }
    
    // 应用上下文特例化操作
    for (const auto& spec : context.specializations) {
        applySpecializationToElementTemplate(resultTemplate.get(), spec);
    }
    
    return resultTemplate;
}

void TemplateManager::applySpecializationToElementTemplate(AdvancedElementTemplateNode* elementTemplate, 
                                                          const SpecializationInfo& spec) {
    switch (spec.type) {
        case SpecializationType::DELETE_PROPERTY: {
            // 删除元素（通过选择器或索引）
            if (spec.index >= 0) {
                // 按索引删除
                elementTemplate->removeChild(static_cast<size_t>(spec.index));
            } else if (!spec.target.empty()) {
                // 按选择器删除
                auto elements = elementTemplate->getElementsByTag(spec.target);
                for (auto element : elements) {
                    elementTemplate->removeChild(element.get());
                }
            }
            break;
        }
        case SpecializationType::INSERT_ELEMENT: {
            // 插入元素
            insertElementAtPosition(elementTemplate, spec);
            break;
        }
        case SpecializationType::DELETE_INHERITANCE: {
            // 删除继承
            elementTemplate->removeInheritance(spec.target);
            break;
        }
        default:
            break;
    }
}

void TemplateManager::insertElementAtPosition(AdvancedElementTemplateNode* elementTemplate, 
                                            const SpecializationInfo& spec) {
    auto children = elementTemplate->getChildren();
    size_t insertPos = children.size(); // 默认插入到末尾
    
    // 计算插入位置
    if (spec.index >= 0) {
        // 基于索引的插入
        switch (spec.position) {
            case InsertPosition::BEFORE:
                insertPos = std::max(0, spec.index);
                break;
            case InsertPosition::AFTER:
                insertPos = std::min(static_cast<int>(children.size()), spec.index + 1);
                break;
            case InsertPosition::REPLACE:
                if (spec.index >= 0 && spec.index < static_cast<int>(children.size())) {
                    elementTemplate->removeChild(static_cast<size_t>(spec.index));
                    insertPos = spec.index;
                }
                break;
            case InsertPosition::AT_TOP:
                insertPos = 0;
                break;
            case InsertPosition::AT_BOTTOM:
                insertPos = children.size();
                break;
        }
    } else if (!spec.target.empty()) {
        // 基于选择器的插入
        auto targetElements = elementTemplate->getElementsByTag(spec.target);
        if (!targetElements.empty()) {
            // 找到第一个匹配元素的位置
            auto targetElement = targetElements[0];
            for (size_t i = 0; i < children.size(); ++i) {
                if (children[i] == targetElement) {
                    switch (spec.position) {
                        case InsertPosition::BEFORE:
                            insertPos = i;
                            break;
                        case InsertPosition::AFTER:
                            insertPos = i + 1;
                            break;
                        case InsertPosition::REPLACE:
                            elementTemplate->removeChild(i);
                            insertPos = i;
                            break;
                        case InsertPosition::AT_TOP:
                            insertPos = 0;
                            break;
                        case InsertPosition::AT_BOTTOM:
                            insertPos = children.size();
                            break;
                    }
                    break;
                }
            }
        }
    }
    
    // 插入新元素（简化实现，直接添加到末尾）
    for (const auto& newElement : spec.content) {
        elementTemplate->addChild(newElement->clone());
    }
}

NodePtr TemplateManager::mergeVarTemplates(const std::vector<TemplateRegistration*>& chain,
                                         const TemplateContext& context) {
    if (chain.empty()) {
        return nullptr;
    }
    
    // 创建结果变量模板
    auto resultTemplate = std::make_shared<AdvancedVarTemplateNode>("merged_var_template");
    
    // 从基类到派生类的顺序合并变量（继承链是反向的）
    for (auto it = chain.rbegin(); it != chain.rend(); ++it) {
        auto registration = *it;
        if (!registration->templateNode) continue;
        
        auto varTemplate = std::dynamic_pointer_cast<AdvancedVarTemplateNode>(registration->templateNode);
        if (!varTemplate) continue;
        
        // 合并变量
        auto variables = varTemplate->getVariables();
        for (const auto& [varName, varValue] : variables) {
            // 后面的模板覆盖前面的变量（继承覆盖）
            resultTemplate->setVariable(varName, varValue);
        }
        
        // 应用特例化操作
        for (const auto& spec : varTemplate->getSpecializations()) {
            applySpecializationToVarTemplate(resultTemplate.get(), spec);
        }
    }
    
    // 应用上下文变量覆盖
    for (const auto& [varName, varValue] : context.variables) {
        resultTemplate->setVariable(varName, varValue);
    }
    
    // 应用上下文特例化操作
    for (const auto& spec : context.specializations) {
        applySpecializationToVarTemplate(resultTemplate.get(), spec);
    }
    
    return resultTemplate;
}

void TemplateManager::applySpecializationToVarTemplate(AdvancedVarTemplateNode* varTemplate, 
                                                      const SpecializationInfo& spec) {
    switch (spec.type) {
        case SpecializationType::DELETE_PROPERTY: {
            // 变量特例化：覆盖变量值或删除变量
            if (!spec.value.empty()) {
                // 覆盖变量值
                varTemplate->setVariable(spec.target, spec.value);
            } else {
                // 删除变量（设置为空值标记删除）
                varTemplate->setVariable(spec.target, "");
            }
            break;
        }
        case SpecializationType::DELETE_INHERITANCE: {
            // 删除继承
            varTemplate->removeInheritance(spec.target);
            break;
        }
        default:
            break;
    }
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
    
    // 解析变量组的继承链
    auto inheritanceChain = resolveInheritanceChain(varGroupName, "@Var");
    
    // 创建临时上下文来合并变量
    TemplateContext tempContext;
    auto mergedTemplate = std::dynamic_pointer_cast<AdvancedVarTemplateNode>(
        mergeVarTemplates(inheritanceChain, tempContext)
    );
    
    if (!mergedTemplate) {
        reportError("Failed to merge variable template: " + varGroupName);
        return "";
    }
    
    std::string result = mergedTemplate->getVariable(varName);
    if (result.empty()) {
        reportError("Variable not found: " + varName + " in group " + varGroupName);
    }
    
    updateStatistics("variable_references_resolved");
    return result;
}

std::string TemplateManager::replaceVariableReferences(const std::string& content, const TemplateContext& context) {
    std::string result = content;
    
    // 查找变量引用模式：VarGroupName(varName)
    // 简化实现：使用正则表达式或字符串搜索
    size_t pos = 0;
    while ((pos = result.find('(', pos)) != std::string::npos) {
        // 向前查找变量组名
        size_t groupStart = pos;
        while (groupStart > 0 && (std::isalnum(result[groupStart - 1]) || result[groupStart - 1] == '_')) {
            groupStart--;
        }
        
        if (groupStart == pos) {
            pos++;
            continue;
        }
        
        // 查找变量名结束位置
        size_t varEnd = result.find(')', pos);
        if (varEnd == std::string::npos) {
            pos++;
            continue;
        }
        
        std::string varGroupName = result.substr(groupStart, pos - groupStart);
        std::string varName = result.substr(pos + 1, varEnd - pos - 1);
        
        // 检查是否有覆盖值 varName = value
        std::string overrideValue;
        size_t equalPos = varName.find('=');
        if (equalPos != std::string::npos) {
            overrideValue = varName.substr(equalPos + 1);
            varName = varName.substr(0, equalPos);
            
            // 移除空格
            varName.erase(0, varName.find_first_not_of(' '));
            varName.erase(varName.find_last_not_of(' ') + 1);
            overrideValue.erase(0, overrideValue.find_first_not_of(' '));
            overrideValue.erase(overrideValue.find_last_not_of(' ') + 1);
        }
        
        // 解析变量引用
        std::string resolvedValue = resolveVariableReference(varGroupName, varName, overrideValue);
        
        // 替换整个变量引用
        result.replace(groupStart, varEnd - groupStart + 1, resolvedValue);
        
        // 从替换后的位置继续搜索
        pos = groupStart + resolvedValue.length();
    }
    
    updateStatistics("variable_replacements");
    return result;
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
        throw std::runtime_error("Maximum inheritance depth exceeded: " + templateName);
    }
    
    std::string key = templateType + "::" + templateName;
    if (visited.find(key) != visited.end()) {
        throw std::runtime_error("Circular inheritance detected: " + key);
    }
    
    visited.insert(key);
    
    auto registration = findTemplate(templateName, templateType);
    if (!registration) {
        reportError("Template not found in inheritance chain: " + templateType + " " + templateName);
        return;
    }
    
    chain.push_back(registration);
    
    // 解析模板的继承信息
    if (templateType == "@Style") {
        auto styleTemplate = std::dynamic_pointer_cast<AdvancedStyleTemplateNode>(registration->templateNode);
        if (styleTemplate) {
            for (const auto& inheritance : styleTemplate->getInheritances()) {
                // 递归解析继承的模板
                resolveInheritanceRecursive(inheritance.templateName, inheritance.templateType, 
                                          chain, visited, depth + 1);
            }
        }
    } else if (templateType == "@Element") {
        auto elementTemplate = std::dynamic_pointer_cast<AdvancedElementTemplateNode>(registration->templateNode);
        if (elementTemplate) {
            for (const auto& inheritance : elementTemplate->getInheritances()) {
                resolveInheritanceRecursive(inheritance.templateName, inheritance.templateType, 
                                          chain, visited, depth + 1);
            }
        }
    } else if (templateType == "@Var") {
        auto varTemplate = std::dynamic_pointer_cast<AdvancedVarTemplateNode>(registration->templateNode);
        if (varTemplate) {
            for (const auto& inheritance : varTemplate->getInheritances()) {
                resolveInheritanceRecursive(inheritance.templateName, inheritance.templateType, 
                                          chain, visited, depth + 1);
            }
        }
    }
    
    visited.erase(key); // 移除访问标记，允许在其他路径中重复访问
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