#pragma once
#include "BaseNode.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <sstream>

namespace CHTL {

// 前向声明
class TemplateNode;
class CustomNode;
class OriginNode;
class ConfigNode;

/**
 * 命名空间节点
 * 表示CHTL中的[Namespace]块
 */
class NamespaceNode : public BaseNode, public std::enable_shared_from_this<NamespaceNode> {
public:
    /**
     * 构造函数
     */
    NamespaceNode(const std::string& name = "");
    
    /**
     * 析构函数
     */
    virtual ~NamespaceNode() = default;
    
    /**
     * 获取命名空间名称
     */
    const std::string& getNamespaceName() const;
    
    /**
     * 设置命名空间名称
     */
    void setNamespaceName(const std::string& name);
    
    /**
     * 获取完整的命名空间路径
     */
    std::string getFullNamespacePath() const;
    
    /**
     * 设置父命名空间
     */
    void setParentNamespace(std::shared_ptr<NamespaceNode> parent);
    
    /**
     * 获取父命名空间
     */
    std::shared_ptr<NamespaceNode> getParentNamespace() const;
    
    /**
     * 添加子命名空间
     */
    void addChildNamespace(std::shared_ptr<NamespaceNode> child);
    
    /**
     * 获取子命名空间
     */
    std::shared_ptr<NamespaceNode> getChildNamespace(const std::string& name) const;
    
    /**
     * 获取所有子命名空间
     */
    const std::vector<std::shared_ptr<NamespaceNode>>& getChildNamespaces() const;
    
    /**
     * 检查是否为根命名空间
     */
    bool isRootNamespace() const;
    
    /**
     * 检查是否为文件默认命名空间
     */
    bool isFileDefaultNamespace() const;
    
    /**
     * 设置为文件默认命名空间
     */
    void setAsFileDefaultNamespace(bool isDefault);
    
    /**
     * 添加模板到命名空间
     */
    void addTemplate(std::shared_ptr<TemplateNode> templateNode);
    
    /**
     * 添加自定义到命名空间
     */
    void addCustom(std::shared_ptr<CustomNode> customNode);
    
    /**
     * 添加原始嵌入到命名空间
     */
    void addOrigin(std::shared_ptr<OriginNode> originNode);
    
    /**
     * 添加配置到命名空间
     */
    void addConfig(std::shared_ptr<ConfigNode> configNode);
    
    /**
     * 查找模板
     */
    std::shared_ptr<TemplateNode> findTemplate(const std::string& type, const std::string& name) const;
    
    /**
     * 查找自定义
     */
    std::shared_ptr<CustomNode> findCustom(const std::string& type, const std::string& name) const;
    
    /**
     * 查找原始嵌入
     */
    std::shared_ptr<OriginNode> findOrigin(const std::string& type, const std::string& name) const;
    
    /**
     * 查找配置
     */
    std::shared_ptr<ConfigNode> findConfig(const std::string& name = "") const;
    
    /**
     * 解析完全限定名称
     * 例如: "space.room.Box" -> 在space.room命名空间中查找Box
     */
    std::shared_ptr<BaseNode> resolveQualifiedName(const std::string& qualifiedName, const std::string& type) const;
    
    /**
     * 合并命名空间（同名命名空间自动合并）
     */
    void mergeNamespace(std::shared_ptr<NamespaceNode> other);
    
    /**
     * 检查命名冲突
     */
    bool hasNamingConflict(const std::string& type, const std::string& name) const;
    
    /**
     * 获取所有符号（用于导出）
     */
    std::unordered_map<std::string, std::vector<std::string>> getAllSymbols() const;
    
    // BaseNode 接口实现
    CHTLNodeType getNodeType() const { return CHTLNodeType::NAMESPACE_NODE; }
    std::string toString() const override;
    NodePtr clone() const override;
    bool validate(ErrorReporter* errorReporter = nullptr) const override;

private:
    std::string m_namespaceName;                                              // 命名空间名称
    std::shared_ptr<NamespaceNode> m_parentNamespace;                        // 父命名空间
    std::vector<std::shared_ptr<NamespaceNode>> m_childNamespaces;           // 子命名空间
    bool m_isFileDefaultNamespace;                                           // 是否为文件默认命名空间
    
