#ifndef NAMESPACEMANAGER_H
#define NAMESPACEMANAGER_H

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include "ConfigurationManager.h"

namespace CHTL {

/**
 * @brief 命名空间项类型枚举
 */
enum class NamespaceItemType {
    CUSTOM_ELEMENT,      // [Custom] @Element
    CUSTOM_STYLE,        // [Custom] @Style
    CUSTOM_VAR,          // [Custom] @Var
    TEMPLATE_ELEMENT,    // [Template] @Element
    TEMPLATE_STYLE,      // [Template] @Style
    TEMPLATE_VAR,        // [Template] @Var
    ORIGIN_HTML,         // [Origin] @Html
    ORIGIN_STYLE,        // [Origin] @Style
    ORIGIN_JAVASCRIPT,   // [Origin] @JavaScript
    ORIGIN_CUSTOM,       // [Origin] @Custom
    IMPORT_HTML,         // [Import] @Html
    IMPORT_STYLE,        // [Import] @Style
    IMPORT_JAVASCRIPT,   // [Import] @JavaScript
    IMPORT_CHTL,         // [Import] @Chtl
    IMPORT_CJMOD,        // [Import] @CJmod
    IMPORT_CONFIG,       // [Import] @Config
    CONFIGURATION        // [Configuration]
};

/**
 * @brief 命名空间项结构
 */
struct NamespaceItem {
    NamespaceItemType type;
    std::string name;
    std::string sourceFile;
    int lineNumber;
    int columnNumber;
    std::string content;
    std::vector<std::string> dependencies;
    
    NamespaceItem() : type(NamespaceItemType::CUSTOM_ELEMENT), lineNumber(0), columnNumber(0) {}
};

/**
 * @brief 命名空间结构
 */
struct Namespace {
    std::string name;
    std::string sourceFile;
    std::vector<std::shared_ptr<NamespaceItem>> items;
    std::vector<std::string> subNamespaces;
    std::unordered_map<std::string, std::shared_ptr<Namespace>> nestedNamespaces;
    bool isDefault;
    
    Namespace() : isDefault(false) {}
};

/**
 * @brief 命名空间冲突信息
 */
struct NamespaceConflict {
    std::string itemName;
    NamespaceItemType itemType;
    std::string namespace1;
    std::string namespace2;
    std::string sourceFile1;
    std::string sourceFile2;
    int line1;
    int line2;
    
    NamespaceConflict() : line1(0), line2(0) {}
};

/**
 * @brief 命名空间管理器
 * 负责管理CHTL项目的所有命名空间操作
 */
class NamespaceManager {
public:
    /**
     * @brief 构造函数
     * @param configManager 配置管理器指针
     */
    NamespaceManager(std::shared_ptr<ConfigurationManager> configManager);
    
    /**
     * @brief 析构函数
     */
    ~NamespaceManager();
    
    /**
     * @brief 创建命名空间
     * @param name 命名空间名称
     * @param sourceFile 源文件路径
     * @param isDefault 是否为默认命名空间
     * @return 是否创建成功
     */
    bool createNamespace(const std::string& name, const std::string& sourceFile, bool isDefault = false);
    
    /**
     * @brief 创建嵌套命名空间
     * @param parentName 父命名空间名称
     * @param childName 子命名空间名称
     * @param sourceFile 源文件路径
     * @return 是否创建成功
     */
    bool createNestedNamespace(const std::string& parentName, const std::string& childName, const std::string& sourceFile);
    
    /**
     * @brief 添加命名空间项
     * @param namespaceName 命名空间名称
     * @param item 命名空间项
     * @return 是否添加成功
     */
    bool addNamespaceItem(const std::string& namespaceName, const std::shared_ptr<NamespaceItem>& item);
    
    /**
     * @brief 获取命名空间
     * @param name 命名空间名称
     * @return 命名空间指针
     */
    std::shared_ptr<Namespace> getNamespace(const std::string& name);
    
    /**
     * @brief 获取所有命名空间
     * @return 命名空间映射
     */
    const std::unordered_map<std::string, std::shared_ptr<Namespace>>& getAllNamespaces() const;
    
