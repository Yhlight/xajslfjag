#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

namespace CHTL {
namespace Node {

// 前向声明
class NodeVisitor;

/**
 * BaseNode - CHTL AST节点基类
 * 所有CHTL节点的基础类
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
        OPERATOR        // 操作符节点(delete, insert, use...)
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
    virtual void accept(NodeVisitor& visitor) = 0;
    
    /**
     * 克隆节点
     * @return 克隆的节点
     */
    virtual std::shared_ptr<BaseNode> clone() const = 0;
};

/**
 * ElementNode - HTML元素节点
 * 内部包含在BaseNode.h中
 */
class ElementNode : public BaseNode {
private:
    std::string tag_name_;
    bool is_self_closing_;
    bool is_custom_element_;
    std::string index_access_;  // [1], [0] 等索引访问
    
public:
    /**
     * 构造函数
     * @param tag_name 标签名
     * @param location 源文件位置
     * @param line 行号
     * @param column 列号
     */
    ElementNode(const std::string& tag_name, 
                const std::string& location = "", 
                int line = 0, 
                int column = 0);
    
    /**
     * 获取标签名
     * @return 标签名
     */
    const std::string& getTagName() const;
    
    /**
     * 设置标签名
     * @param tag_name 标签名
     */
    void setTagName(const std::string& tag_name);
    
    /**
     * 是否为自闭合标签
     * @return 是否自闭合
     */
    bool isSelfClosing() const;
    
    /**
     * 设置是否自闭合
     * @param self_closing 是否自闭合
     */
    void setSelfClosing(bool self_closing);
    
    /**
     * 是否为自定义元素
     * @return 是否为自定义元素
     */
    bool isCustomElement() const;
    
    /**
     * 设置是否为自定义元素
     * @param is_custom 是否为自定义元素
     */
    void setCustomElement(bool is_custom);
    
    /**
     * 获取索引访问
     * @return 索引访问字符串
     */
    const std::string& getIndexAccess() const;
    
    /**
     * 设置索引访问
     * @param index_access 索引访问字符串
     */
    void setIndexAccess(const std::string& index_access);
    
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
     * 生成HTML
     * @return HTML字符串
     */
    std::string generateHTML() const;
    
    /**
     * 获取完整的选择器
     * @return CSS选择器
     */
    std::string getSelector() const;
};

/**
 * TextNode - 文本节点
 * 内部包含在BaseNode.h中
 */
class TextNode : public BaseNode {
private:
    std::string content_;
    bool is_unquoted_literal_;  // 是否为无修饰字面量
    bool is_interpolated_;      // 是否包含插值表达式
    
public:
    /**
     * 构造函数
     * @param content 文本内容
     * @param location 源文件位置
     * @param line 行号
     * @param column 列号
     */
    TextNode(const std::string& content, 
             const std::string& location = "", 
             int line = 0, 
             int column = 0);
    
    /**
     * 获取文本内容
     * @return 文本内容
     */
    const std::string& getContent() const;
    
    /**
     * 设置文本内容
     * @param content 文本内容
     */
    void setContent(const std::string& content);
    
    /**
     * 是否为无修饰字面量
     * @return 是否为无修饰字面量
     */
    bool isUnquotedLiteral() const;
    
    /**
     * 设置是否为无修饰字面量
     * @param is_unquoted 是否为无修饰字面量
     */
    void setUnquotedLiteral(bool is_unquoted);
    
    /**
     * 是否包含插值表达式
     * @return 是否包含插值
     */
    bool isInterpolated() const;
    
    /**
     * 设置是否包含插值表达式
     * @param is_interpolated 是否包含插值
     */
    void setInterpolated(bool is_interpolated);
    
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
     * 处理无修饰字面量
     * @return 处理后的内容
     */
    std::string processUnquotedLiteral() const;
    
    /**
     * 处理插值表达式
     * @return 处理后的内容
     */
    std::string processInterpolation() const;
    
    /**
     * 生成最终HTML文本
     * @return HTML文本
     */
    std::string generateHTML() const;
};

} // namespace Node
} // namespace CHTL