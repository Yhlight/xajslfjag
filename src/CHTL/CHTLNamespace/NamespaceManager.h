#pragma once

#include "../../Util/Common.h"
#include "../CHTLNode/BaseNode.h"

namespace CHTL {

// 命名空间信息
struct NamespaceInfo {
    String name;                                // 命名空间名称
    String fullName;                           // 完整名称（包含父级）
    NamespaceInfo* parent = nullptr;           // 父命名空间
    std::vector<std::unique_ptr<NamespaceInfo>> children;  // 子命名空间
    std::unordered_map<String, BaseNode*> templates;    // 模板映射
    std::unordered_map<String, BaseNode*> customs;      // 自定义映射
    std::unordered_map<String, BaseNode*> origins;      // 原始嵌入映射
    StringUnorderedMap configs;                // 配置映射
    StringVector imports;                      // 导入列表
    bool isDefault = false;                    // 是否为默认命名空间
    
    NamespaceInfo(const String& n) : name(n), fullName(n) {}
    
    // 获取完整名称
    String getFullName() const {
        if (parent && !parent->name.empty()) {
            return parent->getFullName() + "." + name;
        }
        return name;
    }
    
    // 查找子命名空间
    NamespaceInfo* findChild(const String& childName) const {
        for (const auto& child : children) {
            if (child->name == childName) {
                return child.get();
            }
        }
        return nullptr;
    }
    
    // 添加子命名空间
    NamespaceInfo* addChild(const String& childName) {
        auto child = std::make_unique<NamespaceInfo>(childName);
        child->parent = this;
        child->fullName = getFullName() + "." + childName;
        
        NamespaceInfo* childPtr = child.get();
        children.push_back(std::move(child));
        return childPtr;
    }
};

// 命名空间管理器
class NamespaceManager {
private:
    std::unique_ptr<NamespaceInfo> globalNamespace;  // 全局命名空间
    NamespaceInfo* currentNamespace;                 // 当前命名空间
    std::unordered_map<String, NamespaceInfo*> namespaceMap;  // 命名空间快速查找
    bool enableDefaultNamespace = true;              // 启用默认命名空间
    
    // 内部方法
    NamespaceInfo* findNamespaceRecursive(NamespaceInfo* root, const String& name) const;
    void mergeNamespaces(NamespaceInfo* target, NamespaceInfo* source);
    bool hasCircularReference(NamespaceInfo* ns, const String& targetName) const;
    
public:
    // 构造函数
    NamespaceManager();
    ~NamespaceManager() = default;
    
    // 禁用拷贝构造
    NamespaceManager(const NamespaceManager&) = delete;
    NamespaceManager& operator=(const NamespaceManager&) = delete;
    
    // 移动构造
    NamespaceManager(NamespaceManager&&) = default;
    NamespaceManager& operator=(NamespaceManager&&) = default;
    
    // 命名空间操作
    NamespaceInfo* createNamespace(const String& name);
    NamespaceInfo* createNestedNamespace(const String& fullName);
    NamespaceInfo* findNamespace(const String& name) const;
    NamespaceInfo* getCurrentNamespace() const { return currentNamespace; }
    
    // 命名空间切换
    bool enterNamespace(const String& name);
    bool exitNamespace();
    void setCurrentNamespace(NamespaceInfo* ns);
    
    // 命名空间合并
    bool mergeNamespace(const String& sourceName, const String& targetName);
    void autoMergeNamespaces(const String& name);
    
    // 冲突检测
    bool hasConflict(const String& name, const String& itemName, const String& itemType) const;
    StringVector getConflicts(const String& name) const;
    bool validateNamespace(const String& name) const;
    
    // 内容注册
    bool registerTemplate(const String& namespaceName, const String& templateName, BaseNode* templateNode);
    bool registerCustom(const String& namespaceName, const String& customName, BaseNode* customNode);
    bool registerOrigin(const String& namespaceName, const String& originName, BaseNode* originNode);
    bool registerConfig(const String& namespaceName, const String& configName, BaseNode* configNode);
    
    // 内容查找
    BaseNode* findTemplate(const String& name) const;
    BaseNode* findCustom(const String& name) const;
    BaseNode* findOrigin(const String& name) const;
    BaseNode* findConfig(const String& name) const;
    
    // 跨命名空间查找
    BaseNode* findTemplateInNamespace(const String& namespaceName, const String& templateName) const;
    BaseNode* findCustomInNamespace(const String& namespaceName, const String& customName) const;
    
    // 导入管理
    bool addImport(const String& namespaceName, const String& importPath);
    StringVector getImports(const String& namespaceName) const;
    bool hasImport(const String& namespaceName, const String& importPath) const;
    
    // 默认命名空间
    void setEnableDefaultNamespace(bool enable) { enableDefaultNamespace = enable; }
    bool isDefaultNamespaceEnabled() const { return enableDefaultNamespace; }
    NamespaceInfo* createDefaultNamespace(const String& fileName);
    
    // 配置选项
    void setConfiguration(const String& key, const String& value);
    String getConfiguration(const String& key, const String& defaultValue = "") const;
    
    // 工具方法
    StringVector getAllNamespaces() const;
    void printNamespaceTree() const;
    void reset();
    
    // 调试方法
    void dumpNamespaces() const;
    String getNamespaceStatistics() const;
    
private:
    void printNamespaceRecursive(NamespaceInfo* ns, int depth = 0) const;
    void collectNamespacesRecursive(NamespaceInfo* ns, StringVector& result) const;
};

// 命名空间解析器
class NamespaceParser {
public:
    // 解析命名空间路径
    static StringVector parseNamespacePath(const String& fullPath);
    
    // 验证命名空间名称
    static bool isValidNamespaceName(const String& name);
    
    // 构建完整路径
    static String buildFullPath(const StringVector& path);
    
    // 解析 "from namespace" 语法
    static std::pair<String, String> parseFromClause(const String& clause);
};

// 命名空间工具函数
namespace Namespace {
    // 名称验证
    bool isValidName(const String& name);
    bool isReservedName(const String& name);
    
    // 路径操作
    StringVector splitPath(const String& path);
    String joinPath(const StringVector& components);
    String normalizePath(const String& path);
    
    // 冲突解决
    String generateUniqueNamespace(const String& baseName, const StringVector& existingNames);
    String resolveConflict(const String& name, const StringVector& conflicts);
}

} // namespace CHTL