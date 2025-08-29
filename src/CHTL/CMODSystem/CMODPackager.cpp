#include "CMODPackager.h"
#include "../CHTLIOStream/CHTLFileSystem.h"
#include "../../Util/ZIPUtil/ZIPUtil.h"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <regex>
#include <chrono>
#include <iomanip>

namespace CHTL {

namespace fs = std::filesystem;

CMODPackager::CMODPackager() : compressionLevel_(6) {}

bool CMODPackager::package(const std::string& sourceDir, const std::string& outputFile) {
    CMODStructure structure;
    
    // 分析目录结构
    if (!analyzeDirectory(sourceDir, structure)) {
        return false;
    }
    
    // 验证模块结构
    if (!validateModuleStructure(structure)) {
        return false;
    }
    
    // 创建ZIP包
    if (!createZipArchive(structure, outputFile)) {
        lastError_ = "Failed to create CMOD archive: " + outputFile;
        return false;
    }
    
    return true;
}

bool CMODPackager::unpack(const std::string& cmodFile, const std::string& outputDir) {
    // 验证CMOD文件存在
    if (!fs::exists(cmodFile)) {
        lastError_ = "CMOD file not found: " + cmodFile;
        return false;
    }
    
    // 创建输出目录
    if (!createDirectory(outputDir)) {
        lastError_ = "Failed to create output directory: " + outputDir;
        return false;
    }
    
    // 解压文件
    if (!extractZipArchive(cmodFile, outputDir)) {
        lastError_ = "Failed to extract CMOD archive";
        return false;
    }
    
    return true;
}

bool CMODPackager::validateStructure(const std::string& sourceDir) {
    CMODStructure structure;
    
    // 分析目录结构
    if (!analyzeDirectory(sourceDir, structure)) {
        return false;
    }
    
    // 验证模块结构
    return validateModuleStructure(structure);
}

std::optional<CMODInfo> CMODPackager::getInfo(const std::string& cmodFile) {
    // 临时解压目录
    std::string tempDir = fs::temp_directory_path() / "cmod_temp";
    createDirectory(tempDir);
    
    // 解压info文件
    ZIPUtil zip;
    if (!zip.extractFile(cmodFile, "info/module.info", tempDir + "/module.info")) {
        lastError_ = "Failed to extract module info";
        fs::remove_all(tempDir);
        return std::nullopt;
    }
    
    // 解析info文件
    CMODInfo info;
    if (!parseInfoFile(tempDir + "/module.info", info)) {
        fs::remove_all(tempDir);
        return std::nullopt;
    }
    
    // 清理临时文件
    fs::remove_all(tempDir);
    
    return info;
}

std::optional<CMODExport> CMODPackager::getExports(const std::string& cmodFile) {
    // 临时解压目录
    std::string tempDir = fs::temp_directory_path() / "cmod_temp";
    createDirectory(tempDir);
    
    // 解压export文件
    ZIPUtil zip;
    if (!zip.extractFile(cmodFile, "info/export.info", tempDir + "/export.info")) {
        lastError_ = "Failed to extract export info";
        fs::remove_all(tempDir);
        return std::nullopt;
    }
    
    // 读取export文件内容
    std::string content = readFile(tempDir + "/export.info");
    if (content.empty()) {
        fs::remove_all(tempDir);
        return std::nullopt;
    }
    
    // 解析export信息
    CMODExport exports;
    if (!parseExportInfo(content, exports)) {
        fs::remove_all(tempDir);
        return std::nullopt;
    }
    
    // 清理临时文件
    fs::remove_all(tempDir);
    
    return exports;
}

bool CMODPackager::analyzeDirectory(const std::string& dir, CMODStructure& structure) {
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
                structure.sourceFiles.push_back(pathStr);
                
                // 检查主模块文件
                if (pathStr == "src/" + structure.moduleName + ".chtl") {
                    structure.mainModuleFile = pathStr;
                }
            }
            else if (pathStr.starts_with("info/")) {
                // 读取info内容
                if (pathStr == "info/module.info") {
                    structure.infoContent = readFile(path.string());
                }
            }
            else if (pathStr.starts_with("submodules/")) {
                // 处理子模块
                std::regex submoduleRegex("submodules/([^/]+)/(src|info)/(.+)");
                std::smatch match;
                if (std::regex_match(pathStr, match, submoduleRegex)) {
                    std::string submoduleName = match[1];
                    std::string subDir = match[2];
                    
                    // 查找或创建子模块
                    auto it = std::find_if(structure.subModules.begin(), structure.subModules.end(),
                        [&](const CMODStructure::SubModule& sm) { return sm.name == submoduleName; });
                    
                    if (it == structure.subModules.end()) {
                        structure.subModules.push_back({submoduleName, "", ""});
                        it = structure.subModules.end() - 1;
                    }
                    
                    if (subDir == "src") {
                        it->srcPath = pathStr;
                    } else if (subDir == "info") {
                        it->infoPath = pathStr;
                    }
                }
            }
        }
    }
    
    return true;
}

