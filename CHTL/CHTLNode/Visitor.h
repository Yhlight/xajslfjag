#ifndef CHTL_VISITOR_H
#define CHTL_VISITOR_H

namespace CHTL {

// 前向声明所有CHTL节点类型
class ProgramNode;
class CommentNode;
class TextNode;
class LiteralNode;
class IdentifierNode;
class BlockNode;
class StatementListNode;

class AttributeNode;
class ElementNode;
class StyleBlockNode;
class CSSRuleNode;
class ScriptBlockNode;
class PropertyNode;

class TemplateDeclarationNode;
class CustomDeclarationNode;
class TemplateUsageNode;
class CustomUsageNode;
class VariableReferenceNode;
class InheritStatementNode;
class DeleteStatementNode;
class InsertStatementNode;
class IndexAccessNode;

class OriginEmbedNode;
class ConfigurationNode;
class ConfigOptionNode;
class NameGroupNode;
class OriginTypeNode;
class ImportStatementNode;
class UseStatementNode;
class NamespaceDeclarationNode;
class ExceptConstraintNode;
class FromClauseNode;

// 具体的CHTL访问者接口
class CHTLVisitor : public Visitor {
public:
    virtual ~CHTLVisitor() = default;
    
    // 基础节点
    virtual void visitProgram(ProgramNode* node) = 0;
    virtual void visitComment(CommentNode* node) = 0;
    virtual void visitText(TextNode* node) = 0;
    virtual void visitLiteral(LiteralNode* node) = 0;
    virtual void visitIdentifier(IdentifierNode* node) = 0;
    virtual void visitBlock(BlockNode* node) = 0;
    virtual void visitStatementList(StatementListNode* node) = 0;
    
    // HTML元素节点
    virtual void visitAttribute(AttributeNode* node) = 0;
    virtual void visitElement(ElementNode* node) = 0;
    virtual void visitStyleBlock(StyleBlockNode* node) = 0;
    virtual void visitCSSRule(CSSRuleNode* node) = 0;
    virtual void visitScriptBlock(ScriptBlockNode* node) = 0;
    virtual void visitProperty(PropertyNode* node) = 0;
    
    // 模板和自定义节点
    virtual void visitTemplateDeclaration(TemplateDeclarationNode* node) = 0;
    virtual void visitCustomDeclaration(CustomDeclarationNode* node) = 0;
    virtual void visitTemplateUsage(TemplateUsageNode* node) = 0;
    virtual void visitCustomUsage(CustomUsageNode* node) = 0;
    virtual void visitVariableReference(VariableReferenceNode* node) = 0;
    virtual void visitInheritStatement(InheritStatementNode* node) = 0;
    virtual void visitDeleteStatement(DeleteStatementNode* node) = 0;
    virtual void visitInsertStatement(InsertStatementNode* node) = 0;
    virtual void visitIndexAccess(IndexAccessNode* node) = 0;
    
    // 系统节点
    virtual void visitOriginEmbed(OriginEmbedNode* node) = 0;
    virtual void visitConfiguration(ConfigurationNode* node) = 0;
    virtual void visitConfigOption(ConfigOptionNode* node) = 0;
    virtual void visitNameGroup(NameGroupNode* node) = 0;
    virtual void visitOriginType(OriginTypeNode* node) = 0;
    virtual void visitImportStatement(ImportStatementNode* node) = 0;
    virtual void visitUseStatement(UseStatementNode* node) = 0;
    virtual void visitNamespaceDeclaration(NamespaceDeclarationNode* node) = 0;
    virtual void visitExceptConstraint(ExceptConstraintNode* node) = 0;
    virtual void visitFromClause(FromClauseNode* node) = 0;
    
    // 通用访问方法（根据节点类型分发）
    void visit(Node* node) override;
};

// 基础访问者实现（提供默认遍历行为）
class BaseCHTLVisitor : public CHTLVisitor {
public:
    // 默认实现：遍历所有子节点
    void visitProgram(ProgramNode* node) override;
    void visitComment(CommentNode* node) override;
    void visitText(TextNode* node) override;
    void visitLiteral(LiteralNode* node) override;
    void visitIdentifier(IdentifierNode* node) override;
    void visitBlock(BlockNode* node) override;
    void visitStatementList(StatementListNode* node) override;
    
    void visitAttribute(AttributeNode* node) override;
    void visitElement(ElementNode* node) override;
    void visitStyleBlock(StyleBlockNode* node) override;
    void visitCSSRule(CSSRuleNode* node) override;
    void visitScriptBlock(ScriptBlockNode* node) override;
    void visitProperty(PropertyNode* node) override;
    
    void visitTemplateDeclaration(TemplateDeclarationNode* node) override;
    void visitCustomDeclaration(CustomDeclarationNode* node) override;
    void visitTemplateUsage(TemplateUsageNode* node) override;
    void visitCustomUsage(CustomUsageNode* node) override;
    void visitVariableReference(VariableReferenceNode* node) override;
    void visitInheritStatement(InheritStatementNode* node) override;
    void visitDeleteStatement(DeleteStatementNode* node) override;
    void visitInsertStatement(InsertStatementNode* node) override;
    void visitIndexAccess(IndexAccessNode* node) override;
    
    void visitOriginEmbed(OriginEmbedNode* node) override;
    void visitConfiguration(ConfigurationNode* node) override;
    void visitConfigOption(ConfigOptionNode* node) override;
    void visitNameGroup(NameGroupNode* node) override;
    void visitOriginType(OriginTypeNode* node) override;
    void visitImportStatement(ImportStatementNode* node) override;
    void visitUseStatement(UseStatementNode* node) override;
    void visitNamespaceDeclaration(NamespaceDeclarationNode* node) override;
    void visitExceptConstraint(ExceptConstraintNode* node) override;
    void visitFromClause(FromClauseNode* node) override;
    
protected:
    // 辅助方法：访问所有子节点
    void visitChildren(Node* node);
};

} // namespace CHTL

#endif // CHTL_VISITOR_H