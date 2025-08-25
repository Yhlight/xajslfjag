#pragma once

#include "CHTL/Core/Token.h"
#include <vector>
#include <string>

namespace CHTL {
namespace Test {

    class TokenPrint {
    public:
        // Prints a vector of tokens to the console.
        static void Print(const std::vector<Token>& tokens);

        // Returns a string representation of a single token type.
        static const char* GetTokenTypeName(TokenType type);
    };

}
}
