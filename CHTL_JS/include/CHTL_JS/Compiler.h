#pragma once

#include <string>
#include "Core/Config.h"

namespace CHTL_JS {

    class Compiler {
    public:
        Compiler(const CHTL::Config& config);

        // Compiles a CHTL JS source string into a pure JS string.
        std::string Compile(const std::string& source);
    private:
        const CHTL::Config& m_Config;
    };

}
