#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <functional>

namespace CHTL {
namespace Common {

// 变量类型
enum class VariableType {
    STRING,         // 字符串变量
    NUMBER,         // 数值变量
    COLOR,          // 颜色变量
    LENGTH,         // 长度变量
    PERCENTAGE,     // 百分比变量
    BOOLEAN,        // 布尔变量
    ARRAY,          // 数组变量
    OBJECT,         // 对象变量
    FUNCTION,       // 函数变量
    MIXED           // 混合类型
};

// 变量定义
struct VariableDefinition {
    std::string name;                   // 变量名
    VariableType type;                  // 变量类型
    std::string value;                  // 变量值
    std::string default_value;          // 默认值
    bool is_required;                   // 是否必需
    bool is_configurable;               // 是否可配置
    std::vector<std::string> allowed_values;  // 允许的值列表
    std::unordered_map<std::string, std::string> metadata;  // 元数据
    
    VariableDefinition() : type(VariableType::STRING), is_required(false), is_configurable(true) {}
    VariableDefinition(const std::string& n, VariableType t, const std::string& v)
        : name(n), type(t), value(v), is_required(false), is_configurable(true) {}
};

// 变量组定义
struct VariableGroupDefinition {
    std::string name;                               // 变量组名
    std::string group_type;                         // 组类型 (Custom, Template)
    std::unordered_map<std::string, VariableDefinition> variables;  // 变量列表
    std::unordered_map<std::string, std::string> specializations;   // 特例化参数
    std::vector<std::string> dependencies;          // 依赖的其他变量组
    std::string source_file;                        // 定义文件
    bool is_template;                               // 是否为模板
    bool allow_specialization;                     // 是否允许特例化
    
    VariableGroupDefinition() : is_template(false), allow_specialization(true) {}
};

// 变量组实例化
struct VariableGroupInstance {
    std::string group_name;                         // 原组名
    std::string instance_name;                      // 实例名
    std::unordered_map<std::string, std::string> parameters;     // 实例化参数
    std::unordered_map<std::string, std::string> resolved_values; // 解析后的值
    bool is_specialized;                           // 是否为特例化实例
    
    VariableGroupInstance() : is_specialized(false) {}
};

// 特例化结果
struct SpecializationResult {
    bool success;                                   // 特例化是否成功
    std::string instance_id;                       // 实例ID
    std::unordered_map<std::string, std::string> resolved_variables;  // 解析后的变量
    std::vector<std::string> errors;               // 错误信息
    std::vector<std::string> warnings;             // 警告信息
    
    SpecializationResult() : success(false) {}
};

// 变量解析上下文
struct VariableResolutionContext {
    std::string current_namespace;                  // 当前命名空间
    std::string current_group;                      // 当前变量组
    std::unordered_map<std::string, std::string> context_variables;  // 上下文变量
    std::vector<std::string> resolution_stack;     // 解析栈（防止循环引用）
    
    void pushGroup(const std::string& group_name) {
        resolution_stack.push_back(group_name);
    }
    
    void popGroup() {
        if (!resolution_stack.empty()) {
            resolution_stack.pop_back();
        }
    }
    
    bool hasCircularReference(const std::string& group_name) const {
        return std::find(resolution_stack.begin(), resolution_stack.end(), group_name) 
               != resolution_stack.end();
    }
};

// 变量组管理器
class VariableGroupManager {
private:
    std::unordered_map<std::string, VariableGroupDefinition> variable_groups;
    std::unordered_map<std::string, VariableGroupInstance> instances;
    std::unordered_map<std::string, std::vector<std::string>> group_dependencies;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    // 实例计数器
    size_t instance_counter;
    
    // 内部方法
    std::string generateInstanceId();
    bool validateVariableGroup(const VariableGroupDefinition& group);
    bool validateSpecializationParameters(const std::string& group_name, 
                                         const std::unordered_map<std::string, std::string>& params);
    std::string resolveVariableValue(const std::string& value, const VariableResolutionContext& context);
    std::string processVariableExpression(const std::string& expression, 
                                         const std::unordered_map<std::string, std::string>& variables);
    
public:
    VariableGroupManager();
    ~VariableGroupManager() = default;
    
    // 变量组管理
    void addVariableGroup(const VariableGroupDefinition& group);
    void removeVariableGroup(const std::string& group_name);
    bool hasVariableGroup(const std::string& group_name) const;
    const VariableGroupDefinition* getVariableGroup(const std::string& group_name) const;
    std::vector<std::string> getAllVariableGroups() const;
    
    // 变量管理
    void addVariable(const std::string& group_name, const VariableDefinition& variable);
    void removeVariable(const std::string& group_name, const std::string& variable_name);
    bool hasVariable(const std::string& group_name, const std::string& variable_name) const;
    const VariableDefinition* getVariable(const std::string& group_name, const std::string& variable_name) const;
    
    // 变量组特例化
    SpecializationResult specializeGroup(const std::string& group_name, 
                                        const std::unordered_map<std::string, std::string>& parameters);
    SpecializationResult specializeGroup(const std::string& group_name, 
                                        const std::string& specialization_expression);
    
