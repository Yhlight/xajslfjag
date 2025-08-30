#include "Context.h"

namespace CHTL {

CHTLJSContext::CHTLJSContext() = default;
CHTLJSContext::~CHTLJSContext() = default;

void CHTLJSContext::registerVirtualObject(const std::string& name, const std::string& definition) {
    m_virtualObjects[name] = definition;
}

bool CHTLJSContext::hasVirtualObject(const std::string& name) const {
    return m_virtualObjects.find(name) != m_virtualObjects.end();
}

std::string CHTLJSContext::getVirtualObjectDefinition(const std::string& name) const {
    auto it = m_virtualObjects.find(name);
    if (it != m_virtualObjects.end()) {
        return it->second;
    }
    return "";
}

void CHTLJSContext::registerFunction(const std::string& name, const std::string& signature) {
    m_functions[name] = signature;
}

bool CHTLJSContext::hasFunction(const std::string& name) const {
    return m_functions.find(name) != m_functions.end();
}

}