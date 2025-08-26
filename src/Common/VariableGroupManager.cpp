#include "VariableGroupManager.h"
#include "GlobalMap.h"
#include "Context.h"
#include <algorithm>
#include <regex>
#include <sstream>
#include <iomanip>

namespace CHTL {

// ===== VariableGroupManager Implementation =====

VariableGroupManager::VariableGroupManager()
    : enable_type_inference(true)
    , enable_dependency_tracking(true)
    , enable_specialization_cache(true)
    , max_specialization_depth(10)
    , max_parameter_count(50) {
    initializePredefinedGroups();
    initializeBuiltinTypes();
}

void VariableGroupManager::initializePredefinedGroups() {
    // 初始化预定义变量组
    
    // ThemeColor变量组
    VariableGroupDefinition theme_color;
    theme_color.name = "ThemeColor";
    theme_color.type = VariableGroupType::PARAMETERIZED;
    theme_color.description = "主题颜色变量组";
    
    // 添加默认变量
    VariableDefinition primary_color;
    primary_color.name = "primaryColor";
    primary_color.type = VariableType::COLOR;
    primary_color.default_value = "rgb(74, 144, 226)";
    primary_color.description = "主要颜色";
    theme_color.variables["primaryColor"] = primary_color;
    
    VariableDefinition secondary_color;
    secondary_color.name = "secondaryColor";
    secondary_color.type = VariableType::COLOR;
    secondary_color.default_value = "rgb(108, 117, 125)";
    secondary_color.description = "次要颜色";
    theme_color.variables["secondaryColor"] = secondary_color;
    
    VariableDefinition table_color;
    table_color.name = "tableColor";
    table_color.type = VariableType::COLOR;
    table_color.default_value = "rgb(145, 155, 200)";
    table_color.description = "表格颜色";
    theme_color.variables["tableColor"] = table_color;
    
    variable_groups[theme_color.name] = theme_color;
    
    // FontSize变量组
    VariableGroupDefinition font_size;
    font_size.name = "FontSize";
    font_size.type = VariableGroupType::SPECIALIZED;
    font_size.description = "字体大小变量组";
    
    VariableDefinition small_font;
    small_font.name = "small";
    small_font.type = VariableType::LENGTH;
    small_font.default_value = "12px";
    font_size.variables["small"] = small_font;
    
    VariableDefinition medium_font;
    medium_font.name = "medium";
    medium_font.type = VariableType::LENGTH;
    medium_font.default_value = "16px";
    font_size.variables["medium"] = medium_font;
    
    VariableDefinition large_font;
    large_font.name = "large";
    large_font.type = VariableType::LENGTH;
    large_font.default_value = "20px";
    font_size.variables["large"] = large_font;
    
    variable_groups[font_size.name] = font_size;
}

void VariableGroupManager::initializeBuiltinTypes() {
    // 初始化内置类型验证规则
    type_validators[VariableType::COLOR] = [](const std::string& value) {
        // 验证颜色值：rgb(), rgba(), hex, 命名颜色等
        std::regex color_patterns[] = {
            std::regex(R"(^#[0-9a-fA-F]{3,8}$)"),                     // hex
            std::regex(R"(^rgb\s*\(\s*\d+\s*,\s*\d+\s*,\s*\d+\s*\)$)"), // rgb
            std::regex(R"(^rgba\s*\(\s*\d+\s*,\s*\d+\s*,\s*\d+\s*,\s*[\d.]+\s*\)$)"), // rgba
            std::regex(R"(^(red|green|blue|white|black|yellow|purple|orange|pink|gray|grey|transparent)$)", std::regex::icase) // 命名颜色
        };
        
        for (const auto& pattern : color_patterns) {
            if (std::regex_match(value, pattern)) return true;
        }
        return false;
    };
    
    type_validators[VariableType::LENGTH] = [](const std::string& value) {
        // 验证长度值：px, em, rem, %, vw, vh等
        std::regex length_pattern(R"(^[\d.]+\s*(px|em|rem|%|vw|vh|vmin|vmax|pt|pc|in|cm|mm|ex|ch)$)");
        return std::regex_match(value, length_pattern);
    };
    
    type_validators[VariableType::PERCENTAGE] = [](const std::string& value) {
        std::regex percentage_pattern(R"(^[\d.]+\s*%$)");
        return std::regex_match(value, percentage_pattern);
    };
    
    type_validators[VariableType::NUMBER] = [](const std::string& value) {
        std::regex number_pattern(R"(^-?[\d.]+$)");
        return std::regex_match(value, number_pattern);
    };
    
    type_validators[VariableType::STRING] = [](const std::string& value) {
        return true; // 字符串总是有效的
    };
    
    type_validators[VariableType::BOOLEAN] = [](const std::string& value) {
        return value == "true" || value == "false" || value == "1" || value == "0";
    };
}

bool VariableGroupManager::defineVariableGroup(const VariableGroupDefinition& group) {
    if (group.name.empty()) {
        addError("变量组名称不能为空");
        return false;
    }
    
    if (variable_groups.find(group.name) != variable_groups.end()) {
        addWarning("变量组 '" + group.name + "' 已存在，将被覆盖");
    }
    
    // 验证变量组的合法性
    if (!validateVariableGroup(group)) {
        return false;
    }
    
    variable_groups[group.name] = group;
    
    // 更新依赖关系
    if (enable_dependency_tracking) {
        updateDependencies(group.name);
    }
    
    return true;
}

bool VariableGroupManager::removeVariableGroup(const std::string& group_name) {
    auto it = variable_groups.find(group_name);
    if (it == variable_groups.end()) {
        addError("变量组 '" + group_name + "' 不存在");
        return false;
    }
    
    // 检查是否有其他组依赖于此组
    std::vector<std::string> dependents = findDependents(group_name);
    if (!dependents.empty()) {
        addError("无法删除变量组 '" + group_name + "'，被以下组依赖：" + 
                 joinStrings(dependents, ", "));
        return false;
    }
    
    variable_groups.erase(it);
    
    // 清理依赖关系
    dependencies.erase(group_name);
    for (auto& [name, deps] : dependencies) {
        deps.erase(std::remove(deps.begin(), deps.end(), group_name), deps.end());
    }
    
    return true;
}

bool VariableGroupManager::hasVariableGroup(const std::string& group_name) const {
    return variable_groups.find(group_name) != variable_groups.end();
}

VariableGroupDefinition VariableGroupManager::getVariableGroup(const std::string& group_name) const {
    auto it = variable_groups.find(group_name);
    if (it != variable_groups.end()) {
        return it->second;
    }
    return VariableGroupDefinition(); // 返回空定义
}

std::vector<std::string> VariableGroupManager::getAllGroupNames() const {
    std::vector<std::string> names;
    for (const auto& [name, group] : variable_groups) {
        names.push_back(name);
    }
    return names;
}

SpecializationResult VariableGroupManager::specializeGroup(
    const std::string& group_name, 
    const std::unordered_map<std::string, std::string>& parameters) {
    
    SpecializationResult result;
    result.success = false;
    
    auto it = variable_groups.find(group_name);
    if (it == variable_groups.end()) {
        result.errors.push_back("变量组 '" + group_name + "' 不存在");
        return result;
    }
    
    const auto& group = it->second;
    
    // 检查特例化深度
    if (parameters.size() > max_specialization_depth) {
        result.errors.push_back("特例化参数过多，最大支持 " + std::to_string(max_specialization_depth) + " 个");
        return result;
    }
    
    // 创建特例化实例
    VariableGroupInstance instance;
    instance.group_name = group_name;
    instance.instance_id = generateInstanceId(group_name, parameters);
    instance.parameters = parameters;
    
    // 应用参数到变量
    for (const auto& [var_name, var_def] : group.variables) {
        VariableDefinition specialized_var = var_def;
        
        // 检查是否有参数覆盖
        auto param_it = parameters.find(var_name);
        if (param_it != parameters.end()) {
            // 验证参数值
            if (!validateVariableValue(specialized_var.type, param_it->second)) {
                result.errors.push_back("参数 '" + var_name + "' 的值 '" + param_it->second + "' 不符合类型要求");
                continue;
            }
            specialized_var.value = param_it->second;
        } else {
            specialized_var.value = specialized_var.default_value;
        }
        
        instance.variables[var_name] = specialized_var;
    }
    
    // 缓存特例化结果
    if (enable_specialization_cache) {
        specialization_cache[instance.instance_id] = instance;
    }
    
    result.success = true;
    result.instance = instance;
    return result;
}

bool VariableGroupManager::addVariable(const std::string& group_name, const VariableDefinition& variable) {
    auto it = variable_groups.find(group_name);
    if (it == variable_groups.end()) {
        addError("变量组 '" + group_name + "' 不存在");
        return false;
    }
    
    if (!validateVariable(variable)) {
        return false;
    }
    
    it->second.variables[variable.name] = variable;
    return true;
}

bool VariableGroupManager::removeVariable(const std::string& group_name, const std::string& variable_name) {
    auto it = variable_groups.find(group_name);
    if (it == variable_groups.end()) {
        addError("变量组 '" + group_name + "' 不存在");
        return false;
    }
    
    auto var_it = it->second.variables.find(variable_name);
    if (var_it == it->second.variables.end()) {
        addError("变量 '" + variable_name + "' 在组 '" + group_name + "' 中不存在");
        return false;
    }
    
    it->second.variables.erase(var_it);
    return true;
}

bool VariableGroupManager::hasVariable(const std::string& group_name, const std::string& variable_name) const {
    auto it = variable_groups.find(group_name);
    if (it == variable_groups.end()) return false;
    
    return it->second.variables.find(variable_name) != it->second.variables.end();
}

VariableDefinition VariableGroupManager::getVariable(const std::string& group_name, const std::string& variable_name) const {
    auto it = variable_groups.find(group_name);
    if (it != variable_groups.end()) {
        auto var_it = it->second.variables.find(variable_name);
        if (var_it != it->second.variables.end()) {
            return var_it->second;
        }
    }
    return VariableDefinition(); // 返回空定义
}

VariableType VariableGroupManager::inferVariableType(const std::string& value) const {
    if (!enable_type_inference) {
        return VariableType::STRING; // 默认为字符串
    }
    
    // 尝试推断类型
    for (const auto& [type, validator] : type_validators) {
        if (validator(value)) {
            return type;
        }
    }
    
    return VariableType::STRING; // 默认为字符串
}

bool VariableGroupManager::validateVariableValue(VariableType type, const std::string& value) const {
    auto it = type_validators.find(type);
    if (it != type_validators.end()) {
        return it->second(value);
    }
    return true; // 未知类型默认通过
}

std::vector<std::string> VariableGroupManager::getDependencies(const std::string& group_name) const {
    auto it = dependencies.find(group_name);
    if (it != dependencies.end()) {
        return it->second;
    }
    return {};
}

std::vector<std::string> VariableGroupManager::findDependents(const std::string& group_name) const {
    std::vector<std::string> dependents;
    for (const auto& [name, deps] : dependencies) {
        if (std::find(deps.begin(), deps.end(), group_name) != deps.end()) {
            dependents.push_back(name);
        }
    }
    return dependents;
}

bool VariableGroupManager::hasCyclicDependency(const std::string& group_name) const {
    std::unordered_set<std::string> visited;
    std::unordered_set<std::string> recursion_stack;
    return hasCyclicDependencyHelper(group_name, visited, recursion_stack);
}

std::string VariableGroupManager::resolveVariableReference(const std::string& reference, 
                                                          const VariableResolutionContext& context) const {
    // 解析形如 "GroupName.variableName" 的引用
    std::regex reference_pattern(R"(^([A-Za-z_][A-Za-z0-9_]*)(\.([A-Za-z_][A-Za-z0-9_]*))?$)");
    std::smatch matches;
    
    if (!std::regex_match(reference, matches, reference_pattern)) {
        return ""; // 无效的引用格式
    }
    
    std::string group_name = matches[1].str();
    std::string variable_name = matches[3].str();
    
    // 如果没有指定变量名，返回组名
    if (variable_name.empty()) {
        return group_name;
    }
    
    // 查找变量
    auto group_it = variable_groups.find(group_name);
    if (group_it == variable_groups.end()) {
        return ""; // 组不存在
    }
    
    auto var_it = group_it->second.variables.find(variable_name);
    if (var_it == group_it->second.variables.end()) {
        return ""; // 变量不存在
    }
    
    // 返回变量值（优先使用当前值，否则使用默认值）
    return var_it->second.value.empty() ? var_it->second.default_value : var_it->second.value;
}

std::string VariableGroupManager::generateCSS(const std::string& group_name) const {
    auto it = variable_groups.find(group_name);
    if (it == variable_groups.end()) {
        return "";
    }
    
    std::stringstream css;
    css << "/* Variable Group: " << group_name << " */\n";
    css << ":root {\n";
    
    for (const auto& [var_name, var_def] : it->second.variables) {
        std::string css_var_name = "--" + group_name + "-" + var_name;
        std::string value = var_def.value.empty() ? var_def.default_value : var_def.value;
        css << "  " << css_var_name << ": " << value << ";\n";
    }
    
    css << "}\n\n";
    
    // 添加使用示例注释
    css << "/* Usage examples:\n";
    for (const auto& [var_name, var_def] : it->second.variables) {
        std::string css_var_name = "--" + group_name + "-" + var_name;
        css << " * color: var(" << css_var_name << ");\n";
    }
    css << " */\n";
    
    return css.str();
}

std::string VariableGroupManager::generateJavaScript(const std::string& group_name) const {
    auto it = variable_groups.find(group_name);
    if (it == variable_groups.end()) {
        return "";
    }
    
    std::stringstream js;
    js << "// Variable Group: " << group_name << "\n";
    js << "const " << group_name << " = {\n";
    
    bool first = true;
    for (const auto& [var_name, var_def] : it->second.variables) {
        if (!first) js << ",\n";
        first = false;
        
        std::string value = var_def.value.empty() ? var_def.default_value : var_def.value;
        js << "  " << var_name << ": \"" << escapeJavaScriptString(value) << "\"";
    }
    
    js << "\n};\n\n";
    
    // 添加导出
    js << "// Export for use in modules\n";
    js << "if (typeof module !== 'undefined' && module.exports) {\n";
    js << "  module.exports." << group_name << " = " << group_name << ";\n";
    js << "}\n";
    
    return js.str();
}

std::vector<std::string> VariableGroupManager::getLastErrors() const {
    return errors;
}

std::vector<std::string> VariableGroupManager::getLastWarnings() const {
    return warnings;
}

void VariableGroupManager::clearErrors() {
    errors.clear();
}

void VariableGroupManager::clearWarnings() {
    warnings.clear();
}

void VariableGroupManager::enableTypeInference(bool enable) {
    enable_type_inference = enable;
}

void VariableGroupManager::enableDependencyTracking(bool enable) {
    enable_dependency_tracking = enable;
}

void VariableGroupManager::enableSpecializationCache(bool enable) {
    enable_specialization_cache = enable;
    if (!enable) {
        specialization_cache.clear();
    }
}

void VariableGroupManager::setMaxSpecializationDepth(size_t depth) {
    max_specialization_depth = depth;
}

void VariableGroupManager::setMaxParameterCount(size_t count) {
    max_parameter_count = count;
}

void VariableGroupManager::clearSpecializationCache() {
    specialization_cache.clear();
}

std::string VariableGroupManager::generateGroupReport(const std::string& group_name) const {
    auto it = variable_groups.find(group_name);
    if (it == variable_groups.end()) {
        return "变量组 '" + group_name + "' 不存在";
    }
    
    const auto& group = it->second;
    std::stringstream report;
    
    report << "=== 变量组报告: " << group_name << " ===\n";
    report << "类型: " << variableGroupTypeToString(group.type) << "\n";
    report << "描述: " << group.description << "\n";
    report << "变量数量: " << group.variables.size() << "\n\n";
    
    report << "变量列表:\n";
    for (const auto& [var_name, var_def] : group.variables) {
        report << "  - " << var_name << " (" << variableTypeToString(var_def.type) << ")\n";
        report << "    默认值: " << var_def.default_value << "\n";
        if (!var_def.value.empty()) {
            report << "    当前值: " << var_def.value << "\n";
        }
        if (!var_def.description.empty()) {
            report << "    描述: " << var_def.description << "\n";
        }
        report << "\n";
    }
    
    // 依赖信息
    auto deps = getDependencies(group_name);
    if (!deps.empty()) {
        report << "依赖的组: " << joinStrings(deps, ", ") << "\n";
    }
    
    auto dependents = findDependents(group_name);
    if (!dependents.empty()) {
        report << "被依赖于: " << joinStrings(dependents, ", ") << "\n";
    }
    
    return report.str();
}

// ===== 私有方法实现 =====

bool VariableGroupManager::validateVariableGroup(const VariableGroupDefinition& group) const {
    if (group.name.empty()) {
        return false;
    }
    
    // 验证变量名唯一性
    std::unordered_set<std::string> var_names;
    for (const auto& [var_name, var_def] : group.variables) {
        if (var_names.find(var_name) != var_names.end()) {
            return false; // 重复的变量名
        }
        var_names.insert(var_name);
        
        if (!validateVariable(var_def)) {
            return false;
        }
    }
    
    return true;
}

bool VariableGroupManager::validateVariable(const VariableDefinition& variable) const {
    if (variable.name.empty()) {
        return false;
    }
    
    // 验证默认值
    if (!variable.default_value.empty()) {
        if (!validateVariableValue(variable.type, variable.default_value)) {
            return false;
        }
    }
    
    // 验证当前值
    if (!variable.value.empty()) {
        if (!validateVariableValue(variable.type, variable.value)) {
            return false;
        }
    }
    
    return true;
}

void VariableGroupManager::updateDependencies(const std::string& group_name) {
    // 分析变量组中的依赖关系
    auto it = variable_groups.find(group_name);
    if (it == variable_groups.end()) return;
    
    std::vector<std::string> deps;
    
    // 检查变量值中的引用
    for (const auto& [var_name, var_def] : it->second.variables) {
        std::vector<std::string> references = extractVariableReferences(var_def.default_value);
        for (const std::string& ref : references) {
            // 提取组名
            size_t dot_pos = ref.find('.');
            if (dot_pos != std::string::npos) {
                std::string ref_group = ref.substr(0, dot_pos);
                if (ref_group != group_name && 
                    std::find(deps.begin(), deps.end(), ref_group) == deps.end()) {
                    deps.push_back(ref_group);
                }
            }
        }
    }
    
    dependencies[group_name] = deps;
}

std::vector<std::string> VariableGroupManager::extractVariableReferences(const std::string& value) const {
    std::vector<std::string> references;
    
    // 查找形如 ${GroupName.variableName} 的引用
    std::regex ref_pattern(R"(\$\{([A-Za-z_][A-Za-z0-9_]*\.[A-Za-z_][A-Za-z0-9_]*)\})");
    std::sregex_iterator iter(value.begin(), value.end(), ref_pattern);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        references.push_back((*iter)[1].str());
    }
    
