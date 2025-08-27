#pragma once

namespace CHTL {

// 前向声明
class BaseNode;
class ElementNode;
class TextNode;
class CommentNode;
class TemplateNode;
class CustomNode;
class StyleNode;
class ScriptNode;
class OriginNode;
class ImportNode;
class ConfigNode;
class NamespaceNode;
class OperatorNode;

/**
 * @brief 节点访问者接口
 * 实现访问者模式，用于遍历和处理节点树
 */
class NodeVisitor {
public:
    /**
     * @brief 虚析构函数
     */
    virtual ~NodeVisitor() = default;

    /**
     * @brief 访问基础节点
     * @param node 基础节点
     */
    virtual void visitBase(BaseNode& node) = 0;

    /**
     * @brief 访问元素节点
     * @param node 元素节点
     */
    virtual void visitElement(ElementNode& node) = 0;

    /**
     * @brief 访问文本节点
     * @param node 文本节点
     */
    virtual void visitText(TextNode& node) = 0;

    /**
     * @brief 访问注释节点
     * @param node 注释节点
     */
    virtual void visitComment(CommentNode& node) = 0;

    /**
     * @brief 访问模板节点
     * @param node 模板节点
     */
    virtual void visitTemplate(TemplateNode& node) = 0;

    /**
     * @brief 访问自定义节点
     * @param node 自定义节点
     */
    virtual void visitCustom(CustomNode& node) = 0;

    /**
     * @brief 访问样式节点
     * @param node 样式节点
     */
    virtual void visitStyle(StyleNode& node) = 0;

    /**
     * @brief 访问脚本节点
     * @param node 脚本节点
     */
    virtual void visitScript(ScriptNode& node) = 0;

    /**
     * @brief 访问原始节点
     * @param node 原始节点
     */
    virtual void visitOrigin(OriginNode& node) = 0;

    /**
     * @brief 访问导入节点
     * @param node 导入节点
     */
    virtual void visitImport(ImportNode& node) = 0;

    /**
     * @brief 访问配置节点
     * @param node 配置节点
     */
    virtual void visitConfig(ConfigNode& node) = 0;

    /**
     * @brief 访问命名空间节点
     * @param node 命名空间节点
     */
    virtual void visitNamespace(NamespaceNode& node) = 0;

    /**
     * @brief 访问操作符节点
     * @param node 操作符节点
     */
    virtual void visitOperator(OperatorNode& node) = 0;
};

/**
 * @brief 默认节点访问者
 * 提供默认的空实现
 */
class DefaultNodeVisitor : public NodeVisitor {
public:
    virtual void visitBase(BaseNode& /*node*/) override {}
    virtual void visitElement(ElementNode& /*node*/) override {}
    virtual void visitText(TextNode& /*node*/) override {}
    virtual void visitComment(CommentNode& /*node*/) override {}
    virtual void visitTemplate(TemplateNode& /*node*/) override {}
    virtual void visitCustom(CustomNode& /*node*/) override {}
    virtual void visitStyle(StyleNode& /*node*/) override {}
    virtual void visitScript(ScriptNode& /*node*/) override {}
    virtual void visitOrigin(OriginNode& /*node*/) override {}
    virtual void visitImport(ImportNode& /*node*/) override {}
    virtual void visitConfig(ConfigNode& /*node*/) override {}
    virtual void visitNamespace(NamespaceNode& /*node*/) override {}
    virtual void visitOperator(OperatorNode& /*node*/) override {}
};

} // namespace CHTL