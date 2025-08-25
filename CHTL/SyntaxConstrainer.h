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
        // ... and so on for all node types

    private:
        void PushContext(SyntaxContext ctx);
        void PopContext();
        SyntaxContext GetCurrentContext() const;

        void CheckRule(const AstNode& node);

        std::stack<SyntaxContext> m_ContextStack;
    };

}
