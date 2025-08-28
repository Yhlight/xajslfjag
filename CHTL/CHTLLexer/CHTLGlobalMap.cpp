#include "CHTLGlobalMap.h"
#include <algorithm>

namespace CHTL {

CHTLGlobalMap::CHTLGlobalMap() {
    symbols_.reserve(1000); // 预分配空间
}

void CHTLGlobalMap::addSymbol(const GlobalSymbol& symbol) {
    auto newSymbol = createSymbol(symbol);
    symbols_.push_back(newSymbol);
}

std::shared_ptr<GlobalSymbol> CHTLGlobalMap::findSymbol(const std::string& name, 
                                                        GlobalSymbolType type,
                                                        const std::string& namespace_) {
    for (const auto& symbol : symbols_) {
        if (symbol->name == name && symbol->type == type) {
            if (namespace_.empty() || symbol->namespace_ == namespace_) {
                return symbol;
            }
        }
    }
    return nullptr;
}

std::vector<std::shared_ptr<GlobalSymbol>> CHTLGlobalMap::findSymbolsInNamespace(const std::string& namespace_) {
    std::vector<std::shared_ptr<GlobalSymbol>> result;
    for (const auto& symbol : symbols_) {
        if (symbol->namespace_ == namespace_) {
            result.push_back(symbol);
        }
    }
    return result;
}

bool CHTLGlobalMap::hasSymbol(const std::string& name, 
                              GlobalSymbolType type,
                              const std::string& namespace_) {
    return findSymbol(name, type, namespace_) != nullptr;
}

bool CHTLGlobalMap::removeSymbol(const std::string& name, 
                                 GlobalSymbolType type,
                                 const std::string& namespace_) {
    auto it = std::remove_if(symbols_.begin(), symbols_.end(),
        [&](const std::shared_ptr<GlobalSymbol>& symbol) {
            if (symbol->name == name && symbol->type == type) {
                if (namespace_.empty() || symbol->namespace_ == namespace_) {
                    return true;
                }
            }
            return false;
        });
    
    if (it != symbols_.end()) {
        symbols_.erase(it, symbols_.end());
        return true;
    }
    return false;
}

void CHTLGlobalMap::clear() {
    symbols_.clear();
}

std::shared_ptr<GlobalSymbol> CHTLGlobalMap::createSymbol(const GlobalSymbol& symbol) {
    return std::make_shared<GlobalSymbol>(symbol);
}

} // namespace CHTL