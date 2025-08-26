#ifndef CHTL_MANAGER_H
#define CHTL_MANAGER_H

#include "../CHTLLexer/CHTLLexer.h"
#include "../CHTLParser/CHTLParser.h"
#include "../CHTLGenerator/CHTLGenerator.h"
#include "../CHTLContext/CHTLContext.h"
#include "../CHTLContext/GlobalMap.h"
#include "../CHTLLoader/CHTLLoader.h"
#include <string>
#include <memory>
#include <vector>
#include <unordered_map>

namespace CHTL {

class CHTLManager {
private:
    std::unique_ptr<CHTLLexer> lexer;
    std::unique_ptr<CHTLParser> parser;
    std::unique_ptr<CHTLGenerator> generator;
    std::unique_ptr<CHTLContext> context;
    std::unique_ptr<GlobalMap> global_map;
    std::unique_ptr<CHTLLoader> loader;
    
    // Compilation state
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    // Configuration
    struct Config {
        bool pretty_print = true;
        bool debug_mode = false;
        bool strict_mode = true;
        std::string output_dir = "./output";
        std::vector<std::string> include_paths;
    } config;
    
    // Template and custom storage
    std::unordered_map<std::string, std::shared_ptr<CHTLNode>> templates;
    std::unordered_map<std::string, std::shared_ptr<CHTLNode>> customs;
    std::unordered_map<std::string, std::string> variables;
    
    // Module management
    std::unordered_set<std::string> loaded_modules;
    std::unordered_map<std::string, std::string> module_paths;
    
    void collectDefinitions(std::shared_ptr<CHTLNode> ast);
    void processImports(std::shared_ptr<CHTLNode> ast);
    void validateAST(std::shared_ptr<CHTLNode> ast);
    
public:
    CHTLManager();
    ~CHTLManager() = default;
    
    // Main compilation interface
    struct CompilationResult {
        bool success;
        std::string html;
        std::string css;
        std::string js;
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
    };
    
    CompilationResult compileFile(const std::string& filepath);
    CompilationResult compileString(const std::string& source, const std::string& context_path = "");
    
    // Batch compilation
    std::vector<CompilationResult> compileDirectory(const std::string& directory, const std::string& pattern = "*.chtl");
    
    // Configuration
    void setConfig(const std::string& key, const std::string& value);
    std::string getConfig(const std::string& key) const;
    void loadConfigFile(const std::string& config_path);
    
    void setPrettyPrint(bool enable) { config.pretty_print = enable; }
    void setDebugMode(bool enable) { config.debug_mode = enable; }
    void setStrictMode(bool enable) { config.strict_mode = enable; }
    void setOutputDirectory(const std::string& dir) { config.output_dir = dir; }
    void addIncludePath(const std::string& path) { config.include_paths.push_back(path); }
    
    // Module management
    bool loadModule(const std::string& module_name);
    bool isModuleLoaded(const std::string& module_name) const;
    void addModulePath(const std::string& name, const std::string& path);
    
    // Symbol management
    void registerTemplate(const std::string& name, std::shared_ptr<CHTLNode> template_node);
    void registerCustom(const std::string& name, std::shared_ptr<CHTLNode> custom_node);
    void registerVariable(const std::string& name, const std::string& value);
    
    std::shared_ptr<CHTLNode> getTemplate(const std::string& name) const;
    std::shared_ptr<CHTLNode> getCustom(const std::string& name) const;
    std::string getVariable(const std::string& name) const;
    
    // Error handling
    const std::vector<std::string>& getErrors() const { return errors; }
    const std::vector<std::string>& getWarnings() const { return warnings; }
    bool hasErrors() const { return !errors.empty(); }
    bool hasWarnings() const { return !warnings.empty(); }
    void clearErrors() { errors.clear(); }
    void clearWarnings() { warnings.clear(); }
    
    // Utility
    void reset();
    void printStatistics() const;
    
    // File operations
    bool saveToFile(const std::string& filepath, const std::string& content) const;
    bool saveCompilationResult(const CompilationResult& result, const std::string& base_path) const;
};

} // namespace CHTL

#endif // CHTL_MANAGER_H