    // 符号表
    std::unordered_map<std::string, std::shared_ptr<TemplateNode>> m_templates;    // 模板表
    std::unordered_map<std::string, std::shared_ptr<CustomNode>> m_customs;       // 自定义表
    std::unordered_map<std::string, std::shared_ptr<OriginNode>> m_origins;       // 原始嵌入表
    std::unordered_map<std::string, std::shared_ptr<ConfigNode>> m_configs;       // 配置表
    
    /**
     * 生成符号键
     */
    std::string generateSymbolKey(const std::string& type, const std::string& name) const;
    
    /**
     * 递归查找符号
     */
    std::shared_ptr<BaseNode> findSymbolRecursive(const std::string& key) const;
};

/**
 * 命名空间管理器
 * 管理整个项目的命名空间层次结构
 */
class NamespaceManager {
public:
    /**
     * 构造函数
     */
    NamespaceManager();
    
    /**
     * 析构函数
     */
    ~NamespaceManager() = default;
    
    /**
     * 获取根命名空间
     */
    std::shared_ptr<NamespaceNode> getRootNamespace() const;
    
    /**
     * 创建或获取命名空间
     */
    std::shared_ptr<NamespaceNode> createOrGetNamespace(const std::string& namespacePath);
    
    /**
     * 设置当前文件的默认命名空间
     */
    void setCurrentFileNamespace(const std::string& filename, std::shared_ptr<NamespaceNode> namespaceNode);
    
    /**
     * 获取当前文件的默认命名空间
     */
    std::shared_ptr<NamespaceNode> getCurrentFileNamespace(const std::string& filename) const;
    
    /**
     * 解析完全限定名称
     */
    std::shared_ptr<BaseNode> resolveQualifiedName(const std::string& qualifiedName, const std::string& type) const;
    
    /**
     * 添加符号到当前命名空间
     */
    void addSymbolToCurrentNamespace(const std::string& filename, std::shared_ptr<BaseNode> symbol);
    
    /**
     * 查找符号（在当前命名空间及父命名空间中查找）
     */
    std::shared_ptr<BaseNode> findSymbol(const std::string& filename, const std::string& type, const std::string& name) const;
    
    /**
     * 检查是否禁用默认命名空间
     */
    bool isDefaultNamespaceDisabled() const;
    
    /**
     * 设置是否禁用默认命名空间
     */
    void setDefaultNamespaceDisabled(bool disabled);
    
    /**
     * 获取所有命名空间
     */
    std::vector<std::shared_ptr<NamespaceNode>> getAllNamespaces() const;
    
    /**
     * 清理空命名空间
     */
    void cleanupEmptyNamespaces();

private:
    std::shared_ptr<NamespaceNode> m_rootNamespace;                                    // 根命名空间
    std::unordered_map<std::string, std::shared_ptr<NamespaceNode>> m_fileNamespaces; // 文件到命名空间的映射
    bool m_defaultNamespaceDisabled;                                                   // 是否禁用默认命名空间
    
    /**
     * 解析命名空间路径
     */
    std::vector<std::string> parseNamespacePath(const std::string& namespacePath) const;
    
    /**
     * 递归创建命名空间
     */
    std::shared_ptr<NamespaceNode> createNamespaceRecursive(std::shared_ptr<NamespaceNode> parent, 
                                                           const std::vector<std::string>& pathComponents, 
                                                           size_t index);
    
    /**
     * 收集所有命名空间（递归）
     */
    void collectNamespacesRecursive(std::shared_ptr<NamespaceNode> current, 
                                   std::vector<std::shared_ptr<NamespaceNode>>& result) const;
};

} // namespace CHTL