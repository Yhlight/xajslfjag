#pragma once

#include <string>
#include <vector>
#include <map>
#include <functional>
#include <memory>

namespace CHTL {

    using ChtlJsValue = std::string;
    using ChtlJsArgMap = std::map<std::string, ChtlJsValue>;
    using CJMODLogicFn = std::function<std::string(const ChtlJsArgMap&)>;

    // A convenience class that encapsulates the standard process for creating
    // a vir-compatible CHTL JS function.
    class CHTLJSFunction {
    public:
        CHTLJSFunction(const std::string& name, const std::string& pattern);

        // Binds a C++ handler to a named argument in the pattern.
        void Bind(const std::string& argName, CJMODLogicFn handler);

        // Generates the final JS code based on the provided arguments.
        std::string Generate(const ChtlJsArgMap& args);

    private:
        std::string m_Name;
        std::vector<std::string> m_ExpectedArgs;
        std::map<std::string, CJMODLogicFn> m_BoundHandlers;
    };

}
