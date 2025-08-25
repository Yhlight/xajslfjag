#pragma once

#include <string>
#include <map>

namespace CHTL {

    class CMODLoader {
    public:
        // Loads a .cmod file into an in-memory map of <filepath, content>
        static std::map<std::string, std::string> Load(const std::string& cmodFile);
    };

}
