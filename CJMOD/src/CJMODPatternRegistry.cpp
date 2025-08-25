#include "../CJMODPatternRegistry.h"

namespace CHTL {

void CJMODPatternRegistry::Register(const std::string& triggerKeyword, const std::string& pattern, CJMODHandlerFn handler) {
    m_Patterns[triggerKeyword] = {pattern, handler};
}

const PatternInfo* CJMODPatternRegistry::GetPatternInfo(const std::string& triggerKeyword) const {
    auto it = m_Patterns.find(triggerKeyword);
    if (it != m_Patterns.end()) {
        return &it->second;
    }
    return nullptr;
}

}
