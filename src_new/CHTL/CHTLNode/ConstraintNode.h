#pragma once
#include "BaseNode.h"
#include <string>
#include <vector>
#include <unordered_set>

namespace CHTL {

/**
 * 约束类型
 */
enum class ConstraintType {
    EXACT_CONSTRAINT,      // 精确约束 (禁止特定HTML元素或模板/自定义对象)
    TYPE_CONSTRAINT,       // 类型约束 (禁止整个类型如@Html、[Custom]、[Template])
    GLOBAL_CONSTRAINT      // 全局约束 (在命名空间内全局生效)
};

/**
 * 约束目标类型
 */
enum class ConstraintTargetType {
    HTML_ELEMENT,          // HTML元素 (如span, div)
    TEMPLATE_OBJECT,       // 模板对象 ([Template] @Element/@Style/@Var)
    CUSTOM_OBJECT,         // 自定义对象 ([Custom] @Element/@Style/@Var)
    ORIGIN_OBJECT,         // 原始嵌入对象 ([Origin] @Html/@Style/@JavaScript)
    HTML_TYPE,            // @Html类型
    TEMPLATE_TYPE,        // [Template]类型
    CUSTOM_TYPE,          // [Custom]类型
    ORIGIN_TYPE,          // [Origin]类型
    TEMPLATE_VAR_TYPE,    // [Template] @Var类型
    CUSTOM_VAR_TYPE,      // [Custom] @Var类型
    TEMPLATE_STYLE_TYPE,  // [Template] @Style类型
    CUSTOM_STYLE_TYPE,    // [Custom] @Style类型
    TEMPLATE_ELEMENT_TYPE, // [Template] @Element类型
    CUSTOM_ELEMENT_TYPE   // [Custom] @Element类型
};

/**
 * 约束项
 */
struct ConstraintItem {
    ConstraintTargetType targetType;   // 目标类型
    std::string targetName;            // 目标名称（如果是精确约束）
    std::string targetSubType;         // 目标子类型（如@Element, @Style等）
    
    ConstraintItem() : targetType(ConstraintTargetType::HTML_ELEMENT) {}
    
    ConstraintItem(ConstraintTargetType type, const std::string& name = "", const std::string& subType = "")
        : targetType(type), targetName(name), targetSubType(subType) {}
    
    /**
     * 检查是否匹配目标
     */
    bool matches(ConstraintTargetType type, const std::string& name = "", const std::string& subType = "") const;
    
    /**
     * 转换为字符串表示
     */
    std::string toString() const;
};

/**
 * 约束节点
 * 表示CHTL中的except约束
 */
class ConstraintNode : public BaseNode {
public:
    /**
     * 构造函数
     */
    ConstraintNode(ConstraintType constraintType = ConstraintType::EXACT_CONSTRAINT);
    
    /**
     * 析构函数
     */
    virtual ~ConstraintNode() = default;
    
    /**
     * 获取约束类型
     */
    ConstraintType getConstraintType() const;
    
    /**
     * 设置约束类型
     */
    void setConstraintType(ConstraintType type);
    
    /**
     * 添加约束项
     */
    void addConstraintItem(const ConstraintItem& item);
    
    /**
     * 移除约束项
     */
    bool removeConstraintItem(const ConstraintItem& item);
    
    /**
     * 获取所有约束项
     */
    const std::vector<ConstraintItem>& getConstraintItems() const;
    
    /**
     * 检查是否约束了指定目标
     */
    bool isConstraining(ConstraintTargetType targetType, const std::string& targetName = "", 
                       const std::string& targetSubType = "") const;
    
    /**
     * 检查是否为全局约束
     */
    bool isGlobalConstraint() const;
    
    /**
     * 设置为全局约束
     */
    void setAsGlobalConstraint(bool global);
    
    /**
     * 获取约束作用域
     */
    const std::string& getConstraintScope() const;
    
    /**
     * 设置约束作用域
     */
    void setConstraintScope(const std::string& scope);
    
    /**
     * 验证约束的有效性
     */
    bool validateConstraint() const;
    
    // BaseNode 接口实现
    CHTLNodeType getNodeType() const { return CHTLNodeType::CONSTRAINT_NODE; }
    std::string toString() const override;
    NodePtr clone() const override;
    bool validate(ErrorReporter* errorReporter = nullptr) const override;

private:
    ConstraintType m_constraintType;           // 约束类型
    std::vector<ConstraintItem> m_constraintItems; // 约束项列表
    bool m_isGlobal;                          // 是否为全局约束
    std::string m_constraintScope;            // 约束作用域
    
