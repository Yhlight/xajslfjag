#ifndef CMOD_SYSTEM_H
#define CMOD_SYSTEM_H

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

namespace CHTL {

// CMOD (CHTL Module) System
class CMODSystem {
private:
    std::unordered_map<std::string, std::string> loaded_modules;
    std::vector<std::string> module_paths;
    std::vector<std::string> errors;
    
    std::string extractModule(const std::string& cmod_path);
    bool validateCMODStructure(const std::string& module_dir) const;
    
public:
    CMODSystem();
    
    // Module loading
    bool loadCMOD(const std::string& cmod_path);
    bool isModuleLoaded(const std::string& module_name) const;
    std::string getModuleContent(const std::string& module_name) const;
    
    // Module paths
    void addModulePath(const std::string& path);
    void removeModulePath(const std::string& path);
    const std::vector<std::string>& getModulePaths() const { return module_paths; }
    
    // Module discovery
    std::vector<std::string> findModules() const;
    std::vector<std::string> findModulesInPath(const std::string& path) const;
    
    // Module packaging (for creating CMOD files)
    bool packageModule(const std::string& source_dir, const std::string& output_cmod) const;
    bool validateModuleStructure(const std::string& module_dir) const;
    
    // Error handling
    const std::vector<std::string>& getErrors() const { return errors; }
    bool hasErrors() const { return !errors.empty(); }
    void clearErrors() { errors.clear(); }
    
    // Utility
    void clear();
    
private:
    void addError(const std::string& message);
};

} // namespace CHTL

#endif // CMOD_SYSTEM_H