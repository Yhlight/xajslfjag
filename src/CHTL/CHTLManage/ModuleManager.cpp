#include "ModuleManager.h"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <regex>

namespace CHTL {

ModuleManager::ModuleManager() {
    // 设置默认的模块搜索路径
    m_moduleSearchPaths.push_back("./module");
    m_moduleSearchPaths.push_back("./");
}

void ModuleManager::setOfficialModuleDirectory(const std::string& directory) {
    m_officialModuleDirectory = directory;
}

void ModuleManager::addModuleSearchPath(const std::string& path) {
    // 避免重复添加
    if (std::find(m_moduleSearchPaths.begin(), m_moduleSearchPaths.end(), path) 
        == m_moduleSearchPaths.end()) {
        m_moduleSearchPaths.push_back(path);
    }
}

std::vector<std::string> ModuleManager::getModuleSearchPaths() const {
    return m_moduleSearchPaths;
}

std::string ModuleManager::findModule(const std::string& moduleName, ModuleType type) {
    std::string actualModuleName = moduleName;
    
    // 检查是否为官方模块前缀
    if (isOfficialModulePrefix(moduleName)) {
        actualModuleName = parseOfficialModuleName(moduleName);
        
        // 只在官方模块目录搜索
        if (!m_officialModuleDirectory.empty()) {
            return searchModuleInDirectory(m_officialModuleDirectory, actualModuleName, type);
        }
        return "";
    }
    
    // 按优先级搜索：官方模块目录 -> module文件夹 -> 当前目录
    std::vector<std::string> searchDirs;
    
    if (!m_officialModuleDirectory.empty()) {
        searchDirs.push_back(m_officialModuleDirectory);
    }
    
    for (const auto& path : m_moduleSearchPaths) {
        searchDirs.push_back(path);
    }
    
    for (const auto& dir : searchDirs) {
        std::string found = searchModuleInDirectory(dir, actualModuleName, type);
        if (!found.empty()) {
            return found;
        }
    }
    
    return "";
}

std::string ModuleManager::searchModuleInDirectory(const std::string& directory, 
                                                  const std::string& moduleName, 
                                                  ModuleType type) {
    if (!std::filesystem::exists(directory)) {
        return "";
    }
    
    // 支持分类结构：CMOD/CJMOD文件夹
    std::vector<std::string> subdirs = {directory};
    
    if (type == ModuleType::CMOD || type == ModuleType::CHTL_FILE) {
        subdirs.push_back(directory + "/CMOD");
        subdirs.push_back(directory + "/cmod");
        subdirs.push_back(directory + "/Cmod");
    } else if (type == ModuleType::CJMOD) {
        subdirs.push_back(directory + "/CJMOD");
        subdirs.push_back(directory + "/cjmod");
        subdirs.push_back(directory + "/CJmod");
    }
    
    for (const auto& subdir : subdirs) {
        if (!std::filesystem::exists(subdir)) continue;
        
        // 检查是否有具体的文件扩展名
        if (moduleName.find('.') != std::string::npos) {
            // 具体文件名
            std::string fullPath = subdir + "/" + moduleName;
            if (std::filesystem::exists(fullPath)) {
                return fullPath;
            }
        } else {
            // 按类型搜索文件
            std::vector<std::string> extensions;
            if (type == ModuleType::CMOD) {
                extensions = {".cmod", ".chtl"};
            } else if (type == ModuleType::CJMOD) {
                extensions = {".cjmod"};
            } else if (type == ModuleType::CHTL_FILE) {
                extensions = {".chtl"};
            }
            
            for (const auto& ext : extensions) {
                std::string fullPath = subdir + "/" + moduleName + ext;
                if (std::filesystem::exists(fullPath)) {
                    return fullPath;
                }
            }
        }
    }
    
    return "";
}

std::shared_ptr<ModuleInfo> ModuleManager::loadModuleInfo(const std::string& modulePath) {
    if (!std::filesystem::exists(modulePath)) {
        reportError("Module path does not exist: " + modulePath);
        return nullptr;
    }
    
    auto moduleInfo = std::make_shared<ModuleInfo>();
    moduleInfo->path = modulePath;
    moduleInfo->type = getModuleTypeFromExtension(modulePath);
    
    if (moduleInfo->type == ModuleType::CMOD || moduleInfo->type == ModuleType::CJMOD) {
        // 从CMOD/CJMOD文件解析信息
        // 这里需要实现ZIP解压缩或直接读取压缩文件中的info信息
        // 暂时返回基本信息
        std::filesystem::path path(modulePath);
        moduleInfo->name = path.stem().string();
    } else if (moduleInfo->type == ModuleType::CHTL_FILE) {
        // 从CHTL文件解析信息
        std::filesystem::path path(modulePath);
        moduleInfo->name = path.stem().string();
        
        // 尝试查找对应的info文件
        std::string infoPath = path.parent_path().string() + "/info/" + moduleInfo->name + ".chtl";
        if (std::filesystem::exists(infoPath)) {
            auto infoFromFile = parseModuleInfoFile(infoPath);
            if (infoFromFile) {
                *moduleInfo = *infoFromFile;
                moduleInfo->path = modulePath;
                moduleInfo->type = ModuleType::CHTL_FILE;
            }
        }
    }
    
    return moduleInfo;
}

bool ModuleManager::loadModule(const std::string& moduleName, ModuleType type) {
    if (isModuleLoaded(moduleName)) {
        return true; // 已经加载
    }
    
    std::string modulePath = findModule(moduleName, type);
    if (modulePath.empty()) {
        reportError("Module not found: " + moduleName);
        return false;
    }
    
    auto moduleInfo = loadModuleInfo(modulePath);
    if (!moduleInfo) {
        reportError("Failed to load module info: " + moduleName);
        return false;
    }
    
    // 解决依赖
    if (!resolveDependencies(moduleName)) {
        reportError("Failed to resolve dependencies for module: " + moduleName);
        return false;
    }
    
    m_loadedModules[moduleName] = moduleInfo;
    return true;
}

bool ModuleManager::isModuleLoaded(const std::string& moduleName) const {
    return m_loadedModules.find(moduleName) != m_loadedModules.end();
}

std::vector<ModuleDependency> ModuleManager::parseModuleDependencies(const std::string& dependencies) {
    std::vector<ModuleDependency> deps;
    
    if (dependencies.empty()) {
        return deps;
    }
    
    // 简单的依赖解析：用逗号分隔，支持版本号
    std::regex depPattern(R"(\s*([^,\s]+)(?:\s*@([^,\s]+))?\s*)");
    std::sregex_iterator iter(dependencies.begin(), dependencies.end(), depPattern);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        std::smatch match = *iter;
        std::string name = match[1].str();
        std::string version = match[2].str();
        
        deps.emplace_back(name, version, true);
    }
    
