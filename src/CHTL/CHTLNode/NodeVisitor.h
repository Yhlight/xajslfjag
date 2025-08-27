#pragma once

namespace CHTL {

// 前向声明
class ElementNode;
class TextNode;
class CommentNode;
class TemplateNode;
class CustomNode;
class NamespaceNode;
class ConfigurationNode;
class ImportNode;
class OriginNode;
class StyleBlockNode;
class ScriptBlockNode;
class UseStatementNode;
class OperatorNode;

/// AST节点访问者接口
/// 实现访问者模式，用于遍历和处理AST节点
class NodeVisitor {
public:
    virtual ~NodeVisitor() = default;
    
    /// 访问元素节点
    virtual void VisitElement(ElementNode& node) = 0;
    
    /// 访问文本节点
    virtual void VisitText(TextNode& node) = 0;
    
    /// 访问注释节点
    virtual void VisitComment(CommentNode& node) = 0;
    
    /// 访问样式块节点
    virtual void VisitStyleBlock(StyleBlockNode& node) = 0;
    
    /// 访问脚本块节点
    virtual void VisitScriptBlock(ScriptBlockNode& node) = 0;
};

/// 基础遍历访问者，提供默认的遍历行为
class TraversalVisitor : public NodeVisitor {
public:
    /// 访问元素节点
    void VisitElement(ElementNode& node) override;
    
    /// 访问文本节点
    void VisitText(TextNode& node) override;
    
    /// 访问注释节点
    void VisitComment(CommentNode& node) override;
    
    /// 访问样式块节点
    void VisitStyleBlock(StyleBlockNode& node) override;
    
    /// 访问脚本块节点
    void VisitScriptBlock(ScriptBlockNode& node) override;

protected:
    /// 遍历子节点的通用方法
    void TraverseChildren(class BaseNode& node);
};

} // namespace CHTL