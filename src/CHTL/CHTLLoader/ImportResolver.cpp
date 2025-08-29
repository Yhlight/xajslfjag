#include "ImportResolver.h"
#include <filesystem>
#include <algorithm>
#include <queue>

namespace fs = std::filesystem;

namespace CHTL {

ImportResolver::ImportResolver(const ImportResolverConfig& config)
    : config_(config) {
    // 规范化路径
    config_.officialModuleDir = normalizePath(config_.officialModuleDir);
    config_.currentDir = normalizePath(config_.currentDir);
}

std::optional<ResolvedImport> ImportResolver::resolve(ImportNode* importNode) {
    ResolvedImport result;
    result.importType = importNode->getImportType();
    
    const std::string& fromPath = importNode->getFromPath();
    
    // 根据导入类型确定期望的文件类型
    FileType expectedType = FileType::UNKNOWN;
    switch (importNode->getImportType()) {
        case ImportType::HTML:
            expectedType = FileType::HTML;
            break;
        case ImportType::STYLE:
            expectedType = FileType::CSS;
            break;
        case ImportType::JAVASCRIPT:
            expectedType = FileType::JAVASCRIPT;
            break;
        case ImportType::CHTL:
        case ImportType::TEMPLATE_STYLE:
        case ImportType::TEMPLATE_ELEMENT:
        case ImportType::TEMPLATE_VAR:
        case ImportType::CUSTOM_STYLE:
        case ImportType::CUSTOM_ELEMENT:
        case ImportType::CUSTOM_VAR:
        case ImportType::ORIGIN:
        case ImportType::ALL_TEMPLATE:
        case ImportType::ALL_CUSTOM:
        case ImportType::ALL_ORIGIN:
            expectedType = FileType::CHTL;
            break;
        case ImportType::CJMOD:
            expectedType = FileType::CJMOD;
            break;
        case ImportType::CONFIG:
            expectedType = FileType::CHTL;
            break;
    }
    
    // 解析路径
    auto resolvedPath = resolvePath(fromPath, expectedType);
    if (!resolvedPath.has_value()) {
        return std::nullopt;
    }
    
    result.filePath = resolvedPath.value();
    result.fileType = detectFileType(result.filePath);
    
    // 设置命名空间
    if (config_.enableDefaultNamespace && result.fileType == FileType::CHTL) {
        result.namespaceName = getDefaultNamespace(result.filePath);
    }
    
    // 检查是否为官方模块
    if (result.filePath.find(config_.officialModuleDir) == 0) {
        result.isOfficialModule = true;
    }
    
    return result;
}

std::optional<std::string> ImportResolver::resolvePath(const std::string& path, FileType expectedType) {
    // 处理官方模块前缀
    if (hasOfficialModulePrefix(path)) {
        std::string moduleName = removeOfficialModulePrefix(path);
        return resolveInOfficialModules(moduleName, expectedType);
    }
    
    // 检查是否为绝对路径
    if (fs::path(path).is_absolute()) {
        return resolveAbsolutePath(path);
    }
    
    // 检查是否包含路径分隔符（相对路径）
    if (path.find('/') != std::string::npos || path.find('\\') != std::string::npos) {
        std::string fullPath = joinPath(config_.currentDir, path);
        if (fs::exists(fullPath)) {
            return normalizePath(fullPath);
        }
        return std::nullopt;
    }
    
    // 搜索顺序：官方模块 -> 当前目录module文件夹 -> 当前目录
    std::optional<std::string> result;
    
    // 1. 官方模块目录
    result = resolveInOfficialModules(path, expectedType);
    if (result.has_value()) return result;
    
    // 2. 当前目录的module文件夹
    result = resolveInCurrentModules(path, expectedType);
    if (result.has_value()) return result;
    
    // 3. 当前目录
    result = resolveInCurrentDir(path, expectedType);
    if (result.has_value()) return result;
    
    return std::nullopt;
}

std::optional<std::string> ImportResolver::resolveInOfficialModules(const std::string& name, FileType type) {
    if (config_.officialModuleDir.empty() || !fs::exists(config_.officialModuleDir)) {
        return std::nullopt;
    }
    
    // 检查是否有模块结构（CMOD/CJMOD子目录）
    if (hasModuleStructure(config_.officialModuleDir)) {
        if (type == FileType::CJMOD) {
            std::string cjmodDir = getCJMODSubdir(config_.officialModuleDir);
            return searchFile(cjmodDir, name, type);
        } else if (type == FileType::CHTL || type == FileType::CMOD) {
            std::string cmodDir = getCMODSubdir(config_.officialModuleDir);
            return searchFile(cmodDir, name, type);
        }
    }
    
    // 直接在官方模块目录搜索
    return searchFile(config_.officialModuleDir, name, type);
}

std::optional<std::string> ImportResolver::resolveInCurrentModules(const std::string& name, FileType type) {
    std::string moduleDir = joinPath(config_.currentDir, "module");
    if (!fs::exists(moduleDir)) {
        return std::nullopt;
    }
    
    // 检查是否有模块结构
    if (hasModuleStructure(moduleDir)) {
        if (type == FileType::CJMOD) {
            std::string cjmodDir = getCJMODSubdir(moduleDir);
            return searchFile(cjmodDir, name, type);
        } else if (type == FileType::CHTL || type == FileType::CMOD) {
            std::string cmodDir = getCMODSubdir(moduleDir);
            return searchFile(cmodDir, name, type);
        }
    }
    
    return searchFile(moduleDir, name, type);
}

std::optional<std::string> ImportResolver::resolveInCurrentDir(const std::string& name, FileType type) {
    return searchFile(config_.currentDir, name, type);
}

std::optional<std::string> ImportResolver::searchFile(const std::string& dir, const std::string& name, 
                                                     FileType type, bool checkSubdirs) {
    if (!fs::exists(dir) || !fs::is_directory(dir)) {
        return std::nullopt;
    }
    
    // 获取可能的文件扩展名
    auto extensions = getFileExtensions(type);
    
    // 如果名称已包含扩展名
    if (name.find('.') != std::string::npos) {
        std::string fullPath = joinPath(dir, name);
        if (fs::exists(fullPath) && matchesFileType(fullPath, type)) {
            return normalizePath(fullPath);
        }
    } else {
        // 尝试不同的扩展名
        for (const auto& ext : extensions) {
            std::string filename = name + ext;
            std::string fullPath = joinPath(dir, filename);
            if (fs::exists(fullPath)) {
                return normalizePath(fullPath);
            }
        }
    }
    
    // 如果需要检查子目录（用于子模块）
    if (checkSubdirs) {
        for (const auto& entry : fs::directory_iterator(dir)) {
            if (entry.is_directory()) {
                auto result = searchFile(entry.path().string(), name, type, false);
                if (result.has_value()) {
                    return result;
                }
            }
        }
    }
    
    return std::nullopt;
}

FileType ImportResolver::detectFileType(const std::string& path) {
    std::string ext = fs::path(path).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == ".html" || ext == ".htm") return FileType::HTML;
    if (ext == ".css") return FileType::CSS;
    if (ext == ".js" || ext == ".cjjs") return FileType::JAVASCRIPT;
    if (ext == ".chtl") return FileType::CHTL;
    if (ext == ".cmod") return FileType::CMOD;
    if (ext == ".cjmod") return FileType::CJMOD;
    
