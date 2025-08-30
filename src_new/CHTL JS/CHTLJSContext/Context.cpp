#include "Context.h"
#include <sstream>

namespace CHTLJS {

std::string SymbolInfo::toString() const {
    std::ostringstream oss;
    oss << "Symbol(" << static_cast<int>(type) << ", " << name << ", " << value << ")";
    return oss.str();
}

CHTLJSContext::CHTLJSContext() {
}

void CHTLJSContext::addSymbol(const SymbolInfo& symbol) {
    m_symbols[symbol.name] = symbol;
}

SymbolInfo* CHTLJSContext::findSymbol(const std::string& name) {
    auto it = m_symbols.find(name);
    if (it != m_symbols.end()) {
        return &it->second;
    }
    return nullptr;
}

bool CHTLJSContext::hasSymbol(const std::string& name) const {
    return m_symbols.find(name) != m_symbols.end();
}

void CHTLJSContext::registerEnhancedSelector(const std::string& selector) {
    m_enhancedSelectors.insert(selector);
}

std::vector<std::string> CHTLJSContext::getEnhancedSelectors() const {
    std::vector<std::string> selectors;
    for (const auto& selector : m_enhancedSelectors) {
        selectors.push_back(selector);
    }
    return selectors;
}

void CHTLJSContext::registerEventListener(const std::string& event, const std::string& selector) {
    std::string key = selector + ":" + event;
    m_eventListeners[key] = selector + " -> " + event;
}

void CHTLJSContext::registerEventDelegate(const std::string& event, const std::string& selector) {
    std::string key = selector + ":" + event;
    m_eventDelegates[key] = selector + " delegate " + event;
}

void CHTLJSContext::registerVirtualObject(const std::string& name, const std::string& definition) {
    m_virtualObjects[name] = definition;
}

std::string CHTLJSContext::getVirtualObject(const std::string& name) const {
    auto it = m_virtualObjects.find(name);
    if (it != m_virtualObjects.end()) {
        return it->second;
    }
    return "";
}

void CHTLJSContext::reset() {
    m_symbols.clear();
    m_enhancedSelectors.clear();
    m_eventListeners.clear();
    m_eventDelegates.clear();
    m_virtualObjects.clear();
}

std::string CHTLJSContext::getContextStats() const {
    std::ostringstream oss;
    oss << "CHTL JS Context Statistics:\n";
    oss << "  Symbols: " << m_symbols.size() << "\n";
    oss << "  Enhanced Selectors: " << m_enhancedSelectors.size() << "\n";
    oss << "  Event Listeners: " << m_eventListeners.size() << "\n";
    oss << "  Event Delegates: " << m_eventDelegates.size() << "\n";
    oss << "  Virtual Objects: " << m_virtualObjects.size() << "\n";
    return oss.str();
}

} // namespace CHTLJS