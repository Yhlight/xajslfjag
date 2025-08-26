#pragma once

#include <string>
#include <memory>
#include <vector>
#include "CHTL/Core/Ast.h"
#include "CHTL/Core/Config.h"

namespace CHTL {

    class ImportResolver {
    public:
        ImportResolver(const Config& config, const std::string& currentFilePath);
        
        void Resolve(std::shared_ptr<ProgramNode> ast);
        
    private:
        void Visit(AstNodePtr node);
        void ResolveImport(const ImportNode& node);
        
        // Path resolution methods
        std::string ResolvePath(const std::string& importPath, ImportType type);
        std::string ResolveOfficialModule(const std::string& moduleName, ImportType type);
        std::string ResolveLocalModule(const std::string& moduleName, ImportType type);
        std::string ResolveWildcardPath(const std::string& path);
        
        // Helper methods
        std::string GetOfficialModulePath() const;
        std::string GetLocalModulePath() const;
        std::string GetCurrentDirectory() const;
        std::vector<std::string> FindModulesInDirectory(const std::string& dir, const std::string& pattern);
        bool IsFileName(const std::string& path) const;
        bool HasExtension(const std::string& path) const;
        std::string GetExtensionForType(ImportType type) const;
        
    private:
        const Config& m_Config;
        std::string m_CurrentFilePath;
        std::string m_CurrentDirectory;
    };

}