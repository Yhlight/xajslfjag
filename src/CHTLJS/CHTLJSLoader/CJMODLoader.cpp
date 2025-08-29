#include "CJMODLoader.h"
#include "../CJMODSystem/CJMODPackager.h"
#include "../../Error/ErrorReport.h"
#include <memory>

namespace CHTLJS {

CJMODLoader::CJMODLoader() {
    packager_ = std::make_unique<CJMODPackager>();
}

CJMODLoader::~CJMODLoader() = default;

bool CJMODLoader::loadExtension(const std::string& path) {
    try {
        auto cjmod = packager_->loadFromDirectory(path);
        if (cjmod) {
            extensions_[cjmod->getName()] = std::move(cjmod);
            
            ErrorBuilder(ErrorLevel::INFO, ErrorType::INTERNAL_ERROR)
                .withMessage("CJMOD extension loaded successfully")
                .withDetail("Extension: " + path)
                .report();
                
            return true;
        }
    } catch (const std::exception& e) {
        ErrorBuilder(ErrorLevel::ERROR, ErrorType::FILE_NOT_FOUND)
            .withMessage("Failed to load CJMOD extension")
            .withDetail(e.what())
            .report();
    }
    
    return false;
}

bool CJMODLoader::hasExtension(const std::string& name) const {
    return extensions_.find(name) != extensions_.end();
}

std::shared_ptr<CJMOD> CJMODLoader::getExtension(const std::string& name) {
    auto it = extensions_.find(name);
    if (it != extensions_.end()) {
        return it->second;
    }
    return nullptr;
}

std::vector<std::string> CJMODLoader::getLoadedExtensions() const {
    std::vector<std::string> names;
    for (const auto& [name, ext] : extensions_) {
        names.push_back(name);
    }
    return names;
}

void CJMODLoader::unloadExtension(const std::string& name) {
    extensions_.erase(name);
    
    ErrorBuilder(ErrorLevel::INFO, ErrorType::INTERNAL_ERROR)
        .withMessage("CJMOD extension unloaded")
        .withDetail("Extension: " + name)
        .report();
}

void CJMODLoader::unloadAll() {
    extensions_.clear();
}

} // namespace CHTLJS