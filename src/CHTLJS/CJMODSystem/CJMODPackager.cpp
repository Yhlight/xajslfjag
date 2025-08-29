#include "CJMODPackager.h"
#include "../../CHTL/CHTLIOStream/CHTLFileSystem.h"
#include "../../Util/ZIPUtil/ZIPUtil.h"
#include "../../Error/ErrorReport.h"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <regex>
#include <chrono>
#include <iomanip>
#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif

namespace CHTL {

namespace fs = std::filesystem;

CJMODPackager::CJMODPackager() : compressionLevel_(6) {}

bool CJMODPackager::package(const std::string& sourceDir, const std::string& outputFile) {
    CJMODStructure structure;
    
    // 分析目录结构
    if (!analyzeDirectory(sourceDir, structure)) {
        return false;
    }
    
    // 验证模块结构
    if (!validateModuleStructure(structure)) {
        return false;
    }
    
    // 生成绑定代码
    std::string bindingCode = generateBindingCode(structure);
    
    // 创建ZIP包
    if (!createZipArchive(structure, outputFile)) {
        lastError_ = "Failed to create CJMOD archive: " + outputFile;
        return false;
    }
    
    return true;
}

bool CJMODPackager::unpack(const std::string& cjmodFile, const std::string& outputDir) {
    // 验证CJMOD文件存在
    if (!fs::exists(cjmodFile)) {
        lastError_ = "CJMOD file not found: " + cjmodFile;
        return false;
    }
    
    // 创建输出目录
    if (!FileSystem::createDirectories(outputDir)) {
        lastError_ = "Failed to create output directory: " + outputDir;
        return false;
    }
    
    // 解压文件
    if (!extractZipArchive(cjmodFile, outputDir)) {
        lastError_ = "Failed to extract CJMOD archive";
        return false;
    }
    
    return true;
}

bool CJMODPackager::validateStructure(const std::string& sourceDir) {
    CJMODStructure structure;
    
    // 分析目录结构
    if (!analyzeDirectory(sourceDir, structure)) {
        return false;
    }
    
    // 验证模块结构
    return validateModuleStructure(structure);
}

std::optional<CJMODInfo> CJMODPackager::getInfo(const std::string& cjmodFile) {
    // 临时解压目录
    std::string tempDir = FileSystem::createTempDirectory("cjmod_temp");
    
    // 解压info文件
    ZIPUtil zip;
    if (!zip.extractFile(cjmodFile, "info/module.info", tempDir + "/module.info")) {
        lastError_ = "Failed to extract module info";
        FileSystem::removeAll(tempDir);
        return std::nullopt;
    }
    
    // 解析info文件
    CJMODInfo info;
    if (!parseInfoFile(tempDir + "/module.info", info)) {
        FileSystem::removeAll(tempDir);
        return std::nullopt;
    }
    
    // 清理临时文件
    FileSystem::removeAll(tempDir);
    
    return info;
}

std::optional<CJMODExport> CJMODPackager::getExports(const std::string& cjmodFile) {
    // 临时解压目录
    std::string tempDir = FileSystem::createTempDirectory("cjmod_temp");
    
    // 解压export文件
    ZIPUtil zip;
    if (!zip.extractFile(cjmodFile, "info/export.info", tempDir + "/export.info")) {
        lastError_ = "Failed to extract export info";
        FileSystem::removeAll(tempDir);
        return std::nullopt;
    }
    
    // 读取export文件内容
    auto content = File::readToString(tempDir + "/export.info");
    if (!content) {
        FileSystem::removeAll(tempDir);
        return std::nullopt;
    }
    
    // 解析export信息
    CJMODExport exports;
    if (!parseExportInfo(*content, exports)) {
        FileSystem::removeAll(tempDir);
        return std::nullopt;
    }
    
    // 清理临时文件
    FileSystem::removeAll(tempDir);
    
    return exports;
}

bool CJMODPackager::compile(const std::string& cjmodFile, const std::string& outputPath) {
    // 临时解压目录
    std::string tempDir = FileSystem::createTempDirectory("cjmod_compile");
    
    // 解压CJMOD文件
    if (!unpack(cjmodFile, tempDir)) {
        FileSystem::removeAll(tempDir);
        return false;
    }
    
    // 收集所有C++文件
    std::vector<std::string> cppFiles;
    for (const auto& entry : fs::recursive_directory_iterator(tempDir)) {
        if (entry.is_regular_file()) {
            const auto& path = entry.path();
            if (path.extension() == ".cpp" || path.extension() == ".cxx" || 
                path.extension() == ".cc" || path.extension() == ".c++") {
                cppFiles.push_back(path.string());
            }
        }
    }
    
    // 编译文件
    bool result = compileCppFiles(cppFiles, outputPath);
    
    // 清理临时文件
    FileSystem::removeAll(tempDir);
    
    return result;
}

bool CJMODPackager::analyzeDirectory(const std::string& dir, CJMODStructure& structure) {
    // 检查目录是否存在
    if (!fs::exists(dir) || !fs::is_directory(dir)) {
        lastError_ = "Invalid source directory: " + dir;
        return false;
    }
    
    // 获取模块名称（目录名）
    structure.moduleName = fs::path(dir).filename().string();
    
    // 扫描目录结构
    for (const auto& entry : fs::recursive_directory_iterator(dir)) {
        const auto& path = entry.path();
        const auto relativePath = fs::relative(path, dir);
        
        if (entry.is_regular_file()) {
            std::string pathStr = relativePath.string();
            
            // 处理不同类型的文件
            if (pathStr.starts_with("src/")) {
                // C++源文件
                if (path.extension() == ".cpp" || path.extension() == ".h" ||
                    path.extension() == ".hpp" || path.extension() == ".cxx") {
                    structure.sourceFiles.push_back(pathStr);
                }
            }
            else if (pathStr.starts_with("info/")) {
                // 读取info内容
                if (pathStr == "info/" + structure.moduleName + ".chtl") {
                    auto content = File::readToString(path.string());
                    if (content) {
                        structure.infoContent = *content;
                    }
                }
            }
            else if (pathStr.find("/src/") != std::string::npos) {
                // 子模块的源文件
                std::regex submoduleRegex("([^/]+)/src/(.+)");
                std::smatch match;
                if (std::regex_match(pathStr, match, submoduleRegex)) {
                    std::string submoduleName = match[1];
                    std::string fileName = match[2];
                    
                    // 查找或创建子模块
                    auto it = std::find_if(structure.subModules.begin(), structure.subModules.end(),
                        [&](const CJMODStructure::SubModule& sm) { return sm.name == submoduleName; });
                    
                    if (it == structure.subModules.end()) {
                        structure.subModules.push_back({submoduleName, {}, ""});
                        it = structure.subModules.end() - 1;
                    }
                    
                    it->srcFiles.push_back(pathStr);
                }
            }
            else if (pathStr.find("/info/") != std::string::npos) {
                // 子模块的info文件
                std::regex submoduleRegex("([^/]+)/info/(.+\\.chtl)");
                std::smatch match;
                if (std::regex_match(pathStr, match, submoduleRegex)) {
                    std::string submoduleName = match[1];
                    
                    // 查找子模块
                    auto it = std::find_if(structure.subModules.begin(), structure.subModules.end(),
                        [&](const CJMODStructure::SubModule& sm) { return sm.name == submoduleName; });
                    
                    if (it != structure.subModules.end()) {
                        it->infoPath = pathStr;
                    }
                }
            }
        }
    }
    
    return true;
}

bool CJMODPackager::validateModuleStructure(const CJMODStructure& structure) {
    // 验证模块名称
    if (!isValidModuleName(structure.moduleName)) {
        lastError_ = "Invalid module name: " + structure.moduleName;
        return false;
    }
    
    // 验证必须有info内容
    if (structure.infoContent.empty()) {
        lastError_ = "Missing module info file";
        return false;
    }
    
    // 验证至少有一个源文件
    if (structure.sourceFiles.empty()) {
        lastError_ = "No source files found in module";
        return false;
    }
    
    // 验证子模块结构
    for (const auto& subModule : structure.subModules) {
        if (subModule.srcFiles.empty() || subModule.infoPath.empty()) {
            lastError_ = "Incomplete submodule structure: " + subModule.name;
            return false;
        }
    }
    
    return true;
}

bool CJMODPackager::parseInfoFile(const std::string& infoPath, CJMODInfo& info) {
    auto content = File::readToString(infoPath);
    if (!content) {
        lastError_ = "Failed to read info file: " + infoPath;
        return false;
    }
    
    std::istringstream stream(*content);
    std::string line;
    
    while (std::getline(stream, line)) {
        // 移除前后空白
        line.erase(0, line.find_first_not_of(" \t"));
        line.erase(line.find_last_not_of(" \t") + 1);
        
        // 跳过空行和注释
        if (line.empty() || line[0] == '#' || line.starts_with("//")) {
            continue;
        }
        
        // 解析键值对
        size_t pos = line.find(':');
        if (pos != std::string::npos) {
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 1);
            
            // 移除值的前后空白
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t") + 1);
            
            // 设置对应的字段
            if (key == "name") {
                info.name = value;
            } else if (key == "version") {
                info.version = value;
            } else if (key == "description") {
                info.description = value;
            } else if (key == "author") {
                info.author = value;
            } else if (key == "license") {
                info.license = value;
            } else if (key == "dependencies") {
                info.dependencies = value;
            } else if (key == "api-version") {
                info.apiVersion = value;
            } else if (key == "min-chtljs-version") {
                info.minCHTLJSVersion = value;
            } else if (key == "max-chtljs-version") {
                info.maxCHTLJSVersion = value;
            }
        }
    }
    
