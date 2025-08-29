#include "NamespaceResolver.h"

namespace CHTL {

NamespaceResolver::NamespaceResolver() {
    namespaces["__global__"] = "";
}

String NamespaceResolver::resolveNamespace(const String& namespaceName) {
    auto it = namespaces.find(namespaceName);
    return (it != namespaces.end()) ? it->second : namespaceName;
}

bool NamespaceResolver::hasNamespace(const String& namespaceName) const {
    return namespaces.find(namespaceName) != namespaces.end();
}

void NamespaceResolver::addNamespace(const String& namespaceName, const String& resolvedPath) {
    namespaces[namespaceName] = resolvedPath;
}

void NamespaceResolver::removeNamespace(const String& namespaceName) {
    namespaces.erase(namespaceName);
}

String NamespaceResolver::resolveSymbol(const String& symbolName, const String& currentNamespace) {
    return symbolName; // 简化版本
}

bool NamespaceResolver::isSymbolVisible(const String& symbolName, const String& currentNamespace) const {
    return true; // 简化版本
}

void NamespaceResolver::addImport(const String& importPath, const String& alias) {
    imports.push_back(alias.empty() ? importPath : alias);
}

void NamespaceResolver::removeImport(const String& importPath) {
    // 简化版本
}

StringVector NamespaceResolver::getImports() const {
    return imports;
}

} // namespace CHTL