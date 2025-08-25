#pragma once

#include <string>

namespace CHTL {

    class CJMODPackager {
    public:
        static bool Pack(const std::string& sourceDir, const std::string& destinationFile);
    };

}
