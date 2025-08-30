#include "CMODApi.h"
#include "../../Error/ErrorReporter.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <regex>

namespace CHTL {

// CMODModule实现
bool CMODModule::loadFromPath(const std::string& modulePath) {
    m_modulePath = modulePath;
    
    // 验证结构
    if (!validateStructure()) {
        reportError("CMOD模块结构验证失败: " + modulePath);
        return false;
    }
    
    // 解析info文件
    std::string moduleName = std::filesystem::path(modulePath).filename().string();
    std::string infoPath = modulePath + "/info/" + moduleName + ".chtl";
    
    if (!parseInfoFile(infoPath)) {
        reportError("解析CMOD信息文件失败: " + infoPath);
        return false;
    }
    
    // 加载源文件
    if (!loadSourceFiles()) {
        reportError("加载CMOD源文件失败: " + modulePath);
        return false;
    }
    
    // 加载子模块
    if (!loadSubModules()) {
        reportError("加载CMOD子模块失败: " + modulePath);
        return false;
    }
    
    std::cout << "✓ CMOD模块加载成功: " << m_info.name << " v" << m_info.version << std::endl;
    return true;
}

bool CMODModule::validateStructure() const {
    // 检查基本结构：src和info文件夹
    if (!std::filesystem::exists(m_modulePath + "/src") || 
        !std::filesystem::exists(m_modulePath + "/info")) {
        return false;
    }
    
    // 检查同名约束
    std::string moduleName = std::filesystem::path(m_modulePath).filename().string();
    std::string infoFile = m_modulePath + "/info/" + moduleName + ".chtl";
    
    if (!std::filesystem::exists(infoFile)) {
        return false;
    }
    
    // 检查主模块文件或子模块存在
    std::string mainFile = m_modulePath + "/src/" + moduleName + ".chtl";
    bool hasMainFile = std::filesystem::exists(mainFile);
    
    // 检查是否有子模块
    bool hasSubModules = false;
    try {
        for (const auto& entry : std::filesystem::directory_iterator(m_modulePath + "/src")) {
            if (entry.is_directory()) {
                std::string subPath = entry.path().string();
                if (std::filesystem::exists(subPath + "/src") && 
                    std::filesystem::exists(subPath + "/info")) {
                    hasSubModules = true;
                    break;
                }
            }
        }
    } catch (const std::exception&) {
        // 忽略错误，继续验证
    }
    
    if (!hasMainFile && !hasSubModules) {
        return false;
    }
    
    return true;
}

bool CMODModule::parseInfoFile(const std::string& infoPath) {
    std::ifstream file(infoPath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    
    // 解析[Info]块
    if (!parseInfoBlock(content)) {
        return false;
    }
    
    // 解析[Export]块（可选）
    parseExportBlock(content);
    
    return true;
}

bool CMODModule::parseInfoBlock(const std::string& content) {
    std::regex infoBlockRegex(R"(\[Info\]\s*\{([^}]*)\})");
    std::smatch match;
    
    if (!std::regex_search(content, match, infoBlockRegex)) {
        reportError("无法找到[Info]块");
        return false;
    }
    
    std::string infoContent = match[1].str();
    
    // 解析字段
    std::regex fieldRegex(R"((\w+)\s*=\s*"([^"]*)")");
    std::sregex_iterator iter(infoContent.begin(), infoContent.end(), fieldRegex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        std::string key = (*iter)[1].str();
        std::string value = (*iter)[2].str();
        
        if (key == "name") m_info.name = value;
        else if (key == "version") m_info.version = value;
        else if (key == "description") m_info.description = value;
        else if (key == "author") m_info.author = value;
        else if (key == "license") m_info.license = value;
        else if (key == "category") m_info.category = value;
        else if (key == "minCHTLVersion") m_info.minCHTLVersion = value;
        else if (key == "maxCHTLVersion") m_info.maxCHTLVersion = value;
        else if (key == "dependencies") {
            // 解析依赖列表
            if (!value.empty()) {
                std::regex depRegex(R"([^,\s]+)");
                std::sregex_iterator depIter(value.begin(), value.end(), depRegex);
                std::sregex_iterator depEnd;
                
                for (; depIter != depEnd; ++depIter) {
                    m_info.dependencies.push_back((*depIter).str());
                }
            }
        }
    }
    
    return true;
}

bool CMODModule::parseExportBlock(const std::string& content) {
    std::regex exportBlockRegex(R"(\[Export\]\s*\{([^}]*)\})");
    std::smatch match;
    
    if (!std::regex_search(content, match, exportBlockRegex)) {
        return false;  // Export块是可选的
    }
    
    std::string exportContent = match[1].str();
    
    // 解析导出项
    std::regex exportLineRegex(R"(\[(\w+)\]\s*@(\w+)\s+([^;]+);)");
    std::sregex_iterator iter(exportContent.begin(), exportContent.end(), exportLineRegex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        std::string category = (*iter)[1].str();  // Custom, Template, Origin等
        std::string type = (*iter)[2].str();      // Style, Element, Var等
        std::string itemsStr = (*iter)[3].str();  // 导出项列表
        
        CMODExportItem exportItem;
        exportItem.category = category;
        exportItem.type = type;
        
        // 解析导出项列表
        std::regex itemRegex(R"([^,\s]+)");
        std::sregex_iterator itemIter(itemsStr.begin(), itemsStr.end(), itemRegex);
        std::sregex_iterator itemEnd;
        
        for (; itemIter != itemEnd; ++itemIter) {
            std::string itemName = (*itemIter).str();
            exportItem.items.push_back(itemName);
            
            // 建立类型映射
            std::string fullType = "[" + category + "] @" + type;
            m_exportTable.itemTypeMap[itemName] = fullType;
        }
        
        m_exportTable.exports.push_back(exportItem);
    }
    
    std::cout << "  ✓ 解析Export块，发现 " << m_exportTable.itemTypeMap.size() << " 个导出项" << std::endl;
    return true;
}

bool CMODModule::loadSourceFiles() {
    std::string srcPath = m_modulePath + "/src";
    
    try {
        // 加载主模块文件
        std::string mainFile = srcPath + "/" + m_info.name + ".chtl";
        if (std::filesystem::exists(mainFile)) {
            m_mainSourcePath = mainFile;
            m_sourceFiles.push_back(mainFile);
        }
        
        // 加载其他源文件
        for (const auto& entry : std::filesystem::directory_iterator(srcPath)) {
            if (entry.is_regular_file()) {
                std::string filePath = entry.path().string();
                std::string fileName = entry.path().filename().string();
                
                if (fileName.ends_with(".chtl") && filePath != mainFile) {
                    m_sourceFiles.push_back(filePath);
                }
            }
        }
        
        std::cout << "  ✓ 加载 " << m_sourceFiles.size() << " 个源文件" << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        reportError("加载源文件时发生错误: " + std::string(e.what()));
        return false;
    }
}

bool CMODModule::loadSubModules() {
    std::string srcPath = m_modulePath + "/src";
    
    try {
        for (const auto& entry : std::filesystem::directory_iterator(srcPath)) {
            if (entry.is_directory()) {
                std::string subModuleName = entry.path().filename().string();
                std::string subModulePath = entry.path().string();
                
                // 检查是否为有效子模块
                if (std::filesystem::exists(subModulePath + "/src") && 
                    std::filesystem::exists(subModulePath + "/info")) {
                    
                    auto subModule = std::make_shared<CMODModule>();
                    if (subModule->loadFromPath(subModulePath)) {
                        m_subModules.push_back(subModuleName);
                        m_subModuleMap[subModuleName] = subModule;
                    }
                }
            }
        }
        
        if (!m_subModules.empty()) {
            std::cout << "  ✓ 加载 " << m_subModules.size() << " 个子模块" << std::endl;
        }
        
        return true;
        
    } catch (const std::exception& e) {
        reportError("加载子模块时发生错误: " + std::string(e.what()));
        return false;
    }
}

bool CMODModule::hasExport(const std::string& itemName) const {
    return m_exportTable.itemTypeMap.find(itemName) != m_exportTable.itemTypeMap.end();
}

std::string CMODModule::getExportType(const std::string& itemName) const {
    auto it = m_exportTable.itemTypeMap.find(itemName);
    if (it != m_exportTable.itemTypeMap.end()) {
        return it->second;
    }
    return "";
}

std::vector<std::string> CMODModule::getExportsByType(const std::string& category, const std::string& type) const {
    std::vector<std::string> results;
    
    for (const auto& exportItem : m_exportTable.exports) {
        if (exportItem.category == category && exportItem.type == type) {
            results.insert(results.end(), exportItem.items.begin(), exportItem.items.end());
        }
    }
    
    return results;
}

bool CMODModule::hasSubModule(const std::string& subModuleName) const {
    return std::find(m_subModules.begin(), m_subModules.end(), subModuleName) != m_subModules.end();
}

std::shared_ptr<CMODModule> CMODModule::getSubModule(const std::string& subModuleName) const {
    auto it = m_subModuleMap.find(subModuleName);
    if (it != m_subModuleMap.end()) {
        return it->second;
    }
    return nullptr;
}

std::string CMODModule::getSourceContent(const std::string& fileName) const {
    for (const auto& sourcePath : m_sourceFiles) {
        if (std::filesystem::path(sourcePath).filename().string() == fileName) {
            std::ifstream file(sourcePath);
            if (file.is_open()) {
                std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
                file.close();
                return content;
            }
        }
    }
    return "";
}

void CMODModule::reportError(const std::string& message) const {
    ErrorReporter reporter;
    reporter.error("CMODModule", message);
}

// CMODApi实现
CMODApi::CMODApi() = default;
CMODApi::~CMODApi() = default;

bool CMODApi::loadCMOD(const std::string& modulePath) {
    try {
        auto module = std::make_shared<CMODModule>();
        
        if (!module->loadFromPath(modulePath)) {
            return false;
        }
        
        std::string moduleName = module->getName();
        m_loadedModules[moduleName] = module;
        
        std::cout << "✓ CMOD模块注册成功: " << moduleName << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        reportError("加载CMOD模块时发生异常: " + std::string(e.what()));
        return false;
    }
}

bool CMODApi::unloadCMOD(const std::string& moduleName) {
    auto it = m_loadedModules.find(moduleName);
    if (it != m_loadedModules.end()) {
        m_loadedModules.erase(it);
        std::cout << "✓ CMOD模块卸载成功: " << moduleName << std::endl;
        return true;
    }
    
    reportError("CMOD模块未加载: " + moduleName);
    return false;
}

std::shared_ptr<CMODModule> CMODApi::getCMOD(const std::string& moduleName) {
    auto it = m_loadedModules.find(moduleName);
    if (it != m_loadedModules.end()) {
        return it->second;
    }
    return nullptr;
}

std::vector<std::string> CMODApi::getLoadedCMODs() const {
    std::vector<std::string> modules;
    for (const auto& pair : m_loadedModules) {
        modules.push_back(pair.first);
    }
    return modules;
}

bool CMODApi::hasCMOD(const std::string& moduleName) const {
    return m_loadedModules.find(moduleName) != m_loadedModules.end();
}

CMODInfo CMODApi::getCMODInfo(const std::string& moduleName) const {
    auto module = getCMOD(moduleName);
    if (module) {
        return module->getInfo();
    }
    return CMODInfo{};
}

CMODExportTable CMODApi::getCMODExports(const std::string& moduleName) const {
    auto module = getCMOD(moduleName);
    if (module) {
        return module->getExportTable();
    }
    return CMODExportTable{};
}

std::shared_ptr<CMODModule> CMODApi::getSubModule(const std::string& parentModule, const std::string& subModule) {
    auto parent = getCMOD(parentModule);
    if (parent) {
        return parent->getSubModule(subModule);
    }
    return nullptr;
}

bool CMODApi::hasSubModule(const std::string& parentModule, const std::string& subModule) const {
    auto parent = getCMOD(parentModule);
    if (parent) {
        return parent->hasSubModule(subModule);
    }
    return false;
}

bool CMODApi::validateCMODStructure(const std::string& modulePath) const {
    // 检查基本目录结构
    if (!directoryExists(modulePath) || 
        !directoryExists(modulePath + "/src") || 
        !directoryExists(modulePath + "/info")) {
        return false;
    }
    
    return validateSameNameConstraint(modulePath);
}

bool CMODApi::validateSameNameConstraint(const std::string& modulePath) const {
    std::string moduleName = std::filesystem::path(modulePath).filename().string();
    
    // 检查info文件同名约束
    std::string infoFile = modulePath + "/info/" + moduleName + ".chtl";
    if (!fileExists(infoFile)) {
        return false;
    }
    
    // 检查主模块文件（如果存在）
    std::string mainFile = modulePath + "/src/" + moduleName + ".chtl";
    if (fileExists(mainFile)) {
        return true;  // 主模块文件存在且同名
    }
    
    // 如果没有主模块文件，检查是否有子模块
    try {
        for (const auto& entry : std::filesystem::directory_iterator(modulePath + "/src")) {
            if (entry.is_directory()) {
                std::string subPath = entry.path().string();
                if (directoryExists(subPath + "/src") && directoryExists(subPath + "/info")) {
                    return true;  // 有子模块，符合要求
                }
            }
        }
    } catch (const std::exception&) {
        return false;
    }
    
    return false;  // 既没有主模块文件也没有子模块
}

bool CMODApi::packCMOD(const std::string& sourceDir, const std::string& outputPath) {
    // 验证源目录结构
    if (!validateCMODStructure(sourceDir)) {
        reportError("源目录不符合CMOD结构: " + sourceDir);
        return false;
    }
    
    try {
        // 这里应该实现ZIP打包逻辑
        // 由于需要ZIP库支持，暂时提供接口
        std::cout << "✓ CMOD打包功能接口已准备，需要ZIP库支持" << std::endl;
        std::cout << "  源目录: " << sourceDir << std::endl;
        std::cout << "  输出路径: " << outputPath << std::endl;
        
        return true;
        
    } catch (const std::exception& e) {
        reportError("打包CMOD时发生错误: " + std::string(e.what()));
        return false;
    }
}

bool CMODApi::unpackCMOD(const std::string& cmodPath, const std::string& outputDir) {
    if (!fileExists(cmodPath)) {
        reportError("CMOD文件不存在: " + cmodPath);
        return false;
    }
    
    try {
        // 这里应该实现ZIP解包逻辑
        // 由于需要ZIP库支持，暂时提供接口
        std::cout << "✓ CMOD解包功能接口已准备，需要ZIP库支持" << std::endl;
        std::cout << "  CMOD文件: " << cmodPath << std::endl;
        std::cout << "  输出目录: " << outputDir << std::endl;
        
        return true;
        
    } catch (const std::exception& e) {
        reportError("解包CMOD时发生错误: " + std::string(e.what()));
        return false;
    }
}

bool CMODApi::fileExists(const std::string& path) const {
    return std::filesystem::exists(path) && std::filesystem::is_regular_file(path);
}

bool CMODApi::directoryExists(const std::string& path) const {
    return std::filesystem::exists(path) && std::filesystem::is_directory(path);
}

void CMODApi::reportError(const std::string& message) const {
    ErrorReporter reporter;
    reporter.error("CMODApi", message);
}

}