#pragma once

#include "NamespaceManager.h"
#include "../../Util/Common.h"
#include <unordered_map>
#include <vector>
#include <memory>
#include <functional>

namespace CHTL {

// 命名空间类型
enum class NamespaceType {
    ROOT,           // 根命名空间
    NESTED,         // 嵌套命名空间
    IMPORTED,       // 导入的命名空间
    TEMPORARY,      // 临时命名空间
    VIRTUAL         // 虚拟命名空间 (用于解析)
};

// 命名空间可见性
enum class NamespaceVisibility {
    PUBLIC,         // 公共命名空间
    PRIVATE,        // 私有命名空间
    PROTECTED,      // 受保护命名空间
    INTERNAL        // 内部命名空间
};

// 冲突类型
enum class ConflictType {
    NAME_COLLISION,      // 名称冲突
    TYPE_MISMATCH,       // 类型不匹配
    SCOPE_VIOLATION,     // 作用域违反
    CIRCULAR_DEPENDENCY, // 循环依赖
    ACCESS_VIOLATION,    // 访问违反
    VERSION_CONFLICT     // 版本冲突
};

// 冲突解决策略
enum class ConflictResolutionStrategy {
    ERROR,          // 报错
    WARN,           // 警告
    OVERRIDE,       // 覆盖
    MERGE,          // 合并
    IGNORE,         // 忽略
    PROMPT          // 提示用户选择
};

// 冲突信息
struct ConflictInfo {
    ConflictType type;              // 冲突类型
    String description;             // 冲突描述
    String namespaceName1;          // 第一个命名空间
    String namespaceName2;          // 第二个命名空间
    String conflictingItem;         // 冲突项目
    String suggestion;              // 解决建议
    ConflictResolutionStrategy recommendedStrategy; // 推荐策略
    
    ConflictInfo() : type(ConflictType::NAME_COLLISION), recommendedStrategy(ConflictResolutionStrategy::ERROR) {}
};

// 高级命名空间信息
struct AdvancedNamespaceInfo : public NamespaceInfo {
    NamespaceType type;                    // 命名空间类型
    NamespaceVisibility visibility;        // 可见性
    String parentNamespace;                // 父命名空间
    StringVector childNamespaces;          // 子命名空间列表
    StringVector importedNamespaces;       // 导入的命名空间
    StringVector dependencies;             // 依赖的命名空间
    std::unordered_map<String, String> aliases; // 别名映射
    String version;                        // 版本信息
    String author;                         // 作者
    String description;                    // 描述
    bool isSealed;                         // 是否密封 (不允许修改)
    bool isAbstract;                       // 是否抽象 (不能实例化)
    size_t depth;                          // 嵌套深度
    double lastModified;                   // 最后修改时间
    
    AdvancedNamespaceInfo() : type(NamespaceType::ROOT), visibility(NamespaceVisibility::PUBLIC),
                             isSealed(false), isAbstract(false), depth(0), lastModified(0.0) {}
};

// 高级命名空间管理器
class AdvancedNamespaceManager : public NamespaceManager {
public:
    AdvancedNamespaceManager();
    ~AdvancedNamespaceManager() override = default;
    
    // 高级命名空间操作
    bool createNamespace(const String& name, NamespaceType type = NamespaceType::ROOT,
                        NamespaceVisibility visibility = NamespaceVisibility::PUBLIC);
    bool createNestedNamespace(const String& parentName, const String& childName);
    bool createNamespaceHierarchy(const StringVector& hierarchy);
    
    // 命名空间关系管理
    bool setParentNamespace(const String& childName, const String& parentName);
    String getParentNamespace(const String& namespaceName) const;
    StringVector getChildNamespaces(const String& namespaceName) const;
    StringVector getAllAncestors(const String& namespaceName) const;
    StringVector getAllDescendants(const String& namespaceName) const;
    size_t getNamespaceDepth(const String& namespaceName) const;
    
    // 命名空间导入和依赖
    bool importNamespace(const String& targetNamespace, const String& sourceNamespace, const String& alias = "");
    bool addNamespaceDependency(const String& namespaceName, const String& dependency);
    StringVector getNamespaceDependencies(const String& namespaceName) const;
    StringVector getImportedNamespaces(const String& namespaceName) const;
    
