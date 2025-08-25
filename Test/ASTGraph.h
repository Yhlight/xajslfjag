#pragma once

#include "ASTVisitor.h"
#include <iostream>
#include <string>

namespace CHTL {
namespace Test {

    class ASTGraph : public ASTVisitor {
    public:
        void Generate(const ProgramNode& root, const std::string& filename);

        void Visit(const ProgramNode& node) override;
        void Visit(const ElementNode& node) override;
        void Visit(const TextNode& node) override;
        // ... all other visit methods

    private:
        std::string GetNodeId(const AstNode* node);
        int m_NodeCounter = 0;
        std::ostream* m_OutputStream;
    };

}
}
