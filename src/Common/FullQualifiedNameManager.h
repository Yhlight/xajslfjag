#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <functional>

namespace CHTL {
namespace Common {

// 全缀名类型
enum class FullQualifiedNameType {
    CUSTOM_ELEMENT,         // [Custom] @Element
    CUSTOM_STYLE,           // [Custom] @Style  
    CUSTOM_VAR,             // [Custom] @Var
    TEMPLATE_ELEMENT,       // [Template] @Element
    TEMPLATE_STYLE,         // [Template] @Style
    TEMPLATE_VAR,           // [Template] @Var
    ORIGIN_ITEM,            // [Origin] @Type
    CONFIGURATION_ITEM,     // [Configuration] @Config
    NAMESPACE_QUALIFIED,    // Namespace::Item
    UNKNOWN_QUALIFIED
};

// 名称组件
struct NameComponent {
    std::string prefix;                    // 前缀 ([Custom], [Template]等)
    std::string type;                      // 类型 (@Element, @Style等)
    std::string name;                      // 名称
    std::string namespace_name;            // 命名空间名称
    bool has_prefix;                       // 是否有前缀
    bool has_namespace;                    // 是否有命名空间
    
    NameComponent() : has_prefix(false), has_namespace(false) {}
    
    std::string getFullName() const {
        std::string result;
        if (has_namespace && !namespace_name.empty()) {
            result += namespace_name + "::";
        }
        if (has_prefix && !prefix.empty()) {
            result += prefix + " ";
        }
        if (!type.empty()) {
            result += type + " ";
        }
        result += name;
        return result;
    }
    
    std::string getShortName() const {
        return name;
    }
    
    std::string getTypedName() const {
        return type + " " + name;
    }
};

// 全缀名解析结果
struct FullQualifiedNameParseResult {
    bool success;                          // 解析是否成功
    FullQualifiedNameType type;            // 全缀名类型
    NameComponent component;               // 名称组件
    std::string original_name;             // 原始名称
    std::string resolved_name;             // 解析后的名称
    std::string target_namespace;          // 目标命名空间
    std::vector<std::string> candidate_names;  // 候选名称
    std::string error_message;             // 错误信息
    
    FullQualifiedNameParseResult() : success(false), type(FullQualifiedNameType::UNKNOWN_QUALIFIED) {}
};

// 名称解析上下文
struct NameResolutionContext {
    std::string current_namespace;         // 当前命名空间
    std::string current_file;              // 当前文件
    std::vector<std::string> import_namespaces;  // 导入的命名空间
    std::unordered_map<std::string, std::string> aliases;  // 别名映射
    std::vector<std::string> search_order; // 搜索顺序
    bool strict_resolution;                // 严格解析模式
    
    NameResolutionContext() : strict_resolution(false) {}
    
    void addImportNamespace(const std::string& ns) {
        if (std::find(import_namespaces.begin(), import_namespaces.end(), ns) == import_namespaces.end()) {
            import_namespaces.push_back(ns);
        }
    }
    
    void addAlias(const std::string& alias, const std::string& target) {
        aliases[alias] = target;
    }
    
    std::string resolveAlias(const std::string& name) const {
        auto it = aliases.find(name);
        return (it != aliases.end()) ? it->second : name;
    }
};

// 名称注册信息
struct NameRegistration {
    std::string qualified_name;            // 全限定名
    std::string short_name;                // 短名称
    FullQualifiedNameType type;            // 类型
    std::string namespace_name;            // 命名空间
    std::string source_file;               // 源文件
    bool is_exported;                      // 是否导出
    bool is_accessible;                    // 是否可访问
    std::unordered_map<std::string, std::string> metadata;  // 元数据
    
    NameRegistration() : type(FullQualifiedNameType::UNKNOWN_QUALIFIED), 
                        is_exported(true), is_accessible(true) {}
};

// 冲突解决策略
enum class ConflictResolutionStrategy {
    PREFER_CURRENT_NAMESPACE,  // 优先当前命名空间
    PREFER_IMPORTED,           // 优先导入的
    PREFER_EXPLICIT,           // 优先显式指定的
    REPORT_ERROR,              // 报告错误
    USE_FIRST_FOUND           // 使用第一个找到的
};

// 全缀名管理器
class FullQualifiedNameManager {
private:
    std::unordered_map<std::string, NameRegistration> registered_names;
    std::unordered_map<std::string, std::vector<std::string>> namespace_names;
    std::unordered_map<std::string, std::vector<std::string>> type_names;
    std::unordered_map<std::string, std::string> short_name_to_qualified;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    // 配置选项
    ConflictResolutionStrategy conflict_strategy;
    bool enable_namespace_fallback;
    bool enable_type_inference;
    bool strict_name_validation;
    
