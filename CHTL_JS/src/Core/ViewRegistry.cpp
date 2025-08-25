#include "CHTL_JS/Core/ViewRegistry.h"

namespace CHTL_JS {

void ViewRegistry::RegisterView(const std::string& viewName, View view) {
    m_Views[viewName] = std::make_shared<View>(std::move(view));
}

std::shared_ptr<View> ViewRegistry::GetView(const std::string& viewName) {
    auto it = m_Views.find(viewName);
    if (it != m_Views.end()) {
        return it->second;
    }
    return nullptr;
}

}
