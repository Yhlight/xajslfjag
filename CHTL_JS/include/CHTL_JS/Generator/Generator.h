#pragma once

#include "../Core/Ast.h"
#include <string>
#include <memory>

namespace CHTL_JS {

    class Generator {
    public:
        Generator(std::shared_ptr<ProgramNode> program);

        std::string Generate();

    private:
        void Visit(AstNodePtr node);
        void VisitListenNode(std::shared_ptr<ListenNode> node);
        void VisitDelegateNode(std::shared_ptr<DelegateNode> node);
        void VisitAnimateNode(std::shared_ptr<AnimateNode> node);
        void VisitVirObjectDefNode(std::shared_ptr<VirObjectDefNode> node);
        // ... Visit methods for each node type

    private:
        std::shared_ptr<ProgramNode> m_Program;
        std::string m_JsOutput;
        std::map<std::string, std::shared_ptr<VirObjectDefNode>> m_VirSymbolTable;
    };

}