    // 内部方法
    FullQualifiedNameParseResult parseCustomName(const std::string& name);
    FullQualifiedNameParseResult parseTemplateName(const std::string& name);
    FullQualifiedNameParseResult parseOriginName(const std::string& name);
    FullQualifiedNameParseResult parseConfigurationName(const std::string& name);
    FullQualifiedNameParseResult parseNamespaceName(const std::string& name);
    
    std::vector<std::string> searchInNamespace(const std::string& namespace_name, 
                                              const std::string& short_name,
                                              FullQualifiedNameType type = FullQualifiedNameType::UNKNOWN_QUALIFIED);
    
    std::string resolveConflict(const std::vector<std::string>& candidates, 
                               const NameResolutionContext& context);
    
public:
    FullQualifiedNameManager();
    ~FullQualifiedNameManager() = default;
    
    // 名称注册
    void registerName(const NameRegistration& registration);
    void unregisterName(const std::string& qualified_name);
    bool hasRegisteredName(const std::string& qualified_name) const;
    const NameRegistration* getRegistration(const std::string& qualified_name) const;
    
    // 名称解析
    FullQualifiedNameParseResult parseName(const std::string& name);
    FullQualifiedNameParseResult resolveName(const std::string& name, 
                                            const NameResolutionContext& context);
    std::string resolveToQualifiedName(const std::string& name, 
                                      const NameResolutionContext& context);
    std::vector<std::string> findCandidateNames(const std::string& short_name, 
                                               const NameResolutionContext& context);
    
    // 全缀名验证
    bool isValidFullQualifiedName(const std::string& name);
    bool isFullQualifiedName(const std::string& name);
    FullQualifiedNameType detectNameType(const std::string& name);
    
    // 名称转换
    std::string convertToFullQualified(const std::string& short_name, 
                                      FullQualifiedNameType type,
                                      const std::string& namespace_name = "");
    std::string convertToShortName(const std::string& qualified_name);
    std::string addNamespacePrefix(const std::string& name, const std::string& namespace_name);
    std::string removeNamespacePrefix(const std::string& qualified_name);
    
    // 类型特定操作
    void registerCustomElement(const std::string& name, const std::string& namespace_name = "");
    void registerCustomStyle(const std::string& name, const std::string& namespace_name = "");
    void registerCustomVar(const std::string& name, const std::string& namespace_name = "");
    void registerTemplateElement(const std::string& name, const std::string& namespace_name = "");
    void registerTemplateStyle(const std::string& name, const std::string& namespace_name = "");
    void registerTemplateVar(const std::string& name, const std::string& namespace_name = "");
    void registerOriginItem(const std::string& name, const std::string& type, const std::string& namespace_name = "");
    void registerConfiguration(const std::string& name, const std::string& namespace_name = "");
    
    // 命名空间管理
    void addNamespace(const std::string& namespace_name);
    void removeNamespace(const std::string& namespace_name);
    std::vector<std::string> getNamespacesForName(const std::string& short_name) const;
    std::vector<std::string> getNamesInNamespace(const std::string& namespace_name) const;
    std::vector<std::string> getAllNamespaces() const;
    
    // 冲突检测和解决
    std::vector<std::string> detectNameConflicts() const;
    std::vector<std::string> getConflictingNames(const std::string& short_name) const;
    void setConflictResolutionStrategy(ConflictResolutionStrategy strategy);
    ConflictResolutionStrategy getConflictResolutionStrategy() const;
    
    // 搜索和查询
    std::vector<NameRegistration> findNamesByPattern(const std::string& pattern) const;
    std::vector<NameRegistration> findNamesByType(FullQualifiedNameType type) const;
    std::vector<NameRegistration> findNamesByNamespace(const std::string& namespace_name) const;
    std::vector<std::string> suggestSimilarNames(const std::string& name) const;
    
    // 导入和别名处理
    void processImportStatement(const std::string& import_statement, NameResolutionContext& context);
    void addAliasMapping(const std::string& alias, const std::string& qualified_name);
    void removeAliasMapping(const std::string& alias);
    std::string resolveAlias(const std::string& alias) const;
    
    // 验证和检查
    bool validateNameAccess(const std::string& qualified_name, const NameResolutionContext& context) const;
    std::vector<std::string> validateNameResolution(const std::string& name, 
                                                   const NameResolutionContext& context) const;
    bool isNameAccessible(const std::string& qualified_name, const std::string& from_namespace = "") const;
    
    // 配置管理
    void enableNamespaceFallback(bool enable = true);
    void enableTypeInference(bool enable = true);
    void enableStrictValidation(bool enable = true);
    void setConfiguration(const std::string& key, const std::string& value);
    std::string getConfiguration(const std::string& key, const std::string& default_val = "") const;
    