    return FileType::UNKNOWN;
}

std::vector<std::string> ImportResolver::getFileExtensions(FileType type) {
    switch (type) {
        case FileType::HTML:
            return {".html", ".htm"};
        case FileType::CSS:
            return {".css"};
        case FileType::JAVASCRIPT:
            return {".js", ".cjjs"};
        case FileType::CHTL:
            return {".cmod", ".chtl"};  // CMOD优先
        case FileType::CMOD:
            return {".cmod"};
        case FileType::CJMOD:
            return {".cjmod"};
        default:
            return {};
    }
}

bool ImportResolver::hasCircularDependency(const std::string& fromFile, const std::string& toFile) {
    if (!config_.checkCircularDependency) {
        return false;
    }
    
    // 使用BFS检测循环依赖
    std::queue<std::string> toVisit;
    std::unordered_set<std::string> visited;
    
    toVisit.push(toFile);
    
    while (!toVisit.empty()) {
        std::string current = toVisit.front();
        toVisit.pop();
        
        if (current == fromFile) {
            return true;  // 找到循环依赖
        }
        
        if (visited.find(current) != visited.end()) {
            continue;
        }
        
        visited.insert(current);
        
        // 添加当前文件导入的所有文件
        auto it = importGraph_.find(current);
        if (it != importGraph_.end()) {
            for (const auto& imported : it->second) {
                toVisit.push(imported);
            }
        }
    }
    
    return false;
}

void ImportResolver::addImportedFile(const std::string& fromFile, const std::string& toFile) {
    importGraph_[fromFile].insert(toFile);
}

std::string ImportResolver::getDefaultNamespace(const std::string& filePath) {
    return getBasename(filePath);
}

std::string ImportResolver::normalizePath(const std::string& path) {
    return fs::path(path).lexically_normal().string();
}

std::string ImportResolver::joinPath(const std::string& dir, const std::string& file) {
    return (fs::path(dir) / file).string();
}

