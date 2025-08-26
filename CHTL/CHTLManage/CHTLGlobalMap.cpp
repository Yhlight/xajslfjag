#include "CHTLGlobalMap.h"
#include <iostream>
#include <algorithm>

namespace CHTL {

// 移除全局静态变量
// static CHTLGlobalMap global_map_instance;

CHTLGlobalMap& getGlobalMap() {
    // 使用局部静态变量，确保在第一次调用时初始化
    static CHTLGlobalMap global_map_instance;
    return global_map_instance;
}

bool CHTLGlobalMap::addSymbol(const std::string& name, const SymbolInfo& info) {
    std::string full_name = getFullyQualifiedName(name);
    
    // 检查冲突
    if (hasConflict(full_name, info.type)) {
        return false;
    }
    
    symbols[full_name] = info;
    
    // 如果在命名空间内，也添加到命名空间的符号表
    if (!namespace_stack.empty()) {
        std::string ns = getCurrentNamespace();
        if (namespaces.find(ns) != namespaces.end()) {
            namespaces[ns].symbols[name] = info;
        }
    }
    
    return true;
}

bool CHTLGlobalMap::hasSymbol(const std::string& name) const {
    std::string full_name = getFullyQualifiedName(name);
    return symbols.find(full_name) != symbols.end();
}

std::optional<SymbolInfo> CHTLGlobalMap::getSymbol(const std::string& name) const {
    std::string full_name = getFullyQualifiedName(name);
    auto it = symbols.find(full_name);
    if (it != symbols.end()) {
        return it->second;
    }
    return std::nullopt;
}

bool CHTLGlobalMap::removeSymbol(const std::string& name) {
    std::string full_name = getFullyQualifiedName(name);
    return symbols.erase(full_name) > 0;
}

bool CHTLGlobalMap::addNamespace(const std::string& name, const NamespaceInfo& info) {
    if (namespaces.find(name) != namespaces.end()) {
        // 如果命名空间已存在，尝试合并
        return mergeNamespace(name, info);
    }
    
    namespaces[name] = info;
    
    // 更新父子关系
    if (!info.parent.empty() && namespaces.find(info.parent) != namespaces.end()) {
        namespaces[info.parent].children.push_back(name);
    }
    
    return true;
}

bool CHTLGlobalMap::hasNamespace(const std::string& name) const {
    return namespaces.find(name) != namespaces.end();
}

std::optional<NamespaceInfo> CHTLGlobalMap::getNamespace(const std::string& name) const {
    auto it = namespaces.find(name);
    if (it != namespaces.end()) {
        return it->second;
    }
    return std::nullopt;
}

void CHTLGlobalMap::enterNamespace(const std::string& name) {
    namespace_stack.push_back(name);
}

void CHTLGlobalMap::exitNamespace() {
    if (!namespace_stack.empty()) {
        namespace_stack.pop_back();
    }
}

std::string CHTLGlobalMap::getCurrentNamespace() const {
    if (namespace_stack.empty()) {
        return "";
    }
    
    // 构建完整的命名空间路径
    std::string result;
    for (size_t i = 0; i < namespace_stack.size(); ++i) {
        if (i > 0) result += "::";
        result += namespace_stack[i];
    }
    return result;
}

std::string CHTLGlobalMap::getFullyQualifiedName(const std::string& name) const {
    std::string ns = getCurrentNamespace();
    if (ns.empty()) {
        return name;
    }
    return ns + "::" + name;
}

void CHTLGlobalMap::addImport(const ImportInfo& info) {
    imports.push_back(info);
}

bool CHTLGlobalMap::isFileImported(const std::string& file) const {
    return imported_files.find(file) != imported_files.end();
}

void CHTLGlobalMap::markFileImported(const std::string& file) {
    imported_files.insert(file);
}

void CHTLGlobalMap::registerClassName(const std::string& className) {
    class_usage[className]++;
}

void CHTLGlobalMap::registerIdName(const std::string& idName) {
    id_usage[idName]++;
}

int CHTLGlobalMap::getClassUsageCount(const std::string& className) const {
    auto it = class_usage.find(className);
    return it != class_usage.end() ? it->second : 0;
}

int CHTLGlobalMap::getIdUsageCount(const std::string& idName) const {
    auto it = id_usage.find(idName);
    return it != id_usage.end() ? it->second : 0;
}

std::string CHTLGlobalMap::getMostUsedClass() const {
    if (class_usage.empty()) return "";
    
    auto max_it = std::max_element(class_usage.begin(), class_usage.end(),
        [](const auto& a, const auto& b) {
            return a.second < b.second;
        });
    
    return max_it->first;
}

std::string CHTLGlobalMap::getMostUsedId() const {
    if (id_usage.empty()) return "";
    
    auto max_it = std::max_element(id_usage.begin(), id_usage.end(),
        [](const auto& a, const auto& b) {
            return a.second < b.second;
        });
    
    return max_it->first;
}

std::optional<SymbolInfo> CHTLGlobalMap::findSymbol(const std::string& name, 
                                                    const std::string& namespace_hint) const {
    // 1. 尝试在指定命名空间中查找
    if (!namespace_hint.empty()) {
        std::string qualified_name = namespace_hint + "::" + name;
        auto it = symbols.find(qualified_name);
        if (it != symbols.end()) {
            return it->second;
        }
    }
    
    // 2. 尝试在当前命名空间中查找
    std::string current_ns = getCurrentNamespace();
    if (!current_ns.empty()) {
        std::string qualified_name = current_ns + "::" + name;
        auto it = symbols.find(qualified_name);
        if (it != symbols.end()) {
            return it->second;
        }
    }
    
    // 3. 尝试在全局命名空间中查找
    auto it = symbols.find(name);
    if (it != symbols.end()) {
        return it->second;
    }
    
    // 4. 通过导入查找
    for (const auto& import : imports) {
        if (!import.alias.empty() && import.alias == name) {
            // 返回导入的符号信息
            SymbolInfo info;
            info.type = SymbolType::IMPORT;
            info.name = name;
            info.value = import.path;
            return info;
        }
    }
    
    return std::nullopt;
}

bool CHTLGlobalMap::mergeNamespace(const std::string& name, const NamespaceInfo& info) {
    auto& existing = namespaces[name];
    
    // 标记为合并的命名空间
    existing.is_merged = true;
    
    // 合并符号，检测冲突
    for (const auto& [sym_name, sym_info] : info.symbols) {
        if (existing.symbols.find(sym_name) != existing.symbols.end()) {
            // 检测到符号冲突
            std::cerr << "Symbol conflict in namespace " << name 
                     << ": " << sym_name << std::endl;
            return false;
        }
        existing.symbols[sym_name] = sym_info;
    }
    
    // 合并子命名空间
    for (const auto& child : info.children) {
        if (std::find(existing.children.begin(), existing.children.end(), child) 
            == existing.children.end()) {
            existing.children.push_back(child);
        }
    }
    
    return true;
}

bool CHTLGlobalMap::hasConflict(const std::string& name, SymbolType type) const {
    auto it = symbols.find(name);
    if (it == symbols.end()) {
        return false;  // 没有冲突
    }
    
    // 某些类型允许重定义
    const SymbolInfo& existing = it->second;
    
    // 同类型的模板变量和自定义变量可以重定义
    if ((type == SymbolType::TEMPLATE_VAR || type == SymbolType::CUSTOM_VAR) &&
        (existing.type == SymbolType::TEMPLATE_VAR || existing.type == SymbolType::CUSTOM_VAR)) {
        return false;
    }
    
    // 样式组可以被覆盖
    if ((type == SymbolType::TEMPLATE_STYLE_GROUP || 
         type == SymbolType::CUSTOM_STYLE_GROUP ||
         type == SymbolType::NO_VALUE_STYLE_GROUP) &&
        (existing.type == SymbolType::TEMPLATE_STYLE_GROUP || 
         existing.type == SymbolType::CUSTOM_STYLE_GROUP ||
         existing.type == SymbolType::NO_VALUE_STYLE_GROUP)) {
        return false;
    }
    
    return true;  // 其他情况视为冲突
}

std::vector<std::string> CHTLGlobalMap::detectConflicts() const {
    std::vector<std::string> conflicts;
    
    // 检查符号冲突
    std::unordered_map<std::string, std::vector<std::string>> name_to_qualified;
    for (const auto& [qualified_name, info] : symbols) {
        size_t pos = qualified_name.rfind("::");
        std::string simple_name = (pos != std::string::npos) 
            ? qualified_name.substr(pos + 2) 
            : qualified_name;
        name_to_qualified[simple_name].push_back(qualified_name);
    }
    
    // 报告有多个定义的符号
    for (const auto& [name, qualified_names] : name_to_qualified) {
        if (qualified_names.size() > 1) {
            std::string conflict_msg = "Symbol '" + name + "' has multiple definitions: ";
            for (size_t i = 0; i < qualified_names.size(); ++i) {
                if (i > 0) conflict_msg += ", ";
                conflict_msg += qualified_names[i];
            }
            conflicts.push_back(conflict_msg);
        }
    }
    
    return conflicts;
}

void CHTLGlobalMap::clear() {
    symbols.clear();
    namespaces.clear();
    imports.clear();
    imported_files.clear();
    namespace_stack.clear();
    class_usage.clear();
    id_usage.clear();
    configuration = ConfigurationInfo();
}

void CHTLGlobalMap::clearNamespace(const std::string& name) {
    // 清除命名空间及其所有符号
    if (namespaces.find(name) != namespaces.end()) {
        // 清除该命名空间下的所有符号
        auto it = symbols.begin();
        while (it != symbols.end()) {
            if (it->first.find(name + "::") == 0) {
                it = symbols.erase(it);
            } else {
                ++it;
            }
        }
        
        // 递归清除子命名空间
        for (const auto& child : namespaces[name].children) {
            clearNamespace(child);
        }
        
        // 从父命名空间的子列表中移除
        const std::string& parent = namespaces[name].parent;
        if (!parent.empty() && namespaces.find(parent) != namespaces.end()) {
            auto& children = namespaces[parent].children;
            children.erase(
                std::remove(children.begin(), children.end(), name),
                children.end()
            );
        }
        
        // 删除命名空间本身
        namespaces.erase(name);
    }
}

void CHTLGlobalMap::dump() const {
    std::cout << "=== Global Symbol Map ===" << std::endl;
    
    std::cout << "\nSymbols:" << std::endl;
    for (const auto& [name, info] : symbols) {
        std::cout << "  " << name << " (";
        switch (info.type) {
            case SymbolType::TEMPLATE_VAR: std::cout << "Template Var"; break;
            case SymbolType::CUSTOM_VAR: std::cout << "Custom Var"; break;
            case SymbolType::TEMPLATE_STYLE_GROUP: std::cout << "Template Style Group"; break;
            case SymbolType::CUSTOM_STYLE_GROUP: std::cout << "Custom Style Group"; break;
            case SymbolType::NO_VALUE_STYLE_GROUP: std::cout << "No Value Style Group"; break;
            case SymbolType::NAMESPACE: std::cout << "Namespace"; break;
            case SymbolType::IMPORT: std::cout << "Import"; break;
            case SymbolType::ORIGIN_EMBED: std::cout << "Origin Embed"; break;
            case SymbolType::ELEMENT: std::cout << "Element"; break;
            case SymbolType::CLASS_NAME: std::cout << "Class Name"; break;
            case SymbolType::ID_NAME: std::cout << "ID Name"; break;
        }
        std::cout << ") = " << info.value << std::endl;
    }
    
    std::cout << "\nNamespaces:" << std::endl;
    for (const auto& [name, info] : namespaces) {
        std::cout << "  " << name;
        if (!info.parent.empty()) {
            std::cout << " (parent: " << info.parent << ")";
        }
        if (info.is_merged) {
            std::cout << " [merged]";
        }
        std::cout << std::endl;
    }
    
    std::cout << "\nImports:" << std::endl;
    for (const auto& import : imports) {
        std::cout << "  " << import.type << " from " << import.path;
        if (!import.alias.empty()) {
            std::cout << " as " << import.alias;
        }
        if (import.is_origin) {
            std::cout << " [Origin]";
        }
        std::cout << std::endl;
    }
    
    std::cout << "\nConfiguration:" << std::endl;
    std::cout << "  disable_style_auto_add_class: " 
              << (configuration.disable_style_auto_add_class ? "true" : "false") << std::endl;
    std::cout << "  disable_style_auto_add_id: " 
              << (configuration.disable_style_auto_add_id ? "true" : "false") << std::endl;
    std::cout << "  disable_script_auto_add_class: " 
              << (configuration.disable_script_auto_add_class ? "true" : "false") << std::endl;
    std::cout << "  disable_script_auto_add_id: " 
              << (configuration.disable_script_auto_add_id ? "true" : "false") << std::endl;
    std::cout << "  disable_default_namespace: " 
              << (configuration.disable_default_namespace ? "true" : "false") << std::endl;
}

} // namespace CHTL