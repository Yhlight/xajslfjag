#pragma once

#include "../../Util/Common.h"
#include "../CHTLNode/BaseNode.h"

namespace CHTL {

// 约束类型
enum class ConstraintType {
    ELEMENT_CONSTRAINT,     // 元素约束：禁止特定HTML元素
    TEMPLATE_CONSTRAINT,    // 模板约束：禁止模板类型
    CUSTOM_CONSTRAINT,      // 自定义约束：禁止自定义类型
    ORIGIN_CONSTRAINT,      // 原始嵌入约束：禁止原始嵌入类型
    GLOBAL_CONSTRAINT,      // 全局约束：在命名空间级别生效
    PROPERTY_CONSTRAINT     // 属性约束：禁止特定CSS属性
};

// 约束规则
struct ConstraintRule {
    ConstraintType type;           // 约束类型
    String target;                 // 约束目标
    String context;                // 约束上下文
    bool isGlobal = false;         // 是否为全局约束
    StringVector exceptions;       // 例外列表
    String message;                // 约束违反时的错误消息
    int priority = 0;              // 优先级（数值越大优先级越高）
    
    ConstraintRule(ConstraintType t, const String& tgt, const String& ctx = "")
        : type(t), target(tgt), context(ctx) {}
    
    bool operator<(const ConstraintRule& other) const {
        return priority > other.priority; // 高优先级排在前面
    }
};

// 约束违反信息
struct ConstraintViolation {
    ConstraintRule rule;           // 违反的规则
    String location;               // 违反位置
    String description;            // 违反描述
    BaseNode* node = nullptr;      // 违反的节点
    int severity = 1;              // 严重程度（1-5）
    
    ConstraintViolation(const ConstraintRule& r, const String& loc, const String& desc)
        : rule(r), location(loc), description(desc) {}
};

// 约束上下文
enum class ConstraintContext {
    GLOBAL_SCOPE,          // 全局作用域
    NAMESPACE_SCOPE,       // 命名空间作用域
    ELEMENT_SCOPE,         // 元素作用域
    STYLE_SCOPE,           // 样式作用域
    SCRIPT_SCOPE,          // 脚本作用域
    TEMPLATE_SCOPE,        // 模板作用域
    CUSTOM_SCOPE          // 自定义作用域
};

// 约束系统
class ConstraintSystem {
private:
    std::vector<ConstraintRule> rules;              // 约束规则列表
    std::vector<ConstraintViolation> violations;    // 违反记录
    StringUnorderedMap contextRules;                // 上下文特定规则
    bool enableStrictMode = false;                  // 严格模式
    int maxViolations = 100;                        // 最大违反数量
    
    // 内部检查方法
    bool checkElementConstraint(const ConstraintRule& rule, BaseNode* node) const;
    bool checkTemplateConstraint(const ConstraintRule& rule, BaseNode* node) const;
    bool checkCustomConstraint(const ConstraintRule& rule, BaseNode* node) const;
    bool checkOriginConstraint(const ConstraintRule& rule, BaseNode* node) const;
    bool checkGlobalConstraint(const ConstraintRule& rule, BaseNode* node) const;
    bool checkPropertyConstraint(const ConstraintRule& rule, BaseNode* node) const;
    
    // 约束解析
    ConstraintRule parseConstraintRule(const String& ruleString) const;
    ConstraintType parseConstraintType(const String& typeString) const;
    StringVector parseTargets(const String& targetsString) const;
    
    // 上下文管理
    ConstraintContext getCurrentContext(BaseNode* node) const;
    bool isValidInContext(const ConstraintRule& rule, ConstraintContext context) const;
    
public:
    // 构造函数
    ConstraintSystem() = default;
    ~ConstraintSystem() = default;
    
    // 禁用拷贝构造
    ConstraintSystem(const ConstraintSystem&) = delete;
    ConstraintSystem& operator=(const ConstraintSystem&) = delete;
    
