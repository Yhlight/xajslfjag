#include "Builtins/Listen.h"
#include <sstream>

namespace CHTL_JS {
namespace Builtins {

std::string ListenFunction::GenerateCode(const AstNodePtr& target, const AstNodePtr& args) {
    auto objectLiteral = std::dynamic_pointer_cast<ObjectLiteralNode>(args);
    if (!objectLiteral) return ""; // Or throw error

    std::string targetString = GenerateStringFromNode(target);
    std::stringstream ss;

    // Iterate through properties by key, making it order-insensitive
    for (const auto& prop : objectLiteral->Properties) {
        std::string eventName = prop.Key;
        std::string handlerString = GenerateStringFromNode(prop.Value);
        ss << targetString << ".addEventListener('" << eventName << "', " << handlerString << ");\n";
    }

    return ss.str();
}

} // namespace Builtins
} // namespace CHTL_JS