#ifndef CHTL_BASE_NODE_H
#define CHTL_BASE_NODE_H

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

namespace CHTL {

// 节点类型枚举
enum class NodeType {
    // 基础节点
    ELEMENT,        // HTML元素节点
    TEXT,           // 文本节点
    COMMENT,        // 注释节点
    
    // 特殊节点
    TEMPLATE,       // 模板节点
    CUSTOM,         // 自定义节点
    STYLE,          // 样式节点
    SCRIPT,         // 脚本节点
    ORIGIN,         // 原始嵌入节点
    IMPORT,         // 导入节点
    CONFIG,         // 配置节点
    NAMESPACE,      // 命名空间节点
    OPERATOR,       // 操作符节点（delete, insert, use等）
    
    // 根节点
    DOCUMENT        // 文档根节点
};

// 前向声明
class NodeVisitor;

// 基础节点类
class BaseNode : public std::enable_shared_from_this<BaseNode> {
protected:
    NodeType type;
    int line;
    int column;
    std::vector<std::shared_ptr<BaseNode>> children;
    std::weak_ptr<BaseNode> parent;
    
public:
    BaseNode(NodeType t, int l = 0, int c = 0) 
        : type(t), line(l), column(c) {}
    
    virtual ~BaseNode() = default;
    
    // 基本属性访问
    NodeType getType() const { return type; }
    int getLine() const { return line; }
    int getColumn() const { return column; }
    
    // 子节点管理
    void addChild(std::shared_ptr<BaseNode> child);
    void removeChild(std::shared_ptr<BaseNode> child);
    void insertChild(size_t index, std::shared_ptr<BaseNode> child);
    std::vector<std::shared_ptr<BaseNode>>& getChildren() { return children; }
    const std::vector<std::shared_ptr<BaseNode>>& getChildren() const { return children; }
    
    // 父节点管理
    void setParent(std::weak_ptr<BaseNode> p) { parent = p; }
    std::shared_ptr<BaseNode> getParent() const { return parent.lock(); }
    
    // 访问者模式
    virtual void accept(NodeVisitor* visitor) = 0;
    
    // 克隆节点
    virtual std::shared_ptr<BaseNode> clone() const = 0;
    
    // 序列化
    virtual std::string toString() const = 0;
};

// 元素节点类
class ElementNode : public BaseNode {
private:
    std::string tagName;
    std::unordered_map<std::string, std::string> attributes;
    std::vector<std::shared_ptr<BaseNode>> styleNodes;   // 局部样式块
    std::vector<std::shared_ptr<BaseNode>> scriptNodes;  // 局部脚本块
    
public:
    ElementNode(const std::string& tag, int line = 0, int col = 0)
        : BaseNode(NodeType::ELEMENT, line, col), tagName(tag) {}
    
    // 标签名访问
    const std::string& getTagName() const { return tagName; }
    void setTagName(const std::string& tag) { tagName = tag; }
    
    // 属性管理
    void setAttribute(const std::string& name, const std::string& value);
    std::string getAttribute(const std::string& name) const;
    bool hasAttribute(const std::string& name) const;
    void removeAttribute(const std::string& name);
    const std::unordered_map<std::string, std::string>& getAttributes() const { return attributes; }
    
    // 局部样式块管理
    void addStyleNode(std::shared_ptr<BaseNode> style);
    const std::vector<std::shared_ptr<BaseNode>>& getStyleNodes() const { return styleNodes; }
    
    // 局部脚本块管理
    void addScriptNode(std::shared_ptr<BaseNode> script);
    const std::vector<std::shared_ptr<BaseNode>>& getScriptNodes() const { return scriptNodes; }
    
    // 判断是否为单标签
    bool isSelfClosing() const;
    
    // 重写虚函数
    void accept(NodeVisitor* visitor) override;
    std::shared_ptr<BaseNode> clone() const override;
    std::string toString() const override;
};

// 文本节点类
class TextNode : public BaseNode {
private:
    std::string content;
    bool isLiteral;  // 是否为字面量文本（无引号）
    
public:
    TextNode(const std::string& text, bool literal = false, int line = 0, int col = 0)
        : BaseNode(NodeType::TEXT, line, col), content(text), isLiteral(literal) {}
    
    // 内容访问
    const std::string& getContent() const { return content; }
    void setContent(const std::string& text) { content = text; }
    
    // 字面量判断
    bool getIsLiteral() const { return isLiteral; }
    void setIsLiteral(bool literal) { isLiteral = literal; }
    
    // 重写虚函数
    void accept(NodeVisitor* visitor) override;
    std::shared_ptr<BaseNode> clone() const override;
    std::string toString() const override;
};

// 访问者接口
class NodeVisitor {
public:
    virtual ~NodeVisitor() = default;
    
    virtual void visitElementNode(ElementNode* node) = 0;
    virtual void visitTextNode(TextNode* node) = 0;
    // 其他节点类型的访问方法将在各自的文件中定义
};

} // namespace CHTL

#endif // CHTL_BASE_NODE_H