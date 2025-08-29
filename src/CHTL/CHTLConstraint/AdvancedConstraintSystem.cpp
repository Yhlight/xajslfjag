#include "AdvancedConstraintSystem.h"
#include <algorithm>
#include <sstream>
#include <regex>
#include <fstream>
#include <iostream>

namespace CHTL {

// ========== ConstraintRule 基类实现 ==========

ConstraintRule::ConstraintRule(const String& name) 
    : ruleName(name), type(ConstraintType::PRECISE), action(ConstraintAction::DENY), 
      severity(ConstraintSeverity::MEDIUM), isEnabled(true), isGlobal(false), priority(1.0) {
}

bool ConstraintRule::matches(const String& target, const ConstraintContext& context) const {
    if (!isEnabled) {
        return false;
    }
    
    // 检查命名空间约束
    if (!namespace_.empty() && context.currentNamespace != namespace_) {
        return false;
    }
    
    // 检查目标匹配
    if (!matchesTarget(target)) {
        return false;
    }
    
    // 检查例外
    if (isException(target)) {
        return false;
    }
    
    // 检查条件
    if (!condition.empty() && !evaluateCondition(context)) {
        return false;
    }
    
    return true;
}

bool ConstraintRule::matchesType(NodeType nodeType, const ConstraintContext& context) const {
    // 基类默认不进行类型匹配
    return false;
}

bool ConstraintRule::matchesNamespace(const String& namespaceName, const ConstraintContext& context) const {
    if (namespace_.empty()) {
        return true; // 无命名空间约束时匹配所有
    }
    
    return namespace_ == namespaceName;
}

bool ConstraintRule::evaluateCondition(const ConstraintContext& context) const {
    if (condition.empty()) {
        return true;
    }
    
    return evaluateSimpleCondition(condition, context);
}

String ConstraintRule::expandCondition(const String& condition, const ConstraintContext& context) const {
    return expandVariables(condition, context);
}

ConstraintAction ConstraintRule::getAction(const String& target, const ConstraintContext& context) const {
    return action;
}

String ConstraintRule::getReplacementContent(const String& originalContent, const ConstraintContext& context) const {
    auto it = parameters.find("replacement");
    return it != parameters.end() ? expandVariables(it->second, context) : "";
}

String ConstraintRule::getTransformationResult(const String& originalContent, const ConstraintContext& context) const {
    auto it = parameters.find("transform");
    if (it != parameters.end()) {
        return expandVariables(it->second, context);
    }
    return originalContent;
}

bool ConstraintRule::validateRule() const {
    return getValidationErrors().empty();
}

StringVector ConstraintRule::getValidationErrors() const {
    StringVector errors;
    
    if (ruleName.empty()) {
        errors.push_back("规则名称不能为空");
    }
    
    if (targets.empty() && type == ConstraintType::PRECISE) {
        errors.push_back("精确约束必须指定目标");
    }
    
    if (action == ConstraintAction::REPLACE && parameters.find("replacement") == parameters.end()) {
        errors.push_back("替换操作必须指定替换内容");
    }
    
    if (action == ConstraintAction::TRANSFORM && parameters.find("transform") == parameters.end()) {
        errors.push_back("转换操作必须指定转换规则");
    }
    
    return errors;
}

void ConstraintRule::optimizeRule() {
    // 去重目标列表
    std::sort(targets.begin(), targets.end());
    targets.erase(std::unique(targets.begin(), targets.end()), targets.end());
    
    // 去重例外列表
    std::sort(exceptions.begin(), exceptions.end());
    exceptions.erase(std::unique(exceptions.begin(), exceptions.end()), exceptions.end());
}

String ConstraintRule::toString() const {
    std::ostringstream oss;
    oss << "ConstraintRule {\n";
    oss << "  name: " << ruleName << "\n";
    oss << "  description: " << description << "\n";
    oss << "  type: " << static_cast<int>(type) << "\n";
    oss << "  action: " << static_cast<int>(action) << "\n";
    oss << "  severity: " << static_cast<int>(severity) << "\n";
    oss << "  enabled: " << (isEnabled ? "true" : "false") << "\n";
    oss << "  global: " << (isGlobal ? "true" : "false") << "\n";
    oss << "  priority: " << priority << "\n";
    oss << "  targets: [";
    for (size_t i = 0; i < targets.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << targets[i];
    }
    oss << "]\n";
    oss << "  exceptions: [";
    for (size_t i = 0; i < exceptions.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << exceptions[i];
    }
    oss << "]\n";
    oss << "}";
    
    return oss.str();
}

String ConstraintRule::toJSON() const {
    std::ostringstream json;
    json << "{\n";
    json << "  \"name\": \"" << ruleName << "\",\n";
    json << "  \"description\": \"" << description << "\",\n";
    json << "  \"type\": " << static_cast<int>(type) << ",\n";
    json << "  \"action\": " << static_cast<int>(action) << ",\n";
    json << "  \"severity\": " << static_cast<int>(severity) << ",\n";
    json << "  \"enabled\": " << (isEnabled ? "true" : "false") << ",\n";
    json << "  \"global\": " << (isGlobal ? "true" : "false") << ",\n";
    json << "  \"priority\": " << priority << ",\n";
    json << "  \"targets\": [";
    for (size_t i = 0; i < targets.size(); ++i) {
        if (i > 0) json << ", ";
        json << "\"" << targets[i] << "\"";
    }
    json << "],\n";
    json << "  \"exceptions\": [";
    for (size_t i = 0; i < exceptions.size(); ++i) {
        if (i > 0) json << ", ";
        json << "\"" << exceptions[i] << "\"";
    }
    json << "]\n";
    json << "}";
    
    return json.str();
}

bool ConstraintRule::matchesTarget(const String& target) const {
    if (targets.empty()) {
        return true; // 无目标约束时匹配所有
    }
    
    for (const String& pattern : targets) {
        if (ConstraintUtils::matchesPattern(target, pattern)) {
            return true;
        }
    }
    
    return false;
}

bool ConstraintRule::isException(const String& target) const {
    for (const String& exception : exceptions) {
        if (ConstraintUtils::matchesPattern(target, exception)) {
            return true;
        }
    }
    
    return false;
}

bool ConstraintRule::evaluateSimpleCondition(const String& condition, const ConstraintContext& context) const {
    // 简化的条件评估
    String expandedCondition = expandVariables(condition, context);
    
    // 支持基本的条件表达式
    if (expandedCondition == "true") return true;
    if (expandedCondition == "false") return false;
    
    // 检查嵌套深度条件
    std::regex depthRegex(R"(depth\s*([<>=!]+)\s*(\d+))");
    std::smatch match;
    if (std::regex_search(expandedCondition, match, depthRegex)) {
        String op = match[1].str();
        size_t value = std::stoul(match[2].str());
        
        if (op == "<") return context.nestingDepth < value;
        if (op == "<=") return context.nestingDepth <= value;
        if (op == ">") return context.nestingDepth > value;
        if (op == ">=") return context.nestingDepth >= value;
        if (op == "==" || op == "=") return context.nestingDepth == value;
        if (op == "!=") return context.nestingDepth != value;
    }
    
    // 检查命名空间条件
    if (expandedCondition.find("namespace") != String::npos) {
        std::regex nsRegex(R"(namespace\s*==\s*['"]([^'"]+)['"])");
        if (std::regex_search(expandedCondition, match, nsRegex)) {
            return context.currentNamespace == match[1].str();
        }
    }
    
    return true;
}

String ConstraintRule::expandVariables(const String& text, const ConstraintContext& context) const {
    String result = text;
    
    // 替换上下文变量
    for (const auto& [key, value] : context.variables) {
        String placeholder = "${" + key + "}";
        size_t pos = 0;
        while ((pos = result.find(placeholder, pos)) != String::npos) {
            result.replace(pos, placeholder.length(), value);
            pos += value.length();
        }
    }
    
    // 替换内置变量
    result = std::regex_replace(result, std::regex(R"(\$\{namespace\})"), context.currentNamespace);
    result = std::regex_replace(result, std::regex(R"(\$\{depth\})"), std::to_string(context.nestingDepth));
    result = std::regex_replace(result, std::regex(R"(\$\{template\})"), context.currentTemplate);
    result = std::regex_replace(result, std::regex(R"(\$\{custom\})"), context.currentCustom);
    
    return result;
}

// ========== TypeConstraintRule 实现 ==========

TypeConstraintRule::TypeConstraintRule(const String& name) : ConstraintRule(name) {
    type = ConstraintType::TYPE_BASED;
}

bool TypeConstraintRule::isTypeAllowed(NodeType nodeType) const {
    if (allowedTypes.empty()) {
        return true; // 无限制时允许所有类型
    }
    
    String typeName = nodeTypeToString(nodeType);
    return std::find(allowedTypes.begin(), allowedTypes.end(), typeName) != allowedTypes.end();
}

bool TypeConstraintRule::isTypeDenied(NodeType nodeType) const {
    String typeName = nodeTypeToString(nodeType);
    return std::find(deniedTypes.begin(), deniedTypes.end(), typeName) != deniedTypes.end();
}

NodeType TypeConstraintRule::getTransformedType(NodeType originalType) const {
    String originalTypeName = nodeTypeToString(originalType);
    auto it = typeTransforms.find(originalTypeName);
    
    if (it != typeTransforms.end()) {
        return parseNodeType(it->second);
    }
    
    return originalType;
}

bool TypeConstraintRule::matchesType(NodeType nodeType, const ConstraintContext& context) const {
    if (!isEnabled) {
        return false;
    }
    
    // 检查命名空间约束
    if (!matchesNamespace(context.currentNamespace, context)) {
        return false;
    }
    
    // 检查是否为目标类型
    bool isTargetType = false;
    if (targets.empty()) {
        isTargetType = true; // 无目标限制时匹配所有类型
    } else {
        String typeName = nodeTypeToString(nodeType);
        isTargetType = std::find(targets.begin(), targets.end(), typeName) != targets.end();
    }
    
    return isTargetType && !isException(nodeTypeToString(nodeType));
}

ConstraintAction TypeConstraintRule::getAction(const String& target, const ConstraintContext& context) const {
    // 根据类型约束返回相应的动作
    NodeType nodeType = parseNodeType(target);
    
    if (isTypeDenied(nodeType)) {
        return ConstraintAction::DENY;
    }
    
    if (!isTypeAllowed(nodeType)) {
        return action;
    }
    
    // 如果有类型转换，返回转换动作
    if (typeTransforms.find(target) != typeTransforms.end()) {
        return ConstraintAction::TRANSFORM;
    }
    
    return ConstraintAction::ALLOW;
}

String TypeConstraintRule::getTransformationResult(const String& originalContent, const ConstraintContext& context) const {
    // 进行类型转换
    auto it = typeTransforms.find(originalContent);
    if (it != typeTransforms.end()) {
        return it->second;
    }
    
    return ConstraintRule::getTransformationResult(originalContent, context);
}

void TypeConstraintRule::addAllowedType(const String& typeName) {
    if (std::find(allowedTypes.begin(), allowedTypes.end(), typeName) == allowedTypes.end()) {
        allowedTypes.push_back(typeName);
    }
}

void TypeConstraintRule::addDeniedType(const String& typeName) {
    if (std::find(deniedTypes.begin(), deniedTypes.end(), typeName) == deniedTypes.end()) {
        deniedTypes.push_back(typeName);
    }
}

void TypeConstraintRule::addTypeTransform(const String& fromType, const String& toType) {
    typeTransforms[fromType] = toType;
}

void TypeConstraintRule::setAllowedTypes(const StringVector& types) {
    allowedTypes = types;
}

void TypeConstraintRule::setDeniedTypes(const StringVector& types) {
    deniedTypes = types;
}

NodeType TypeConstraintRule::parseNodeType(const String& typeName) const {
    static const std::unordered_map<String, NodeType> typeMap = {
        {"ELEMENT", NodeType::ELEMENT},
        {"TEMPLATE", NodeType::TEMPLATE},
        {"CUSTOM", NodeType::CUSTOM},
        {"ORIGIN", NodeType::ORIGIN},
        {"IMPORT", NodeType::IMPORT},
        {"NAMESPACE", NodeType::NAMESPACE},
        {"CONFIGURATION", NodeType::CONFIGURATION},
        {"CHTLJS", NodeType::CHTLJS},
        {"COMMENT", NodeType::COMMENT},
        {"TEXT", NodeType::TEXT}
    };
    
    auto it = typeMap.find(typeName);
    return it != typeMap.end() ? it->second : NodeType::UNKNOWN;
}

String TypeConstraintRule::nodeTypeToString(NodeType type) const {
    switch (type) {
        case NodeType::ELEMENT: return "ELEMENT";
        case NodeType::TEMPLATE: return "TEMPLATE";
        case NodeType::CUSTOM: return "CUSTOM";
        case NodeType::ORIGIN: return "ORIGIN";
        case NodeType::IMPORT: return "IMPORT";
        case NodeType::NAMESPACE: return "NAMESPACE";
        case NodeType::CONFIGURATION: return "CONFIGURATION";
        case NodeType::CHTLJS: return "CHTLJS";
        case NodeType::COMMENT: return "COMMENT";
        case NodeType::TEXT: return "TEXT";
        default: return "UNKNOWN";
    }
}

// ========== GlobalConstraintRule 实现 ==========

GlobalConstraintRule::GlobalConstraintRule(const String& name) 
    : ConstraintRule(name), maxTotalElements(1000), maxNestingDepth(50), 
      maxTemplateCount(100), maxCustomCount(100), maxOriginCount(50), maxImportCount(50) {
    type = ConstraintType::GLOBAL;
    isGlobal = true;
}

bool GlobalConstraintRule::checkGlobalLimits(const ConstraintContext& context) const {
    // 检查嵌套深度
    if (!checkNestingDepth(context.nestingDepth)) {
        return false;
    }
    
    // 检查其他全局限制
    for (const auto& [limitName, limitValue] : globalLimits) {
        if (!isWithinLimit(limitName, parseLimit(limitValue))) {
            return false;
        }
    }
    
    return true;
}

bool GlobalConstraintRule::checkElementCount(const String& elementType, size_t currentCount) const {
    if (elementType == "TEMPLATE" && currentCount >= maxTemplateCount) {
        return false;
    }
    if (elementType == "CUSTOM" && currentCount >= maxCustomCount) {
        return false;
    }
    if (elementType == "ORIGIN" && currentCount >= maxOriginCount) {
        return false;
    }
    if (elementType == "IMPORT" && currentCount >= maxImportCount) {
        return false;
    }
    
    return true;
}

bool GlobalConstraintRule::checkNestingDepth(size_t currentDepth) const {
    return currentDepth <= maxNestingDepth;
}

bool GlobalConstraintRule::checkRequiredElements(const StringVector& presentElements) const {
    for (const String& required : requiredElements) {
        if (std::find(presentElements.begin(), presentElements.end(), required) == presentElements.end()) {
            return false; // 缺少必需元素
        }
    }
    return true;
}

bool GlobalConstraintRule::checkForbiddenElements(const StringVector& presentElements) const {
    for (const String& forbidden : forbiddenElements) {
        if (std::find(presentElements.begin(), presentElements.end(), forbidden) != presentElements.end()) {
            return false; // 包含禁止元素
        }
    }
    return true;
}

bool GlobalConstraintRule::matches(const String& target, const ConstraintContext& context) const {
    if (!ConstraintRule::matches(target, context)) {
        return false;
    }
    
    return checkGlobalLimits(context);
}

ConstraintAction GlobalConstraintRule::getAction(const String& target, const ConstraintContext& context) const {
    if (!checkGlobalLimits(context)) {
        return action;
    }
    
    return ConstraintAction::ALLOW;
}

void GlobalConstraintRule::setElementLimit(const String& elementType, size_t limit) {
    if (elementType == "TEMPLATE") {
        maxTemplateCount = limit;
    } else if (elementType == "CUSTOM") {
        maxCustomCount = limit;
    } else if (elementType == "ORIGIN") {
        maxOriginCount = limit;
    } else if (elementType == "IMPORT") {
        maxImportCount = limit;
    }
}

void GlobalConstraintRule::addRequiredElement(const String& elementName) {
    if (std::find(requiredElements.begin(), requiredElements.end(), elementName) == requiredElements.end()) {
        requiredElements.push_back(elementName);
    }
}

void GlobalConstraintRule::addForbiddenElement(const String& elementName) {
    if (std::find(forbiddenElements.begin(), forbiddenElements.end(), elementName) == forbiddenElements.end()) {
        forbiddenElements.push_back(elementName);
    }
}

void GlobalConstraintRule::setGlobalLimit(const String& limitName, const String& limitValue) {
    globalLimits[limitName] = limitValue;
}

bool GlobalConstraintRule::isWithinLimit(const String& limitName, size_t currentValue) const {
    auto it = globalLimits.find(limitName);
    if (it != globalLimits.end()) {
        size_t limitValue = parseLimit(it->second);
        return currentValue <= limitValue;
    }
    return true;
}

size_t GlobalConstraintRule::parseLimit(const String& limitValue) const {
    try {
        return std::stoul(limitValue);
    } catch (const std::exception&) {
        return SIZE_MAX; // 解析失败时返回最大值
    }
}

// ========== ConditionalConstraintRule 实现 ==========

ConditionalConstraintRule::ConditionalConstraintRule(const String& name) : ConstraintRule(name) {
    type = ConstraintType::CONDITIONAL;
}

bool ConditionalConstraintRule::evaluateIfCondition(const ConstraintContext& context) const {
    return evaluateComplexCondition(ifCondition, context);
}

ConstraintAction ConditionalConstraintRule::getThenAction() const {
    return parseAction(thenAction);
}

ConstraintAction ConditionalConstraintRule::getElseAction() const {
    return parseAction(elseAction);
}

bool ConditionalConstraintRule::evaluateCondition(const ConstraintContext& context) const {
    if (!ifCondition.empty()) {
        return evaluateIfCondition(context);
    }
    
    return ConstraintRule::evaluateCondition(context);
}

ConstraintAction ConditionalConstraintRule::getAction(const String& target, const ConstraintContext& context) const {
    if (evaluateIfCondition(context)) {
        return getThenAction();
    } else {
        ConstraintAction elseAct = getElseAction();
        return elseAct != ConstraintAction::ALLOW ? elseAct : ConstraintRule::getAction(target, context);
    }
}

void ConditionalConstraintRule::setCondition(const String& condition, const String& thenAct, const String& elseAct) {
    ifCondition = condition;
    thenAction = thenAct;
    elseAction = elseAct;
}

void ConditionalConstraintRule::addConditionVariable(const String& variableName) {
    if (std::find(conditionVariables.begin(), conditionVariables.end(), variableName) == conditionVariables.end()) {
        conditionVariables.push_back(variableName);
    }
}

ConstraintAction ConditionalConstraintRule::parseAction(const String& actionString) const {
    static const std::unordered_map<String, ConstraintAction> actionMap = {
        {"DENY", ConstraintAction::DENY},
        {"ALLOW", ConstraintAction::ALLOW},
        {"RESTRICT", ConstraintAction::RESTRICT},
        {"WARN", ConstraintAction::WARN},
        {"REPLACE", ConstraintAction::REPLACE},
        {"TRANSFORM", ConstraintAction::TRANSFORM},
        {"DELEGATE", ConstraintAction::DELEGATE},
        {"MONITOR", ConstraintAction::MONITOR}
    };
    
    auto it = actionMap.find(actionString);
    return it != actionMap.end() ? it->second : ConstraintAction::ALLOW;
}

bool ConditionalConstraintRule::evaluateComplexCondition(const String& condition, const ConstraintContext& context) const {
    // 扩展变量
    String expandedCondition = expandVariables(condition, context);
    
    // 支持逻辑操作符
    if (expandedCondition.find(" AND ") != String::npos) {
        size_t pos = expandedCondition.find(" AND ");
        String left = expandedCondition.substr(0, pos);
        String right = expandedCondition.substr(pos + 5);
        return evaluateSimpleCondition(left, context) && evaluateSimpleCondition(right, context);
    }
    
    if (expandedCondition.find(" OR ") != String::npos) {
        size_t pos = expandedCondition.find(" OR ");
        String left = expandedCondition.substr(0, pos);
        String right = expandedCondition.substr(pos + 4);
        return evaluateSimpleCondition(left, context) || evaluateSimpleCondition(right, context);
    }
    
    if (expandedCondition.find("NOT ") == 0) {
        String negatedCondition = expandedCondition.substr(4);
        return !evaluateSimpleCondition(negatedCondition, context);
    }
    
    return evaluateSimpleCondition(expandedCondition, context);
}

// ========== AdvancedConstraintSystem 实现 ==========

AdvancedConstraintSystem::AdvancedConstraintSystem() : globalMode(false), strictMode(false) {
    initializeBuiltinRules();
    initializeDefaultConfiguration();
}

void AdvancedConstraintSystem::addRule(std::shared_ptr<ConstraintRule> rule) {
    if (rule && !rule->ruleName.empty()) {
        rules[rule->ruleName] = rule;
        
        // 根据类型添加到相应的分类列表
        if (auto typeRule = std::dynamic_pointer_cast<TypeConstraintRule>(rule)) {
            typeRules.push_back(typeRule);
        } else if (auto globalRule = std::dynamic_pointer_cast<GlobalConstraintRule>(rule)) {
            globalRules.push_back(globalRule);
        } else if (auto conditionalRule = std::dynamic_pointer_cast<ConditionalConstraintRule>(rule)) {
            conditionalRules.push_back(conditionalRule);
        }
    }
}

void AdvancedConstraintSystem::removeRule(const String& ruleName) {
    auto it = rules.find(ruleName);
    if (it != rules.end()) {
        auto rule = it->second;
        rules.erase(it);
        
        // 从分类列表中移除
        typeRules.erase(
            std::remove_if(typeRules.begin(), typeRules.end(),
                          [&](const std::weak_ptr<TypeConstraintRule>& weakPtr) {
                              auto ptr = weakPtr.lock();
                              return !ptr || ptr->ruleName == ruleName;
                          }),
            typeRules.end()
        );
        
        globalRules.erase(
            std::remove_if(globalRules.begin(), globalRules.end(),
                          [&](const std::weak_ptr<GlobalConstraintRule>& weakPtr) {
                              auto ptr = weakPtr.lock();
                              return !ptr || ptr->ruleName == ruleName;
                          }),
            globalRules.end()
        );
        
        conditionalRules.erase(
            std::remove_if(conditionalRules.begin(), conditionalRules.end(),
                          [&](const std::weak_ptr<ConditionalConstraintRule>& weakPtr) {
                              auto ptr = weakPtr.lock();
                              return !ptr || ptr->ruleName == ruleName;
                          }),
            conditionalRules.end()
        );
    }
}

std::shared_ptr<ConstraintRule> AdvancedConstraintSystem::getRule(const String& ruleName) const {
    auto it = rules.find(ruleName);
    return it != rules.end() ? it->second : nullptr;
}

bool AdvancedConstraintSystem::hasRule(const String& ruleName) const {
    return rules.find(ruleName) != rules.end();
}

StringVector AdvancedConstraintSystem::getAllRuleNames() const {
    StringVector names;
    for (const auto& [name, rule] : rules) {
        names.push_back(name);
    }
    return names;
}

void AdvancedConstraintSystem::addTypeRule(std::shared_ptr<TypeConstraintRule> rule) {
    addRule(rule);
}

void AdvancedConstraintSystem::addGlobalRule(std::shared_ptr<GlobalConstraintRule> rule) {
    addRule(rule);
}

void AdvancedConstraintSystem::addConditionalRule(std::shared_ptr<ConditionalConstraintRule> rule) {
    addRule(rule);
}

StringVector AdvancedConstraintSystem::getTypeRules() const {
    StringVector names;
    for (const auto& rule : typeRules) {
        if (rule) {
            names.push_back(rule->ruleName);
        }
    }
    return names;
}

StringVector AdvancedConstraintSystem::getGlobalRules() const {
    StringVector names;
    for (const auto& rule : globalRules) {
        if (rule) {
            names.push_back(rule->ruleName);
        }
    }
    return names;
}

StringVector AdvancedConstraintSystem::getConditionalRules() const {
    StringVector names;
    for (const auto& rule : conditionalRules) {
        if (rule) {
            names.push_back(rule->ruleName);
        }
    }
    return names;
}

bool AdvancedConstraintSystem::checkConstraints(const String& target, const ConstraintContext& context) const {
    auto applicableRules = getApplicableRules(target, context);
    
    for (const auto& rule : applicableRules) {
        if (rule->matches(target, context)) {
            ConstraintAction action = rule->getAction(target, context);
            
            if (action == ConstraintAction::DENY) {
                const_cast<AdvancedConstraintSystem*>(this)->triggerConstraintViolation(target, context, action);
                return false;
            } else if (action == ConstraintAction::WARN) {
                const_cast<AdvancedConstraintSystem*>(this)->triggerConstraintWarning(target, context, action);
            }
        }
    }
    
    return true;
}

bool AdvancedConstraintSystem::checkTypeConstraints(NodeType nodeType, const ConstraintContext& context) const {
    for (const auto& rule : typeRules) {
        if (rule && rule->matchesType(nodeType, context)) {
            ConstraintAction action = rule->getAction(rule->nodeTypeToString(nodeType), context);
            
            if (action == ConstraintAction::DENY) {
                const_cast<AdvancedConstraintSystem*>(this)->triggerConstraintViolation(
                    rule->nodeTypeToString(nodeType), context, action);
                return false;
            }
        }
    }
    
    return true;
}

bool AdvancedConstraintSystem::checkGlobalConstraints(const ConstraintContext& context) const {
    for (const auto& rule : globalRules) {
        if (rule && !rule->checkGlobalLimits(context)) {
            const_cast<AdvancedConstraintSystem*>(this)->triggerConstraintViolation(
                "global", context, rule->action);
            return false;
        }
    }
    
    return true;
}

std::vector<ConstraintAction> AdvancedConstraintSystem::getApplicableActions(const String& target, const ConstraintContext& context) const {
    std::vector<ConstraintAction> actions;
    auto applicableRules = getApplicableRules(target, context);
    
    for (const auto& rule : applicableRules) {
        if (rule->matches(target, context)) {
            actions.push_back(rule->getAction(target, context));
        }
    }
    
    return actions;
}

String AdvancedConstraintSystem::applyConstraints(const String& content, const ConstraintContext& context) const {
    String result = content;
    auto applicableRules = getApplicableRules(content, context);
    
    for (const auto& rule : applicableRules) {
        if (rule->matches(content, context)) {
            ConstraintAction action = rule->getAction(content, context);
            
            switch (action) {
                case ConstraintAction::REPLACE:
                    result = rule->getReplacementContent(content, context);
                    break;
                case ConstraintAction::TRANSFORM:
                    result = rule->getTransformationResult(result, context);
                    break;
                case ConstraintAction::DENY:
                    return ""; // 被拒绝的内容返回空字符串
                default:
                    break;
            }
            
            const_cast<AdvancedConstraintSystem*>(this)->triggerConstraintApplied(content, context, action);
        }
    }
    
    return result;
}

String AdvancedConstraintSystem::transformContent(const String& content, const ConstraintContext& context) const {
    String result = content;
    auto applicableRules = getApplicableRules(content, context);
    
    for (const auto& rule : applicableRules) {
        if (rule->matches(content, context)) {
            ConstraintAction action = rule->getAction(content, context);
            
            if (action == ConstraintAction::TRANSFORM) {
                result = rule->getTransformationResult(result, context);
            }
        }
    }
    
    return result;
}

StringVector AdvancedConstraintSystem::getConstraintWarnings(const String& target, const ConstraintContext& context) const {
    StringVector warnings;
    auto applicableRules = getApplicableRules(target, context);
    
    for (const auto& rule : applicableRules) {
        if (rule->matches(target, context)) {
            ConstraintAction action = rule->getAction(target, context);
            
            if (action == ConstraintAction::WARN) {
                warnings.push_back("警告: " + rule->description);
            }
        }
    }
    
    return warnings;
}

bool AdvancedConstraintSystem::validateAllRules() const {
    for (const auto& [name, rule] : rules) {
        if (!rule->validateRule()) {
            return false;
        }
    }
    return true;
}

StringVector AdvancedConstraintSystem::getValidationErrors() const {
    StringVector allErrors;
    
    for (const auto& [name, rule] : rules) {
        auto errors = rule->getValidationErrors();
        for (const String& error : errors) {
            allErrors.push_back("[" + name + "] " + error);
        }
    }
    
    return allErrors;
}

StringVector AdvancedConstraintSystem::checkRuleConflicts() const {
    StringVector conflicts;
    
    for (auto it1 = rules.begin(); it1 != rules.end(); ++it1) {
        for (auto it2 = std::next(it1); it2 != rules.end(); ++it2) {
            if (hasRuleConflict(it1->second, it2->second)) {
                conflicts.push_back(describeRuleConflict(it1->second, it2->second));
            }
        }
    }
    
    return conflicts;
}

void AdvancedConstraintSystem::optimizeRules() {
    for (auto& [name, rule] : rules) {
        rule->optimizeRule();
    }
}

ConstraintContext AdvancedConstraintSystem::createContext(const String& namespaceName) const {
    ConstraintContext context;
    context.currentNamespace = namespaceName;
    return context;
}

void AdvancedConstraintSystem::updateContext(ConstraintContext& context, const String& key, const String& value) const {
    context.variables[key] = value;
}

void AdvancedConstraintSystem::pushContextScope(ConstraintContext& context, const String& scopeName) const {
    context.ancestorElements.push_back(context.parentElement);
    context.parentElement = scopeName;
    context.nestingDepth++;
}

void AdvancedConstraintSystem::popContextScope(ConstraintContext& context) const {
    if (!context.ancestorElements.empty()) {
        context.parentElement = context.ancestorElements.back();
        context.ancestorElements.pop_back();
        context.nestingDepth = std::max(context.nestingDepth - 1, size_t(0));
    }
}

std::unordered_map<String, size_t> AdvancedConstraintSystem::getConstraintStatistics() const {
    std::unordered_map<String, size_t> stats;
    
    stats["totalRules"] = rules.size();
    stats["typeRules"] = typeRules.size();
    stats["globalRules"] = globalRules.size();
    stats["conditionalRules"] = conditionalRules.size();
    
    size_t enabledCount = 0;
    for (const auto& [name, rule] : rules) {
        if (rule->isEnabled) {
            enabledCount++;
        }
    }
    stats["enabledRules"] = enabledCount;
    
    return stats;
}

StringVector AdvancedConstraintSystem::getMostUsedRules() const {
    StringVector mostUsed;
    
    // 按使用次数排序
    std::vector<std::pair<String, size_t>> usage;
    for (const auto& [name, count] : ruleUsageStats) {
        usage.push_back({name, count});
    }
    
    std::sort(usage.begin(), usage.end(), 
             [](const auto& a, const auto& b) { return a.second > b.second; });
    
    for (const auto& [name, count] : usage) {
        mostUsed.push_back(name);
        if (mostUsed.size() >= 10) break; // 返回前10个
    }
    
    return mostUsed;
}

StringVector AdvancedConstraintSystem::getUnusedRules() const {
    StringVector unused;
    
    for (const auto& [name, rule] : rules) {
        if (ruleUsageStats.find(name) == ruleUsageStats.end() || ruleUsageStats.at(name) == 0) {
            unused.push_back(name);
        }
    }
    
    return unused;
}

double AdvancedConstraintSystem::getConstraintCoverage() const {
    if (rules.empty()) {
        return 0.0;
    }
    
    size_t usedRules = 0;
    for (const auto& [name, rule] : rules) {
        if (ruleUsageStats.find(name) != ruleUsageStats.end() && ruleUsageStats.at(name) > 0) {
            usedRules++;
        }
    }
    
    return static_cast<double>(usedRules) / rules.size() * 100.0;
}

void AdvancedConstraintSystem::enableRule(const String& ruleName, bool enable) {
    auto rule = getRule(ruleName);
    if (rule) {
        rule->isEnabled = enable;
    }
}

void AdvancedConstraintSystem::setRulePriority(const String& ruleName, double priority) {
    auto rule = getRule(ruleName);
    if (rule) {
        rule->priority = priority;
    }
}

void AdvancedConstraintSystem::setGlobalMode(bool global) {
    globalMode = global;
}

void AdvancedConstraintSystem::setStrictMode(bool strict) {
    strictMode = strict;
}

void AdvancedConstraintSystem::loadStrictRuleSet() {
    // 加载严格规则集
    auto strictRule = std::make_shared<GlobalConstraintRule>("strict_global");
    strictRule->description = "严格的全局约束";
    strictRule->maxNestingDepth = 20;
    strictRule->maxTemplateCount = 50;
    strictRule->maxCustomCount = 50;
    strictRule->action = ConstraintAction::DENY;
    addGlobalRule(strictRule);
    
    auto typeRule = std::make_shared<TypeConstraintRule>("strict_types");
    typeRule->description = "严格的类型约束";
    typeRule->addDeniedType("ORIGIN");
    typeRule->action = ConstraintAction::DENY;
    addTypeRule(typeRule);
}

void AdvancedConstraintSystem::loadPermissiveRuleSet() {
    // 加载宽松规则集
    auto permissiveRule = std::make_shared<GlobalConstraintRule>("permissive_global");
    permissiveRule->description = "宽松的全局约束";
    permissiveRule->maxNestingDepth = 100;
    permissiveRule->maxTemplateCount = 500;
    permissiveRule->maxCustomCount = 500;
    permissiveRule->action = ConstraintAction::WARN;
    addGlobalRule(permissiveRule);
}

void AdvancedConstraintSystem::loadSecurityRuleSet() {
    // 加载安全规则集
    auto securityRule = std::make_shared<ConstraintRule>("security_basic");
    securityRule->description = "基础安全约束";
    securityRule->targets = {"script", "origin", "javascript"};
    securityRule->action = ConstraintAction::WARN;
    securityRule->severity = ConstraintSeverity::HIGH;
    addRule(securityRule);
}

void AdvancedConstraintSystem::loadPerformanceRuleSet() {
    // 加载性能规则集
    auto perfRule = std::make_shared<GlobalConstraintRule>("performance_limits");
    perfRule->description = "性能优化约束";
    perfRule->maxNestingDepth = 30;
    perfRule->maxTemplateCount = 100;
    perfRule->action = ConstraintAction::WARN;
    addGlobalRule(perfRule);
}

void AdvancedConstraintSystem::loadCompatibilityRuleSet() {
    // 加载兼容性规则集
    auto compatRule = std::make_shared<TypeConstraintRule>("compatibility_types");
    compatRule->description = "兼容性约束";
    compatRule->addTypeTransform("LEGACY_ELEMENT", "ELEMENT");
    compatRule->action = ConstraintAction::TRANSFORM;
    addTypeRule(compatRule);
}

void AdvancedConstraintSystem::dumpRules() const {
    std::cout << "=== 约束系统规则列表 ===\n";
    std::cout << "总规则数: " << rules.size() << "\n\n";
    
    for (const auto& [name, rule] : rules) {
        std::cout << "规则: " << name << "\n";
        std::cout << "描述: " << rule->description << "\n";
        std::cout << "类型: " << static_cast<int>(rule->type) << "\n";
        std::cout << "动作: " << static_cast<int>(rule->action) << "\n";
        std::cout << "启用: " << (rule->isEnabled ? "是" : "否") << "\n";
        std::cout << "优先级: " << rule->priority << "\n";
        std::cout << "使用次数: " << ruleUsageStats.count(name) ? ruleUsageStats.at(name) : 0 << "\n";
        std::cout << "---\n";
    }
}

void AdvancedConstraintSystem::analyzeRulePerformance() const {
    std::cout << "\n=== 约束系统性能分析 ===\n";
    
    auto stats = getConstraintStatistics();
    std::cout << "总规则数: " << stats["totalRules"] << "\n";
    std::cout << "启用规则数: " << stats["enabledRules"] << "\n";
    std::cout << "规则覆盖率: " << getConstraintCoverage() << "%\n";
    
    StringVector mostUsed = getMostUsedRules();
    std::cout << "\n最常用规则:\n";
    for (size_t i = 0; i < std::min(size_t(5), mostUsed.size()); ++i) {
        std::cout << "  " << (i + 1) << ". " << mostUsed[i] 
                  << " (使用 " << ruleUsageStats.at(mostUsed[i]) << " 次)\n";
    }
    
    StringVector unused = getUnusedRules();
    std::cout << "\n未使用规则数: " << unused.size() << "\n";
}

void AdvancedConstraintSystem::generateConstraintReport() const {
    std::cout << "\n=== 约束系统报告 ===\n";
    
    dumpRules();
    analyzeRulePerformance();
    
    // 验证报告
    StringVector errors = getValidationErrors();
    if (errors.empty()) {
        std::cout << "\n✅ 所有规则验证通过\n";
    } else {
        std::cout << "\n❌ 发现 " << errors.size() << " 个验证错误:\n";
        for (const String& error : errors) {
            std::cout << "  - " << error << "\n";
        }
    }
    
    // 冲突报告
    StringVector conflicts = checkRuleConflicts();
    if (conflicts.empty()) {
        std::cout << "\n✅ 无规则冲突\n";
    } else {
        std::cout << "\n⚠️  发现 " << conflicts.size() << " 个规则冲突:\n";
        for (const String& conflict : conflicts) {
            std::cout << "  - " << conflict << "\n";
        }
    }
}

void AdvancedConstraintSystem::validateSystemIntegrity() const {
    std::cout << "\n=== 约束系统完整性检查 ===\n";
    
    if (validateAllRules()) {
        std::cout << "✅ 系统完整性检查通过\n";
    } else {
        std::cout << "❌ 系统完整性检查失败\n";
    }
    
    generateConstraintReport();
}

void AdvancedConstraintSystem::onConstraintViolation(ConstraintEventCallback callback) {
    violationCallbacks.push_back(callback);
}

void AdvancedConstraintSystem::onConstraintWarning(ConstraintEventCallback callback) {
    warningCallbacks.push_back(callback);
}

void AdvancedConstraintSystem::onConstraintApplied(ConstraintEventCallback callback) {
    appliedCallbacks.push_back(callback);
}

// 内部辅助方法实现
std::vector<std::shared_ptr<ConstraintRule>> AdvancedConstraintSystem::getApplicableRules(const String& target, const ConstraintContext& context) const {
    std::vector<std::shared_ptr<ConstraintRule>> applicable;
    
    for (const auto& [name, rule] : rules) {
        if (rule->isEnabled && (rule->isGlobal || globalMode)) {
            if (rule->matches(target, context)) {
                applicable.push_back(rule);
                ruleUsageStats[name]++;
            }
        }
    }
    
    return sortRulesByPriority(applicable);
}

std::vector<std::shared_ptr<ConstraintRule>> AdvancedConstraintSystem::sortRulesByPriority(const std::vector<std::shared_ptr<ConstraintRule>>& rules) const {
    std::vector<std::shared_ptr<ConstraintRule>> sorted = rules;
    
    std::sort(sorted.begin(), sorted.end(), 
             [](const std::shared_ptr<ConstraintRule>& a, const std::shared_ptr<ConstraintRule>& b) {
                 return a->priority > b->priority;
             });
    
    return sorted;
}

bool AdvancedConstraintSystem::hasRuleConflict(std::shared_ptr<ConstraintRule> rule1, std::shared_ptr<ConstraintRule> rule2) const {
    // 检查规则是否有冲突的目标和相反的动作
    for (const String& target1 : rule1->targets) {
        for (const String& target2 : rule2->targets) {
            if (ConstraintUtils::matchesPattern(target1, target2) || ConstraintUtils::matchesPattern(target2, target1)) {
                if ((rule1->action == ConstraintAction::DENY && rule2->action == ConstraintAction::ALLOW) ||
                    (rule1->action == ConstraintAction::ALLOW && rule2->action == ConstraintAction::DENY)) {
                    return true;
                }
            }
        }
    }
    
    return false;
}

String AdvancedConstraintSystem::describeRuleConflict(std::shared_ptr<ConstraintRule> rule1, std::shared_ptr<ConstraintRule> rule2) const {
    return "规则 '" + rule1->ruleName + "' 与 '" + rule2->ruleName + "' 存在冲突";
}

void AdvancedConstraintSystem::triggerConstraintViolation(const String& target, const ConstraintContext& context, ConstraintAction action) const {
    for (const auto& callback : violationCallbacks) {
        callback(target, context, action);
    }
}

void AdvancedConstraintSystem::triggerConstraintWarning(const String& target, const ConstraintContext& context, ConstraintAction action) const {
    for (const auto& callback : warningCallbacks) {
        callback(target, context, action);
    }
}

void AdvancedConstraintSystem::triggerConstraintApplied(const String& target, const ConstraintContext& context, ConstraintAction action) const {
    for (const auto& callback : appliedCallbacks) {
        callback(target, context, action);
    }
}

void AdvancedConstraintSystem::initializeBuiltinRules() {
    // 初始化内置的基础规则
    loadPermissiveRuleSet();
}

void AdvancedConstraintSystem::initializeDefaultConfiguration() {
    globalMode = false;
    strictMode = false;
}

// ========== ConstraintBuilder 实现 ==========

ConstraintBuilder::ConstraintBuilder(const String& ruleName) 
    : ruleType(ConstraintType::PRECISE), maxElementCount(SIZE_MAX), maxNestingDepth(SIZE_MAX) {
    baseRule = std::make_shared<ConstraintRule>(ruleName);
}

ConstraintBuilder& ConstraintBuilder::description(const String& desc) {
    baseRule->description = desc;
    return *this;
}

ConstraintBuilder& ConstraintBuilder::type(ConstraintType constraintType) {
    ruleType = constraintType;
    baseRule->type = constraintType;
    return *this;
}

ConstraintBuilder& ConstraintBuilder::action(ConstraintAction constraintAction) {
    baseRule->action = constraintAction;
    return *this;
}

ConstraintBuilder& ConstraintBuilder::severity(ConstraintSeverity constraintSeverity) {
    baseRule->severity = constraintSeverity;
    return *this;
}

ConstraintBuilder& ConstraintBuilder::priority(double rulePriority) {
    baseRule->priority = rulePriority;
    return *this;
}

ConstraintBuilder& ConstraintBuilder::target(const String& targetName) {
    baseRule->targets.push_back(targetName);
    return *this;
}

ConstraintBuilder& ConstraintBuilder::targets(const StringVector& targetNames) {
    baseRule->targets.insert(baseRule->targets.end(), targetNames.begin(), targetNames.end());
    return *this;
}

ConstraintBuilder& ConstraintBuilder::except(const String& exceptionName) {
    baseRule->exceptions.push_back(exceptionName);
    return *this;
}

ConstraintBuilder& ConstraintBuilder::exceptions(const StringVector& exceptionNames) {
    baseRule->exceptions.insert(baseRule->exceptions.end(), exceptionNames.begin(), exceptionNames.end());
    return *this;
}

ConstraintBuilder& ConstraintBuilder::condition(const String& conditionExpression) {
    baseRule->condition = conditionExpression;
    return *this;
}

ConstraintBuilder& ConstraintBuilder::parameter(const String& key, const String& value) {
    baseRule->parameters[key] = value;
    return *this;
}

ConstraintBuilder& ConstraintBuilder::parameters(const StringUnorderedMap& params) {
    baseRule->parameters.insert(params.begin(), params.end());
    return *this;
}

ConstraintBuilder& ConstraintBuilder::global(bool isGlobal) {
    baseRule->isGlobal = isGlobal;
    return *this;
}

ConstraintBuilder& ConstraintBuilder::namespace_(const String& namespaceName) {
    baseRule->namespace_ = namespaceName;
    return *this;
}

ConstraintBuilder& ConstraintBuilder::enabled(bool isEnabled) {
    baseRule->isEnabled = isEnabled;
    return *this;
}

ConstraintBuilder& ConstraintBuilder::allowType(const String& typeName) {
    allowedTypes.push_back(typeName);
    return *this;
}

ConstraintBuilder& ConstraintBuilder::denyType(const String& typeName) {
    deniedTypes.push_back(typeName);
    return *this;
}

ConstraintBuilder& ConstraintBuilder::transformType(const String& fromType, const String& toType) {
    typeTransforms[fromType] = toType;
    return *this;
}

ConstraintBuilder& ConstraintBuilder::maxElements(size_t maxCount) {
    maxElementCount = maxCount;
    return *this;
}

ConstraintBuilder& ConstraintBuilder::maxDepth(size_t maxNestingDepth) {
    this->maxNestingDepth = maxNestingDepth;
    return *this;
}

ConstraintBuilder& ConstraintBuilder::requireElement(const String& elementName) {
    requiredElements.push_back(elementName);
    return *this;
}

ConstraintBuilder& ConstraintBuilder::forbidElement(const String& elementName) {
    forbiddenElements.push_back(elementName);
    return *this;
}

ConstraintBuilder& ConstraintBuilder::ifCondition(const String& condition, const String& thenAction, const String& elseAction) {
    ifCond = condition;
    thenAct = thenAction;
    elseAct = elseAction;
    return *this;
}

std::shared_ptr<ConstraintRule> ConstraintBuilder::build() {
    if (validateBuild()) {
        return baseRule;
    }
    return nullptr;
}

std::shared_ptr<TypeConstraintRule> ConstraintBuilder::buildTypeRule() {
    if (!validateBuild()) {
        return nullptr;
    }
    
    auto typeRule = std::make_shared<TypeConstraintRule>(baseRule->ruleName);
    
    // 复制基础属性
    typeRule->description = baseRule->description;
    typeRule->action = baseRule->action;
    typeRule->severity = baseRule->severity;
    typeRule->targets = baseRule->targets;
    typeRule->exceptions = baseRule->exceptions;
    typeRule->condition = baseRule->condition;
    typeRule->parameters = baseRule->parameters;
    typeRule->isEnabled = baseRule->isEnabled;
    typeRule->isGlobal = baseRule->isGlobal;
    typeRule->namespace_ = baseRule->namespace_;
    typeRule->priority = baseRule->priority;
    
    // 设置类型特定属性
    typeRule->setAllowedTypes(allowedTypes);
    typeRule->setDeniedTypes(deniedTypes);
    for (const auto& [from, to] : typeTransforms) {
        typeRule->addTypeTransform(from, to);
    }
    
    return typeRule;
}

std::shared_ptr<GlobalConstraintRule> ConstraintBuilder::buildGlobalRule() {
    if (!validateBuild()) {
        return nullptr;
    }
    
    auto globalRule = std::make_shared<GlobalConstraintRule>(baseRule->ruleName);
    
    // 复制基础属性
    globalRule->description = baseRule->description;
    globalRule->action = baseRule->action;
    globalRule->severity = baseRule->severity;
    globalRule->targets = baseRule->targets;
    globalRule->exceptions = baseRule->exceptions;
    globalRule->condition = baseRule->condition;
    globalRule->parameters = baseRule->parameters;
    globalRule->isEnabled = baseRule->isEnabled;
    globalRule->isGlobal = true; // 全局规则必须是全局的
    globalRule->namespace_ = baseRule->namespace_;
    globalRule->priority = baseRule->priority;
    
    // 设置全局特定属性
    if (maxElementCount != SIZE_MAX) {
        globalRule->maxTotalElements = maxElementCount;
    }
    if (maxNestingDepth != SIZE_MAX) {
        globalRule->maxNestingDepth = maxNestingDepth;
    }
    
    for (const String& required : requiredElements) {
        globalRule->addRequiredElement(required);
    }
    
    for (const String& forbidden : forbiddenElements) {
        globalRule->addForbiddenElement(forbidden);
    }
    
    return globalRule;
}

std::shared_ptr<ConditionalConstraintRule> ConstraintBuilder::buildConditionalRule() {
    if (!validateBuild()) {
        return nullptr;
    }
    
    auto conditionalRule = std::make_shared<ConditionalConstraintRule>(baseRule->ruleName);
    
    // 复制基础属性
    conditionalRule->description = baseRule->description;
    conditionalRule->action = baseRule->action;
    conditionalRule->severity = baseRule->severity;
    conditionalRule->targets = baseRule->targets;
    conditionalRule->exceptions = baseRule->exceptions;
    conditionalRule->condition = baseRule->condition;
    conditionalRule->parameters = baseRule->parameters;
    conditionalRule->isEnabled = baseRule->isEnabled;
    conditionalRule->isGlobal = baseRule->isGlobal;
    conditionalRule->namespace_ = baseRule->namespace_;
    conditionalRule->priority = baseRule->priority;
    
    // 设置条件特定属性
    if (!ifCond.empty()) {
        conditionalRule->setCondition(ifCond, thenAct, elseAct);
    }
    
    return conditionalRule;
}

bool ConstraintBuilder::validateBuild() const {
    return buildErrors.empty() && baseRule && !baseRule->ruleName.empty();
}

void ConstraintBuilder::addBuildError(const String& error) {
    buildErrors.push_back(error);
}

// ========== ConstraintUtils 实现 ==========

namespace ConstraintUtils {

bool parseConstraintExpression(const String& expression, StringVector& targets, StringVector& exceptions) {
    // 解析约束表达式，格式: target1, target2 except exception1, exception2
    size_t exceptPos = expression.find(" except ");
    
    String targetsPart = (exceptPos != String::npos) ? expression.substr(0, exceptPos) : expression;
    String exceptionsPart = (exceptPos != String::npos) ? expression.substr(exceptPos + 8) : "";
    
    // 解析目标列表
    std::istringstream targetsStream(targetsPart);
    String target;
    while (std::getline(targetsStream, target, ',')) {
        target.erase(0, target.find_first_not_of(" \t"));
        target.erase(target.find_last_not_of(" \t") + 1);
        if (!target.empty()) {
            targets.push_back(target);
        }
    }
    
    // 解析例外列表
    if (!exceptionsPart.empty()) {
        std::istringstream exceptionsStream(exceptionsPart);
        String exception;
        while (std::getline(exceptionsStream, exception, ',')) {
            exception.erase(0, exception.find_first_not_of(" \t"));
            exception.erase(exception.find_last_not_of(" \t") + 1);
            if (!exception.empty()) {
                exceptions.push_back(exception);
            }
        }
    }
    
    return !targets.empty();
}

String normalizeConstraintTarget(const String& target) {
    String normalized = target;
    
    // 去除前后空白
    normalized.erase(0, normalized.find_first_not_of(" \t"));
    normalized.erase(normalized.find_last_not_of(" \t") + 1);
    
    // 转换为小写
    std::transform(normalized.begin(), normalized.end(), normalized.begin(), ::tolower);
    
    return normalized;
}

bool isValidConstraintExpression(const String& expression) {
    StringVector targets, exceptions;
    return parseConstraintExpression(expression, targets, exceptions);
}

bool evaluateConditionExpression(const String& expression, const StringUnorderedMap& variables) {
    String expandedExpression = expandConditionVariables(expression, variables);
    
    // 简化的条件表达式评估
    if (expandedExpression == "true") return true;
    if (expandedExpression == "false") return false;
    
    // 数值比较
    std::regex numericRegex(R"((\d+)\s*([<>=!]+)\s*(\d+))");
    std::smatch match;
    if (std::regex_search(expandedExpression, match, numericRegex)) {
        int left = std::stoi(match[1].str());
        String op = match[2].str();
        int right = std::stoi(match[3].str());
        
        if (op == "<") return left < right;
        if (op == "<=") return left <= right;
        if (op == ">") return left > right;
        if (op == ">=") return left >= right;
        if (op == "==" || op == "=") return left == right;
        if (op == "!=") return left != right;
    }
    
    return true;
}

String expandConditionVariables(const String& expression, const StringUnorderedMap& variables) {
    String result = expression;
    
    for (const auto& [key, value] : variables) {
        String placeholder = "${" + key + "}";
        size_t pos = 0;
        while ((pos = result.find(placeholder, pos)) != String::npos) {
            result.replace(pos, placeholder.length(), value);
            pos += value.length();
        }
    }
    
    return result;
}

StringVector extractConditionVariables(const String& expression) {
    StringVector variables;
    std::regex varRegex(R"(\$\{([^}]+)\})");
    std::sregex_iterator iter(expression.begin(), expression.end(), varRegex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        variables.push_back(iter->str(1));
    }
    
    return variables;
}

bool matchesPattern(const String& target, const String& pattern) {
    // 支持通配符匹配
    if (pattern == "*") {
        return true;
    }
    
    if (pattern.find('*') != String::npos) {
        return matchesGlob(target, pattern);
    }
    
    if (pattern.find('^') == 0 || pattern.find('$') == pattern.length() - 1) {
        return matchesRegex(target, pattern);
    }
    
    return target == pattern;
}

bool matchesGlob(const String& target, const String& globPattern) {
    // 简化的glob模式匹配
    String regexPattern = globPattern;
    
    // 转义特殊字符
    regexPattern = std::regex_replace(regexPattern, std::regex(R"(\.)"), R"(\.)");
    regexPattern = std::regex_replace(regexPattern, std::regex(R"(\+)"), R"(\+)");
    regexPattern = std::regex_replace(regexPattern, std::regex(R"(\?)"), R"(\?)");
    regexPattern = std::regex_replace(regexPattern, std::regex(R"(\()"), R"(\()");
    regexPattern = std::regex_replace(regexPattern, std::regex(R"(\))"), R"(\))");
    regexPattern = std::regex_replace(regexPattern, std::regex(R"(\[)"), R"(\[)");
    regexPattern = std::regex_replace(regexPattern, std::regex(R"(\])"), R"(\])");
    regexPattern = std::regex_replace(regexPattern, std::regex(R"(\^)"), R"(\^)");
    regexPattern = std::regex_replace(regexPattern, std::regex(R"(\$)"), R"(\$)");
    regexPattern = std::regex_replace(regexPattern, std::regex(R"(\{)"), R"(\{)");
    regexPattern = std::regex_replace(regexPattern, std::regex(R"(\})"), R"(\})");
    regexPattern = std::regex_replace(regexPattern, std::regex(R"(\|)"), R"(\|)");
    
    // 转换通配符
    regexPattern = std::regex_replace(regexPattern, std::regex(R"(\*)"), ".*");
    
    return std::regex_match(target, std::regex(regexPattern));
}

bool matchesRegex(const String& target, const String& regexPattern) {
    try {
        return std::regex_match(target, std::regex(regexPattern));
    } catch (const std::exception&) {
        return false;
    }
}

} // namespace ConstraintUtils

} // namespace CHTL