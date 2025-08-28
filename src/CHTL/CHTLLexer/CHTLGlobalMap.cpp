#include "CHTLGlobalMap.h"
#include <iostream>
#include <algorithm>

namespace CHTL {

CHTLGlobalMap::CHTLGlobalMap() {
    // 初始化默认配置
    auto defaultConfig = std::make_shared<CHTLConfiguration>();
    defaultConfig->settings["INDEX_INITIAL_COUNT"] = "0";
    defaultConfig->settings["DISABLE_NAME_GROUP"] = "false";
    defaultConfig->settings["DISABLE_CUSTOM_ORIGIN_TYPE"] = "false";
    defaultConfig->settings["DEBUG_MODE"] = "false";
    defaultConfig->settings["DISABLE_DEFAULT_NAMESPACE"] = "false";
    defaultConfig->settings["DISABLE_STYLE_AUTO_ADD_CLASS"] = "false";
    defaultConfig->settings["DISABLE_STYLE_AUTO_ADD_ID"] = "false";
    defaultConfig->settings["DISABLE_SCRIPT_AUTO_ADD_CLASS"] = "true";
    defaultConfig->settings["DISABLE_SCRIPT_AUTO_ADD_ID"] = "true";
    
    configurations.push_back(defaultConfig);
    activeConfiguration = defaultConfig;
    
    // 设置默认模块搜索路径
    officialModulePath = "./module";
    moduleSearchPaths.push_back("./module");
    moduleSearchPaths.push_back(".");
}

// 模板管理方法
void CHTLGlobalMap::addStyleTemplate(const std::string& name, std::shared_ptr<CHTLTemplate> template_) {
    styleTemplates[name] = template_;
}

void CHTLGlobalMap::addElementTemplate(const std::string& name, std::shared_ptr<CHTLTemplate> template_) {
    elementTemplates[name] = template_;
}

void CHTLGlobalMap::addVarTemplate(const std::string& name, std::shared_ptr<CHTLTemplate> template_) {
    varTemplates[name] = template_;
}

std::shared_ptr<CHTLTemplate> CHTLGlobalMap::getStyleTemplate(const std::string& name) {
    auto it = styleTemplates.find(name);
    return it != styleTemplates.end() ? it->second : nullptr;
}

std::shared_ptr<CHTLTemplate> CHTLGlobalMap::getElementTemplate(const std::string& name) {
    auto it = elementTemplates.find(name);
    return it != elementTemplates.end() ? it->second : nullptr;
}

std::shared_ptr<CHTLTemplate> CHTLGlobalMap::getVarTemplate(const std::string& name) {
    auto it = varTemplates.find(name);
    return it != varTemplates.end() ? it->second : nullptr;
}

bool CHTLGlobalMap::hasStyleTemplate(const std::string& name) const {
    return styleTemplates.find(name) != styleTemplates.end();
}

bool CHTLGlobalMap::hasElementTemplate(const std::string& name) const {
    return elementTemplates.find(name) != elementTemplates.end();
}

bool CHTLGlobalMap::hasVarTemplate(const std::string& name) const {
    return varTemplates.find(name) != varTemplates.end();
}

// 自定义管理方法
void CHTLGlobalMap::addStyleCustom(const std::string& name, std::shared_ptr<CHTLCustom> custom) {
    styleCustoms[name] = custom;
}

void CHTLGlobalMap::addElementCustom(const std::string& name, std::shared_ptr<CHTLCustom> custom) {
    elementCustoms[name] = custom;
}

void CHTLGlobalMap::addVarCustom(const std::string& name, std::shared_ptr<CHTLCustom> custom) {
    varCustoms[name] = custom;
}

std::shared_ptr<CHTLCustom> CHTLGlobalMap::getStyleCustom(const std::string& name) {
    auto it = styleCustoms.find(name);
    return it != styleCustoms.end() ? it->second : nullptr;
}

std::shared_ptr<CHTLCustom> CHTLGlobalMap::getElementCustom(const std::string& name) {
    auto it = elementCustoms.find(name);
    return it != elementCustoms.end() ? it->second : nullptr;
}

std::shared_ptr<CHTLCustom> CHTLGlobalMap::getVarCustom(const std::string& name) {
    auto it = varCustoms.find(name);
    return it != varCustoms.end() ? it->second : nullptr;
}

bool CHTLGlobalMap::hasStyleCustom(const std::string& name) const {
    return styleCustoms.find(name) != styleCustoms.end();
}

bool CHTLGlobalMap::hasElementCustom(const std::string& name) const {
    return elementCustoms.find(name) != elementCustoms.end();
}

bool CHTLGlobalMap::hasVarCustom(const std::string& name) const {
    return varCustoms.find(name) != varCustoms.end();
}

// 配置管理方法
void CHTLGlobalMap::addConfiguration(std::shared_ptr<CHTLConfiguration> config) {
    configurations.push_back(config);
}

void CHTLGlobalMap::setActiveConfiguration(const std::string& name) {
    for (const auto& config : configurations) {
        if (config->name == name) {
            activeConfiguration = config;
            return;
        }
    }
}

std::shared_ptr<CHTLConfiguration> CHTLGlobalMap::getActiveConfiguration() const {
    return activeConfiguration;
}

std::shared_ptr<CHTLConfiguration> CHTLGlobalMap::getConfiguration(const std::string& name) const {
    for (const auto& config : configurations) {
        if (config->name == name) {
            return config;
        }
    }
    return nullptr;
}

// 命名空间管理方法
void CHTLGlobalMap::addNamespace(const std::string& name, std::shared_ptr<CHTLNamespace> ns) {
    namespaces[name] = ns;
}

void CHTLGlobalMap::setCurrentNamespace(const std::string& name) {
    currentNamespace = name;
}

std::string CHTLGlobalMap::getCurrentNamespace() const {
    return currentNamespace;
}

std::shared_ptr<CHTLNamespace> CHTLGlobalMap::getNamespace(const std::string& name) const {
    auto it = namespaces.find(name);
    return it != namespaces.end() ? it->second : nullptr;
}

// 导入管理方法
void CHTLGlobalMap::addImport(std::shared_ptr<CHTLImport> import) {
    imports.push_back(import);
}

std::vector<std::shared_ptr<CHTLImport>> CHTLGlobalMap::getImports() const {
    return imports;
}

// 文件管理方法
void CHTLGlobalMap::setCurrentFile(const std::string& fileName, const std::string& filePath) {
    currentFileName = fileName;
    currentFilePath = filePath;
}

std::string CHTLGlobalMap::getCurrentFileName() const {
    return currentFileName;
}

std::string CHTLGlobalMap::getCurrentFilePath() const {
    return currentFilePath;
}

// 模块搜索路径管理
void CHTLGlobalMap::addModuleSearchPath(const std::string& path) {
    if (std::find(moduleSearchPaths.begin(), moduleSearchPaths.end(), path) == moduleSearchPaths.end()) {
        moduleSearchPaths.push_back(path);
    }
}

void CHTLGlobalMap::setOfficialModulePath(const std::string& path) {
    officialModulePath = path;
}

std::vector<std::string> CHTLGlobalMap::getModuleSearchPaths() const {
    return moduleSearchPaths;
}

std::string CHTLGlobalMap::getOfficialModulePath() const {
    return officialModulePath;
}

// 冲突检测
bool CHTLGlobalMap::hasNameConflict(const std::string& name, TemplateType type) const {
    switch (type) {
        case TemplateType::STYLE:
            return hasStyleTemplate(name);
        case TemplateType::ELEMENT:
            return hasElementTemplate(name);
        case TemplateType::VAR:
            return hasVarTemplate(name);
        default:
            return false;
    }
}

bool CHTLGlobalMap::hasNameConflict(const std::string& name, CustomType type) const {
    switch (type) {
        case CustomType::STYLE:
            return hasStyleCustom(name);
        case CustomType::ELEMENT:
            return hasElementCustom(name);
        case CustomType::VAR:
            return hasVarCustom(name);
        default:
            return false;
    }
}

bool CHTLGlobalMap::hasNamespaceConflict(const std::string& name) const {
    return namespaces.find(name) != namespaces.end();
}

// 清理方法
void CHTLGlobalMap::clear() {
    clearTemplates();
    clearCustoms();
    clearConfigurations();
    clearNamespaces();
    clearImports();
}

void CHTLGlobalMap::clearTemplates() {
    styleTemplates.clear();
    elementTemplates.clear();
    varTemplates.clear();
}

void CHTLGlobalMap::clearCustoms() {
    styleCustoms.clear();
    elementCustoms.clear();
    varCustoms.clear();
}

void CHTLGlobalMap::clearConfigurations() {
    configurations.clear();
    activeConfiguration = nullptr;
}

void CHTLGlobalMap::clearNamespaces() {
    namespaces.clear();
    currentNamespace.clear();
}

void CHTLGlobalMap::clearImports() {
    imports.clear();
}

// 调试方法
void CHTLGlobalMap::printStatus() const {
    std::cout << "=== CHTL Global Map Status ===" << std::endl;
    std::cout << "Style Templates: " << styleTemplates.size() << std::endl;
    std::cout << "Element Templates: " << elementTemplates.size() << std::endl;
    std::cout << "Var Templates: " << varTemplates.size() << std::endl;
    std::cout << "Style Customs: " << styleCustoms.size() << std::endl;
    std::cout << "Element Customs: " << elementCustoms.size() << std::endl;
    std::cout << "Var Customs: " << varCustoms.size() << std::endl;
    std::cout << "Configurations: " << configurations.size() << std::endl;
    std::cout << "Namespaces: " << namespaces.size() << std::endl;
    std::cout << "Imports: " << imports.size() << std::endl;
    std::cout << "Current Namespace: " << currentNamespace << std::endl;
    std::cout << "Current File: " << currentFileName << std::endl;
    std::cout << "=============================" << std::endl;
}

size_t CHTLGlobalMap::getTemplateCount() const {
    return styleTemplates.size() + elementTemplates.size() + varTemplates.size();
}

size_t CHTLGlobalMap::getCustomCount() const {
    return styleCustoms.size() + elementCustoms.size() + varCustoms.size();
}

size_t CHTLGlobalMap::getConfigurationCount() const {
    return configurations.size();
}

size_t CHTLGlobalMap::getNamespaceCount() const {
    return namespaces.size();
}

size_t CHTLGlobalMap::getImportCount() const {
    return imports.size();
}

} // namespace CHTL