#pragma once

#include <string>
#include <memory>
#include "CHTL_JS/Core/Ast.h"

namespace CHTL_JS {

    class AnimateHandler {
    public:
        // Generate JavaScript code for animate() function
        static std::string Generate(const AnimateNode& node);
        
    private:
        static std::string GenerateTargetArray(const AnimateNode& node);
        static std::string GenerateCSSProperties(const std::map<std::string, std::string>& props);
        static std::string GenerateKeyframes(const AnimateNode& node);
    };

}