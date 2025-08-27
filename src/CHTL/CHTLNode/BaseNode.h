#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

namespace CHTL {

/// AST节点类型枚举
enum class NodeType {
    // 基础节点
    ELEMENT,            // 元素节点
    TEXT,               // 文本节点
    COMMENT,            // 注释节点
    
    // 结构节点
    TEMPLATE,           // 模板节点 [Template]
    CUSTOM,             // 自定义节点 [Custom]
    NAMESPACE,          // 命名空间节点 [Namespace]
    CONFIGURATION,      // 配置节点 [Configuration]
    IMPORT,             // 导入节点 [Import]
    ORIGIN,             // 原始嵌入节点 [Origin]
    
    // 特殊节点
    STYLE_BLOCK,        // 样式块节点 style {}
    SCRIPT_BLOCK,       // 脚本块节点 script {}
    USE_STATEMENT,      // use语句节点
    EXCEPT_CLAUSE,      // except子句节点
    OPERATOR,           // 操作符节点 (delete, insert, inherit等)
    
    // 属性和值
    ATTRIBUTE,          // 属性节点
    ATTRIBUTE_VALUE,    // 属性值节点
    STYLE_PROPERTY,     // 样式属性节点
    VARIABLE_REFERENCE, // 变量引用节点
    
    UNKNOWN             // 未知节点
};

/// 节点位置信息
struct NodePosition {
    size_t line;        // 行号
    size_t column;      // 列号
    size_t offset;      // 偏移量
    
    NodePosition(size_t l = 1, size_t c = 1, size_t o = 0)
        : line(l), column(c), offset(o) {}
};

/// AST节点基类
class BaseNode : public std::enable_shared_from_this<BaseNode> {
protected:
    NodeType nodeType;
    NodePosition position;
    std::weak_ptr<BaseNode> parent;
    std::vector<std::shared_ptr<BaseNode>> children;
    std::unordered_map<std::string, std::string> metadata;

public:
    /// 构造函数
    explicit BaseNode(NodeType type, const NodePosition& pos = NodePosition());
    
    /// 虚析构函数
    virtual ~BaseNode() = default;
    
    // 禁止拷贝构造和赋值
    BaseNode(const BaseNode&) = delete;
    BaseNode& operator=(const BaseNode&) = delete;
    
    // 允许移动
    BaseNode(BaseNode&&) noexcept = default;
    BaseNode& operator=(BaseNode&&) noexcept = default;
    
    /// 获取节点类型
    NodeType GetNodeType() const { return nodeType; }
    
    /// 获取位置信息
    const NodePosition& GetPosition() const { return position; }
    
    /// 设置位置信息
    void SetPosition(const NodePosition& pos) { position = pos; }
    
    /// 获取父节点
    std::shared_ptr<BaseNode> GetParent() const { return parent.lock(); }
    
    /// 设置父节点
    void SetParent(std::shared_ptr<BaseNode> parentNode) { parent = parentNode; }
    
    /// 添加子节点
    void AddChild(std::shared_ptr<BaseNode> child);
    
    /// 插入子节点
    void InsertChild(size_t index, std::shared_ptr<BaseNode> child);
    
    /// 移除子节点
    void RemoveChild(std::shared_ptr<BaseNode> child);
    
    /// 移除指定索引的子节点
    void RemoveChild(size_t index);
    
    /// 获取子节点列表
    const std::vector<std::shared_ptr<BaseNode>>& GetChildren() const { return children; }
    
    /// 获取子节点数量
    size_t GetChildCount() const { return children.size(); }
    
    /// 获取指定索引的子节点
    std::shared_ptr<BaseNode> GetChild(size_t index) const;
    
    /// 查找特定类型的子节点
    std::vector<std::shared_ptr<BaseNode>> FindChildrenByType(NodeType type) const;
    
    /// 查找第一个特定类型的子节点
    std::shared_ptr<BaseNode> FindFirstChildByType(NodeType type) const;
    
    /// 设置元数据
    void SetMetadata(const std::string& key, const std::string& value);
    
    /// 获取元数据
    std::string GetMetadata(const std::string& key) const;
    
    /// 检查是否有元数据
    bool HasMetadata(const std::string& key) const;
    
    /// 获取所有元数据
    const std::unordered_map<std::string, std::string>& GetAllMetadata() const { return metadata; }
    
    /// 克隆节点
    virtual std::shared_ptr<BaseNode> Clone() const = 0;
    
    /// 验证节点
    virtual bool Validate() const;
    
    /// 获取节点描述（用于调试）
    virtual std::string GetDescription() const;
    
    /// 转换为字符串表示
    virtual std::string ToString() const;
    
    /// 接受访问者模式
    virtual void Accept(class NodeVisitor& visitor) = 0;
};

/// 元素节点类
class ElementNode : public BaseNode {
private:
    std::string tagName;                                        // 标签名
    std::unordered_map<std::string, std::string> attributes;   // 属性映射
    bool isSelfClosing;                                         // 是否自闭合标签

public:
    /// 构造函数
    explicit ElementNode(const std::string& tag, const NodePosition& pos = NodePosition());
    
