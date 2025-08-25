#pragma once

#include "CHTL/Core/Ast.h"
#include <string>

namespace CHTL {

    class ImportResolver {
    public:
        void Resolve(std::shared_ptr<ProgramNode> ast);

    private:
        void Visit(AstNodePtr node);
        void ResolveImport(const ImportNode& node);
        std::string ResolvePath(const std::string& importPath);
    };

}
