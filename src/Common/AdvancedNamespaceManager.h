#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <functional>

namespace CHTL {
namespace Common {

// 命名空间类型
enum class NamespaceType {
    EXPLICIT,       // 显式声明的命名空间
    DEFAULT,        // 默认命名空间（从文件名生成）
    IMPORTED,       // 导入的命名空间
    MERGED,         // 合并的命名空间
    VIRTUAL         // 虚拟命名空间（用于组织）
};

// 命名空间作用域
enum class NamespaceScope {
    FILE,           // 文件级作用域
    MODULE,         // 模块级作用域
    GLOBAL,         // 全局作用域
    TEMPORARY       // 临时作用域
};

// 命名空间冲突类型
enum class ConflictType {
    NAME_COLLISION,     // 名称冲突
    TYPE_MISMATCH,      // 类型不匹配
    SCOPE_VIOLATION,    // 作用域违规
    CIRCULAR_REFERENCE, // 循环引用
    ACCESS_DENIED       // 访问拒绝
};

// 命名空间元素
struct NamespaceElement {
    std::string name;           // 元素名称
    std::string type;           // 元素类型 (@Style, @Element, @Var等)
    std::string source_file;    // 源文件路径
    NamespaceScope scope;       // 作用域
    std::unordered_map<std::string, std::string> metadata;  // 元数据
    bool is_exported;           // 是否导出
    bool is_accessible;         // 是否可访问
    
    NamespaceElement() : scope(NamespaceScope::FILE), is_exported(true), is_accessible(true) {}
    NamespaceElement(const std::string& n, const std::string& t, const std::string& file) 
        : name(n), type(t), source_file(file), scope(NamespaceScope::FILE), 
          is_exported(true), is_accessible(true) {}
};

// 命名空间定义
struct NamespaceDefinition {
    std::string name;                               // 命名空间名称
    NamespaceType type;                            // 命名空间类型
    NamespaceScope scope;                          // 作用域
    std::string source_file;                       // 定义文件
    std::vector<std::string> parent_namespaces;    // 父命名空间
    std::vector<std::string> child_namespaces;     // 子命名空间
    std::unordered_map<std::string, NamespaceElement> elements;  // 包含的元素
    std::unordered_set<std::string> dependencies;  // 依赖的命名空间
    std::unordered_map<std::string, std::string> aliases;       // 别名映射
    bool auto_merge_enabled;                       // 是否启用自动合并
    bool default_namespace_disabled;               // 是否禁用默认命名空间
    
    NamespaceDefinition() : type(NamespaceType::EXPLICIT), scope(NamespaceScope::FILE),
                           auto_merge_enabled(true), default_namespace_disabled(false) {}
};

// 命名空间冲突信息
struct NamespaceConflict {
    ConflictType type;              // 冲突类型
    std::string namespace1;         // 冲突的命名空间1
    std::string namespace2;         // 冲突的命名空间2
    std::string element_name;       // 冲突的元素名称
    std::string description;        // 冲突描述
    std::vector<std::string> suggested_solutions;  // 建议的解决方案
    bool is_resolvable;            // 是否可以自动解决
    
    NamespaceConflict(ConflictType t, const std::string& ns1, const std::string& ns2, 
                     const std::string& element = "")
        : type(t), namespace1(ns1), namespace2(ns2), element_name(element), is_resolvable(false) {}
};

// 命名空间解析结果
struct NamespaceResolutionResult {
    bool success;                           // 解析是否成功
    std::string resolved_namespace;         // 解析后的命名空间
    std::string resolved_element;           // 解析后的元素
    std::string full_qualified_name;       // 完全限定名
    std::vector<std::string> search_path;  // 搜索路径
    std::vector<NamespaceConflict> conflicts;  // 发现的冲突
    std::string error_message;             // 错误消息
    
    NamespaceResolutionResult() : success(false) {}
};

// 高级命名空间管理器
class AdvancedNamespaceManager {
private:
    std::unordered_map<std::string, NamespaceDefinition> namespaces;
    std::unordered_map<std::string, std::vector<std::string>> merged_namespaces;
    std::unordered_map<std::string, std::string> file_to_namespace;
    std::unordered_map<std::string, std::string> default_namespaces;
    std::vector<NamespaceConflict> conflict_history;
    
    // 配置选项
    bool auto_merge_enabled;
    bool default_namespace_enabled;
    bool conflict_detection_enabled;
    bool circular_reference_detection;
    
    // 内部方法
    void detectConflicts(const std::string& namespace_name);
    void autoMergeNamespaces(const std::string& namespace_name);
    void validateNamespaceHierarchy();
    std::string generateDefaultNamespace(const std::string& file_path);
    bool isCircularReference(const std::string& ns1, const std::string& ns2);
    
public:
    AdvancedNamespaceManager();
    ~AdvancedNamespaceManager() = default;
    
    // 命名空间管理
    void addNamespace(const NamespaceDefinition& namespace_def);
    void removeNamespace(const std::string& namespace_name);
    bool hasNamespace(const std::string& namespace_name) const;
    const NamespaceDefinition* getNamespace(const std::string& namespace_name) const;
    std::vector<std::string> getAllNamespaces() const;
    
    // 元素管理
    void addElement(const std::string& namespace_name, const NamespaceElement& element);
    void removeElement(const std::string& namespace_name, const std::string& element_name);
    bool hasElement(const std::string& namespace_name, const std::string& element_name) const;
    const NamespaceElement* getElement(const std::string& namespace_name, const std::string& element_name) const;
    