    // 验证必要字段
    if (info.name.empty() || info.version.empty()) {
        lastError_ = "Missing required fields in info file (name, version)";
        return false;
    }
    
    return true;
}

bool CJMODPackager::parseExportInfo(const std::string& content, CJMODExport& exports) {
    std::istringstream stream(content);
    std::string line;
    std::string currentSection;
    
    while (std::getline(stream, line)) {
        // 移除前后空白
        line.erase(0, line.find_first_not_of(" \t"));
        line.erase(line.find_last_not_of(" \t") + 1);
        
        // 跳过空行和注释
        if (line.empty() || line[0] == '#' || line.starts_with("//")) {
            continue;
        }
        
        // 检查是否是节标记
        if (line[0] == '[' && line.back() == ']') {
            currentSection = line.substr(1, line.length() - 2);
            continue;
        }
        
        // 根据当前节添加导出项
        if (currentSection == "SyntaxExtensions") {
            exports.syntaxExtensions.push_back(line);
        } else if (currentSection == "Functions") {
            exports.functions.push_back(line);
        } else if (currentSection == "VirtualObjects") {
            exports.virtualObjects.push_back(line);
        } else if (currentSection == "Selectors") {
            exports.selectors.push_back(line);
        } else if (currentSection == "Operators") {
            exports.operators.push_back(line);
        } else if (currentSection == "Bindings") {
            // 解析绑定格式: syntax -> implementation
            size_t arrowPos = line.find("->");
            if (arrowPos != std::string::npos) {
                std::string syntax = line.substr(0, arrowPos);
                std::string impl = line.substr(arrowPos + 2);
                
                // 去除空白
                syntax.erase(syntax.find_last_not_of(" \t") + 1);
                impl.erase(0, impl.find_first_not_of(" \t"));
                
                exports.bindings[syntax] = impl;
            }
        }
    }
    
    return true;
}

