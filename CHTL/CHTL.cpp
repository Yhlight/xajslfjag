#include "CHTL.h"

namespace CHTL {

std::string compileToHTML(const std::string& chtl_source, const CompileOptions& options) {
    auto result = compile(chtl_source, options);
    return result.html;
}

std::string compileToCSS(const std::string& chtl_source, const CompileOptions& options) {
    auto result = compile(chtl_source, options);
    return result.css;
}

std::string compileToJS(const std::string& chtl_source, const CompileOptions& options) {
    auto result = compile(chtl_source, options);
    return result.js;
}

CompileResult compile(const std::string& chtl_source, const CompileOptions& options) {
    CompileResult result;
    result.success = false;
    
    try {
        // Create manager and configure it
        CHTLManager manager;
        manager.setPrettyPrint(options.pretty_print);
        manager.setDebugMode(options.debug_mode);
        manager.setStrictMode(options.strict_mode);
        
        for (const auto& path : options.include_paths) {
            manager.addIncludePath(path);
        }
        
        // Compile the source
        auto manager_result = manager.compileString(chtl_source);
        
        result.success = manager_result.success;
        result.html = manager_result.html;
        result.css = manager_result.css;
        result.js = manager_result.js;
        result.errors = manager_result.errors;
        
    } catch (const std::exception& e) {
        result.errors.push_back("Compilation failed: " + std::string(e.what()));
    }
    
    return result;
}

CompileResult compileFile(const std::string& filepath, const CompileOptions& options) {
    CompileResult result;
    result.success = false;
    
    try {
        // Create manager and configure it
        CHTLManager manager;
        manager.setPrettyPrint(options.pretty_print);
        manager.setDebugMode(options.debug_mode);
        manager.setStrictMode(options.strict_mode);
        
        for (const auto& path : options.include_paths) {
            manager.addIncludePath(path);
        }
        
        // Compile the file
        auto manager_result = manager.compileFile(filepath);
        
        result.success = manager_result.success;
        result.html = manager_result.html;
        result.css = manager_result.css;
        result.js = manager_result.js;
        result.errors = manager_result.errors;
        
    } catch (const std::exception& e) {
        result.errors.push_back("File compilation failed: " + std::string(e.what()));
    }
    
    return result;
}

const char* getVersion() {
    return VERSION;
}

int getVersionMajor() {
    return VERSION_MAJOR;
}

int getVersionMinor() {
    return VERSION_MINOR;
}

int getVersionPatch() {
    return VERSION_PATCH;
}

} // namespace CHTL