#include "VariableManager.h"
#include <sstream>
#include <algorithm>
#include <unordered_set>

namespace CHTL {

// ========== VariableContext 实现 ==========

VariableContext::VariableContext() {
}

void VariableContext::setVariable(const std::string& groupName, const std::string& varName, const std::string& value) {
    m_variables[groupName][varName] = value;
}

std::string VariableContext::getVariable(const std::string& groupName, const std::string& varName) const {
    // 优先检查覆盖值
    auto overrideGroupIt = m_overrides.find(groupName);
    if (overrideGroupIt != m_overrides.end()) {
        auto overrideVarIt = overrideGroupIt->second.find(varName);
        if (overrideVarIt != overrideGroupIt->second.end()) {
            return overrideVarIt->second;
        }
    }
    
    // 检查普通变量值
    auto groupIt = m_variables.find(groupName);
    if (groupIt != m_variables.end()) {
        auto varIt = groupIt->second.find(varName);
        if (varIt != groupIt->second.end()) {
            return varIt->second;
        }
    }
    
    return ""; // 变量不存在
}

bool VariableContext::hasVariable(const std::string& groupName, const std::string& varName) const {
    // 检查覆盖值
    auto overrideGroupIt = m_overrides.find(groupName);
    if (overrideGroupIt != m_overrides.end()) {
        if (overrideGroupIt->second.find(varName) != overrideGroupIt->second.end()) {
            return true;
        }
    }
    
    // 检查普通变量值
    auto groupIt = m_variables.find(groupName);
    if (groupIt != m_variables.end()) {
        return groupIt->second.find(varName) != groupIt->second.end();
    }
    
    return false;
}

void VariableContext::overrideVariable(const std::string& groupName, const std::string& varName, const std::string& value) {
    m_overrides[groupName][varName] = value;
}

std::unordered_map<std::string, std::string> VariableContext::getVariableGroup(const std::string& groupName) const {
    std::unordered_map<std::string, std::string> result;
    
    // 获取基础变量
    auto groupIt = m_variables.find(groupName);
    if (groupIt != m_variables.end()) {
        result = groupIt->second;
    }
    
    // 应用覆盖值
    auto overrideGroupIt = m_overrides.find(groupName);
    if (overrideGroupIt != m_overrides.end()) {
        for (const auto& [varName, value] : overrideGroupIt->second) {
            result[varName] = value;
        }
    }
    
    return result;
}

void VariableContext::merge(const VariableContext& other) {
    // 合并普通变量（不覆盖已有变量）
    for (const auto& [groupName, groupVars] : other.m_variables) {
        for (const auto& [varName, value] : groupVars) {
            if (!hasVariable(groupName, varName)) {
                setVariable(groupName, varName, value);
            }
        }
    }
    
    // 合并覆盖值（覆盖已有的覆盖值）
    for (const auto& [groupName, groupOverrides] : other.m_overrides) {
        for (const auto& [varName, value] : groupOverrides) {
            overrideVariable(groupName, varName, value);
        }
    }
}

void VariableContext::clear() {
    m_variables.clear();
    m_overrides.clear();
}

std::unique_ptr<VariableContext> VariableContext::createChild() const {
    auto child = std::make_unique<VariableContext>();
    child->merge(*this);
    return child;
}

// ========== VariableManager 实现 ==========

VariableManager::VariableManager() 
    : m_templateManager(nullptr), m_errorReporter(nullptr),
      m_variablePattern(R"(\b(\w+)\s*\(\s*(\w+)\s*\))") {
}

void VariableManager::setTemplateManager(TemplateManager* templateManager) {
    m_templateManager = templateManager;
}

void VariableManager::setErrorReporter(ErrorReporter* errorReporter) {
    m_errorReporter = errorReporter;
}

void VariableManager::registerVariableGroup(std::shared_ptr<AdvancedVarTemplateNode> varTemplate) {
    if (!varTemplate) return;
    
    std::string groupName = varTemplate->getName();
    m_variableGroups[groupName] = varTemplate;
    updateStatistics("variable_groups_registered");
}

std::shared_ptr<AdvancedVarTemplateNode> VariableManager::getVariableGroup(const std::string& groupName) {
    auto it = m_variableGroups.find(groupName);
    if (it != m_variableGroups.end()) {
        return it->second;
    }
    
    // 如果没有找到，尝试从模板管理器获取
    if (m_templateManager) {
        auto registration = m_templateManager->findTemplate(groupName, "@Var");
        if (registration) {
            auto varTemplate = std::dynamic_pointer_cast<AdvancedVarTemplateNode>(registration->templateNode);
            if (varTemplate) {
                m_variableGroups[groupName] = varTemplate;
                return varTemplate;
            }
        }
    }
    
    return nullptr;
}

bool VariableManager::hasVariableGroup(const std::string& groupName) const {
    return m_variableGroups.find(groupName) != m_variableGroups.end();
}

std::string VariableManager::resolveVariableReference(const std::string& reference) {
    VariableContext emptyContext;
    return resolveVariableReference(reference, emptyContext);
}

std::string VariableManager::resolveVariableReference(const std::string& reference, const VariableContext& context) {
    auto replacement = parseVariableReference(reference);
    if (replacement.varGroupName.empty() || replacement.varName.empty()) {
        reportError("Invalid variable reference: " + reference);
        return reference; // 返回原始引用
    }
    
    // 优先从上下文获取
    if (context.hasVariable(replacement.varGroupName, replacement.varName)) {
        replacement.resolvedValue = context.getVariable(replacement.varGroupName, replacement.varName);
        updateStatistics("variables_resolved_from_context");
        return replacement.resolvedValue;
    }
    
    // 从变量组获取
    auto varGroup = getVariableGroup(replacement.varGroupName);
    if (!varGroup) {
        reportError("Variable group not found: " + replacement.varGroupName);
        return reference;
    }
    
    // 解析继承链
    auto inheritanceChain = resolveInheritanceChain(replacement.varGroupName);
    auto mergedContext = mergeInheritanceChain(inheritanceChain);
    
    // 应用特化操作
    applyVariableSpecializations(mergedContext, varGroup->getSpecializations());
    
    // 获取最终值
    replacement.resolvedValue = mergedContext.getVariable(replacement.varGroupName, replacement.varName);
    
    if (replacement.resolvedValue.empty()) {
        reportError("Variable not found: " + replacement.varName + " in group " + replacement.varGroupName);
        return reference;
    }
    
    updateStatistics("variables_resolved_from_groups");
    return replacement.resolvedValue;
}

std::string VariableManager::resolveVariableReferences(const std::string& content) {
    VariableContext emptyContext;
    return resolveVariableReferences(content, emptyContext);
}

std::string VariableManager::resolveVariableReferences(const std::string& content, const VariableContext& context) {
    std::string result = content;
    std::smatch match;
    
    // 查找并替换所有变量引用
    while (std::regex_search(result, match, m_variablePattern)) {
        std::string fullMatch = match[0].str();
        std::string groupName = match[1].str();
        std::string varName = match[2].str();
        
        // 构造标准引用格式
        std::string standardRef = groupName + "(" + varName + ")";
        std::string resolvedValue = resolveVariableReference(standardRef, context);
        
        // 替换第一个匹配项
        size_t pos = result.find(fullMatch);
        if (pos != std::string::npos) {
            result.replace(pos, fullMatch.length(), resolvedValue);
        }
    }
    
    updateStatistics("variable_replacements_performed");
    return result;
}

std::vector<std::shared_ptr<AdvancedVarTemplateNode>> VariableManager::resolveInheritanceChain(const std::string& groupName) {
    std::vector<std::shared_ptr<AdvancedVarTemplateNode>> chain;
    std::unordered_set<std::string> visited;
    
    std::function<void(const std::string&)> collectInheritance = [&](const std::string& currentGroupName) {
        if (visited.find(currentGroupName) != visited.end()) {
            reportError("Circular inheritance detected in variable group: " + currentGroupName);
            return;
        }
        
        visited.insert(currentGroupName);
        
        auto varGroup = getVariableGroup(currentGroupName);
        if (!varGroup) {
            reportError("Variable group not found in inheritance chain: " + currentGroupName);
            return;
        }
        
        // 先处理父级继承
        for (const auto& inheritance : varGroup->getInheritances()) {
            if (inheritance.inheritanceType == InheritanceType::COMPOSITION ||
                inheritance.inheritanceType == InheritanceType::EXPLICIT) {
                collectInheritance(inheritance.templateName);
            }
        }
        
        // 然后添加当前节点
        chain.push_back(varGroup);
    };
    
    collectInheritance(groupName);
    updateStatistics("inheritance_chains_resolved");
    return chain;
}

VariableContext VariableManager::mergeInheritanceChain(const std::vector<std::shared_ptr<AdvancedVarTemplateNode>>& inheritanceChain) {
    VariableContext mergedContext;
    
    // 按继承顺序合并变量（后面的覆盖前面的）
    for (auto varTemplate : inheritanceChain) {
        if (!varTemplate) continue;
        
        std::string groupName = varTemplate->getName();
        
        // 添加变量组的所有变量
        for (const auto& [varName, value] : varTemplate->getVariables()) {
            mergedContext.setVariable(groupName, varName, value);
        }
    }
    
    updateStatistics("inheritance_chains_merged");
    return mergedContext;
}

void VariableManager::applyVariableSpecializations(VariableContext& context, const std::vector<SpecializationInfo>& specializations) {
    for (const auto& spec : specializations) {
        if (spec.type == SpecializationType::DELETE_PROPERTY) {
            // 删除变量（通过设置空值）
            // 这里需要更复杂的逻辑来真正删除变量
            // 简化实现：设置空值
            if (!spec.target.empty()) {
                // spec.target 应该包含组名.变量名的格式
                size_t dotPos = spec.target.find('.');
                if (dotPos != std::string::npos) {
                    std::string groupName = spec.target.substr(0, dotPos);
                    std::string varName = spec.target.substr(dotPos + 1);
                    context.overrideVariable(groupName, varName, "");
                }
            }
        } else if (spec.type == SpecializationType::INSERT_ELEMENT) {
            // 插入/覆盖变量值
            if (!spec.target.empty() && !spec.value.empty()) {
                size_t dotPos = spec.target.find('.');
                if (dotPos != std::string::npos) {
                    std::string groupName = spec.target.substr(0, dotPos);
                    std::string varName = spec.target.substr(dotPos + 1);
                    context.overrideVariable(groupName, varName, spec.value);
                }
            }
        }
    }
    
    updateStatistics("variable_specializations_applied");
}

std::vector<VariableReplacement> VariableManager::findVariableReferences(const std::string& content) {
    std::vector<VariableReplacement> references;
    std::smatch match;
    std::string::const_iterator searchStart(content.cbegin());
    
    while (std::regex_search(searchStart, content.cend(), match, m_variablePattern)) {
        VariableReplacement replacement;
        replacement.varGroupName = match[1].str();
        replacement.varName = match[2].str();
        references.push_back(replacement);
        
        searchStart = match.suffix().first;
    }
    
    return references;
}

bool VariableManager::validateVariableReference(const VariableReplacement& replacement) {
    if (replacement.varGroupName.empty() || replacement.varName.empty()) {
        return false;
    }
    
    auto varGroup = getVariableGroup(replacement.varGroupName);
    if (!varGroup) {
        return false;
    }
    
    return varGroup->hasVariable(replacement.varName);
}

std::string VariableManager::generateVariableReport() const {
    std::stringstream ss;
    
    ss << "Variable System Report:\n";
    ss << "======================\n\n";
    
    ss << "Registered Variable Groups (" << m_variableGroups.size() << "):\n";
    for (const auto& [groupName, varTemplate] : m_variableGroups) {
        ss << "  " << groupName << ":\n";
        if (varTemplate) {
            for (const auto& [varName, value] : varTemplate->getVariables()) {
                ss << "    " << varName << " = \"" << value << "\"\n";
            }
            
            // 显示继承信息
            const auto& inheritances = varTemplate->getInheritances();
            if (!inheritances.empty()) {
                ss << "    Inherits from: ";
                for (size_t i = 0; i < inheritances.size(); ++i) {
                    if (i > 0) ss << ", ";
                    ss << inheritances[i].templateName;
                }
                ss << "\n";
            }
            
            // 显示特化信息
            const auto& specializations = varTemplate->getSpecializations();
            if (!specializations.empty()) {
                ss << "    Specializations: " << specializations.size() << "\n";
            }
        }
        ss << "\n";
    }
    
    ss << "Statistics:\n";
    for (const auto& [key, count] : m_statistics) {
        ss << "  " << key << ": " << count << "\n";
    }
    
    return ss.str();
}

VariableReplacement VariableManager::parseVariableReference(const std::string& reference) {
    VariableReplacement replacement;
    std::smatch match;
    
    if (std::regex_match(reference, match, m_variablePattern)) {
        replacement.varGroupName = match[1].str();
        replacement.varName = match[2].str();
    }
    
    return replacement;
}

VariableContext VariableManager::createContextFromVariableGroup(const std::string& groupName) {
    VariableContext context;
    
    auto varGroup = getVariableGroup(groupName);
    if (varGroup) {
        for (const auto& [varName, value] : varGroup->getVariables()) {
            context.setVariable(groupName, varName, value);
        }
    }
    
    return context;
}

VariableContext VariableManager::createContextFromParameters(const std::unordered_map<std::string, std::string>& parameters) {
    VariableContext context;
    
    // 假设参数格式为 "groupName.varName" = value
    for (const auto& [key, value] : parameters) {
        size_t dotPos = key.find('.');
        if (dotPos != std::string::npos) {
            std::string groupName = key.substr(0, dotPos);
            std::string varName = key.substr(dotPos + 1);
            context.setVariable(groupName, varName, value);
        }
    }
    
    return context;
}

void VariableManager::clear() {
    m_variableGroups.clear();
    m_statistics.clear();
}

void VariableManager::reportError(const std::string& message) {
    if (m_errorReporter) {
        m_errorReporter->error(ErrorType::TEMPLATE_ERROR, message);
    }
}

void VariableManager::updateStatistics(const std::string& key) {
    m_statistics[key]++;
}

bool VariableManager::hasCircularInheritance(const std::string& groupName, std::unordered_set<std::string>& visited) {
    if (visited.find(groupName) != visited.end()) {
        return true; // 检测到循环
    }
    
    visited.insert(groupName);
    
    auto varGroup = getVariableGroup(groupName);
    if (varGroup) {
        for (const auto& inheritance : varGroup->getInheritances()) {
            if (hasCircularInheritance(inheritance.templateName, visited)) {
                return true;
            }
        }
    }
    
    visited.erase(groupName);
    return false;
}

} // namespace CHTL