    // 别名管理
    bool addNamespaceAlias(const String& namespaceName, const String& itemName, const String& alias);
    String resolveAlias(const String& namespaceName, const String& alias) const;
    StringVector getAliases(const String& namespaceName, const String& itemName) const;
    
    // 命名空间合并
    bool mergeNamespaces(const String& targetNamespace, const String& sourceNamespace);
    bool canMergeNamespaces(const String& namespace1, const String& namespace2) const;
    StringVector getMergeConflicts(const String& namespace1, const String& namespace2) const;
    
    // 冲突检测和解决
    std::vector<ConflictInfo> detectAllConflicts() const;
    std::vector<ConflictInfo> detectNamespaceConflicts(const String& namespaceName) const;
    bool hasConflicts(const String& namespaceName) const;
    bool resolveConflict(const ConflictInfo& conflict, ConflictResolutionStrategy strategy);
    void setConflictResolutionStrategy(ConflictType type, ConflictResolutionStrategy strategy);
    
    // 循环依赖检测
    bool hasCircularDependencies() const;
    StringVector findCircularDependencies() const;
    bool wouldCreateCircularDependency(const String& namespaceName, const String& dependency) const;
    
    // 访问控制
    bool setNamespaceVisibility(const String& namespaceName, NamespaceVisibility visibility);
    NamespaceVisibility getNamespaceVisibility(const String& namespaceName) const;
    bool canAccess(const String& accessorNamespace, const String& targetNamespace) const;
    bool canAccess(const String& accessorNamespace, const String& targetNamespace, const String& itemName) const;
    
    // 命名空间查找和解析
    String resolveFullyQualifiedName(const String& namespaceName, const String& itemName) const;
    String findItemInNamespace(const String& itemName, const String& startNamespace = "") const;
    StringVector findAllMatches(const String& itemName, const String& startNamespace = "") const;
    String resolveNamespaceReference(const String& reference, const String& currentNamespace = "") const;
    
    // 命名空间验证
    bool validateNamespaceName(const String& name) const;
    bool validateNamespaceHierarchy() const;
    StringVector getValidationErrors() const;
    bool isValidNamespaceStructure() const;
    
    // 命名空间优化
    void optimizeNamespaceStructure();
    void compactNamespaces();
    void eliminateUnusedNamespaces();
    void optimizeImports();
    
    // 版本管理
    bool setNamespaceVersion(const String& namespaceName, const String& version);
    String getNamespaceVersion(const String& namespaceName) const;
    bool isVersionCompatible(const String& namespaceName, const String& requiredVersion) const;
    std::vector<ConflictInfo> checkVersionConflicts() const;
    
    // 密封和抽象命名空间
    bool sealNamespace(const String& namespaceName);
    bool unsealNamespace(const String& namespaceName);
    bool isNamespaceSealed(const String& namespaceName) const;
    bool markNamespaceAbstract(const String& namespaceName);
    bool isNamespaceAbstract(const String& namespaceName) const;
    
    // 命名空间统计和信息
    size_t getTotalNamespaceCount() const;
    size_t getMaxNamespaceDepth() const;
    StringVector getNamespacesByType(NamespaceType type) const;
    StringVector getNamespacesByVisibility(NamespaceVisibility visibility) const;
    std::unordered_map<String, size_t> getNamespaceStatistics() const;
    
    // 搜索和过滤
    StringVector searchNamespaces(const String& pattern) const;
    StringVector filterNamespaces(std::function<bool(const AdvancedNamespaceInfo&)> predicate) const;
    StringVector getNamespacesInScope(const String& currentNamespace) const;
    
    // 序列化和导入导出
    String exportNamespaceHierarchy() const;
    String exportNamespace(const String& namespaceName) const;
    bool importNamespaceHierarchy(const String& hierarchyData);
    bool importNamespaceFromString(const String& namespaceData);
    
