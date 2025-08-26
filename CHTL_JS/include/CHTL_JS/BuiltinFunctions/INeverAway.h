#pragma once

#include <string>
#include <memory>
#include <map>
#include "CHTL_JS/Core/Ast.h"

namespace CHTL_JS {

    // iNeverAway creates a set of marker functions with state-based overloading
    class INeverAwayHandler {
    public:
        static std::string Generate(const struct INeverAwayNode& node);
        
    private:
        struct FunctionSignature {
            std::string Name;
            std::optional<std::string> State; // For Void<A>, Void<B> etc.
            std::vector<std::string> Parameters;
            std::string Body;
        };
        
        static std::vector<FunctionSignature> ParseFunctions(const INeverAwayNode& node);
        static std::string GenerateStateDispatcher(const std::string& baseName, 
                                                  const std::vector<FunctionSignature>& overloads);
    };

}