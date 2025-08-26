#pragma once

#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <memory>

namespace CHTL {
namespace Common {

// 语法上下文类型
enum class SyntaxContext {
    GLOBAL_STYLE_BLOCK,     // 全局样式块
    LOCAL_STYLE_BLOCK,      // 局部样式块
    LOCAL_SCRIPT_BLOCK,     // 局部脚本块
    GLOBAL_SCRIPT_BLOCK,    // 全局脚本块（除局部script外的其他script）
    HTML_ELEMENT,           // HTML元素上下文
    TEMPLATE_BLOCK,         // 模板块
    CUSTOM_BLOCK,           // 自定义块
    CONFIGURATION,          // 配置块
    NAMESPACE,              // 命名空间
    IMPORT,                 // 导入块
    TEXT_NODE,              // 文本节点
    ROOT_LEVEL,             // 根级别
    UNKNOWN
};

// CHTL语法特性
enum class CHTLSyntaxFeature {
    // 变量系统
    TEMPLATE_VARIABLE,      // 模板变量的使用
    CUSTOM_VARIABLE,        // 自定义变量的使用
    VARIABLE_SPECIALIZATION,// 自定义变量的特例化
    
    // 模板系统
    TEMPLATE_STYLE,         // 模板样式组
    TEMPLATE_ELEMENT,       // 模板元素
    TEMPLATE_VAR,           // 模板变量组
    
    // 自定义系统
    CUSTOM_STYLE,           // 自定义样式组
    CUSTOM_ELEMENT,         // 自定义元素
    CUSTOM_VAR,             // 自定义变量组
    CUSTOM_STYLE_SPECIALIZATION, // 自定义样式组的特例化
    
    // 无值样式组
    NO_VALUE_STYLE_GROUP,   // 无值样式组
    
    // 操作符
    DELETE_PROPERTY,        // delete属性
    DELETE_INHERITANCE,     // delete继承
    INHERITANCE,            // 继承(样式组之间的继承)
    
    // 特殊功能
    GENERATOR_COMMENT,      // 生成器注释 --
    FULL_QUALIFIED_NAME,    // 全缀名
    ORIGIN_EMBED,           // 原始嵌入(任意类型)
    NAMESPACE_FROM,         // 从命名空间中获取(from语法)
    
    // CHTL JS特有语法
    ENHANCED_SELECTOR,      // {{&}} 等特供语法
    VIR_OBJECT,             // vir虚对象
    LISTEN_FUNCTION,        // listen函数
    DELEGATE_FUNCTION,      // delegate函数
    ANIMATE_FUNCTION,       // animate函数
    ARROW_OPERATOR,         // -> 操作符
    
    UNKNOWN_FEATURE
};

// 约束规则
struct ConstraintRule {
    SyntaxContext context;
    std::unordered_set<CHTLSyntaxFeature> allowed_features;
    std::unordered_set<CHTLSyntaxFeature> forbidden_features;
    std::string description;
    bool is_strict;
    
    ConstraintRule() : context(SyntaxContext::UNKNOWN), is_strict(true) {}
    
    ConstraintRule(SyntaxContext ctx, const std::string& desc)
        : context(ctx), description(desc), is_strict(true) {}
    
    void allowFeature(CHTLSyntaxFeature feature) {
        allowed_features.insert(feature);
        forbidden_features.erase(feature);
    }
    
    void forbidFeature(CHTLSyntaxFeature feature) {
        forbidden_features.insert(feature);
        allowed_features.erase(feature);
    }
    
    bool isFeatureAllowed(CHTLSyntaxFeature feature) const {
        if (!forbidden_features.empty() && forbidden_features.count(feature)) {
            return false;
        }
        return allowed_features.empty() || allowed_features.count(feature);
    }
};

// 约束违反信息
struct ConstraintViolation {
    SyntaxContext context;
    CHTLSyntaxFeature feature;
    std::string location;
    size_t line;
    size_t column;
    std::string message;
    std::string suggestion;
    
    ConstraintViolation() : context(SyntaxContext::UNKNOWN), 
                           feature(CHTLSyntaxFeature::UNKNOWN_FEATURE), 
                           line(0), column(0) {}
    