    // 错误处理
    std::vector<std::string> getErrors() const { return errors; }
    std::vector<std::string> getWarnings() const { return warnings; }
    void clearErrors() { errors.clear(); }
    void clearWarnings() { warnings.clear(); }
    
    // 调试和报告
    std::string generateNameReport() const;
    std::string generateNamespaceReport() const;
    std::string generateConflictReport() const;
    void dumpRegisteredNames() const;
    
    // 序列化
    std::string serializeRegistrations() const;
    void deserializeRegistrations(const std::string& data);
    void exportToFile(const std::string& file_path) const;
    void importFromFile(const std::string& file_path);
    
private:
    void addError(const std::string& message);
    void addWarning(const std::string& message);
    
    // 内部解析方法
    NameComponent parseNameComponent(const std::string& name);
    std::string extractPrefix(const std::string& name);
    std::string extractType(const std::string& name);
    std::string extractNamespace(const std::string& name);
    std::string extractBaseName(const std::string& name);
    
    // 内部验证方法
    bool isValidPrefix(const std::string& prefix);
    bool isValidType(const std::string& type);
    bool isValidBaseName(const std::string& name);
    bool isValidNamespace(const std::string& namespace_name);
    
    // 内部搜索方法
    std::vector<std::string> searchByShortName(const std::string& short_name);
    std::vector<std::string> searchByType(FullQualifiedNameType type);
    std::vector<std::string> searchInContext(const std::string& name, const NameResolutionContext& context);
    
    // 相似度计算
    double calculateNameSimilarity(const std::string& name1, const std::string& name2);
    
    // 配置存储
    std::unordered_map<std::string, std::string> configuration;
};

// 全缀名工具类
class FullQualifiedNameUtils {
public:
    // 名称解析
    static bool isFullQualifiedName(const std::string& name);
    static bool hasPrefix(const std::string& name);
    static bool hasNamespace(const std::string& name);
    static FullQualifiedNameType detectType(const std::string& name);
    
    // 名称分解
    static std::string extractPrefix(const std::string& qualified_name);
    static std::string extractType(const std::string& qualified_name);
    static std::string extractBaseName(const std::string& qualified_name);
    static std::string extractNamespace(const std::string& qualified_name);
    
    // 名称构建
    static std::string buildQualifiedName(const std::string& prefix, const std::string& type, 
                                         const std::string& name, const std::string& namespace_name = "");
    static std::string addPrefix(const std::string& name, const std::string& prefix);
    static std::string addNamespace(const std::string& name, const std::string& namespace_name);
    static std::string removePrefix(const std::string& qualified_name);
    static std::string removeNamespace(const std::string& qualified_name);
    
    // 名称验证
    static bool isValidQualifiedName(const std::string& name);
    static bool isValidPrefix(const std::string& prefix);
    static bool isValidType(const std::string& type);
    static bool isValidBaseName(const std::string& name);
    static std::vector<std::string> validateNameFormat(const std::string& name);
    
    // 名称比较
    static bool areNamesEquivalent(const std::string& name1, const std::string& name2);
    static bool isNameMatch(const std::string& qualified_name, const std::string& short_name);
    static bool isTypeMatch(const std::string& qualified_name, FullQualifiedNameType type);
    static bool isNamespaceMatch(const std::string& qualified_name, const std::string& namespace_name);
    
    // 名称转换
    static std::string normalizeQualifiedName(const std::string& name);
    static std::string canonicalizeQualifiedName(const std::string& name);
    static std::string shortenQualifiedName(const std::string& name, const std::string& current_namespace = "");
    
    // 字符串处理
    static std::string trim(const std::string& str);
    static std::vector<std::string> splitQualifiedName(const std::string& name);
    static std::string joinNameParts(const std::vector<std::string>& parts);
    
    // 类型映射
    static std::string typeToString(FullQualifiedNameType type);
    static FullQualifiedNameType stringToType(const std::string& type_str);
    static std::vector<std::string> getAllValidTypes();
    static std::vector<std::string> getAllValidPrefixes();
};

// 预定义名称模式
namespace NamePatterns {
    extern const std::string CUSTOM_ELEMENT_PATTERN;
    extern const std::string CUSTOM_STYLE_PATTERN;
    extern const std::string CUSTOM_VAR_PATTERN;
    extern const std::string TEMPLATE_ELEMENT_PATTERN;
    extern const std::string TEMPLATE_STYLE_PATTERN;
    extern const std::string TEMPLATE_VAR_PATTERN;
    extern const std::string ORIGIN_PATTERN;
    extern const std::string CONFIGURATION_PATTERN;
    extern const std::string NAMESPACE_PATTERN;
    extern const std::string QUALIFIED_NAME_PATTERN;
}

} // namespace Common
} // namespace CHTL