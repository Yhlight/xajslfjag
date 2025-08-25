#pragma once

#include "ASTVisitor.h"
#include <iostream>

namespace CHTL {
namespace Test {

    class ASTPrint : public ASTVisitor {
    public:
        void Print(const ProgramNode& root);

        void Visit(const ProgramNode& node) override;
        void Visit(const ElementNode& node) override;
        void Visit(const TextNode& node) override;
        // ... all other visit methods

    private:
        void Indent();
        int m_IndentLevel = 0;
    };

}
}
