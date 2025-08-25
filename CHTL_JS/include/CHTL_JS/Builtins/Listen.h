#pragma once

#include <string>
#include "../Core/Ast.h"

namespace CHTL_JS {
namespace Builtins {

    // Handles parsing and code generation for the built-in listen() function.
    class ListenFunction {
    public:
        // Takes a node representing the object listen is being called on,
        // and a node representing the argument block.
        static std::string GenerateCode(const AstNodePtr& target, const AstNodePtr& args);
    };

}
}