    // 移动构造
    ConstraintSystem(ConstraintSystem&&) = default;
    ConstraintSystem& operator=(ConstraintSystem&&) = default;
    
    // 规则管理
    void addRule(const ConstraintRule& rule);
    void addRule(ConstraintType type, const String& target, const String& context = "");
    void removeRule(const String& target);
    void clearRules();
    
    // 约束解析（从CHTL语法）
    void parseExceptClause(const String& exceptClause, const String& context = "");
    void parseGlobalConstraint(const String& constraint, const String& namespaceName);
    
    // 约束检查
    bool validate(BaseNode* node, ConstraintContext context = ConstraintContext::GLOBAL_SCOPE);
    bool validateTree(BaseNode* root);
    bool isAllowed(const String& target, ConstraintContext context) const;
    
    // 违反管理
    void recordViolation(const ConstraintRule& rule, const String& location, const String& description, BaseNode* node = nullptr);
    std::vector<ConstraintViolation> getViolations() const { return violations; }
    void clearViolations() { violations.clear(); }
    
    // 预定义约束
    void setupDefaultConstraints();
    void setupGlobalStyleConstraints();   // 全局样式块约束
    void setupLocalStyleConstraints();    // 局部样式块约束
    void setupScriptConstraints();        // 脚本块约束
    void setupTemplateConstraints();      // 模板约束
    void setupCustomConstraints();        // 自定义约束
    
    // 配置
    void setStrictMode(bool strict) { enableStrictMode = strict; }
    bool isStrictMode() const { return enableStrictMode; }
    void setMaxViolations(int max) { maxViolations = max; }
    
    // 查询方法
    StringVector getRules() const;
    StringVector getRulesForContext(ConstraintContext context) const;
    bool hasRuleForTarget(const String& target) const;
    
    // 统计信息
    size_t getRuleCount() const { return rules.size(); }
    size_t getViolationCount() const { return violations.size(); }
    String getStatistics() const;
    
    // 调试方法
    void printRules() const;
    void printViolations() const;
    void dumpConstraints() const;
};

// 约束构建器
class ConstraintBuilder {
private:
    ConstraintRule rule;
    
public:
    ConstraintBuilder(ConstraintType type, const String& target);
    
    ConstraintBuilder& inContext(const String& context);
    ConstraintBuilder& withMessage(const String& message);
    ConstraintBuilder& withPriority(int priority);
    ConstraintBuilder& asGlobal();
    ConstraintBuilder& withExceptions(const StringVector& exceptions);
    
    ConstraintRule build() const { return rule; }
};

// 约束解析器
class ConstraintParser {
public:
    // 解析except语句
    static std::vector<ConstraintRule> parseExceptStatement(const String& statement, const String& context = "");
    
    // 解析约束目标
    static StringVector parseTargets(const String& targets);
    
    // 解析约束类型
    static ConstraintType parseType(const String& target);
    
    // 解析完整约束语法
    static ConstraintRule parseConstraintSyntax(const String& syntax);
    
    // 验证约束语法
    static bool validateConstraintSyntax(const String& syntax);
    
private:
    static bool isHTMLElement(const String& target);
    static bool isTemplateType(const String& target);
    static bool isCustomType(const String& target);
    static bool isOriginType(const String& target);
};

// 约束工具函数
namespace Constraint {
    // 目标匹配
    bool matchesTarget(const String& rule, const String& target);
    bool matchesPattern(const String& pattern, const String& text);
    
    // 上下文检查
    bool isValidForScope(ConstraintType type, ConstraintContext context);
    String contextToString(ConstraintContext context);
    ConstraintContext stringToContext(const String& contextStr);
    
    // 错误消息
    String formatViolationMessage(const ConstraintViolation& violation);
    String generateConstraintMessage(const ConstraintRule& rule);
    
    // 规则优化
    std::vector<ConstraintRule> optimizeRules(const std::vector<ConstraintRule>& rules);
    std::vector<ConstraintRule> mergeRules(const std::vector<ConstraintRule>& rules);
}

} // namespace CHTL