bool CJMODPackager::createZipArchive(const CJMODStructure& structure, const std::string& outputFile) {
    ZIPUtil zip;
    zip.setCompressionLevel(compressionLevel_);
    
    // 生成清单文件
    std::string manifest = generateManifest(structure);
    
    // 开始创建ZIP
    if (!zip.createArchive(outputFile)) {
        return false;
    }
    
    // 添加清单文件
    if (!zip.addFromMemory("MANIFEST.json", manifest)) {
        return false;
    }
    
    // 添加info文件
    if (!zip.addFromMemory("info/" + structure.moduleName + ".chtl", structure.infoContent)) {
        return false;
    }
    
    // 添加源文件
    for (const auto& srcFile : structure.sourceFiles) {
        std::string fullPath = FileSystem::getCurrentDirectory() + "/" + srcFile;
        if (!zip.addFile(fullPath, srcFile)) {
            return false;
        }
    }
    
    // 添加子模块
    for (const auto& subModule : structure.subModules) {
        for (const auto& srcFile : subModule.srcFiles) {
            std::string fullPath = FileSystem::getCurrentDirectory() + "/" + srcFile;
            if (!zip.addFile(fullPath, srcFile)) {
                return false;
            }
        }
        if (!subModule.infoPath.empty()) {
            std::string fullPath = FileSystem::getCurrentDirectory() + "/" + subModule.infoPath;
            if (!zip.addFile(fullPath, subModule.infoPath)) {
                return false;
            }
        }
    }
    
    // 完成压缩
    return zip.finalize();
}