    return references;
}

bool VariableGroupManager::hasCyclicDependencyHelper(const std::string& group_name,
                                                    std::unordered_set<std::string>& visited,
                                                    std::unordered_set<std::string>& recursion_stack) const {
    visited.insert(group_name);
    recursion_stack.insert(group_name);
    
    auto deps = getDependencies(group_name);
    for (const std::string& dep : deps) {
        if (recursion_stack.find(dep) != recursion_stack.end()) {
            return true; // 发现循环
        }
        
        if (visited.find(dep) == visited.end()) {
            if (hasCyclicDependencyHelper(dep, visited, recursion_stack)) {
                return true;
            }
        }
    }
    
    recursion_stack.erase(group_name);
    return false;
}

std::string VariableGroupManager::generateInstanceId(const std::string& group_name,
                                                    const std::unordered_map<std::string, std::string>& parameters) const {
    std::stringstream id;
    id << group_name << "_";
    
    // 按键名排序以确保ID一致性
    std::vector<std::pair<std::string, std::string>> sorted_params(parameters.begin(), parameters.end());
    std::sort(sorted_params.begin(), sorted_params.end());
    
    for (const auto& [key, value] : sorted_params) {
        id << key << "=" << value << "_";
    }
    
    return id.str();
}

void VariableGroupManager::addError(const std::string& message) {
    errors.push_back(message);
}

