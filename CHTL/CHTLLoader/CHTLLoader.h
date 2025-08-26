#ifndef CHTL_LOADER_H
#define CHTL_LOADER_H

#include <string>
#include <vector>
#include <memory>
#include <unordered_set>
#include <filesystem>

namespace CHTL {

class CHTLLoader {
private:
    std::vector<std::string> include_paths;
    std::unordered_set<std::string> loaded_files;
    std::vector<std::string> errors;
    
    std::string resolveIncludePath(const std::string& filepath) const;
    bool isValidChtlFile(const std::string& filepath) const;
    std::string normalizePath(const std::string& path) const;
    
public:
    CHTLLoader();
    
    // File loading
    std::string loadFile(const std::string& filepath);
    bool fileExists(const std::string& filepath) const;
    
    // Directory operations
    std::vector<std::string> findFilesInDirectory(const std::string& directory, const std::string& pattern = "*.chtl") const;
    std::vector<std::string> findModulesInDirectory(const std::string& directory) const;
    
    // Include path management
    void addIncludePath(const std::string& path);
    void removeIncludePath(const std::string& path);
    void clearIncludePaths();
    const std::vector<std::string>& getIncludePaths() const { return include_paths; }
    
    // Module loading
    std::string loadModule(const std::string& module_name);
    std::string findModuleFile(const std::string& module_name) const;
    bool isModuleLoaded(const std::string& module_name) const;
    
    // CMOD support
    std::string loadCmodFile(const std::string& cmod_path);
    bool isCmodFile(const std::string& filepath) const;
    
    // Cache management
    bool isFileLoaded(const std::string& filepath) const;
    void markFileAsLoaded(const std::string& filepath);
    void clearLoadedFiles();
    
    // Error handling
    const std::vector<std::string>& getErrors() const { return errors; }
    bool hasErrors() const { return !errors.empty(); }
    void clearErrors() { errors.clear(); }
    
    // Utility
    std::string getFileDirectory(const std::string& filepath) const;
    std::string getFileName(const std::string& filepath) const;
    std::string getFileExtension(const std::string& filepath) const;
    bool isAbsolutePath(const std::string& path) const;
    
private:
    void addError(const std::string& message);
};

} // namespace CHTL

#endif // CHTL_LOADER_H