std::string ImportResolver::getDirectory(const std::string& path) {
    return fs::path(path).parent_path().string();
}

std::string ImportResolver::getFilename(const std::string& path) {
    return fs::path(path).filename().string();
}

std::string ImportResolver::getBasename(const std::string& path) {
    return fs::path(path).stem().string();
}

bool ImportResolver::hasModuleStructure(const std::string& dir) {
    // 检查是否存在CMOD或CJMOD子目录
    std::vector<std::string> possibleDirs = {
        "CMOD", "cmod", "Cmod",
        "CJMOD", "cjmod", "CJmod"
    };
    
    for (const auto& subdir : possibleDirs) {
        if (fs::exists(joinPath(dir, subdir))) {
            return true;
        }
    }
    
    return false;
}

std::string ImportResolver::getCMODSubdir(const std::string& dir) {
    std::vector<std::string> possibleDirs = {"CMOD", "cmod", "Cmod"};
    
    for (const auto& subdir : possibleDirs) {
        std::string path = joinPath(dir, subdir);
        if (fs::exists(path)) {
            return path;
        }
    }
    
    return dir;  // 默认返回原目录
}

std::string ImportResolver::getCJMODSubdir(const std::string& dir) {
    std::vector<std::string> possibleDirs = {"CJMOD", "cjmod", "CJmod"};
    
    for (const auto& subdir : possibleDirs) {
        std::string path = joinPath(dir, subdir);
        if (fs::exists(path)) {
            return path;
        }
    }
    
    return dir;  // 默认返回原目录
}

bool ImportResolver::hasOfficialModulePrefix(const std::string& path) {
    return path.find("chtl::") == 0;
}

std::string ImportResolver::removeOfficialModulePrefix(const std::string& path) {
    if (hasOfficialModulePrefix(path)) {
        return path.substr(6);  // 移除 "chtl::"
    }
    return path;
}

std::vector<std::string> ImportResolver::resolveWildcard(const std::string& pattern, FileType type) {
    std::vector<std::string> results;
    
    // 解析通配符模式
    size_t wildcardPos = pattern.find('*');
    if (wildcardPos == std::string::npos) {
        // 没有通配符，直接返回
        auto resolved = resolvePath(pattern, type);
        if (resolved.has_value()) {
            results.push_back(resolved.value());
        }
        return results;
    }
    
    // 提取目录和模式
    std::string dir = getDirectory(pattern);
    std::string filePattern = getFilename(pattern);
    
    // 在目录中查找匹配的文件
    if (fs::exists(dir) && fs::is_directory(dir)) {
        for (const auto& entry : fs::directory_iterator(dir)) {
            if (entry.is_regular_file()) {
                std::string filename = entry.path().filename().string();
                // 简单的通配符匹配（只支持*）
                if (filePattern == "*" || filePattern == "*.*") {
                    if (matchesFileType(entry.path().string(), type)) {
                        results.push_back(normalizePath(entry.path().string()));
                    }
                } else if (filePattern.find("*.") == 0) {
                    // 匹配特定扩展名
                    std::string ext = filePattern.substr(1);
                    if (filename.size() > ext.size() && 
                        filename.substr(filename.size() - ext.size()) == ext) {
                        results.push_back(normalizePath(entry.path().string()));
                    }
                }
            }
        }
    }
    
    return results;
}

bool ImportResolver::matchesFileType(const std::string& path, FileType type) {
    FileType actualType = detectFileType(path);
    
    // 特殊处理：CHTL类型可以匹配CHTL和CMOD文件
    if (type == FileType::CHTL) {
        return actualType == FileType::CHTL || actualType == FileType::CMOD;
    }
    
    return actualType == type;
}

// DefaultFileSystem 实现

bool DefaultFileSystem::exists(const std::string& path) {
    return fs::exists(path);
}

bool DefaultFileSystem::isDirectory(const std::string& path) {
    return fs::is_directory(path);
}

bool DefaultFileSystem::isFile(const std::string& path) {
    return fs::is_regular_file(path);
}

std::vector<std::string> DefaultFileSystem::listDirectory(const std::string& path) {
    std::vector<std::string> results;
    
    if (fs::exists(path) && fs::is_directory(path)) {
        for (const auto& entry : fs::directory_iterator(path)) {
            results.push_back(entry.path().filename().string());
        }
    }
    
    return results;
}

std::string DefaultFileSystem::getCurrentDirectory() {
    return fs::current_path().string();
}

std::string DefaultFileSystem::getAbsolutePath(const std::string& path) {
    return fs::absolute(path).string();
}

} // namespace CHTL