void VariableGroupManager::addWarning(const std::string& message) {
    warnings.push_back(message);
}

std::string VariableGroupManager::joinStrings(const std::vector<std::string>& strings, const std::string& delimiter) const {
    if (strings.empty()) return "";
    
    std::stringstream result;
    for (size_t i = 0; i < strings.size(); ++i) {
        if (i > 0) result << delimiter;
        result << strings[i];
    }
    return result.str();
}

std::string VariableGroupManager::escapeJavaScriptString(const std::string& str) const {
    std::string escaped;
    for (char c : str) {
        switch (c) {
            case '"': escaped += "\\\""; break;
            case '\\': escaped += "\\\\"; break;
            case '\n': escaped += "\\n"; break;
            case '\r': escaped += "\\r"; break;
            case '\t': escaped += "\\t"; break;
            default: escaped += c; break;
        }
    }
    return escaped;
}

// ===== VariableGroupUtils Implementation =====

VariableType VariableGroupUtils::parseVariableType(const std::string& type_str) {
    std::string lower_type = type_str;
    std::transform(lower_type.begin(), lower_type.end(), lower_type.begin(), ::tolower);
    
    if (lower_type == "color") return VariableType::COLOR;
    if (lower_type == "length") return VariableType::LENGTH;
    if (lower_type == "percentage") return VariableType::PERCENTAGE;
    if (lower_type == "number") return VariableType::NUMBER;
    if (lower_type == "string") return VariableType::STRING;
    if (lower_type == "boolean") return VariableType::BOOLEAN;
    if (lower_type == "expression") return VariableType::EXPRESSION;
    
    return VariableType::STRING; // 默认
}

