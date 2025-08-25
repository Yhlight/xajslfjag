#pragma once

#include <string>

namespace CHTL {

    class JSCompiler {
    public:
        // Compiles/Validates a JavaScript source string.
        // Returns the processed JS (for now, it's a passthrough).
        std::string Compile(const std::string& source);
    };

}