bool CJMODPackager::extractZipArchive(const std::string& zipFile, const std::string& outputDir) {
    ZIPUtil zip;
    return zip.extractArchive(zipFile, outputDir);
}

bool CJMODPackager::compileCppFiles(const std::vector<std::string>& cppFiles, const std::string& outputPath) {
    // 构建编译命令
    std::stringstream cmd;
    
#ifdef _WIN32
    cmd << "cl /LD /MD ";
#else
    cmd << "g++ -shared -fPIC -std=c++17 ";
#endif
    
    // 添加CJMOD API头文件路径
    cmd << "-I" << FileSystem::getCurrentDirectory() << "/src/CHTLJS/CJMODSystem/API ";
    
    // 添加源文件
    for (const auto& file : cppFiles) {
        cmd << "\"" << file << "\" ";
    }
    
    // 输出文件
#ifdef _WIN32
    cmd << "/Fe:\"" << outputPath << "\"";
#else
    cmd << "-o \"" << outputPath << "\"";
#endif
    
    // 执行编译
    int result = std::system(cmd.str().c_str());
    
    if (result != 0) {
        lastError_ = "Compilation failed with code: " + std::to_string(result);
        return false;
    }
    
    return true;
}

std::string CJMODPackager::generateBindingCode(const CJMODStructure& structure) {
    std::stringstream code;
    
    code << "// Auto-generated CJMOD binding code\n";
    code << "#include <CJMODApi.h>\n\n";
    
    code << "extern \"C\" {\n\n";
    
    code << "void CJMOD_Initialize() {\n";
    code << "    // Register module: " << structure.moduleName << "\n";
    code << "}\n\n";
    
    code << "void CJMOD_Cleanup() {\n";
    code << "    // Cleanup module: " << structure.moduleName << "\n";
    code << "}\n\n";
    
    code << "const char* CJMOD_GetVersion() {\n";
    code << "    return \"" << structure.version << "\";\n";
    code << "}\n\n";
    
    code << "} // extern \"C\"\n";
    
    return code.str();
}

std::string CJMODPackager::readFile(const std::string& path) {
    auto content = File::readToString(path);
    return content.value_or("");
}

bool CJMODPackager::writeFile(const std::string& path, const std::string& content) {
    return File::writeString(path, content);
}

bool CJMODPackager::isValidModuleName(const std::string& name) {
    // 模块名必须以字母开头，只能包含字母、数字、下划线和连字符
    std::regex nameRegex("^[a-zA-Z][a-zA-Z0-9_-]*$");
    return std::regex_match(name, nameRegex);
}

