#include "CMODLoader.h"
#include "../CHTLContext/Context.h"
#include "../CHTLIOStream/CHTLFileSystem.h"
#include "../CHTLParser/Parser.h"
#include "../CHTLLexer/Lexer.h"
#include "../../Error/ErrorReport.h"
#include <filesystem>
#include <fstream>
#include <algorithm>

namespace CHTL {

namespace fs = std::filesystem;

CMODLoader::CMODLoader(std::shared_ptr<CompileContext> context)
    : context_(context) {
    // 设置默认官方模块路径
    if (config_.officialModulePath.empty()) {
        // 尝试从编译器路径推导官方模块路径
        auto execPath = fs::current_path();
        auto modulePath = execPath / "module";
        if (fs::exists(modulePath)) {
            config_.officialModulePath = modulePath.string();
        }
    }
}

bool CMODLoader::loadModule(const std::string& modulePath) {
    // 检查循环依赖
    if (checkCircularDependency(modulePath)) {
        lastError_ = "Circular dependency detected: " + modulePath;
        return false;
    }
    
    currentLoadingChain_.push_back(modulePath);
    
    // 查找模块
    auto foundPath = findModule(modulePath);
    if (!foundPath) {
        lastError_ = "Module not found: " + modulePath;
        currentLoadingChain_.pop_back();
        return false;
    }
    
    bool result = false;
    
    // 判断是CMOD还是CHTL文件
    std::string pathStr = foundPath.value();
    if (pathStr.size() > 5 && pathStr.substr(pathStr.size() - 5) == ".cmod") {
        // 提取CMOD到缓存
        std::string extractPath;
        if (!extractCMOD(foundPath.value(), extractPath)) {
            currentLoadingChain_.pop_back();
            return false;
        }
        
        // 获取模块名
        std::string moduleName = fs::path(modulePath).stem().string();
        
        // 加载解压后的模块
        result = loadExtractedModule(extractPath, moduleName);
    } else if (pathStr.size() > 5 && pathStr.substr(pathStr.size() - 5) == ".chtl") {
        // 直接加载CHTL文件
        result = loadCHTLFile(foundPath.value());
    } else {
        lastError_ = "Unknown module type: " + foundPath.value();
    }
    
    currentLoadingChain_.pop_back();
    return result;
}

bool CMODLoader::loadCHTLFile(const std::string& chtlPath) {
    // 检查文件是否存在
    if (!fs::exists(chtlPath)) {
        lastError_ = "CHTL file not found: " + chtlPath;
        return false;
    }
    
    // 获取模块名（文件名不含扩展名）
    std::string moduleName = fs::path(chtlPath).stem().string();
    
    // 检查是否已加载
    if (isModuleLoaded(moduleName)) {
        return true;  // 已加载，直接返回成功
    }
    
    // 处理CHTL文件
    if (!processCHTLFile(chtlPath)) {
        return false;
    }
    
    // 记录已加载模块
    LoadedModule module;
    module.info.name = moduleName;
    module.sourcePath = chtlPath;
    module.isCMOD = false;
    
    loadedModules_[moduleName] = module;
    
    return true;
}

bool CMODLoader::loadFromDirectory(const std::string& dir) {
    if (!fs::exists(dir) || !fs::is_directory(dir)) {
        lastError_ = "Invalid directory: " + dir;
        return false;
    }
    
    CMODPackager packager;
    CMODStructure structure;
    
    // 分析目录结构
    if (!packager.analyzeDirectory(dir, structure)) {
        lastError_ = packager.getLastError();
        return false;
    }
    
    // 解析info内容
    if (!structure.infoContent.empty()) {
        // 写入临时文件并解析
        std::string tempInfoPath = fs::temp_directory_path() / "temp_info.chtl";
        std::ofstream tempFile(tempInfoPath);
        tempFile << structure.infoContent;
        tempFile.close();
        
        if (!processCHTLFile(tempInfoPath)) {
            fs::remove(tempInfoPath);
            return false;
        }
        
        fs::remove(tempInfoPath);
    }
    
    // 加载所有源文件
    for (const auto& srcFile : structure.sourceFiles) {
        std::string fullPath = fs::path(dir) / srcFile;
        if (!processCHTLFile(fullPath)) {
            return false;
        }
    }
    
    // 创建模块信息
    LoadedModule module;
    module.info.name = structure.moduleName;
    module.sourcePath = dir;
    module.isCMOD = false;
    
    loadedModules_[structure.moduleName] = module;
    
    return true;
}

std::optional<std::string> CMODLoader::findModule(const std::string& moduleName) {
    // 获取搜索路径列表
    auto searchPaths = getModuleSearchPaths(moduleName);
    
    for (const auto& path : searchPaths) {
        if (fs::exists(path)) {
            return path;
        }
    }
    
    return std::nullopt;
}

std::optional<CMODInfo> CMODLoader::getLoadedModuleInfo(const std::string& moduleName) const {
    auto it = loadedModules_.find(moduleName);
    if (it != loadedModules_.end()) {
        return it->second.info;
    }
    return std::nullopt;
}

std::optional<CMODExport> CMODLoader::getModuleExports(const std::string& moduleName) const {
    auto it = loadedModules_.find(moduleName);
    if (it != loadedModules_.end()) {
        return it->second.exports;
    }
    return std::nullopt;
}

bool CMODLoader::isModuleLoaded(const std::string& moduleName) const {
    return loadedModules_.find(moduleName) != loadedModules_.end();
}

std::vector<std::string> CMODLoader::getLoadedModules() const {
    std::vector<std::string> modules;
    for (const auto& [name, _] : loadedModules_) {
        modules.push_back(name);
    }
    return modules;
}

bool CMODLoader::extractCMOD(const std::string& cmodPath, std::string& extractPath) {
    // 创建缓存目录
    if (!fs::exists(config_.cacheDirectory)) {
        fs::create_directories(config_.cacheDirectory);
    }
    
    // 生成提取路径
    std::string moduleName = fs::path(cmodPath).stem().string();
    extractPath = fs::path(config_.cacheDirectory) / moduleName;
    
    // 如果已缓存且启用缓存，直接返回
    if (config_.cacheExtractedModules && fs::exists(extractPath)) {
        return true;
    }
    
    // 解压CMOD
    CMODPackager packager;
    if (!packager.unpack(cmodPath, extractPath)) {
        lastError_ = packager.getLastError();
        return false;
    }
    
    return true;
}

bool CMODLoader::loadExtractedModule(const std::string& extractPath, const std::string& moduleName) {
    // 检查是否已加载
    if (isModuleLoaded(moduleName)) {
        return true;
    }
    
    // 获取模块信息
    CMODPackager packager;
    auto infoPath = fs::path(extractPath) / "info" / "module.info";
    
    CMODInfo info;
    // 这里需要解析info文件
    // 暂时创建基本信息
    info.name = moduleName;
    
    // 处理依赖
    if (config_.autoExtractDependencies && !info.dependencies.empty()) {
        if (!processModuleDependencies(info)) {
            return false;
        }
    }
    
    // 加载源文件
    auto srcDir = fs::path(extractPath) / "src";
    if (fs::exists(srcDir)) {
        for (const auto& entry : fs::directory_iterator(srcDir)) {
            if (entry.path().extension() == ".chtl") {
                if (!processCHTLFile(entry.path().string())) {
                    return false;
                }
            }
        }
    }
    
    // 记录已加载模块
    LoadedModule module;
    module.info = info;
    module.sourcePath = extractPath;
    module.isCMOD = true;
    
    loadedModules_[moduleName] = module;
    
    return true;
}

bool CMODLoader::processCHTLFile(const std::string& chtlPath) {
    try {
        // 读取文件内容
        auto contentOpt = File::readToString(chtlPath);
        if (!contentOpt) {
            lastError_ = "Failed to read CHTL file: " + chtlPath;
            return false;
        }
        
        // 创建词法分析器
        auto lexer = std::make_shared<Lexer>(contentOpt.value(), context_);
        
        // 创建解析器
        Parser parser(lexer, context_);
        
        // 解析文件
        auto ast = parser.parse();
        if (!ast) {
            lastError_ = "Failed to parse CHTL file: " + chtlPath;
            return false;
        }
        
        // TODO: Store AST for later use
        // context_->addAST(chtlPath, ast);
        
        return true;
        
    } catch (const std::exception& e) {
        lastError_ = "Error processing CHTL file: " + std::string(e.what());
        return false;
    }
}

bool CMODLoader::processModuleDependencies(const CMODInfo& info) {
    // 解析依赖字符串
    std::vector<std::string> dependencies;
    std::stringstream ss(info.dependencies);
    std::string dep;
    
    while (std::getline(ss, dep, ',')) {
        // 去除空格
        dep.erase(0, dep.find_first_not_of(" \t"));
        dep.erase(dep.find_last_not_of(" \t") + 1);
        
        if (!dep.empty()) {
            dependencies.push_back(dep);
        }
    }
    
    // 加载每个依赖
    for (const auto& dependency : dependencies) {
        if (!loadModule(dependency)) {
            lastError_ = "Failed to load dependency: " + dependency;
            return false;
        }
    }
    
    return true;
}

std::string CMODLoader::resolveModulePath(const std::string& moduleName, ModuleSearchPath searchPath) {
    fs::path basePath;
    
    switch (searchPath) {
        case ModuleSearchPath::OFFICIAL:
            basePath = config_.officialModulePath;
            break;
        case ModuleSearchPath::LOCAL:
            basePath = fs::current_path() / "module";
            break;
        case ModuleSearchPath::CURRENT:
            basePath = fs::current_path();
            break;
        case ModuleSearchPath::ABSOLUTE:
            return moduleName;  // 已经是绝对路径
    }
    
    // 根据module文件夹结构类型处理
    if (searchPath == ModuleSearchPath::OFFICIAL || searchPath == ModuleSearchPath::LOCAL) {
        // 检查是否有分类结构
        auto cmodDir = basePath / "CMOD";
        auto cjmodDir = basePath / "CJMOD";
        
        if (fs::exists(cmodDir) || fs::exists(cjmodDir)) {
            // 分类结构
            if (fs::exists(cmodDir)) {
                // 先查找.cmod文件
                auto cmodPath = cmodDir / (moduleName + ".cmod");
                if (fs::exists(cmodPath)) {
                    return cmodPath.string();
                }
                
                // 再查找.chtl文件
                auto chtlPath = cmodDir / (moduleName + ".chtl");
                if (fs::exists(chtlPath)) {
                    return chtlPath.string();
                }
            }
        } else {
            // 混杂结构
            // 优先.cmod
            auto cmodPath = basePath / (moduleName + ".cmod");
            if (fs::exists(cmodPath)) {
                return cmodPath.string();
            }
            
            // 其次.chtl
            auto chtlPath = basePath / (moduleName + ".chtl");
            if (fs::exists(chtlPath)) {
                return chtlPath.string();
            }
        }
    } else {
        // 当前目录只查找.cmod和.chtl
        auto cmodPath = basePath / (moduleName + ".cmod");
        if (fs::exists(cmodPath)) {
            return cmodPath.string();
        }
        
        auto chtlPath = basePath / (moduleName + ".chtl");
        if (fs::exists(chtlPath)) {
            return chtlPath.string();
        }
    }
    
    return "";
}

std::vector<std::string> CMODLoader::getModuleSearchPaths(const std::string& moduleName) {
    std::vector<std::string> paths;
    
    // 检查是否是绝对路径
    if (fs::path(moduleName).is_absolute()) {
        paths.push_back(moduleName);
        return paths;
    }
    
    // 检查是否有官方模块前缀
    if (isOfficialModulePrefix(moduleName)) {
        // 去除前缀
        std::string realName = moduleName.substr(6);  // "chtl::"长度为6
        std::string path = resolveModulePath(realName, ModuleSearchPath::OFFICIAL);
        if (!path.empty()) {
            paths.push_back(path);
        }
        return paths;
    }
    
    // 按优先级搜索：官方模块 -> 本地module文件夹 -> 当前目录
    std::string path;
    
    // 1. 官方模块目录
    path = resolveModulePath(moduleName, ModuleSearchPath::OFFICIAL);
    if (!path.empty()) {
        paths.push_back(path);
    }
    
    // 2. 当前目录的module文件夹
    path = resolveModulePath(moduleName, ModuleSearchPath::LOCAL);
    if (!path.empty()) {
        paths.push_back(path);
    }
    
    // 3. 当前目录
    path = resolveModulePath(moduleName, ModuleSearchPath::CURRENT);
    if (!path.empty()) {
        paths.push_back(path);
    }
    
    return paths;
}

bool CMODLoader::isOfficialModulePrefix(const std::string& moduleName) {
    return moduleName.find("chtl::") == 0;
}

bool CMODLoader::cleanupCache() {
    if (fs::exists(config_.cacheDirectory)) {
        try {
            fs::remove_all(config_.cacheDirectory);
            return true;
        } catch (const std::exception& e) {
            lastError_ = "Failed to cleanup cache: " + std::string(e.what());
            return false;
        }
    }
    return true;
}

bool CMODLoader::validateModuleName(const std::string& name) {
    // 模块名规则：字母开头，可包含字母、数字、下划线、连字符
    if (name.empty()) return false;
    
    if (!std::isalpha(name[0])) return false;
    
    return std::all_of(name.begin(), name.end(), [](char c) {
        return std::isalnum(c) || c == '_' || c == '-';
    });
}

bool CMODLoader::checkCircularDependency(const std::string& moduleName) {
    return std::find(currentLoadingChain_.begin(), 
                     currentLoadingChain_.end(), 
                     moduleName) != currentLoadingChain_.end();
}

} // namespace CHTL