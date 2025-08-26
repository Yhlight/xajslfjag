#include "GlobalMap.h"
#include <algorithm>

namespace CHTL {

GlobalMap::GlobalMap() {
    initializeBuiltins();
}

void GlobalMap::initializeBuiltins() {
    // Initialize HTML tags
    html_tags = {
        "html", "head", "body", "title", "meta", "link", "script", "style",
        "div", "span", "p", "h1", "h2", "h3", "h4", "h5", "h6",
        "a", "img", "ul", "ol", "li", "table", "tr", "td", "th",
        "form", "input", "button", "textarea", "select", "option",
        "nav", "header", "footer", "section", "article", "aside",
        "main", "figure", "figcaption", "br", "hr"
    };
    
    // Initialize CHTL keywords
    keywords = {
        "text", "style", "script", "inherit", "delete", "insert",
        "after", "before", "replace", "from", "as", "except", "use"
    };
}

bool GlobalMap::addSymbol(const std::string& namespace_name, const std::string& name, const Symbol& symbol) {
    createNamespace(namespace_name);
    symbols[namespace_name][name] = symbol;
    return true;
}

bool GlobalMap::hasSymbol(const std::string& namespace_name, const std::string& name) const {
    auto ns_it = symbols.find(namespace_name);
    if (ns_it == symbols.end()) return false;
    
    return ns_it->second.find(name) != ns_it->second.end();
}

bool GlobalMap::hasSymbol(const std::string& name) const {
    // Search in global namespace first
    if (hasSymbol("", name)) return true;
    
    // Search in all namespaces
    for (const auto& ns_pair : symbols) {
        if (ns_pair.second.find(name) != ns_pair.second.end()) {
            return true;
        }
    }
    
    return false;
}

Symbol* GlobalMap::getSymbol(const std::string& namespace_name, const std::string& name) {
    auto ns_it = symbols.find(namespace_name);
    if (ns_it == symbols.end()) return nullptr;
    
    auto sym_it = ns_it->second.find(name);
    if (sym_it == ns_it->second.end()) return nullptr;
    
    return &sym_it->second;
}

Symbol* GlobalMap::getSymbol(const std::string& name) {
    // Search in global namespace first
    auto symbol = getSymbol("", name);
    if (symbol) return symbol;
    
    // Search in all namespaces
    for (auto& ns_pair : symbols) {
        auto sym_it = ns_pair.second.find(name);
        if (sym_it != ns_pair.second.end()) {
            return &sym_it->second;
        }
    }
    
    return nullptr;
}

bool GlobalMap::removeSymbol(const std::string& namespace_name, const std::string& name) {
    auto ns_it = symbols.find(namespace_name);
    if (ns_it == symbols.end()) return false;
    
    return ns_it->second.erase(name) > 0;
}

void GlobalMap::createNamespace(const std::string& namespace_name) {
    if (symbols.find(namespace_name) == symbols.end()) {
        symbols[namespace_name] = std::unordered_map<std::string, Symbol>();
    }
}

bool GlobalMap::hasNamespace(const std::string& namespace_name) const {
    return symbols.find(namespace_name) != symbols.end();
}

std::vector<std::string> GlobalMap::getNamespaces() const {
    std::vector<std::string> namespaces;
    for (const auto& ns_pair : symbols) {
        namespaces.push_back(ns_pair.first);
    }
    return namespaces;
}

bool GlobalMap::isHtmlTag(const std::string& tag) const {
    return html_tags.find(tag) != html_tags.end();
}

bool GlobalMap::isKeyword(const std::string& word) const {
    return keywords.find(word) != keywords.end();
}

void GlobalMap::addModuleAlias(const std::string& module, const std::string& alias) {
    module_aliases[alias] = module;
}

void GlobalMap::addImportedModule(const std::string& module) {
    imported_modules.insert(module);
}

bool GlobalMap::isModuleImported(const std::string& module) const {
    return imported_modules.find(module) != imported_modules.end();
}

std::string GlobalMap::resolveModuleAlias(const std::string& alias) const {
    auto it = module_aliases.find(alias);
    return (it != module_aliases.end()) ? it->second : alias;
}

std::vector<Symbol> GlobalMap::getSymbolsByType(SymbolType type) const {
    std::vector<Symbol> result;
    for (const auto& ns_pair : symbols) {
        for (const auto& sym_pair : ns_pair.second) {
            if (sym_pair.second.type == type) {
                result.push_back(sym_pair.second);
            }
        }
    }
    return result;
}

std::vector<Symbol> GlobalMap::getSymbolsInNamespace(const std::string& namespace_name) const {
    std::vector<Symbol> result;
    auto ns_it = symbols.find(namespace_name);
    if (ns_it != symbols.end()) {
        for (const auto& sym_pair : ns_it->second) {
            result.push_back(sym_pair.second);
        }
    }
    return result;
}

void GlobalMap::clear() {
    symbols.clear();
    module_aliases.clear();
    imported_modules.clear();
    initializeBuiltins();
}

} // namespace CHTL