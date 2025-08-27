#include "GlobalMap.h"
#include <sstream>
#include <algorithm>

namespace CHTL {

GlobalMap::GlobalMap() : autoClassCounter(0), autoIdCounter(0) {}

GlobalMap::~GlobalMap() {}

void GlobalMap::AddTemplate(const std::string& type, const std::string& name, std::shared_ptr<TemplateNode> node) {
    std::string key = MakeKey(type, name);
    templates[key] = node;
}

std::shared_ptr<TemplateNode> GlobalMap::GetTemplate(const std::string& type, const std::string& name) const {
    std::string key = MakeKey(type, name);
    auto it = templates.find(key);
    if (it != templates.end()) {
        return it->second;
    }
    return nullptr;
}

bool GlobalMap::HasTemplate(const std::string& type, const std::string& name) const {
    std::string key = MakeKey(type, name);
    return templates.find(key) != templates.end();
}

std::vector<std::string> GlobalMap::GetTemplateNames(const std::string& type) const {
    std::vector<std::string> names;
    for (const auto& pair : templates) {
        auto [t, n] = SplitKey(pair.first);
        if (t == type) {
            names.push_back(n);
        }
    }
    return names;
}

void GlobalMap::AddCustom(const std::string& type, const std::string& name, std::shared_ptr<CustomNode> node) {
    std::string key = MakeKey(type, name);
    customs[key] = node;
}

std::shared_ptr<CustomNode> GlobalMap::GetCustom(const std::string& type, const std::string& name) const {
    std::string key = MakeKey(type, name);
    auto it = customs.find(key);
    if (it != customs.end()) {
        return it->second;
    }
    return nullptr;
}

bool GlobalMap::HasCustom(const std::string& type, const std::string& name) const {
    std::string key = MakeKey(type, name);
    return customs.find(key) != customs.end();
}

std::vector<std::string> GlobalMap::GetCustomNames(const std::string& type) const {
    std::vector<std::string> names;
    for (const auto& pair : customs) {
        auto [t, n] = SplitKey(pair.first);
        if (t == type) {
            names.push_back(n);
        }
    }
    return names;
}

void GlobalMap::AddOrigin(const std::string& type, const std::string& name, std::shared_ptr<OriginNode> node) {
    std::string key = MakeKey(type, name);
    origins[key] = node;
}

std::shared_ptr<OriginNode> GlobalMap::GetOrigin(const std::string& type, const std::string& name) const {
    std::string key = MakeKey(type, name);
    auto it = origins.find(key);
    if (it != origins.end()) {
        return it->second;
    }
    return nullptr;
}

bool GlobalMap::HasOrigin(const std::string& type, const std::string& name) const {
    std::string key = MakeKey(type, name);
    return origins.find(key) != origins.end();
}

std::vector<std::string> GlobalMap::GetOriginNames(const std::string& type) const {
    std::vector<std::string> names;
    for (const auto& pair : origins) {
        auto [t, n] = SplitKey(pair.first);
        if (t == type) {
            names.push_back(n);
        }
    }
    return names;
}

void GlobalMap::AddConfig(const std::string& name, std::shared_ptr<ConfigNode> node) {
    configs[name] = node;
}

std::shared_ptr<ConfigNode> GlobalMap::GetConfig(const std::string& name) const {
    auto it = configs.find(name);
    if (it != configs.end()) {
        return it->second;
    }
    return nullptr;
}

std::shared_ptr<ConfigNode> GlobalMap::GetDefaultConfig() const {
    return GetConfig("");  // 空字符串表示默认配置
}

bool GlobalMap::HasConfig(const std::string& name) const {
    return configs.find(name) != configs.end();
}

std::vector<std::string> GlobalMap::GetConfigNames() const {
    std::vector<std::string> names;
    for (const auto& pair : configs) {
        if (!pair.first.empty()) {  // 排除默认配置
            names.push_back(pair.first);
        }
    }
    return names;
}

void GlobalMap::AddNamespace(const std::string& name, std::shared_ptr<NamespaceNode> node) {
    namespaces[name] = node;
}

std::shared_ptr<NamespaceNode> GlobalMap::GetNamespace(const std::string& name) const {
    auto it = namespaces.find(name);
    if (it != namespaces.end()) {
        return it->second;
    }
    return nullptr;
}

bool GlobalMap::HasNamespace(const std::string& name) const {
    return namespaces.find(name) != namespaces.end();
}

std::vector<std::string> GlobalMap::GetNamespaceNames() const {
    std::vector<std::string> names;
    for (const auto& pair : namespaces) {
        names.push_back(pair.first);
    }
    return names;
}

void GlobalMap::EnterNamespace(const std::string& name) {
    namespaceStack.push_back(name);
}

void GlobalMap::ExitNamespace() {
    if (!namespaceStack.empty()) {
        namespaceStack.pop_back();
    }
}

std::string GlobalMap::GetCurrentNamespace() const {
    std::stringstream ss;
    for (size_t i = 0; i < namespaceStack.size(); ++i) {
        if (i > 0) ss << ".";
        ss << namespaceStack[i];
    }
    return ss.str();
}

std::string GlobalMap::GetFullName(const std::string& name) const {
    std::string currentNs = GetCurrentNamespace();
    if (currentNs.empty()) {
        return name;
    }
    return currentNs + "." + name;
}

void GlobalMap::AddImportedFile(const std::string& path) {
    importedFiles[path] = true;
}

bool GlobalMap::IsFileImported(const std::string& path) const {
    return importedFiles.find(path) != importedFiles.end();
}

std::vector<std::string> GlobalMap::GetImportedFiles() const {
    std::vector<std::string> files;
    for (const auto& pair : importedFiles) {
        files.push_back(pair.first);
    }
    return files;
}

std::string GlobalMap::GenerateClassName() {
    std::string className;
    do {
        className = "chtl-class-" + std::to_string(++autoClassCounter);
    } while (IsClassNameUsed(className));
    RegisterClassName(className);
    return className;
}

std::string GlobalMap::GenerateId() {
    std::string id;
    do {
        id = "chtl-id-" + std::to_string(++autoIdCounter);
    } while (IsIdUsed(id));
    RegisterId(id);
    return id;
}

void GlobalMap::RegisterClassName(const std::string& className) {
    usedClassNames[className] = true;
}

void GlobalMap::RegisterId(const std::string& id) {
    usedIds[id] = true;
}

bool GlobalMap::IsClassNameUsed(const std::string& className) const {
    return usedClassNames.find(className) != usedClassNames.end();
}

bool GlobalMap::IsIdUsed(const std::string& id) const {
    return usedIds.find(id) != usedIds.end();
}

void GlobalMap::Clear() {
    templates.clear();
    customs.clear();
    origins.clear();
    configs.clear();
    namespaces.clear();
    namespaceStack.clear();
    importedFiles.clear();
    usedClassNames.clear();
    usedIds.clear();
    autoClassCounter = 0;
    autoIdCounter = 0;
}

void GlobalMap::Merge(const GlobalMap& other) {
    // 合并模板
    for (const auto& pair : other.templates) {
        templates[pair.first] = pair.second;
    }
    
    // 合并自定义
    for (const auto& pair : other.customs) {
        customs[pair.first] = pair.second;
    }
    
    // 合并原始嵌入
    for (const auto& pair : other.origins) {
        origins[pair.first] = pair.second;
    }
    
    // 合并配置
    for (const auto& pair : other.configs) {
        configs[pair.first] = pair.second;
    }
    
    // 合并命名空间
    for (const auto& pair : other.namespaces) {
        namespaces[pair.first] = pair.second;
    }
    
    // 合并已导入文件
    for (const auto& pair : other.importedFiles) {
        importedFiles[pair.first] = pair.second;
    }
    
    // 合并已使用的类名和ID
    for (const auto& pair : other.usedClassNames) {
        usedClassNames[pair.first] = pair.second;
    }
    
    for (const auto& pair : other.usedIds) {
        usedIds[pair.first] = pair.second;
    }
}

std::string GlobalMap::MakeKey(const std::string& type, const std::string& name) const {
    return type + ":" + name;
}

std::pair<std::string, std::string> GlobalMap::SplitKey(const std::string& key) const {
    size_t pos = key.find(':');
    if (pos != std::string::npos) {
        return {key.substr(0, pos), key.substr(pos + 1)};
    }
    return {"", key};
}

} // namespace CHTL