    // 自动合并功能
    void enableAutoMerge(bool enable = true);
    bool isAutoMergeEnabled() const;
    void mergeNamespaces(const std::string& target_namespace, const std::string& source_namespace);
    std::vector<std::string> getMergedNamespaces(const std::string& namespace_name) const;
    
    // 默认命名空间
    void enableDefaultNamespace(bool enable = true);
    bool isDefaultNamespaceEnabled() const;
    void setDefaultNamespace(const std::string& file_path, const std::string& namespace_name = "");
    std::string getDefaultNamespace(const std::string& file_path) const;
    void disableDefaultNamespaceForFile(const std::string& file_path);
    
    // 冲突检测和解决
    void enableConflictDetection(bool enable = true);
    std::vector<NamespaceConflict> detectAllConflicts();
    std::vector<NamespaceConflict> detectConflicts(const std::string& namespace_name);
    bool resolveConflict(const NamespaceConflict& conflict);
    void clearConflictHistory();
    std::vector<NamespaceConflict> getConflictHistory() const;
    
    // 命名空间解析
    NamespaceResolutionResult resolveElement(const std::string& element_reference);
    NamespaceResolutionResult resolveElement(const std::string& namespace_name, const std::string& element_name);
    std::vector<std::string> getSearchPath(const std::string& current_namespace = "") const;
    
    // 导入和依赖管理
    void addDependency(const std::string& namespace_name, const std::string& dependency);
    void removeDependency(const std::string& namespace_name, const std::string& dependency);
    std::vector<std::string> getDependencies(const std::string& namespace_name) const;
    std::vector<std::string> getTransitiveDependencies(const std::string& namespace_name) const;
    bool hasCircularDependency(const std::string& namespace_name) const;
    
    // 别名管理
    void addAlias(const std::string& namespace_name, const std::string& alias, const std::string& target);
    void removeAlias(const std::string& namespace_name, const std::string& alias);
    std::string resolveAlias(const std::string& namespace_name, const std::string& alias) const;
    std::unordered_map<std::string, std::string> getAliases(const std::string& namespace_name) const;
    
    // 导入文件处理
    void processImportedFile(const std::string& file_path, const std::string& importing_namespace = "");
    void setFileNamespace(const std::string& file_path, const std::string& namespace_name);
    std::string getFileNamespace(const std::string& file_path) const;
    
    // 查询和搜索
    std::vector<NamespaceElement> findElements(const std::string& pattern) const;
    std::vector<NamespaceElement> findElementsByType(const std::string& type) const;
    std::vector<std::string> findNamespaces(const std::string& pattern) const;
    
    // 验证和检查
    bool validateNamespaceName(const std::string& name) const;
    bool validateElementAccess(const std::string& namespace_name, const std::string& element_name, 
                              const std::string& accessing_namespace = "") const;
    std::vector<std::string> getAccessibleElements(const std::string& namespace_name, 
                                                  const std::string& accessing_namespace = "") const;
    
    // 配置管理
    void setConfiguration(const std::string& key, const std::string& value);
    std::string getConfiguration(const std::string& key, const std::string& default_val = "") const;
    void resetToDefaults();
    
    // 报告和调试
    std::string generateNamespaceReport() const;
    std::string generateConflictReport() const;
    std::string generateDependencyGraph() const;
    void exportNamespaceData(const std::string& file_path) const;
    void importNamespaceData(const std::string& file_path);
};

// 命名空间工具类
class NamespaceUtils {
public:
    // 名称验证
    static bool isValidNamespaceName(const std::string& name);
    static bool isValidElementName(const std::string& name);
    static bool isReservedName(const std::string& name);
    
    // 路径处理
    static std::string extractNamespaceFromPath(const std::string& file_path);
    static std::string normalizeNamespaceName(const std::string& name);
    static std::string getParentNamespace(const std::string& namespace_name);
    static std::vector<std::string> splitNamespacePath(const std::string& full_name);
    
    // 冲突解决建议
    static std::vector<std::string> suggestConflictResolution(const NamespaceConflict& conflict);
    static std::string generateUniqueElementName(const std::string& base_name, 
                                                const std::vector<std::string>& existing_names);
    
    // 搜索和匹配
    static bool matchesPattern(const std::string& name, const std::string& pattern);
    static std::vector<std::string> findSimilarNames(const std::string& target, 
                                                    const std::vector<std::string>& candidates);
    
    // 依赖分析
    static std::vector<std::string> topologicalSort(const std::unordered_map<std::string, std::vector<std::string>>& dependencies);
    static bool hasCycle(const std::unordered_map<std::string, std::vector<std::string>>& dependencies);
    
    // 序列化
    static std::string serializeNamespaceDefinition(const NamespaceDefinition& def);
    static NamespaceDefinition deserializeNamespaceDefinition(const std::string& data);
};

// 命名空间配置常量
namespace NamespaceConfig {
    extern const std::string AUTO_MERGE_ENABLED;
    extern const std::string DEFAULT_NAMESPACE_ENABLED;
    extern const std::string CONFLICT_DETECTION_ENABLED;
    extern const std::string CIRCULAR_REFERENCE_DETECTION;
    extern const std::string MAX_MERGE_DEPTH;
    extern const std::string DEFAULT_NAMESPACE_PREFIX;
    extern const std::string RESERVED_NAMESPACE_NAMES;
}

} // namespace Common
} // namespace CHTL