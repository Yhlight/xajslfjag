#include "ConstraintSystem.h"
#include "../../Util/StringUtils.h"
#include <iostream>
#include <algorithm>
#include <regex>

namespace CHTL {

void ConstraintSystem::addRule(const ConstraintRule& rule) {
    rules.push_back(rule);
    
    // 保持规则按优先级排序
    std::sort(rules.begin(), rules.end());
}

void ConstraintSystem::addRule(ConstraintType type, const String& target, const String& context) {
    ConstraintRule rule(type, target, context);
    rule.message = "约束违反: 不允许使用 '" + target + "'";
    addRule(rule);
}

void ConstraintSystem::parseExceptClause(const String& exceptClause, const String& context) {
    // 解析 "except span, [Custom] @Element Box" 语法
    String trimmed = Util::StringUtils::trim(exceptClause);
    
    if (!Util::StringUtils::starts_with(trimmed, "except ")) {
        return;
    }
    
    String targets = trimmed.substr(7); // 移除 "except "
    auto constraintRules = ConstraintParser::parseExceptStatement(targets, context);
    
    for (const auto& rule : constraintRules) {
        addRule(rule);
    }
}

void ConstraintSystem::parseGlobalConstraint(const String& constraint, const String& namespaceName) {
    // 解析全局约束语法
    // TODO: 实现ConstraintParser::parseConstraintSyntax方法
    // auto rule = ConstraintParser::parseConstraintSyntax(constraint);
    // 暂时使用简单解析
    ConstraintRule rule(ConstraintType::GLOBAL_CONSTRAINT, constraint, namespaceName);
    rule.isGlobal = true;
    rule.context = namespaceName;
    
    addRule(rule.type, rule.target, rule.context);
}

bool ConstraintSystem::validate(BaseNode* node, ConstraintContext context) {
    if (!node) {
        return true;
    }
    
    bool isValid = true;
    
    for (const auto& rule : rules) {
        // 检查规则是否适用于当前上下文
        if (!isValidInContext(rule, context)) {
            continue;
        }
        
        bool violated = false;
        
        switch (rule.type) {
            case ConstraintType::ELEMENT_CONSTRAINT:
                violated = checkElementConstraint(rule, node);
                break;
            case ConstraintType::TEMPLATE_CONSTRAINT:
                violated = checkTemplateConstraint(rule, node);
                break;
            case ConstraintType::CUSTOM_CONSTRAINT:
                violated = checkCustomConstraint(rule, node);
                break;
            case ConstraintType::ORIGIN_CONSTRAINT:
                violated = checkOriginConstraint(rule, node);
                break;
            case ConstraintType::GLOBAL_CONSTRAINT:
                violated = checkGlobalConstraint(rule, node);
                break;
            case ConstraintType::PROPERTY_CONSTRAINT:
                violated = checkPropertyConstraint(rule, node);
                break;
        }
        
        if (violated) {
            String location = "Line " + std::to_string(node->getPosition().line) + 
                            ", Column " + std::to_string(node->getPosition().column);
            String description = "节点 '" + node->getValue() + "' 违反了约束: " + rule.target;
            
            recordViolation(rule, location, description, node);
            isValid = false;
            
            // 严格模式下立即停止
            if (enableStrictMode) {
                break;
            }
        }
    }
    
    return isValid;
}

bool ConstraintSystem::validateTree(BaseNode* root) {
    if (!root) {
        return true;
    }
    
    bool isValid = true;
    ConstraintContext context = getCurrentContext(root);
    
    // 验证当前节点
    if (!validate(root, context)) {
        isValid = false;
    }
    
    // 递归验证子节点
    for (size_t i = 0; i < root->getChildCount(); ++i) {
        if (!validateTree(root->getChild(i))) {
            isValid = false;
        }
    }
    
    return isValid;
}

bool ConstraintSystem::checkElementConstraint(const ConstraintRule& rule, BaseNode* node) const {
    if (node->getType() != NodeType::ELEMENT) {
        return false;
    }
    
    String elementName = node->getValue();
    
    // 检查是否匹配约束目标
    return Constraint::matchesTarget(rule.target, elementName);
}

bool ConstraintSystem::checkTemplateConstraint(const ConstraintRule& rule, BaseNode* node) const {
    NodeType nodeType = node->getType();
    
    // 检查模板相关节点类型
    bool isTemplateNode = (nodeType == NodeType::TEMPLATE ||
                          nodeType == NodeType::TEMPLATE_STYLE ||
                          nodeType == NodeType::TEMPLATE_ELEMENT ||
                          nodeType == NodeType::TEMPLATE_VAR);
    
    if (!isTemplateNode) {
        return false;
    }
    
    String templateType = node->getAttribute("templateType");
    String fullTarget = "[Template] @" + templateType;
    
    return Constraint::matchesTarget(rule.target, fullTarget);
}

bool ConstraintSystem::checkCustomConstraint(const ConstraintRule& rule, BaseNode* node) const {
    NodeType nodeType = node->getType();
    
    // 检查自定义相关节点类型
    bool isCustomNode = (nodeType == NodeType::CUSTOM ||
                        nodeType == NodeType::CUSTOM_STYLE ||
                        nodeType == NodeType::CUSTOM_ELEMENT ||
                        nodeType == NodeType::CUSTOM_VAR);
    
    if (!isCustomNode) {
        return false;
    }
    
    String customType = node->getAttribute("customType");
    String fullTarget = "[Custom] @" + customType;
    
    return Constraint::matchesTarget(rule.target, fullTarget);
}

bool ConstraintSystem::checkOriginConstraint(const ConstraintRule& rule, BaseNode* node) const {
    NodeType nodeType = node->getType();
    
    // 检查原始嵌入相关节点类型
    bool isOriginNode = (nodeType == NodeType::ORIGIN ||
                        nodeType == NodeType::ORIGIN_HTML ||
                        nodeType == NodeType::ORIGIN_STYLE ||
                        nodeType == NodeType::ORIGIN_JAVASCRIPT ||
                        nodeType == NodeType::ORIGIN_CUSTOM);
    
    if (!isOriginNode) {
        return false;
    }
    
    String originType = node->getAttribute("originType");
    String fullTarget = "[Origin] @" + originType;
    
    return Constraint::matchesTarget(rule.target, fullTarget);
}

bool ConstraintSystem::checkGlobalConstraint(const ConstraintRule& rule, BaseNode* node) const {
    // 全局约束检查所有节点
    String nodeValue = node->getValue();
    String nodeTypeStr = std::to_string(static_cast<int>(node->getType()));
    
    return Constraint::matchesTarget(rule.target, nodeValue) ||
           Constraint::matchesTarget(rule.target, nodeTypeStr);
}

bool ConstraintSystem::checkPropertyConstraint(const ConstraintRule& rule, BaseNode* node) const {
    if (node->getType() != NodeType::ATTRIBUTE) {
        return false;
    }
    
    String propertyName = node->getValue();
    return Constraint::matchesTarget(rule.target, propertyName);
}

ConstraintContext ConstraintSystem::getCurrentContext(BaseNode* node) const {
    if (!node) {
        return ConstraintContext::GLOBAL_SCOPE;
    }
    
    NodeType nodeType = node->getType();
    
    switch (nodeType) {
        case NodeType::STYLE:
            return ConstraintContext::STYLE_SCOPE;
        case NodeType::SCRIPT:
            return ConstraintContext::SCRIPT_SCOPE;
        case NodeType::TEMPLATE:
        case NodeType::TEMPLATE_STYLE:
        case NodeType::TEMPLATE_ELEMENT:
        case NodeType::TEMPLATE_VAR:
            return ConstraintContext::TEMPLATE_SCOPE;
        case NodeType::CUSTOM:
        case NodeType::CUSTOM_STYLE:
        case NodeType::CUSTOM_ELEMENT:
        case NodeType::CUSTOM_VAR:
            return ConstraintContext::CUSTOM_SCOPE;
        case NodeType::ELEMENT:
            return ConstraintContext::ELEMENT_SCOPE;
        case NodeType::NAMESPACE:
            return ConstraintContext::NAMESPACE_SCOPE;
        default:
            return ConstraintContext::GLOBAL_SCOPE;
    }
}

bool ConstraintSystem::isValidInContext(const ConstraintRule& rule, ConstraintContext context) const {
    // 全局约束在所有上下文中生效
    if (rule.isGlobal) {
        return true;
    }
    
    // 检查约束类型与上下文的兼容性
    return Constraint::isValidForScope(rule.type, context);
}

void ConstraintSystem::recordViolation(const ConstraintRule& rule, const String& location, 
                                     const String& description, BaseNode* node) {
    if (violations.size() >= static_cast<size_t>(maxViolations)) {
        return; // 达到最大违反数量限制
    }
    
    ConstraintViolation violation(rule, location, description);
    violation.node = node;
    violation.severity = enableStrictMode ? 5 : 3;
    
    violations.push_back(violation);
}

void ConstraintSystem::setupDefaultConstraints() {
    // 设置默认的约束规则
    
    // 全局样式块约束
    setupGlobalStyleConstraints();
    
    // 局部样式块约束
    setupLocalStyleConstraints();
    
    // 脚本块约束
    setupScriptConstraints();
    
    // 模板约束
    setupTemplateConstraints();
    
    // 自定义约束
    setupCustomConstraints();
}

void ConstraintSystem::setupGlobalStyleConstraints() {
    // 全局样式块只能允许：
    // - 模板变量的使用
    // - 自定义变量的使用
    // - 自定义变量的特例化
    // - 模板样式组
    // - 自定义样式组
    // - 无值样式组
    // - 自定义样式组的特例化
    // - delete属性
    // - delete继承
    // - 继承(样式组之间的继承)
    // - 生成器注释
    // - 全缀名
    // - 任意类型的原始嵌入
    // - 从命名空间中拿到某一个模板变量等
    
    // 添加全局样式块的约束
    ConstraintRule rule(ConstraintType::ELEMENT_CONSTRAINT, "script", "global_style");
    rule.message = "全局样式块中不允许使用script元素";
    addRule(rule);
}

void ConstraintSystem::setupLocalStyleConstraints() {
    // 局部样式块允许的内容与全局样式块类似
    // 但在特定元素作用域内
}

void ConstraintSystem::setupScriptConstraints() {
    // 除了局部script外，其他script禁止使用任何CHTL语法
    // 通常为模板变量，自定义变量组，变量组特例化，命名空间from
    // 特别允许的存在是--注释以及原始嵌入(任意类型)
}

void ConstraintSystem::setupTemplateConstraints() {
    // 模板内部的约束规则
}

void ConstraintSystem::setupCustomConstraints() {
    // 自定义内部的约束规则
}

void ConstraintSystem::printRules() const {
    std::cout << "\n=== Constraint Rules ===" << std::endl;
    
    for (size_t i = 0; i < rules.size(); ++i) {
        const auto& rule = rules[i];
        std::cout << "Rule " << (i + 1) << ": ";
        
        switch (rule.type) {
            case ConstraintType::ELEMENT_CONSTRAINT:
                std::cout << "Element";
                break;
            case ConstraintType::TEMPLATE_CONSTRAINT:
                std::cout << "Template";
                break;
            case ConstraintType::CUSTOM_CONSTRAINT:
                std::cout << "Custom";
                break;
            case ConstraintType::ORIGIN_CONSTRAINT:
                std::cout << "Origin";
                break;
            case ConstraintType::GLOBAL_CONSTRAINT:
                std::cout << "Global";
                break;
            case ConstraintType::PROPERTY_CONSTRAINT:
                std::cout << "Property";
                break;
        }
        
        std::cout << " - Target: " << rule.target;
        if (!rule.context.empty()) {
            std::cout << " (Context: " << rule.context << ")";
        }
        if (rule.isGlobal) {
            std::cout << " [Global]";
        }
        std::cout << std::endl;
    }
}

void ConstraintSystem::printViolations() const {
    if (violations.empty()) {
        std::cout << "No constraint violations found." << std::endl;
        return;
    }
    
    std::cout << "\n=== Constraint Violations ===" << std::endl;
    
    for (size_t i = 0; i < violations.size(); ++i) {
        const auto& violation = violations[i];
        std::cout << "Violation " << (i + 1) << ": " << violation.description << std::endl;
        std::cout << "  Location: " << violation.location << std::endl;
        std::cout << "  Rule: " << violation.rule.target << std::endl;
        std::cout << "  Severity: " << violation.severity << std::endl;
        std::cout << std::endl;
    }
}

// ConstraintBuilder实现
ConstraintBuilder::ConstraintBuilder(ConstraintType type, const String& target) 
    : rule(type, target) {
}

ConstraintBuilder& ConstraintBuilder::inContext(const String& context) {
    rule.context = context;
    return *this;
}

ConstraintBuilder& ConstraintBuilder::withMessage(const String& message) {
    rule.message = message;
    return *this;
}

ConstraintBuilder& ConstraintBuilder::withPriority(int priority) {
    rule.priority = priority;
    return *this;
}

ConstraintBuilder& ConstraintBuilder::asGlobal() {
    rule.isGlobal = true;
    return *this;
}

ConstraintBuilder& ConstraintBuilder::withExceptions(const StringVector& exceptions) {
    rule.exceptions = exceptions;
    return *this;
}

// ConstraintParser实现
std::vector<ConstraintRule> ConstraintParser::parseExceptStatement(const String& statement, const String& context) {
    std::vector<ConstraintRule> rules;
    
    auto targets = parseTargets(statement);
    
    for (const auto& target : targets) {
        ConstraintType type = parseType(target);
        ConstraintRule rule(type, target, context);
        rule.message = "约束违反: except 禁止使用 '" + target + "'";
        rules.push_back(rule);
    }
    
    return rules;
}

StringVector ConstraintParser::parseTargets(const String& targets) {
    // 使用逗号分割目标
    return Util::StringUtils::split(targets, ",");
}

ConstraintType ConstraintParser::parseType(const String& target) {
    String trimmed = Util::StringUtils::trim(target);
    
    if (isHTMLElement(trimmed)) {
        return ConstraintType::ELEMENT_CONSTRAINT;
    } else if (isTemplateType(trimmed)) {
        return ConstraintType::TEMPLATE_CONSTRAINT;
    } else if (isCustomType(trimmed)) {
        return ConstraintType::CUSTOM_CONSTRAINT;
    } else if (isOriginType(trimmed)) {
        return ConstraintType::ORIGIN_CONSTRAINT;
    } else {
        return ConstraintType::GLOBAL_CONSTRAINT;
    }
}

bool ConstraintParser::isHTMLElement(const String& target) {
    // 检查是否为HTML元素
    StringVector htmlElements = {
        "div", "span", "p", "h1", "h2", "h3", "h4", "h5", "h6",
        "a", "img", "ul", "ol", "li", "table", "tr", "td", "th",
        "form", "input", "button", "textarea", "select", "option"
    };
    
    return std::find(htmlElements.begin(), htmlElements.end(), target) != htmlElements.end();
}

bool ConstraintParser::isTemplateType(const String& target) {
    return target.find("[Template]") != String::npos;
}

bool ConstraintParser::isCustomType(const String& target) {
    return target.find("[Custom]") != String::npos;
}

bool ConstraintParser::isOriginType(const String& target) {
    return target.find("[Origin]") != String::npos || target.find("@Html") != String::npos;
}

// Constraint工具函数实现
namespace Constraint {

bool matchesTarget(const String& rule, const String& target) {
    // 支持简单的通配符匹配
    if (rule == target) {
        return true;
    }
    
    // 支持前缀匹配
    if (rule.back() == '*') {
        String prefix = rule.substr(0, rule.length() - 1);
        return Util::StringUtils::starts_with(target, prefix);
    }
    
    return false;
}

bool isValidForScope(ConstraintType type, ConstraintContext context) {
    switch (context) {
        case ConstraintContext::GLOBAL_SCOPE:
            return true; // 全局作用域允许所有约束类型
        case ConstraintContext::STYLE_SCOPE:
            return type == ConstraintType::PROPERTY_CONSTRAINT ||
                   type == ConstraintType::TEMPLATE_CONSTRAINT ||
                   type == ConstraintType::CUSTOM_CONSTRAINT;
        case ConstraintContext::SCRIPT_SCOPE:
            return type == ConstraintType::TEMPLATE_CONSTRAINT ||
                   type == ConstraintType::ORIGIN_CONSTRAINT;
        case ConstraintContext::ELEMENT_SCOPE:
            return type == ConstraintType::ELEMENT_CONSTRAINT ||
                   type == ConstraintType::TEMPLATE_CONSTRAINT ||
                   type == ConstraintType::CUSTOM_CONSTRAINT;
        default:
            return true;
    }
}

String contextToString(ConstraintContext context) {
    switch (context) {
        case ConstraintContext::GLOBAL_SCOPE: return "Global";
        case ConstraintContext::NAMESPACE_SCOPE: return "Namespace";
        case ConstraintContext::ELEMENT_SCOPE: return "Element";
        case ConstraintContext::STYLE_SCOPE: return "Style";
        case ConstraintContext::SCRIPT_SCOPE: return "Script";
        case ConstraintContext::TEMPLATE_SCOPE: return "Template";
        case ConstraintContext::CUSTOM_SCOPE: return "Custom";
        default: return "Unknown";
    }
}

} // namespace Constraint

} // namespace CHTL