    ConstraintViolation(SyntaxContext ctx, CHTLSyntaxFeature feat, 
                       const std::string& loc, size_t l, size_t c, const std::string& msg)
        : context(ctx), feature(feat), location(loc), line(l), column(c), message(msg) {}
    
    std::string toString() const {
        return location + ":" + std::to_string(line) + ":" + std::to_string(column) + " - " + message;
    }
};

// 语法约束检查结果
struct ConstraintCheckResult {
    std::vector<ConstraintViolation> violations;
    std::vector<std::string> warnings;
    bool is_valid;
    size_t features_checked;
    
    ConstraintCheckResult() : is_valid(true), features_checked(0) {}
    
    void addViolation(const ConstraintViolation& violation) {
        violations.push_back(violation);
        is_valid = false;
    }
    
    void addWarning(const std::string& warning) {
        warnings.push_back(warning);
    }
    
    bool hasViolations() const { return !violations.empty(); }
    bool hasWarnings() const { return !warnings.empty(); }
    size_t getViolationCount() const { return violations.size(); }
};

// CHTL语法约束器
class SyntaxConstraints {
public:
    SyntaxConstraints();
    ~SyntaxConstraints();
    
    // 初始化约束规则
    void initializeStandardRules();
    void initializeStrictRules();
    void initializeCustomRules();
    
    // 约束检查
    
    // 检查整个源代码的语法约束
    ConstraintCheckResult checkSourceConstraints(const std::string& source_code, 
                                                 const std::string& file_path = "");
    
    // 检查特定上下文中的语法特性
    bool checkFeatureInContext(CHTLSyntaxFeature feature, SyntaxContext context) const;
    
    // 检查语法片段的合法性
    ConstraintCheckResult checkFragment(const std::string& fragment, SyntaxContext context,
                                       size_t line_offset = 0, size_t column_offset = 0);
    
    // 上下文检测
    
    // 检测当前位置的语法上下文
    SyntaxContext detectContext(const std::string& source, size_t position) const;
    
    // 检查是否在特定上下文中
    bool isInGlobalStyleBlock(const std::string& source, size_t position) const;
    bool isInLocalStyleBlock(const std::string& source, size_t position) const;
    bool isInLocalScriptBlock(const std::string& source, size_t position) const;
    bool isInGlobalScriptBlock(const std::string& source, size_t position) const;
    
    // 语法特性检测
    
    // 检测代码中使用的CHTL语法特性
    std::vector<CHTLSyntaxFeature> detectUsedFeatures(const std::string& code) const;
    
    // 检查是否使用了模板变量
    bool usesTemplateVariable(const std::string& code) const;
    
    // 检查是否使用了自定义变量
    bool usesCustomVariable(const std::string& code) const;
    
    // 检查是否使用了CHTL JS特有语法
    bool usesCHTLJSFeatures(const std::string& code) const;
    
    // 检查是否使用了原始嵌入
    bool usesOriginEmbed(const std::string& code) const;
    
    // 检查是否使用了生成器注释
    bool usesGeneratorComment(const std::string& code) const;
    
    // 约束规则管理
    
    // 添加约束规则
    void addConstraintRule(const ConstraintRule& rule);
    
    // 移除约束规则
    void removeConstraintRule(SyntaxContext context);
    
    // 获取上下文的约束规则
    ConstraintRule getConstraintRule(SyntaxContext context) const;
    
    // 修改约束规则
    void modifyConstraintRule(SyntaxContext context, CHTLSyntaxFeature feature, bool allow);
    
    // 预设约束配置
    
    // 应用标准CHTL约束配置（严格按照目标规划文档）
    void applyStandardCHTLConstraints();
    
    // 应用开发模式约束（相对宽松）
    void applyDevelopmentConstraints();
    
    // 应用生产模式约束（最严格）
    void applyProductionConstraints();
    
    // 验证和建议
    
    // 验证语法使用的正确性
    std::vector<std::string> validateSyntaxUsage(const std::string& source) const;
    
    // 提供语法修正建议
    std::vector<std::string> getSyntaxSuggestions(const ConstraintViolation& violation) const;
    
    // 自动修正语法错误（如果可能）
    std::string autoCorrectSyntax(const std::string& source) const;
    
    // 特殊规则处理
    
