#include "ImportManager.h"
#include <algorithm>
#include <cctype>
#include <sstream>
#include <fstream>
#include <regex>

namespace CHTL {

ImportManager::ImportManager(std::shared_ptr<ConfigurationManager> configManager, const std::string& basePath)
    : configManager_(configManager), basePath_(basePath) {
}

ImportManager::~ImportManager() {
}

std::vector<std::shared_ptr<ImportStatement>> ImportManager::parse(const std::string& source) {
    clearErrors();
    std::vector<std::shared_ptr<ImportStatement>> statements;
    
    if (source.empty()) {
        return statements;
    }
    
    std::istringstream iss(source);
    std::string line;
    int lineNumber = 1;
    
    while (std::getline(iss, line)) {
        // 查找[Import]语句
        if (line.find("[Import]") != std::string::npos) {
            auto statement = parseImportLine(line, lineNumber);
            if (statement) {
                statements.push_back(statement);
            }
        }
        lineNumber++;
    }
    
    return statements;
}

std::shared_ptr<ImportStatement> ImportManager::parseImportLine(const std::string& line, int lineNumber) {
    auto statement = std::make_shared<ImportStatement>();
    statement->lineNumber = lineNumber;
    statement->columnNumber = 1;
    
    size_t position = 0;
    
    // 跳过空白字符
    while (position < line.length() && std::isspace(line[position])) {
        position++;
    }
    
    // 匹配 "[Import]"
    if (line.substr(position, 8) != "[Import]") {
        addError("期望关键字 '[Import]'");
        return nullptr;
    }
    position += 8;
    
    // 跳过空白字符
    while (position < line.length() && std::isspace(line[position])) {
        position++;
    }
    
    // 解析导入类型
    std::string typeText;
    while (position < line.length() && line[position] != ' ' && line[position] != '\t' && line[position] != 'f') {
        typeText += line[position];
        position++;
    }
    
    statement->type = parseImportType(typeText);
    
    // 跳过空白字符
    while (position < line.length() && std::isspace(line[position])) {
        position++;
    }
    
    // 匹配 "from"
    if (line.substr(position, 4) != "from") {
        addError("期望关键字 'from'");
        return nullptr;
    }
    position += 4;
    
    // 跳过空白字符
    while (position < line.length() && std::isspace(line[position])) {
        position++;
    }
    
    // 解析路径和别名
    std::string pathText;
    while (position < line.length() && line[position] != ';' && line[position] != '{') {
        pathText += line[position];
        position++;
    }
    
    parsePathAndAlias(pathText, statement);
    
    return statement;
}

ImportType ImportManager::parseImportType(const std::string& typeText) {
    if (typeText == "@Html") return ImportType::HTML;
    if (typeText == "@Style") return ImportType::STYLE;
    if (typeText == "@JavaScript") return ImportType::JAVASCRIPT;
    if (typeText == "@Chtl") return ImportType::CHTL;
    if (typeText == "@CJmod") return ImportType::CJMOD;
    if (typeText == "@Config") return ImportType::CONFIG;
    
    // 检查是否是自定义类型
    if (typeText.find("@") == 0) {
        return ImportType::CUSTOM;
    }
    
    return ImportType::CHTL; // 默认类型
}

void ImportManager::parsePathAndAlias(const std::string& pathText, std::shared_ptr<ImportStatement>& statement) {
    // 去除首尾空白字符
    std::string trimmed = pathText;
    trimmed.erase(0, trimmed.find_first_not_of(" \t"));
    trimmed.erase(trimmed.find_last_not_of(" \t") + 1);
    
    // 检查是否有别名（as语法）
    size_t asPos = trimmed.find(" as ");
    if (asPos != std::string::npos) {
        statement->alias = trimmed.substr(asPos + 4);
        trimmed = trimmed.substr(0, asPos);
    }
    
    // 去除路径的首尾空白字符
    trimmed.erase(0, trimmed.find_first_not_of(" \t"));
    trimmed.erase(trimmed.find_last_not_of(" \t") + 1);
    
    // 检查是否是通配符导入
    if (trimmed.find("*") != std::string::npos) {
        statement->isWildcard = true;
        statement->wildcardPattern = trimmed;
    }
    
    // 检查是否是子模块导入（格式：Module.SubModule）
    if (trimmed.find(".") != std::string::npos && !statement->isWildcard) {
        // 检查是否包含文件扩展名，如果有则不可能是子模块
        std::string lowerTrimmed = trimmed;
        std::transform(lowerTrimmed.begin(), lowerTrimmed.end(), lowerTrimmed.begin(), ::tolower);
        
        bool hasExtension = lowerTrimmed.find(".html") != std::string::npos ||
                           lowerTrimmed.find(".css") != std::string::npos ||
                           lowerTrimmed.find(".js") != std::string::npos ||
                           lowerTrimmed.find(".chtl") != std::string::npos ||
                           lowerTrimmed.find(".cmod") != std::string::npos ||
                           lowerTrimmed.find(".cjmod") != std::string::npos;
        
        // 如果没有文件扩展名，且格式符合 Module.SubModule，则认为是子模块
        if (!hasExtension && trimmed.find(".") != std::string::npos) {
            auto components = parseModulePath(trimmed);
            if (components.size() >= 2) { // 至少有两个组件才算是子模块
                statement->subModules = components;
            }
        }
    }
    
    statement->path = trimmed;
}

ImportResult ImportManager::executeImport(const std::shared_ptr<ImportStatement>& statement) {
    ImportResult result;
    
    if (!statement) {
        result.errorMessage = "导入语句为空";
        return result;
    }
    
    // 检查循环依赖
    if (checkCircularDependency(basePath_, statement->path)) {
        result.errorMessage = "检测到循环依赖: " + statement->path;
        return result;
    }
    
    // 检查重复导入
    if (checkDuplicateImport(basePath_, statement->path)) {
        result.warnings.push_back("重复导入: " + statement->path);
    }
    
    try {
        if (statement->isWildcard) {
            // 处理通配符导入
            auto files = resolveWildcardPath(statement->wildcardPattern, basePath_);
            if (files.empty()) {
                result.errorMessage = "通配符路径未找到匹配文件: " + statement->wildcardPattern;
                return result;
            }
            
            result.success = true;
            result.importedFiles = files;
            
            // 读取第一个文件作为主要内容
            if (!files.empty()) {
                result.filePath = files[0];
                result.content = readFileContent(files[0]);
            }
            
        } else if (!statement->subModules.empty()) {
            // 处理子模块导入
            auto files = resolveSubModulePath(statement->path, basePath_);
            if (files.empty()) {
                result.errorMessage = "子模块路径未找到: " + statement->path;
                return result;
            }
            
            result.success = true;
            result.importedFiles = files;
            
            // 读取第一个文件作为主要内容
            if (!files.empty()) {
                result.filePath = files[0];
                result.content = readFileContent(files[0]);
            }
            
        } else {
            // 处理普通文件导入
            auto searchPaths = getSearchPaths(statement->type);
            std::string foundFile = searchFile(statement->path, statement->type, searchPaths);
            
            if (foundFile.empty()) {
                result.errorMessage = "未找到文件: " + statement->path;
                return result;
            }
            
            result.success = true;
            result.filePath = foundFile;
            result.content = readFileContent(foundFile);
        }
        
        // 记录导入历史
        if (result.success) {
            importHistory_[basePath_].push_back(statement->path);
        }
        
    } catch (const std::exception& e) {
        result.errorMessage = "导入过程中发生异常: " + std::string(e.what());
    }
    
    return result;
}

std::vector<std::string> ImportManager::resolveWildcardPath(const std::string& pattern, const std::string& basePath) {
    std::vector<std::string> files;
    
    // 解析通配符模式
    std::string dirPath = basePath;
    std::string filePattern = pattern;
    
    // 提取目录路径
    size_t lastSlash = pattern.find_last_of("/\\");
    if (lastSlash != std::string::npos) {
        dirPath = combinePath(basePath, pattern.substr(0, lastSlash));
        filePattern = pattern.substr(lastSlash + 1);
    }
    
    // 确定文件扩展名
    std::vector<std::string> extensions;
    if (filePattern.find("*.cmod") != std::string::npos) {
        extensions = {".cmod"};
    } else if (filePattern.find("*.chtl") != std::string::npos) {
        extensions = {".chtl"};
    } else if (filePattern.find("*") != std::string::npos) {
        extensions = {".cmod", ".chtl", ".html", ".css", ".js"};
    }
    
    // 获取目录中的文件
    files = getFilesInDirectory(dirPath, extensions);
    
    // 过滤匹配的文件
    std::vector<std::string> matchedFiles;
    for (const auto& file : files) {
        std::string fileName = std::filesystem::path(file).filename().string();
        if (matchWildcardPattern(filePattern, fileName)) {
            matchedFiles.push_back(file);
        }
    }
    
    return matchedFiles;
}

std::vector<std::string> ImportManager::resolveSubModulePath(const std::string& modulePath, const std::string& basePath) {
    std::vector<std::string> files;
    auto pathComponents = parseModulePath(modulePath);
    
    if (pathComponents.empty()) {
        return files;
    }
    
    // 构建搜索路径
    std::vector<std::string> searchPaths;
    
    // 官方模块路径
    searchPaths.push_back(combinePath(basePath, "module"));
    searchPaths.push_back(combinePath(basePath, "module/CMOD"));
    searchPaths.push_back(combinePath(basePath, "module/CJMOD"));
    
    // 当前目录模块路径
    searchPaths.push_back(combinePath(basePath, "Modules"));
    searchPaths.push_back(combinePath(basePath, "Modules/CMOD"));
    searchPaths.push_back(combinePath(basePath, "Modules/CJMOD"));
    
    // 搜索子模块 - 按照路径组件的层次结构构建路径
    for (const auto& searchPath : searchPaths) {
        if (isDirectory(searchPath)) {
            // 构建完整的子模块路径
            std::string currentPath = searchPath;
            bool pathValid = true;
            
            // 逐层构建路径
            for (const auto& component : pathComponents) {
                currentPath = combinePath(currentPath, component);
                if (!isDirectory(currentPath)) {
                    pathValid = false;
                    break;
                }
            }
            
            // 如果路径有效，获取该目录下的文件
            if (pathValid) {
                auto componentFiles = getFilesInDirectory(currentPath, {".cmod", ".chtl", ".cjmod"});
                files.insert(files.end(), componentFiles.begin(), componentFiles.end());
            }
        }
    }
    
    return files;
}

std::string ImportManager::searchFile(const std::string& fileName, ImportType importType, const std::vector<std::string>& searchPaths) {
    std::vector<std::string> extensions;
    
    // 根据导入类型确定文件扩展名
    switch (importType) {
        case ImportType::HTML:
            extensions = {".html", ".htm"};
            break;
        case ImportType::STYLE:
            extensions = {".css", ".scss", ".less"};
            break;
        case ImportType::JAVASCRIPT:
            extensions = {".js", ".mjs"};
            break;
        case ImportType::CHTL:
            extensions = {".cmod", ".chtl"};
            break;
        case ImportType::CJMOD:
            extensions = {".cjmod"};
            break;
        case ImportType::CONFIG:
            extensions = {".ini", ".conf", ".config"};
            break;
        default:
            extensions = {""};
            break;
    }
    
    // 如果文件名已经包含扩展名，直接搜索
    if (getFileExtension(fileName) != "") {
        for (const auto& searchPath : searchPaths) {
            std::string fullPath = combinePath(searchPath, fileName);
            if (fileExists(fullPath)) {
                return fullPath;
            }
        }
        return "";
    }
    
    // 搜索不同扩展名的文件
    for (const auto& searchPath : searchPaths) {
        for (const auto& ext : extensions) {
            std::string fullPath = combinePath(searchPath, fileName + ext);
            if (fileExists(fullPath)) {
                return fullPath;
            }
        }
    }
    
    return "";
}

std::vector<std::string> ImportManager::getSearchPaths(ImportType importType) {
    std::vector<std::string> paths;
    
    // 基础路径
    if (!basePath_.empty()) {
        paths.push_back(basePath_);
    }
    
    // 根据导入类型添加特定搜索路径
    switch (importType) {
        case ImportType::CHTL:
        case ImportType::CJMOD:
            // 官方模块路径
            paths.push_back(combinePath(basePath_, "module"));
            paths.push_back(combinePath(basePath_, "module/CMOD"));
            paths.push_back(combinePath(basePath_, "module/CJMOD"));
            
            // 当前目录模块路径
            paths.push_back(combinePath(basePath_, "Modules"));
            paths.push_back(combinePath(basePath_, "Modules/CMOD"));
            paths.push_back(combinePath(basePath_, "Modules/CJMOD"));
            break;
            
        case ImportType::HTML:
        case ImportType::STYLE:
        case ImportType::JAVASCRIPT:
            // 当前目录
            paths.push_back(basePath_);
            break;
            
        default:
            paths.push_back(basePath_);
            break;
    }
    
    return paths;
}

bool ImportManager::checkCircularDependency(const std::string& filePath, const std::string& importedFile) {
    // 检查当前导入链中是否已经包含该文件
    for (const auto& file : currentImportChain_) {
        if (file == importedFile) {
            return true;
        }
    }
    
    // 检查导入历史中是否存在循环依赖
    auto it = importHistory_.find(importedFile);
    if (it != importHistory_.end()) {
        for (const auto& imported : it->second) {
            if (imported == filePath) {
                return true;
            }
        }
    }
    
    return false;
}

bool ImportManager::checkDuplicateImport(const std::string& filePath, const std::string& importedFile) {
    auto it = importHistory_.find(filePath);
    if (it != importHistory_.end()) {
        for (const auto& imported : it->second) {
            if (imported == importedFile) {
                return true;
            }
        }
    }
    return false;
}

void ImportManager::setBasePath(const std::string& basePath) {
    basePath_ = basePath;
}

std::string ImportManager::getBasePath() const {
    return basePath_;
}

const std::unordered_map<std::string, std::vector<std::string>>& ImportManager::getImportHistory() const {
    return importHistory_;
}

void ImportManager::clearImportHistory() {
    importHistory_.clear();
}

std::vector<std::string> ImportManager::getErrors() const {
    return errors_;
}

void ImportManager::clearErrors() {
    errors_.clear();
}

void ImportManager::addError(const std::string& error) {
    errors_.push_back(error);
}

bool ImportManager::fileExists(const std::string& filePath) {
    return std::filesystem::exists(filePath);
}

std::string ImportManager::readFileContent(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        addError("无法打开文件: " + filePath);
        return "";
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

std::string ImportManager::getFileExtension(const std::string& filePath) {
    std::filesystem::path path(filePath);
    return path.extension().string();
}

std::string ImportManager::normalizePath(const std::string& path) {
    std::filesystem::path fsPath(path);
    return fsPath.lexically_normal().string();
}

std::string ImportManager::combinePath(const std::string& base, const std::string& relative) {
    std::filesystem::path basePath(base);
    std::filesystem::path relativePath(relative);
    return (basePath / relativePath).string();
}

bool ImportManager::isDirectory(const std::string& path) {
    return std::filesystem::is_directory(path);
}

std::vector<std::string> ImportManager::getFilesInDirectory(const std::string& dirPath, const std::vector<std::string>& extensions) {
    std::vector<std::string> files;
    
    if (!isDirectory(dirPath)) {
        return files;
    }
    
    try {
        for (const auto& entry : std::filesystem::directory_iterator(dirPath)) {
            if (entry.is_regular_file()) {
                std::string filePath = entry.path().string();
                std::string extension = getFileExtension(filePath);
                
                // 如果没有指定扩展名，或者扩展名匹配
                if (extensions.empty() || 
                    std::find(extensions.begin(), extensions.end(), extension) != extensions.end()) {
                    files.push_back(filePath);
                }
            }
        }
    } catch (const std::exception& e) {
        addError("读取目录失败: " + dirPath + " - " + e.what());
    }
    
    return files;
}

bool ImportManager::matchWildcardPattern(const std::string& pattern, const std::string& fileName) {
    // 简单的通配符匹配实现
    std::string regexPattern = pattern;
    
    // 转义特殊字符
    std::string escaped = "";
    for (char c : regexPattern) {
        if (c == '.' || c == '^' || c == '$' || c == '+' || c == '?' || c == '(' || c == ')' || c == '[' || c == ']' || c == '{' || c == '}') {
            escaped += '\\';
        }
        escaped += c;
    }
    
    // 将*转换为.*
    size_t pos = 0;
    while ((pos = escaped.find("*", pos)) != std::string::npos) {
        escaped.replace(pos, 1, ".*");
        pos += 2;
    }
    
    try {
        std::regex regex(escaped);
        return std::regex_match(fileName, regex);
    } catch (const std::regex_error& e) {
        addError("正则表达式错误: " + escaped + " - " + e.what());
        return false;
    }
}

std::vector<std::string> ImportManager::parseModulePath(const std::string& modulePath) {
    std::vector<std::string> components;
    std::istringstream iss(modulePath);
    std::string component;
    
    while (std::getline(iss, component, '.')) {
        if (!component.empty()) {
            components.push_back(component);
        }
    }
    
    return components;
}

} // namespace CHTL