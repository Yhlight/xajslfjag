#pragma once

#include "ConstraintSystem.h"
#include "../../Util/Common.h"
#include <unordered_map>
#include <vector>
#include <functional>
#include <memory>

namespace CHTL {

// 约束类型枚举
enum class ConstraintType {
    PRECISE,            // 精确约束 (具体项目名称)
    TYPE_BASED,         // 类型约束 (基于节点类型)
    GLOBAL,             // 全局约束 (影响整个文档)
    CONTEXTUAL,         // 上下文约束 (基于当前上下文)
    CONDITIONAL,        // 条件约束 (基于条件表达式)
    INHERITANCE,        // 继承约束 (基于继承关系)
    SCOPE,              // 作用域约束 (基于命名空间)
    PERMISSION,         // 权限约束 (基于访问权限)
    RESOURCE,           // 资源约束 (基于资源限制)
    VALIDATION,         // 验证约束 (基于验证规则)
    PERFORMANCE,        // 性能约束 (基于性能要求)
    SECURITY            // 安全约束 (基于安全策略)
};

// 约束操作类型
enum class ConstraintAction {
    DENY,               // 拒绝 (禁止使用)
    ALLOW,              // 允许 (明确允许)
    RESTRICT,           // 限制 (有条件允许)
    WARN,               // 警告 (允许但发出警告)
    REPLACE,            // 替换 (用其他内容替换)
    TRANSFORM,          // 转换 (转换为其他形式)
    DELEGATE,           // 委托 (委托给其他处理器)
    MONITOR             // 监控 (允许但记录使用)
};

// 约束严重级别
enum class ConstraintSeverity {
    CRITICAL,           // 严重 (必须阻止)
    HIGH,               // 高 (强烈建议阻止)
    MEDIUM,             // 中等 (建议阻止)
    LOW,                // 低 (轻微警告)
    INFO                // 信息 (仅记录)
};

// 约束上下文信息
struct ConstraintContext {
    String currentNamespace;        // 当前命名空间
    String currentTemplate;         // 当前模板
    String currentCustom;           // 当前自定义
    String parentElement;           // 父元素
    StringVector ancestorElements;  // 祖先元素链
    StringUnorderedMap variables;   // 上下文变量
    size_t nestingDepth;           // 嵌套深度
    bool isInTemplate;             // 是否在模板中
    bool isInCustom;               // 是否在自定义中
    bool isInOrigin;               // 是否在原始嵌入中
    
    ConstraintContext() : nestingDepth(0), isInTemplate(false), isInCustom(false), isInOrigin(false) {}
};

// 约束规则
class ConstraintRule {
public:
    String ruleName;                // 规则名称
    String description;             // 规则描述
    ConstraintType type;           // 约束类型
    ConstraintAction action;       // 约束操作
    ConstraintSeverity severity;   // 严重级别
    StringVector targets;          // 约束目标
    StringVector exceptions;       // 例外列表
    String condition;              // 条件表达式
    StringUnorderedMap parameters; // 参数映射
    bool isEnabled;                // 是否启用
    bool isGlobal;                 // 是否全局生效
    String namespace_;             // 适用的命名空间
    double priority;               // 优先级 (数值越大优先级越高)
    
    explicit ConstraintRule(const String& name);
    virtual ~ConstraintRule() = default;
    
    // 规则匹配
    virtual bool matches(const String& target, const ConstraintContext& context) const;
    virtual bool matchesType(NodeType nodeType, const ConstraintContext& context) const;
    virtual bool matchesNamespace(const String& namespaceName, const ConstraintContext& context) const;
    
    // 条件评估
    virtual bool evaluateCondition(const ConstraintContext& context) const;
    virtual String expandCondition(const String& condition, const ConstraintContext& context) const;
    
    // 规则应用
    virtual ConstraintAction getAction(const String& target, const ConstraintContext& context) const;
    virtual String getReplacementContent(const String& originalContent, const ConstraintContext& context) const;
    virtual String getTransformationResult(const String& originalContent, const ConstraintContext& context) const;
    
    // 验证和优化
    virtual bool validateRule() const;
    virtual StringVector getValidationErrors() const;
    virtual void optimizeRule();
    
    // 序列化
    virtual String toString() const;
    virtual String toJSON() const;
    virtual bool fromString(const String& ruleString);
    virtual bool fromJSON(const String& jsonString);
    
protected:
    // 内部辅助方法
    bool matchesTarget(const String& target) const;
    bool isException(const String& target) const;
    bool evaluateSimpleCondition(const String& condition, const ConstraintContext& context) const;
    String expandVariables(const String& text, const ConstraintContext& context) const;
};

// 类型约束规则
class TypeConstraintRule : public ConstraintRule {
public:
    StringVector allowedTypes;     // 允许的类型列表
    StringVector deniedTypes;      // 禁止的类型列表
    StringUnorderedMap typeTransforms; // 类型转换映射
    
