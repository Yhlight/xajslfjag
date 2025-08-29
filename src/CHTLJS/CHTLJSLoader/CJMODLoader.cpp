#include "CJMODLoader.h"
#include "../../Error/ErrorReport.h"

namespace CHTLJS {

CJMODLoader::CJMODLoader() {
    ErrorBuilder(ErrorLevel::INFO, ErrorType::INTERNAL_ERROR)
        .withMessage("CJMODLoader initialized")
        .report();
}

CJMODLoader::~CJMODLoader() = default;

bool CJMODLoader::loadExtension(const std::string& path) {
    // TODO: 实现扩展加载
    (void)path;
    return false;
}

bool CJMODLoader::unloadExtension(const std::string& name) {
    auto it = extensions_.find(name);
    if (it != extensions_.end()) {
        extensions_.erase(it);
        return true;
    }
    return false;
}

std::vector<std::string> CJMODLoader::getLoadedExtensions() const {
    std::vector<std::string> names;
    for (const auto& [name, ext] : extensions_) {
        names.push_back(name);
    }
    return names;
}

bool CJMODLoader::isExtensionLoaded(const std::string& name) const {
    return extensions_.find(name) != extensions_.end();
}

std::shared_ptr<CJMODExtension> CJMODLoader::getExtensionInfo(const std::string& name) const {
    auto it = extensions_.find(name);
    if (it != extensions_.end()) {
        return it->second;
    }
    return nullptr;
}

bool CJMODLoader::validateExtension(const CJMODExtension& ext) {
    if (ext.name.empty() || ext.version.empty() || ext.entryPoint.empty()) {
        return false;
    }
    return true;
}

std::shared_ptr<CJMODExtension> CJMODLoader::parseManifest(const std::string& manifestPath) {
    // TODO: 实现清单解析
    (void)manifestPath;
    return nullptr;
}

} // namespace CHTLJS