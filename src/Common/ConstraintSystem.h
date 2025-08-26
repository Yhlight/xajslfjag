#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <functional>

namespace CHTL {
namespace Common {

// 约束类型
enum class ConstraintType {
    HTML_ELEMENT_CONSTRAINT,    // HTML元素约束
    CUSTOM_OBJECT_CONSTRAINT,   // 自定义/模板对象约束
    TYPE_CONSTRAINT,            // 类型约束 (@Html, [Custom]等)
    GLOBAL_CONSTRAINT,          // 全局约束
    NAMESPACE_CONSTRAINT,       // 命名空间约束
    SYNTAX_CONSTRAINT          // 语法约束
};

// 约束作用域
enum class ConstraintScope {
    LOCAL,          // 局部约束
    NAMESPACE,      // 命名空间约束
    GLOBAL,         // 全局约束
    INHERITABLE     // 可继承约束
};

// 约束级别
enum class ConstraintLevel {
    ERROR,          // 错误级别，阻止编译
    WARNING,        // 警告级别，允许编译但给出警告
    INFO            // 信息级别，仅提示
};

// 约束条件
struct ConstraintCondition {
    std::string name;               // 约束名称
    ConstraintType type;            // 约束类型
    ConstraintScope scope;          // 约束作用域
    ConstraintLevel level;          // 约束级别
    std::vector<std::string> targets;      // 约束目标
    std::vector<std::string> exceptions;   // 例外列表(except关键字)
    std::unordered_map<std::string, std::string> parameters;  // 约束参数
    
    // 验证函数
    std::function<bool(const std::string&, const std::unordered_map<std::string, std::string>&)> validator;
    
    ConstraintCondition() : type(ConstraintType::SYNTAX_CONSTRAINT), 
                           scope(ConstraintScope::LOCAL), 
                           level(ConstraintLevel::ERROR) {}
};

// 约束违规信息
struct ConstraintViolation {
    std::string message;
    std::string target;
    std::string context;
    ConstraintLevel level;
    size_t position;
    std::string suggested_fix;
    
    ConstraintViolation(const std::string& msg, const std::string& tgt, 
                       ConstraintLevel lvl, size_t pos = 0) 
        : message(msg), target(tgt), level(lvl), position(pos) {}
};

// 约束验证结果
struct ConstraintValidationResult {
    bool is_valid;
    std::vector<ConstraintViolation> violations;
    std::vector<std::string> warnings;
    std::vector<std::string> suggestions;
    
    ConstraintValidationResult() : is_valid(true) {}
    
    void addViolation(const ConstraintViolation& violation) {
        violations.push_back(violation);
        if (violation.level == ConstraintLevel::ERROR) {
            is_valid = false;
        }
    }
};

// 约束上下文
class ConstraintContext {
public:
    std::string current_namespace;
    std::string current_element;
    std::string current_context_type;  // style, script, template, custom等
    std::unordered_map<std::string, std::string> context_data;
    std::vector<std::string> active_constraints;
    
    void setContext(const std::string& type, const std::string& element = "");
    void addContextData(const std::string& key, const std::string& value);
    std::string getContextData(const std::string& key, const std::string& default_val = "") const;
    bool hasConstraint(const std::string& constraint_name) const;
};

// 约束系统管理器
class ConstraintSystem {
private:
    std::unordered_map<std::string, ConstraintCondition> constraints;
    std::unordered_map<std::string, std::vector<std::string>> namespace_constraints;
    std::unordered_map<std::string, std::vector<std::string>> global_constraints;
    std::vector<ConstraintViolation> violation_history;
    
    // 内置约束
    void initializeBuiltinConstraints();
    void setupHTMLElementConstraints();
    void setupTemplateConstraints();
    void setupCustomElementConstraints();
    void setupImportConstraints();
    void setupSyntaxConstraints();
    
public:
    ConstraintSystem();
    ~ConstraintSystem() = default;
    
    // 约束管理
    void addConstraint(const ConstraintCondition& constraint);
    void removeConstraint(const std::string& name);
    bool hasConstraint(const std::string& name) const;
    const ConstraintCondition* getConstraint(const std::string& name) const;
    
