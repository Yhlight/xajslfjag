#include "CHTL/SemanticAnalyzer/ImportResolver.h"
#include <filesystem>
#include <stdexcept>
#include <algorithm>
#include <regex>

namespace fs = std::filesystem;

namespace CHTL {

ImportResolver::ImportResolver(const Config& config, const std::string& currentFilePath)
    : m_Config(config), m_CurrentFilePath(currentFilePath) {
    m_CurrentDirectory = fs::path(currentFilePath).parent_path().string();
}

void ImportResolver::Resolve(std::shared_ptr<ProgramNode> ast) {
    Visit(ast);
}

void ImportResolver::Visit(AstNodePtr node) {
    if (auto importNode = std::dynamic_pointer_cast<ImportNode>(node)) {
        ResolveImport(*importNode);
    }
    // TODO: Recursively visit children based on node type
}

void ImportResolver::ResolveImport(const ImportNode& node) {
    std::string fullPath = ResolvePath(node.Path, node.Type);
    if (fullPath.empty()) {
        throw std::runtime_error("Failed to resolve import path: " + node.Path);
    }
    // TODO: Load, compile, and merge the file at fullPath
}

std::string ImportResolver::ResolvePath(const std::string& importPath, ImportType type) {
    const std::string officialPrefix = "chtl::";
    std::string path = importPath;
    bool searchOfficialOnly = false;

    // Check for official module prefix
    if (importPath.rfind(officialPrefix, 0) == 0) {
        searchOfficialOnly = true;
        path = importPath.substr(officialPrefix.length());
    }

    // Handle wildcard imports
    if (path.find('*') != std::string::npos) {
        return ResolveWildcardPath(path);
    }

    // For @Html, @Style, @JavaScript types
    if (type == ImportType::Html || type == ImportType::Style || type == ImportType::JavaScript) {
        // These must be in current directory only
        if (HasExtension(path)) {
            // Specific file with extension
            fs::path fullPath = fs::path(m_CurrentDirectory) / path;
            if (fs::exists(fullPath)) {
                return fullPath.string();
            }
        } else {
            // File name without extension - search by type
            std::string ext = GetExtensionForType(type);
            fs::path fullPath = fs::path(m_CurrentDirectory) / (path + ext);
            if (fs::exists(fullPath)) {
                return fullPath.string();
            }
        }
        return "";
    }

    // For @Chtl type
    if (type == ImportType::Chtl) {
        // Handle submodule syntax (e.g., Chtholly.Space or Chtholly/Space)
        std::string modulePath = path;
        std::replace(modulePath.begin(), modulePath.end(), '.', '/');
        
        if (searchOfficialOnly) {
            return ResolveOfficialModule(modulePath, type);
        }
        
        // Search order: official -> local module -> current directory
        std::string result = ResolveOfficialModule(modulePath, type);
        if (!result.empty()) return result;
        
        result = ResolveLocalModule(modulePath, type);
        if (!result.empty()) return result;
        
        // Search in current directory
        if (HasExtension(path)) {
            fs::path fullPath = fs::path(m_CurrentDirectory) / path;
            if (fs::exists(fullPath)) return fullPath.string();
        } else {
            // Try .cmod first, then .chtl
            fs::path cmodPath = fs::path(m_CurrentDirectory) / (path + ".cmod");
            if (fs::exists(cmodPath)) return cmodPath.string();
            
            fs::path chtlPath = fs::path(m_CurrentDirectory) / (path + ".chtl");
            if (fs::exists(chtlPath)) return chtlPath.string();
        }
    }

    // For @CJmod type
    if (type == ImportType::CJmod) {
        if (searchOfficialOnly) {
            return ResolveOfficialModule(path, type);
        }
        
        // Search order: official -> local module -> current directory
        std::string result = ResolveOfficialModule(path, type);
        if (!result.empty()) return result;
        
        result = ResolveLocalModule(path, type);
        if (!result.empty()) return result;
        
        // Search in current directory
        if (HasExtension(path)) {
            fs::path fullPath = fs::path(m_CurrentDirectory) / path;
            if (fs::exists(fullPath)) return fullPath.string();
        } else {
            fs::path cjmodPath = fs::path(m_CurrentDirectory) / (path + ".cjmod");
            if (fs::exists(cjmodPath)) return cjmodPath.string();
        }
    }

    return "";
}

std::string ImportResolver::ResolveOfficialModule(const std::string& moduleName, ImportType type) {
    std::string officialPath = GetOfficialModulePath();
    
    // Check CMOD/CJMOD subdirectories structure
    if (type == ImportType::Chtl) {
        fs::path cmodDir = fs::path(officialPath) / "CMOD";
        if (fs::exists(cmodDir)) {
            // Search in CMOD directory
            if (HasExtension(moduleName)) {
                fs::path fullPath = cmodDir / moduleName;
                if (fs::exists(fullPath)) return fullPath.string();
            } else {
                // Try .cmod first, then .chtl
                fs::path cmodPath = cmodDir / (moduleName + ".cmod");
                if (fs::exists(cmodPath)) return cmodPath.string();
                
                fs::path chtlPath = cmodDir / (moduleName + ".chtl");
                if (fs::exists(chtlPath)) return chtlPath.string();
            }
        }
    } else if (type == ImportType::CJmod) {
        fs::path cjmodDir = fs::path(officialPath) / "CJMOD";
        if (fs::exists(cjmodDir)) {
            if (HasExtension(moduleName)) {
                fs::path fullPath = cjmodDir / moduleName;
                if (fs::exists(fullPath)) return fullPath.string();
            } else {
                fs::path cjmodPath = cjmodDir / (moduleName + ".cjmod");
                if (fs::exists(cjmodPath)) return cjmodPath.string();
            }
        }
    }
    
    // Fallback to mixed structure
    if (HasExtension(moduleName)) {
        fs::path fullPath = fs::path(officialPath) / moduleName;
        if (fs::exists(fullPath)) return fullPath.string();
    } else {
        if (type == ImportType::Chtl) {
            fs::path cmodPath = fs::path(officialPath) / (moduleName + ".cmod");
            if (fs::exists(cmodPath)) return cmodPath.string();
            
            fs::path chtlPath = fs::path(officialPath) / (moduleName + ".chtl");
            if (fs::exists(chtlPath)) return chtlPath.string();
        } else if (type == ImportType::CJmod) {
            fs::path cjmodPath = fs::path(officialPath) / (moduleName + ".cjmod");
            if (fs::exists(cjmodPath)) return cjmodPath.string();
        }
    }
    
    return "";
}

std::string ImportResolver::ResolveLocalModule(const std::string& moduleName, ImportType type) {
    std::string localModulePath = GetLocalModulePath();
    
    // Same logic as official module but for local module directory
    // Check CMOD/CJMOD subdirectories structure first
    if (type == ImportType::Chtl) {
        fs::path cmodDir = fs::path(localModulePath) / "CMOD";
        if (fs::exists(cmodDir)) {
            if (HasExtension(moduleName)) {
                fs::path fullPath = cmodDir / moduleName;
                if (fs::exists(fullPath)) return fullPath.string();
            } else {
                fs::path cmodPath = cmodDir / (moduleName + ".cmod");
                if (fs::exists(cmodPath)) return cmodPath.string();
                
                fs::path chtlPath = cmodDir / (moduleName + ".chtl");
                if (fs::exists(chtlPath)) return chtlPath.string();
            }
        }
    } else if (type == ImportType::CJmod) {
        fs::path cjmodDir = fs::path(localModulePath) / "CJMOD";
        if (fs::exists(cjmodDir)) {
            if (HasExtension(moduleName)) {
                fs::path fullPath = cjmodDir / moduleName;
                if (fs::exists(fullPath)) return fullPath.string();
            } else {
                fs::path cjmodPath = cjmodDir / (moduleName + ".cjmod");
                if (fs::exists(cjmodPath)) return cjmodPath.string();
            }
        }
    }
    
    // Fallback to mixed structure
    if (HasExtension(moduleName)) {
        fs::path fullPath = fs::path(localModulePath) / moduleName;
        if (fs::exists(fullPath)) return fullPath.string();
    } else {
        if (type == ImportType::Chtl) {
            fs::path cmodPath = fs::path(localModulePath) / (moduleName + ".cmod");
            if (fs::exists(cmodPath)) return cmodPath.string();
            
            fs::path chtlPath = fs::path(localModulePath) / (moduleName + ".chtl");
            if (fs::exists(chtlPath)) return chtlPath.string();
        } else if (type == ImportType::CJmod) {
            fs::path cjmodPath = fs::path(localModulePath) / (moduleName + ".cjmod");
            if (fs::exists(cjmodPath)) return cjmodPath.string();
        }
    }
    
    return "";
}

std::string ImportResolver::ResolveWildcardPath(const std::string& path) {
    // Handle wildcard imports like "path/*", "path/*.cmod", etc.
    // This would return a special marker or multiple paths
    // For now, return empty string as placeholder
    return "";
}

std::string ImportResolver::GetOfficialModulePath() const {
    // Get the path to the official module directory
    // This should be relative to the compiler executable
    // For now, use a placeholder
    return fs::current_path().string() + "/module";
}

std::string ImportResolver::GetLocalModulePath() const {
    return m_CurrentDirectory + "/module";
}

std::string ImportResolver::GetCurrentDirectory() const {
    return m_CurrentDirectory;
}

bool ImportResolver::HasExtension(const std::string& path) const {
    return path.find('.') != std::string::npos && 
           path.find('.') > path.find_last_of("/\\");
}

std::string ImportResolver::GetExtensionForType(ImportType type) const {
    switch (type) {
        case ImportType::Html: return ".html";
        case ImportType::Style: return ".css";
        case ImportType::JavaScript: return ".js";
        case ImportType::Chtl: return ".chtl";
        case ImportType::CJmod: return ".cjmod";
        default: return "";
    }
}

}
