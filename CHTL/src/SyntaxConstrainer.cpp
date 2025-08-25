#include "../SyntaxConstrainer.h"
#include <stdexcept>

namespace CHTL {

void SyntaxConstrainer::PushContext(SyntaxContext ctx) { m_ContextStack.push(ctx); }
void SyntaxConstrainer::PopContext() { m_ContextStack.pop(); }
SyntaxContext SyntaxConstrainer::GetCurrentContext() const { return m_ContextStack.top(); }

void SyntaxConstrainer::Validate(const ProgramNode& root) {
    PushContext(SyntaxContext::Global);
    root.Accept(this);
    PopContext();
}

void SyntaxConstrainer::Visit(const ProgramNode& node) {
    CheckRule(node);
    for (const auto& child : node.Children) {
        child->Accept(this);
    }
}

void SyntaxConstrainer::Visit(const ElementNode& node) {
    CheckRule(node);
    PushContext(SyntaxContext::ElementBody);
    for (const auto& child : node.Children) {
        child->Accept(this);
    }
    PopContext();
}

// ... other visit methods following the same pattern

void SyntaxConstrainer::CheckRule(const AstNode& node) {
    SyntaxContext currentCtx = GetCurrentContext();

    // Example for GlobalStyle context
    if (currentCtx == SyntaxContext::GlobalStyle) {
        // Only allow specific node types
        if (dynamic_cast<const VarUsageNode*>(&node) || 
            dynamic_cast<const TemplateUsageNode*>(&node) ||
            dynamic_cast<const InheritNode*>(&node) ||
            dynamic_cast<const CommentNode*>(&node) ||
            dynamic_cast<const OriginNode*>(&node)) {
            // Allowed
        } else {
            throw std::runtime_error("Syntax Error: Invalid statement in a global style block.");
        }
    }
    // ... rules for all other contexts
}

}
