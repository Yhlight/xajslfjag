#include "CHTL/CHTLCompiler.h"

namespace CHTL {

// This function now has a real implementation.
Config ConvertNodeToConfig(const ConfigNode& node) {
    Config cfg;
    // This is still a simplified conversion. A full implementation would need to
    // parse the string values from the settings map into their correct types (bool, int, etc.)
    // and handle the nested [Name] and [OriginType] blocks properly.

    // For now, we assume a simple key-value mapping for demonstration.
    for (const auto& setting : node.Settings) {
        if (setting.first == "DEBUG_MODE") {
            cfg.DebugMode = (setting.second == "true");
        } else if (setting.first == "DISABLE_NAME_GROUP") {
            cfg.DisableNameGroup = (setting.second == "true");
        }
        // ... and so on for all other direct config properties.
    }

    // TODO: Add logic to parse the nested [Name] and [OriginType] blocks
    // from a more complex AST representation of ConfigNode.

    return cfg;
}

// ... (rest of CHTLCompiler.cpp)

}