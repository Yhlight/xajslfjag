#include "InheritanceManager.h"
#include <algorithm>
#include <sstream>
#include <regex>
#include <iostream>

namespace CHTL {

// InheritanceManager 实现
InheritanceManager::InheritanceManager() {
}

void InheritanceManager::registerTemplate(std::shared_ptr<TemplateNode> templateNode) {
    if (!templateNode) return;
    
    String key = makeObjectKey(templateNode->templateName, templateNode->templateType);
    templates[key] = templateNode;
}

void InheritanceManager::registerCustom(std::shared_ptr<CustomNode> customNode) {
    if (!customNode) return;
    
    String key = makeObjectKey(customNode->customName, customNode->customType);
    customs[key] = customNode;
}

void InheritanceManager::clearTemplates() {
    templates.clear();
}

void InheritanceManager::clearCustoms() {
    customs.clear();
}

void InheritanceManager::clearAll() {
    templates.clear();
    customs.clear();
    inheritanceRelations.clear();
    currentConflicts.clear();
}

bool InheritanceManager::addInheritanceRelation(const InheritanceRelation& relation) {
    // 验证继承类型是否匹配
    if (!isValidInheritanceType(relation.childType, relation.parentType)) {
        InheritanceConflict conflict(InheritanceConflictType::TYPE_MISMATCH,
                                    "Type mismatch: " + relation.childType + " cannot inherit from " + relation.parentType);
        reportConflict(conflict);
        return false;
    }
    
    // 检查父对象是否存在
    String parentKey = makeObjectKey(relation.parentName, relation.parentType);
    if (templates.find(parentKey) == templates.end() && customs.find(parentKey) == customs.end()) {
        InheritanceConflict conflict(InheritanceConflictType::UNDEFINED_PARENT,
                                    "Parent object not found: " + relation.parentName + " (" + relation.parentType + ")");
        reportConflict(conflict);
        return false;
    }
    
    // 检查是否会造成循环依赖
    inheritanceRelations.push_back(relation);
    if (hasCircularDependency(relation.childName, relation.childType)) {
        inheritanceRelations.pop_back(); // 撤销添加
        
        StringVector cycle = getCircularDependencyChain(relation.childName, relation.childType);
        String cycleStr = "";
        for (size_t i = 0; i < cycle.size(); ++i) {
            if (i > 0) cycleStr += " -> ";
            cycleStr += cycle[i];
        }
        
        InheritanceConflict conflict(InheritanceConflictType::CIRCULAR_DEPENDENCY,
                                    "Circular dependency detected: " + cycleStr);
        reportConflict(conflict);
        return false;
    }
    
    return true;
}

bool InheritanceManager::removeInheritanceRelation(const String& childName, const String& childType,
                                                   const String& parentName, const String& parentType) {
    auto it = std::find_if(inheritanceRelations.begin(), inheritanceRelations.end(),
        [&](const InheritanceRelation& rel) {
            return rel.childName == childName && rel.childType == childType &&
                   rel.parentName == parentName && rel.parentType == parentType;
        });
    
    if (it != inheritanceRelations.end()) {
        inheritanceRelations.erase(it);
        return true;
    }
    
    return false;
}

StringVector InheritanceManager::getInheritanceRelations(const String& objectName, const String& objectType) const {
    StringVector relations;
    
    for (const auto& rel : inheritanceRelations) {
        if (rel.childName == objectName && rel.childType == objectType) {
            relations.push_back(rel.parentName + " (" + rel.parentType + ")");
        }
    }
    
    return relations;
}

InheritanceRelation InheritanceManager::parseInheritanceExpression(const String& childName, const String& childType,
                                                                   const String& expression, bool isExplicit) {
    InheritanceRelation relation;
    relation.childName = childName;
    relation.childType = childType;
    relation.isExplicit = isExplicit;
    relation.inheritanceExpression = expression;
    
    if (isExplicit) {
        // 解析显性继承: inherit @Style ThemeColor
        return InheritanceExpressionParser::parseExplicitInheritance(childName, childType, expression);
    } else {
        // 解析隐性继承: @Style ThemeColor;
        return InheritanceExpressionParser::parseImplicitInheritance(childName, childType, expression);
    }
}

InheritanceResolution InheritanceManager::resolveInheritance(const String& objectName, const String& objectType) {
    InheritanceResolution resolution;
    resolution.objectName = objectName;
    resolution.objectType = objectType;
    
    // 构建继承链
    resolution.inheritanceChain = buildInheritanceChain(objectName, objectType);
    
    // 检测冲突
    resolution.conflicts = detectConflicts(objectName, objectType);
    
    // 解析属性
    resolution.finalProperties = resolveProperties(objectName, objectType);
    resolution.propertyHistory = resolvePropertyHistory(objectName, objectType);
    
    resolution.isValid = resolution.conflicts.empty() || 
                        std::none_of(resolution.conflicts.begin(), resolution.conflicts.end(),
                                    [](const InheritanceConflict& conflict) {
                                        return conflict.type == InheritanceConflictType::CIRCULAR_DEPENDENCY;
                                    });
    
    return resolution;
}

StringVector InheritanceManager::buildInheritanceChain(const String& objectName, const String& objectType) {
    StringVector chain;
    std::set<String> visited;
    
    std::function<void(const String&, const String&)> buildChainRecursive = 
        [&](const String& name, const String& type) {
            String key = makeObjectKey(name, type);
            if (visited.find(key) != visited.end()) {
                return; // 避免循环
            }
            
            visited.insert(key);
            chain.push_back(name + " (" + type + ")");
            
            // 获取直接父对象
            StringVector parents = getDirectParents(name, type);
            for (const auto& parent : parents) {
                // 解析父对象名称和类型
                size_t pos = parent.find(" (");
                if (pos != String::npos) {
                    String parentName = parent.substr(0, pos);
                    String parentType = parent.substr(pos + 2);
                    parentType = parentType.substr(0, parentType.length() - 1); // 移除 ')'
                    
                    buildChainRecursive(parentName, parentType);
                }
            }
        };
    
    buildChainRecursive(objectName, objectType);
    return chain;
}

bool InheritanceManager::hasCircularDependency(const String& objectName, const String& objectType) {
    std::set<String> visiting;
    std::set<String> visited;
    StringVector path;
    
    return detectCircularDependencyRecursive(objectName, objectType, visiting, visited, path);
}

StringVector InheritanceManager::getCircularDependencyChain(const String& objectName, const String& objectType) {
    std::set<String> visiting;
    std::set<String> visited;
    StringVector path;
    
    detectCircularDependencyRecursive(objectName, objectType, visiting, visited, path);
    return path;
}

std::vector<InheritanceConflict> InheritanceManager::detectConflicts(const String& objectName, const String& objectType) {
    std::vector<InheritanceConflict> conflicts;
    
    // 检查循环依赖
    if (hasCircularDependency(objectName, objectType)) {
        StringVector cycle = getCircularDependencyChain(objectName, objectType);
        String cycleStr = "";
        for (size_t i = 0; i < cycle.size(); ++i) {
            if (i > 0) cycleStr += " -> ";
            cycleStr += cycle[i];
        }
        
        conflicts.emplace_back(InheritanceConflictType::CIRCULAR_DEPENDENCY,
                              "Circular dependency: " + cycleStr);
    }
    
    // 检查属性冲突
    auto propertyHistory = resolvePropertyHistory(objectName, objectType);
    std::unordered_map<String, std::vector<ResolvedProperty>> propertyGroups;
    
    for (const auto& prop : propertyHistory) {
        propertyGroups[prop.name].push_back(prop);
    }
    
    for (const auto& group : propertyGroups) {
        if (group.second.size() > 1) {
            // 存在属性冲突
            conflicts.emplace_back(InheritanceConflictType::PROPERTY_OVERRIDE,
                                  "Property conflict for '" + group.first + "': multiple definitions found");
        }
    }
    
    return conflicts;
}

StringUnorderedMap InheritanceManager::resolveProperties(const String& objectName, const String& objectType) {
    StringUnorderedMap resolvedProperties;
    
    // 获取继承链
    StringVector chain = buildInheritanceChain(objectName, objectType);
    
    // 从继承链的末尾开始合并属性（从最基础的父类开始）
    for (auto it = chain.rbegin(); it != chain.rend(); ++it) {
        // 解析对象名称和类型
        size_t pos = it->find(" (");
        if (pos != String::npos) {
            String name = it->substr(0, pos);
            String type = it->substr(pos + 2);
            type = type.substr(0, type.length() - 1); // 移除 ')'
            
            // 获取对象的属性
            StringUnorderedMap objectProperties;
            String key = makeObjectKey(name, type);
            
            auto templateIt = templates.find(key);
            if (templateIt != templates.end()) {
                if (type == "Style") {
                    auto styleTemplate = std::dynamic_pointer_cast<StyleTemplateNode>(templateIt->second);
                    if (styleTemplate) {
                        objectProperties = styleTemplate->styleProperties;
                    }
                }
                // 其他类型的模板...
            } else {
                auto customIt = customs.find(key);
                if (customIt != customs.end()) {
                    // 处理自定义对象...
                }
            }
            
            // 合并属性（子类属性覆盖父类属性）
            resolvedProperties = mergeProperties(resolvedProperties, objectProperties);
        }
    }
    
    return resolvedProperties;
}

std::vector<ResolvedProperty> InheritanceManager::resolvePropertyHistory(const String& objectName, const String& objectType) {
    std::vector<ResolvedProperty> history;
    
    // 获取继承链
    StringVector chain = buildInheritanceChain(objectName, objectType);
    
    int depth = 0;
    for (auto it = chain.rbegin(); it != chain.rend(); ++it, ++depth) {
        // 解析对象名称和类型
        size_t pos = it->find(" (");
        if (pos != String::npos) {
            String name = it->substr(0, pos);
            String type = it->substr(pos + 2);
            type = type.substr(0, type.length() - 1); // 移除 ')'
            
            // 获取对象的属性
            StringUnorderedMap objectProperties;
            String key = makeObjectKey(name, type);
            
            auto templateIt = templates.find(key);
            if (templateIt != templates.end()) {
                if (type == "Style") {
                    auto styleTemplate = std::dynamic_pointer_cast<StyleTemplateNode>(templateIt->second);
                    if (styleTemplate) {
                        objectProperties = styleTemplate->styleProperties;
                    }
                }
            }
            
            // 添加到历史记录
            for (const auto& prop : objectProperties) {
                ResolvedProperty resolvedProp(prop.first, prop.second, name + " (" + type + ")",
                                             calculatePropertyPriority(name + " (" + type + ")", depth));
                history.push_back(resolvedProp);
            }
        }
    }
    
    return history;
}

bool InheritanceManager::isTemplateRegistered(const String& name, const String& type) const {
    String key = makeObjectKey(name, type);
    return templates.find(key) != templates.end();
}

bool InheritanceManager::isCustomRegistered(const String& name, const String& type) const {
    String key = makeObjectKey(name, type);
    return customs.find(key) != customs.end();
}

bool InheritanceManager::hasInheritanceRelation(const String& childName, const String& childType) const {
    return std::any_of(inheritanceRelations.begin(), inheritanceRelations.end(),
        [&](const InheritanceRelation& rel) {
            return rel.childName == childName && rel.childType == childType;
        });
}

std::shared_ptr<TemplateNode> InheritanceManager::getTemplate(const String& name, const String& type) const {
    String key = makeObjectKey(name, type);
    auto it = templates.find(key);
    return (it != templates.end()) ? it->second : nullptr;
}

std::shared_ptr<CustomNode> InheritanceManager::getCustom(const String& name, const String& type) const {
    String key = makeObjectKey(name, type);
    auto it = customs.find(key);
    return (it != customs.end()) ? it->second : nullptr;
}

bool InheritanceManager::validateInheritanceSystem() {
    currentConflicts.clear();
    
    // 检查所有注册的对象
    for (const auto& templatePair : templates) {
        auto conflicts = detectConflicts(templatePair.second->templateName, templatePair.second->templateType);
        currentConflicts.insert(currentConflicts.end(), conflicts.begin(), conflicts.end());
    }
    
    for (const auto& customPair : customs) {
        auto conflicts = detectConflicts(customPair.second->customName, customPair.second->customType);
        currentConflicts.insert(currentConflicts.end(), conflicts.begin(), conflicts.end());
    }
    
    return currentConflicts.empty();
}

std::vector<InheritanceConflict> InheritanceManager::getAllConflicts() {
    return currentConflicts;
}

String InheritanceManager::toString() const {
    std::ostringstream oss;
    oss << "Inheritance Manager:\n";
    oss << "  Templates: " << templates.size() << "\n";
    oss << "  Customs: " << customs.size() << "\n";
    oss << "  Inheritance Relations: " << inheritanceRelations.size() << "\n";
    
    if (!inheritanceRelations.empty()) {
        oss << "  Relations:\n";
        for (const auto& rel : inheritanceRelations) {
            oss << "    " << rel.toString() << "\n";
        }
    }
    
    return oss.str();
}

String InheritanceManager::getInheritanceTree(const String& objectName, const String& objectType) const {
    std::ostringstream oss;
    
    std::function<void(const String&, const String&, int)> printTreeRecursive = 
        [&](const String& name, const String& type, int depth) {
            for (int i = 0; i < depth; ++i) {
                oss << "  ";
            }
            oss << "- " << name << " (" << type << ")\n";
            
            StringVector parents = getDirectParents(name, type);
            for (const auto& parent : parents) {
                size_t pos = parent.find(" (");
                if (pos != String::npos) {
                    String parentName = parent.substr(0, pos);
                    String parentType = parent.substr(pos + 2);
                    parentType = parentType.substr(0, parentType.length() - 1); // 移除 ')'
                    
                    printTreeRecursive(parentName, parentType, depth + 1);
                }
            }
        };
    
    printTreeRecursive(objectName, objectType, 0);
    return oss.str();
}

void InheritanceManager::printInheritanceGraph() const {
    std::cout << "Inheritance Graph:\n";
    
    for (const auto& templatePair : templates) {
        std::cout << getInheritanceTree(templatePair.second->templateName, templatePair.second->templateType);
    }
    
    for (const auto& customPair : customs) {
        std::cout << getInheritanceTree(customPair.second->customName, customPair.second->customType);
    }
}

// 私有方法实现
String InheritanceManager::makeObjectKey(const String& name, const String& type) const {
    return type + "::" + name;
}

bool InheritanceManager::isValidInheritanceType(const String& childType, const String& parentType) const {
    // 同类型的继承总是有效的
    if (childType == parentType) {
        return true;
    }
    
    // 根据CHTL规范，只要类型匹配就可以继承
    // 这里可以扩展更复杂的继承规则
    return childType == parentType;
}

bool InheritanceManager::detectCircularDependencyRecursive(const String& objectName, const String& objectType,
                                                          std::set<String>& visiting, std::set<String>& visited,
                                                          StringVector& path) const {
    String key = makeObjectKey(objectName, objectType);
    
    if (visiting.find(key) != visiting.end()) {
        // 发现循环
        path.push_back(objectName + " (" + objectType + ")");
        return true;
    }
    
    if (visited.find(key) != visited.end()) {
        // 已经访问过，没有循环
        return false;
    }
    
    visiting.insert(key);
    path.push_back(objectName + " (" + objectType + ")");
    
    // 检查所有父对象
    StringVector parents = getDirectParents(objectName, objectType);
    for (const auto& parent : parents) {
        size_t pos = parent.find(" (");
        if (pos != String::npos) {
            String parentName = parent.substr(0, pos);
            String parentType = parent.substr(pos + 2);
            parentType = parentType.substr(0, parentType.length() - 1); // 移除 ')'
            
            if (detectCircularDependencyRecursive(parentName, parentType, visiting, visited, path)) {
                return true;
            }
        }
    }
    
    visiting.erase(key);
    visited.insert(key);
    path.pop_back();
    
    return false;
}

StringUnorderedMap InheritanceManager::mergeProperties(const StringUnorderedMap& base, const StringUnorderedMap& override) const {
    StringUnorderedMap merged = base;
    
    for (const auto& pair : override) {
        merged[pair.first] = pair.second; // 覆盖属性
    }
    
    return merged;
}

void InheritanceManager::applyPropertyOverrides(StringUnorderedMap& properties, const String& objectName, const String& objectType) const {
    // 这里可以实现更复杂的属性覆盖逻辑
    // 比如处理 delete 操作等
}

StringVector InheritanceManager::getDirectParents(const String& objectName, const String& objectType) const {
    StringVector parents;
    
    for (const auto& rel : inheritanceRelations) {
        if (rel.childName == objectName && rel.childType == objectType) {
            parents.push_back(rel.parentName + " (" + rel.parentType + ")");
        }
    }
    
    return parents;
}

StringVector InheritanceManager::getDirectChildren(const String& objectName, const String& objectType) const {
    StringVector children;
    
    for (const auto& rel : inheritanceRelations) {
        if (rel.parentName == objectName && rel.parentType == objectType) {
            children.push_back(rel.childName + " (" + rel.childType + ")");
        }
    }
    
    return children;
}

int InheritanceManager::calculatePropertyPriority(const String& sourceObject, int depth) const {
    // 深度越小，优先级越高（子类优先于父类）
    return 1000 - depth;
}

void InheritanceManager::reportConflict(const InheritanceConflict& conflict) {
    currentConflicts.push_back(conflict);
}

// InheritanceExpressionParser 实现
InheritanceRelation InheritanceExpressionParser::parseExplicitInheritance(const String& childName, const String& childType,
                                                                          const String& expression) {
    InheritanceRelation relation;
    relation.childName = childName;
    relation.childType = childType;
    relation.isExplicit = true;
    relation.inheritanceExpression = expression;
    
    // 解析表达式: inherit @Style ThemeColor
    std::regex explicitRegex(R"(inherit\s+@(\w+)\s+(\w+))");
    std::smatch match;
    
    if (std::regex_search(expression, match, explicitRegex)) {
        relation.parentType = match[1].str();
        relation.parentName = match[2].str();
        
        // 确定继承类型
        if (relation.childType == relation.parentType) {
            relation.type = InheritanceType::TEMPLATE_TO_TEMPLATE; // 可能需要更精确的判断
        }
    }
    
    return relation;
}

InheritanceRelation InheritanceExpressionParser::parseImplicitInheritance(const String& childName, const String& childType,
                                                                          const String& expression) {
    InheritanceRelation relation;
    relation.childName = childName;
    relation.childType = childType;
    relation.isExplicit = false;
    relation.inheritanceExpression = expression;
    
    // 解析表达式: @Style ThemeColor;
    std::regex implicitRegex(R"(@(\w+)\s+(\w+))");
    std::smatch match;
    
    if (std::regex_search(expression, match, implicitRegex)) {
        relation.parentType = match[1].str();
        relation.parentName = match[2].str();
        
        // 确定继承类型
        if (relation.childType == relation.parentType) {
            relation.type = InheritanceType::TEMPLATE_TO_TEMPLATE; // 可能需要更精确的判断
        }
    }
    
    return relation;
}

bool InheritanceExpressionParser::isValidInheritanceExpression(const String& expression) {
    // 检查显性继承格式
    std::regex explicitRegex(R"(inherit\s+@\w+\s+\w+)");
    if (std::regex_match(expression, explicitRegex)) {
        return true;
    }
    
    // 检查隐性继承格式
    std::regex implicitRegex(R"(@\w+\s+\w+)");
    if (std::regex_match(expression, implicitRegex)) {
        return true;
    }
    
    return false;
}

StringVector InheritanceExpressionParser::validateInheritanceExpression(const String& expression) {
    StringVector errors;
    
    if (!isValidInheritanceExpression(expression)) {
        errors.push_back("Invalid inheritance expression format: " + expression);
    }
    
    return errors;
}

bool InheritanceExpressionParser::parseTypeAndName(const String& expression, String& type, String& name) {
    std::regex typeNameRegex(R"(@(\w+)\s+(\w+))");
    std::smatch match;
    
    if (std::regex_search(expression, match, typeNameRegex)) {
        type = match[1].str();
        name = match[2].str();
        return true;
    }
    
    return false;
}

String InheritanceExpressionParser::extractTypeFromExpression(const String& expression) {
    String type, name;
    if (parseTypeAndName(expression, type, name)) {
        return type;
    }
    return "";
}

String InheritanceExpressionParser::extractNameFromExpression(const String& expression) {
    String type, name;
    if (parseTypeAndName(expression, type, name)) {
        return name;
    }
    return "";
}

bool InheritanceExpressionParser::isExplicitInheritanceKeyword(const String& expression) {
    return expression.find("inherit") != String::npos;
}

// InheritanceConflictResolver 实现
ResolvedProperty InheritanceConflictResolver::resolvePropertyConflict(const std::vector<ResolvedProperty>& conflictingProperties,
                                                                     ResolutionStrategy strategy) {
    if (conflictingProperties.empty()) {
        return ResolvedProperty();
    }
    
    if (conflictingProperties.size() == 1) {
        return conflictingProperties[0];
    }
    
    // 根据策略选择胜出的属性
    ResolvedProperty winner = conflictingProperties[0];
    
    for (const auto& prop : conflictingProperties) {
        int currentPriority = getStrategyPriority(winner, strategy);
        int candidatePriority = getStrategyPriority(prop, strategy);
        
        if (candidatePriority > currentPriority) {
            winner = prop;
        }
    }
    
    return winner;
}

InheritanceResolution InheritanceConflictResolver::resolveInheritanceConflicts(const InheritanceResolution& resolution,
                                                                              ResolutionStrategy strategy) {
    InheritanceResolution resolved = resolution;
    
    // 处理属性冲突
    std::unordered_map<String, std::vector<ResolvedProperty>> propertyGroups;
    
    for (const auto& prop : resolution.propertyHistory) {
        propertyGroups[prop.name].push_back(prop);
    }
    
    resolved.finalProperties.clear();
    resolved.propertyHistory.clear();
    
    for (const auto& group : propertyGroups) {
        ResolvedProperty resolvedProp = resolvePropertyConflict(group.second, strategy);
        resolved.finalProperties[resolvedProp.name] = resolvedProp.value;
        resolved.propertyHistory.push_back(resolvedProp);
    }
    
    return resolved;
}

String InheritanceConflictResolver::generateResolutionSuggestion(const InheritanceConflict& conflict) {
    switch (conflict.type) {
        case InheritanceConflictType::CIRCULAR_DEPENDENCY:
            return "Remove one of the inheritance relationships to break the circular dependency.";
        case InheritanceConflictType::PROPERTY_OVERRIDE:
            return "Use explicit property values in the child object to resolve conflicts.";
        case InheritanceConflictType::TYPE_MISMATCH:
            return "Ensure that the child and parent objects have compatible types.";
        case InheritanceConflictType::UNDEFINED_PARENT:
            return "Define the parent object before creating the inheritance relationship.";
        default:
            return "Review the inheritance structure and resolve conflicts manually.";
    }
}

int InheritanceConflictResolver::getStrategyPriority(const ResolvedProperty& property, ResolutionStrategy strategy) {
    switch (strategy) {
        case ResolutionStrategy::CHILD_PRIORITY:
            return property.priority; // 子对象优先级更高
        case ResolutionStrategy::PARENT_PRIORITY:
            return -property.priority; // 父对象优先级更高
        case ResolutionStrategy::EXPLICIT_PRIORITY:
            return property.priority + 1000; // 显性声明优先
        case ResolutionStrategy::ORDER_PRIORITY:
            return property.priority; // 按声明顺序
        case ResolutionStrategy::TYPE_PRIORITY:
            return property.priority; // 按类型优先级
        default:
            return property.priority;
    }
}

} // namespace CHTL