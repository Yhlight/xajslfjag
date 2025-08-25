#include "CHTL_JS/BuiltinFunctions/DelegateHandler.h"
#include "CHTL_JS/BuiltinFunctions/DelegateRegistry.h"

namespace CHTL_JS {

void DelegateHandler::Process(const DelegateNode& node) {
    // This is a simplification. A real implementation would get the parent selector
    // from the LHS of the -> operator.
    std::string parentSelector = node.ParentSelector->ToString(); 

    DelegateInfo info;
    info.TargetSelector = node.TargetSelector->ToString();
    
    // Extract event map from the AST node
    for (const auto& pair : node.Handlers) {
        info.Events[pair.first] = pair.second->ToString();
    }

    DelegateRegistry::GetInstance().Register(parentSelector, info);
}

}