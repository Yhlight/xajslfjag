#pragma once

#include "BaseNode.h"
#include "../../Util/Common.h"

namespace CHTL {

// 约束类型枚举
enum class ConstraintType {
    PRECISE,    // 精确约束 - 作用于HTML元素、自定义与模板对象
    TYPE,       // 类型约束 - 作用于@Html、[Custom]、[Template]
    GLOBAL      // 全局约束 - 命名空间内使用
};

// 约束目标类型
enum class ConstraintTargetType {
    HTML_ELEMENT,       // HTML元素 (div, span等)
    CUSTOM_ELEMENT,     // 自定义元素 [Custom] @Element
    CUSTOM_STYLE,       // 自定义样式组 [Custom] @Style
    CUSTOM_VAR,         // 自定义变量组 [Custom] @Var
    TEMPLATE_ELEMENT,   // 模板元素 [Template] @Element
    TEMPLATE_STYLE,     // 模板样式组 [Template] @Style
    TEMPLATE_VAR,       // 模板变量组 [Template] @Var
    ORIGIN_HTML,        // 原始HTML嵌入 @Html
    ORIGIN_STYLE,       // 原始样式嵌入 @Style
    ORIGIN_JAVASCRIPT,  // 原始JS嵌入 @JavaScript
    ORIGIN_CUSTOM,      // 自定义原始嵌入
    CUSTOM_TYPE,        // [Custom]类型
    TEMPLATE_TYPE       // [Template]类型
};

// 约束目标
struct ConstraintTarget {
    ConstraintTargetType targetType;
    String name;               // 具体名称 (如元素名、模板名等)
    String customType;         // 自定义类型名 (如@Vue)
    
    ConstraintTarget(ConstraintTargetType type, const String& targetName = "", const String& customTypeName = "")
        : targetType(type), name(targetName), customType(customTypeName) {}
        
    bool operator==(const ConstraintTarget& other) const {
        return targetType == other.targetType && name == other.name && customType == other.customType;
    }
};

// 约束节点
class ConstraintNode : public BaseNode {
public:
    ConstraintType constraintType;
    std::vector<ConstraintTarget> targets;
    String scope;  // 约束作用域 (for global constraints)
    
    explicit ConstraintNode(ConstraintType type, const Position& pos = Position());
    ~ConstraintNode() override = default;
    
    // 添加约束目标
    void addTarget(const ConstraintTarget& target);
    void addTarget(ConstraintTargetType targetType, const String& name = "", const String& customType = "");
    
    // 检查是否约束了指定目标
    bool constrains(const ConstraintTarget& target) const;
    bool constrainsHTMLElement(const String& elementName) const;
    bool constrainsCustomElement(const String& elementName) const;
    bool constrainsCustomStyle(const String& styleName) const;
    bool constrainsTemplateType() const;
    bool constrainsCustomType() const;
    bool constrainsOriginType(const String& originType) const;
    
    // 验证约束
    bool validateTarget(const BaseNode* node) const;
    bool validateInScope(const String& currentScope) const;
    
    // 获取约束描述
    String getConstraintDescription() const;
    StringVector getTargetNames() const;
    
    // 序列化
    virtual String toString() const override;
    String toHTML() const;
    
    // 静态工厂方法
    static std::unique_ptr<ConstraintNode> createPreciseConstraint(const std::vector<ConstraintTarget>& targets, const Position& pos = Position());
    static std::unique_ptr<ConstraintNode> createTypeConstraint(const std::vector<ConstraintTarget>& targets, const Position& pos = Position());
    static std::unique_ptr<ConstraintNode> createGlobalConstraint(const std::vector<ConstraintTarget>& targets, const String& scope, const Position& pos = Position());
    
    // 解析约束语法
    static ConstraintTarget parseConstraintTarget(const String& targetString);
    static std::vector<ConstraintTarget> parseConstraintTargets(const String& targetsString);
    
private:
    // 辅助方法
    bool matchesTarget(const ConstraintTarget& target, const BaseNode* node) const;
    String targetTypeToString(ConstraintTargetType type) const;
    ConstraintTargetType stringToTargetType(const String& typeStr) const;
};

// 约束验证器
class ConstraintValidator {
public:
    // 验证节点是否违反约束
    static bool validateNode(const BaseNode* node, const std::vector<ConstraintNode*>& constraints);
    
    // 获取适用的约束
    static std::vector<ConstraintNode*> getApplicableConstraints(const BaseNode* node, const std::vector<ConstraintNode*>& allConstraints);
    
    // 检查作用域约束
    static bool isInScope(const BaseNode* node, const ConstraintNode* constraint);
    
    // 收集当前作用域的所有约束
    static std::vector<ConstraintNode*> collectScopeConstraints(const BaseNode* currentNode, const BaseNode* rootNode);
    
    // 错误报告
    static String generateConstraintViolationMessage(const BaseNode* node, const ConstraintNode* violatedConstraint);
};

// 约束管理器
class ConstraintManager {
private:
    std::vector<std::unique_ptr<ConstraintNode>> globalConstraints;
    std::unordered_map<String, std::vector<std::unique_ptr<ConstraintNode>>> scopeConstraints;
    
public:
    // 添加约束
    void addGlobalConstraint(std::unique_ptr<ConstraintNode> constraint);
    void addScopeConstraint(const String& scope, std::unique_ptr<ConstraintNode> constraint);
    
    // 获取约束
    std::vector<ConstraintNode*> getGlobalConstraints() const;
    std::vector<ConstraintNode*> getScopeConstraints(const String& scope) const;
    std::vector<ConstraintNode*> getAllConstraints() const;
    
    // 验证
    bool validateNode(const BaseNode* node, const String& currentScope = "") const;
    StringVector getViolationMessages(const BaseNode* node, const String& currentScope = "") const;
    
    // 清理
    void clear();
    void clearScope(const String& scope);
    
    // 调试
    void printConstraints() const;
    size_t getConstraintCount() const;
};

} // namespace CHTL