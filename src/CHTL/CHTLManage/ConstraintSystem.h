#ifndef CHTL_CONSTRAINT_SYSTEM_H
#define CHTL_CONSTRAINT_SYSTEM_H

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <functional>

namespace CHTL {

// 约束类型
enum class ConstraintType {
    SYNTAX_BOUNDARY,      // 语法边界约束
    CONTEXT_RESTRICTION,  // 上下文限制
    NODE_EXCLUSION,      // 节点排除约束
    SEQUENCE_ORDER,      // 序列顺序约束
    NESTING_LIMIT,       // 嵌套限制
    ATTRIBUTE_REQUIREMENT, // 属性要求
    VALUE_CONSTRAINT     // 值约束
};

// 约束级别
enum class ConstraintLevel {
    ERROR,      // 错误级别，必须满足
    WARNING,    // 警告级别，建议满足
    INFO        // 信息级别，仅提示
};

// 约束结果
struct ConstraintResult {
    bool satisfied = true;
    ConstraintLevel level = ConstraintLevel::INFO;
    std::string message;
    size_t line = 0;
    size_t column = 0;
    std::string context;
};

// 基础约束类
class Constraint {
public:
    Constraint(const std::string& name, ConstraintType type, ConstraintLevel level)
        : name_(name), type_(type), level_(level) {}
    
    virtual ~Constraint() = default;
    
    // 检查约束
    virtual ConstraintResult check(const void* context) const = 0;
    
    // 获取约束信息
    const std::string& getName() const { return name_; }
    ConstraintType getType() const { return type_; }
    ConstraintLevel getLevel() const { return level_; }
    
    // 启用/禁用约束
    void setEnabled(bool enabled) { enabled_ = enabled; }
    bool isEnabled() const { return enabled_; }

protected:
    std::string name_;
    ConstraintType type_;
    ConstraintLevel level_;
    bool enabled_ = true;
};

// 语法边界约束
class SyntaxBoundaryConstraint : public Constraint {
public:
    SyntaxBoundaryConstraint(const std::string& name, ConstraintLevel level = ConstraintLevel::ERROR)
        : Constraint(name, ConstraintType::SYNTAX_BOUNDARY, level) {}
    
    // 设置允许的前置节点类型
    void setAllowedPredecessors(const std::unordered_set<std::string>& types) {
        allowedPredecessors_ = types;
    }
    
    // 设置允许的后置节点类型
    void setAllowedSuccessors(const std::unordered_set<std::string>& types) {
        allowedSuccessors_ = types;
    }
    
    // 设置允许的父节点类型
    void setAllowedParents(const std::unordered_set<std::string>& types) {
        allowedParents_ = types;
    }
    
    // 设置允许的子节点类型
    void setAllowedChildren(const std::unordered_set<std::string>& types) {
        allowedChildren_ = types;
    }
    
    ConstraintResult check(const void* context) const override;

private:
    std::unordered_set<std::string> allowedPredecessors_;
    std::unordered_set<std::string> allowedSuccessors_;
    std::unordered_set<std::string> allowedParents_;
    std::unordered_set<std::string> allowedChildren_;
};

// except约束实现
class ExceptConstraint : public Constraint {
public:
    ExceptConstraint(const std::string& name)
        : Constraint(name, ConstraintType::NODE_EXCLUSION, ConstraintLevel::ERROR) {}
    
    // 添加排除的节点类型
    void addExcludedType(const std::string& type) {
        excludedTypes_.insert(type);
    }
    
    // 添加排除的属性
    void addExcludedAttribute(const std::string& attr) {
        excludedAttributes_.insert(attr);
    }
    
    // 添加排除的值
    void addExcludedValue(const std::string& value) {
        excludedValues_.insert(value);
    }
    
    ConstraintResult check(const void* context) const override;

private:
    std::unordered_set<std::string> excludedTypes_;
    std::unordered_set<std::string> excludedAttributes_;
    std::unordered_set<std::string> excludedValues_;
};

// 约束管理器
class ConstraintManager {
public:
    static ConstraintManager& getInstance() {
        static ConstraintManager instance;
        return instance;
    }
    
    // 注册约束
    void registerConstraint(std::shared_ptr<Constraint> constraint);
    
    // 获取约束
    std::shared_ptr<Constraint> getConstraint(const std::string& name) const;
    
    // 检查所有约束
    std::vector<ConstraintResult> checkAll(const void* context) const;
    
    // 检查特定类型的约束
    std::vector<ConstraintResult> checkByType(ConstraintType type, const void* context) const;
    
    // 启用/禁用约束
    void enableConstraint(const std::string& name, bool enable);
    
    // 清空所有约束
    void clearAll();
    
    // 加载预定义约束
    void loadPredefinedConstraints();

private:
    ConstraintManager() = default;
    ConstraintManager(const ConstraintManager&) = delete;
    ConstraintManager& operator=(const ConstraintManager&) = delete;
    