    explicit TypeConstraintRule(const String& name);
    ~TypeConstraintRule() override = default;
    
    // 类型检查
    bool isTypeAllowed(NodeType nodeType) const;
    bool isTypeDenied(NodeType nodeType) const;
    NodeType getTransformedType(NodeType originalType) const;
    
    // 重写基类方法
    bool matchesType(NodeType nodeType, const ConstraintContext& context) const override;
    ConstraintAction getAction(const String& target, const ConstraintContext& context) const override;
    String getTransformationResult(const String& originalContent, const ConstraintContext& context) const override;
    
    // 类型特定方法
    void addAllowedType(const String& typeName);
    void addDeniedType(const String& typeName);
    void addTypeTransform(const String& fromType, const String& toType);
    void setAllowedTypes(const StringVector& types);
    void setDeniedTypes(const StringVector& types);
    
private:
    NodeType parseNodeType(const String& typeName) const;
    String nodeTypeToString(NodeType type) const;
};

// 全局约束规则
class GlobalConstraintRule : public ConstraintRule {
public:
    size_t maxTotalElements;       // 最大元素总数
    size_t maxNestingDepth;        // 最大嵌套深度
    size_t maxTemplateCount;       // 最大模板数量
    size_t maxCustomCount;         // 最大自定义数量
    size_t maxOriginCount;         // 最大原始嵌入数量
    size_t maxImportCount;         // 最大导入数量
    StringVector requiredElements; // 必需元素列表
    StringVector forbiddenElements; // 禁止元素列表
    StringUnorderedMap globalLimits; // 全局限制映射
    
    explicit GlobalConstraintRule(const String& name);
    ~GlobalConstraintRule() override = default;
    
    // 全局检查
    bool checkGlobalLimits(const ConstraintContext& context) const;
    bool checkElementCount(const String& elementType, size_t currentCount) const;
    bool checkNestingDepth(size_t currentDepth) const;
    bool checkRequiredElements(const StringVector& presentElements) const;
    bool checkForbiddenElements(const StringVector& presentElements) const;
    
    // 重写基类方法
    bool matches(const String& target, const ConstraintContext& context) const override;
    ConstraintAction getAction(const String& target, const ConstraintContext& context) const override;
    
    // 全局特定方法
    void setElementLimit(const String& elementType, size_t limit);
    void addRequiredElement(const String& elementName);
    void addForbiddenElement(const String& elementName);
    void setGlobalLimit(const String& limitName, const String& limitValue);
    
private:
    bool isWithinLimit(const String& limitName, size_t currentValue) const;
    size_t parseLimit(const String& limitValue) const;
};

// 条件约束规则
class ConditionalConstraintRule : public ConstraintRule {
public:
    String ifCondition;            // if条件
    String thenAction;             // then动作
    String elseAction;             // else动作
    StringVector conditionVariables; // 条件变量列表
    
    explicit ConditionalConstraintRule(const String& name);
    ~ConditionalConstraintRule() override = default;
    
    // 条件处理
    bool evaluateIfCondition(const ConstraintContext& context) const;
    ConstraintAction getThenAction() const;
    ConstraintAction getElseAction() const;
    
    // 重写基类方法
    bool evaluateCondition(const ConstraintContext& context) const override;
    ConstraintAction getAction(const String& target, const ConstraintContext& context) const override;
    
    // 条件特定方法
    void setCondition(const String& condition, const String& thenAct, const String& elseAct = "");
    void addConditionVariable(const String& variableName);
    
private:
    ConstraintAction parseAction(const String& actionString) const;
    bool evaluateComplexCondition(const String& condition, const ConstraintContext& context) const;
};

// 高级约束系统
class AdvancedConstraintSystem : public ConstraintSystem {
public:
    AdvancedConstraintSystem();
    ~AdvancedConstraintSystem() override = default;
    
    // 规则管理
    void addRule(std::shared_ptr<ConstraintRule> rule);
    void removeRule(const String& ruleName);
    std::shared_ptr<ConstraintRule> getRule(const String& ruleName) const;
    bool hasRule(const String& ruleName) const;
    StringVector getAllRuleNames() const;
    
