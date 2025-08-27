#pragma once

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
    
    // 操作符节点
    DELETE_OP,      // delete操作
    INSERT_OP,      // insert操作
    USE_OP,         // use操作
    
    // 其他
    ATTRIBUTE,      // 属性节点
    SELECTOR,       // 选择器节点
    EXPRESSION,     // 表达式节点
};

// 前向声明
class BaseNode;
using NodePtr = std::shared_ptr<BaseNode>;

// 基础节点类
class BaseNode {
public:
    BaseNode(NodeType type) : nodeType(type), line(0), column(0) {}
    virtual ~BaseNode() = default;
    
    // 获取节点类型
    NodeType GetType() const { return nodeType; }
    
    // 获取节点名称
    virtual std::string GetName() const { return ""; }
    
    // 生成代码
    virtual std::string Generate() const = 0;
    
    // 克隆节点
    virtual NodePtr Clone() const = 0;
    
    // 访问者模式
    virtual void Accept(class NodeVisitor* visitor) = 0;
    
    // 子节点管理
    virtual void AddChild(NodePtr child) { children.push_back(child); }
    virtual void RemoveChild(NodePtr child);
    virtual NodePtr GetChild(size_t index) const;
    virtual size_t GetChildCount() const { return children.size(); }
    virtual std::vector<NodePtr>& GetChildren() { return children; }
    virtual const std::vector<NodePtr>& GetChildren() const { return children; }
    
    // 位置信息
    void SetPosition(int l, int c) { line = l; column = c; }
    int GetLine() const { return line; }
    int GetColumn() const { return column; }
    
    // 调试信息
    virtual std::string ToString() const;
    
protected:
    NodeType nodeType;
    std::vector<NodePtr> children;
    int line;
    int column;
};

// 元素节点
class ElementNode : public BaseNode {
public:
    ElementNode(const std::string& tag);
    
    std::string GetName() const override { return tagName; }
    std::string Generate() const override;
    NodePtr Clone() const override;
    void Accept(NodeVisitor* visitor) override;
    
    // 属性管理
    void SetAttribute(const std::string& name, const std::string& value);
    std::string GetAttribute(const std::string& name) const;
    bool HasAttribute(const std::string& name) const;
    void RemoveAttribute(const std::string& name);
    std::unordered_map<std::string, std::string>& GetAttributes() { return attributes; }
    const std::unordered_map<std::string, std::string>& GetAttributes() const { return attributes; }
    
    // 样式和脚本节点
    void SetStyleNode(NodePtr style) { styleNode = style; }
    void SetScriptNode(NodePtr script) { scriptNode = script; }
    NodePtr GetStyleNode() const { return styleNode; }
    NodePtr GetScriptNode() const { return scriptNode; }
    
    // 标签类型
    bool IsSelfClosing() const;
    bool IsVoidElement() const;
    
private:
    std::string tagName;
    std::unordered_map<std::string, std::string> attributes;
    NodePtr styleNode;
    NodePtr scriptNode;
};

// 文本节点
class TextNode : public BaseNode {
public:
    TextNode(const std::string& content);
    
    std::string GetContent() const { return content; }
    void SetContent(const std::string& c) { content = c; }
    
    std::string Generate() const override;
    NodePtr Clone() const override;
    void Accept(NodeVisitor* visitor) override;
    
private:
    std::string content;
};

// 访问者接口
class NodeVisitor {
public:
    virtual ~NodeVisitor() = default;
    
    virtual void VisitElementNode(ElementNode* node) = 0;
    virtual void VisitTextNode(TextNode* node) = 0;
    virtual void VisitCommentNode(class CommentNode* node) = 0;
    virtual void VisitTemplateNode(class TemplateNode* node) = 0;
    virtual void VisitCustomNode(class CustomNode* node) = 0;
    virtual void VisitStyleNode(class StyleNode* node) = 0;
    virtual void VisitScriptNode(class ScriptNode* node) = 0;
    virtual void VisitOriginNode(class OriginNode* node) = 0;
    virtual void VisitImportNode(class ImportNode* node) = 0;
    virtual void VisitConfigNode(class ConfigNode* node) = 0;
    virtual void VisitNamespaceNode(class NamespaceNode* node) = 0;
    virtual void VisitOperatorNode(class OperatorNode* node) = 0;
};

} // namespace CHTL