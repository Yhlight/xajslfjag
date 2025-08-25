#pragma once

#include "CHTL/Core/Config.h"

namespace CHTL {
namespace Test {

    class ConfigurationInfo {
    public:
        // Prints all settings from a Config object to the console.
        static void Print(const Config& config);
    };

}
}