std::string VariableGroupUtils::variableTypeToString(VariableType type) {
    switch (type) {
        case VariableType::COLOR: return "color";
        case VariableType::LENGTH: return "length";
        case VariableType::PERCENTAGE: return "percentage";
        case VariableType::NUMBER: return "number";
        case VariableType::STRING: return "string";
        case VariableType::BOOLEAN: return "boolean";
        case VariableType::EXPRESSION: return "expression";
        default: return "unknown";
    }
}

VariableGroupType VariableGroupUtils::parseVariableGroupType(const std::string& type_str) {
    std::string lower_type = type_str;
    std::transform(lower_type.begin(), lower_type.end(), lower_type.begin(), ::tolower);
    
    if (lower_type == "basic") return VariableGroupType::BASIC;
    if (lower_type == "specialized") return VariableGroupType::SPECIALIZED;
    if (lower_type == "parameterized") return VariableGroupType::PARAMETERIZED;
    if (lower_type == "template") return VariableGroupType::TEMPLATE;
    
    return VariableGroupType::BASIC; // 默认
}

std::string VariableGroupUtils::variableGroupTypeToString(VariableGroupType type) {
    switch (type) {
        case VariableGroupType::BASIC: return "basic";
        case VariableGroupType::SPECIALIZED: return "specialized";
        case VariableGroupType::PARAMETERIZED: return "parameterized";
        case VariableGroupType::TEMPLATE: return "template";
        default: return "unknown";
    }
}

