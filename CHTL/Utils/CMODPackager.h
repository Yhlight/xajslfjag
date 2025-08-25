#pragma once

#include <string>

namespace CHTL {

    class CMODPackager {
    public:
        // Packs a directory into a .cmod file.
        // Returns true on success, false on failure.
        static bool Pack(const std::string& sourceDir, const std::string& destinationFile);
    };

}
