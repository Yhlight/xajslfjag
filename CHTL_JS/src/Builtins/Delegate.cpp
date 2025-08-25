#include "Builtins/Delegate.h"
#include "Core/EventDelegateRegistry.h"

namespace CHTL_JS {
namespace Builtins {

void DelegateFunction::Process(const AstNodePtr& target, const AstNodePtr& args) {
    auto argsObject = std::dynamic_pointer_cast<ObjectLiteralNode>(args);
    if (!argsObject) return;

    std::string parentSelector = GenerateStringFromNode(target);
    std::string childSelector = "";

    // Safely find the 'target' property
    for (const auto& prop : argsObject->Properties) {
        if (prop.Key == "target") {
            childSelector = GenerateStringFromNode(prop.Value);
            break;
        }
    }
    if (childSelector.empty()) { /* Error */ return; }

    // Process other properties as events
    for (const auto& prop : argsObject->Properties) {
        if (prop.Key != "target") {
            std::string eventType = prop.Key;
            std::string handlerCode = GenerateStringFromNode(prop.Value);
            EventDelegateRegistry::GetInstance().Register(parentSelector, {childSelector, eventType, handlerCode});
        }
    }
}

}
}
