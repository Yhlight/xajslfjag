#pragma once

#include <string>

namespace CHTL {

    class CSSCompiler {
    public:
        // Compiles/Validates a CSS source string.
        // Returns the processed CSS (for now, it's a passthrough).
        std::string Compile(const std::string& source);
    };

}