    // 调试和诊断
    void dumpNamespaceHierarchy() const;
    void printNamespaceTree(const String& rootNamespace = "") const;
    void generateNamespaceReport() const;
    void validateIntegrity() const;
    
    // 事件系统
    using NamespaceEventCallback = std::function<void(const String&, const String&)>;
    void onNamespaceCreated(NamespaceEventCallback callback);
    void onNamespaceDeleted(NamespaceEventCallback callback);
    void onNamespaceMerged(NamespaceEventCallback callback);
    void onConflictDetected(std::function<void(const ConflictInfo&)> callback);
    
private:
    std::unordered_map<String, std::shared_ptr<AdvancedNamespaceInfo>> advancedNamespaces;
    std::unordered_map<ConflictType, ConflictResolutionStrategy> conflictStrategies;
    std::vector<NamespaceEventCallback> creationCallbacks;
    std::vector<NamespaceEventCallback> deletionCallbacks;
    std::vector<NamespaceEventCallback> mergeCallbacks;
    std::vector<std::function<void(const ConflictInfo&)>> conflictCallbacks;
    
    bool enableCircularDependencyCheck;
    bool enableAccessControl;
    bool enableVersionCheck;
    size_t maxNamespaceDepth;
    
    // 内部辅助方法
    std::shared_ptr<AdvancedNamespaceInfo> getAdvancedNamespaceInfo(const String& name) const;
    void createAdvancedNamespaceInfo(const String& name, NamespaceType type, NamespaceVisibility visibility);
    
    // 冲突检测内部方法
    std::vector<ConflictInfo> detectNameCollisions() const;
    std::vector<ConflictInfo> detectTypeMismatches() const;
    std::vector<ConflictInfo> detectScopeViolations() const;
    std::vector<ConflictInfo> detectCircularDependenciesInternal() const;
    std::vector<ConflictInfo> detectAccessViolations() const;
    
    // 合并内部方法
    bool mergeNamespaceContents(const String& target, const String& source);
    bool mergeTemplates(const String& target, const String& source);
    bool mergeCustoms(const String& target, const String& source);
    bool mergeOrigins(const String& target, const String& source);
    
    // 循环依赖检测内部方法
    bool hasCircularDependencyDFS(const String& namespaceName, StringUnorderedSet& visited, 
                                 StringUnorderedSet& recursionStack) const;
    
    // 访问控制内部方法
    bool isAccessibleFromNamespace(const String& accessorNamespace, const String& targetNamespace) const;
    bool isInSameHierarchy(const String& namespace1, const String& namespace2) const;
    
    // 优化内部方法
    void removeEmptyNamespaces();
    void consolidateSimilarNamespaces();
    void reorderNamespaceHierarchy();
    
    // 版本检查内部方法
    bool parseVersion(const String& version, std::vector<int>& versionNumbers) const;
    int compareVersions(const String& version1, const String& version2) const;
    
    // 事件触发方法
    void triggerNamespaceCreated(const String& namespaceName);
    void triggerNamespaceDeleted(const String& namespaceName);
    void triggerNamespaceMerged(const String& targetNamespace, const String& sourceNamespace);
    void triggerConflictDetected(const ConflictInfo& conflict);
    
    // 验证内部方法
    bool validateNamespaceReference(const String& reference) const;
    bool validateImportChain(const String& namespaceName, StringUnorderedSet& visited) const;
    
    // 序列化内部方法
    String serializeNamespaceInfo(const AdvancedNamespaceInfo& info) const;
    bool deserializeNamespaceInfo(const String& data, AdvancedNamespaceInfo& info) const;
    
    // 工具方法
    String generateConflictDescription(const ConflictInfo& conflict) const;
    String generateNamespaceId(const String& namespaceName) const;
    double getCurrentTime() const;
    
    // 初始化方法
    void initializeDefaultStrategies();
    void initializeBuiltinNamespaces();
};

// 命名空间构建器 - 流式API
class NamespaceBuilder {
public:
    explicit NamespaceBuilder(const String& namespaceName);
    ~NamespaceBuilder() = default;
    