bool VariableGroupUtils::isValidVariableName(const std::string& name) {
    if (name.empty()) return false;
    
    // 变量名必须以字母或下划线开头，后面可以跟字母、数字或下划线
    std::regex name_pattern(R"(^[A-Za-z_][A-Za-z0-9_]*$)");
    return std::regex_match(name, name_pattern);
}

bool VariableGroupUtils::isValidGroupName(const std::string& name) {
    return isValidVariableName(name); // 使用相同的规则
}

std::string VariableGroupUtils::normalizeVariableValue(const std::string& value, VariableType type) {
    std::string normalized = value;
    
    // 去除首尾空白
    normalized.erase(0, normalized.find_first_not_of(" \t\n\r"));
    normalized.erase(normalized.find_last_not_of(" \t\n\r") + 1);
    
    // 根据类型进行特定的标准化
    switch (type) {
        case VariableType::COLOR:
            // 标准化颜色值：将hex颜色转为小写
            if (normalized[0] == '#') {
                std::transform(normalized.begin(), normalized.end(), normalized.begin(), ::tolower);
            }
            break;
            
        case VariableType::LENGTH:
            // 标准化长度值：确保单位小写
            {
                std::regex length_pattern(R"(^([\d.]+)\s*([A-Za-z%]+)$)");
                std::smatch matches;
                if (std::regex_match(normalized, matches, length_pattern)) {
                    std::string number = matches[1].str();
                    std::string unit = matches[2].str();
                    std::transform(unit.begin(), unit.end(), unit.begin(), ::tolower);
                    normalized = number + unit;
                }
            }
            break;
            
        case VariableType::BOOLEAN:
            // 标准化布尔值
            if (normalized == "1" || normalized == "true") {
                normalized = "true";
            } else if (normalized == "0" || normalized == "false") {
                normalized = "false";
            }
            break;
            
        default:
            break;
    }
    
    return normalized;
}

