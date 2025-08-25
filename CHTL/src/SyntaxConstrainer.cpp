#include "../SyntaxConstrainer.h"
#include <stdexcept>
#include <typeinfo>
#include <string>

namespace CHTL {

void SyntaxConstrainer::PushContext(SyntaxContext ctx) { m_ContextStack.push(ctx); }
void SyntaxConstrainer::PopContext() { m_ContextStack.pop(); }
SyntaxContext SyntaxConstrainer::GetCurrentContext() const { 
    return m_ContextStack.empty() ? SyntaxContext::Global : m_ContextStack.top(); 
}

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

void SyntaxConstrainer::Visit(const StyleNode& node) {
    CheckRule(node);
    // Determine if this is a local or global style block
    SyntaxContext parentCtx = GetCurrentContext();
    if (parentCtx == SyntaxContext::ElementBody) {
        PushContext(SyntaxContext::LocalStyle);
    } else {
        PushContext(SyntaxContext::GlobalStyle);
    }
    
    for (const auto& rule : node.Rules) {
        rule->Accept(this);
    }
    PopContext();
}

void SyntaxConstrainer::Visit(const ScriptNode& node) {
    CheckRule(node);
    // Determine if this is a local or global script block
    SyntaxContext parentCtx = GetCurrentContext();
    if (parentCtx == SyntaxContext::ElementBody) {
        PushContext(SyntaxContext::LocalScript);
    } else {
        PushContext(SyntaxContext::GlobalScript);
    }
    // Script content is processed by CHTL JS compiler, not here
    PopContext();
}

void SyntaxConstrainer::Visit(const TemplateDefinitionNode& node) {
    CheckRule(node);
    PushContext(SyntaxContext::TemplateDef);
    for (const auto& child : node.Body) {
        child->Accept(this);
    }
    PopContext();
}

void SyntaxConstrainer::Visit(const CustomDefinitionNode& node) {
    CheckRule(node);
    PushContext(SyntaxContext::CustomDef);
    for (const auto& child : node.Body) {
        child->Accept(this);
    }
    PopContext();
}

void SyntaxConstrainer::Visit(const NamespaceNode& node) {
    CheckRule(node);
    // Namespace maintains current context
    for (const auto& child : node.Body) {
        child->Accept(this);
    }
}

void SyntaxConstrainer::Visit(const CommentNode& node) {
    // Comments are always allowed
    CheckRule(node);
}

void SyntaxConstrainer::Visit(const OriginNode& node) {
    // Origin nodes are always allowed
    CheckRule(node);
}

void SyntaxConstrainer::Visit(const TextNode& node) {
    CheckRule(node);
}

void SyntaxConstrainer::Visit(const AttributeNode& node) {
    CheckRule(node);
}

void SyntaxConstrainer::Visit(const VarUsageNode& node) {
    CheckRule(node);
}

void SyntaxConstrainer::Visit(const TemplateUsageNode& node) {
    CheckRule(node);
}

void SyntaxConstrainer::Visit(const InheritNode& node) {
    CheckRule(node);
}

void SyntaxConstrainer::Visit(const DeleteNode& node) {
    CheckRule(node);
}

void SyntaxConstrainer::Visit(const ImportNode& node) {
    CheckRule(node);
}

void SyntaxConstrainer::Visit(const ConfigNode& node) {
    CheckRule(node);
}

void SyntaxConstrainer::CheckRule(const AstNode& node) {
    SyntaxContext currentCtx = GetCurrentContext();
    std::string nodeType = typeid(node).name();
    
    // Comments and Origin nodes are allowed everywhere
    if (dynamic_cast<const CommentNode*>(&node) || 
        dynamic_cast<const OriginNode*>(&node)) {
        return;
    }

    switch (currentCtx) {
        case SyntaxContext::GlobalStyle:
            // According to CHTL docs: Global style blocks only allow:
            // - Template variable usage
            // - Custom variable usage and specialization
            // - Template/custom/valueless style groups
            // - Delete attributes/inheritance
            // - Inheritance between style groups
            // - Generator comments
            // - Full prefix names
            // - Origin embeds
            // - Namespace from
            if (!dynamic_cast<const VarUsageNode*>(&node) && 
                !dynamic_cast<const TemplateUsageNode*>(&node) &&
                !dynamic_cast<const InheritNode*>(&node) &&
                !dynamic_cast<const DeleteNode*>(&node) &&
                !dynamic_cast<const InlineStyleRuleNode*>(&node) &&
                !dynamic_cast<const SelectorBlockNode*>(&node)) {
                throw std::runtime_error("Syntax Error: '" + nodeType + 
                    "' is not allowed in global style blocks. Only template/custom variables, "
                    "style groups, inheritance, and delete operations are permitted.");
            }
            break;
            
        case SyntaxContext::LocalStyle:
            // Local style blocks can use everything global style can use
            // Plus they handle selector automation
            // The actual selector rules are checked elsewhere
            break;
            
        case SyntaxContext::GlobalScript:
            // According to CHTL docs: Global scripts (except local script) forbid ANY CHTL syntax
            // Only allowed: -- comments and origin embeds
            throw std::runtime_error("Syntax Error: CHTL syntax is not allowed in global script blocks. "
                "Only JavaScript code, -- comments, and origin embeds are permitted.");
            break;
            
        case SyntaxContext::LocalScript:
            // According to CHTL docs: Local scripts allow:
            // - Template variables
            // - Custom variable groups and specialization
            // - Namespace from
            // - -- comments and origin embeds
            if (!dynamic_cast<const VarUsageNode*>(&node) && 
                !dynamic_cast<const TemplateUsageNode*>(&node)) {
                throw std::runtime_error("Syntax Error: '" + nodeType + 
                    "' is not allowed in local script blocks. Only template/custom variables, "
                    "namespace from, -- comments, and origin embeds are permitted.");
            }
            break;
            
        case SyntaxContext::ElementBody:
            // Elements can contain other elements, text, attributes, style, script
            if (!dynamic_cast<const ElementNode*>(&node) &&
                !dynamic_cast<const TextNode*>(&node) &&
                !dynamic_cast<const AttributeNode*>(&node) &&
                !dynamic_cast<const StyleNode*>(&node) &&
                !dynamic_cast<const ScriptNode*>(&node) &&
                !dynamic_cast<const TemplateUsageNode*>(&node)) {
                throw std::runtime_error("Syntax Error: '" + nodeType + 
                    "' is not allowed inside element bodies.");
            }
            break;
            
        case SyntaxContext::TemplateDef:
        case SyntaxContext::CustomDef:
            // Templates and customs can contain various definitions based on their type
            // The specific rules depend on the template/custom type which is checked elsewhere
            break;
            
        case SyntaxContext::Global:
            // At global scope, allow:
            // - Elements (html, body, etc.)
            // - Template/Custom definitions
            // - Import statements
            // - Namespace declarations
            // - Configuration blocks
            // - Global style/script blocks
            // - Origin nodes
            // - Comments
            if (!dynamic_cast<const ElementNode*>(&node) &&
                !dynamic_cast<const TemplateDefinitionNode*>(&node) &&
                !dynamic_cast<const CustomDefinitionNode*>(&node) &&
                !dynamic_cast<const ImportNode*>(&node) &&
                !dynamic_cast<const NamespaceNode*>(&node) &&
                !dynamic_cast<const ConfigNode*>(&node) &&
                !dynamic_cast<const StyleNode*>(&node) &&
                !dynamic_cast<const ScriptNode*>(&node)) {
                throw std::runtime_error("Syntax Error: '" + nodeType + 
                    "' is not allowed at global scope.");
            }
            break;
    }
}

}
