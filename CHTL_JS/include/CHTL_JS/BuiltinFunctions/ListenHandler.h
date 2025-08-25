#pragma once

#include "CHTL_JS/Core/Ast.h"
#include <string>

namespace CHTL_JS {

    // Handles the generation of code for the built-in listen() function.
    class ListenHandler {
    public:
        static std::string Generate(const ListenNode& node);
    };

}
