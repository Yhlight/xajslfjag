#pragma once

#include <string>
#include <memory>
#include <stack>
#include "CHTL/Core/Ast.h"
#include "CHTL/Core/GlobalMap.h"
#include "CHTL/Core/Config.h"

namespace CHTL {

    class NamespaceResolver {
    public:
        NamespaceResolver(const Config& config);
        
        // Process AST and build namespace-aware global map
        GlobalMap Resolve(std::shared_ptr<ProgramNode> ast, const std::string& defaultNamespace = "");
        
    private:
        void Visit(AstNodePtr node, GlobalMap& currentScope);
        void ProcessNamespaceNode(std::shared_ptr<NamespaceNode> node, GlobalMap& parentScope);
        
        std::string GetCurrentNamespacePath() const;
        
    private:
        const Config& m_Config;
        std::stack<std::string> m_NamespaceStack;
    };

}