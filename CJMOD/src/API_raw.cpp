#include "CJMOD/API_raw.h"
#include "../CJMODPatternRegistry.h"

namespace CHTL {

void RegisterCJMOD_Raw(const std::string& triggerKeyword, const std::string& pattern, CJMODHandlerFn handler) {
    CJMODPatternRegistry::GetInstance().Register(triggerKeyword, pattern, handler);
}

}