    // 基础配置
    NamespaceBuilder& type(NamespaceType namespaceType);
    NamespaceBuilder& visibility(NamespaceVisibility vis);
    NamespaceBuilder& version(const String& ver);
    NamespaceBuilder& author(const String& authorName);
    NamespaceBuilder& description(const String& desc);
    
    // 层次结构
    NamespaceBuilder& parent(const String& parentName);
    NamespaceBuilder& child(const String& childName);
    NamespaceBuilder& children(const StringVector& childNames);
    
    // 导入和依赖
    NamespaceBuilder& import(const String& importedNamespace, const String& alias = "");
    NamespaceBuilder& dependency(const String& dependencyNamespace);
    NamespaceBuilder& dependencies(const StringVector& dependencyNamespaces);
    
    // 别名
    NamespaceBuilder& alias(const String& itemName, const String& aliasName);
    NamespaceBuilder& aliases(const std::unordered_map<String, String>& aliasMap);
    
    // 特性
    NamespaceBuilder& sealed(bool isSealed = true);
    NamespaceBuilder& abstract(bool isAbstract = true);
    
    // 构建
    bool build(AdvancedNamespaceManager& manager);
    std::shared_ptr<AdvancedNamespaceInfo> buildInfo();
    String buildToString();
    
private:
    std::shared_ptr<AdvancedNamespaceInfo> namespaceInfo;
    StringVector buildErrors;
    
    void addBuildError(const String& error);
    bool validateBuild() const;
};

// 命名空间查询器
class NamespaceQuery {
public:
    explicit NamespaceQuery(const AdvancedNamespaceManager& manager);
    ~NamespaceQuery() = default;
    
    // 查询构建器
    NamespaceQuery& byName(const String& name);
    NamespaceQuery& byType(NamespaceType type);
    NamespaceQuery& byVisibility(NamespaceVisibility visibility);
    NamespaceQuery& byParent(const String& parentName);
    NamespaceQuery& byDepth(size_t depth);
    NamespaceQuery& byVersion(const String& version);
    NamespaceQuery& containing(const String& itemName);
    NamespaceQuery& sealed(bool isSealed = true);
    NamespaceQuery& abstract(bool isAbstract = true);
    
    // 查询执行
    StringVector execute() const;
    size_t count() const;
    bool exists() const;
    String first() const;
    
    // 结果处理
    NamespaceQuery& orderBy(const String& field, bool ascending = true);
    NamespaceQuery& limit(size_t maxResults);
    NamespaceQuery& skip(size_t skipCount);
    
private:
    const AdvancedNamespaceManager& manager;
    std::function<bool(const AdvancedNamespaceInfo&)> predicate;
    String orderField;
    bool orderAscending;
    size_t maxResults;
    size_t skipCount;
    
    void addCondition(std::function<bool(const AdvancedNamespaceInfo&)> condition);
    bool defaultPredicate(const AdvancedNamespaceInfo& info) const;
};

// 命名空间工具函数
namespace NamespaceUtils {
    // 名称解析
    std::pair<String, String> parseQualifiedName(const String& qualifiedName);
    String joinNamespacePath(const StringVector& pathComponents);
    StringVector splitNamespacePath(const String& namespacePath);
    String getNamespaceFromPath(const String& namespacePath);
    String getItemFromPath(const String& namespacePath);
    
    // 模式匹配
    bool matchesPattern(const String& namespaceName, const String& pattern);
    StringVector expandWildcard(const StringVector& allNamespaces, const String& wildcardPattern);
    
    // 名称规范化
    String normalizeNamespaceName(const String& name);
    bool isValidNamespaceName(const String& name);
    String sanitizeNamespaceName(const String& name);
    
    // 冲突处理
    String generateUniqueNamespaceName(const StringVector& existingNames, const String& baseName);
    String suggestAlternativeName(const StringVector& existingNames, const String& conflictingName);
    
    // 统计和分析
    std::unordered_map<String, size_t> analyzeNamespaceUsage(const AdvancedNamespaceManager& manager);
    StringVector findUnusedNamespaces(const AdvancedNamespaceManager& manager);
    StringVector findOrphanedNamespaces(const AdvancedNamespaceManager& manager);
}

} // namespace CHTL