    /// 设置标签名
    void SetTagName(const std::string& tag) { tagName = tag; }
    
    /// 获取标签名
    const std::string& GetTagName() const { return tagName; }
    
    /// 设置属性
    void SetAttribute(const std::string& name, const std::string& value);
    
    /// 获取属性值
    std::string GetAttribute(const std::string& name) const;
    
    /// 检查是否有属性
    bool HasAttribute(const std::string& name) const;
    
    /// 移除属性
    void RemoveAttribute(const std::string& name);
    
    /// 获取所有属性
    const std::unordered_map<std::string, std::string>& GetAttributes() const { return attributes; }
    
    /// 设置是否为自闭合标签
    void SetSelfClosing(bool selfClosing) { isSelfClosing = selfClosing; }
    
    /// 检查是否为自闭合标签
    bool IsSelfClosing() const { return isSelfClosing; }
    
    /// 添加CSS类
    void AddCssClass(const std::string& className);
    
    /// 移除CSS类
    void RemoveCssClass(const std::string& className);
    
    /// 检查是否有CSS类
    bool HasCssClass(const std::string& className) const;
    
    /// 设置ID
    void SetId(const std::string& id);
    
    /// 获取ID
    std::string GetId() const;
    
    /// 获取文本内容（所有子TextNode的内容）
    std::string GetTextContent() const;
    
    /// 克隆节点
    std::shared_ptr<BaseNode> Clone() const override;
    
    /// 验证节点
    bool Validate() const override;
    
    /// 获取节点描述
    std::string GetDescription() const override;
    
    /// 接受访问者
    void Accept(NodeVisitor& visitor) override;
};

/// 文本节点类
class TextNode : public BaseNode {
private:
    std::string content;        // 文本内容
    bool isUnquoted;           // 是否为无修饰字面量

public:
    /// 构造函数
    explicit TextNode(const std::string& text, const NodePosition& pos = NodePosition());
    
    /// 设置文本内容
    void SetContent(const std::string& text) { content = text; }
    
    /// 获取文本内容
    const std::string& GetContent() const { return content; }
    
    /// 设置是否为无修饰字面量
    void SetUnquoted(bool unquoted) { isUnquoted = unquoted; }
    
    /// 检查是否为无修饰字面量
    bool IsUnquoted() const { return isUnquoted; }
    
    /// 获取处理后的文本内容（处理转义字符等）
    std::string GetProcessedContent() const;
    
    /// 检查是否为空白文本
    bool IsWhitespace() const;
    
    /// 克隆节点
    std::shared_ptr<BaseNode> Clone() const override;
    
    /// 验证节点
    bool Validate() const override;
    
    /// 获取节点描述
    std::string GetDescription() const override;
    
    /// 接受访问者
    void Accept(NodeVisitor& visitor) override;
};

/// 注释节点类
class CommentNode : public BaseNode {
public:
    /// 注释类型
    enum class CommentType {
        LINE,           // 单行注释 //
        BLOCK,          // 块注释 /* */
        GENERATOR       // 生成器注释 --
    };

private:
    std::string content;        // 注释内容
    CommentType commentType;    // 注释类型

public:
    /// 构造函数
    explicit CommentNode(const std::string& text, CommentType type, const NodePosition& pos = NodePosition());
    
    /// 设置注释内容
    void SetContent(const std::string& text) { content = text; }
    
    /// 获取注释内容
    const std::string& GetContent() const { return content; }
    
    /// 设置注释类型
    void SetCommentType(CommentType type) { commentType = type; }
    
    /// 获取注释类型
    CommentType GetCommentType() const { return commentType; }
    
    /// 是否为生成器注释
    bool IsGeneratorComment() const { return commentType == CommentType::GENERATOR; }
    
    /// 获取注释类型名称
    std::string GetCommentTypeName() const;
    
    /// 克隆节点
    std::shared_ptr<BaseNode> Clone() const override;
    
    /// 验证节点
    bool Validate() const override;
    
    /// 获取节点描述
    std::string GetDescription() const override;
    
    /// 接受访问者
    void Accept(NodeVisitor& visitor) override;
};

/// 节点类型转换为字符串
std::string NodeTypeToString(NodeType type);

/// 创建节点的工厂函数
class NodeFactory {
public:
    /// 创建元素节点
    static std::shared_ptr<ElementNode> CreateElement(const std::string& tagName, const NodePosition& pos = NodePosition());
    
    /// 创建文本节点
    static std::shared_ptr<TextNode> CreateText(const std::string& content, const NodePosition& pos = NodePosition());
    
    /// 创建注释节点
    static std::shared_ptr<CommentNode> CreateComment(const std::string& content, CommentNode::CommentType type, const NodePosition& pos = NodePosition());
};

} // namespace CHTL