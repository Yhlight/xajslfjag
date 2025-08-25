#include "CJMOD/CJMOD_API.h"
#include <sstream>
#include <stdexcept>
#include <set>

namespace CHTL {

// --- syntaxAnalys Implementation ---
syntaxAnalys(const std::string& pattern, const std::string& ignore) {
    Syntax syntax;
    syntax.OriginalPattern = pattern;
    std::set<char> ignoreSet(ignore.begin(), ignore.end());

    std::string currentToken;
    for (char c : pattern) {
        if (isspace(c) || ignoreSet.count(c)) {
            if (!currentToken.empty()) {
                auto arg = std::make_shared<Arg>();
                arg->Name = currentToken;
                syntax.Args.push_back(arg);
                currentToken.clear();
            }
        } else {
            currentToken += c;
        }
    }
    if (!currentToken.empty()) {
        auto arg = std::make_shared<Arg>();
        arg->Name = currentToken;
        syntax.Args.push_back(arg);
    }
    return syntax;
}

// --- Syntax Class Method Implementations ---
void Syntax::Bind(const std::string& argName, std::function<std::string(const std::string&)> func) {
    bool isPlaceholder = (argName == "$");
    for (auto& arg : Args) {
        if (isPlaceholder) {
            if (arg->Name == "$" && !arg->BoundFunc) { // Find next unbound placeholder
                arg->SetBinding(func);
                return;
            }
        } else {
            if (arg->Name == argName) {
                arg->SetBinding(func);
                return;
            }
        }
    }
    throw std::runtime_error("Argument not found for binding: " + argName);
}

void Syntax::Transform(const std::string& argName, const std::string& jsCode) {
    // Similar logic to Bind for finding the correct Arg
    // ... (implementation omitted for brevity)
}

std::string Syntax::Result() {
    std::string finalCode = OriginalPattern;
    for (const auto& arg : Args) {
        size_t pos = finalCode.find(arg->Name);
        if (pos != std::string::npos) {
            finalCode.replace(pos, arg->Name.length(), arg->TransformedCode);
        }
    }
    return finalCode;
}

// --- generateCode Implementation ---
std::string generateCode(Syntax& syntax) {
    return syntax.Result();
}

}