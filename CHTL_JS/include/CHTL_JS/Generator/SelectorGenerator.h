#pragma once

#include <string>
#include <memory>
#include "CHTL_JS/Core/Ast.h"

namespace CHTL_JS {

    class SelectorGenerator {
    public:
        // Generate JavaScript code for CHTL JS selectors
        static std::string Generate(const SelectorNode& node);
        
    private:
        // Determine selector type and generate appropriate code
        static std::string GenerateByType(const std::string& selector, std::optional<int> index);
        
        // Check if selector is a tag name
        static bool IsTagSelector(const std::string& selector);
    };

}