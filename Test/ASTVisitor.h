#pragma once

#include "CHTL/Core/Ast.h"

namespace CHTL {
namespace Test {

    // Base class for visiting the AST
    class ASTVisitor {
    public:
        virtual ~ASTVisitor() = default;

        virtual void Visit(const ProgramNode& node) = 0;
        virtual void Visit(const ElementNode& node) = 0;
        virtual void Visit(const TextNode& node) = 0;
        // ... one visit method for every single AST node type
    };

}
}
