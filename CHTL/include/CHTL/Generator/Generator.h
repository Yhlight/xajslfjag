#pragma once

#include "CHTL/Core/Ast.h"
#include <string>
#include <memory>

#include "CHTL/Core/GlobalMap.h"

namespace CHTL {

    class Generator {
    public:
        Generator(std::shared_ptr<ProgramNode> program, const GlobalMap& globals);

        std::string Generate();

    private:
        void Visit(AstNodePtr node);
        void VisitElementNode(std::shared_ptr<ElementNode> node);
        void VisitTextNode(std::shared_ptr<TextNode> node);
        void VisitCommentNode(std::shared_ptr<CommentNode> node);
        void VisitOriginNode(std::shared_ptr<OriginNode> node);
        void VisitOriginUsageNode(std::shared_ptr<OriginUsageNode> node);
        void VisitScriptNode(std::shared_ptr<ScriptNode> node);

        void AppendIndent();

    private:
        std::shared_ptr<ProgramNode> m_Program;
        const GlobalMap& m_Globals;
        std::string m_HtmlOutput;
        std::string m_CssOutput; // For later use with style blocks
        int m_IndentLevel = 0;
        
        // For & reference resolution
        std::string m_CurrentElementClass;
        std::string m_CurrentElementId;
    };

}