    // 规则类型管理
    void addTypeRule(std::shared_ptr<TypeConstraintRule> rule);
    void addGlobalRule(std::shared_ptr<GlobalConstraintRule> rule);
    void addConditionalRule(std::shared_ptr<ConditionalConstraintRule> rule);
    StringVector getTypeRules() const;
    StringVector getGlobalRules() const;
    StringVector getConditionalRules() const;
    
    // 约束检查
    bool checkConstraints(const String& target, const ConstraintContext& context) const;
    bool checkTypeConstraints(NodeType nodeType, const ConstraintContext& context) const;
    bool checkGlobalConstraints(const ConstraintContext& context) const;
    std::vector<ConstraintAction> getApplicableActions(const String& target, const ConstraintContext& context) const;
    
    // 约束应用
    String applyConstraints(const String& content, const ConstraintContext& context) const;
    String transformContent(const String& content, const ConstraintContext& context) const;
    StringVector getConstraintWarnings(const String& target, const ConstraintContext& context) const;
    
    // 约束验证
    bool validateAllRules() const;
    StringVector getValidationErrors() const;
    StringVector checkRuleConflicts() const;
    void optimizeRules();
    
    // 上下文管理
    ConstraintContext createContext(const String& namespaceName = "") const;
    void updateContext(ConstraintContext& context, const String& key, const String& value) const;
    void pushContextScope(ConstraintContext& context, const String& scopeName) const;
    void popContextScope(ConstraintContext& context) const;
    
    // 统计和分析
    std::unordered_map<String, size_t> getConstraintStatistics() const;
    StringVector getMostUsedRules() const;
    StringVector getUnusedRules() const;
    double getConstraintCoverage() const;
    
    // 配置管理
    void enableRule(const String& ruleName, bool enable = true);
    void setRulePriority(const String& ruleName, double priority);
    void setGlobalMode(bool global);
    void setStrictMode(bool strict);
    
    // 导入导出
    bool exportRules(const String& filePath) const;
    bool importRules(const String& filePath);
    String exportRulesToString() const;
    bool importRulesFromString(const String& rulesData);
    
    // 预设规则集
    void loadStrictRuleSet();
    void loadPermissiveRuleSet();
    void loadSecurityRuleSet();
    void loadPerformanceRuleSet();
    void loadCompatibilityRuleSet();
    
    // 调试和诊断
    void dumpRules() const;
    void analyzeRulePerformance() const;
    void generateConstraintReport() const;
    void validateSystemIntegrity() const;
    
    // 事件系统
    using ConstraintEventCallback = std::function<void(const String&, const ConstraintContext&, ConstraintAction)>;
    void onConstraintViolation(ConstraintEventCallback callback);
    void onConstraintWarning(ConstraintEventCallback callback);
    void onConstraintApplied(ConstraintEventCallback callback);
    
private:
    std::unordered_map<String, std::shared_ptr<ConstraintRule>> rules;
    std::vector<std::shared_ptr<TypeConstraintRule>> typeRules;
    std::vector<std::shared_ptr<GlobalConstraintRule>> globalRules;
    std::vector<std::shared_ptr<ConditionalConstraintRule>> conditionalRules;
    
    std::vector<ConstraintEventCallback> violationCallbacks;
    std::vector<ConstraintEventCallback> warningCallbacks;
    std::vector<ConstraintEventCallback> appliedCallbacks;
    
    bool globalMode;
    bool strictMode;
    mutable std::unordered_map<String, size_t> ruleUsageStats;
    
    // 内部辅助方法
    std::vector<std::shared_ptr<ConstraintRule>> getApplicableRules(const String& target, const ConstraintContext& context) const;
    std::vector<std::shared_ptr<ConstraintRule>> sortRulesByPriority(const std::vector<std::shared_ptr<ConstraintRule>>& rules) const;
    
    // 约束应用内部方法
    String applyRuleTransformation(const String& content, std::shared_ptr<ConstraintRule> rule, const ConstraintContext& context) const;
    String applyRuleReplacement(const String& content, std::shared_ptr<ConstraintRule> rule, const ConstraintContext& context) const;
    
    // 冲突检测
    bool hasRuleConflict(std::shared_ptr<ConstraintRule> rule1, std::shared_ptr<ConstraintRule> rule2) const;
    String describeRuleConflict(std::shared_ptr<ConstraintRule> rule1, std::shared_ptr<ConstraintRule> rule2) const;
    
    // 事件触发
    void triggerConstraintViolation(const String& target, const ConstraintContext& context, ConstraintAction action) const;
    void triggerConstraintWarning(const String& target, const ConstraintContext& context, ConstraintAction action) const;
    void triggerConstraintApplied(const String& target, const ConstraintContext& context, ConstraintAction action) const;
    
