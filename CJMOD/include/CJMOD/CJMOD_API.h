#pragma once

#include <string>
#include <functional>
#include <vector>
#include <map>
#include <memory>

namespace CHTL {

    // Represents a parsed argument value from a CHTL JS function call
    using ChtlJsValue = std::string; // Can be extended to a variant later

    // A map representing the parsed key-value arguments from a {...} block
    using ChtlJsArgMap = std::map<std::string, ChtlJsValue>;

    // The function provided by a CJMOD developer to implement the logic.
    // It receives the parsed arguments and should return the final JS code.
    using CJMODLogicFn = std::function<std::string(const ChtlJsArgMap&)>;

    // This class is a concrete tool for parsing CHTL JS-style function arguments.
    // It is used internally by the compiler when a CJMOD function is called.
    class CHTLJSFunction {
    public:
        std::string Name;
        CJMODLogicFn LogicHandler;

        CHTLJSFunction(const std::string& name, CJMODLogicFn handler)
            : Name(name), LogicHandler(handler) {}

        // Parses a {...} block from the source and returns the map.
        ChtlJsArgMap Parse(const std::string& source_view);

        // Executes the developer's logic with the parsed map.
        std::string Execute(const ChtlJsArgMap& args) {
            return LogicHandler(args);
        }
    };

    // The internal function used to register an extension.
    void RegisterCJMOD(const std::string& moduleName, const std::string& functionName, CJMODLogicFn fn);

}

// The public-facing macro for developers
#define CJMOD_REGISTER(ModuleName, FunctionName, Lambda) \
    extern "C" void CHTL_CJMOD_##ModuleName##_##FunctionName() { \
        CHTL::RegisterCJMOD(#ModuleName, #FunctionName, Lambda); \
    }