    // 变量组实例化
    std::string instantiateGroup(const std::string& group_name, 
                                const std::unordered_map<std::string, std::string>& parameters = {});
    bool removeInstance(const std::string& instance_id);
    const VariableGroupInstance* getInstance(const std::string& instance_id) const;
    
    // 变量值解析
    std::string resolveVariable(const std::string& group_name, const std::string& variable_name,
                               const VariableResolutionContext& context = {});
    std::string resolveVariable(const std::string& instance_id, const std::string& variable_name);
    std::unordered_map<std::string, std::string> resolveAllVariables(const std::string& group_name,
                                                                    const VariableResolutionContext& context = {});
    
    // 特例化语法解析
    std::unordered_map<std::string, std::string> parseSpecializationExpression(const std::string& expression);
    bool isValidSpecializationSyntax(const std::string& expression);
    
    // 依赖管理
    void addDependency(const std::string& group_name, const std::string& dependency);
    void removeDependency(const std::string& group_name, const std::string& dependency);
    std::vector<std::string> getDependencies(const std::string& group_name) const;
    std::vector<std::string> getResolutionOrder() const;
    bool hasCircularDependencies() const;
    
    // 变量表达式处理
    std::string evaluateExpression(const std::string& expression, 
                                  const std::unordered_map<std::string, std::string>& variables);
    bool isVariableReference(const std::string& value);
    std::string extractVariableReference(const std::string& value);
    
    // 类型转换和验证
    bool validateVariableType(const std::string& value, VariableType type);
    std::string convertVariableType(const std::string& value, VariableType from_type, VariableType to_type);
    VariableType inferVariableType(const std::string& value);
    
    // 搜索和查询
    std::vector<VariableDefinition> findVariables(const std::string& pattern) const;
    std::vector<std::string> findGroupsByVariable(const std::string& variable_name) const;
    std::vector<std::string> getInstancesByGroup(const std::string& group_name) const;
    
    // 导入导出
    void exportGroup(const std::string& group_name, const std::string& file_path) const;
    void importGroup(const std::string& file_path);
    std::string serializeGroup(const std::string& group_name) const;
    void deserializeGroup(const std::string& serialized_data);
    
    // 错误处理
    std::vector<std::string> getErrors() const { return errors; }
    std::vector<std::string> getWarnings() const { return warnings; }
    void clearErrors() { errors.clear(); }
    void clearWarnings() { warnings.clear(); }
    
    // 调试和报告
    std::string generateGroupReport(const std::string& group_name) const;
    std::string generateInstanceReport(const std::string& instance_id) const;
    std::string generateDependencyReport() const;
    void dumpState() const;
    
private:
    void addError(const std::string& message);
    void addWarning(const std::string& message);
    
    // 内部验证方法
    bool isValidVariableName(const std::string& name) const;
    bool isValidGroupName(const std::string& name) const;
    bool checkVariableConstraints(const VariableDefinition& variable, const std::string& value) const;
    
    // 内部解析方法
    std::string interpolateVariables(const std::string& template_str, 
                                    const std::unordered_map<std::string, std::string>& variables);
    std::vector<std::string> extractVariableReferences(const std::string& expression);
    
    // 循环依赖检测
    bool detectCircularDependency(const std::string& group_name, 
                                 std::unordered_set<std::string>& visited,
                                 std::unordered_set<std::string>& rec_stack) const;
};

// 变量组工具类
class VariableGroupUtils {
public:
    // 表达式解析
    static std::vector<std::string> tokenizeExpression(const std::string& expression);
    static std::unordered_map<std::string, std::string> parseParameterList(const std::string& params);
    static std::string normalizeVariableName(const std::string& name);
    
    // 类型检测
    static VariableType detectType(const std::string& value);
    static bool isColorValue(const std::string& value);
    static bool isLengthValue(const std::string& value);
    static bool isPercentageValue(const std::string& value);
    static bool isNumberValue(const std::string& value);
    
    // 值转换
    static std::string convertColor(const std::string& color, const std::string& format);
    static std::string convertLength(const std::string& length, const std::string& unit);
    static std::string normalizeValue(const std::string& value, VariableType type);
    
    // 特例化语法
    static bool isSpecializationSyntax(const std::string& expression);
    static std::string extractGroupName(const std::string& expression);
    static std::string extractParameters(const std::string& expression);
    
    // 验证
    static bool isValidVariableReference(const std::string& reference);
    static bool isValidParameterSyntax(const std::string& params);
    static std::vector<std::string> validateGroupDefinition(const VariableGroupDefinition& group);
    
    // 字符串处理
    static std::string trim(const std::string& str);
    static std::vector<std::string> split(const std::string& str, char delimiter);
    static std::string join(const std::vector<std::string>& parts, const std::string& separator);
};

// 预定义变量组类型
namespace PredefinedVariableGroups {
    extern const std::string COLORS;
    extern const std::string FONTS;
    extern const std::string SPACING;
    extern const std::string BREAKPOINTS;
    extern const std::string ANIMATIONS;
    extern const std::string SHADOWS;
    extern const std::string GRADIENTS;
}

} // namespace Common
} // namespace CHTL