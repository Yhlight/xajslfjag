#pragma once

#include <string>
#include <vector>
#include <map>
#include <variant>
#include <optional>
#include <stdexcept>

namespace CHTL {

// Represents any value that can be passed as an argument in CHTL JS
using ChtlJsValue = std::variant<std::string, double, bool, std::nullptr_t>;

class CHTLJSFunction {
public:
    // The CHTL JS parser will populate this map with the parsed arguments
    std::map<std::string, ChtlJsValue> Arguments;

    template<typename T>
    T GetArgument(const std::string& key) {
        auto it = Arguments.find(key);
        if (it == Arguments.end()) {
            throw std::runtime_error("Missing required argument: " + key);
        }
        if (!std::holds_alternative<T>(it->second)) {
            throw std::runtime_error("Invalid type for argument: " + key);
        }
        return std::get<T>(it->second);
    }

    template<typename T>
    T GetArgument(const std::string& key, T defaultValue) {
        auto it = Arguments.find(key);
        if (it == Arguments.end()) {
            return defaultValue;
        }
        if (!std::holds_alternative<T>(it->second)) {
            // Or maybe just return defaultValue?
            throw std::runtime_error("Invalid type for argument: " + key);
        }
        return std::get<T>(it->second);
    }

    // Method to be implemented by the CJMOD developer
    virtual std::string generate() = 0;
};

}
