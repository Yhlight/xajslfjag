#include "CMODManager.h"
#include "../../Util/ZIPUtil/ZIPManager.h"
#include <fstream>
#include <filesystem>
#include <regex>
#include <chrono>
#include <algorithm>

namespace CHTL {

// CMODStructure 实现
bool CMODStructure::validate() const {
    if (moduleName.empty()) return false;
    if (rootPath.empty()) return false;
    
    // 检查src和info路径存在性
    std::filesystem::path srcPathObj(srcPath);
    std::filesystem::path infoPathObj(infoPath);
    
    if (!std::filesystem::exists(srcPathObj) || !std::filesystem::is_directory(srcPathObj)) {
        return false;
    }
    
    if (!std::filesystem::exists(infoPathObj) || !std::filesystem::is_directory(infoPathObj)) {
        return false;
    }
    
    // 检查info文件存在性（必须）
    if (!std::filesystem::exists(infoFile)) {
        return false;
    }
    
    // 如果没有子模块，主模块文件必须存在
    if (subModules.empty() && !hasMainModule) {
        return false;
    }
    
    // 检查子模块结构
    for (const auto& subModule : subModules) {
        if (subModule.name.empty()) return false;
        
        std::filesystem::path subSrcPath(subModule.srcPath);
        std::filesystem::path subInfoPath(subModule.infoPath);
        
        if (!std::filesystem::exists(subSrcPath) || !std::filesystem::is_directory(subSrcPath)) {
            return false;
        }
        
        if (!std::filesystem::exists(subInfoPath) || !std::filesystem::is_directory(subInfoPath)) {
            return false;
        }
        
        // 子模块的info文件必须存在
        if (!std::filesystem::exists(subModule.infoFile)) {
            return false;
        }
        
        // 子模块的主文件检查
        if (subModule.hasMainFile && !std::filesystem::exists(subModule.mainFile)) {
            return false;
        }
    }
    
    return true;
}

String CMODStructure::toString() const {
    String result = "CMOD Structure: " + moduleName + "\n";
    result += "Root: " + rootPath + "\n";
    result += "Src: " + srcPath + "\n";
    result += "Info: " + infoPath + "\n";
    
    if (hasMainModule) {
        result += "Main Module: " + mainModuleFile + "\n";
    }
    
    result += "Info File: " + infoFile + "\n";
    
    if (!subModules.empty()) {
        result += "Sub Modules:\n";
        for (const auto& subModule : subModules) {
            result += "  " + subModule.name + ":\n";
            result += "    Src: " + subModule.srcPath + "\n";
            result += "    Info: " + subModule.infoPath + "\n";
            if (subModule.hasMainFile) {
                result += "    Main: " + subModule.mainFile + "\n";
            }
            result += "    Info File: " + subModule.infoFile + "\n";
        }
    }
    
    if (!additionalFiles.empty()) {
        result += "Additional Files:\n";
        for (const auto& file : additionalFiles) {
            result += "  " + file + "\n";
        }
    }
    
    result += "Valid: " + String(isValid ? "true" : "false") + "\n";
    
    return result;
}

StringVector CMODStructure::getAllFiles() const {
    StringVector files;
    
    if (hasMainModule && !mainModuleFile.empty()) {
        files.push_back(mainModuleFile);
    }
    
    if (!infoFile.empty()) {
        files.push_back(infoFile);
    }
    
    for (const auto& subModule : subModules) {
        if (subModule.hasMainFile && !subModule.mainFile.empty()) {
            files.push_back(subModule.mainFile);
        }
        if (!subModule.infoFile.empty()) {
            files.push_back(subModule.infoFile);
        }
    }
    
    for (const auto& file : additionalFiles) {
        files.push_back(file);
    }
    
    return files;
}

// CMODInfo::ExportTable 实现
bool CMODInfo::ExportTable::isEmpty() const {
    return customStyles.empty() && customElements.empty() && customVars.empty() &&
           templateStyles.empty() && templateElements.empty() && templateVars.empty() &&
           origins.empty() && configurations.empty();
}

String CMODInfo::ExportTable::toString() const {
    String result = "[Export]\n{\n";
    
    if (!customStyles.empty()) {
        result += "    [Custom] @Style ";
        for (size_t i = 0; i < customStyles.size(); ++i) {
            result += customStyles[i];
            if (i < customStyles.size() - 1) result += ", ";
        }
        result += ";\n";
    }
    
    if (!customElements.empty()) {
        result += "    [Custom] @Element ";
        for (size_t i = 0; i < customElements.size(); ++i) {
            result += customElements[i];
            if (i < customElements.size() - 1) result += ", ";
        }
        result += ";\n";
    }
    
    if (!customVars.empty()) {
        result += "    [Custom] @Var ";
        for (size_t i = 0; i < customVars.size(); ++i) {
            result += customVars[i];
            if (i < customVars.size() - 1) result += ", ";
        }
        result += ";\n";
    }
    
    if (!templateStyles.empty()) {
        result += "    [Template] @Style ";
        for (size_t i = 0; i < templateStyles.size(); ++i) {
            result += templateStyles[i];
            if (i < templateStyles.size() - 1) result += ", ";
        }
        result += ";\n";
    }
    
    if (!templateElements.empty()) {
        result += "    [Template] @Element ";
        for (size_t i = 0; i < templateElements.size(); ++i) {
            result += templateElements[i];
            if (i < templateElements.size() - 1) result += ", ";
        }
        result += ";\n";
    }
    
    if (!templateVars.empty()) {
        result += "    [Template] @Var ";
        for (size_t i = 0; i < templateVars.size(); ++i) {
            result += templateVars[i];
            if (i < templateVars.size() - 1) result += ", ";
        }
        result += ";\n";
    }
    
    if (!origins.empty()) {
        result += "    [Origin] ";
        for (size_t i = 0; i < origins.size(); ++i) {
            result += origins[i];
            if (i < origins.size() - 1) result += ", ";
        }
        result += ";\n";
    }
    
    if (!configurations.empty()) {
        result += "    [Configuration] ";
        for (size_t i = 0; i < configurations.size(); ++i) {
            result += configurations[i];
            if (i < configurations.size() - 1) result += ", ";
        }
        result += ";\n";
    }
    
    result += "}\n";
    
    return result;
}

// CMODInfo 实现
bool CMODInfo::isValid() const {
    // 基本字段验证
    if (name.empty()) return false;
    if (version.empty()) return false;
    if (author.empty()) return false;
    
    // 版本格式验证（简单的语义版本检查）
    std::regex versionRegex(R"(^(\d+)\.(\d+)\.(\d+)([+-][a-zA-Z0-9\-\.]*)?$)");
    if (!std::regex_match(version, versionRegex)) return false;
    
    if (!minCHTLVersion.empty() && !std::regex_match(minCHTLVersion, versionRegex)) return false;
    if (!maxCHTLVersion.empty() && !std::regex_match(maxCHTLVersion, versionRegex)) return false;
    
    return true;
}

String CMODInfo::generateInfoContent() const {
    String result = "[Info]\n{\n";
    
    result += "    name = \"" + name + "\";\n";
    result += "    version = \"" + version + "\";\n";
    result += "    description = \"" + description + "\";\n";
    result += "    author = \"" + author + "\";\n";
    result += "    license = \"" + license + "\";\n";
    
    if (!dependencies.empty()) {
        result += "    dependencies = \"";
        for (size_t i = 0; i < dependencies.size(); ++i) {
            result += dependencies[i];
            if (i < dependencies.size() - 1) result += ", ";
        }
        result += "\";\n";
    } else {
        result += "    dependencies = \"\";\n";
    }
    
    result += "    category = \"" + category + "\";\n";
    result += "    minCHTLVersion = \"" + minCHTLVersion + "\";\n";
    result += "    maxCHTLVersion = \"" + maxCHTLVersion + "\";\n";
    
    // 添加额外元数据
    for (const auto& meta : metadata) {
        result += "    " + meta.first + " = \"" + meta.second + "\";\n";
    }
    
    result += "}\n\n";
    
    // 添加导出表
    if (!exportTable.isEmpty()) {
        result += exportTable.toString();
    }
    
    return result;
}

void CMODInfo::parseFromContent(const String& content) {
    // 简化的Info块解析
    std::regex infoBlockRegex(R"(\[Info\]\s*\{([^}]*)\})");
    std::smatch infoMatch;
    
    if (std::regex_search(content, infoMatch, infoBlockRegex)) {
        String infoBlock = infoMatch[1].str();
        
        // 解析各个字段
        std::regex fieldRegex(R"((\w+)\s*=\s*\"([^\"]*)\"\s*;)");
        std::sregex_iterator iter(infoBlock.begin(), infoBlock.end(), fieldRegex);
        std::sregex_iterator end;
        
        for (; iter != end; ++iter) {
            String key = (*iter)[1].str();
            String value = (*iter)[2].str();
            
            if (key == "name") name = value;
            else if (key == "version") version = value;
            else if (key == "description") description = value;
            else if (key == "author") author = value;
            else if (key == "license") license = value;
            else if (key == "dependencies") {
                if (!value.empty()) {
                    std::regex depRegex(R"([^,]+)");
                    std::sregex_iterator depIter(value.begin(), value.end(), depRegex);
                    std::sregex_iterator depEnd;
                    
                    for (; depIter != depEnd; ++depIter) {
                        String dep = (*depIter)[0].str();
                        // 去除前后空格
                        dep.erase(0, dep.find_first_not_of(" \t"));
                        dep.erase(dep.find_last_not_of(" \t") + 1);
                        if (!dep.empty()) {
                            dependencies.push_back(dep);
                        }
                    }
                }
            }
            else if (key == "category") category = value;
            else if (key == "minCHTLVersion") minCHTLVersion = value;
            else if (key == "maxCHTLVersion") maxCHTLVersion = value;
            else {
                // 额外元数据
                metadata[key] = value;
            }
        }
    }
    
    // 解析Export块（如果存在）
    std::regex exportBlockRegex(R"(\[Export\]\s*\{([^}]*)\})");
    std::smatch exportMatch;
    
    if (std::regex_search(content, exportMatch, exportBlockRegex)) {
        String exportBlock = exportMatch[1].str();
        
        // 解析导出项
        std::regex exportItemRegex(R"(\[(\w+)\]\s*@(\w+)\s*([^;]*);)");
        std::sregex_iterator exportIter(exportBlock.begin(), exportBlock.end(), exportItemRegex);
        std::sregex_iterator exportEnd;
        
        for (; exportIter != exportEnd; ++exportIter) {
            String blockType = (*exportIter)[1].str();
            String itemType = (*exportIter)[2].str();
            String items = (*exportIter)[3].str();
            
            // 解析项目列表
            std::regex itemRegex(R"([^,]+)");
            std::sregex_iterator itemIter(items.begin(), items.end(), itemRegex);
            std::sregex_iterator itemEnd;
            
            StringVector itemList;
            for (; itemIter != itemEnd; ++itemIter) {
                String item = (*itemIter)[0].str();
                item.erase(0, item.find_first_not_of(" \t"));
                item.erase(item.find_last_not_of(" \t") + 1);
                if (!item.empty()) {
                    itemList.push_back(item);
                }
            }
            
            // 分类存储
            if (blockType == "Custom") {
                if (itemType == "Style") {
                    exportTable.customStyles = itemList;
                } else if (itemType == "Element") {
                    exportTable.customElements = itemList;
                } else if (itemType == "Var") {
                    exportTable.customVars = itemList;
                }
            } else if (blockType == "Template") {
                if (itemType == "Style") {
                    exportTable.templateStyles = itemList;
                } else if (itemType == "Element") {
                    exportTable.templateElements = itemList;
                } else if (itemType == "Var") {
                    exportTable.templateVars = itemList;
                }
            } else if (blockType == "Origin") {
                for (const auto& item : itemList) {
                    exportTable.origins.push_back("@" + itemType + " " + item);
                }
            } else if (blockType == "Configuration") {
                for (const auto& item : itemList) {
                    exportTable.configurations.push_back("@" + itemType + " " + item);
                }
            }
        }
    }
}

String CMODInfo::generateExportTable() const {
    return exportTable.toString();
}

// CMODPackage 实现
CMODPackage::CMODPackage(const String& packagePath) 
    : packagePath(packagePath), loaded(false), parsed(false) {
}

bool CMODPackage::load() {
    if (loaded) return true;
    
    clearErrors();
    
    // 检查包路径是否存在
    if (!std::filesystem::exists(packagePath)) {
        reportError("Package path does not exist: " + packagePath);
        return false;
    }
    
    // 分析结构
    if (!analyzeStructure()) {
        reportError("Failed to analyze package structure");
        return false;
    }
    
    // 加载模块内容
    if (!loadModuleContents()) {
        reportError("Failed to load module contents");
        return false;
    }
    
    loaded = true;
    return true;
}

bool CMODPackage::parse() {
    if (!loaded) {
        if (!load()) return false;
    }
    
    if (parsed) return true;
    
    clearErrors();
    
    // 解析模块信息
    if (!parseModuleInfo()) {
        reportError("Failed to parse module info");
        return false;
    }
    
    // 生成导出表
    generateExportTable();
    
    parsed = true;
    return true;
}

String CMODPackage::getMainModuleContent() const {
    if (!loaded) return "";
    
    auto it = moduleContents.find(structure.mainModuleFile);
    return it != moduleContents.end() ? it->second : "";
}

String CMODPackage::getInfoContent() const {
    if (!loaded) return "";
    
    auto it = moduleContents.find(structure.infoFile);
    return it != moduleContents.end() ? it->second : "";
}

String CMODPackage::getSubModuleContent(const String& subModuleName) const {
    if (!loaded) return "";
    
    for (const auto& subModule : structure.subModules) {
        if (subModule.name == subModuleName) {
            auto it = moduleContents.find(subModule.mainFile);
            return it != moduleContents.end() ? it->second : "";
        }
    }
    
    return "";
}

StringVector CMODPackage::getSubModuleNames() const {
    StringVector names;
    for (const auto& subModule : structure.subModules) {
        names.push_back(subModule.name);
    }
    return names;
}

std::unique_ptr<BaseNode> CMODPackage::getMainModuleAST() const {
    // 这里需要实际的解析器来生成AST
    // 暂时返回空指针，实际实现时需要调用Parser
    return nullptr;
}

std::unique_ptr<BaseNode> CMODPackage::getSubModuleAST(const String& subModuleName) const {
    // 这里需要实际的解析器来生成AST
    // 暂时返回空指针，实际实现时需要调用Parser
    return nullptr;
}

StringUnorderedMap CMODPackage::getExports() const {
    StringUnorderedMap exports;
    
    if (!parsed) return exports;
    
    // 合并所有导出项
    for (const auto& item : info.exportTable.customStyles) {
        exports["Custom.Style." + item] = "CustomStyle";
    }
    for (const auto& item : info.exportTable.customElements) {
        exports["Custom.Element." + item] = "CustomElement";
    }
    for (const auto& item : info.exportTable.customVars) {
        exports["Custom.Var." + item] = "CustomVar";
    }
    for (const auto& item : info.exportTable.templateStyles) {
        exports["Template.Style." + item] = "TemplateStyle";
    }
    for (const auto& item : info.exportTable.templateElements) {
        exports["Template.Element." + item] = "TemplateElement";
    }
    for (const auto& item : info.exportTable.templateVars) {
        exports["Template.Var." + item] = "TemplateVar";
    }
    for (const auto& item : info.exportTable.origins) {
        exports["Origin." + item] = "Origin";
    }
    for (const auto& item : info.exportTable.configurations) {
        exports["Configuration." + item] = "Configuration";
    }
    
    return exports;
}

StringVector CMODPackage::getExportedCustomStyles() const {
    return info.exportTable.customStyles;
}

StringVector CMODPackage::getExportedCustomElements() const {
    return info.exportTable.customElements;
}

StringVector CMODPackage::getExportedCustomVars() const {
    return info.exportTable.customVars;
}

StringVector CMODPackage::getExportedTemplateStyles() const {
    return info.exportTable.templateStyles;
}

StringVector CMODPackage::getExportedTemplateElements() const {
    return info.exportTable.templateElements;
}

StringVector CMODPackage::getExportedTemplateVars() const {
    return info.exportTable.templateVars;
}

StringVector CMODPackage::getExportedOrigins() const {
    return info.exportTable.origins;
}

StringVector CMODPackage::getExportedConfigurations() const {
    return info.exportTable.configurations;
}

bool CMODPackage::hasDependency(const String& dependency) const {
    return std::find(info.dependencies.begin(), info.dependencies.end(), dependency) != info.dependencies.end();
}

bool CMODPackage::validate() const {
    if (!loaded) return false;
    
    // 验证结构
    if (!structure.validate()) return false;
    
    // 验证信息
    if (!info.isValid()) return false;
    
    // 验证文件完整性
    StringVector requiredFiles = structure.getAllFiles();
    for (const auto& file : requiredFiles) {
        if (moduleContents.find(file) == moduleContents.end()) {
            return false;
        }
    }
    
    return true;
}

StringVector CMODPackage::getValidationErrors() const {
    StringVector validationErrors;
    
    if (!loaded) {
        validationErrors.push_back("Package not loaded");
        return validationErrors;
    }
    
    // 结构验证错误
    if (!structure.validate()) {
        validationErrors.push_back("Invalid package structure");
        
        if (structure.moduleName.empty()) {
            validationErrors.push_back("Module name is empty");
        }
        
        if (!std::filesystem::exists(structure.srcPath)) {
            validationErrors.push_back("Src directory does not exist: " + structure.srcPath);
        }
        
        if (!std::filesystem::exists(structure.infoPath)) {
            validationErrors.push_back("Info directory does not exist: " + structure.infoPath);
        }
        
        if (!std::filesystem::exists(structure.infoFile)) {
            validationErrors.push_back("Info file does not exist: " + structure.infoFile);
        }
        
        if (structure.subModules.empty() && !structure.hasMainModule) {
            validationErrors.push_back("No main module and no sub modules found");
        }
    }
    
    // 信息验证错误
    if (!info.isValid()) {
        validationErrors.push_back("Invalid module info");
        
        if (info.name.empty()) {
            validationErrors.push_back("Module name is empty in info");
        }
        
        if (info.version.empty()) {
            validationErrors.push_back("Module version is empty");
        }
        
        if (info.author.empty()) {
            validationErrors.push_back("Module author is empty");
        }
    }
    
    return validationErrors;
}

bool CMODPackage::isVersionCompatible(const String& chtlVersion) const {
    if (info.minCHTLVersion.empty() && info.maxCHTLVersion.empty()) {
        return true; // 无版本限制
    }
    
    // 简化的版本比较（实际实现时应该使用更复杂的版本比较逻辑）
    if (!info.minCHTLVersion.empty() && chtlVersion < info.minCHTLVersion) {
        return false;
    }
    
    if (!info.maxCHTLVersion.empty() && chtlVersion > info.maxCHTLVersion) {
        return false;
    }
    
    return true;
}

String CMODPackage::getCompatibilityMessage(const String& chtlVersion) const {
    if (isVersionCompatible(chtlVersion)) {
        return "Compatible";
    }
    
    String message = "Incompatible: ";
    
    if (!info.minCHTLVersion.empty() && chtlVersion < info.minCHTLVersion) {
        message += "Requires CHTL version >= " + info.minCHTLVersion;
    }
    
    if (!info.maxCHTLVersion.empty() && chtlVersion > info.maxCHTLVersion) {
        if (message.length() > 14) message += ", ";
        message += "Requires CHTL version <= " + info.maxCHTLVersion;
    }
    
    return message;
}

// 私有方法实现
bool CMODPackage::analyzeStructure() {
    std::filesystem::path packagePathObj(packagePath);
    
    // 获取模块名称（包路径的最后一个组件）
    structure.moduleName = packagePathObj.filename().string();
    structure.rootPath = packagePath;
    structure.srcPath = packagePath + "/src";
    structure.infoPath = packagePath + "/info";
    
    // 检查基本目录结构
    if (!directoryExists(structure.srcPath)) {
        reportError("Src directory not found: " + structure.srcPath);
        return false;
    }
    
    if (!directoryExists(structure.infoPath)) {
        reportError("Info directory not found: " + structure.infoPath);
        return false;
    }
    
    // 设置文件路径
    structure.mainModuleFile = structure.srcPath + "/" + structure.moduleName + ".chtl";
    structure.infoFile = structure.infoPath + "/" + structure.moduleName + ".chtl";
    
    // 检查主模块文件是否存在
    structure.hasMainModule = fileExists(structure.mainModuleFile);
    
    // 分析子模块
    StringVector srcContents = listDirectory(structure.srcPath);
    for (const auto& item : srcContents) {
        std::filesystem::path itemPath = std::filesystem::path(structure.srcPath) / item;
        
        if (std::filesystem::is_directory(itemPath)) {
            // 检查是否为子模块
            String subModuleSrcPath = itemPath.string() + "/src";
            String subModuleInfoPath = itemPath.string() + "/info";
            
            if (directoryExists(subModuleSrcPath) && directoryExists(subModuleInfoPath)) {
                CMODStructure::SubModule subModule;
                subModule.name = item;
                subModule.srcPath = subModuleSrcPath;
                subModule.infoPath = subModuleInfoPath;
                subModule.mainFile = subModuleSrcPath + "/" + item + ".chtl";
                subModule.infoFile = subModuleInfoPath + "/" + item + ".chtl";
                subModule.hasMainFile = fileExists(subModule.mainFile);
                
                structure.subModules.push_back(subModule);
            }
        } else if (itemPath.extension() == ".chtl" && itemPath.filename() != structure.moduleName + ".chtl") {
            // 附加的CHTL文件
            structure.additionalFiles.push_back(itemPath.string());
        }
    }
    
    // 验证结构
    structure.isValid = structure.validate();
    
    return structure.isValid;
}

bool CMODPackage::loadModuleContents() {
    moduleContents.clear();
    
    // 加载info文件
    String infoContent = readFile(structure.infoFile);
    if (infoContent.empty()) {
        reportError("Failed to read info file: " + structure.infoFile);
        return false;
    }
    moduleContents[structure.infoFile] = infoContent;
    
    // 加载主模块文件（如果存在）
    if (structure.hasMainModule) {
        String mainContent = readFile(structure.mainModuleFile);
        if (mainContent.empty()) {
            reportError("Failed to read main module file: " + structure.mainModuleFile);
            return false;
        }
        moduleContents[structure.mainModuleFile] = mainContent;
    }
    
    // 加载子模块文件
    for (const auto& subModule : structure.subModules) {
        // 加载子模块info文件
        String subInfoContent = readFile(subModule.infoFile);
        if (subInfoContent.empty()) {
            reportError("Failed to read sub module info file: " + subModule.infoFile);
            return false;
        }
        moduleContents[subModule.infoFile] = subInfoContent;
        
        // 加载子模块主文件（如果存在）
        if (subModule.hasMainFile) {
            String subMainContent = readFile(subModule.mainFile);
            if (subMainContent.empty()) {
                reportError("Failed to read sub module main file: " + subModule.mainFile);
                return false;
            }
            moduleContents[subModule.mainFile] = subMainContent;
        }
    }
    
    // 加载附加文件
    for (const auto& additionalFile : structure.additionalFiles) {
        String additionalContent = readFile(additionalFile);
        if (!additionalContent.empty()) {
            moduleContents[additionalFile] = additionalContent;
        }
    }
    
    return true;
}

bool CMODPackage::parseModuleInfo() {
    auto it = moduleContents.find(structure.infoFile);
    if (it == moduleContents.end()) {
        reportError("Info file content not found");
        return false;
    }
    
    try {
        info.parseFromContent(it->second);
        
        // 验证解析结果
        if (!info.isValid()) {
            reportError("Parsed info is invalid");
            return false;
        }
        
        return true;
    } catch (const std::exception& e) {
        reportError("Failed to parse info content: " + String(e.what()));
        return false;
    }
}

void CMODPackage::generateExportTable() {
    // 这里应该分析所有模块文件，提取导出的Custom、Template、Origin等
    // 简化实现，暂时跳过自动生成
    // 实际实现时需要调用Parser分析AST，提取导出项
}

String CMODPackage::readFile(const String& filePath) const {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        return "";
    }
    
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    return content;
}

