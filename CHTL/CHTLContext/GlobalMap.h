#ifndef CHTL_GLOBALMAP_H
#define CHTL_GLOBALMAP_H

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace CHTL {

enum class SymbolType {
    TEMPLATE_STYLE,
    TEMPLATE_ELEMENT, 
    TEMPLATE_VAR,
    CUSTOM_STYLE,
    CUSTOM_ELEMENT,
    CUSTOM_VAR,
    ORIGIN_HTML,
    ORIGIN_STYLE,
    ORIGIN_JAVASCRIPT,
    ORIGIN_CUSTOM
};

struct Symbol {
    SymbolType type;
    std::string name;
    std::string namespace_name;
    std::string content;
    std::unordered_map<std::string, std::string> properties;
    
    Symbol(SymbolType t = SymbolType::TEMPLATE_STYLE, 
           const std::string& n = "",
           const std::string& ns = "",
           const std::string& c = "")
        : type(t), name(n), namespace_name(ns), content(c) {}
};

class GlobalMap {
private:
    // namespace -> symbol_name -> Symbol
    std::unordered_map<std::string, std::unordered_map<std::string, Symbol>> symbols;
    
    // HTML tags and keywords
    std::unordered_set<std::string> html_tags;
    std::unordered_set<std::string> keywords;
    
    // Module imports
    std::unordered_map<std::string, std::string> module_aliases;
    std::unordered_set<std::string> imported_modules;
    
    void initializeBuiltins();
    
public:
    GlobalMap();
    
    // Symbol management
    bool addSymbol(const std::string& namespace_name, const std::string& name, const Symbol& symbol);
    bool hasSymbol(const std::string& namespace_name, const std::string& name) const;
    bool hasSymbol(const std::string& name) const; // Search in current and global namespace
    
    Symbol* getSymbol(const std::string& namespace_name, const std::string& name);
    Symbol* getSymbol(const std::string& name); // Search in current and global namespace
    
    bool removeSymbol(const std::string& namespace_name, const std::string& name);
    
    // Namespace management
    void createNamespace(const std::string& namespace_name);
    bool hasNamespace(const std::string& namespace_name) const;
    std::vector<std::string> getNamespaces() const;
    
    // Built-in checks
    bool isHtmlTag(const std::string& tag) const;
    bool isKeyword(const std::string& word) const;
    
    // Module management
    void addModuleAlias(const std::string& module, const std::string& alias);
    void addImportedModule(const std::string& module);
    bool isModuleImported(const std::string& module) const;
    std::string resolveModuleAlias(const std::string& alias) const;
    
    // Query methods
    std::vector<Symbol> getSymbolsByType(SymbolType type) const;
    std::vector<Symbol> getSymbolsInNamespace(const std::string& namespace_name) const;
    
    void clear();
};

} // namespace CHTL

#endif // CHTL_GLOBALMAP_H