    // 检查原始嵌入的任意位置使用规则
    bool validateOriginEmbedUsage(const std::string& source, size_t position) const;
    
    // 检查生成器注释的任意位置使用规则
    bool validateGeneratorCommentUsage(const std::string& source, size_t position) const;
    
    // 检查CHTL JS特供语法（{{&}}等）的使用
    bool validateCHTLJSSpecialSyntax(const std::string& source, size_t position) const;
    
    // 统计和分析
    
    // 获取约束违反统计
    std::unordered_map<SyntaxContext, size_t> getViolationStatistics(
        const ConstraintCheckResult& result) const;
    
    // 分析语法特性使用情况
    std::unordered_map<CHTLSyntaxFeature, size_t> analyzeFeatureUsage(const std::string& source) const;
    
    // 调试和诊断
    void dumpConstraintRules() const;
    void dumpViolations(const ConstraintCheckResult& result) const;
    std::string generateConstraintReport(const ConstraintCheckResult& result) const;
    
    // 配置
    void setStrictMode(bool strict) { strict_mode = strict; }
    void setAllowExperimental(bool allow) { allow_experimental = allow; }
    bool isStrictMode() const { return strict_mode; }

private:
    std::unordered_map<SyntaxContext, ConstraintRule> constraint_rules;
    bool strict_mode;
    bool allow_experimental;
    
    // 内部方法
    
    // 上下文分析
    SyntaxContext analyzeContextAtPosition(const std::string& source, size_t position) const;
    std::pair<size_t, size_t> findEnclosingBlock(const std::string& source, size_t position) const;
    std::string getBlockType(const std::string& source, size_t block_start) const;
    
    // 语法特性检测
    std::vector<CHTLSyntaxFeature> scanForFeatures(const std::string& code) const;
    bool matchesFeaturePattern(const std::string& code, CHTLSyntaxFeature feature) const;
    
    // 模式匹配
    bool matchesTemplateVariable(const std::string& code, size_t position) const;
    bool matchesCustomVariable(const std::string& code, size_t position) const;
    bool matchesOriginEmbed(const std::string& code, size_t position) const;
    bool matchesGeneratorComment(const std::string& code, size_t position) const;
    bool matchesCHTLJSSelector(const std::string& code, size_t position) const;
    
    // 规则初始化
    void initializeGlobalStyleRules();
    void initializeLocalStyleRules();
    void initializeLocalScriptRules();
    void initializeGlobalScriptRules();
    
    // 违反检测
    std::vector<ConstraintViolation> findViolationsInContext(const std::string& code, 
                                                            SyntaxContext context,
                                                            size_t line_offset,
                                                            size_t column_offset) const;
    
    // 工具方法
    std::string getContextName(SyntaxContext context) const;
    std::string getFeatureName(CHTLSyntaxFeature feature) const;
    std::pair<size_t, size_t> getLineColumn(const std::string& source, size_t position) const;
    
    // 建议生成
    std::string generateSuggestion(CHTLSyntaxFeature feature, SyntaxContext context) const;
    std::vector<SyntaxContext> getValidContextsForFeature(CHTLSyntaxFeature feature) const;
};

// 语法约束工具类
class ConstraintUtils {
public:
    // 获取语法上下文名称
    static std::string getContextName(SyntaxContext context);
    
    // 获取语法特性名称
    static std::string getFeatureName(CHTLSyntaxFeature feature);
    
    // 检查特性是否是CHTL JS特有的
    static bool isCHTLJSFeature(CHTLSyntaxFeature feature);
    
    // 检查特性是否可以在任意位置使用
    static bool isUniversalFeature(CHTLSyntaxFeature feature);
    
    // 获取特性的推荐使用上下文
    static std::vector<SyntaxContext> getRecommendedContexts(CHTLSyntaxFeature feature);
    
    // 生成约束违反的详细说明
    static std::string generateViolationExplanation(CHTLSyntaxFeature feature, 
                                                    SyntaxContext context);
    
    // 检查两个上下文是否兼容
    static bool areContextsCompatible(SyntaxContext from, SyntaxContext to);
    
    // 获取上下文的层级关系
    static int getContextLevel(SyntaxContext context);
};

} // namespace Common
} // namespace CHTL