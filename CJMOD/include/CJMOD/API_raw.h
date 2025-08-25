#pragma once

#include <string>
#include <vector>
#include <functional>
#include <map>

// This is the FINAL, correct, low-level API for CJMOD development.

namespace CHTL {

    // Contains source code snippets captured by the scanner that correspond to
    // placeholders (e.g., '$source') in a pattern.
    using CapturedArgs = std::map<std::string, std::string>;

    // The handler function provided by the developer.
    // It receives the captured arguments and must return the final JS code string.
    using CJMODHandlerFn = std::function<std::string(const CapturedArgs&)>;

    // Registers a raw, pattern-based CJMOD function.
    // This is the core of the raw API.
    void RegisterCJMOD_Raw(
        const std::string& triggerKeyword, // The keyword that starts the pattern matching
        const std::string& pattern,      // The full pattern to match, e.g., "query from $source ..."
        CJMODHandlerFn handler
    );
} 

// The public-facing macro for developers to register their raw extensions.
#define CJMOD_REGISTER_RAW(TriggerKeyword, Pattern, HandlerLambda) 
    extern "C" void CHTL_CJMOD_##TriggerKeyword() { 
        CHTL::RegisterCJMOD_Raw(#TriggerKeyword, Pattern, HandlerLambda); 
    }