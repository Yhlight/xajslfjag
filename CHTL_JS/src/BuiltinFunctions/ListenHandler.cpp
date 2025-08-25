#include "CHTL_JS/BuiltinFunctions/ListenHandler.h"
#include <sstream>

namespace CHTL_JS {

std::string ListenHandler::Generate(const ListenNode& node) {
    std::stringstream ss;
    // The target of the listen call is handled by the generator visiting the LHS of the -> operator.
    // This handler just generates the .listen({...}) part.
    ss << ".addEventListener({\n";
    bool first = true;
    for (const auto& pair : node.Handlers) {
        if (!first) {
            ss << ",\n";
        }
        ss << "  '" << pair.first << "': " << pair.second->ToString(); // Assuming handler AST node can be converted to string
        first = false;
    }
    ss << "\n});";
    return ss.str();
}

} // namespace CHTL_JS
