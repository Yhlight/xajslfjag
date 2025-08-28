#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include "ConfigurationManager.h"

namespace CHTL {

// 命名空间项类型
enum class NamespaceItemType {
    CUSTOM_ELEMENT,
    CUSTOM_STYLE,
    CUSTOM_VAR,
    TEMPLATE_ELEMENT,
    TEMPLATE_STYLE,
    TEMPLATE_VAR,
    ORIGIN_HTML,
    ORIGIN_STYLE,
    ORIGIN_JAVASCRIPT,
    ORIGIN_CUSTOM
};

// 命名空间项
struct NamespaceItem {
    std::string name;
    NamespaceItemType type;
    std::string sourceFile;
    size_t line;
    size_t column;
    std::string content;
    
    NamespaceItem(const std::string& name, NamespaceItemType type,
                  const std::string& sourceFile = "", size_t line = 0, size_t column = 0,
                  const std::string& content = "")
        : name(name), type(type), sourceFile(sourceFile), line(line), column(column), content(content) {}
};

// 命名空间
struct Namespace {
    std::string name;
    std::string sourceFile;
    std::vector<std::shared_ptr<NamespaceItem>> items;
    std::vector<std::string> subNamespaces;
    std::unordered_map<std::string, std::shared_ptr<Namespace>> nestedNamespaces;
    
    Namespace(const std::string& name, const std::string& sourceFile = "")
        : name(name), sourceFile(sourceFile) {}
};

// 命名空间冲突
struct NamespaceConflict {
    std::string itemName;
    NamespaceItemType type;
    std::vector<std::string> conflictingNamespaces;
    std::vector<std::string> sourceFiles;
    std::vector<size_t> lineNumbers;
    
    NamespaceConflict(const std::string& itemName, NamespaceItemType type)
        : itemName(itemName), type(type) {}
};

// 命名空间管理器
class NamespaceManager {
public:
    NamespaceManager(std::shared_ptr<ConfigurationManager> configManager);
    ~NamespaceManager() = default;
    
    // 命名空间创建
    bool createNamespace(const std::string& name, const std::string& sourceFile = "");
    bool createNestedNamespace(const std::string& parent, const std::string& child, const std::string& sourceFile = "");
    
    // 命名空间项管理
    bool addNamespaceItem(const std::string& namespaceName, std::shared_ptr<NamespaceItem> item);
    std::shared_ptr<NamespaceItem> getNamespaceItem(const std::string& namespaceName, const std::string& itemName);
    std::vector<std::shared_ptr<NamespaceItem>> getNamespaceItems(const std::string& namespaceName);
    bool removeNamespaceItem(const std::string& namespaceName, const std::string& itemName);
    
    // 命名空间查询
    std::shared_ptr<Namespace> getNamespace(const std::string& name) const;
    std::vector<std::string> getNamespaceNames() const;
    bool hasNamespace(const std::string& name) const;
    size_t getNamespaceCount() const { return namespaces_.size(); }
    
    // 命名空间合并
    bool mergeNamespaces(const std::string& name);
    bool mergeNestedNamespaces(const std::string& parent, const std::string& child);
    
    // 冲突检测
    std::vector<NamespaceConflict> detectConflicts();
    bool hasConflicts() const;
    std::vector<NamespaceConflict> getConflictsForNamespace(const std::string& namespaceName);
    
    // 命名空间路径解析
    std::vector<std::string> parseNamespacePath(const std::string& path);
    std::string normalizeNamespacePath(const std::string& path);
    std::string resolveNamespacePath(const std::string& path);
    
    // 默认命名空间
    void setDefaultNamespace(const std::string& name);
    std::string getDefaultNamespace() const { return defaultNamespace_; }
    bool isDefaultNamespaceEnabled() const;
    
    // 命名空间继承
    bool inheritNamespace(const std::string& child, const std::string& parent);
    std::vector<std::string> getInheritanceChain(const std::string& namespaceName);
    
    // 命名空间导出
    std::string exportNamespace(const std::string& name) const;
    bool importNamespace(const std::string& exportData, const std::string& name = "");
    
    // 命名空间清理
    void clearNamespace(const std::string& name);
    void clearAllNamespaces();
    
    // 命名空间验证
    bool validateNamespace(const std::string& name);
    std::vector<std::string> getValidationErrors(const std::string& name);
    
    // 统计信息
    struct NamespaceStatistics {
        size_t totalNamespaces;
        size_t totalItems;
        size_t totalConflicts;
        size_t mergedNamespaces;
        size_t nestedNamespaces;
        
        NamespaceStatistics() : totalNamespaces(0), totalItems(0), totalConflicts(0),
                               mergedNamespaces(0), nestedNamespaces(0) {}
    };
    
    const NamespaceStatistics& getStatistics() const { return statistics_; }
    void clearStatistics();
    
    // 调试信息
    std::string getDebugInfo() const;
    std::string getNamespaceInfo(const std::string& name) const;
    
    // 重置
    void reset();
    
private:
    std::shared_ptr<ConfigurationManager> configManager_;
    std::unordered_map<std::string, std::shared_ptr<Namespace>> namespaces_;
    std::string defaultNamespace_;
    std::unordered_map<std::string, std::string> inheritanceMap_;
    NamespaceStatistics statistics_;
    
    // 辅助函数
    bool isValidNamespaceName(const std::string& name) const;
    bool isValidNamespaceItem(const std::shared_ptr<NamespaceItem>& item) const;
    std::string generateNamespaceKey(const std::string& prefix, const std::string& suffix) const;
    
    // 命名空间路径处理
    std::vector<std::string> splitNamespacePath(const std::string& path);
    std::string joinNamespacePath(const std::vector<std::string>& parts);
    bool isAbsoluteNamespacePath(const std::string& path) const;
    
    // 冲突检测
    bool hasConflict(const std::string& itemName, NamespaceItemType type, const std::string& namespaceName);
    void addConflict(const std::string& itemName, NamespaceItemType type, 
                    const std::string& namespaceName, const std::string& sourceFile, size_t line);
    
    // 合并逻辑
    void mergeItems(std::vector<std::shared_ptr<NamespaceItem>>& target, 
                   const std::vector<std::shared_ptr<NamespaceItem>>& source);
    void mergeSubNamespaces(std::vector<std::string>& target, 
                           const std::vector<std::string>& source);
    void mergeNestedNamespaces(std::unordered_map<std::string, std::shared_ptr<Namespace>>& target,
                              const std::unordered_map<std::string, std::shared_ptr<Namespace>>& source);
    
    // 继承逻辑
    void buildInheritanceMap();
    bool hasCircularInheritance(const std::string& namespaceName, std::unordered_set<std::string>& visited);
    
    // 统计更新
    void updateStatistics(const std::string& type, size_t value = 1);
    
    // 配置相关
    bool isDefaultNamespaceDisabled() const;
    std::string getDefaultNamespaceFromConfig() const;
};

} // namespace CHTL