    return deps;
}

bool ModuleManager::resolveDependencies(const std::string& moduleName) {
    auto moduleIt = m_loadedModules.find(moduleName);
    if (moduleIt == m_loadedModules.end()) {
        return false;
    }
    
    auto dependencies = parseModuleDependencies(moduleIt->second->dependencies);
    
    for (const auto& dep : dependencies) {
        if (!dep.required) continue;
        
        if (!isModuleLoaded(dep.name)) {
            if (!loadModule(dep.name)) {
                reportError("Failed to load dependency: " + dep.name + " for module: " + moduleName);
                return false;
            }
        }
    }
    
    return true;
}

bool ModuleManager::checkCircularDependency(const std::string& moduleName, 
                                           std::vector<std::string>& visited) {
    if (std::find(visited.begin(), visited.end(), moduleName) != visited.end()) {
        return true; // 发现循环依赖
    }
    
    visited.push_back(moduleName);
    
    auto moduleIt = m_loadedModules.find(moduleName);
    if (moduleIt != m_loadedModules.end()) {
        auto dependencies = parseModuleDependencies(moduleIt->second->dependencies);
        for (const auto& dep : dependencies) {
            if (checkCircularDependency(dep.name, visited)) {
                return true;
            }
        }
    }
    
    visited.pop_back();
    return false;
}

bool ModuleManager::packModule(const std::string& moduleDirectory, const std::string& outputPath) {
    // 验证模块结构
    if (!validateModuleStructure(moduleDirectory)) {
        reportError("Invalid module structure: " + moduleDirectory);
        return false;
    }
    
    // 这里需要实现ZIP压缩功能
    // 暂时返回true，实际实现需要zip库
    
    reportError("Module packing not yet implemented");
    return false;
}

