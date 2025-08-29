#include "CJMODRuntime.h"
#include "../../../Error/ErrorReport.h"
#include <fstream>
#include <regex>
#include <filesystem>
// TODO: Replace with proper JSON library
// #include <json/json.h>  // 假设使用jsoncpp

#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif

namespace CHTL {

// CJMODRuntimeContext implementation
std::string CJMODRuntimeContext::getCompilerVersion() const {
    return "1.0.0";
}

std::string CJMODRuntimeContext::getCurrentFile() const {
    // 从编译器上下文获取
    return currentFile_;
}

size_t CJMODRuntimeContext::getCurrentLine() const {
    return currentLine_;
}

size_t CJMODRuntimeContext::getCurrentColumn() const {
    return currentColumn_;
}

void CJMODRuntimeContext::log(const std::string& message) {
    ErrorReport::getInstance().info("[CJMOD] " + message);
}

void CJMODRuntimeContext::logError(const std::string& error) {
    ErrorReport::getInstance().error("[CJMOD] " + error);
}

void CJMODRuntimeContext::logWarning(const std::string& warning) {
    ErrorReport::getInstance().warning("[CJMOD] " + warning);
}

std::string CJMODRuntimeContext::getConfig(const std::string& key) const {
    auto it = config_.find(key);
    return it != config_.end() ? it->second : "";
}

void CJMODRuntimeContext::setCacheValue(const std::string& key, const std::string& value) {
    cache_[key] = value;
}

std::string CJMODRuntimeContext::getCacheValue(const std::string& key) const {
    auto it = cache_.find(key);
    return it != cache_.end() ? it->second : "";
}

bool CJMODRuntimeContext::hasCacheValue(const std::string& key) const {
    return cache_.find(key) != cache_.end();
}

// CJMODRuntime implementation
CJMODRuntime::~CJMODRuntime() {
    unloadAll();
}

bool CJMODRuntime::loadModule(const std::string& path) {
    // 检查是否为.cjmod文件
    if (path.substr(path.length() - 6) != ".cjmod") {
        ErrorBuilder(ErrorLevel::ERROR, ErrorType::IO_ERROR)
            .withMessage("Not a CJMOD file")
            .atLocation(path, 0, 0)
            .report();
        return false;
    }
    
    // 解析CJMOD文件获取信息
    CJMODPackager packager;
    CJMODStructure structure;
    
    // TODO: Implement proper CJMOD file parsing
    // For now, create a simple CJMODInfo
    CJMODInfo info;
    info.name = std::filesystem::path(path).stem().string();
    info.version = "1.0.0";
    
    // 检查是否已加载
    if (modules_.find(info.name) != modules_.end()) {
        ErrorBuilder(ErrorLevel::WARNING, ErrorType::REFERENCE_ERROR)
            .withMessage("Module already loaded: " + info.name)
            .report();
        return false;
    }
    
    // 如果有C++扩展，加载动态库
    if (info.hasExtension) {
        return loadModule(info, info.extensionPath);
    }
    
    // 只有语法定义，直接加载
    LoadedModule module;
    module.info = info;
    module.syntaxPatterns = parseSyntaxDefinitions(info.syntaxDefinitions);
    
    modules_[info.name] = std::move(module);
    return true;
}

bool CJMODRuntime::loadModule(const CJMODInfo& info, const std::string& extensionPath) {
    // 加载动态库
    void* handle = loadDynamicLibrary(extensionPath);
    if (!handle) {
        return false;
    }
    
    // 创建扩展实例
    auto extension = createExtension(handle);
    if (!extension) {
        unloadDynamicLibrary(handle);
        return false;
    }
    
    // 初始化扩展
    if (!extension->initialize(context_.get())) {
        unloadDynamicLibrary(handle);
        return false;
    }
    
    // 创建模块记录
    LoadedModule module;
    module.info = info;
    module.handle = handle;
    module.extension = std::move(extension);
    module.syntaxPatterns = parseSyntaxDefinitions(info.syntaxDefinitions);
    
    modules_[info.name] = std::move(module);
    
    context_->log("Loaded CJMOD: " + info.name + " v" + info.version);
    return true;
}

std::vector<SyntaxPattern> CJMODRuntime::getAllSyntaxPatterns() const {
    std::vector<SyntaxPattern> allPatterns;
    
    for (const auto& [name, module] : modules_) {
        allPatterns.insert(allPatterns.end(), 
                          module.syntaxPatterns.begin(), 
                          module.syntaxPatterns.end());
    }
    
    return allPatterns;
}

ProcessResult CJMODRuntime::processFragment(
    const std::string& moduleName,
    const std::string& syntaxName,
    const std::string& fragment,
    const std::map<std::string, std::string>& captures
) {
    ProcessResult result;
    
    auto it = modules_.find(moduleName);
    if (it == modules_.end()) {
        result.errorMessage = "Module not found: " + moduleName;
        return result;
    }
    
    const auto& module = it->second;
    
    // 如果有C++扩展，调用它
    if (module.extension) {
        try {
            result = module.extension->process(syntaxName, fragment, captures);
        } catch (const std::exception& e) {
            result.success = false;
            result.errorMessage = "Extension error: " + std::string(e.what());
        }
    } else {
        // 纯语法定义，使用默认处理
        result.success = true;
        result.generatedCode = fragment; // 默认原样返回
    }
    
    return result;
}

void CJMODRuntime::unloadModule(const std::string& moduleName) {
    auto it = modules_.find(moduleName);
    if (it == modules_.end()) {
        return;
    }
    
    auto& module = it->second;
    
    if (module.extension) {
        module.extension->cleanup();
    }
    
    if (module.handle) {
        unloadDynamicLibrary(module.handle);
    }
    
    modules_.erase(it);
    context_->log("Unloaded CJMOD: " + moduleName);
}

void CJMODRuntime::unloadAll() {
    for (auto& [name, module] : modules_) {
        if (module.extension) {
            module.extension->cleanup();
        }
        
        if (module.handle) {
            unloadDynamicLibrary(module.handle);
        }
    }
    
    modules_.clear();
}

void* CJMODRuntime::loadDynamicLibrary(const std::string& path) {
    void* handle = nullptr;
    
#ifdef _WIN32
    handle = LoadLibraryA(path.c_str());
    if (!handle) {
        DWORD error = GetLastError();
        ErrorBuilder(ErrorLevel::ERROR, ErrorType::IO_ERROR)
            .withMessage("Failed to load DLL")
            .withDetail("Error code: " + std::to_string(error))
            .atLocation(path, 0, 0)
            .report();
    }
#else
    handle = dlopen(path.c_str(), RTLD_LAZY);
    if (!handle) {
        ErrorBuilder(ErrorLevel::ERROR, ErrorType::IO_ERROR)
            .withMessage("Failed to load shared library")
            .withDetail(dlerror())
            .atLocation(path, 0, 0)
            .report();
    }
#endif
    
    return handle;
}

void CJMODRuntime::unloadDynamicLibrary(void* handle) {
    if (!handle) return;
    
#ifdef _WIN32
    FreeLibrary((HMODULE)handle);
#else
    dlclose(handle);
#endif
}

std::unique_ptr<CJMODExtension> CJMODRuntime::createExtension(void* handle) {
    if (!handle) return nullptr;
    
    // 获取工厂函数
    CreateExtensionFunc createFunc = nullptr;
    // DestroyExtensionFunc destroyFunc = nullptr;
    
#ifdef _WIN32
    createFunc = (CreateExtensionFunc)GetProcAddress((HMODULE)handle, CJMOD_CREATE_EXTENSION_FUNC);
    // destroyFunc = (DestroyExtensionFunc)GetProcAddress((HMODULE)handle, CJMOD_DESTROY_EXTENSION_FUNC);
#else
    createFunc = (CreateExtensionFunc)dlsym(handle, CJMOD_CREATE_EXTENSION_FUNC);
    // destroyFunc = (DestroyExtensionFunc)dlsym(handle, CJMOD_DESTROY_EXTENSION_FUNC);
#endif
    
    if (!createFunc) {
        ErrorBuilder(ErrorLevel::ERROR, ErrorType::INTERNAL_ERROR)
            .withMessage("Extension missing create function")
            .report();
        return nullptr;
    }
    
    // 创建扩展实例
    CJMODExtension* rawPtr = createFunc();
    if (!rawPtr) {
        ErrorBuilder(ErrorLevel::ERROR, ErrorType::INTERNAL_ERROR)
            .withMessage("Failed to create extension instance")
            .report();
        return nullptr;
    }
    
    // 包装为unique_ptr
    // TODO: Implement custom deleter if needed
    return std::unique_ptr<CJMODExtension>(rawPtr);
}

std::vector<SyntaxPattern> CJMODRuntime::parseSyntaxDefinitions(
    const std::map<std::string, std::string>& definitions
) {
    std::vector<SyntaxPattern> patterns;
    
    for (const auto& [name, jsonStr] : definitions) {
        try {
            // TODO: Replace with proper JSON parsing
            // For now, use simple string parsing
            SyntaxPattern pattern;
            pattern.name = name;
            
            // Simple regex extraction (find pattern between quotes)
            std::regex patternRegex(R"(\"pattern\"\s*:\s*\"([^\"]*)\"))");
            std::smatch match;
            if (std::regex_search(jsonStr, match, patternRegex)) {
                pattern.regex = match[1];
            }
            
            // Simple processor extraction
            std::regex processorRegex(R"(\"processor\"\s*:\s*\"([^\"]*)\"))");
            if (std::regex_search(jsonStr, match, processorRegex)) {
                pattern.processor = match[1];
            }
            
            patterns.push_back(pattern);
            
        } catch (const std::exception& e) {
            context_->logError("Error parsing syntax definition " + name + ": " + e.what());
        }
    }
    
    return patterns;
}

} // namespace CHTL