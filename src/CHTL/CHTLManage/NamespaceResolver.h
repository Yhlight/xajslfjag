#pragma once

#include "../../Util/Common.h"
#include "../CHTLNode/BaseNode.h"
#include <unordered_map>
#include <memory>

namespace CHTL {

// 命名空间解析器
class NamespaceResolver {
public:
    NamespaceResolver();
    ~NamespaceResolver() = default;
    
    // 基本功能
    String resolveNamespace(const String& namespaceName);
    bool hasNamespace(const String& namespaceName) const;
    void addNamespace(const String& namespaceName, const String& resolvedPath);
    void removeNamespace(const String& namespaceName);
    
    // 符号解析
    String resolveSymbol(const String& symbolName, const String& currentNamespace = "");
    bool isSymbolVisible(const String& symbolName, const String& currentNamespace = "") const;
    
    // 导入管理
    void addImport(const String& importPath, const String& alias = "");
    void removeImport(const String& importPath);
    StringVector getImports() const;
    
private:
    std::unordered_map<String, String> namespaces;
    std::unordered_map<String, String> symbols;
    StringVector imports;
};

} // namespace CHTL