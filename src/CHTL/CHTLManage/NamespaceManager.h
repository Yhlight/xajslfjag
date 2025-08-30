#pragma once
#include "../CHTLNode/NamespaceNode.h"
#include "../CHTLNode/TemplateNode.h"
#include "../CHTLNode/AdvancedTemplateNode.h"
#include "../CHTLNode/OriginNode.h"
#include "../CHTLNode/ConfigNode.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <filesystem>

namespace CHTL {

/**
 * 命名空间符号类型
 */
enum class NamespaceSymbolType {
    TEMPLATE_STYLE,      // 样式组模板
    TEMPLATE_ELEMENT,    // 元素模板
    TEMPLATE_VAR,        // 变量组模板
    CUSTOM_STYLE,        // 自定义样式组
    CUSTOM_ELEMENT,      // 自定义元素
    CUSTOM_VAR,          // 自定义变量组
    ORIGIN_NODE,         // 原始嵌入节点
    CONFIG_NODE,         // 配置节点
    NAMESPACE_NODE       // 命名空间节点
};

/**
 * 命名空间符号信息
 */
struct NamespaceSymbolInfo {
    std::string name;                    // 符号名称
    NamespaceSymbolType type;            // 符号类型
    std::shared_ptr<BaseNode> node;      // 对应的节点
    std::string namespacePath;          // 所属命名空间路径
    std::string sourceFile;             // 源文件路径
    
    NamespaceSymbolInfo() = default;
    NamespaceSymbolInfo(const std::string& n, NamespaceSymbolType t, std::shared_ptr<BaseNode> node_ptr, 
                       const std::string& ns = "", const std::string& file = "")
        : name(n), type(t), node(node_ptr), namespacePath(ns), sourceFile(file) {}
};

/**
 * 命名空间冲突类型
 */
enum class ConflictType {
    NO_CONFLICT,        // 无冲突
    NAME_CONFLICT,      // 名称冲突
    TYPE_MISMATCH,      // 类型不匹配
    MULTIPLE_DEFINITION // 多重定义
};

/**
 * 命名空间管理器
 * 负责管理CHTL中的命名空间系统
 */
class CHTLNamespaceManager {
public:
    /**
     * 构造函数
     */
    CHTLNamespaceManager();
    
    /**
     * 析构函数
     */
    ~CHTLNamespaceManager() = default;
    
    /**
     * 注册命名空间
     */
    void registerNamespace(std::shared_ptr<NamespaceNode> namespaceNode, const std::string& sourceFile = "");
    
    /**
     * 注册符号到命名空间
     */
    void registerSymbol(const std::string& namespacePath, const NamespaceSymbolInfo& symbol);
    
    /**
     * 查找符号
     */
    std::vector<NamespaceSymbolInfo> findSymbol(const std::string& name, NamespaceSymbolType type = NamespaceSymbolType::TEMPLATE_STYLE, 
                                       const std::string& currentNamespace = "") const;
    
    /**
     * 通过完整路径查找符号
     */
    NamespaceSymbolInfo findSymbolByPath(const std::string& fullPath) const;
    
    /**
     * 解析命名空间引用
     * 例如: "@Element Box from space.room2"
     */
    NamespaceSymbolInfo resolveNamespaceReference(const std::string& symbolName, NamespaceSymbolType type, 
                                        const std::string& namespacePath, 
                                        const std::string& currentNamespace = "") const;
    
    /**
     * 合并同名命名空间
     */
    void mergeNamespaces();
    
    /**
     * 检查命名空间冲突
     */
    ConflictType checkConflict(const std::string& namespacePath, const NamespaceSymbolInfo& symbol) const;
    
    /**
     * 创建文件默认命名空间
     */
    std::string createDefaultFileNamespace(const std::string& filePath) const;
    
    /**
     * 设置默认命名空间功能是否启用
     */
    void setDefaultNamespaceEnabled(bool enabled);
    
    /**
     * 获取所有命名空间
     */
    const std::unordered_map<std::string, std::shared_ptr<NamespaceNode>>& getAllNamespaces() const;
    
    /**
     * 获取命名空间符号表
     */
    const std::unordered_map<std::string, std::vector<NamespaceSymbolInfo>>& getSymbolTable() const;
    
    /**
     * 获取命名空间下的所有符号
     */
    std::vector<NamespaceSymbolInfo> getNamespaceSymbols(const std::string& namespacePath) const;
    
    /**
     * 获取命名空间的子命名空间
     */
    std::vector<std::string> getChildNamespaces(const std::string& namespacePath) const;
    
    /**
     * 检查命名空间是否存在
     */
    bool namespaceExists(const std::string& namespacePath) const;
    
    /**
     * 获取命名空间层次结构
     */
    std::vector<std::string> getNamespaceHierarchy(const std::string& namespacePath) const;
    
    /**
     * 导入命名空间符号
     */
    void importNamespaceSymbols(const std::string& fromNamespace, const std::string& toNamespace, 
                               const std::vector<NamespaceSymbolType>& symbolTypes = {});
    
    /**
     * 生成冲突报告
     */
    std::string generateConflictReport() const;
    
    /**
     * 生成命名空间使用报告
     */
    std::string generateUsageReport() const;

private:
    std::unordered_map<std::string, std::shared_ptr<NamespaceNode>> m_namespaces;       // 命名空间映射
    std::unordered_map<std::string, std::vector<NamespaceSymbolInfo>> m_symbolTable;            // 符号表 (namespacePath -> symbols)
    std::unordered_map<std::string, std::string> m_namespaceHierarchy;                 // 命名空间层次 (child -> parent)
    std::unordered_map<std::string, std::vector<std::string>> m_childNamespaces;       // 子命名空间 (parent -> children)
    bool m_defaultNamespaceEnabled;                                                    // 是否启用默认命名空间
    
    /**
     * 规范化命名空间路径
     */
    std::string normalizeNamespacePath(const std::string& path) const;
    
    /**
     * 分割命名空间路径
     */
    std::vector<std::string> splitNamespacePath(const std::string& path) const;
    
    /**
     * 构建命名空间路径
     */
    std::string buildNamespacePath(const std::vector<std::string>& parts) const;
    
    /**
     * 检查符号兼容性
     */
    bool isSymbolCompatible(const NamespaceSymbolInfo& existing, const NamespaceSymbolInfo& newSymbol) const;
    
    /**
     * 合并符号信息
     */
    NamespaceSymbolInfo mergeNamespaceSymbolInfo(const NamespaceSymbolInfo& existing, const NamespaceSymbolInfo& newSymbol) const;
    
    /**
     * 查找最近匹配的命名空间
     */
    std::string findClosestNamespace(const std::string& currentNamespace, const std::string& targetName) const;
    
    /**
     * 获取符号类型字符串
     */
    std::string getNamespaceSymbolTypeString(NamespaceSymbolType type) const;
};

} // namespace CHTL