std::vector<std::string> VariableGroupUtils::extractVariableReferences(const std::string& expression) {
    std::vector<std::string> references;
    
    // 查找各种形式的变量引用
    std::vector<std::regex> patterns = {
        std::regex(R"(\$\{([A-Za-z_][A-Za-z0-9_]*(?:\.[A-Za-z_][A-Za-z0-9_]*)?)\})"), // ${Group.var}
        std::regex(R"(\$([A-Za-z_][A-Za-z0-9_]*(?:\.[A-Za-z_][A-Za-z0-9_]*)?)\b)"),   // $Group.var
        std::regex(R"(var\(([A-Za-z_][A-Za-z0-9_-]*)\))"),                            // var(--css-var)
    };
    
    for (const auto& pattern : patterns) {
        std::sregex_iterator iter(expression.begin(), expression.end(), pattern);
        std::sregex_iterator end;
        
        for (; iter != end; ++iter) {
            references.push_back((*iter)[1].str());
        }
    }
    
    return references;
}

bool VariableGroupUtils::validateVariableReference(const std::string& reference) {
    // 验证引用格式：GroupName.variableName 或 variableName
    std::regex ref_pattern(R"(^[A-Za-z_][A-Za-z0-9_]*(?:\.[A-Za-z_][A-Za-z0-9_]*)?$)");
    return std::regex_match(reference, ref_pattern);
}

