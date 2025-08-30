#include "ConstraintValidator.h"
#include "../CHTLNode/BaseNode.h"
#include <algorithm>
#include <regex>
#include <stack>

namespace CHTL {

// ConstraintRule实现
bool ConstraintRule::matches(const String& context) const {
    if (pattern.empty()) return false;
    
    try {
        std::regex contextRegex(pattern);
        return std::regex_match(context, contextRegex);
    } catch (const std::regex_error&) {
        return false;
    }
}

String ConstraintRule::toString() const {
    return "Rule: " + name + " (Type: " + ruleTypeToString(type) + 
           ", Pattern: " + pattern + ", Action: " + actionTypeToString(action) + ")";
}

String ConstraintRule::ruleTypeToString(RuleType type) {
    switch (type) {
        case RuleType::SCOPE_CONSTRAINT: return "SCOPE_CONSTRAINT";
        case RuleType::ACCESS_CONSTRAINT: return "ACCESS_CONSTRAINT";
        case RuleType::TYPE_CONSTRAINT: return "TYPE_CONSTRAINT";
        case RuleType::USAGE_CONSTRAINT: return "USAGE_CONSTRAINT";
        default: return "UNKNOWN";
    }
}

String ConstraintRule::actionTypeToString(ActionType action) {
    switch (action) {
        case ActionType::ALLOW: return "ALLOW";
        case ActionType::DENY: return "DENY";
        case ActionType::WARN: return "WARN";
        case ActionType::TRANSFORM: return "TRANSFORM";
        default: return "UNKNOWN";
    }
}

// ConstraintContext实现
void ConstraintContext::pushScope(const String& scopeName) {
    scopeStack.push_back(scopeName);
}

void ConstraintContext::popScope() {
    if (!scopeStack.empty()) {
        scopeStack.pop_back();
    }
}

String ConstraintContext::getCurrentScope() const {
    if (scopeStack.empty()) return "";
    return scopeStack.back();
}

String ConstraintContext::getFullScope() const {
    if (scopeStack.empty()) return "";
    
    String fullScope;
    for (size_t i = 0; i < scopeStack.size(); ++i) {
        if (i > 0) fullScope += "::";
        fullScope += scopeStack[i];
    }
    return fullScope;
}

bool ConstraintContext::isInScope(const String& scopeName) const {
    return std::find(scopeStack.begin(), scopeStack.end(), scopeName) != scopeStack.end();
}

void ConstraintContext::setVariable(const String& name, const String& value) {
    variables[name] = value;
}

String ConstraintContext::getVariable(const String& name) const {
    auto it = variables.find(name);
    return (it != variables.end()) ? it->second : "";
}

bool ConstraintContext::hasVariable(const String& name) const {
    return variables.find(name) != variables.end();
}

// ConstraintValidator实现
ConstraintValidator::ConstraintValidator(const ConstraintConfig& config) 
    : config(config), validationDepth(0) {
    initializeBuiltinRules();
}

ValidationResult ConstraintValidator::validate(BaseNode* ast) {
    ValidationResult result;
    result.success = false;
    
    if (!ast) {
        result.errors.push_back("AST为空");
        return result;
    }
    
    try {
        // 初始化验证上下文
        ConstraintContext context;
        validationDepth = 0;
        
        // 第一遍：收集所有约束定义
        collectConstraintDefinitions(ast, context, result);
        
        // 第二遍：验证约束
        validateConstraints(ast, context, result);
        
        // 第三遍：应用约束转换
        if (config.enableTransformations) {
            applyConstraintTransformations(ast, context, result);
        }
        
        result.success = result.errors.empty();
        
    } catch (const std::exception& e) {
        result.errors.push_back("约束验证异常: " + String(e.what()));
    }
    
    return result;
}

void ConstraintValidator::collectConstraintDefinitions(BaseNode* node, ConstraintContext& context, ValidationResult& result) {
    if (!node) return;
    
    // 处理约束定义节点
    if (node->type == NodeType::CONSTRAINT) {
        processConstraintDefinition(node, context, result);
    }
    
    // 处理except约束
    if (node->hasAttribute("except")) {
        processExceptConstraint(node, context, result);
    }
    
    // 更新作用域上下文
    if (isNodeScope(node)) {
        context.pushScope(getNodeScopeName(node));
        validationDepth++;
    }
    
    // 递归处理子节点
    for (const auto& child : node->children) {
        collectConstraintDefinitions(child.get(), context, result);
    }
    
    if (isNodeScope(node)) {
        context.popScope();
        validationDepth--;
    }
}

void ConstraintValidator::processConstraintDefinition(BaseNode* constraintNode, ConstraintContext& context, ValidationResult& result) {
    String constraintName = constraintNode->value;
    if (constraintName.empty()) {
        result.errors.push_back("约束名称不能为空");
        return;
    }
    
    ConstraintRule rule;
    rule.name = constraintName;
    rule.context = context.getFullScope();
    rule.definitionNode = constraintNode;
    
    // 解析约束类型
    if (constraintNode->hasAttribute("type")) {
        rule.type = parseRuleType(constraintNode->getAttribute("type"));
    }
    
    // 解析约束模式
    if (constraintNode->hasAttribute("pattern")) {
        rule.pattern = constraintNode->getAttribute("pattern");
    }
    
    // 解析约束动作
    if (constraintNode->hasAttribute("action")) {
        rule.action = parseActionType(constraintNode->getAttribute("action"));
    }
    
    // 解析约束条件
    if (constraintNode->hasAttribute("condition")) {
        rule.condition = constraintNode->getAttribute("condition");
    }
    
    // 解析错误消息
    if (constraintNode->hasAttribute("message")) {
        rule.message = constraintNode->getAttribute("message");
    }
    
    // 注册约束规则
    constraintRules.push_back(rule);
    result.appliedConstraints.push_back(rule);
}

void ConstraintValidator::processExceptConstraint(BaseNode* node, ConstraintContext& context, ValidationResult& result) {
    String exceptClause = node->getAttribute("except");
    
    // 解析except子句
    StringVector exceptions = parseExceptClause(exceptClause);
    
    for (const String& exception : exceptions) {
        ConstraintRule rule;
        rule.name = "except_" + exception;
        rule.type = RuleType::SCOPE_CONSTRAINT;
        rule.action = ActionType::DENY;
        rule.pattern = exception;
        rule.context = context.getFullScope();
        rule.message = "访问被except约束限制: " + exception;
        rule.definitionNode = node;
        
        constraintRules.push_back(rule);
    }
}

StringVector ConstraintValidator::parseExceptClause(const String& exceptClause) {
    StringVector exceptions;
    
    // 处理逗号分隔的异常列表
    std::regex commaRegex(R"(\s*,\s*)");
    std::sregex_token_iterator iter(exceptClause.begin(), exceptClause.end(), commaRegex, -1);
    std::sregex_token_iterator end;
    
    for (; iter != end; ++iter) {
        String exception = iter->str();
        if (!exception.empty()) {
            exceptions.push_back(exception);
        }
    }
    
    return exceptions;
}

void ConstraintValidator::validateConstraints(BaseNode* node, ConstraintContext& context, ValidationResult& result) {
    if (!node) return;
    
    // 更新作用域上下文
    if (isNodeScope(node)) {
        context.pushScope(getNodeScopeName(node));
    }
    
    // 验证当前节点的约束
    validateNodeConstraints(node, context, result);
    
    // 递归验证子节点
    for (const auto& child : node->children) {
        validateConstraints(child.get(), context, result);
    }
    
    if (isNodeScope(node)) {
        context.popScope();
    }
}

void ConstraintValidator::validateNodeConstraints(BaseNode* node, ConstraintContext& context, ValidationResult& result) {
    String nodeContext = buildNodeContext(node, context);
    
    // 检查所有适用的约束规则
    for (const ConstraintRule& rule : constraintRules) {
        if (isRuleApplicable(rule, node, context)) {
            validateRule(rule, node, context, result);
        }
    }
    
    // 检查内置约束
    validateBuiltinConstraints(node, context, result);
}

bool ConstraintValidator::isRuleApplicable(const ConstraintRule& rule, BaseNode* node, const ConstraintContext& context) {
    // 检查作用域
    if (!rule.context.empty() && context.getFullScope().find(rule.context) == String::npos) {
        return false;
    }
    
    // 检查模式匹配
    if (!rule.pattern.empty()) {
        String nodeContext = buildNodeContext(node, context);
        if (!rule.matches(nodeContext)) {
            return false;
        }
    }
    
    // 检查条件
    if (!rule.condition.empty()) {
        if (!evaluateCondition(rule.condition, node, context)) {
            return false;
        }
    }
    
    return true;
}

void ConstraintValidator::validateRule(const ConstraintRule& rule, BaseNode* node, const ConstraintContext& context, ValidationResult& result) {
    switch (rule.action) {
        case ActionType::ALLOW:
            // 允许访问，无需额外处理
            break;
            
        case ActionType::DENY:
            {
                String message = rule.message.empty() ? 
                    ("约束违反: " + rule.name) : rule.message;
                result.errors.push_back(message + " at " + getNodeLocation(node));
            }
            break;
            
        case ActionType::WARN:
            {
                String message = rule.message.empty() ? 
                    ("约束警告: " + rule.name) : rule.message;
                result.warnings.push_back(message + " at " + getNodeLocation(node));
            }
            break;
            
        case ActionType::TRANSFORM:
            {
                ConstraintViolation violation;
                violation.rule = rule;
                violation.node = node;
                violation.context = context.getFullScope();
                violation.message = rule.message;
                
                result.violations.push_back(violation);
            }
            break;
    }
}

void ConstraintValidator::validateBuiltinConstraints(BaseNode* node, const ConstraintContext& context, ValidationResult& result) {
    // 验证访问权限约束
    validateAccessConstraints(node, context, result);
    
    // 验证类型约束
    validateTypeConstraints(node, context, result);
    
    // 验证使用约束
    validateUsageConstraints(node, context, result);
}

void ConstraintValidator::validateAccessConstraints(BaseNode* node, const ConstraintContext& context, ValidationResult& result) {
    // 检查private/protected访问
    if (node->hasAttribute("access")) {
        String access = node->getAttribute("access");
        if (access == "private" || access == "protected") {
            String currentScope = context.getCurrentScope();
            String nodeScope = getNodeScopeName(node);
            
            if (access == "private" && currentScope != nodeScope) {
                result.errors.push_back("访问私有成员: " + node->value + " at " + getNodeLocation(node));
            } else if (access == "protected" && !isInheritanceScope(currentScope, nodeScope)) {
                result.errors.push_back("访问受保护成员: " + node->value + " at " + getNodeLocation(node));
            }
        }
    }
}

void ConstraintValidator::validateTypeConstraints(BaseNode* node, const ConstraintContext& context, ValidationResult& result) {
    // 检查类型兼容性
    if (node->hasAttribute("expected_type")) {
        String expectedType = node->getAttribute("expected_type");
        String actualType = getNodeType(node);
        
        if (!isTypeCompatible(actualType, expectedType)) {
            result.errors.push_back("类型不匹配: 期望 " + expectedType + 
                                  ", 实际 " + actualType + " at " + getNodeLocation(node));
        }
    }
}

void ConstraintValidator::validateUsageConstraints(BaseNode* node, const ConstraintContext& context, ValidationResult& result) {
    // 检查使用限制
    if (node->hasAttribute("usage_constraint")) {
        String constraint = node->getAttribute("usage_constraint");
        
        if (constraint == "readonly" && isModifyingOperation(node)) {
            result.errors.push_back("尝试修改只读对象: " + node->value + " at " + getNodeLocation(node));
        } else if (constraint == "once" && hasBeenUsed(node, context)) {
            result.errors.push_back("重复使用一次性对象: " + node->value + " at " + getNodeLocation(node));
        }
    }
}

void ConstraintValidator::applyConstraintTransformations(BaseNode* ast, ConstraintContext& context, ValidationResult& result) {
    for (const ConstraintViolation& violation : result.violations) {
        if (violation.rule.action == ActionType::TRANSFORM) {
            applyTransformation(violation, result);
        }
    }
}

void ConstraintValidator::applyTransformation(const ConstraintViolation& violation, ValidationResult& result) {
    BaseNode* node = violation.node;
    const ConstraintRule& rule = violation.rule;
    
    // 根据规则类型应用不同的转换
    switch (rule.type) {
        case RuleType::ACCESS_CONSTRAINT:
            transformAccessViolation(node, rule, result);
            break;
            
        case RuleType::TYPE_CONSTRAINT:
            transformTypeViolation(node, rule, result);
            break;
            
        case RuleType::SCOPE_CONSTRAINT:
            transformScopeViolation(node, rule, result);
            break;
            
        case RuleType::USAGE_CONSTRAINT:
            transformUsageViolation(node, rule, result);
            break;
    }
}

void ConstraintValidator::transformAccessViolation(BaseNode* node, const ConstraintRule& rule, ValidationResult& result) {
    // 自动添加访问修饰符
    if (!node->hasAttribute("access")) {
        node->setAttribute("access", "public");
        result.transformations.push_back("添加访问修饰符: " + node->value);
    }
}

void ConstraintValidator::transformTypeViolation(BaseNode* node, const ConstraintRule& rule, ValidationResult& result) {
    // 自动类型转换
    if (node->hasAttribute("expected_type")) {
        String expectedType = node->getAttribute("expected_type");
        node->setAttribute("transformed_type", expectedType);
        result.transformations.push_back("类型转换: " + node->value + " -> " + expectedType);
    }
}

void ConstraintValidator::transformScopeViolation(BaseNode* node, const ConstraintRule& rule, ValidationResult& result) {
    // 自动作用域调整
    String newScope = "public::" + node->value;
    node->setAttribute("resolved_scope", newScope);
    result.transformations.push_back("作用域调整: " + node->value + " -> " + newScope);
}

void ConstraintValidator::transformUsageViolation(BaseNode* node, const ConstraintRule& rule, ValidationResult& result) {
    // 自动使用约束处理
    node->setAttribute("usage_handled", "true");
    result.transformations.push_back("使用约束处理: " + node->value);
}

void ConstraintValidator::initializeBuiltinRules() {
    // 初始化内置约束规则
    
    // 私有访问约束
    ConstraintRule privateRule;
    privateRule.name = "private_access";
    privateRule.type = RuleType::ACCESS_CONSTRAINT;
    privateRule.action = ActionType::DENY;
    privateRule.pattern = R"(.*private.*)";
    privateRule.message = "私有成员访问被拒绝";
    builtinRules.push_back(privateRule);
    
    // 类型安全约束
    ConstraintRule typeSafetyRule;
    typeSafetyRule.name = "type_safety";
    typeSafetyRule.type = RuleType::TYPE_CONSTRAINT;
    typeSafetyRule.action = ActionType::WARN;
    typeSafetyRule.message = "类型不安全操作";
    builtinRules.push_back(typeSafetyRule);
    
    // 空指针约束
    ConstraintRule nullPtrRule;
    nullPtrRule.name = "null_pointer";
    nullPtrRule.type = RuleType::USAGE_CONSTRAINT;
    nullPtrRule.action = ActionType::DENY;
    nullPtrRule.pattern = R"(.*null.*)";
    nullPtrRule.message = "空指针访问";
    builtinRules.push_back(nullPtrRule);
}

// 辅助方法实现
RuleType ConstraintValidator::parseRuleType(const String& typeStr) {
    if (typeStr == "scope") return RuleType::SCOPE_CONSTRAINT;
    if (typeStr == "access") return RuleType::ACCESS_CONSTRAINT;
    if (typeStr == "type") return RuleType::TYPE_CONSTRAINT;
    if (typeStr == "usage") return RuleType::USAGE_CONSTRAINT;
    return RuleType::SCOPE_CONSTRAINT;
}

ActionType ConstraintValidator::parseActionType(const String& actionStr) {
    if (actionStr == "allow") return ActionType::ALLOW;
    if (actionStr == "deny") return ActionType::DENY;
    if (actionStr == "warn") return ActionType::WARN;
    if (actionStr == "transform") return ActionType::TRANSFORM;
    return ActionType::DENY;
}

bool ConstraintValidator::isNodeScope(BaseNode* node) {
    return node && (node->type == NodeType::NAMESPACE ||
                   node->type == NodeType::TEMPLATE ||
                   node->type == NodeType::CUSTOM ||
                   node->type == NodeType::FUNCTION);
}

String ConstraintValidator::getNodeScopeName(BaseNode* node) {
    if (!node) return "";
    return node->value;
}

String ConstraintValidator::buildNodeContext(BaseNode* node, const ConstraintContext& context) {
    String nodeContext = context.getFullScope();
    if (!nodeContext.empty()) nodeContext += "::";
    nodeContext += (node ? node->value : "unknown");
    return nodeContext;
}

bool ConstraintValidator::evaluateCondition(const String& condition, BaseNode* node, const ConstraintContext& context) {
    // 简化的条件评估
    if (condition == "true") return true;
    if (condition == "false") return false;
    
    // 检查变量条件
    if (condition.find("=") != String::npos) {
        size_t eqPos = condition.find("=");
        String varName = condition.substr(0, eqPos);
        String varValue = condition.substr(eqPos + 1);
        
        return context.getVariable(varName) == varValue;
    }
    
    return true;
}

String ConstraintValidator::getNodeLocation(BaseNode* node) {
    if (!node) return "unknown";
    return "line:" + std::to_string(node->position.line) + 
           ", col:" + std::to_string(node->position.column);
}

bool ConstraintValidator::isInheritanceScope(const String& currentScope, const String& nodeScope) {
    // 简化的继承关系检查
    return currentScope.find(nodeScope) != String::npos;
}

String ConstraintValidator::getNodeType(BaseNode* node) {
    if (!node) return "unknown";
    
    switch (node->type) {
        case NodeType::TEMPLATE: return "template";
        case NodeType::CUSTOM: return "custom";
        case NodeType::ELEMENT: return "element";
        case NodeType::TEXT: return "text";
        default: return "unknown";
    }
}

bool ConstraintValidator::isTypeCompatible(const String& actualType, const String& expectedType) {
    if (actualType == expectedType) return true;
    
    // 简化的类型兼容性检查
    if (expectedType == "any") return true;
    if (actualType == "unknown") return false;
    
    return false;
}

bool ConstraintValidator::isModifyingOperation(BaseNode* node) {
    if (!node) return false;
    
    // 检查是否是修改操作
    return node->type == NodeType::ASSIGNMENT ||
           node->hasAttribute("operation") && 
           node->getAttribute("operation") == "modify";
}

bool ConstraintValidator::hasBeenUsed(BaseNode* node, const ConstraintContext& context) {
    // 简化的使用追踪
    return node && node->hasAttribute("used") && 
           node->getAttribute("used") == "true";
}

void ConstraintValidator::addRule(const ConstraintRule& rule) {
    constraintRules.push_back(rule);
}

void ConstraintValidator::removeRule(const String& ruleName) {
    constraintRules.erase(
        std::remove_if(constraintRules.begin(), constraintRules.end(),
                      [&ruleName](const ConstraintRule& rule) {
                          return rule.name == ruleName;
                      }),
        constraintRules.end());
}

std::vector<ConstraintRule> ConstraintValidator::getRules() const {
    return constraintRules;
}

ConstraintRule ConstraintValidator::getRule(const String& ruleName) const {
    auto it = std::find_if(constraintRules.begin(), constraintRules.end(),
                          [&ruleName](const ConstraintRule& rule) {
                              return rule.name == ruleName;
                          });
    
    if (it != constraintRules.end()) {
        return *it;
    }
    
    return ConstraintRule();
}

void ConstraintValidator::clearRules() {
    constraintRules.clear();
}

void ConstraintValidator::setConfig(const ConstraintConfig& newConfig) {
    config = newConfig;
}

ConstraintConfig ConstraintValidator::getConfig() const {
    return config;
}

size_t ConstraintValidator::getRuleCount() const {
    return constraintRules.size();
}

} // namespace CHTL