    // 初始化方法
    void initializeBuiltinRules();
    void initializeDefaultConfiguration();
};

// 约束构建器
class ConstraintBuilder {
public:
    explicit ConstraintBuilder(const String& ruleName);
    ~ConstraintBuilder() = default;
    
    // 基础配置
    ConstraintBuilder& description(const String& desc);
    ConstraintBuilder& type(ConstraintType constraintType);
    ConstraintBuilder& action(ConstraintAction constraintAction);
    ConstraintBuilder& severity(ConstraintSeverity constraintSeverity);
    ConstraintBuilder& priority(double rulePriority);
    
    // 目标和例外
    ConstraintBuilder& target(const String& targetName);
    ConstraintBuilder& targets(const StringVector& targetNames);
    ConstraintBuilder& except(const String& exceptionName);
    ConstraintBuilder& exceptions(const StringVector& exceptionNames);
    
    // 条件和参数
    ConstraintBuilder& condition(const String& conditionExpression);
    ConstraintBuilder& parameter(const String& key, const String& value);
    ConstraintBuilder& parameters(const StringUnorderedMap& params);
    
    // 作用域
    ConstraintBuilder& global(bool isGlobal = true);
    ConstraintBuilder& namespace_(const String& namespaceName);
    ConstraintBuilder& enabled(bool isEnabled = true);
    
    // 类型约束特定
    ConstraintBuilder& allowType(const String& typeName);
    ConstraintBuilder& denyType(const String& typeName);
    ConstraintBuilder& transformType(const String& fromType, const String& toType);
    
    // 全局约束特定
    ConstraintBuilder& maxElements(size_t maxCount);
    ConstraintBuilder& maxDepth(size_t maxNestingDepth);
    ConstraintBuilder& requireElement(const String& elementName);
    ConstraintBuilder& forbidElement(const String& elementName);
    
    // 条件约束特定
    ConstraintBuilder& ifCondition(const String& condition, const String& thenAction, const String& elseAction = "");
    
    // 构建方法
    std::shared_ptr<ConstraintRule> build();
    std::shared_ptr<TypeConstraintRule> buildTypeRule();
    std::shared_ptr<GlobalConstraintRule> buildGlobalRule();
    std::shared_ptr<ConditionalConstraintRule> buildConditionalRule();
    
private:
    std::shared_ptr<ConstraintRule> baseRule;
    ConstraintType ruleType;
    StringVector buildErrors;
    
    // 类型约束数据
    StringVector allowedTypes;
    StringVector deniedTypes;
    StringUnorderedMap typeTransforms;
    
    // 全局约束数据
    size_t maxElementCount;
    size_t maxNestingDepth;
    StringVector requiredElements;
    StringVector forbiddenElements;
    
    // 条件约束数据
    String ifCond;
    String thenAct;
    String elseAct;
    
    bool validateBuild() const;
    void addBuildError(const String& error);
};

// 约束工具函数
namespace ConstraintUtils {
    // 约束表达式解析
    bool parseConstraintExpression(const String& expression, StringVector& targets, StringVector& exceptions);
    String normalizeConstraintTarget(const String& target);
    bool isValidConstraintExpression(const String& expression);
    
    // 条件表达式处理
    bool evaluateConditionExpression(const String& expression, const StringUnorderedMap& variables);
    String expandConditionVariables(const String& expression, const StringUnorderedMap& variables);
    StringVector extractConditionVariables(const String& expression);
    
    // 约束优化
    std::vector<std::shared_ptr<ConstraintRule>> optimizeRuleSet(const std::vector<std::shared_ptr<ConstraintRule>>& rules);
    std::vector<std::shared_ptr<ConstraintRule>> removeRedundantRules(const std::vector<std::shared_ptr<ConstraintRule>>& rules);
    std::vector<std::shared_ptr<ConstraintRule>> mergeCompatibleRules(const std::vector<std::shared_ptr<ConstraintRule>>& rules);
    
    // 约束分析
    std::unordered_map<String, StringVector> analyzeRuleDependencies(const std::vector<std::shared_ptr<ConstraintRule>>& rules);
    StringVector findUnreachableRules(const std::vector<std::shared_ptr<ConstraintRule>>& rules);
    double calculateRuleComplexity(std::shared_ptr<ConstraintRule> rule);
    
    // 约束模式匹配
    bool matchesPattern(const String& target, const String& pattern);
    bool matchesGlob(const String& target, const String& globPattern);
    bool matchesRegex(const String& target, const String& regexPattern);
}

} // namespace CHTL