    /**
     * @brief 检查命名空间是否存在
     * @param name 命名空间名称
     * @return 是否存在
     */
    bool namespaceExists(const std::string& name) const;
    
    /**
     * @brief 检查命名空间项是否存在
     * @param namespaceName 命名空间名称
     * @param itemName 项名称
     * @param itemType 项类型
     * @return 是否存在
     */
    bool namespaceItemExists(const std::string& namespaceName, const std::string& itemName, NamespaceItemType itemType) const;
    
    /**
     * @brief 合并同名命名空间
     * @param name 命名空间名称
     * @return 是否合并成功
     */
    bool mergeNamespaces(const std::string& name);
    
    /**
     * @brief 检测命名空间冲突
     * @return 冲突信息列表
     */
    std::vector<NamespaceConflict> detectConflicts() const;
    
    /**
     * @brief 解析命名空间路径
     * @param path 命名空间路径（如：space.room）
     * @return 命名空间组件列表
     */
    std::vector<std::string> parseNamespacePath(const std::string& path) const;
    
    /**
     * @brief 获取命名空间项
     * @param namespacePath 命名空间路径
     * @param itemName 项名称
     * @param itemType 项类型
     * @return 命名空间项指针
     */
    std::shared_ptr<NamespaceItem> getNamespaceItem(const std::string& namespacePath, const std::string& itemName, NamespaceItemType itemType) const;
    
    /**
     * @brief 创建默认命名空间
     * @param sourceFile 源文件路径
     * @return 默认命名空间名称
     */
    std::string createDefaultNamespace(const std::string& sourceFile);
    
    /**
     * @brief 设置默认命名空间启用状态
     * @param enabled 是否启用
     */
    void setDefaultNamespaceEnabled(bool enabled);
    
    /**
     * @brief 获取默认命名空间启用状态
     * @return 是否启用
     */
    bool isDefaultNamespaceEnabled() const;
    
    /**
     * @brief 清除命名空间
     * @param name 命名空间名称
     */
    void clearNamespace(const std::string& name);
    
    /**
     * @brief 清除所有命名空间
     */
    void clearAllNamespaces();
    
    /**
     * @brief 获取命名空间统计信息
     * @return 统计信息字符串
     */
    std::string getStatistics() const;
    
    /**
     * @brief 验证命名空间名称
     * @param name 命名空间名称
     * @return 是否有效
     */
    bool validateNamespaceName(const std::string& name) const;

private:
    std::shared_ptr<ConfigurationManager> configManager_;
    std::unordered_map<std::string, std::shared_ptr<Namespace>> namespaces_;
    bool defaultNamespaceEnabled_;
    
    /**
     * @brief 查找命名空间（支持嵌套路径）
     * @param path 命名空间路径
     * @return 命名空间指针
     */
    std::shared_ptr<Namespace> findNamespace(const std::string& path) const;
    
    /**
     * @brief 创建嵌套命名空间路径
     * @param parent 父命名空间
     * @param pathComponents 路径组件
     * @param sourceFile 源文件路径
     * @return 是否创建成功
     */
    bool createNestedNamespacePath(std::shared_ptr<Namespace> parent, const std::vector<std::string>& pathComponents, const std::string& sourceFile);
    
    /**
     * @brief 检查命名空间项冲突
     * @param namespace1 命名空间1
     * @param namespace2 命名空间2
     * @param conflicts 冲突列表
     */
    void checkNamespaceConflicts(const std::shared_ptr<Namespace>& namespace1, 
                                const std::shared_ptr<Namespace>& namespace2,
                                std::vector<NamespaceConflict>& conflicts) const;
    
    /**
     * @brief 从文件名生成默认命名空间名称
     * @param filePath 文件路径
     * @return 默认命名空间名称
     */
    std::string generateDefaultNamespaceName(const std::string& filePath) const;
    
    /**
     * @brief 检查命名空间项类型兼容性
     * @param type1 类型1
     * @param type2 类型2
     * @return 是否兼容
     */
    bool areItemTypesCompatible(NamespaceItemType type1, NamespaceItemType type2) const;
};

} // namespace CHTL

#endif // NAMESPACEMANAGER_H