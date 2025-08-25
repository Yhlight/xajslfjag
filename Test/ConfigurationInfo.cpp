#include "ConfigurationInfo.h"
#include <iostream>

namespace CHTL {
namespace Test {

void ConfigurationInfo::Print(const Config& config) {
    std::cout << "--- CHTL Configuration --- B" << std::endl;
    std::cout << "Debug Mode: " << (config.DebugMode ? "true" : "false") << std::endl;
    // ... print all other boolean flags

    std::cout << "\n--- Keyword Names ---" << std::endl;
    for (const auto& pair : config.Names) {
        // Need a way to convert ConfigurableKeyword enum to string here
        std::cout << "Keyword " << static_cast<int>(pair.first) << ": ";
        for (const auto& alias : pair.second) {
            std::cout << "'" << alias << "' ";
        }
        std::cout << std::endl;
    }

    std::cout << "\n--- Custom Origin Types ---" << std::endl;
    for (const auto& pair : config.OriginTypes) {
        std::cout << pair.first << " -> " << pair.second << std::endl;
    }
    std::cout << "--------------------------" << std::endl;
}

} // namespace Test
} // namespace CHTL