    /**
     * 解析约束目标类型
     */
    ConstraintTargetType parseTargetType(const std::string& typeString) const;
};

/**
 * 约束管理器
 * 负责管理和验证约束
 */
class ConstraintManager {
public:
    /**
     * 构造函数
     */
    ConstraintManager();
    
    /**
     * 析构函数
     */
    ~ConstraintManager() = default;
    
    /**
     * 添加约束
     */
    void addConstraint(std::shared_ptr<ConstraintNode> constraint, const std::string& scope = "");
    
    /**
     * 移除约束
     */
    bool removeConstraint(std::shared_ptr<ConstraintNode> constraint, const std::string& scope = "");
    
    /**
     * 检查是否被约束
     */
    bool isConstrained(ConstraintTargetType targetType, const std::string& targetName = "", 
                      const std::string& targetSubType = "", const std::string& scope = "") const;
    
    /**
     * 获取作用域内的约束
     */
    std::vector<std::shared_ptr<ConstraintNode>> getConstraintsInScope(const std::string& scope) const;
    
    /**
     * 获取全局约束
     */
    std::vector<std::shared_ptr<ConstraintNode>> getGlobalConstraints() const;
    
    /**
     * 清除所有约束
     */
    void clearAllConstraints();
    
    /**
     * 清除作用域约束
     */
    void clearScopeConstraints(const std::string& scope);
    
    /**
     * 验证节点是否违反约束
     */
    bool validateNode(std::shared_ptr<BaseNode> node, const std::string& scope = "") const;
    
    /**
     * 获取违反的约束
     */
    std::vector<std::shared_ptr<ConstraintNode>> getViolatedConstraints(
        ConstraintTargetType targetType, const std::string& targetName = "", 
        const std::string& targetSubType = "", const std::string& scope = "") const;
    
    /**
     * 生成约束报告
     */
    std::string generateConstraintReport() const;
    
    /**
     * 设置错误报告器
     */
    void setErrorReporter(ErrorReporter* errorReporter);

private:
    std::unordered_map<std::string, std::vector<std::shared_ptr<ConstraintNode>>> m_scopeConstraints; // 作用域约束
    std::vector<std::shared_ptr<ConstraintNode>> m_globalConstraints; // 全局约束
    ErrorReporter* m_errorReporter;                                   // 错误报告器
    
    /**
     * 检查作用域约束
     */
    bool checkScopeConstraints(ConstraintTargetType targetType, const std::string& targetName,
                              const std::string& targetSubType, const std::string& scope) const;
    
    /**
     * 检查全局约束
     */
    bool checkGlobalConstraints(ConstraintTargetType targetType, const std::string& targetName,
                               const std::string& targetSubType) const;
    
    /**
     * 从节点推断目标类型
     */
    ConstraintTargetType inferTargetTypeFromNode(std::shared_ptr<BaseNode> node) const;
    
    /**
     * 获取节点名称
     */
    std::string getNodeName(std::shared_ptr<BaseNode> node) const;
    
    /**
     * 获取节点子类型
     */
    std::string getNodeSubType(std::shared_ptr<BaseNode> node) const;
};

/**
 * 约束验证器
 * 用于在解析过程中验证约束
 */
class ConstraintValidator {
public:
    /**
     * 构造函数
     */
    ConstraintValidator(ConstraintManager* manager);
    
    /**
     * 析构函数
     */
    ~ConstraintValidator() = default;
    
    /**
     * 验证HTML元素
     */
    bool validateHtmlElement(const std::string& elementName, const std::string& scope = "") const;
    
    /**
     * 验证模板使用
     */
    bool validateTemplateUsage(const std::string& templateType, const std::string& templateName, 
                              const std::string& scope = "") const;
    
    /**
     * 验证自定义使用
     */
    bool validateCustomUsage(const std::string& customType, const std::string& customName, 
                            const std::string& scope = "") const;
    
    /**
     * 验证原始嵌入使用
     */
    bool validateOriginUsage(const std::string& originType, const std::string& originName, 
                            const std::string& scope = "") const;
    
    /**
     * 验证类型使用
     */
    bool validateTypeUsage(const std::string& typeString, const std::string& scope = "") const;
    
    /**
     * 设置当前作用域
     */
    void setCurrentScope(const std::string& scope);
    
    /**
     * 获取当前作用域
     */
    const std::string& getCurrentScope() const;

private:
    ConstraintManager* m_constraintManager;  // 约束管理器
    std::string m_currentScope;              // 当前作用域
    
    /**
     * 解析类型字符串到目标类型
     */
    ConstraintTargetType parseTypeString(const std::string& typeString) const;
};

} // namespace CHTL