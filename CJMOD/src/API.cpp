#include "CJMOD/API.h"
#include "CHTL_JS/Core/ViewRegistry.h"

namespace CHTL {

void bindVir(const std::string& functionName) {
    CHTL_JS::ViewRegistry::GetInstance().RegisterVirCompatible(functionName);
}

}