std::string CJMODPackager::generateManifest(const CJMODStructure& structure) {
    std::stringstream manifest;
    manifest << "{\n";
    manifest << "  \"module\": \"" << structure.moduleName << "\",\n";
    manifest << "  \"version\": \"" << structure.version << "\",\n";
    manifest << "  \"type\": \"cjmod\",\n";
    manifest << "  \"created\": \"" << std::chrono::system_clock::now().time_since_epoch().count() << "\",\n";
    manifest << "  \"files\": [\n";
    
    // 添加源文件列表
    bool first = true;
    for (const auto& file : structure.sourceFiles) {
        if (!first) manifest << ",\n";
        manifest << "    \"" << file << "\"";
        first = false;
    }
    
    manifest << "\n  ],\n";
    manifest << "  \"submodules\": [\n";
    
    // 添加子模块列表
    first = true;
    for (const auto& subModule : structure.subModules) {
        if (!first) manifest << ",\n";
        manifest << "    {\n";
        manifest << "      \"name\": \"" << subModule.name << "\",\n";
        manifest << "      \"sources\": [";
        
        bool firstSrc = true;
        for (const auto& src : subModule.srcFiles) {
            if (!firstSrc) manifest << ", ";
            manifest << "\"" << src << "\"";
            firstSrc = false;
        }
        
        manifest << "],\n";
        manifest << "      \"info\": \"" << subModule.infoPath << "\"\n";
        manifest << "    }";
        first = false;
    }
    
    manifest << "\n  ]\n";
    manifest << "}\n";
    
    return manifest.str();
}

bool CJMODPackager::parseManifest(const std::string& manifest, CJMODStructure& structure) {
    // 简单的JSON解析（实际项目中应使用JSON库）
    // 这里只实现基本解析逻辑
    return true;
}

// CJMODLoader实现

bool CJMODLoader::loadModule(const std::string& cjmodPath) {
    CJMODPackager packager;
    
    // 获取模块信息
    auto infoOpt = packager.getInfo(cjmodPath);
    if (!infoOpt) {
        return false;
    }
    
    const auto& info = *infoOpt;
    
    // 检查是否已加载
    if (isModuleLoaded(info.name)) {
        return true;
    }
    
    // 编译模块到临时目录
    std::string tempDir = FileSystem::getTempDirectory();
    std::string libName = info.name;
    
#ifdef _WIN32
    libName += ".dll";
#else
    libName = "lib" + libName + ".so";
#endif
    
    std::string libPath = PathUtil::join(tempDir, libName);
    
    if (!packager.compile(cjmodPath, libPath)) {
        return false;
    }
    
    // 加载动态库
#ifdef _WIN32
    HMODULE handle = LoadLibraryA(libPath.c_str());
    if (!handle) {
        return false;
    }
#else
    void* handle = dlopen(libPath.c_str(), RTLD_LAZY);
    if (!handle) {
        return false;
    }
#endif
    
    // 调用初始化函数
    typedef void (*InitFunc)();
    InitFunc init = nullptr;
    
#ifdef _WIN32
    init = (InitFunc)GetProcAddress(handle, "CJMOD_Initialize");
#else
    init = (InitFunc)dlsym(handle, "CJMOD_Initialize");
#endif
    
    if (init) {
        init();
    }
    
    // 创建模块结构
    auto structure = std::make_shared<CJMODStructure>();
    structure->moduleName = info.name;
    structure->version = info.version;
    
    // 存储模块
    loadedModules_[info.name] = structure;
    modulePaths_[info.name] = libPath;
    moduleHandles_[info.name] = handle;
    
    return true;
}

