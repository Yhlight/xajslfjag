#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

namespace CHTL {
namespace Node {

/**
 * BaseNode - CHTL AST节点基类
 * 新目录结构下的核心基类
 */
class BaseNode {
public:
    /**
     * 节点类型枚举
     */
    enum class NodeType {
        // 基础节点
        ELEMENT,        // HTML元素节点
        TEXT,           // 文本节点
        
        // 特殊节点
        COMMENT,        // 注释节点
        TEMPLATE,       // 模板节点
        CUSTOM,         // 自定义节点
        STYLE,          // 样式节点
        SCRIPT,         // 脚本节点
        ORIGIN,         // 原始嵌入节点
        IMPORT,         // 导入节点
        CONFIG,         // 配置节点
        NAMESPACE,      // 命名空间节点
        OPERATOR,       // 操作符节点(delete, insert...)
        USE             // Use节点
    };

protected:
    NodeType node_type_;
    std::string source_location_;
    int line_number_;
    int column_number_;
    std::weak_ptr<BaseNode> parent_;
    std::vector<std::shared_ptr<BaseNode>> children_;
    std::unordered_map<std::string, std::string> attributes_;

public:
    /**
     * 构造函数
     * @param type 节点类型
     * @param location 源文件位置
     * @param line 行号
     * @param column 列号
     */
    BaseNode(NodeType type, const std::string& location = "", int line = 0, int column = 0);
    
    /**
     * 虚析构函数
     */
    virtual ~BaseNode() = default;
    
    /**
     * 获取节点类型
     * @return 节点类型
     */
    NodeType getNodeType() const;
    
    /**
     * 获取源文件位置
     * @return 源文件位置
     */
    const std::string& getSourceLocation() const;
    
    /**
     * 获取行号
     * @return 行号
     */
    int getLineNumber() const;
    
    /**
     * 获取列号
     * @return 列号
     */
    int getColumnNumber() const;
    
    /**
     * 添加子节点
     * @param child 子节点
     */
    void addChild(std::shared_ptr<BaseNode> child);
    
    /**
     * 获取子节点列表
     * @return 子节点列表
     */
    const std::vector<std::shared_ptr<BaseNode>>& getChildren() const;
    
    /**
     * 设置父节点
     * @param parent 父节点
     */
    void setParent(std::shared_ptr<BaseNode> parent);
    
    /**
     * 获取父节点
     * @return 父节点
     */
    std::shared_ptr<BaseNode> getParent() const;
    
    /**
     * 设置属性
     * @param key 属性键
     * @param value 属性值
     */
    void setAttribute(const std::string& key, const std::string& value);
    
    /**
     * 获取属性
     * @param key 属性键
     * @return 属性值
     */
    std::string getAttribute(const std::string& key) const;
    
    /**
     * 获取所有属性
     * @return 属性映射
     */
    const std::unordered_map<std::string, std::string>& getAttributes() const;
    
    /**
     * 判断是否有指定属性
     * @param key 属性键
     * @return 是否存在
     */
    bool hasAttribute(const std::string& key) const;
    
    /**
     * 转换为字符串表示
     * @return 字符串表示
     */
    virtual std::string toString() const = 0;
    
    /**
     * 接受访问者模式
     * @param visitor 访问者
     */
    virtual void accept(class NodeVisitor& visitor) = 0;
    
    /**
     * 克隆节点
     * @return 克隆的节点
     */
    virtual std::shared_ptr<BaseNode> clone() const = 0;
};

} // namespace Node
} // namespace CHTL