bool CMODPackage::fileExists(const String& filePath) const {
    return std::filesystem::exists(filePath) && std::filesystem::is_regular_file(filePath);
}

bool CMODPackage::directoryExists(const String& dirPath) const {
    return std::filesystem::exists(dirPath) && std::filesystem::is_directory(dirPath);
}

StringVector CMODPackage::listDirectory(const String& dirPath) const {
    StringVector contents;
    
    try {
        for (const auto& entry : std::filesystem::directory_iterator(dirPath)) {
            contents.push_back(entry.path().filename().string());
        }
    } catch (const std::filesystem::filesystem_error&) {
        // 目录不存在或无法访问
    }
    
    return contents;
}

void CMODPackage::reportError(const String& message) {
    errors.push_back(message);
}

// CMODFactory 实现
std::unique_ptr<CMODPackage> CMODFactory::loadPackage(const String& packagePath) {
    auto package = std::make_unique<CMODPackage>(packagePath);
    
    if (!package->load()) {
        return nullptr;
    }
    
    if (!package->parse()) {
        return nullptr;
    }
    
    return package;
}

std::unique_ptr<CMODPackager> CMODFactory::createPackager(const CMODPackager::PackagingOptions& options) {
    return std::make_unique<CMODPackager>(options);
}

std::unique_ptr<CMODUnpacker> CMODFactory::createUnpacker(const CMODUnpacker::UnpackingOptions& options) {
    return std::make_unique<CMODUnpacker>(options);
}

std::unique_ptr<CMODManager> CMODFactory::createManager(const CMODManager::ManagerConfig& config) {
    return std::make_unique<CMODManager>();
}

bool CMODFactory::quickPackage(const String& sourceDir, const String& outputPath) {
    CMODPackager::PackagingOptions options;
    auto packager = createPackager(options);
    return packager->packageDirectory(sourceDir, outputPath);
}

bool CMODFactory::quickUnpackage(const String& cmodPath, const String& outputDir) {
    CMODUnpacker::UnpackingOptions options;
    auto unpacker = createUnpacker(options);
    return unpacker->unpackage(cmodPath, outputDir);
}

CMODStructure CMODFactory::quickAnalyze(const String& path) {
    if (std::filesystem::is_directory(path)) {
        // 分析目录结构
        CMODPackage package(path);
        if (package.load()) {
            return package.getStructure();
        }
    } else if (std::filesystem::is_regular_file(path) && 
               std::filesystem::path(path).extension() == ".cmod") {
        // 分析CMOD文件
        CMODUnpacker::UnpackingOptions options;
        auto unpacker = createUnpacker(options);
        return unpacker->analyzeCMODFile(path);
    }
    
    return CMODStructure(); // 返回无效结构
}

} // namespace CHTL