#include "VirtualObjectManager.h"
#include "../../Error/ErrorReport.h"

namespace CHTLJS {

VirtualObjectManager::VirtualObjectManager() {
    CHTL::ErrorBuilder(CHTL::ErrorLevel::INFO, CHTL::ErrorType::INTERNAL_ERROR)
        .withMessage("VirtualObjectManager initialized")
        .report();
}

VirtualObjectManager::~VirtualObjectManager() = default;

void VirtualObjectManager::registerVirtualObject(const std::string& name, 
                                               std::shared_ptr<VirtualObject> obj) {
    virtualObjects_[name] = obj;
    
    CHTL::ErrorBuilder(CHTL::ErrorLevel::INFO, CHTL::ErrorType::INTERNAL_ERROR)
        .withMessage("Virtual object registered")
        .withDetail("Name: " + name)
        .report();
}

std::shared_ptr<VirtualObject> VirtualObjectManager::getVirtualObject(const std::string& name) {
    auto it = virtualObjects_.find(name);
    if (it != virtualObjects_.end()) {
        return it->second;
    }
    return nullptr;
}

bool VirtualObjectManager::hasVirtualObject(const std::string& name) const {
    return virtualObjects_.find(name) != virtualObjects_.end();
}

void VirtualObjectManager::removeVirtualObject(const std::string& name) {
    virtualObjects_.erase(name);
}

std::vector<std::string> VirtualObjectManager::getAllVirtualObjectNames() const {
    std::vector<std::string> names;
    for (const auto& [name, obj] : virtualObjects_) {
        names.push_back(name);
    }
    return names;
}

void VirtualObjectManager::clearAll() {
    virtualObjects_.clear();
}

} // namespace CHTLJS