    // 例外处理 (except关键字)
    void addException(const std::string& constraint_name, const std::string& exception);
    void removeException(const std::string& constraint_name, const std::string& exception);
    bool isException(const std::string& constraint_name, const std::string& target) const;
    
    // 命名空间约束
    void addNamespaceConstraint(const std::string& namespace_name, const std::string& constraint_name);
    void removeNamespaceConstraint(const std::string& namespace_name, const std::string& constraint_name);
    std::vector<std::string> getNamespaceConstraints(const std::string& namespace_name) const;
    
    // 全局约束
    void addGlobalConstraint(const std::string& constraint_name);
    void removeGlobalConstraint(const std::string& constraint_name);
    std::vector<std::string> getGlobalConstraints() const;
    
    // 约束验证
    ConstraintValidationResult validateElement(const std::string& element_name, 
                                              const std::string& context_type,
                                              const ConstraintContext& context) const;
    
    ConstraintValidationResult validateAttribute(const std::string& element_name,
                                               const std::string& attribute_name,
                                               const std::string& attribute_value,
                                               const ConstraintContext& context) const;
    
    ConstraintValidationResult validateSyntax(const std::string& syntax_element,
                                            const std::string& usage_context,
                                            const ConstraintContext& context) const;
    
    ConstraintValidationResult validateType(const std::string& type_name,
                                          const std::string& usage_context,
                                          const ConstraintContext& context) const;
    
    // 批量验证
    ConstraintValidationResult validateAll(const std::vector<std::string>& elements,
                                         const ConstraintContext& context) const;
    
    // 约束解析 (处理except语句)
    void parseExceptStatement(const std::string& statement);
    void parseConstraintDefinition(const std::string& definition);
    
    // 错误和建议
    std::vector<ConstraintViolation> getViolationHistory() const;
    void clearViolationHistory();
    std::vector<std::string> getSuggestions(const std::string& target, 
                                          const std::string& context) const;
    
    // 约束激活/禁用
    void enableConstraint(const std::string& name);
    void disableConstraint(const std::string& name);
    bool isConstraintEnabled(const std::string& name) const;
    
    // 约束报告
    std::string generateConstraintReport() const;
    std::string generateViolationSummary() const;
};

// 约束工具类
class ConstraintUtils {
public:
    // HTML元素验证
    static bool isValidHTMLElement(const std::string& element_name);
    static bool isValidHTMLAttribute(const std::string& element_name, const std::string& attribute_name);
    static std::vector<std::string> getValidAttributesForElement(const std::string& element_name);
    
    // 类型验证
    static bool isValidCustomType(const std::string& type_name);
    static bool isValidTemplateType(const std::string& type_name);
    static bool isValidOriginType(const std::string& type_name);
    
    // 语法验证
    static bool isValidIdentifier(const std::string& identifier);
    static bool isValidSelector(const std::string& selector);
    static bool isValidCSSProperty(const std::string& property);
    
    // 约束匹配
    static bool matchesPattern(const std::string& target, const std::string& pattern);
    static std::vector<std::string> extractExceptions(const std::string& except_statement);
    
    // 建议生成
    static std::string suggestValidElement(const std::string& invalid_element);
    static std::string suggestValidAttribute(const std::string& element, const std::string& invalid_attr);
    static std::string suggestValidType(const std::string& invalid_type);
};

// 预定义约束常量
namespace PredefinedConstraints {
    extern const std::string HTML_ELEMENT_WHITELIST;
    extern const std::string TEMPLATE_TYPE_CONSTRAINT;
    extern const std::string CUSTOM_TYPE_CONSTRAINT;
    extern const std::string IMPORT_TYPE_CONSTRAINT;
    extern const std::string NAMESPACE_NAMING_CONSTRAINT;
    extern const std::string SELECTOR_SYNTAX_CONSTRAINT;
    extern const std::string ATTRIBUTE_VALIDATION_CONSTRAINT;
}

} // namespace Common
} // namespace CHTL