std::string VariableGroupUtils::generateVariableId(const std::string& group_name, const std::string& variable_name) {
    return group_name + "." + variable_name;
}

// ===== 预定义变量组 =====

namespace PredefinedVariableGroups {
    const VariableGroupDefinition THEME_COLORS = []() {
        VariableGroupDefinition group;
        group.name = "ThemeColors";
        group.type = VariableGroupType::PARAMETERIZED;
        group.description = "主题颜色变量组";
        
        VariableDefinition primary;
        primary.name = "primary";
        primary.type = VariableType::COLOR;
        primary.default_value = "#007bff";
        group.variables["primary"] = primary;
        
        VariableDefinition secondary;
        secondary.name = "secondary";
        secondary.type = VariableType::COLOR;
        secondary.default_value = "#6c757d";
        group.variables["secondary"] = secondary;
        
        return group;
    }();
    
    const VariableGroupDefinition SPACING = []() {
        VariableGroupDefinition group;
        group.name = "Spacing";
        group.type = VariableGroupType::SPECIALIZED;
        group.description = "间距变量组";
        
        VariableDefinition small;
        small.name = "small";
        small.type = VariableType::LENGTH;
        small.default_value = "8px";
        group.variables["small"] = small;
        
        VariableDefinition medium;
        medium.name = "medium";
        medium.type = VariableType::LENGTH;
        medium.default_value = "16px";
        group.variables["medium"] = medium;
        
        VariableDefinition large;
        large.name = "large";
        large.type = VariableType::LENGTH;
        large.default_value = "24px";
        group.variables["large"] = large;
        
        return group;
    }();
}

} // namespace CHTL