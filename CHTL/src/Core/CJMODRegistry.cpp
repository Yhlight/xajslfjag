#include "CHTL/Core/CJMODRegistry.h"
#include "UnifiedScanner/CHTLUnifiedScanner.h"

namespace CHTL {

void CJMODRegistry::Register(const std::string& moduleName, const std::string& functionName, Factory factory) {
    m_Modules[moduleName][functionName] = factory;
}

void CJMODRegistry::Activate(const std::string& moduleName, CHTLUnifiedScanner* scanner) {
    auto module_it = m_Modules.find(moduleName);
    if (module_it != m_Modules.end()) {
        for (const auto& pair : module_it->second) {
            auto cjmod_function = pair.second(); // Call the factory to create an instance
            scanner->RegisterFunction(cjmod_function);
        }
    }
}

} // namespace CHTL
