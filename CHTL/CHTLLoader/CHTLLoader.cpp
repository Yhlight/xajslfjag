#include "CHTLLoader.h"
#include <fstream>
#include <sstream>
#include <algorithm>

namespace CHTL {

CHTLLoader::CHTLLoader() {
    // Add default include paths
    include_paths.push_back(".");
    include_paths.push_back("./modules");
}

std::string CHTLLoader::loadFile(const std::string& filepath) {
    std::string resolved_path = resolveIncludePath(filepath);
    
    if (resolved_path.empty()) {
        addError("File not found: " + filepath);
        return "";
    }
    
    if (!isValidChtlFile(resolved_path)) {
        addError("Invalid CHTL file: " + resolved_path);
        return "";
    }
    
    std::ifstream file(resolved_path);
    if (!file.is_open()) {
        addError("Cannot open file: " + resolved_path);
        return "";
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();
    
    markFileAsLoaded(resolved_path);
    return buffer.str();
}

bool CHTLLoader::fileExists(const std::string& filepath) const {
    std::filesystem::path path(filepath);
    return std::filesystem::exists(path) && std::filesystem::is_regular_file(path);
}

std::vector<std::string> CHTLLoader::findFilesInDirectory(const std::string& directory, const std::string& pattern) const {
    std::vector<std::string> files;
    
    try {
        std::filesystem::path dir_path(directory);
        if (!std::filesystem::exists(dir_path) || !std::filesystem::is_directory(dir_path)) {
            return files;
        }
        
        for (const auto& entry : std::filesystem::directory_iterator(dir_path)) {
            if (entry.is_regular_file()) {
                std::string filename = entry.path().filename().string();
                
                // Simple pattern matching for *.chtl
                if (pattern == "*.chtl" && filename.length() >= 5 && 
                    filename.substr(filename.length() - 5) == ".chtl") {
                    files.push_back(entry.path().string());
                } else if (pattern == "*" || filename.find(pattern) != std::string::npos) {
                    files.push_back(entry.path().string());
                }
            }
        }
    } catch (const std::filesystem::filesystem_error& e) {
        // Handle filesystem errors silently for now
    }
    
    return files;
}

std::vector<std::string> CHTLLoader::findModulesInDirectory(const std::string& directory) const {
    std::vector<std::string> modules;
    
    // Find .cmod files
    auto cmod_files = findFilesInDirectory(directory, "*.cmod");
    modules.insert(modules.end(), cmod_files.begin(), cmod_files.end());
    
    // Find .chtl files
    auto chtl_files = findFilesInDirectory(directory, "*.chtl");
    modules.insert(modules.end(), chtl_files.begin(), chtl_files.end());
    
    return modules;
}

void CHTLLoader::addIncludePath(const std::string& path) {
    std::string normalized = normalizePath(path);
    if (std::find(include_paths.begin(), include_paths.end(), normalized) == include_paths.end()) {
        include_paths.push_back(normalized);
    }
}

void CHTLLoader::removeIncludePath(const std::string& path) {
    std::string normalized = normalizePath(path);
    include_paths.erase(
        std::remove(include_paths.begin(), include_paths.end(), normalized),
        include_paths.end()
    );
}

void CHTLLoader::clearIncludePaths() {
    include_paths.clear();
    // Keep current directory
    include_paths.push_back(".");
}

std::string CHTLLoader::loadModule(const std::string& module_name) {
    std::string module_file = findModuleFile(module_name);
    if (module_file.empty()) {
        addError("Module not found: " + module_name);
        return "";
    }
    
    return loadFile(module_file);
}

std::string CHTLLoader::findModuleFile(const std::string& module_name) const {
    // Search in include paths
    for (const auto& include_path : include_paths) {
        // Try .cmod first
        std::filesystem::path cmod_path = std::filesystem::path(include_path) / (module_name + ".cmod");
        if (fileExists(cmod_path.string())) {
            return cmod_path.string();
        }
        
        // Try .chtl
        std::filesystem::path chtl_path = std::filesystem::path(include_path) / (module_name + ".chtl");
        if (fileExists(chtl_path.string())) {
            return chtl_path.string();
        }
        
        // Try without extension
        std::filesystem::path bare_path = std::filesystem::path(include_path) / module_name;
        if (fileExists(bare_path.string())) {
            return bare_path.string();
        }
    }
    
    return "";
}

bool CHTLLoader::isModuleLoaded(const std::string& module_name) const {
    std::string module_file = findModuleFile(module_name);
    return !module_file.empty() && isFileLoaded(module_file);
}

std::string CHTLLoader::loadCmodFile(const std::string& cmod_path) {
    // CMOD files are compressed/packaged CHTL modules
    // This is a simplified implementation
    if (!isCmodFile(cmod_path)) {
        addError("Not a CMOD file: " + cmod_path);
        return "";
    }
    
    // For now, treat CMOD files like regular files
    return loadFile(cmod_path);
}

bool CHTLLoader::isCmodFile(const std::string& filepath) const {
    return getFileExtension(filepath) == ".cmod";
}

bool CHTLLoader::isFileLoaded(const std::string& filepath) const {
    std::string normalized = normalizePath(filepath);
    return loaded_files.find(normalized) != loaded_files.end();
}

void CHTLLoader::markFileAsLoaded(const std::string& filepath) {
    std::string normalized = normalizePath(filepath);
    loaded_files.insert(normalized);
}

void CHTLLoader::clearLoadedFiles() {
    loaded_files.clear();
}

std::string CHTLLoader::getFileDirectory(const std::string& filepath) const {
    std::filesystem::path path(filepath);
    return path.parent_path().string();
}

std::string CHTLLoader::getFileName(const std::string& filepath) const {
    std::filesystem::path path(filepath);
    return path.filename().string();
}

std::string CHTLLoader::getFileExtension(const std::string& filepath) const {
    std::filesystem::path path(filepath);
    return path.extension().string();
}

bool CHTLLoader::isAbsolutePath(const std::string& path) const {
    std::filesystem::path fs_path(path);
    return fs_path.is_absolute();
}

std::string CHTLLoader::resolveIncludePath(const std::string& filepath) const {
    // If it's an absolute path, check if it exists
    if (isAbsolutePath(filepath)) {
        return fileExists(filepath) ? filepath : "";
    }
    
    // Search in include paths
    for (const auto& include_path : include_paths) {
        std::filesystem::path full_path = std::filesystem::path(include_path) / filepath;
        if (fileExists(full_path.string())) {
            return full_path.string();
        }
    }
    
    return "";
}

bool CHTLLoader::isValidChtlFile(const std::string& filepath) const {
    std::string ext = getFileExtension(filepath);
    return ext == ".chtl" || ext == ".cmod" || ext.empty();
}

std::string CHTLLoader::normalizePath(const std::string& path) const {
    try {
        std::filesystem::path fs_path(path);
        return fs_path.lexically_normal().string();
    } catch (const std::exception&) {
        return path;
    }
}

void CHTLLoader::addError(const std::string& message) {
    errors.push_back("Loader error: " + message);
}

} // namespace CHTL