#include "ImportManager.h"
#include <regex>
#include <algorithm>
#include <sstream>
#include <fstream>
#include <filesystem>

namespace CHTL {
namespace Common {

// ImportItem实现
bool ImportItem::requiresAlias() const {
    return type == ImportType::HTML_FILE || 
           type == ImportType::STYLE_FILE || 
           type == ImportType::JAVASCRIPT_FILE;
}

bool ImportItem::shouldSkip() const {
    return requiresAlias() && !has_alias;
}

// ImportManager实现
ImportManager::ImportManager() 
    : use_module_dir_structure(false), import_counter(0) {
    current_directory = ".";
    official_module_dir = "./module";
}

ImportManager::~ImportManager() = default;

void ImportManager::addModuleSearchPath(const std::string& path) {
    if (std::find(module_search_paths.begin(), module_search_paths.end(), path) == module_search_paths.end()) {
        module_search_paths.push_back(path);
    }
}

ImportItem ImportManager::parseImportStatement(const std::string& statement) {
    ImportItem item;
    
    std::string type_part, target_part, path_part, alias_part;
    if (!ImportUtils::parseImportStatement(statement, type_part, target_part, path_part, alias_part)) {
        addError("Failed to parse import statement: " + statement);
        return item;
    }
    
    // 解析导入类型
    item.type = parseImportType(type_part);
    item.target_name = target_part;
    item.import_path = ImportUtils::stripQuotes(path_part);
    
    if (!alias_part.empty()) {
        item.setAlias(alias_part);
    }
    
    return item;
}

// 官方模块前缀处理 (chtl::)
bool ImportManager::isOfficialModulePrefix(const std::string& path) {
    return path.substr(0, 6) == "chtl::";
}

std::string ImportManager::extractOfficialModuleName(const std::string& path) {
    if (isOfficialModulePrefix(path)) {
        return path.substr(6); // 移除 "chtl::" 前缀
    }
    return path;
}

std::string ImportManager::resolveOfficialModulePath(const std::string& module_name) {
    // 在官方模块目录中搜索
    std::string official_path = joinPath(official_module_dir, module_name);
    
    // 搜索.cmod文件
    std::string cmod_path = official_path + ".cmod";
    if (fileExists(cmod_path)) {
        return cmod_path;
    }
    
    // 搜索.chtl文件
    std::string chtl_path = official_path + ".chtl";
    if (fileExists(chtl_path)) {
        return chtl_path;
    }
    
    return "";
}

// 通配符导入处理 (.* 和 /*)
bool ImportManager::isWildcardImport(const std::string& path) {
    return path.find(".*") != std::string::npos || 
           path.find("/*") != std::string::npos;
}

std::vector<std::string> ImportManager::expandWildcardPath(const std::string& wildcard_path) {
    std::vector<std::string> results;
    
    if (!isWildcardImport(wildcard_path)) {
        results.push_back(wildcard_path);
        return results;
    }
    
    // 解析通配符路径
    std::string base_path;
    std::string pattern;
    
    size_t wildcard_pos = wildcard_path.find(".*");
    if (wildcard_pos != std::string::npos) {
        base_path = wildcard_path.substr(0, wildcard_pos);
        pattern = wildcard_path.substr(wildcard_pos + 2); // 跳过 ".*"
    } else {
        wildcard_pos = wildcard_path.find("/*");
        if (wildcard_pos != std::string::npos) {
            base_path = wildcard_path.substr(0, wildcard_pos);
            pattern = wildcard_path.substr(wildcard_pos + 2); // 跳过 "/*"
        }
    }
    
    // 查找匹配的文件
    results = findMatchingFiles(base_path, pattern);
    
    return results;
}

std::vector<std::string> ImportManager::findMatchingFiles(const std::string& directory, const std::string& pattern) {
    std::vector<std::string> matches;
    
    if (!directoryExists(directory)) {
        return matches;
    }
    
    auto files = listDirectory(directory);
    
    for (const auto& file : files) {
        if (pattern.empty()) {
            // 无特定模式，包含所有.cmod和.chtl文件
            std::string ext = getFileExtension(file);
            if (ext == ".cmod" || ext == ".chtl") {
                matches.push_back(joinPath(directory, file));
            }
        } else if (pattern == ".cmod") {
            // 只包含.cmod文件
            if (getFileExtension(file) == ".cmod") {
                matches.push_back(joinPath(directory, file));
            }
        } else if (pattern == ".chtl") {
            // 只包含.chtl文件
            if (getFileExtension(file) == ".chtl") {
                matches.push_back(joinPath(directory, file));
            }
        }
    }
    
    return matches;
}

// 子模块访问处理 (Module.Submodule)
bool ImportManager::isSubmoduleAccess(const std::string& path) {
    // 检查是否包含子模块分隔符 ('.' 或 '/')，但不是通配符
    return path.find('.') != std::string::npos && path.find(".*") == std::string::npos;
}

std::vector<std::string> ImportManager::parseSubmodulePath(const std::string& path) {
    std::vector<std::string> components;
    
    // 支持 '.' 和 '/' 作为分隔符
    std::string delimiter = ".";
    if (path.find('/') != std::string::npos) {
        delimiter = "/";
    }
    
    std::string temp = path;
    size_t pos = 0;
    
    while ((pos = temp.find(delimiter)) != std::string::npos) {
        std::string component = temp.substr(0, pos);
        if (!component.empty()) {
            components.push_back(component);
        }
        temp.erase(0, pos + delimiter.length());
    }
    
    if (!temp.empty()) {
        components.push_back(temp);
    }
    
    return components;
}

std::string ImportManager::resolveSubmodulePath(const std::string& module_path) {
    auto components = parseSubmodulePath(module_path);
    
    if (components.empty()) {
        return "";
    }
    
    // 构建子模块路径
    std::string current_path = components[0];
    
    // 检查是否是官方模块
    if (isOfficialModulePrefix(current_path)) {
        current_path = resolveOfficialModulePath(extractOfficialModuleName(current_path));
        if (current_path.empty()) {
            return "";
        }
        
        // 对于官方模块，需要在模块内部查找子模块
        // 这里简化处理，实际应该解析.cmod文件结构
        for (size_t i = 1; i < components.size(); ++i) {
            current_path = joinPath(current_path, components[i]);
        }
    } else {
        // 普通模块路径解析
        for (size_t i = 1; i < components.size(); ++i) {
            current_path = joinPath(current_path, components[i]);
        }
        
        // 尝试添加扩展名
        if (!fileExists(current_path + ".cmod") && !fileExists(current_path + ".chtl")) {
            if (fileExists(current_path + ".cmod")) {
                current_path += ".cmod";
            } else if (fileExists(current_path + ".chtl")) {
                current_path += ".chtl";
            }
        }
    }
    
    return current_path;
}

bool ImportManager::addImport(const ImportItem& item) {
    if (!validateImport(item)) {
        return false;
    }
    
    if (isDuplicateImport(item)) {
        addWarning("Duplicate import detected: " + item.target_name);
        return false;
    }
    
    std::string import_id = generateImportId();
    imports[import_id] = item;
    
    return true;
}

bool ImportManager::processImport(const std::string& import_id) {
    auto it = imports.find(import_id);
    if (it == imports.end()) {
        addError("Import not found: " + import_id);
        return false;
    }
    
    ImportItem& item = it->second;
    
    if (item.is_processed) {
        return true;
    }
    
    // 检查是否需要跳过（如@Html without as）
    if (item.shouldSkip()) {
        item.is_processed = true;
        addWarning("Skipping import without alias: " + item.target_name);
        return true;
    }
    
    // 解析路径
    PathResolution resolution;
    
    if (isOfficialModulePrefix(item.import_path)) {
        // 官方模块处理
        std::string module_name = extractOfficialModuleName(item.import_path);
        item.resolved_path = resolveOfficialModulePath(module_name);
    } else if (isWildcardImport(item.import_path)) {
        // 通配符导入处理
        auto expanded_paths = expandWildcardPath(item.import_path);
        if (!expanded_paths.empty()) {
            item.resolved_path = expanded_paths[0]; // 使用第一个匹配
            // 其他匹配可以作为额外的导入项处理
        }
    } else if (isSubmoduleAccess(item.import_path)) {
        // 子模块访问处理
        item.resolved_path = resolveSubmodulePath(item.import_path);
    } else {
        // 普通路径解析
        if (item.type == ImportType::CHTL_FILE) {
            resolution = resolveChtlPath(item.import_path);
        } else if (item.type == ImportType::CJMOD_FILE) {
            resolution = resolveCJmodPath(item.import_path);
        } else {
            resolution = resolveFilePath(item.import_path, item.type);
        }
        
        if (resolution.success) {
            item.resolved_path = resolution.resolved_path;
        }
    }
    
    if (item.resolved_path.empty()) {
        addError("Failed to resolve import path: " + item.import_path);
        return false;
    }
    
    // 检查循环依赖
    if (hasCircularDependency(import_id)) {
        addError("Circular dependency detected for import: " + item.target_name);
        return false;
    }
    
    // 创建带名原始嵌入节点（如果需要）
    if (item.has_alias) {
        createNamedOriginNode(item);
    }
    
    item.is_processed = true;
    processed_imports.insert(import_id);
    
    return true;
}

bool ImportManager::processAllImports() {
    bool all_success = true;
    
    for (auto& pair : imports) {
        if (!processImport(pair.first)) {
            all_success = false;
        }
    }
    
    return all_success;
}

PathResolution ImportManager::resolveFilePath(const std::string& path, ImportType type) {
    PathResolution resolution;
    resolution.original_path = path;
    resolution.type = classifyPath(path);
    
    std::vector<std::string> extensions = getFileExtensions(type);
    std::vector<std::string> search_dirs = {current_directory};
    
    // 根据路径类型进行解析
    switch (resolution.type) {
        case PathResolutionType::NAME_ONLY: {
            for (const auto& dir : search_dirs) {
                auto candidates = searchInDirectory(dir, path, extensions);
                resolution.candidates.insert(resolution.candidates.end(), candidates.begin(), candidates.end());
            }
            break;
        }
        
        case PathResolutionType::SPECIFIC_NAME: {
            for (const auto& dir : search_dirs) {
                std::string full_path = joinPath(dir, path);
                if (fileExists(full_path)) {
                    resolution.candidates.push_back(full_path);
                }
            }
            break;
        }
        
        case PathResolutionType::SPECIFIC_PATH: {
            if (fileExists(path)) {
                resolution.candidates.push_back(path);
            }
            break;
        }
        
        case PathResolutionType::DIRECTORY_PATH: {
            resolution.setError("Directory path not allowed for file imports");
            return resolution;
        }
        
        case PathResolutionType::WILDCARD_PATH: {
            auto expanded = expandWildcardPath(path);
            resolution.candidates.insert(resolution.candidates.end(), expanded.begin(), expanded.end());
            break;
        }
    }
    
    if (!resolution.candidates.empty()) {
        resolution.resolved_path = findBestMatch(resolution.candidates, type);
        resolution.success = !resolution.resolved_path.empty();
    } else {
        resolution.setError("No matching files found for: " + path);
    }
    
    return resolution;
}

PathResolution ImportManager::resolveChtlPath(const std::string& path) {
    PathResolution resolution;
    resolution.original_path = path;
    resolution.type = classifyPath(path);
    
    // CHTL模块搜索顺序：官方模块目录 -> 当前目录module文件夹 -> 当前目录
    std::vector<std::string> search_dirs = {
        official_module_dir,
        joinPath(current_directory, "module"),
        current_directory
    };
    
    std::vector<std::string> extensions = {".cmod", ".chtl"};
    
    for (const auto& dir : search_dirs) {
        if (!directoryExists(dir)) continue;
        
        switch (resolution.type) {
            case PathResolutionType::NAME_ONLY: {
                auto candidates = searchInDirectory(dir, path, extensions);
                resolution.candidates.insert(resolution.candidates.end(), candidates.begin(), candidates.end());
                break;
            }
            
            case PathResolutionType::SPECIFIC_NAME: {
                std::string full_path = joinPath(dir, path);
                if (fileExists(full_path)) {
                    resolution.candidates.push_back(full_path);
                }
                break;
            }
            
            case PathResolutionType::SPECIFIC_PATH: {
                if (fileExists(path)) {
                    resolution.candidates.push_back(path);
                }
                break;
            }
        }
    }
    
    if (!resolution.candidates.empty()) {
        resolution.resolved_path = findBestMatch(resolution.candidates, ImportType::CHTL_FILE);
        resolution.success = !resolution.resolved_path.empty();
    } else {
        resolution.setError("CHTL module not found: " + path);
    }
    
    return resolution;
}

PathResolution ImportManager::resolveCJmodPath(const std::string& path) {
    PathResolution resolution;
    resolution.original_path = path;
    resolution.type = classifyPath(path);
    
    // CJmod搜索策略与CHTL相同，但只匹配.cjmod文件
    std::vector<std::string> search_dirs = {
        official_module_dir,
        joinPath(current_directory, "module"),
        current_directory
    };
    
    std::vector<std::string> extensions = {".cjmod"};
    
    for (const auto& dir : search_dirs) {
        if (!directoryExists(dir)) continue;
        
        auto candidates = searchInDirectory(dir, path, extensions);
        resolution.candidates.insert(resolution.candidates.end(), candidates.begin(), candidates.end());
    }
    
    if (!resolution.candidates.empty()) {
        resolution.resolved_path = resolution.candidates[0]; // CJmod优先第一个匹配
        resolution.success = true;
    } else {
        resolution.setError("CJmod module not found: " + path);
    }
    
    return resolution;
}

std::vector<std::string> ImportManager::resolveWildcardPath(const std::string& pattern) {
    return expandWildcardPath(pattern);
}

bool ImportManager::hasCircularDependency(const std::string& import_id) const {
    std::unordered_set<std::string> visited;
    std::unordered_set<std::string> rec_stack;
    return hasCyclicDependencyHelper(import_id, visited, rec_stack);
}

bool ImportManager::isDuplicateImport(const ImportItem& item) const {
    for (const auto& pair : imports) {
        const ImportItem& existing = pair.second;
        if (existing.target_name == item.target_name && 
            existing.type == item.type &&
            arePathsEquivalent(existing.import_path, item.import_path)) {
            return true;
        }
    }
    return false;
}

std::vector<std::string> ImportManager::checkPathExpressionConflicts() const {
    std::vector<std::string> conflicts;
    std::unordered_map<std::string, std::vector<std::string>> canonical_paths;
    
    // 将所有路径标准化并分组
    for (const auto& pair : imports) {
        const ImportItem& item = pair.second;
        std::string canonical = getCanonicalPath(item.import_path);
        canonical_paths[canonical].push_back(item.import_path);
    }
    
    // 查找有多个表达方式的路径
    for (const auto& pair : canonical_paths) {
        if (pair.second.size() > 1) {
            std::string conflict = "Path expressed in multiple ways: ";
            for (size_t i = 0; i < pair.second.size(); ++i) {
                if (i > 0) conflict += ", ";
                conflict += pair.second[i];
            }
            conflicts.push_back(conflict);
        }
    }
    
    return conflicts;
}

bool ImportManager::validateImport(const ImportItem& item) const {
    // 验证目标名称
    if (!isValidTargetName(item.target_name, item.type)) {
        return false;
    }
    
    // 验证导入路径
    if (!isValidImportPath(item.import_path)) {
        return false;
    }
    
    // 验证别名
    if (item.has_alias && !isValidAlias(item.alias_name, item.type)) {
        return false;
    }
    
    // 检查是否需要as子句
    if (requiresAsClause(item.type) && !item.has_alias) {
        return false;
    }
    
    return true;
}

bool ImportManager::requiresAsClause(ImportType type) const {
    return type == ImportType::HTML_FILE || 
           type == ImportType::STYLE_FILE || 
           type == ImportType::JAVASCRIPT_FILE;
}

bool ImportManager::createNamedOriginNode(const ImportItem& item) {
    // 创建带名原始嵌入节点的逻辑
    // 这里需要与AST系统集成
    return true;
}

bool ImportManager::isValidAlias(const std::string& alias, ImportType type) const {
    if (alias.empty()) return false;
    
    // 检查标识符有效性
    std::regex identifier_regex(R"([a-zA-Z_][a-zA-Z0-9_]*)");
    return std::regex_match(alias, identifier_regex);
}

// 内部方法实现
ImportType ImportManager::parseImportType(const std::string& type_string) const {
    if (type_string == "@Html") return ImportType::HTML_FILE;
    if (type_string == "@Style") return ImportType::STYLE_FILE;
    if (type_string == "@JavaScript") return ImportType::JAVASCRIPT_FILE;
    if (type_string == "@Chtl") return ImportType::CHTL_FILE;
    if (type_string == "@CJmod") return ImportType::CJMOD_FILE;
    if (type_string == "@Config") return ImportType::CONFIG_ITEM;
    
    if (type_string.find("[Template]") != std::string::npos) {
        if (type_string.find("@Style") != std::string::npos) return ImportType::TEMPLATE_STYLE;
        if (type_string.find("@Element") != std::string::npos) return ImportType::TEMPLATE_ELEMENT;
        if (type_string.find("@Var") != std::string::npos) return ImportType::TEMPLATE_VAR;
        return ImportType::ALL_TEMPLATES;
    }
    
    if (type_string.find("[Custom]") != std::string::npos) {
        if (type_string.find("@Style") != std::string::npos) return ImportType::CUSTOM_STYLE;
        if (type_string.find("@Element") != std::string::npos) return ImportType::CUSTOM_ELEMENT;
        if (type_string.find("@Var") != std::string::npos) return ImportType::CUSTOM_VAR;
        return ImportType::ALL_CUSTOMS;
    }
    
    if (type_string.find("[Origin]") != std::string::npos) {
        return ImportType::ORIGIN_ITEM;
    }
    
    return ImportType::UNKNOWN;
}

PathResolutionType ImportManager::classifyPath(const std::string& path) const {
    if (path.find(".*") != std::string::npos || path.find("/*") != std::string::npos) {
        return PathResolutionType::WILDCARD_PATH;
    }
    
    if (path.find('/') != std::string::npos || path.find('\\') != std::string::npos) {
        return PathResolutionType::SPECIFIC_PATH;
    }
    
    if (path.find('.') != std::string::npos && path.find(".*") == std::string::npos) {
        return PathResolutionType::SPECIFIC_NAME;
    }
    
    return PathResolutionType::NAME_ONLY;
}

bool ImportManager::fileExists(const std::string& path) const {
    std::ifstream file(path);
    return file.good();
}

bool ImportManager::directoryExists(const std::string& path) const {
    return std::filesystem::exists(path) && std::filesystem::is_directory(path);
}

std::vector<std::string> ImportManager::searchInDirectory(const std::string& dir, const std::string& name, 
                                                         const std::vector<std::string>& extensions) const {
    std::vector<std::string> results;
    
    for (const auto& ext : extensions) {
        std::string full_path = joinPath(dir, name + ext);
        if (fileExists(full_path)) {
            results.push_back(full_path);
        }
    }
    
    return results;
}

std::string ImportManager::findBestMatch(const std::vector<std::string>& candidates, ImportType type) const {
    if (candidates.empty()) return "";
    
    // 优先级：.cmod > .chtl > 其他
    for (const auto& candidate : candidates) {
        if (candidate.find(".cmod") != std::string::npos) {
            return candidate;
        }
    }
    
    for (const auto& candidate : candidates) {
        if (candidate.find(".chtl") != std::string::npos) {
            return candidate;
        }
    }
    
    return candidates[0];
}

bool ImportManager::hasCyclicDependencyHelper(const std::string& import_id, 
                                             std::unordered_set<std::string>& visited,
                                             std::unordered_set<std::string>& rec_stack) const {
    visited.insert(import_id);
    rec_stack.insert(import_id);
    
    auto it = dependencies.find(import_id);
    if (it != dependencies.end()) {
        for (const auto& dep : it->second) {
            if (rec_stack.find(dep) != rec_stack.end()) {
                return true; // 发现循环
            }
            
            if (visited.find(dep) == visited.end() && 
                hasCyclicDependencyHelper(dep, visited, rec_stack)) {
                return true;
            }
        }
    }
    
    rec_stack.erase(import_id);
    return false;
}

std::string ImportManager::getCanonicalPath(const std::string& path) const {
    // 简化的路径标准化
    std::string canonical = path;
    
    // 统一路径分隔符
    std::replace(canonical.begin(), canonical.end(), '\\', '/');
    
    // 移除相对路径标记
    while (canonical.find("./") == 0) {
        canonical = canonical.substr(2);
    }
    
    return canonical;
}

bool ImportManager::arePathsEquivalent(const std::string& path1, const std::string& path2) const {
    return getCanonicalPath(path1) == getCanonicalPath(path2);
}

std::string ImportManager::generateImportId() {
    return "import_" + std::to_string(++import_counter);
}

std::vector<std::string> ImportManager::getFileExtensions(ImportType type) const {
    switch (type) {
        case ImportType::HTML_FILE: return {".html", ".htm"};
        case ImportType::STYLE_FILE: return {".css"};
        case ImportType::JAVASCRIPT_FILE: return {".js"};
        case ImportType::CHTL_FILE: return {".chtl"};
        case ImportType::CJMOD_FILE: return {".cjmod"};
        default: return {};
    }
}

std::vector<std::string> ImportManager::listDirectory(const std::string& dir) const {
    std::vector<std::string> files;
    
    try {
        for (const auto& entry : std::filesystem::directory_iterator(dir)) {
            if (entry.is_regular_file()) {
                files.push_back(entry.path().filename().string());
            }
        }
    } catch (const std::filesystem::filesystem_error&) {
        // 目录不存在或无法访问
    }
    
    return files;
}

std::string ImportManager::getFileExtension(const std::string& path) const {
    size_t dot_pos = path.find_last_of('.');
    if (dot_pos != std::string::npos && dot_pos < path.length() - 1) {
        return path.substr(dot_pos);
    }
    return "";
}

std::string ImportManager::joinPath(const std::string& dir, const std::string& file) const {
    if (dir.empty()) return file;
    if (file.empty()) return dir;
    
    std::string result = dir;
    if (result.back() != '/' && result.back() != '\\') {
        result += "/";
    }
    result += file;
    
    return result;
}

void ImportManager::addError(const std::string& message) {
    errors.push_back(message);
}

void ImportManager::addWarning(const std::string& message) {
    warnings.push_back(message);
}

// ImportUtils实现
bool ImportUtils::parseImportStatement(const std::string& statement,
                                      std::string& type_part,
                                      std::string& target_part,
                                      std::string& path_part,
                                      std::string& alias_part) {
    // 简化的Import语句解析
    std::regex import_regex(R"(\[Import\]\s*(.+?)\s+from\s+(.+?)(?:\s+as\s+(.+))?$)");
    std::smatch match;
    
    if (std::regex_match(statement, match, import_regex)) {
        std::string import_spec = match[1].str();
        path_part = match[2].str();
        alias_part = match[3].str();
        
        // 解析import_spec以提取type和target
        size_t space_pos = import_spec.find_last_of(' ');
        if (space_pos != std::string::npos) {
            type_part = import_spec.substr(0, space_pos);
            target_part = import_spec.substr(space_pos + 1);
        } else {
            type_part = import_spec;
            target_part = "";
        }
        
        return true;
    }
    
    return false;
}

bool ImportUtils::hasOfficialPrefix(const std::string& path) {
    return path.substr(0, 6) == "chtl::";
}

std::string ImportUtils::stripOfficialPrefix(const std::string& path) {
    if (hasOfficialPrefix(path)) {
        return path.substr(6);
    }
    return path;
}

} // namespace Common
} // namespace CHTL