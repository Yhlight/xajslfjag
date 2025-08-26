#pragma once

#include "Test/ASTVisitor.h"
#include <stack>
#include <vector>

namespace CHTL {

    enum class SyntaxContext {
        Global,
        ElementBody,
        LocalStyle,
        GlobalStyle,
        LocalScript,
        GlobalScript,
        TemplateDef,
        CustomDef
    };

    class SyntaxConstrainer : public Test::ASTVisitor {
    public:
        void Validate(const ProgramNode& root);

        // Override all Visit methods from ASTVisitor
        void Visit(const ProgramNode& node) override;
        void Visit(const ElementNode& node) override;
        void Visit(const TextNode& node) override;
        void Visit(const AttributeNode& node) override;
        void Visit(const StyleNode& node) override;
        void Visit(const ScriptNode& node) override;
        void Visit(const CommentNode& node) override;
        void Visit(const TemplateDefinitionNode& node) override;
        void Visit(const CustomDefinitionNode& node) override;
        void Visit(const ImportNode& node) override;
        void Visit(const NamespaceNode& node) override;
        void Visit(const ConfigNode& node) override;
        void Visit(const VarUsageNode& node) override;
        void Visit(const TemplateUsageNode& node) override;
        void Visit(const InheritNode& node) override;
        void Visit(const DeleteNode& node) override;
        void Visit(const OriginNode& node) override;
        void Visit(const InlineStyleRuleNode& node) override { CheckRule(node); }
        void Visit(const SelectorBlockNode& node) override { CheckRule(node); }
        void Visit(const UseNode& node) override { CheckRule(node); }

    private:
        void PushContext(SyntaxContext ctx);
        void PopContext();
        SyntaxContext GetCurrentContext() const;

        void CheckRule(const AstNode& node);

        std::stack<SyntaxContext> m_ContextStack;
    };

}