    std::unordered_map<std::string, std::shared_ptr<Constraint>> constraints_;
    
    // 创建预定义约束
    void createCHTLConstraints();
    void createCHTLJSConstraints();
    void createImportConstraints();
    void createOriginConstraints();
};

// CHTL特定约束
namespace CHTLConstraints {
    // 模板约束
    std::shared_ptr<Constraint> createTemplateConstraint();
    
    // 自定义元素约束
    std::shared_ptr<Constraint> createCustomElementConstraint();
    
    // 局部样式块约束
    std::shared_ptr<Constraint> createLocalStyleConstraint();
    
    // 局部脚本块约束
    std::shared_ptr<Constraint> createLocalScriptConstraint();
    
    // 配置块约束
    std::shared_ptr<Constraint> createConfigurationConstraint();
    
    // 命名空间约束
    std::shared_ptr<Constraint> createNamespaceConstraint();
    
    // 导入约束
    std::shared_ptr<Constraint> createImportConstraint();
    
    // 原始嵌入约束
    std::shared_ptr<Constraint> createOriginConstraint();
}

// CHTL JS特定约束
namespace CHTLJSConstraints {
    // 模块导入约束
    std::shared_ptr<Constraint> createModuleImportConstraint();
    
    // 增强选择器约束
    std::shared_ptr<Constraint> createEnhancedSelectorConstraint();
    
    // 虚拟对象约束
    std::shared_ptr<Constraint> createVirtualObjectConstraint();
    
    // 事件委托约束
    std::shared_ptr<Constraint> createEventDelegationConstraint();
    
    // 动画约束
    std::shared_ptr<Constraint> createAnimationConstraint();
}

// 约束检查器
class ConstraintChecker {
public:
    ConstraintChecker();
    ~ConstraintChecker() = default;
    
    // 设置严格模式
    void setStrictMode(bool strict) { strictMode_ = strict; }
    
    // 检查AST节点
    std::vector<ConstraintResult> checkNode(const void* node);
    
    // 检查完整的AST
    std::vector<ConstraintResult> checkAST(const void* ast);
    
    // 设置自定义约束检查函数
    void setCustomChecker(std::function<std::vector<ConstraintResult>(const void*)> checker) {
        customChecker_ = checker;
    }
    
    // 获取错误数量
    size_t getErrorCount() const { return errorCount_; }
    
    // 获取警告数量
    size_t getWarningCount() const { return warningCount_; }
    
    // 重置计数器
    void resetCounters() {
        errorCount_ = 0;
        warningCount_ = 0;
    }

private:
    bool strictMode_ = false;
    size_t errorCount_ = 0;
    size_t warningCount_ = 0;
    std::function<std::vector<ConstraintResult>(const void*)> customChecker_;
    
    // 更新计数器
    void updateCounters(const std::vector<ConstraintResult>& results);
};

// 约束构建器（流式API）
class ConstraintBuilder {
public:
    ConstraintBuilder(const std::string& name) : name_(name) {}
    
    // 设置约束类型
    ConstraintBuilder& ofType(ConstraintType type) {
        type_ = type;
        return *this;
    }
    
    // 设置约束级别
    ConstraintBuilder& withLevel(ConstraintLevel level) {
        level_ = level;
        return *this;
    }
    
    // 添加语法边界规则
    ConstraintBuilder& allowedBefore(const std::string& type) {
        allowedBefore_.insert(type);
        return *this;
    }
    
    ConstraintBuilder& allowedAfter(const std::string& type) {
        allowedAfter_.insert(type);
        return *this;
    }
    
    ConstraintBuilder& allowedParent(const std::string& type) {
        allowedParents_.insert(type);
        return *this;
    }
    
    ConstraintBuilder& allowedChild(const std::string& type) {
        allowedChildren_.insert(type);
        return *this;
    }
    
    // 添加排除规则
    ConstraintBuilder& except(const std::string& type) {
        exceptTypes_.insert(type);
        return *this;
    }
    
    ConstraintBuilder& exceptAttribute(const std::string& attr) {
        exceptAttributes_.insert(attr);
        return *this;
    }
    
    // 构建约束
    std::shared_ptr<Constraint> build();

private:
    std::string name_;
    ConstraintType type_ = ConstraintType::SYNTAX_BOUNDARY;
    ConstraintLevel level_ = ConstraintLevel::ERROR;
    
    std::unordered_set<std::string> allowedBefore_;
    std::unordered_set<std::string> allowedAfter_;
    std::unordered_set<std::string> allowedParents_;
    std::unordered_set<std::string> allowedChildren_;
    std::unordered_set<std::string> exceptTypes_;
    std::unordered_set<std::string> exceptAttributes_;
};

} // namespace CHTL

#endif // CHTL_CONSTRAINT_SYSTEM_H