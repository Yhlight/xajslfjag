#pragma once

#include "BaseNode/BaseNode.h"

namespace CHTL {
namespace Node {

/**
 * NamespaceNode - 命名空间节点
 * 处理[Namespace]声明
 */
class NamespaceNode : public BaseNode {
private:
    std::string namespace_name_;
    std::vector<std::string> namespace_path_;   // 嵌套命名空间路径
    bool is_default_;                          // 是否为默认命名空间（从文件名生成）
    bool is_merged_;                           // 是否与同名命名空间合并
    std::unordered_map<std::string, std::string> symbols_; // 命名空间中的符号
    
public:
    /**
     * 构造函数
     * @param name 命名空间名称
     * @param is_default 是否为默认命名空间
     * @param location 源文件位置
     * @param line 行号
     * @param column 列号
     */
    NamespaceNode(const std::string& name,
                  bool is_default = false,
                  const std::string& location = "", 
                  int line = 0, 
                  int column = 0);
    
    /**
     * 获取命名空间名称
     * @return 命名空间名称
     */
    const std::string& getNamespaceName() const;
    
    /**
     * 设置命名空间名称
     * @param name 命名空间名称
     */
    void setNamespaceName(const std::string& name);
    
    /**
     * 获取命名空间路径
     * @return 命名空间路径
     */
    const std::vector<std::string>& getNamespacePath() const;
    
    /**
     * 设置命名空间路径
     * @param path 命名空间路径
     */
    void setNamespacePath(const std::vector<std::string>& path);
    
    /**
     * 是否为默认命名空间
     * @return 是否为默认命名空间
     */
    bool isDefault() const;
    
    /**
     * 设置是否为默认命名空间
     * @param is_default 是否为默认命名空间
     */
    void setDefault(bool is_default);
    
    /**
     * 是否已合并
     * @return 是否已合并
     */
    bool isMerged() const;
    
    /**
     * 设置是否已合并
     * @param is_merged 是否已合并
     */
    void setMerged(bool is_merged);
    
    /**
     * 添加符号
     * @param symbol_name 符号名称
     * @param symbol_value 符号值
     */
    void addSymbol(const std::string& symbol_name, const std::string& symbol_value);
    
    /**
     * 获取符号
     * @param symbol_name 符号名称
     * @return 符号值
     */
    std::string getSymbol(const std::string& symbol_name) const;
    
    /**
     * 检查符号是否存在
     * @param symbol_name 符号名称
     * @return 是否存在
     */
    bool hasSymbol(const std::string& symbol_name) const;
    
    /**
     * 获取所有符号
     * @return 符号映射
     */
    const std::unordered_map<std::string, std::string>& getSymbols() const;
    
    /**
     * 转换为字符串表示
     * @return 字符串表示
     */
    std::string toString() const override;
    
    /**
     * 接受访问者模式
     * @param visitor 访问者
     */
    void accept(NodeVisitor& visitor) override;
    
    /**
     * 克隆节点
     * @return 克隆的节点
     */
    std::shared_ptr<BaseNode> clone() const override;
    
    /**
     * 获取完整命名空间名称
     * @return 完整命名空间名称
     */
    std::string getFullNamespaceName() const;
    
    /**
     * 解析嵌套命名空间
     * @param name 命名空间名称
     */
    void parseNestedNamespace(const std::string& name);
    
    /**
     * 合并命名空间
     * @param other 其他命名空间节点
     * @return 是否成功合并
     */
    bool mergeNamespace(const NamespaceNode& other);
    
    /**
     * 检查命名空间冲突
     * @param other 其他命名空间节点
     * @return 是否有冲突
     */
    bool hasConflictWith(const NamespaceNode& other) const;
};

} // namespace Node
} // namespace CHTL