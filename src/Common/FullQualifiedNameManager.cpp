#include "FullQualifiedNameManager.h"
#include "GlobalMap.h"
#include "Context.h"
#include <algorithm>
#include <regex>
#include <sstream>

namespace CHTL {

// ===== FullQualifiedNameManager Implementation =====

FullQualifiedNameManager::FullQualifiedNameManager()
    : enable_namespace_resolution(true)
    , enable_alias_resolution(true)
    , enable_conflict_detection(true)
    , enable_type_inference(true)
    , strict_mode(false)
    , max_resolution_depth(10) {
    initializeBuiltinTypes();
    initializeNamePatterns();
}

void FullQualifiedNameManager::initializeBuiltinTypes() {
    // 初始化内置类型
    builtin_types = {
        "Element", "Style", "Var", "Template", "Custom", "Configuration",
        "Html", "CSS", "JavaScript", "Origin", "Import", "Namespace"
    };
    
    // 初始化类型前缀映射
    type_prefixes = {
        {"[Template]", "Template"},
        {"[Custom]", "Custom"},
        {"[Configuration]", "Configuration"},
        {"[Origin]", "Origin"},
        {"[Import]", "Import"},
        {"[Namespace]", "Namespace"}
    };
    
    // 初始化修饰符映射
    type_modifiers = {
        {"@Element", "Element"},
        {"@Style", "Style"},
        {"@Var", "Var"},
        {"@Html", "Html"},
        {"@CSS", "CSS"},
        {"@JavaScript", "JavaScript"},
        {"@Config", "Configuration"}
    };
}

void FullQualifiedNameManager::initializeNamePatterns() {
    // 初始化名称匹配模式
    
    // 全缀名模式: [Type] @Modifier Name
    full_qualified_patterns = {
        std::regex(R"(\[([A-Za-z]+)\]\s*@([A-Za-z]+)\s+([A-Za-z_][A-Za-z0-9_]*))"),  // [Custom] @Element Box
        std::regex(R"(\[([A-Za-z]+)\]\s+([A-Za-z_][A-Za-z0-9_]*))"),                 // [Custom] Box
        std::regex(R"(@([A-Za-z]+)\s+([A-Za-z_][A-Za-z0-9_]*))"),                     // @Element Box
        std::regex(R"(([A-Za-z_][A-Za-z0-9_]*)::\s*([A-Za-z_][A-Za-z0-9_]*))"),       // Namespace::Item
    };
    
    // 命名空间模式
    namespace_patterns = {
        std::regex(R"(([A-Za-z_][A-Za-z0-9_]*)::\s*([A-Za-z_][A-Za-z0-9_]*))"),      // Namespace::Item
        std::regex(R"(([A-Za-z_][A-Za-z0-9_]*)\.([A-Za-z_][A-Za-z0-9_]*))"),          // Namespace.Item
    };
    
    // 简单名称模式
    simple_name_pattern = std::regex(R"(^[A-Za-z_][A-Za-z0-9_]*$)");
}

FullQualifiedNameParseResult FullQualifiedNameManager::parseName(const std::string& name) {
    FullQualifiedNameParseResult result;
    result.success = false;
    result.original_name = name;
    
    if (name.empty()) {
        result.errors.push_back("名称不能为空");
        return result;
    }
    
    // 尝试匹配不同的名称模式
    for (const auto& pattern : full_qualified_patterns) {
        std::smatch matches;
        if (std::regex_match(name, matches, pattern)) {
            result = parseFullQualifiedName(name, matches);
            if (result.success) {
                break;
            }
        }
    }
    
    // 如果全缀名匹配失败，尝试命名空间模式
    if (!result.success) {
        for (const auto& pattern : namespace_patterns) {
            std::smatch matches;
            if (std::regex_match(name, matches, pattern)) {
                result = parseNamespacedName(name, matches);
                if (result.success) {
                    break;
                }
            }
        }
    }
    
    // 如果都失败，尝试简单名称
    if (!result.success) {
        std::smatch matches;
        if (std::regex_match(name, matches, simple_name_pattern)) {
            result = parseSimpleName(name);
        }
    }
    
    // 最终验证
    if (!result.success) {
        result.errors.push_back("无法解析名称: " + name);
    } else {
        if (!validateNameComponents(result)) {
            result.success = false;
            result.errors.push_back("名称组件验证失败");
        }
    }
    
    return result;
}

FullQualifiedNameParseResult FullQualifiedNameManager::parseFullQualifiedName(const std::string& name, const std::smatch& matches) {
    FullQualifiedNameParseResult result;
    result.type = FullQualifiedNameType::FULL_QUALIFIED;
    result.original_name = name;
    
    if (matches.size() >= 4) {
        // [Type] @Modifier Name 格式
        result.type_prefix = matches[1].str();
        result.type_modifier = matches[2].str();
        result.base_name = matches[3].str();
        
        // 验证类型前缀和修饰符
        if (type_prefixes.find("[" + result.type_prefix + "]") != type_prefixes.end() &&
            type_modifiers.find("@" + result.type_modifier) != type_modifiers.end()) {
            result.resolved_type = type_prefixes["[" + result.type_prefix + "]"];
            result.resolved_modifier = type_modifiers["@" + result.type_modifier];
            result.success = true;
        } else {
            result.errors.push_back("未知的类型前缀或修饰符");
        }
    }
    else if (matches.size() >= 3) {
        if (matches[0].str().find("@") != std::string::npos) {
            // @Modifier Name 格式
            result.type_modifier = matches[1].str();
            result.base_name = matches[2].str();
            
            if (type_modifiers.find("@" + result.type_modifier) != type_modifiers.end()) {
                result.resolved_modifier = type_modifiers["@" + result.type_modifier];
                result.success = true;
            } else {
                result.errors.push_back("未知的类型修饰符");
            }
        } else {
            // [Type] Name 格式
            result.type_prefix = matches[1].str();
            result.base_name = matches[2].str();
            
            if (type_prefixes.find("[" + result.type_prefix + "]") != type_prefixes.end()) {
                result.resolved_type = type_prefixes["[" + result.type_prefix + "]"];
                result.success = true;
            } else {
                result.errors.push_back("未知的类型前缀");
            }
        }
    }
    
    return result;
}

FullQualifiedNameParseResult FullQualifiedNameManager::parseNamespacedName(const std::string& name, const std::smatch& matches) {
    FullQualifiedNameParseResult result;
    result.type = FullQualifiedNameType::NAMESPACED;
    result.original_name = name;
    
    if (matches.size() >= 3) {
        result.namespace_name = matches[1].str();
        result.base_name = matches[2].str();
        result.success = true;
    } else {
        result.errors.push_back("命名空间名称解析失败");
    }
    
    return result;
}

FullQualifiedNameParseResult FullQualifiedNameManager::parseSimpleName(const std::string& name) {
    FullQualifiedNameParseResult result;
    result.type = FullQualifiedNameType::SIMPLE;
    result.original_name = name;
    result.base_name = name;
    result.success = true;
    
    return result;
}

bool FullQualifiedNameManager::validateNameComponents(const FullQualifiedNameParseResult& result) const {
    // 验证基础名称
    if (result.base_name.empty()) {
        return false;
    }
    
    // 验证名称格式
    if (!std::regex_match(result.base_name, simple_name_pattern)) {
        return false;
    }
    
    // 验证类型前缀
    if (!result.type_prefix.empty()) {
        std::string full_prefix = "[" + result.type_prefix + "]";
        if (type_prefixes.find(full_prefix) == type_prefixes.end()) {
            return false;
        }
    }
    
    // 验证类型修饰符
    if (!result.type_modifier.empty()) {
        std::string full_modifier = "@" + result.type_modifier;
        if (type_modifiers.find(full_modifier) == type_modifiers.end()) {
            return false;
        }
    }
    
    // 验证命名空间
    if (!result.namespace_name.empty()) {
        if (!std::regex_match(result.namespace_name, simple_name_pattern)) {
            return false;
        }
    }
    
    return true;
}

std::string FullQualifiedNameManager::resolveName(const std::string& name, const NameResolutionContext& context) {
    auto parse_result = parseName(name);
    if (!parse_result.success) {
        addError("无法解析名称: " + name);
        return "";
    }
    
    // 根据类型进行不同的解析策略
    switch (parse_result.type) {
        case FullQualifiedNameType::FULL_QUALIFIED:
            return resolveFullQualifiedName(parse_result, context);
            
        case FullQualifiedNameType::NAMESPACED:
            return resolveNamespacedName(parse_result, context);
            
        case FullQualifiedNameType::SIMPLE:
            return resolveSimpleName(parse_result, context);
            
        default:
            addError("未知的名称类型");
            return "";
    }
}

std::string FullQualifiedNameManager::resolveFullQualifiedName(const FullQualifiedNameParseResult& parse_result, const NameResolutionContext& context) {
    std::string resolved_name;
    
    // 构建完整的限定名称
    if (!parse_result.type_prefix.empty()) {
        resolved_name += "[" + parse_result.type_prefix + "]";
    }
    
    if (!parse_result.type_modifier.empty()) {
        if (!resolved_name.empty()) resolved_name += " ";
        resolved_name += "@" + parse_result.type_modifier;
    }
    
    if (!resolved_name.empty()) resolved_name += " ";
    resolved_name += parse_result.base_name;
    
    // 检查是否存在注册
    auto registration_key = generateRegistrationKey(parse_result);
    auto it = name_registrations.find(registration_key);
    
    if (it != name_registrations.end()) {
        // 使用已注册的名称
        return it->second.resolved_name;
    } else {
        // 注册新名称
        NameRegistration registration;
        registration.original_name = parse_result.original_name;
        registration.resolved_name = resolved_name;
        registration.type = parse_result.type;
        registration.namespace_context = context.current_namespace;
        registration.file_context = context.current_file;
        
        name_registrations[registration_key] = registration;
        return resolved_name;
    }
}

std::string FullQualifiedNameManager::resolveNamespacedName(const FullQualifiedNameParseResult& parse_result, const NameResolutionContext& context) {
    // 检查命名空间是否存在
    if (!hasNamespace(parse_result.namespace_name)) {
        if (enable_namespace_resolution) {
            addWarning("命名空间 '" + parse_result.namespace_name + "' 不存在，自动创建");
            registerNamespace(parse_result.namespace_name);
        } else {
            addError("命名空间 '" + parse_result.namespace_name + "' 不存在");
            return "";
        }
    }
    
    // 构建完整名称
    std::string resolved_name = parse_result.namespace_name + "::" + parse_result.base_name;
    
    // 检查冲突
    if (enable_conflict_detection) {
        auto conflicts = detectNameConflicts(resolved_name, context);
        if (!conflicts.empty()) {
            if (strict_mode) {
                addError("名称 '" + resolved_name + "' 存在冲突");
                return "";
            } else {
                for (const auto& conflict : conflicts) {
                    addWarning("名称冲突: " + conflict);
                }
            }
        }
    }
    
    return resolved_name;
}

std::string FullQualifiedNameManager::resolveSimpleName(const FullQualifiedNameParseResult& parse_result, const NameResolutionContext& context) {
    // 检查别名
    if (enable_alias_resolution) {
        std::string alias_key = generateAliasKey(parse_result.base_name, context);
        auto alias_it = name_aliases.find(alias_key);
        if (alias_it != name_aliases.end()) {
            return alias_it->second;
        }
    }
    
    // 检查当前命名空间
    if (!context.current_namespace.empty()) {
        std::string namespaced_name = context.current_namespace + "::" + parse_result.base_name;
        if (hasRegisteredName(namespaced_name)) {
            return namespaced_name;
        }
    }
    
    // 类型推断
    if (enable_type_inference) {
        auto inferred_type = inferNameType(parse_result.base_name, context);
        if (!inferred_type.empty()) {
            std::string full_name = "@" + inferred_type + " " + parse_result.base_name;
            return full_name;
        }
    }
    
    // 返回原始名称
    return parse_result.base_name;
}

std::vector<std::string> FullQualifiedNameManager::detectNameConflicts(const std::string& name, const NameResolutionContext& context) const {
    std::vector<std::string> conflicts;
    
    // 检查已注册的名称
    for (const auto& [key, registration] : name_registrations) {
        if (registration.resolved_name == name && registration.namespace_context != context.current_namespace) {
            conflicts.push_back("与命名空间 '" + registration.namespace_context + "' 中的名称冲突");
        }
    }
    
    // 检查别名冲突
    for (const auto& [alias_key, alias_value] : name_aliases) {
        if (alias_value == name) {
            conflicts.push_back("与别名 '" + extractNameFromAliasKey(alias_key) + "' 冲突");
        }
    }
    
    return conflicts;
}

std::string FullQualifiedNameManager::inferNameType(const std::string& name, const NameResolutionContext& context) const {
    // 基于命名约定推断类型
    
    // 样式相关
    if (name.find("Style") != std::string::npos || name.find("CSS") != std::string::npos) {
        return "Style";
    }
    
    // 元素相关
    if (name.find("Element") != std::string::npos || name.find("Component") != std::string::npos) {
        return "Element";
    }
    
    // 变量相关
    if (name.find("Var") != std::string::npos || name.find("Variable") != std::string::npos) {
        return "Var";
    }
    
    // 配置相关
    if (name.find("Config") != std::string::npos || name.find("Configuration") != std::string::npos) {
        return "Config";
    }
    
    // 基于上下文推断
    if (!context.expected_type.empty()) {
        return context.expected_type;
    }
    
    return ""; // 无法推断
}

// 注册和管理
bool FullQualifiedNameManager::registerName(const std::string& name, const NameRegistration& registration) {
    auto parse_result = parseName(name);
    if (!parse_result.success) {
        addError("无法注册无效名称: " + name);
        return false;
    }
    
    std::string registration_key = generateRegistrationKey(parse_result);
    
    // 检查是否已存在
    if (name_registrations.find(registration_key) != name_registrations.end()) {
        if (strict_mode) {
            addError("名称 '" + name + "' 已被注册");
            return false;
        } else {
            addWarning("名称 '" + name + "' 已存在，将被覆盖");
        }
    }
    
    name_registrations[registration_key] = registration;
    return true;
}

bool FullQualifiedNameManager::unregisterName(const std::string& name) {
    auto parse_result = parseName(name);
    if (!parse_result.success) {
        return false;
    }
    
    std::string registration_key = generateRegistrationKey(parse_result);
    auto it = name_registrations.find(registration_key);
    
    if (it != name_registrations.end()) {
        name_registrations.erase(it);
        return true;
    }
    
    return false;
}

bool FullQualifiedNameManager::hasRegisteredName(const std::string& name) const {
    auto parse_result = const_cast<FullQualifiedNameManager*>(this)->parseName(name);
    if (!parse_result.success) {
        return false;
    }
    
    std::string registration_key = generateRegistrationKey(parse_result);
    return name_registrations.find(registration_key) != name_registrations.end();
}

NameRegistration FullQualifiedNameManager::getNameRegistration(const std::string& name) const {
    auto parse_result = const_cast<FullQualifiedNameManager*>(this)->parseName(name);
    if (!parse_result.success) {
        return NameRegistration();
    }
    
    std::string registration_key = generateRegistrationKey(parse_result);
    auto it = name_registrations.find(registration_key);
    
    if (it != name_registrations.end()) {
        return it->second;
    }
    
    return NameRegistration();
}

// 别名管理
bool FullQualifiedNameManager::addAlias(const std::string& alias, const std::string& target, const NameResolutionContext& context) {
    if (alias.empty() || target.empty()) {
        addError("别名和目标名称不能为空");
        return false;
    }
    
    std::string alias_key = generateAliasKey(alias, context);
    
    // 检查别名是否已存在
    if (name_aliases.find(alias_key) != name_aliases.end()) {
        if (strict_mode) {
            addError("别名 '" + alias + "' 已存在");
            return false;
        } else {
            addWarning("别名 '" + alias + "' 已存在，将被覆盖");
        }
    }
    
    name_aliases[alias_key] = target;
    return true;
}

bool FullQualifiedNameManager::removeAlias(const std::string& alias, const NameResolutionContext& context) {
    std::string alias_key = generateAliasKey(alias, context);
    auto it = name_aliases.find(alias_key);
    
    if (it != name_aliases.end()) {
        name_aliases.erase(it);
        return true;
    }
    
    return false;
}

bool FullQualifiedNameManager::hasAlias(const std::string& alias, const NameResolutionContext& context) const {
    std::string alias_key = generateAliasKey(alias, context);
    return name_aliases.find(alias_key) != name_aliases.end();
}

std::string FullQualifiedNameManager::resolveAlias(const std::string& alias, const NameResolutionContext& context) const {
    std::string alias_key = generateAliasKey(alias, context);
    auto it = name_aliases.find(alias_key);
    
    if (it != name_aliases.end()) {
        return it->second;
    }
    
    return alias; // 如果没有别名，返回原名称
}

// 命名空间管理
bool FullQualifiedNameManager::registerNamespace(const std::string& namespace_name) {
    if (namespace_name.empty()) {
        addError("命名空间名称不能为空");
        return false;
    }
    
    if (!std::regex_match(namespace_name, simple_name_pattern)) {
        addError("命名空间名称格式无效: " + namespace_name);
        return false;
    }
    
    if (registered_namespaces.find(namespace_name) != registered_namespaces.end()) {
        addWarning("命名空间 '" + namespace_name + "' 已存在");
        return true; // 已存在但不报错
    }
    
    registered_namespaces.insert(namespace_name);
    return true;
}

bool FullQualifiedNameManager::unregisterNamespace(const std::string& namespace_name) {
    auto it = registered_namespaces.find(namespace_name);
    if (it != registered_namespaces.end()) {
        // 检查命名空间中是否还有注册的名称
        bool has_names = false;
        for (const auto& [key, registration] : name_registrations) {
            if (registration.namespace_context == namespace_name) {
                has_names = true;
                break;
            }
        }
        
        if (has_names && strict_mode) {
            addError("无法删除命名空间 '" + namespace_name + "'，其中还有注册的名称");
            return false;
        }
        
        registered_namespaces.erase(it);
        return true;
    }
    
    return false;
}

bool FullQualifiedNameManager::hasNamespace(const std::string& namespace_name) const {
    return registered_namespaces.find(namespace_name) != registered_namespaces.end();
}

std::vector<std::string> FullQualifiedNameManager::getAllNamespaces() const {
    return std::vector<std::string>(registered_namespaces.begin(), registered_namespaces.end());
}

// 冲突解决
ConflictResolutionStrategy FullQualifiedNameManager::getDefaultResolutionStrategy() const {
    return default_resolution_strategy;
}

void FullQualifiedNameManager::setDefaultResolutionStrategy(ConflictResolutionStrategy strategy) {
    default_resolution_strategy = strategy;
}

std::string FullQualifiedNameManager::resolveConflict(const std::string& name, const std::vector<std::string>& candidates, ConflictResolutionStrategy strategy) {
    if (candidates.empty()) {
        return "";
    }
    
    if (candidates.size() == 1) {
        return candidates[0];
    }
    
    switch (strategy) {
        case ConflictResolutionStrategy::FIRST_MATCH:
            return candidates[0];
            
        case ConflictResolutionStrategy::LAST_MATCH:
            return candidates.back();
            
        case ConflictResolutionStrategy::PREFER_NAMESPACE:
            {
                // 优先选择带命名空间的名称
                for (const auto& candidate : candidates) {
                    if (candidate.find("::") != std::string::npos) {
                        return candidate;
                    }
                }
                return candidates[0];
            }
            
        case ConflictResolutionStrategy::PREFER_EXPLICIT:
            {
                // 优先选择显式限定的名称
                for (const auto& candidate : candidates) {
                    if (candidate.find("[") != std::string::npos || candidate.find("@") != std::string::npos) {
                        return candidate;
                    }
                }
                return candidates[0];
            }
            
        case ConflictResolutionStrategy::ERROR:
        default:
            addError("名称 '" + name + "' 存在多个候选项，无法解决冲突");
            return "";
    }
}

// 配置选项
void FullQualifiedNameManager::enableNamespaceResolution(bool enable) {
    enable_namespace_resolution = enable;
}

void FullQualifiedNameManager::enableAliasResolution(bool enable) {
    enable_alias_resolution = enable;
}

void FullQualifiedNameManager::enableConflictDetection(bool enable) {
    enable_conflict_detection = enable;
}

void FullQualifiedNameManager::enableTypeInference(bool enable) {
    enable_type_inference = enable;
}

void FullQualifiedNameManager::setStrictMode(bool strict) {
    strict_mode = strict;
}

void FullQualifiedNameManager::setMaxResolutionDepth(size_t depth) {
    max_resolution_depth = depth;
}

// 验证和检查
bool FullQualifiedNameManager::isValidName(const std::string& name) const {
    auto result = const_cast<FullQualifiedNameManager*>(this)->parseName(name);
    return result.success;
}

std::vector<std::string> FullQualifiedNameManager::getNameSuggestions(const std::string& partial_name) const {
    std::vector<std::string> suggestions;
    
    // 基于已注册的名称生成建议
    for (const auto& [key, registration] : name_registrations) {
        if (registration.resolved_name.find(partial_name) == 0) {
            suggestions.push_back(registration.resolved_name);
        }
        if (registration.original_name.find(partial_name) == 0) {
            suggestions.push_back(registration.original_name);
        }
    }
    
    // 基于别名生成建议
    for (const auto& [alias_key, target] : name_aliases) {
        std::string alias_name = extractNameFromAliasKey(alias_key);
        if (alias_name.find(partial_name) == 0) {
            suggestions.push_back(alias_name);
        }
    }
    
    // 基于命名空间生成建议
    for (const auto& namespace_name : registered_namespaces) {
        std::string suggestion = namespace_name + "::";
        if (suggestion.find(partial_name) == 0) {
            suggestions.push_back(suggestion);
        }
    }
    
    // 去重并排序
    std::sort(suggestions.begin(), suggestions.end());
    suggestions.erase(std::unique(suggestions.begin(), suggestions.end()), suggestions.end());
    
    return suggestions;
}

// 错误处理
std::vector<std::string> FullQualifiedNameManager::getLastErrors() const {
    return errors;
}

std::vector<std::string> FullQualifiedNameManager::getLastWarnings() const {
    return warnings;
}

void FullQualifiedNameManager::clearErrors() {
    errors.clear();
}

void FullQualifiedNameManager::clearWarnings() {
    warnings.clear();
}

// 报告生成
std::string FullQualifiedNameManager::generateNameReport() const {
    std::stringstream report;
    
    report << "=== 全缀名管理器报告 ===\n\n";
    
    // 注册的命名空间
    report << "注册的命名空间 (" << registered_namespaces.size() << "):\n";
    for (const auto& ns : registered_namespaces) {
        report << "  - " << ns << "\n";
    }
    report << "\n";
    
    // 注册的名称
    report << "注册的名称 (" << name_registrations.size() << "):\n";
    for (const auto& [key, registration] : name_registrations) {
        report << "  - " << registration.original_name;
        if (registration.original_name != registration.resolved_name) {
            report << " -> " << registration.resolved_name;
        }
        if (!registration.namespace_context.empty()) {
            report << " (命名空间: " << registration.namespace_context << ")";
        }
        report << "\n";
    }
    report << "\n";
    
    // 别名
    report << "别名映射 (" << name_aliases.size() << "):\n";
    for (const auto& [alias_key, target] : name_aliases) {
        std::string alias_name = extractNameFromAliasKey(alias_key);
        report << "  - " << alias_name << " -> " << target << "\n";
    }
    
    return report.str();
}

// 私有辅助方法
std::string FullQualifiedNameManager::generateRegistrationKey(const FullQualifiedNameParseResult& parse_result) const {
    std::stringstream key;
    
    if (!parse_result.type_prefix.empty()) {
        key << "[" << parse_result.type_prefix << "]";
    }
    
    if (!parse_result.type_modifier.empty()) {
        if (key.tellp() > 0) key << " ";
        key << "@" << parse_result.type_modifier;
    }
    
    if (!parse_result.namespace_name.empty()) {
        if (key.tellp() > 0) key << " ";
        key << parse_result.namespace_name << "::";
    }
    
    if (key.tellp() > 0) key << " ";
    key << parse_result.base_name;
    
    return key.str();
}

std::string FullQualifiedNameManager::generateAliasKey(const std::string& alias, const NameResolutionContext& context) const {
    return context.current_namespace + "::" + context.current_file + "::" + alias;
}

std::string FullQualifiedNameManager::extractNameFromAliasKey(const std::string& alias_key) const {
    size_t last_separator = alias_key.find_last_of("::");
    if (last_separator != std::string::npos && last_separator > 0) {
        return alias_key.substr(last_separator + 1);
    }
    return alias_key;
}

void FullQualifiedNameManager::addError(const std::string& message) const {
    errors.push_back(message);
}

void FullQualifiedNameManager::addWarning(const std::string& message) const {
    warnings.push_back(message);
}

// ===== FullQualifiedNameUtils Implementation =====

FullQualifiedNameType FullQualifiedNameUtils::parseType(const std::string& name) {
    if (name.empty()) return FullQualifiedNameType::SIMPLE;
    
    // 检查是否包含类型前缀和修饰符
    if (name.find("[") != std::string::npos && name.find("@") != std::string::npos) {
        return FullQualifiedNameType::FULL_QUALIFIED;
    }
    
    // 检查命名空间
    if (name.find("::") != std::string::npos) {
        return FullQualifiedNameType::NAMESPACED;
    }
    
    // 检查是否只有类型修饰符
    if (name.find("@") != std::string::npos) {
        return FullQualifiedNameType::FULL_QUALIFIED;
    }
    
    return FullQualifiedNameType::SIMPLE;
}

bool FullQualifiedNameUtils::isFullyQualified(const std::string& name) {
    return parseType(name) == FullQualifiedNameType::FULL_QUALIFIED;
}

bool FullQualifiedNameUtils::isNamespaced(const std::string& name) {
    return name.find("::") != std::string::npos;
}

bool FullQualifiedNameUtils::hasTypePrefix(const std::string& name) {
    return name.find("[") != std::string::npos && name.find("]") != std::string::npos;
}

bool FullQualifiedNameUtils::hasTypeModifier(const std::string& name) {
    return name.find("@") != std::string::npos;
}

std::string FullQualifiedNameUtils::extractBaseName(const std::string& name) {
    std::string base_name = name;
    
    // 移除命名空间前缀
    size_t namespace_pos = base_name.find("::");
    if (namespace_pos != std::string::npos) {
        base_name = base_name.substr(namespace_pos + 2);
    }
    
    // 移除类型前缀和修饰符
    std::regex cleanup_pattern(R"(^(\[[^\]]+\]\s*)?(@[A-Za-z]+\s+)?(.+)$)");
    std::smatch matches;
    
    if (std::regex_match(base_name, matches, cleanup_pattern)) {
        return matches[3].str();
    }
    
    return base_name;
}

std::string FullQualifiedNameUtils::extractNamespace(const std::string& name) {
    size_t namespace_pos = name.find("::");
    if (namespace_pos != std::string::npos) {
        return name.substr(0, namespace_pos);
    }
    return "";
}

std::string FullQualifiedNameUtils::extractTypePrefix(const std::string& name) {
    std::regex prefix_pattern(R"^\[([^\]]+)\]");
    std::smatch matches;
    
    if (std::regex_search(name, matches, prefix_pattern)) {
        return matches[1].str();
    }
    
    return "";
}

std::string FullQualifiedNameUtils::extractTypeModifier(const std::string& name) {
    std::regex modifier_pattern(R"@([A-Za-z]+)");
    std::smatch matches;
    
    if (std::regex_search(name, matches, modifier_pattern)) {
        return matches[1].str();
    }
    
    return "";
}

std::string FullQualifiedNameUtils::normalizeName(const std::string& name) {
    std::string normalized = name;
    
    // 移除多余空格
    std::regex extra_spaces(R"(\s+)");
    normalized = std::regex_replace(normalized, extra_spaces, " ");
    
    // 标准化分隔符周围的空格
    std::regex namespace_separator(R"(\s*::\s*)");
    normalized = std::regex_replace(normalized, namespace_separator, "::");
    
    // 去除首尾空格
    normalized.erase(0, normalized.find_first_not_of(" \t\n\r"));
    normalized.erase(normalized.find_last_not_of(" \t\n\r") + 1);
    
    return normalized;
}

bool FullQualifiedNameUtils::isValidNameChar(char c) {
    return std::isalnum(c) || c == '_' || c == ':' || c == '[' || c == ']' || 
           c == '@' || c == ' ';
}

std::string FullQualifiedNameUtils::generateUniqueId(const std::string& base_name, const std::string& context) {
    static std::unordered_map<std::string, int> name_counters;
    
    std::string key = context + "::" + base_name;
    int& counter = name_counters[key];
    
    if (counter == 0) {
        counter = 1;
        return base_name;
    } else {
        return base_name + "_" + std::to_string(counter++);
    }
}

// ===== 名称模式 =====

namespace NamePatterns {
    const std::regex FULL_QUALIFIED_NAME(R"(\[([A-Za-z]+)\]\s*@([A-Za-z]+)\s+([A-Za-z_][A-Za-z0-9_]*))");
    const std::regex NAMESPACED_NAME(R"(([A-Za-z_][A-Za-z0-9_]*)::\s*([A-Za-z_][A-Za-z0-9_]*))");
    const std::regex TYPE_PREFIX_NAME(R"(\[([A-Za-z]+)\]\s+([A-Za-z_][A-Za-z0-9_]*))");
    const std::regex TYPE_MODIFIER_NAME(R"(@([A-Za-z]+)\s+([A-Za-z_][A-Za-z0-9_]*))");
    const std::regex SIMPLE_NAME(R"(^[A-Za-z_][A-Za-z0-9_]*$)");
    const std::regex NAMESPACE_SEPARATOR(R"(::)");
    const std::regex TYPE_PREFIX(R"(\[([A-Za-z]+)\])");
    const std::regex TYPE_MODIFIER(R"(@([A-Za-z]+))");
}

} // namespace CHTL