bool CMODPackager::validateModuleStructure(const CMODStructure& structure) {
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
        if (subModule.srcPath.empty() || subModule.infoPath.empty()) {
            lastError_ = "Incomplete submodule structure: " + subModule.name;
            return false;
        }
    }
    
    return true;
}

bool CMODPackager::parseInfoFile(const std::string& infoPath, CMODInfo& info) {
    std::ifstream file(infoPath);
    if (!file.is_open()) {
        lastError_ = "Failed to open info file: " + infoPath;
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        // 移除前后空白
        line.erase(0, line.find_first_not_of(" \t"));
        line.erase(line.find_last_not_of(" \t") + 1);
        
        // 跳过空行和注释
        if (line.empty() || line[0] == '#') {
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
            } else if (key == "category") {
                info.category = value;
            } else if (key == "min-chtl-version") {
                info.minCHTLVersion = value;
            } else if (key == "max-chtl-version") {
                info.maxCHTLVersion = value;
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

bool CMODPackager::parseExportInfo(const std::string& content, CMODExport& exports) {
    std::istringstream stream(content);
    std::string line;
    std::string currentSection;
    
    while (std::getline(stream, line)) {
        // 移除前后空白
        line.erase(0, line.find_first_not_of(" \t"));
        line.erase(line.find_last_not_of(" \t") + 1);
        
        // 跳过空行和注释
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        // 检查是否是节标记
        if (line[0] == '[' && line.back() == ']') {
            currentSection = line.substr(1, line.length() - 2);
            continue;
        }
        
        // 根据当前节添加导出项
        if (currentSection == "CustomStyles") {
            exports.customStyles.push_back(line);
        } else if (currentSection == "CustomElements") {
            exports.customElements.push_back(line);
        } else if (currentSection == "CustomVars") {
            exports.customVars.push_back(line);
        } else if (currentSection == "TemplateStyles") {
            exports.templateStyles.push_back(line);
        } else if (currentSection == "TemplateElements") {
            exports.templateElements.push_back(line);
        } else if (currentSection == "TemplateVars") {
            exports.templateVars.push_back(line);
        } else if (currentSection == "OriginHtml") {
            exports.originHtml.push_back(line);
        } else if (currentSection == "OriginStyle") {
            exports.originStyle.push_back(line);
        } else if (currentSection == "OriginJavascript") {
            exports.originJavascript.push_back(line);
        } else if (currentSection == "Configurations") {
            exports.configurations.push_back(line);
        } else if (currentSection.starts_with("Origin:")) {
            // 自定义Origin类型
            std::string originType = currentSection.substr(7);
            exports.customOriginTypes[originType].push_back(line);
        }
    }
    
    return true;
}

bool CMODPackager::createZipArchive(const CMODStructure& structure, const std::string& outputFile) {
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
    if (!zip.addFromMemory("info/module.info", structure.infoContent)) {
        return false;
    }
    
    // 添加源文件
    for (const auto& srcFile : structure.sourceFiles) {
        std::string fullPath = fs::current_path() / srcFile;
        if (!zip.addFile(fullPath, srcFile)) {
            return false;
        }
    }
    
    // 添加子模块
    for (const auto& subModule : structure.subModules) {
        if (!subModule.srcPath.empty()) {
            std::string fullPath = fs::current_path() / subModule.srcPath;
            if (!zip.addFile(fullPath, subModule.srcPath)) {
                return false;
            }
        }
        if (!subModule.infoPath.empty()) {
            std::string fullPath = fs::current_path() / subModule.infoPath;
            if (!zip.addFile(fullPath, subModule.infoPath)) {
                return false;
            }
        }
    }
    
    // 完成压缩
    return zip.finalize();
}

bool CMODPackager::extractZipArchive(const std::string& zipFile, const std::string& outputDir) {
    ZIPUtil zip;
    return zip.extractArchive(zipFile, outputDir);
}

std::string CMODPackager::readFile(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) {
        return "";
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

bool CMODPackager::writeFile(const std::string& path, const std::string& content) {
    // 创建父目录
    fs::path filePath(path);
    if (filePath.has_parent_path()) {
        fs::create_directories(filePath.parent_path());
    }
    
    std::ofstream file(path, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }
    
    file << content;
    return file.good();
}

bool CMODPackager::copyFile(const std::string& src, const std::string& dst) {
    try {
        fs::copy_file(src, dst, fs::copy_options::overwrite_existing);
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

bool CMODPackager::createDirectory(const std::string& path) {
    try {
        fs::create_directories(path);
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

bool CMODPackager::isValidModuleName(const std::string& name) {
    // 模块名必须以字母开头，只能包含字母、数字、下划线和连字符
    std::regex nameRegex("^[a-zA-Z][a-zA-Z0-9_-]*$");
    return std::regex_match(name, nameRegex);
}

std::string CMODPackager::generateManifest(const CMODStructure& structure) {
    std::stringstream manifest;
    manifest << "{\n";
    manifest << "  \"module\": \"" << structure.moduleName << "\",\n";
    manifest << "  \"version\": \"" << structure.version << "\",\n";
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
        manifest << "      \"src\": \"" << subModule.srcPath << "\",\n";
        manifest << "      \"info\": \"" << subModule.infoPath << "\"\n";
        manifest << "    }";
        first = false;
    }
    
    manifest << "\n  ]\n";
    manifest << "}\n";
    
    return manifest.str();
}

bool CMODPackager::parseManifest(const std::string& manifest, CMODStructure& structure) {
    // 简单的JSON解析（实际项目中应使用JSON库）
    // 这里只实现基本解析逻辑
    return true;
}

// CMODLoader实现

bool CMODLoader::loadModule(const std::string& cmodFile) {
    CMODPackager packager;
    
    // 获取模块信息
    auto infoOpt = packager.getInfo(cmodFile);
    if (!infoOpt) {
        return false;
    }
    
    const auto& info = *infoOpt;
    
    // 检查是否已加载
    if (isModuleLoaded(info.name)) {
        return true;
    }
    
    // 解压到临时目录
    std::string tempDir = fs::temp_directory_path() / ("cmod_" + info.name);
    if (!packager.unpack(cmodFile, tempDir)) {
        return false;
    }
    
    // 创建模块结构
    auto structure = std::make_shared<CMODStructure>();
    if (!packager.analyzeDirectory(tempDir, *structure)) {
        fs::remove_all(tempDir);
        return false;
    }
    
    // 存储模块
    loadedModules_[info.name] = structure;
    modulePaths_[info.name] = tempDir;
    
    return true;
}

bool CMODLoader::loadFromDirectory(const std::string& dir) {
    CMODPackager packager;
    
    // 验证目录结构
    if (!packager.validateStructure(dir)) {
        return false;
    }
    
    // 创建模块结构
    auto structure = std::make_shared<CMODStructure>();
    if (!packager.analyzeDirectory(dir, *structure)) {
        return false;
    }
    
    // 检查是否已加载
    if (isModuleLoaded(structure->moduleName)) {
        return true;
    }
    
    // 存储模块
    loadedModules_[structure->moduleName] = structure;
    modulePaths_[structure->moduleName] = dir;
    
    return true;
}

std::shared_ptr<CMODStructure> CMODLoader::getModule(const std::string& moduleName) const {
    auto it = loadedModules_.find(moduleName);
    if (it != loadedModules_.end()) {
        return it->second;
    }
    return nullptr;
}

bool CMODLoader::isModuleLoaded(const std::string& moduleName) const {
    return loadedModules_.find(moduleName) != loadedModules_.end();
}

std::optional<std::string> CMODLoader::getModulePath(const std::string& moduleName) const {
    auto it = modulePaths_.find(moduleName);
    if (it != modulePaths_.end()) {
        return it->second;
    }
    return std::nullopt;
}

void CMODLoader::unloadModule(const std::string& moduleName) {
    auto it = modulePaths_.find(moduleName);
    if (it != modulePaths_.end()) {
        // 如果是临时目录，删除它
        if (it->second.find("cmod_") != std::string::npos) {
            fs::remove_all(it->second);
        }
        modulePaths_.erase(it);
    }
    
    loadedModules_.erase(moduleName);
}

void CMODLoader::clearAll() {
    // 清理所有临时目录
    for (const auto& [name, path] : modulePaths_) {
        if (path.find("cmod_") != std::string::npos) {
            fs::remove_all(path);
        }
    }
    
    loadedModules_.clear();
    modulePaths_.clear();
}

} // namespace CHTL