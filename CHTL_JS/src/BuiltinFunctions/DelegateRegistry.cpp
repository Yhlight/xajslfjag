#include "CHTL_JS/BuiltinFunctions/DelegateRegistry.h"
#include <sstream>

namespace CHTL_JS {

DelegateRegistry& DelegateRegistry::GetInstance() {
    static DelegateRegistry instance;
    return instance;
}

void DelegateRegistry::Register(const std::string& parentSelector, const DelegateInfo& info) {
    m_Delegations[parentSelector].push_back(info);
}

std::string DelegateRegistry::GenerateFinalCode() {
    std::stringstream ss;
    for (const auto& pair : m_Delegations) {
        const std::string& parentSelector = pair.first;
        const auto& infos = pair.second;
        ss << "document.querySelector('" << parentSelector << "').addEventListener('click', (e) => {\n"; // Simplified to click
        for (const auto& info : infos) {
            ss << "  if (e.target.matches('" << info.TargetSelector << "')) {\n";
            // This is a simplification. A real implementation would iterate info.Events
            ss << "    (" << info.Events.at("click") << ")(e);\n"; 
            ss << "  }\n";
        }
        ss << "});\n";
    }
    return ss.str();
}

} // namespace CHTL_JS
