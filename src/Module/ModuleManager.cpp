#include "ModuleManager.h"
#include <iostream>
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <sstream>

namespace CHTL::Module {

// Helper function for ends_with (C++17 compatible)
bool ends_with(const String& str, const String& suffix) {
    if (str.length() >= suffix.length()) {
        return str.compare(str.length() - suffix.length(), suffix.length(), suffix) == 0;
    }
    return false;
}

// ModuleDependency实现
ModuleDependency::ModuleDependency(const String& name, const String& ver, 
                                  bool optional, bool dev)
    : moduleName(name), version(ver), isOptional(optional), 
      isDevDependency(dev), source("local") {
}

// ModuleMetadata实现
ModuleMetadata::ModuleMetadata() 
    : type(ModuleType::CHTL_MODULE), lastModified(0) {
}

String ModuleMetadata::toString() const {
    std::ostringstream oss;
    oss << "Module: " << name << "\n";
    oss << "Version: " << version << "\n";
    oss << "Type: ";
    
    switch (type) {
        case ModuleType::CHTL_MODULE: oss << "CHTL"; break;
        case ModuleType::CMOD_MODULE: oss << "CMOD"; break;
        case ModuleType::CJMOD_MODULE: oss << "CJMOD"; break;
        case ModuleType::JS_MODULE: oss << "JavaScript"; break;
        case ModuleType::CSS_MODULE: oss << "CSS"; break;
        case ModuleType::BINARY_MODULE: oss << "Binary"; break;
        case ModuleType::PLUGIN_MODULE: oss << "Plugin"; break;
    }
    
    oss << "\n";
    oss << "Description: " << description << "\n";
    oss << "Author: " << author << "\n";
    oss << "Entry Point: " << entryPoint << "\n";
    oss << "Dependencies: " << dependencies.size() << "\n";
    
    return oss.str();
}

String ModuleMetadata::toJSON() const {
    std::ostringstream oss;
    oss << "{\n";
    oss << "  \"name\": \"" << name << "\",\n";
    oss << "  \"version\": \"" << version << "\",\n";
    oss << "  \"description\": \"" << description << "\",\n";
    oss << "  \"author\": \"" << author << "\",\n";
    oss << "  \"license\": \"" << license << "\",\n";
    oss << "  \"entryPoint\": \"" << entryPoint << "\",\n";
    oss << "  \"dependencies\": [\n";
    
    for (size_t i = 0; i < dependencies.size(); ++i) {
        oss << "    {\n";
        oss << "      \"name\": \"" << dependencies[i].moduleName << "\",\n";
        oss << "      \"version\": \"" << dependencies[i].version << "\",\n";
        oss << "      \"optional\": " << (dependencies[i].isOptional ? "true" : "false") << "\n";
        oss << "    }";
        
        if (i < dependencies.size() - 1) {
            oss << ",";
        }
        oss << "\n";
    }
    
    oss << "  ]\n";
    oss << "}";
    
    return oss.str();
}

// StandardModule实现
StandardModule::StandardModule(const ModuleMetadata& metadata)
    : metadata_(metadata), status_(ModuleStatus::NOT_LOADED) {
}

StandardModule::~StandardModule() {
    if (status_ == ModuleStatus::LOADED) {
        unload();
    }
}

bool StandardModule::initialize() {
    if (status_ != ModuleStatus::NOT_LOADED) {
        return false;
    }
    
    try {
        if (doInitialize()) {
            setStatus(ModuleStatus::LOADING);
            return true;
        }
    } catch (const std::exception& e) {
        setError("Initialization failed: " + String(e.what()));
    }
    
    return false;
}

bool StandardModule::load() {
    if (status_ != ModuleStatus::LOADING && status_ != ModuleStatus::NOT_LOADED) {
        return false;
    }
    
    try {
        if (doLoad()) {
            setStatus(ModuleStatus::LOADED);
            return true;
        }
    } catch (const std::exception& e) {
        setError("Load failed: " + String(e.what()));
        setStatus(ModuleStatus::ERROR);
    }
    
    return false;
}

bool StandardModule::unload() {
    if (status_ != ModuleStatus::LOADED) {
        return false;
    }
    
    try {
        setStatus(ModuleStatus::UNLOADING);
        
        if (doUnload()) {
            setStatus(ModuleStatus::UNLOADED);
            return true;
        }
    } catch (const std::exception& e) {
        setError("Unload failed: " + String(e.what()));
        setStatus(ModuleStatus::ERROR);
    }
    
    return false;
}

bool StandardModule::reload() {
    if (!unload()) {
        return false;
    }
    
    setStatus(ModuleStatus::NOT_LOADED);
    return initialize() && load();
}

const ModuleMetadata& StandardModule::getMetadata() const {
    return metadata_;
}

ModuleStatus StandardModule::getStatus() const {
    return status_;
}

String StandardModule::getLastError() const {
    return lastError_;
}

bool StandardModule::hasExport(const String& name) const {
    return exports_.find(name) != exports_.end();
}

String StandardModule::getExport(const String& name) const {
    auto it = exports_.find(name);
    return it != exports_.end() ? it->second : "";
}

StringUnorderedMap StandardModule::getAllExports() const {
    return exports_;
}

void StandardModule::setConfig(const String& key, const String& value) {
    config_[key] = value;
}

String StandardModule::getConfig(const String& key) const {
    auto it = config_.find(key);
    return it != config_.end() ? it->second : "";
}

StringUnorderedMap StandardModule::getAllConfig() const {
    return config_;
}

void StandardModule::addExport(const String& name, const String& value) {
    exports_[name] = value;
}

void StandardModule::removeExport(const String& name) {
    exports_.erase(name);
}

void StandardModule::setStatus(ModuleStatus status) {
    ModuleStatus oldStatus = status_;
    status_ = status;
    onStatusChanged(oldStatus, status);
}

void StandardModule::setError(const String& error) {
    lastError_ = error;
}

bool StandardModule::doInitialize() {
    // 默认实现，子类可以重写
    return true;
}

bool StandardModule::doLoad() {
    // 默认实现，子类可以重写
    return true;
}

bool StandardModule::doUnload() {
    // 默认实现，子类可以重写
    return true;
}

void StandardModule::onStatusChanged(ModuleStatus oldStatus, ModuleStatus newStatus) {
    // 默认实现，子类可以重写
}

// CHTLModuleLoader实现
bool CHTLModuleLoader::canLoad(const String& modulePath) const {
    return ends_with(modulePath, ".chtl") || ends_with(modulePath, ".chtlm");
}

std::unique_ptr<IModule> CHTLModuleLoader::loadModule(const String& modulePath) {
    if (!canLoad(modulePath)) {
        return nullptr;
    }
    
    ModuleMetadata metadata = extractMetadata(modulePath);
    if (metadata.name.empty()) {
        return nullptr;
    }
    
    auto module = std::make_unique<StandardModule>(metadata);
    
    // 加载CHTL模块的具体逻辑
    if (module->initialize() && module->load()) {
        return module;
    }
    
    return nullptr;
}

bool CHTLModuleLoader::validateModule(const String& modulePath) const {
    try {
        std::ifstream file(modulePath);
        if (!file.is_open()) {
            return false;
        }
        
        String content((std::istreambuf_iterator<char>(file)),
                      std::istreambuf_iterator<char>());
        
        // 基本的CHTL语法验证
        return content.find("@template") != String::npos ||
               content.find("@custom") != String::npos ||
               content.find("@module") != String::npos;
        
    } catch (...) {
        return false;
    }
}

ModuleMetadata CHTLModuleLoader::extractMetadata(const String& modulePath) const {
    ModuleMetadata metadata;
    
    try {
        std::ifstream file(modulePath);
        if (!file.is_open()) {
            return metadata;
        }
        
        String content((std::istreambuf_iterator<char>(file)),
                      std::istreambuf_iterator<char>());
        
        if (parseCHTLModule(content, metadata)) {
            metadata.sourceFile = modulePath;
            metadata.type = ModuleType::CHTL_MODULE;
            
            // 获取文件修改时间
            try {
                auto ftime = std::filesystem::last_write_time(modulePath);
                metadata.lastModified = std::chrono::duration_cast<std::chrono::seconds>(
                    ftime.time_since_epoch()).count();
            } catch (...) {
                metadata.lastModified = 0;
            }
        }
        
    } catch (...) {
        // 返回空的metadata
    }
    
    return metadata;
}

bool CHTLModuleLoader::parseCHTLModule(const String& content, ModuleMetadata& metadata) const {
    // 简单的CHTL模块解析
    size_t pos = 0;
    
    // 查找@module声明
    pos = content.find("@module");
    if (pos != String::npos) {
        size_t nameStart = content.find_first_not_of(" \t", pos + 7);
        if (nameStart != String::npos) {
            size_t nameEnd = content.find_first_of(" \t\n{", nameStart);
            if (nameEnd != String::npos) {
                metadata.name = content.substr(nameStart, nameEnd - nameStart);
            }
        }
    }
    
    // 查找版本信息
    pos = content.find("@version");
    if (pos != String::npos) {
        size_t versionStart = content.find_first_not_of(" \t", pos + 8);
        if (versionStart != String::npos) {
            size_t versionEnd = content.find_first_of(" \t\n", versionStart);
            if (versionEnd != String::npos) {
                metadata.version = content.substr(versionStart, versionEnd - versionStart);
            }
        }
    }
    
    // 查找描述信息
    pos = content.find("@description");
    if (pos != String::npos) {
        size_t descStart = content.find_first_not_of(" \t", pos + 12);
        if (descStart != String::npos) {
            size_t descEnd = content.find('\n', descStart);
            if (descEnd != String::npos) {
                metadata.description = content.substr(descStart, descEnd - descStart);
            }
        }
    }
    
    return !metadata.name.empty();
}

// CMODModuleLoader实现
bool CMODModuleLoader::canLoad(const String& modulePath) const {
    return ends_with(modulePath, ".cmod");
}

std::unique_ptr<IModule> CMODModuleLoader::loadModule(const String& modulePath) {
    if (!canLoad(modulePath)) {
        return nullptr;
    }
    
    ModuleMetadata metadata = extractMetadata(modulePath);
    if (metadata.name.empty()) {
        return nullptr;
    }
    
    auto module = std::make_unique<StandardModule>(metadata);
    
    if (module->initialize() && module->load()) {
        return module;
    }
    
    return nullptr;
}

bool CMODModuleLoader::validateModule(const String& modulePath) const {
    String tempDir;
    return unpackCMOD(modulePath, tempDir);
}

ModuleMetadata CMODModuleLoader::extractMetadata(const String& modulePath) const {
    ModuleMetadata metadata;
    
    String tempDir;
    if (unpackCMOD(modulePath, tempDir)) {
        // 读取info文件
        String infoPath = tempDir + "/info";
        std::ifstream infoFile(infoPath);
        
        if (infoFile.is_open()) {
            String infoContent((std::istreambuf_iterator<char>(infoFile)),
                              std::istreambuf_iterator<char>());
            
            if (parseCMODInfo(infoContent, metadata)) {
                metadata.sourceFile = modulePath;
                metadata.type = ModuleType::CMOD_MODULE;
            }
        }
        
        // 清理临时目录
        try {
            std::filesystem::remove_all(tempDir);
        } catch (...) {
            // 忽略清理错误
        }
    }
    
    return metadata;
}

bool CMODModuleLoader::unpackCMOD(const String& cmodPath, String& tempDir) const {
    // 这里需要实际的CMOD解压缩实现
    // 暂时返回false
    return false;
}

bool CMODModuleLoader::parseCMODInfo(const String& infoContent, ModuleMetadata& metadata) const {
    // 简单的info文件解析
    std::istringstream iss(infoContent);
    String line;
    
    while (std::getline(iss, line)) {
        size_t colonPos = line.find(':');
        if (colonPos != String::npos) {
            String key = line.substr(0, colonPos);
            String value = line.substr(colonPos + 1);
            
            // 去除空白
            key.erase(0, key.find_first_not_of(" \t"));
            key.erase(key.find_last_not_of(" \t") + 1);
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t") + 1);
            
            if (key == "name") {
                metadata.name = value;
            } else if (key == "version") {
                metadata.version = value;
            } else if (key == "description") {
                metadata.description = value;
            } else if (key == "author") {
                metadata.author = value;
            } else if (key == "license") {
                metadata.license = value;
            } else if (key == "entry") {
                metadata.entryPoint = value;
            }
        }
    }
    
    return !metadata.name.empty();
}

// ModuleManager实现
ModuleManager& ModuleManager::getInstance() {
    static ModuleManager instance;
    return instance;
}

void ModuleManager::registerLoader(ModuleType type, std::unique_ptr<IModuleLoader> loader) {
    loaders_[type] = std::move(loader);
}

void ModuleManager::unregisterLoader(ModuleType type) {
    loaders_.erase(type);
}

IModuleLoader* ModuleManager::getLoader(ModuleType type) {
    auto it = loaders_.find(type);
    return it != loaders_.end() ? it->second.get() : nullptr;
}

bool ModuleManager::loadModule(const String& modulePath, const String& alias) {
    ModuleType type = detectModuleType(modulePath);
    IModuleLoader* loader = getLoader(type);
    
    if (!loader) {
        return false;
    }
    
    auto module = loader->loadModule(modulePath);
    if (!module) {
        return false;
    }
    
    String moduleName = alias.empty() ? module->getMetadata().name : alias;
    modules_[moduleName] = std::move(module);
    
    if (eventCallback_) {
        notifyModuleEvent(moduleName, ModuleStatus::LOADED);
    }
    
    return true;
}

bool ModuleManager::unloadModule(const String& moduleName) {
    auto it = modules_.find(moduleName);
    if (it == modules_.end()) {
        return false;
    }
    
    bool success = it->second->unload();
    if (success) {
        modules_.erase(it);
        
        if (eventCallback_) {
            notifyModuleEvent(moduleName, ModuleStatus::UNLOADED);
        }
    }
    
    return success;
}

bool ModuleManager::reloadModule(const String& moduleName) {
    auto it = modules_.find(moduleName);
    if (it == modules_.end()) {
        return false;
    }
    
    return it->second->reload();
}

IModule* ModuleManager::getModule(const String& moduleName) {
    auto it = modules_.find(moduleName);
    return it != modules_.end() ? it->second.get() : nullptr;
}

const IModule* ModuleManager::getModule(const String& moduleName) const {
    auto it = modules_.find(moduleName);
    return it != modules_.end() ? it->second.get() : nullptr;
}

bool ModuleManager::hasModule(const String& moduleName) const {
    return modules_.find(moduleName) != modules_.end();
}

StringVector ModuleManager::getLoadedModules() const {
    StringVector result;
    for (const auto& [name, module] : modules_) {
        result.push_back(name);
    }
    return result;
}

size_t ModuleManager::getModuleCount() const {
    return modules_.size();
}

void ModuleManager::addSearchPath(const String& path) {
    if (std::find(searchPaths_.begin(), searchPaths_.end(), path) == searchPaths_.end()) {
        searchPaths_.push_back(path);
    }
}

void ModuleManager::removeSearchPath(const String& path) {
    searchPaths_.erase(std::remove(searchPaths_.begin(), searchPaths_.end(), path),
                      searchPaths_.end());
}

StringVector ModuleManager::getSearchPaths() const {
    return searchPaths_;
}

String ModuleManager::findModule(const String& moduleName) const {
    for (const auto& searchPath : searchPaths_) {
        StringVector extensions = {".chtl", ".cmod", ".js", ".css"};
        
        for (const auto& ext : extensions) {
            String fullPath = searchPath + "/" + moduleName + ext;
            
            try {
                if (std::filesystem::exists(fullPath)) {
                    return fullPath;
                }
            } catch (...) {
                continue;
            }
        }
    }
    
    return "";
}

ModuleType ModuleManager::detectModuleType(const String& modulePath) const {
    if (ends_with(modulePath, ".chtl") || ends_with(modulePath, ".chtlm")) {
        return ModuleType::CHTL_MODULE;
    } else if (ends_with(modulePath, ".cmod")) {
        return ModuleType::CMOD_MODULE;
    } else if (ends_with(modulePath, ".cjmod")) {
        return ModuleType::CJMOD_MODULE;
    } else if (ends_with(modulePath, ".js")) {
        return ModuleType::JS_MODULE;
    } else if (ends_with(modulePath, ".css")) {
        return ModuleType::CSS_MODULE;
    }
    
    return ModuleType::CHTL_MODULE; // 默认类型
}

void ModuleManager::notifyModuleEvent(const String& moduleName, ModuleStatus status) {
    if (eventCallback_) {
        eventCallback_(moduleName, status);
    }
}

void ModuleManager::setModuleEventCallback(ModuleEventCallback callback) {
    eventCallback_ = callback;
}

String ModuleManager::getModuleReport() const {
    std::ostringstream oss;
    
    oss << "Module Manager Report\n";
    oss << "=====================\n";
    oss << "Total Modules: " << modules_.size() << "\n";
    oss << "Search Paths: " << searchPaths_.size() << "\n\n";
    
    oss << "Loaded Modules:\n";
    for (const auto& [name, module] : modules_) {
        oss << "  " << name << " (" << module->getMetadata().version << ")\n";
        oss << "    Status: ";
        
        switch (module->getStatus()) {
            case ModuleStatus::NOT_LOADED: oss << "Not Loaded"; break;
            case ModuleStatus::LOADING: oss << "Loading"; break;
            case ModuleStatus::LOADED: oss << "Loaded"; break;
            case ModuleStatus::ERROR: oss << "Error"; break;
            case ModuleStatus::UNLOADING: oss << "Unloading"; break;
            case ModuleStatus::UNLOADED: oss << "Unloaded"; break;
        }
        
        oss << "\n";
        
        if (module->getStatus() == ModuleStatus::ERROR) {
            oss << "    Error: " << module->getLastError() << "\n";
        }
    }
    
    return oss.str();
}

void ModuleManager::unloadAllModules() {
    StringVector moduleNames;
    for (const auto& [name, module] : modules_) {
        moduleNames.push_back(name);
    }
    
    for (const auto& name : moduleNames) {
        unloadModule(name);
    }
}

void ModuleManager::reset() {
    unloadAllModules();
    searchPaths_.clear();
    globalConfig_.clear();
    metadataCache_.clear();
    cacheTimestamps_.clear();
}

// ModuleGuard实现
ModuleGuard::ModuleGuard(const String& modulePath)
    : moduleName_(""), shouldUnload_(false) {
    
    ModuleManager& manager = ModuleManager::getInstance();
    
    if (manager.loadModule(modulePath)) {
        // 从路径中提取模块名
        size_t lastSlash = modulePath.find_last_of("/\\");
        size_t lastDot = modulePath.find_last_of('.');
        
        if (lastSlash != String::npos && lastDot != String::npos && lastDot > lastSlash) {
            moduleName_ = modulePath.substr(lastSlash + 1, lastDot - lastSlash - 1);
        } else if (lastDot != String::npos) {
            moduleName_ = modulePath.substr(0, lastDot);
        } else {
            moduleName_ = modulePath;
        }
        
        shouldUnload_ = true;
    }
}

ModuleGuard::~ModuleGuard() {
    if (shouldUnload_ && !moduleName_.empty()) {
        ModuleManager::getInstance().unloadModule(moduleName_);
    }
}

IModule* ModuleGuard::getModule() {
    if (moduleName_.empty()) {
        return nullptr;
    }
    
    return ModuleManager::getInstance().getModule(moduleName_);
}

bool ModuleGuard::isLoaded() const {
    return !moduleName_.empty() && 
           ModuleManager::getInstance().hasModule(moduleName_);
}

// ModuleFactory的静态成员
std::map<String, std::function<std::unique_ptr<IModule>(const ModuleMetadata&)>> ModuleFactory::creators_;

std::unique_ptr<IModule> ModuleFactory::createModule(const String& typeName, const ModuleMetadata& metadata) {
    auto it = creators_.find(typeName);
    if (it != creators_.end()) {
        return it->second(metadata);
    }
    
    // 默认创建StandardModule
    return std::make_unique<StandardModule>(metadata);
}

StringVector ModuleFactory::getRegisteredTypes() {
    StringVector types;
    for (const auto& [typeName, creator] : creators_) {
        types.push_back(typeName);
    }
    return types;
}

} // namespace CHTL::Module