#include "ModuleManager.h"
#include "../../Error/ErrorReporter.h"
#include <iostream>
#include <fstream>
#include <algorithm>

namespace CHTL {

ModuleManager::ModuleManager() {
    // 初始化官方模块目录
    m_officialModuleDirectory = "./module";  // 默认官方模块目录
    
    // 添加搜索路径
    m_modulePaths.push_back(m_officialModuleDirectory);
}

ModuleManager::~ModuleManager() = default;

bool ModuleManager::loadModule(const std::string& modulePath) {
    try {
        if (!directoryExists(modulePath)) {
            reportError("模块路径不存在: " + modulePath);
            return false;
        }
        
        ModuleInfo moduleInfo;
        moduleInfo.path = modulePath;
        moduleInfo.name = getBaseName(modulePath);
        
        // 检测目录结构
        moduleInfo.structure = detectDirectoryStructure(modulePath);
        
        // 验证模块结构
        if (!validateModuleStructure(modulePath, ModuleType::CMOD)) {
            // 尝试作为CJMOD加载
            if (!validateModuleStructure(modulePath, ModuleType::CJMOD)) {
                // 尝试作为混合模块加载
                if (!loadHybridModule(modulePath)) {
                    reportError("无效的模块结构: " + modulePath);
                    return false;
                }
                moduleInfo.type = ModuleType::HYBRID;
            } else {
                moduleInfo.type = ModuleType::CJMOD;
            }
        } else {
            moduleInfo.type = ModuleType::CMOD;
        }
        
        // 解析模块信息
        std::string infoPath = modulePath + "/info/" + moduleInfo.name + ".chtl";
        if (!parseModuleInfo(infoPath, moduleInfo)) {
            reportError("无法解析模块信息: " + infoPath);
            return false;
        }
        
        // 扫描子模块
        moduleInfo.subModules = scanSubModules(modulePath);
        
        // 注册模块
        m_loadedModules[moduleInfo.name] = moduleInfo;
        
        std::cout << "✓ 成功加载模块: " << moduleInfo.name << " (类型: " << 
                     (moduleInfo.type == ModuleType::CMOD ? "CMOD" : 
                      moduleInfo.type == ModuleType::CJMOD ? "CJMOD" : "HYBRID") << ")" << std::endl;
        
        return true;
        
    } catch (const std::exception& e) {
        reportError("加载模块时发生异常: " + std::string(e.what()));
        return false;
    }
}

DirectoryStructure ModuleManager::detectDirectoryStructure(const std::string& directoryPath) {
    // 检查是否存在分类文件夹
    if (detectCMODFolder(directoryPath) || detectCJMODFolder(directoryPath)) {
        return DirectoryStructure::CLASSIFIED;
    }
    
    // 检查是否为混杂结构
    auto files = listFilesInDirectory(directoryPath);
    bool hasCMOD = false, hasCJMOD = false, hasCHTL = false;
    
    for (const auto& file : files) {
        std::string ext = getFileExtension(file);
        if (ext == ".cmod") hasCMOD = true;
        else if (ext == ".cjmod") hasCJMOD = true;
        else if (ext == ".chtl") hasCHTL = true;
    }
    
    if ((hasCMOD || hasCHTL) && hasCJMOD) {
        return DirectoryStructure::MIXED;
    }
    
    return DirectoryStructure::MIXED;  // 默认混杂结构
}

bool ModuleManager::detectCMODFolder(const std::string& path) {
    std::vector<std::string> cmodVariants = {"CMOD", "cmod", "Cmod"};
    
    for (const auto& variant : cmodVariants) {
        if (directoryExists(path + "/" + variant)) {
            return true;
        }
    }
    
    return false;
}

bool ModuleManager::detectCJMODFolder(const std::string& path) {
    std::vector<std::string> cjmodVariants = {"CJMOD", "cjmod", "CJmod"};
    
    for (const auto& variant : cjmodVariants) {
        if (directoryExists(path + "/" + variant)) {
            return true;
        }
    }
    
    return false;
}

std::string ModuleManager::normalizeFolderName(const std::string& folderName, ModuleType type) {
    if (type == ModuleType::CMOD) {
        if (folderName == "cmod" || folderName == "Cmod") return "CMOD";
        return folderName;
    } else if (type == ModuleType::CJMOD) {
        if (folderName == "cjmod" || folderName == "CJmod") return "CJMOD";
        return folderName;
    }
    
    return folderName;
}

bool ModuleManager::scanMixedStructure(const std::string& modulePath, std::vector<ModuleFile>& files) {
    try {
        auto fileList = listFilesInDirectory(modulePath);
        
        for (const auto& file : fileList) {
            ModuleFile moduleFile;
            moduleFile.name = getBaseName(file);
            moduleFile.path = modulePath + "/" + file;
            moduleFile.extension = getFileExtension(file);
            
            if (moduleFile.extension == ".chtl") {
                moduleFile.type = ModuleType::CHTL;
            } else if (moduleFile.extension == ".cmod") {
                moduleFile.type = ModuleType::CMOD;
            } else if (moduleFile.extension == ".cjmod") {
                moduleFile.type = ModuleType::CJMOD;
            } else {
                continue;  // 跳过不相关文件
            }
            
            files.push_back(moduleFile);
        }
        
        std::cout << "  ✓ 混杂结构扫描完成，发现 " << files.size() << " 个模块文件" << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        reportError("扫描混杂结构时发生错误: " + std::string(e.what()));
        return false;
    }
}

bool ModuleManager::scanClassifiedStructure(const std::string& modulePath, std::vector<ModuleFile>& files) {
    try {
        // 扫描CMOD文件夹
        std::vector<std::string> cmodVariants = {"CMOD", "cmod", "Cmod"};
        for (const auto& variant : cmodVariants) {
            std::string cmodPath = modulePath + "/" + variant;
            if (directoryExists(cmodPath)) {
                auto cmodFiles = listFilesInDirectory(cmodPath);
                for (const auto& file : cmodFiles) {
                    std::string ext = getFileExtension(file);
                    if (ext == ".chtl" || ext == ".cmod") {
                        ModuleFile moduleFile;
                        moduleFile.name = getBaseName(file);
                        moduleFile.path = cmodPath + "/" + file;
                        moduleFile.extension = ext;
                        moduleFile.type = (ext == ".cmod") ? ModuleType::CMOD : ModuleType::CHTL;
                        files.push_back(moduleFile);
                    }
                }
                break;
            }
        }
        
        // 扫描CJMOD文件夹
        std::vector<std::string> cjmodVariants = {"CJMOD", "cjmod", "CJmod"};
        for (const auto& variant : cjmodVariants) {
            std::string cjmodPath = modulePath + "/" + variant;
            if (directoryExists(cjmodPath)) {
                auto cjmodFiles = listFilesInDirectory(cjmodPath);
                for (const auto& file : cjmodFiles) {
                    std::string ext = getFileExtension(file);
                    if (ext == ".cjmod") {
                        ModuleFile moduleFile;
                        moduleFile.name = getBaseName(file);
                        moduleFile.path = cjmodPath + "/" + file;
                        moduleFile.extension = ext;
                        moduleFile.type = ModuleType::CJMOD;
                        files.push_back(moduleFile);
                    }
                }
                break;
            }
        }
        
        std::cout << "  ✓ 分类结构扫描完成，发现 " << files.size() << " 个模块文件" << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        reportError("扫描分类结构时发生错误: " + std::string(e.what()));
        return false;
    }
}

std::string ModuleManager::getOfficialModuleDirectory() {
    return m_officialModuleDirectory;
}

bool ModuleManager::searchInOfficialModules(const std::string& moduleName, std::string& foundPath) {
    std::string officialPath = getOfficialModuleDirectory();
    
    // 检测官方模块目录结构
    DirectoryStructure structure = detectDirectoryStructure(officialPath);
    
    if (structure == DirectoryStructure::CLASSIFIED) {
        // 在分类结构中搜索
        std::vector<std::string> cmodVariants = {"CMOD", "cmod", "Cmod"};
        for (const auto& variant : cmodVariants) {
            std::string searchPath = officialPath + "/" + variant + "/" + moduleName;
            if (directoryExists(searchPath + ".cmod") || fileExists(searchPath + ".cmod")) {
                foundPath = searchPath + ".cmod";
                return true;
            }
            if (directoryExists(searchPath + ".chtl") || fileExists(searchPath + ".chtl")) {
                foundPath = searchPath + ".chtl";
                return true;
            }
        }
        
        std::vector<std::string> cjmodVariants = {"CJMOD", "cjmod", "CJmod"};
        for (const auto& variant : cjmodVariants) {
            std::string searchPath = officialPath + "/" + variant + "/" + moduleName;
            if (directoryExists(searchPath + ".cjmod") || fileExists(searchPath + ".cjmod")) {
                foundPath = searchPath + ".cjmod";
                return true;
            }
        }
    } else {
        // 在混杂结构中搜索
        if (fileExists(officialPath + "/" + moduleName + ".cmod")) {
            foundPath = officialPath + "/" + moduleName + ".cmod";
            return true;
        }
        if (fileExists(officialPath + "/" + moduleName + ".chtl")) {
            foundPath = officialPath + "/" + moduleName + ".chtl";
            return true;
        }
        if (fileExists(officialPath + "/" + moduleName + ".cjmod")) {
            foundPath = officialPath + "/" + moduleName + ".cjmod";
            return true;
        }
    }
    
    return false;
}

bool ModuleManager::isOfficialModulePrefix(const std::string& moduleName) {
    return moduleName.find("chtl::") == 0;
}

std::string ModuleManager::removeOfficialPrefix(const std::string& moduleName) {
    if (isOfficialModulePrefix(moduleName)) {
        return moduleName.substr(6);  // 移除"chtl::"前缀
    }
    return moduleName;
}

bool ModuleManager::resolveModulePath(const std::string& moduleName, std::string& resolvedPath) {
    std::string actualModuleName = moduleName;
    
    // 处理官方模块前缀
    if (isOfficialModulePrefix(moduleName)) {
        actualModuleName = removeOfficialPrefix(moduleName);
        return searchInOfficialModules(actualModuleName, resolvedPath);
    }
    
    // 按语法文档要求的搜索顺序：官方模块目录 → 当前目录module文件夹 → 当前目录
    std::vector<std::string> searchPaths = {
        getOfficialModuleDirectory(),
        "./module",
        "."
    };
    
    for (const auto& searchPath : searchPaths) {
        if (!directoryExists(searchPath)) continue;
        
        DirectoryStructure structure = detectDirectoryStructure(searchPath);
        
        if (structure == DirectoryStructure::CLASSIFIED) {
            // 在分类结构中搜索，优先CMOD
            std::vector<std::string> cmodVariants = {"CMOD", "cmod", "Cmod"};
            for (const auto& variant : cmodVariants) {
                std::string cmodPath = searchPath + "/" + variant;
                if (directoryExists(cmodPath)) {
                    if (fileExists(cmodPath + "/" + actualModuleName + ".cmod")) {
                        resolvedPath = cmodPath + "/" + actualModuleName + ".cmod";
                        return true;
                    }
                    if (fileExists(cmodPath + "/" + actualModuleName + ".chtl")) {
                        resolvedPath = cmodPath + "/" + actualModuleName + ".chtl";
                        return true;
                    }
                }
            }
        } else {
            // 在混杂结构中搜索，优先CMOD
            if (fileExists(searchPath + "/" + actualModuleName + ".cmod")) {
                resolvedPath = searchPath + "/" + actualModuleName + ".cmod";
                return true;
            }
            if (fileExists(searchPath + "/" + actualModuleName + ".chtl")) {
                resolvedPath = searchPath + "/" + actualModuleName + ".chtl";
                return true;
            }
        }
    }
    
    reportError("无法找到模块: " + moduleName);
    return false;
}

std::vector<SubModuleInfo> ModuleManager::scanSubModules(const std::string& modulePath) {
    std::vector<SubModuleInfo> subModules;
    
    try {
        std::string srcPath = modulePath + "/src";
        if (!directoryExists(srcPath)) {
            return subModules;
        }
        
        // 扫描src目录中的子目录
        for (const auto& entry : std::filesystem::directory_iterator(srcPath)) {
            if (entry.is_directory()) {
                std::string subModuleName = entry.path().filename().string();
                std::string subModulePath = entry.path().string();
                
                // 检查是否为有效子模块（包含src和info文件夹）
                if (directoryExists(subModulePath + "/src") && directoryExists(subModulePath + "/info")) {
                    SubModuleInfo subModule;
                    subModule.name = subModuleName;
                    subModule.path = subModulePath;
                    
                    // 检测子模块类型
                    auto files = listFilesInDirectory(subModulePath + "/src");
                    bool hasCPP = false, hasCHTL = false;
                    
                    for (const auto& file : files) {
                        std::string ext = getFileExtension(file);
                        if (ext == ".cpp" || ext == ".h") hasCPP = true;
                        else if (ext == ".chtl") hasCHTL = true;
                    }
                    
                    if (hasCPP) {
                        subModule.type = ModuleType::CJMOD;
                    } else if (hasCHTL) {
                        subModule.type = ModuleType::CMOD;
                    } else {
                        subModule.type = ModuleType::CHTL;
                    }
                    
                    subModule.files = files;
                    subModules.push_back(subModule);
                }
            }
        }
        
        if (!subModules.empty()) {
            std::cout << "  ✓ 发现 " << subModules.size() << " 个子模块" << std::endl;
        }
        
    } catch (const std::exception& e) {
        reportError("扫描子模块时发生错误: " + std::string(e.what()));
    }
    
    return subModules;
}

bool ModuleManager::loadHybridModule(const std::string& modulePath) {
    std::string cmodPath, cjmodPath;
    
    if (!separateCMODAndCJMOD(modulePath, cmodPath, cjmodPath)) {
        return false;
    }
    
    // 验证CMOD部分
    if (!cmodPath.empty() && !validateModuleStructure(cmodPath, ModuleType::CMOD)) {
        reportError("混合模块的CMOD部分结构无效: " + cmodPath);
        return false;
    }
    
    // 验证CJMOD部分
    if (!cjmodPath.empty() && !validateModuleStructure(cjmodPath, ModuleType::CJMOD)) {
        reportError("混合模块的CJMOD部分结构无效: " + cjmodPath);
        return false;
    }
    
    std::cout << "  ✓ 混合模块验证通过" << std::endl;
    return true;
}

bool ModuleManager::separateCMODAndCJMOD(const std::string& hybridPath, std::string& cmodPath, std::string& cjmodPath) {
    // 查找CMOD文件夹
    std::vector<std::string> cmodVariants = {"CMOD", "cmod", "Cmod"};
    for (const auto& variant : cmodVariants) {
        std::string path = hybridPath + "/" + variant;
        if (directoryExists(path)) {
            cmodPath = path;
            break;
        }
    }
    
    // 查找CJMOD文件夹
    std::vector<std::string> cjmodVariants = {"CJMOD", "cjmod", "CJmod"};
    for (const auto& variant : cjmodVariants) {
        std::string path = hybridPath + "/" + variant;
        if (directoryExists(path)) {
            cjmodPath = path;
            break;
        }
    }
    
    return !cmodPath.empty() || !cjmodPath.empty();
}

bool ModuleManager::validateModuleStructure(const std::string& modulePath, ModuleType expectedType) {
    // 检查基本结构：src和info文件夹
    if (!directoryExists(modulePath + "/src") || !directoryExists(modulePath + "/info")) {
        return false;
    }
    
    // 验证同名约束
    if (!validateSameNameConstraint(modulePath)) {
        return false;
    }
    
    // 验证info文件
    std::string moduleName = getBaseName(modulePath);
    std::string infoFile = modulePath + "/info/" + moduleName + ".chtl";
    if (!validateInfoFile(infoFile)) {
        return false;
    }
    
    // 根据类型验证src内容
    std::string srcPath = modulePath + "/src";
    auto files = listFilesInDirectory(srcPath);
    
    if (expectedType == ModuleType::CMOD) {
        // CMOD应包含.chtl文件
        bool hasMainFile = fileExists(srcPath + "/" + moduleName + ".chtl");
        bool hasSubModules = !scanSubModules(modulePath).empty();
        
        if (!hasMainFile && !hasSubModules) {
            reportError("CMOD模块缺少主模块文件或子模块: " + modulePath);
            return false;
        }
    } else if (expectedType == ModuleType::CJMOD) {
        // CJMOD应包含.cpp和.h文件
        bool hasCPP = false;
        for (const auto& file : files) {
            std::string ext = getFileExtension(file);
            if (ext == ".cpp" || ext == ".h") {
                hasCPP = true;
                break;
            }
        }
        
        if (!hasCPP) {
            reportError("CJMOD模块缺少C++源文件: " + modulePath);
            return false;
        }
    }
    
    return true;
}

bool ModuleManager::validateSameNameConstraint(const std::string& modulePath) {
    std::string moduleName = getBaseName(modulePath);
    
    // 检查info文件是否同名
    std::string infoFile = modulePath + "/info/" + moduleName + ".chtl";
    if (!fileExists(infoFile)) {
        reportError("模块信息文件名不匹配，期望: " + infoFile);
        return false;
    }
    
    // 对于有主模块文件的情况，检查主模块文件是否同名
    std::string mainFile = modulePath + "/src/" + moduleName + ".chtl";
    if (fileExists(mainFile)) {
        // 主模块文件存在且同名，符合要求
        return true;
    }
    
    // 如果没有主模块文件，检查是否有子模块
    auto subModules = scanSubModules(modulePath);
    if (subModules.empty()) {
        reportError("模块既没有主模块文件也没有子模块: " + modulePath);
        return false;
    }
    
    return true;
}

bool ModuleManager::validateInfoFile(const std::string& infoPath) {
    if (!fileExists(infoPath)) {
        reportError("模块信息文件不存在: " + infoPath);
        return false;
    }
    
    std::ifstream file(infoPath);
    if (!file.is_open()) {
        reportError("无法打开模块信息文件: " + infoPath);
        return false;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    
    // 检查是否包含[Info]块
    if (content.find("[Info]") == std::string::npos) {
        reportError("模块信息文件缺少[Info]块: " + infoPath);
        return false;
    }
    
    return true;
}

bool ModuleManager::parseModuleInfo(const std::string& infoPath, ModuleInfo& info) {
    if (!fileExists(infoPath)) {
        return false;
    }
    
    std::ifstream file(infoPath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    
    // 解析[Info]块
    if (!parseInfoBlock(content, info)) {
        return false;
    }
    
    // 解析[Export]块
    if (!parseExportBlock(content, info.exportTable)) {
        // Export块是可选的，不影响模块加载
        std::cout << "  ⚠️ 模块信息文件缺少[Export]块: " << infoPath << std::endl;
    }
    
    return true;
}

bool ModuleManager::parseInfoBlock(const std::string& content, ModuleInfo& info) {
    std::regex infoBlockRegex(R"(\[Info\]\s*\{([^}]*)\})");
    std::smatch match;
    
    if (!std::regex_search(content, match, infoBlockRegex)) {
        reportError("无法解析[Info]块");
        return false;
    }
    
    std::string infoContent = match[1].str();
    
    // 解析各个字段
    std::regex fieldRegex(R"((\w+)\s*=\s*"([^"]*)")");
    std::sregex_iterator iter(infoContent.begin(), infoContent.end(), fieldRegex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        std::string key = (*iter)[1].str();
        std::string value = (*iter)[2].str();
        
        if (key == "name") info.name = value;
        else if (key == "version") info.version = value;
        else if (key == "description") info.description = value;
        else if (key == "author") info.author = value;
        else if (key == "license") info.license = value;
        else if (key == "category") info.category = value;
        else if (key == "minCHTLVersion") info.minCHTLVersion = value;
        else if (key == "maxCHTLVersion") info.maxCHTLVersion = value;
        else if (key == "dependencies") {
            // 解析依赖列表
            if (!value.empty()) {
                std::regex depRegex(R"([^,\s]+)");
                std::sregex_iterator depIter(value.begin(), value.end(), depRegex);
                std::sregex_iterator depEnd;
                
                for (; depIter != depEnd; ++depIter) {
                    info.dependencies.push_back((*depIter).str());
                }
            }
        }
    }
    
    return true;
}

bool ModuleManager::parseExportBlock(const std::string& content, std::unordered_map<std::string, std::string>& exportTable) {
    std::regex exportBlockRegex(R"(\[Export\]\s*\{([^}]*)\})");
    std::smatch match;
    
    if (!std::regex_search(content, match, exportBlockRegex)) {
        return false;  // Export块是可选的
    }
    
    std::string exportContent = match[1].str();
    
    // 解析导出项
    std::regex exportItemRegex(R"(\[(\w+)\]\s*@(\w+)\s+([^;]+);)");
    std::sregex_iterator iter(exportContent.begin(), exportContent.end(), exportItemRegex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        std::string category = (*iter)[1].str();  // Custom, Template, Origin等
        std::string type = (*iter)[2].str();      // Style, Element, Var等
        std::string items = (*iter)[3].str();     // 导出项列表
        
        // 解析导出项列表
        std::regex itemRegex(R"([^,\s]+)");
        std::sregex_iterator itemIter(items.begin(), items.end(), itemRegex);
        std::sregex_iterator itemEnd;
        
        for (; itemIter != itemEnd; ++itemIter) {
            std::string itemName = (*itemIter).str();
            std::string fullType = "[" + category + "] @" + type;
            exportTable[itemName] = fullType;
        }
    }
    
    return true;
}

std::vector<std::string> ModuleManager::resolveBatchImport(const std::string& pattern) {
    std::vector<std::string> resolvedPaths;
    
    if (!isWildcardPattern(pattern)) {
        // 不是通配符模式，按普通路径处理
        std::string resolvedPath;
        if (resolveModulePath(pattern, resolvedPath)) {
            resolvedPaths.push_back(resolvedPath);
        }
        return resolvedPaths;
    }
    
    return expandWildcardPattern(pattern);
}

bool ModuleManager::isWildcardPattern(const std::string& pattern) {
    return pattern.find(".*") != std::string::npos || 
           pattern.find("/*") != std::string::npos ||
           pattern.find("*.") != std::string::npos;
}

std::vector<std::string> ModuleManager::expandWildcardPattern(const std::string& pattern) {
    std::vector<std::string> expandedPaths;
    
    try {
        // 解析模式
        size_t wildcardPos = pattern.find(".*");
        if (wildcardPos == std::string::npos) {
            wildcardPos = pattern.find("/*");
        }
        
        if (wildcardPos != std::string::npos) {
            std::string basePath = pattern.substr(0, wildcardPos);
            std::string extension = "";
            
            // 检查是否有扩展名过滤
            if (pattern.find(".*.cmod") != std::string::npos || pattern.find("/*.cmod") != std::string::npos) {
                extension = ".cmod";
            } else if (pattern.find(".*.chtl") != std::string::npos || pattern.find("/*.chtl") != std::string::npos) {
                extension = ".chtl";
            } else if (pattern.find(".*.cjmod") != std::string::npos || pattern.find("/*.cjmod") != std::string::npos) {
                extension = ".cjmod";
            }
            
            // 扫描目录
            if (directoryExists(basePath)) {
                auto files = listFilesInDirectory(basePath, extension);
                for (const auto& file : files) {
                    expandedPaths.push_back(basePath + "/" + file);
                }
            }
        }
        
    } catch (const std::exception& e) {
        reportError("展开通配符模式时发生错误: " + std::string(e.what()));
    }
    
    return expandedPaths;
}

bool ModuleManager::resolveSubModulePath(const std::string& parentModule, const std::string& subModule, std::string& resolvedPath) {
    std::string parentPath;
    if (!resolveModulePath(parentModule, parentPath)) {
        return false;
    }
    
    // 获取父模块目录
    std::string parentDir = getDirectoryName(parentPath);
    
    // 查找子模块
    std::string subModulePath = parentDir + "/src/" + subModule;
    if (directoryExists(subModulePath)) {
        resolvedPath = subModulePath;
        return true;
    }
    
    reportError("无法找到子模块: " + parentModule + "." + subModule);
    return false;
}

// 文件系统辅助方法实现
std::vector<std::string> ModuleManager::listFilesInDirectory(const std::string& directory, const std::string& extension) {
    std::vector<std::string> files;
    
    try {
        if (!directoryExists(directory)) {
            return files;
        }
        
        for (const auto& entry : std::filesystem::directory_iterator(directory)) {
            if (entry.is_regular_file()) {
                std::string filename = entry.path().filename().string();
                if (extension.empty() || getFileExtension(filename) == extension) {
                    files.push_back(filename);
                }
            }
        }
    } catch (const std::exception& e) {
        reportError("列出目录文件时发生错误: " + std::string(e.what()));
    }
    
    return files;
}

bool ModuleManager::fileExists(const std::string& path) {
    return std::filesystem::exists(path) && std::filesystem::is_regular_file(path);
}

bool ModuleManager::directoryExists(const std::string& path) {
    return std::filesystem::exists(path) && std::filesystem::is_directory(path);
}

std::string ModuleManager::getFileExtension(const std::string& filename) {
    size_t pos = filename.find_last_of('.');
    if (pos != std::string::npos) {
        return filename.substr(pos);
    }
    return "";
}

std::string ModuleManager::getBaseName(const std::string& filename) {
    std::string basename = std::filesystem::path(filename).filename().string();
    size_t pos = basename.find_last_of('.');
    if (pos != std::string::npos) {
        return basename.substr(0, pos);
    }
    return basename;
}

std::string ModuleManager::getDirectoryName(const std::string& path) {
    return std::filesystem::path(path).parent_path().string();
}

bool ModuleManager::unloadModule(const std::string& moduleName) {
    auto it = m_loadedModules.find(moduleName);
    if (it != m_loadedModules.end()) {
        m_loadedModules.erase(it);
        std::cout << "✓ 卸载模块: " << moduleName << std::endl;
        return true;
    }
    
    reportError("模块未加载: " + moduleName);
    return false;
}

ModuleInfo ModuleManager::getModuleInfo(const std::string& moduleName) {
    auto it = m_loadedModules.find(moduleName);
    if (it != m_loadedModules.end()) {
        return it->second;
    }
    
    return ModuleInfo{};  // 返回空的模块信息
}

std::vector<std::string> ModuleManager::getLoadedModules() {
    std::vector<std::string> modules;
    for (const auto& pair : m_loadedModules) {
        modules.push_back(pair.first);
    }
    return modules;
}

void ModuleManager::reportError(const std::string& message) {
    ErrorReporter reporter;
    reporter.error("ModuleManager", message);
}

}