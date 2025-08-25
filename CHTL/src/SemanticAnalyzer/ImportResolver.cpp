#include "CHTL/SemanticAnalyzer/ImportResolver.h"
#include <stdexcept>

namespace CHTL {

void ImportResolver::Resolve(std::shared_ptr<ProgramNode> ast) {
    Visit(ast);
}

void ImportResolver::Visit(AstNodePtr node) {
    if (auto importNode = std::dynamic_pointer_cast<ImportNode>(node)) {
        ResolveImport(*importNode);
    }
    // Recursively visit children
    // ... (logic to get children from node and call Visit)
}

void ImportResolver::ResolveImport(const ImportNode& node) {
    std::string fullPath = ResolvePath(node.Path);
    // ... (load, compile, and merge the file at fullPath)
}

std::string ImportResolver::ResolvePath(const std::string& importPath) {
    const std::string officialPrefix = "chtl::";
    std::string moduleName = importPath;
    bool searchOfficialOnly = false;

    if (importPath.rfind(officialPrefix, 0) == 0) { // Starts with chtl::
        searchOfficialOnly = true;
        moduleName = importPath.substr(officialPrefix.length());
    }

    if (searchOfficialOnly) {
        // Search only in the official module directory
        // return FindModuleInDirectory(GetOfficialModulePath(), moduleName);
    } else {
        // Existing logic: search official, then local
        // std::string path = FindModuleInDirectory(GetOfficialModulePath(), moduleName);
        // if (!path.empty()) return path;
        // return FindModuleInDirectory(GetLocalModulePath(), moduleName);
    }
    // Placeholder return
    return "";
}

}
