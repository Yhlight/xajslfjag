#pragma once

#include "CHTL/Core/Ast.h"
#include "CHTL/Core/GlobalMap.h"
#include <memory>

namespace CHTL {

    class Resolver {
    public:
        Resolver(std::shared_ptr<ProgramNode> ast, const GlobalMap& globals);

        std::shared_ptr<ProgramNode> Resolve();

    private:
        std::shared_ptr<ProgramNode> m_Ast;
        const GlobalMap& m_Globals;
        std::unordered_map<std::string, std::shared_ptr<TemplateDefinitionNode>> m_ResolvedTemplateCache;

        // Main resolution logic
        std::shared_ptr<TemplateDefinitionNode> ResolveTemplate(const std::string& name);

        // Visitor methods
        void VisitNode(AstNodePtr& node);
        void VisitElementNode(std::shared_ptr<ElementNode> node);
        void VisitStyleNode(std::shared_ptr<StyleNode> node);
        void VisitAttributeNode(std::shared_ptr<AttributeNode> node);
        void VisitInlineStyleRuleNode(std::shared_ptr<InlineStyleRuleNode> node);
    };

}