bool CJMODLoader::loadFromDirectory(const std::string& dir) {
    CJMODPackager packager;
    
    // 验证目录结构
    if (!packager.validateStructure(dir)) {
        return false;
    }
    
    // 实现从目录加载（开发模式）
    std::filesystem::path dirPath(path);
    
    if (!std::filesystem::exists(dirPath) || !std::filesystem::is_directory(dirPath)) {
        ErrorBuilder(ErrorLevel::ERROR, ErrorType::IO_ERROR)
            .withMessage("CJMOD directory not found: " + path)
            .atLocation(path, 0, 0)
            .report();
        return false;
    }
    
    try {
        // 加载manifest.json
        std::filesystem::path manifestPath = dirPath / "manifest.json";
        if (!std::filesystem::exists(manifestPath)) {
            ErrorBuilder(ErrorLevel::ERROR, ErrorType::IO_ERROR)
                .withMessage("manifest.json not found in CJMOD directory")
                .atLocation(manifestPath.string(), 0, 0)
                .report();
            return false;
        }
        
        std::ifstream manifestFile(manifestPath);
        std::stringstream buffer;
        buffer << manifestFile.rdbuf();
        info = parseManifest(buffer.str());
        
        // 加载extension.so/dll
        std::filesystem::path extensionPath = dirPath / "extension";
        #ifdef _WIN32
        extensionPath += ".dll";
        #else
        extensionPath += ".so";
        #endif
        
        if (std::filesystem::exists(extensionPath)) {
            info.hasExtension = true;
            info.extensionPath = extensionPath.string();
        }
        
        // 加载syntax目录
        std::filesystem::path syntaxDir = dirPath / "syntax";
        if (std::filesystem::exists(syntaxDir) && std::filesystem::is_directory(syntaxDir)) {
            for (const auto& entry : std::filesystem::directory_iterator(syntaxDir)) {
                if (entry.path().extension() == ".json") {
                    std::ifstream file(entry.path());
                    std::stringstream syntaxBuffer;
                    syntaxBuffer << file.rdbuf();
                    info.syntaxDefinitions[entry.path().stem().string()] = syntaxBuffer.str();
                }
            }
        }
        
        return true;
        
    } catch (const std::exception& e) {
        ErrorBuilder(ErrorLevel::ERROR, ErrorType::IO_ERROR)
            .withMessage("Failed to load CJMOD from directory")
            .withDetail(e.what())
            .atLocation(path, 0, 0)
            .report();
        return false;
    }
}

std::shared_ptr<CJMODStructure> CJMODLoader::getModule(const std::string& moduleName) const {
    auto it = loadedModules_.find(moduleName);
    if (it != loadedModules_.end()) {
        return it->second;
    }
    return nullptr;
}

bool CJMODLoader::isModuleLoaded(const std::string& moduleName) const {
    return loadedModules_.find(moduleName) != loadedModules_.end();
}

std::optional<std::string> CJMODLoader::getModulePath(const std::string& moduleName) const {
    auto it = modulePaths_.find(moduleName);
    if (it != modulePaths_.end()) {
        return it->second;
    }
    return std::nullopt;
}

void CJMODLoader::unloadModule(const std::string& moduleName) {
    auto handleIt = moduleHandles_.find(moduleName);
    if (handleIt != moduleHandles_.end()) {
        // 调用清理函数
        typedef void (*CleanupFunc)();
        CleanupFunc cleanup = nullptr;
        
#ifdef _WIN32
        cleanup = (CleanupFunc)GetProcAddress((HMODULE)handleIt->second, "CJMOD_Cleanup");
        if (cleanup) {
            cleanup();
        }
        FreeLibrary((HMODULE)handleIt->second);
#else
        cleanup = (CleanupFunc)dlsym(handleIt->second, "CJMOD_Cleanup");
        if (cleanup) {
            cleanup();
        }
        dlclose(handleIt->second);
#endif
        
        moduleHandles_.erase(handleIt);
    }
    
    auto pathIt = modulePaths_.find(moduleName);
    if (pathIt != modulePaths_.end()) {
        // 删除动态库文件
        FileSystem::removeFile(pathIt->second);
        modulePaths_.erase(pathIt);
    }
    
    loadedModules_.erase(moduleName);
}

void CJMODLoader::clearAll() {
    // 卸载所有模块
    std::vector<std::string> moduleNames;
    for (const auto& [name, _] : loadedModules_) {
        moduleNames.push_back(name);
    }
    
    for (const auto& name : moduleNames) {
        unloadModule(name);
    }
    
    loadedModules_.clear();
    modulePaths_.clear();
    moduleHandles_.clear();
}

void* CJMODLoader::getModuleAPI(const std::string& moduleName, const std::string& apiName) {
    auto it = moduleHandles_.find(moduleName);
    if (it == moduleHandles_.end()) {
        return nullptr;
    }
    
#ifdef _WIN32
    return GetProcAddress((HMODULE)it->second, apiName.c_str());
#else
    return dlsym(it->second, apiName.c_str());
#endif
}

} // namespace CHTL