bool ModuleManager::unpackModule(const std::string& cmodPath, const std::string& outputDirectory) {
    // 这里需要实现ZIP解压缩功能
    // 暂时返回true，实际实现需要zip库
    
    reportError("Module unpacking not yet implemented");
    return false;
}

bool ModuleManager::isOfficialModulePrefix(const std::string& moduleName) {
    return moduleName.substr(0, 6) == "chtl::";
}

std::string ModuleManager::parseOfficialModuleName(const std::string& moduleName) {
    if (isOfficialModulePrefix(moduleName)) {
        return moduleName.substr(6); // 移除"chtl::"前缀
    }
    return moduleName;
}

ModuleType ModuleManager::getModuleTypeFromExtension(const std::string& filePath) {
    std::filesystem::path path(filePath);
    std::string ext = path.extension().string();
    
    if (ext == ".cmod") {
        return ModuleType::CMOD;
    } else if (ext == ".cjmod") {
        return ModuleType::CJMOD;
    } else if (ext == ".chtl") {
        return ModuleType::CHTL_FILE;
    }
    
    return ModuleType::UNKNOWN;
}

std::vector<std::string> ModuleManager::getLoadedModules() const {
    std::vector<std::string> modules;
    for (const auto& [name, info] : m_loadedModules) {
        modules.push_back(name);
    }
    return modules;
}

std::string ModuleManager::generateModuleReport() const {
    std::stringstream ss;
    
    ss << "Module Manager Report:\n";
    ss << "=====================\n\n";
    
    ss << "Official Module Directory: " << m_officialModuleDirectory << "\n\n";
    
    ss << "Module Search Paths:\n";
    for (const auto& path : m_moduleSearchPaths) {
        ss << "  " << path << "\n";
    }
    ss << "\n";
    
    ss << "Loaded Modules (" << m_loadedModules.size() << "):\n";
    for (const auto& [name, info] : m_loadedModules) {
        ss << "  " << name << " (v" << info->version << ")\n";
        ss << "    Type: ";
        switch (info->type) {
            case ModuleType::CMOD: ss << "CMOD"; break;
            case ModuleType::CJMOD: ss << "CJMOD"; break;
            case ModuleType::CHTL_FILE: ss << "CHTL File"; break;
            default: ss << "Unknown"; break;
        }
        ss << "\n";
        ss << "    Path: " << info->path << "\n";
        if (!info->dependencies.empty()) {
            ss << "    Dependencies: " << info->dependencies << "\n";
        }
        ss << "\n";
    }
    
    return ss.str();
}

bool ModuleManager::validateModuleStructure(const std::string& modulePath) {
    if (!std::filesystem::exists(modulePath)) {
        return false;
    }
    
    if (std::filesystem::is_directory(modulePath)) {
        // 检查CMOD结构：需要src和info文件夹
        std::string srcPath = modulePath + "/src";
        std::string infoPath = modulePath + "/info";
        
        return std::filesystem::exists(srcPath) && std::filesystem::exists(infoPath);
    }
    
    return true; // 单个文件总是有效的
}

std::shared_ptr<ModuleInfo> ModuleManager::parseModuleInfoFile(const std::string& infoFilePath) {
    // 这里需要解析CHTL格式的info文件
    // 暂时返回基本信息
    auto info = std::make_shared<ModuleInfo>();
    
    std::ifstream file(infoFilePath);
    if (!file.is_open()) {
        return nullptr;
    }
    
    // 简单的解析实现
    std::string line;
    while (std::getline(file, line)) {
        if (line.find("name =") != std::string::npos) {
            size_t pos = line.find("\"");
            if (pos != std::string::npos) {
                size_t endPos = line.find("\"", pos + 1);
                if (endPos != std::string::npos) {
                    info->name = line.substr(pos + 1, endPos - pos - 1);
                }
            }
        }
        // 类似地解析其他字段...
    }
    
    return info;
}

void ModuleManager::clear() {
    m_loadedModules.clear();
    m_dependencyGraph.clear();
}

void ModuleManager::reportError(const std::string& message) {
    // 使用ErrorReporter的正确接口
    ErrorReporter reporter;
    reporter.error(ErrorType::SYNTAX_ERROR, message);
}

} // namespace CHTL