#include "GlobalMap.h"
#include "Token.h"
#include <iostream>
#include <algorithm>

namespace CHTL {

GlobalMap::GlobalMap() : current_namespace("") {
    initializeHtmlTags();
    initializeKeywords();
}

void GlobalMap::initializeHtmlTags() {
    // 常用HTML标签
    html_tags = {
        "html", "head", "body", "title", "meta", "link", "style", "script",
        "div", "span", "p", "h1", "h2", "h3", "h4", "h5", "h6",
        "ul", "ol", "li", "dl", "dt", "dd",
        "table", "thead", "tbody", "tfoot", "tr", "th", "td",
        "form", "input", "button", "select", "option", "textarea", "label",
        "a", "img", "video", "audio", "canvas", "svg",
        "header", "nav", "main", "section", "article", "aside", "footer",
        "figure", "figcaption", "blockquote", "pre", "code",
        "strong", "em", "b", "i", "u", "small", "sub", "sup",
        "br", "hr", "wbr"
    };
}

void GlobalMap::initializeKeywords() {
    // CHTL关键字映射
    keywords = {
        {"text", TokenType::TEXT_KEYWORD},
        {"style", TokenType::STYLE_KEYWORD},
        {"script", TokenType::SCRIPT_KEYWORD},
        {"inherit", TokenType::INHERIT},
        {"delete", TokenType::DELETE},
        {"insert", TokenType::INSERT},
        {"after", TokenType::AFTER},
        {"before", TokenType::BEFORE},
        {"replace", TokenType::REPLACE},
        {"from", TokenType::FROM},
        {"as", TokenType::AS},
        {"except", TokenType::EXCEPT},
        {"use", TokenType::USE},
        {"html5", TokenType::HTML5}
    };
}

bool GlobalMap::addSymbol(const std::string& name, const SymbolInfo& info) {
    return addSymbol(current_namespace, name, info);
}

bool GlobalMap::addSymbol(const std::string& namespace_name, const std::string& name, const SymbolInfo& info) {
    if (symbols[namespace_name].find(name) != symbols[namespace_name].end()) {
        return false; // 符号已存在
    }
    
    SymbolInfo new_info = info;
    new_info.namespace_name = namespace_name;
    new_info.file_path = current_file;
    
    symbols[namespace_name][name] = new_info;
    return true;
}

SymbolInfo* GlobalMap::findSymbol(const std::string& name) {
    return findSymbol(current_namespace, name);
}

SymbolInfo* GlobalMap::findSymbol(const std::string& namespace_name, const std::string& name) {
    auto ns_it = symbols.find(namespace_name);
    if (ns_it != symbols.end()) {
        auto sym_it = ns_it->second.find(name);
        if (sym_it != ns_it->second.end()) {
            return &sym_it->second;
        }
    }
    
    // 如果在指定命名空间中找不到，尝试在全局命名空间中查找
    if (!namespace_name.empty()) {
        return findSymbol("", name);
    }
    
    return nullptr;
}

bool GlobalMap::removeSymbol(const std::string& name) {
    return removeSymbol(current_namespace, name);
}

bool GlobalMap::removeSymbol(const std::string& namespace_name, const std::string& name) {
    auto ns_it = symbols.find(namespace_name);
    if (ns_it != symbols.end()) {
        auto sym_it = ns_it->second.find(name);
        if (sym_it != ns_it->second.end()) {
            ns_it->second.erase(sym_it);
            return true;
        }
    }
    return false;
}

void GlobalMap::setCurrentNamespace(const std::string& namespace_name) {
    current_namespace = namespace_name;
    if (!namespace_name.empty() && symbols.find(namespace_name) == symbols.end()) {
        createNamespace(namespace_name);
    }
}

std::string GlobalMap::getCurrentNamespace() const {
    return current_namespace;
}

void GlobalMap::createNamespace(const std::string& namespace_name) {
    if (symbols.find(namespace_name) == symbols.end()) {
        symbols[namespace_name] = std::unordered_map<std::string, SymbolInfo>();
    }
}

bool GlobalMap::namespaceExists(const std::string& namespace_name) const {
    return symbols.find(namespace_name) != symbols.end();
}

std::vector<std::string> GlobalMap::getAllNamespaces() const {
    std::vector<std::string> namespaces;
    for (const auto& pair : symbols) {
        namespaces.push_back(pair.first);
    }
    return namespaces;
}

bool GlobalMap::isHtmlTag(const std::string& tag) const {
    return html_tags.find(tag) != html_tags.end();
}

bool GlobalMap::isKeyword(const std::string& word) const {
    return keywords.find(word) != keywords.end();
}

TokenType GlobalMap::getKeywordType(const std::string& word) const {
    auto it = keywords.find(word);
    if (it != keywords.end()) {
        return it->second;
    }
    return TokenType::IDENTIFIER;
}

void GlobalMap::setConfiguration(const std::string& config_name, const std::string& key, const std::string& value) {
    configurations[config_name][key] = value;
}

std::string GlobalMap::getConfiguration(const std::string& config_name, const std::string& key) const {
    auto config_it = configurations.find(config_name);
    if (config_it != configurations.end()) {
        auto key_it = config_it->second.find(key);
        if (key_it != config_it->second.end()) {
            return key_it->second;
        }
    }
    return "";
}

bool GlobalMap::hasConfiguration(const std::string& config_name) const {
    return configurations.find(config_name) != configurations.end();
}

void GlobalMap::setCurrentFile(const std::string& file_path) {
    current_file = file_path;
}

std::string GlobalMap::getCurrentFile() const {
    return current_file;
}

void GlobalMap::addImportedModule(const std::string& module_name, const std::vector<std::string>& symbols) {
    imported_modules[module_name] = symbols;
}

bool GlobalMap::isModuleImported(const std::string& module_name) const {
    return imported_modules.find(module_name) != imported_modules.end();
}

std::vector<std::string> GlobalMap::getImportedSymbols(const std::string& module_name) const {
    auto it = imported_modules.find(module_name);
    if (it != imported_modules.end()) {
        return it->second;
    }
    return {};
}

bool GlobalMap::hasCyclicDependency(const std::string& symbol_name, const std::string& dependency) const {
    // 简单的循环依赖检查 - 实际实现可能需要更复杂的算法
    auto symbol = findSymbol(symbol_name);
    if (!symbol) return false;
    
    std::unordered_set<std::string> visited;
    std::function<bool(const std::string&)> checkCycle = [&](const std::string& name) -> bool {
        if (visited.find(name) != visited.end()) {
            return name == dependency;
        }
        visited.insert(name);
        
        auto sym = findSymbol(name);
        if (sym) {
            for (const auto& dep : sym->dependencies) {
                if (checkCycle(dep)) {
                    return true;
                }
            }
        }
        return false;
    };
    
    return checkCycle(symbol_name);
}

std::vector<std::string> GlobalMap::getDependencies(const std::string& symbol_name) const {
    auto symbol = findSymbol(symbol_name);
    if (symbol) {
        return symbol->dependencies;
    }
    return {};
}

void GlobalMap::markAsExported(const std::string& symbol_name) {
    auto symbol = findSymbol(symbol_name);
    if (symbol) {
        symbol->is_exported = true;
    }
}

bool GlobalMap::isExported(const std::string& symbol_name) const {
    auto symbol = findSymbol(symbol_name);
    return symbol && symbol->is_exported;
}

std::vector<SymbolInfo> GlobalMap::getExportedSymbols() const {
    std::vector<SymbolInfo> exported;
    for (const auto& ns_pair : symbols) {
        for (const auto& sym_pair : ns_pair.second) {
            if (sym_pair.second.is_exported) {
                exported.push_back(sym_pair.second);
            }
        }
    }
    return exported;
}

void GlobalMap::clear() {
    symbols.clear();
    configurations.clear();
    imported_modules.clear();
    current_namespace.clear();
    current_file.clear();
}

void GlobalMap::clearNamespace(const std::string& namespace_name) {
    auto it = symbols.find(namespace_name);
    if (it != symbols.end()) {
        symbols.erase(it);
    }
}

void GlobalMap::printSymbolTable() const {
    std::cout << "=== Symbol Table ===" << std::endl;
    for (const auto& ns_pair : symbols) {
        std::cout << "Namespace: " << (ns_pair.first.empty() ? "<global>" : ns_pair.first) << std::endl;
        for (const auto& sym_pair : ns_pair.second) {
            const auto& info = sym_pair.second;
            std::cout << "  " << sym_pair.first << " (" << static_cast<int>(info.type) << ")"
                      << " at " << info.file_path << ":" << info.line << ":" << info.column
                      << (info.is_exported ? " [exported]" : "") << std::endl;
        }
    }
}

size_t GlobalMap::getSymbolCount() const {
    size_t count = 0;
    for (const auto& ns_pair : symbols) {
        count += ns_pair.second.size();
    }
    return count;
}

size_t GlobalMap::getNamespaceCount() const {
    return symbols.size();
}

} // namespace CHTL