#include "GlobalMap.hpp"
#include <iostream>
#include <algorithm>

namespace CHTL {

std::unique_ptr<GlobalMap> GlobalMap::instance = nullptr;

GlobalMap& GlobalMap::getInstance() {
    if (!instance) {
        instance = std::make_unique<GlobalMap>();
    }
    return *instance;
}

void GlobalMap::reset() {
    instance.reset();
}

void GlobalMap::setConfiguration(const Configuration& config) {
    globalConfig = config;
}

const GlobalMap::Configuration& GlobalMap::getConfiguration() const {
    return globalConfig;
}

void GlobalMap::mergeConfiguration(const Configuration& config) {
    // 合并配置，新配置覆盖旧配置
    globalConfig.INDEX_INITIAL_COUNT = config.INDEX_INITIAL_COUNT;
    globalConfig.DISABLE_NAME_GROUP = config.DISABLE_NAME_GROUP;
    globalConfig.DISABLE_CUSTOM_ORIGIN_TYPE = config.DISABLE_CUSTOM_ORIGIN_TYPE;
    globalConfig.DEBUG_MODE = config.DEBUG_MODE;
    globalConfig.DISABLE_STYLE_AUTO_ADD_CLASS = config.DISABLE_STYLE_AUTO_ADD_CLASS;
    globalConfig.DISABLE_STYLE_AUTO_ADD_ID = config.DISABLE_STYLE_AUTO_ADD_ID;
    globalConfig.DISABLE_SCRIPT_AUTO_ADD_CLASS = config.DISABLE_SCRIPT_AUTO_ADD_CLASS;
    globalConfig.DISABLE_SCRIPT_AUTO_ADD_ID = config.DISABLE_SCRIPT_AUTO_ADD_ID;
    globalConfig.DISABLE_DEFAULT_NAMESPACE = config.DISABLE_DEFAULT_NAMESPACE;
    
    // 合并Name配置
    for (const auto& pair : config.nameConfig) {
        globalConfig.nameConfig[pair.first] = pair.second;
    }
    
    // 合并原始类型配置
    for (const auto& pair : config.originTypes) {
        globalConfig.originTypes[pair.first] = pair.second;
    }
}

void GlobalMap::registerStyleTemplate(const std::string& name, const TemplateInfo& info) {
    std::string fullName = resolveFullName(name);
    styleTemplates[fullName] = info;
}

void GlobalMap::registerElementTemplate(const std::string& name, const TemplateInfo& info) {
    std::string fullName = resolveFullName(name);
    elementTemplates[fullName] = info;
}

void GlobalMap::registerVarTemplate(const std::string& name, const TemplateInfo& info) {
    std::string fullName = resolveFullName(name);
    varTemplates[fullName] = info;
}

GlobalMap::TemplateInfo* GlobalMap::getStyleTemplate(const std::string& name) {
    auto it = styleTemplates.find(name);
    return (it != styleTemplates.end()) ? &it->second : nullptr;
}

GlobalMap::TemplateInfo* GlobalMap::getElementTemplate(const std::string& name) {
    auto it = elementTemplates.find(name);
    return (it != elementTemplates.end()) ? &it->second : nullptr;
}

GlobalMap::TemplateInfo* GlobalMap::getVarTemplate(const std::string& name) {
    auto it = varTemplates.find(name);
    return (it != varTemplates.end()) ? &it->second : nullptr;
}

void GlobalMap::registerStyleCustom(const std::string& name, const CustomInfo& info) {
    std::string fullName = resolveFullName(name);
    styleCustoms[fullName] = info;
}

void GlobalMap::registerElementCustom(const std::string& name, const CustomInfo& info) {
    std::string fullName = resolveFullName(name);
    elementCustoms[fullName] = info;
}

void GlobalMap::registerVarCustom(const std::string& name, const CustomInfo& info) {
    std::string fullName = resolveFullName(name);
    varCustoms[fullName] = info;
}

GlobalMap::CustomInfo* GlobalMap::getStyleCustom(const std::string& name) {
    auto it = styleCustoms.find(name);
    return (it != styleCustoms.end()) ? &it->second : nullptr;
}

GlobalMap::CustomInfo* GlobalMap::getElementCustom(const std::string& name) {
    auto it = elementCustoms.find(name);
    return (it != elementCustoms.end()) ? &it->second : nullptr;
}

GlobalMap::CustomInfo* GlobalMap::getVarCustom(const std::string& name) {
    auto it = varCustoms.find(name);
    return (it != varCustoms.end()) ? &it->second : nullptr;
}

void GlobalMap::registerOrigin(const std::string& name, const OriginInfo& info) {
    std::string fullName = resolveFullName(name);
    origins[fullName] = info;
}

GlobalMap::OriginInfo* GlobalMap::getOrigin(const std::string& name) {
    auto it = origins.find(name);
    return (it != origins.end()) ? &it->second : nullptr;
}

void GlobalMap::registerImport(const std::string& name, const ImportInfo& info) {
    imports[name] = info;
}

GlobalMap::ImportInfo* GlobalMap::getImport(const std::string& name) {
    auto it = imports.find(name);
    return (it != imports.end()) ? &it->second : nullptr;
}

bool GlobalMap::isFileLoaded(const std::string& filePath) {
    return loadedFiles.find(filePath) != loadedFiles.end();
}

void GlobalMap::markFileLoaded(const std::string& filePath) {
    loadedFiles.insert(filePath);
}

void GlobalMap::enterNamespace(const std::string& name) {
    if (currentNamespace.empty()) {
        currentNamespace = name;
    } else {
        currentNamespace += "." + name;
    }
}

void GlobalMap::exitNamespace() {
    size_t lastDot = currentNamespace.find_last_of('.');
    if (lastDot != std::string::npos) {
        currentNamespace = currentNamespace.substr(0, lastDot);
    } else {
        currentNamespace.clear();
    }
}

std::string GlobalMap::getCurrentNamespace() const {
    return currentNamespace;
}

void GlobalMap::registerNamespace(const std::string& name, const NamespaceInfo& info) {
    namespaces[name] = info;
}

GlobalMap::NamespaceInfo* GlobalMap::getNamespace(const std::string& name) {
    auto it = namespaces.find(name);
    return (it != namespaces.end()) ? &it->second : nullptr;
}

std::string GlobalMap::resolveFullName(const std::string& name) const {
    if (currentNamespace.empty()) {
        return name;
    }
    return currentNamespace + "." + name;
}

GlobalMap::TemplateInfo* GlobalMap::findTemplate(const std::string& type, const std::string& name) {
    // 首先在当前命名空间查找
    std::string fullName = resolveFullName(name);
    
    if (type == "Style") {
        auto it = styleTemplates.find(fullName);
        if (it != styleTemplates.end()) return &it->second;
        // 如果在当前命名空间找不到，尝试全局查找
        it = styleTemplates.find(name);
        return (it != styleTemplates.end()) ? &it->second : nullptr;
    } else if (type == "Element") {
        auto it = elementTemplates.find(fullName);
        if (it != elementTemplates.end()) return &it->second;
        it = elementTemplates.find(name);
        return (it != elementTemplates.end()) ? &it->second : nullptr;
    } else if (type == "Var") {
        auto it = varTemplates.find(fullName);
        if (it != varTemplates.end()) return &it->second;
        it = varTemplates.find(name);
        return (it != varTemplates.end()) ? &it->second : nullptr;
    }
    
    return nullptr;
}

GlobalMap::CustomInfo* GlobalMap::findCustom(const std::string& type, const std::string& name) {
    // 首先在当前命名空间查找
    std::string fullName = resolveFullName(name);
    
    if (type == "Style") {
        auto it = styleCustoms.find(fullName);
        if (it != styleCustoms.end()) return &it->second;
        it = styleCustoms.find(name);
        return (it != styleCustoms.end()) ? &it->second : nullptr;
    } else if (type == "Element") {
        auto it = elementCustoms.find(fullName);
        if (it != elementCustoms.end()) return &it->second;
        it = elementCustoms.find(name);
        return (it != elementCustoms.end()) ? &it->second : nullptr;
    } else if (type == "Var") {
        auto it = varCustoms.find(fullName);
        if (it != varCustoms.end()) return &it->second;
        it = varCustoms.find(name);
        return (it != varCustoms.end()) ? &it->second : nullptr;
    }
    
    return nullptr;
}

GlobalMap::OriginInfo* GlobalMap::findOrigin(const std::string& name) {
    std::string fullName = resolveFullName(name);
    auto it = origins.find(fullName);
    if (it != origins.end()) return &it->second;
    
    it = origins.find(name);
    return (it != origins.end()) ? &it->second : nullptr;
}

bool GlobalMap::hasConflict(const std::string& type, const std::string& name) const {
    // 检查同名冲突
    return !getConflicts(type, name).empty();
}

std::vector<std::string> GlobalMap::getConflicts(const std::string& type, const std::string& name) const {
    std::vector<std::string> conflicts;
    
    if (type == "Style") {
        if (styleTemplates.find(name) != styleTemplates.end()) {
            conflicts.push_back("Template");
        }
        if (styleCustoms.find(name) != styleCustoms.end()) {
            conflicts.push_back("Custom");
        }
    }
    // 其他类型的冲突检测...
    
    return conflicts;
}

void GlobalMap::dumpState() const {
    if (globalConfig.DEBUG_MODE) {
        std::cout << "=== GlobalMap State ===\n";
        std::cout << "Current Namespace: " << currentNamespace << "\n";
        std::cout << "Style Templates: " << styleTemplates.size() << "\n";
        std::cout << "Element Templates: " << elementTemplates.size() << "\n";
        std::cout << "Var Templates: " << varTemplates.size() << "\n";
        std::cout << "Style Customs: " << styleCustoms.size() << "\n";
        std::cout << "Element Customs: " << elementCustoms.size() << "\n";
        std::cout << "Var Customs: " << varCustoms.size() << "\n";
        std::cout << "Origins: " << origins.size() << "\n";
        std::cout << "Imports: " << imports.size() << "\n";
        std::cout << "Namespaces: " << namespaces.size() << "\n";
        std::cout << "========================\n";
    }
}

void GlobalMap::clear() {
    styleTemplates.clear();
    elementTemplates.clear();
    varTemplates.clear();
    styleCustoms.clear();
    elementCustoms.clear();
    varCustoms.clear();
    origins.clear();
    imports.clear();
    namespaces.clear();
    loadedFiles.clear();
    currentNamespace.clear();
}

}