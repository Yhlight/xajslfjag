#include "NamespaceNode.h"
#include "TemplateNode.h"
#include "AdvancedTemplateNode.h"
#include "OriginNode.h"
#include "ConfigNode.h"
#include <sstream>
#include <algorithm>

namespace CHTL {

// ========== NamespaceNode 实现 ==========

NamespaceNode::NamespaceNode(const std::string& name) 
    : BaseNode(CHTLNodeType::NAMESPACE_NODE, name), 
      m_namespaceName(name), 
      m_isFileDefaultNamespace(false) {
}

const std::string& NamespaceNode::getNamespaceName() const {
    return m_namespaceName;
}

void NamespaceNode::setNamespaceName(const std::string& name) {
    m_namespaceName = name;
    setName(name);
}

std::string NamespaceNode::getFullNamespacePath() const {
    if (isRootNamespace()) {
        return "";
    }
    
    std::vector<std::string> pathComponents;
    auto current = shared_from_this();
    
    while (current && !current->isRootNamespace()) {
        pathComponents.push_back(current->getNamespaceName());
        current = std::dynamic_pointer_cast<NamespaceNode>(current->getParentNamespace());
    }
    
    std::reverse(pathComponents.begin(), pathComponents.end());
    
    std::string path;
    for (size_t i = 0; i < pathComponents.size(); ++i) {
        if (i > 0) path += ".";
        path += pathComponents[i];
    }
    
    return path;
}

void NamespaceNode::setParentNamespace(std::shared_ptr<NamespaceNode> parent) {
    m_parentNamespace = parent;
}

std::shared_ptr<NamespaceNode> NamespaceNode::getParentNamespace() const {
    return m_parentNamespace;
}

void NamespaceNode::addChildNamespace(std::shared_ptr<NamespaceNode> child) {
    // 检查是否已存在同名子命名空间
    for (auto& existing : m_childNamespaces) {
        if (existing->getNamespaceName() == child->getNamespaceName()) {
            // 合并命名空间
            existing->mergeNamespace(child);
            return;
        }
    }
    
    child->setParentNamespace(shared_from_this());
    m_childNamespaces.push_back(child);
    addChild(child);
}

std::shared_ptr<NamespaceNode> NamespaceNode::getChildNamespace(const std::string& name) const {
    for (auto& child : m_childNamespaces) {
        if (child->getNamespaceName() == name) {
            return child;
        }
    }
    return nullptr;
}

const std::vector<std::shared_ptr<NamespaceNode>>& NamespaceNode::getChildNamespaces() const {
    return m_childNamespaces;
}

bool NamespaceNode::isRootNamespace() const {
    return m_parentNamespace == nullptr && m_namespaceName.empty();
}

bool NamespaceNode::isFileDefaultNamespace() const {
    return m_isFileDefaultNamespace;
}

void NamespaceNode::setAsFileDefaultNamespace(bool isDefault) {
    m_isFileDefaultNamespace = isDefault;
}

void NamespaceNode::addTemplate(std::shared_ptr<TemplateNode> templateNode) {
    if (!templateNode) return;
    
    std::string key = generateSymbolKey("template", templateNode->getName());
    m_templates[key] = templateNode;
    addChild(templateNode);
}

void NamespaceNode::addCustom(std::shared_ptr<CustomNode> customNode) {
    if (!customNode) return;
    
    std::string key = generateSymbolKey("custom", customNode->getName());
    m_customs[key] = customNode;
    addChild(customNode);
}

void NamespaceNode::addOrigin(std::shared_ptr<OriginNode> originNode) {
    if (!originNode) return;
    
    std::string key = generateSymbolKey("origin", originNode->getName());
    m_origins[key] = originNode;
    addChild(originNode);
}

void NamespaceNode::addConfig(std::shared_ptr<ConfigNode> configNode) {
    if (!configNode) return;
    
    std::string key = generateSymbolKey("config", configNode->getName());
    m_configs[key] = configNode;
    addChild(configNode);
}

std::shared_ptr<TemplateNode> NamespaceNode::findTemplate(const std::string& type, const std::string& name) const {
    std::string key = generateSymbolKey("template", type + ":" + name);
    auto it = m_templates.find(key);
    if (it != m_templates.end()) {
        return it->second;
    }
    
    // 在父命名空间中查找
    if (m_parentNamespace) {
        return m_parentNamespace->findTemplate(type, name);
    }
    
    return nullptr;
}

std::shared_ptr<CustomNode> NamespaceNode::findCustom(const std::string& type, const std::string& name) const {
    std::string key = generateSymbolKey("custom", type + ":" + name);
    auto it = m_customs.find(key);
    if (it != m_customs.end()) {
        return it->second;
    }
    
    // 在父命名空间中查找
    if (m_parentNamespace) {
        return m_parentNamespace->findCustom(type, name);
    }
    
    return nullptr;
}

std::shared_ptr<OriginNode> NamespaceNode::findOrigin(const std::string& type, const std::string& name) const {
    std::string key = generateSymbolKey("origin", type + ":" + name);
    auto it = m_origins.find(key);
    if (it != m_origins.end()) {
        return it->second;
    }
    
    // 在父命名空间中查找
    if (m_parentNamespace) {
        return m_parentNamespace->findOrigin(type, name);
    }
    
    return nullptr;
}

std::shared_ptr<ConfigNode> NamespaceNode::findConfig(const std::string& name) const {
    std::string key = generateSymbolKey("config", name);
    auto it = m_configs.find(key);
    if (it != m_configs.end()) {
        return it->second;
    }
    
    // 在父命名空间中查找
    if (m_parentNamespace) {
        return m_parentNamespace->findConfig(name);
    }
    
    return nullptr;
}

std::shared_ptr<BaseNode> NamespaceNode::resolveQualifiedName(const std::string& qualifiedName, const std::string& type) const {
    size_t lastDot = qualifiedName.find_last_of('.');
    
    if (lastDot == std::string::npos) {
        // 没有命名空间前缀，在当前命名空间查找
        if (type == "template") {
            return findTemplate("", qualifiedName);
        } else if (type == "custom") {
            return findCustom("", qualifiedName);
        } else if (type == "origin") {
            return findOrigin("", qualifiedName);
        } else if (type == "config") {
            return findConfig(qualifiedName);
        }
        return nullptr;
    }
    
    // 解析命名空间路径和符号名称
    std::string namespacePath = qualifiedName.substr(0, lastDot);
    std::string symbolName = qualifiedName.substr(lastDot + 1);
    
    // 查找目标命名空间
    auto targetNamespace = shared_from_this();
    
    // 解析命名空间路径
    std::istringstream pathStream(namespacePath);
    std::string component;
    
    while (std::getline(pathStream, component, '.')) {
        targetNamespace = targetNamespace->getChildNamespace(component);
        if (!targetNamespace) {
            return nullptr; // 命名空间不存在
        }
    }
    
    // 在目标命名空间中查找符号
    if (type == "template") {
        return targetNamespace->findTemplate("", symbolName);
    } else if (type == "custom") {
        return targetNamespace->findCustom("", symbolName);
    } else if (type == "origin") {
        return targetNamespace->findOrigin("", symbolName);
    } else if (type == "config") {
        return targetNamespace->findConfig(symbolName);
    }
    
    return nullptr;
}

void NamespaceNode::mergeNamespace(std::shared_ptr<NamespaceNode> other) {
    if (!other || other->getNamespaceName() != m_namespaceName) {
        return;
    }
    
    // 合并模板
    for (const auto& [key, templateNode] : other->m_templates) {
        if (m_templates.find(key) == m_templates.end()) {
            m_templates[key] = templateNode;
            addChild(templateNode);
        }
    }
    
    // 合并自定义
    for (const auto& [key, customNode] : other->m_customs) {
        if (m_customs.find(key) == m_customs.end()) {
            m_customs[key] = customNode;
            addChild(customNode);
        }
    }
    
    // 合并原始嵌入
    for (const auto& [key, originNode] : other->m_origins) {
        if (m_origins.find(key) == m_origins.end()) {
            m_origins[key] = originNode;
            addChild(originNode);
        }
    }
    
    // 合并配置
    for (const auto& [key, configNode] : other->m_configs) {
        if (m_configs.find(key) == m_configs.end()) {
            m_configs[key] = configNode;
            addChild(configNode);
        }
    }
    
    // 合并子命名空间
    for (auto& otherChild : other->m_childNamespaces) {
        addChildNamespace(otherChild);
    }
}

bool NamespaceNode::hasNamingConflict(const std::string& type, const std::string& name) const {
    std::string key = generateSymbolKey(type, name);
    
    if (type == "template") {
        return m_templates.find(key) != m_templates.end();
    } else if (type == "custom") {
        return m_customs.find(key) != m_customs.end();
    } else if (type == "origin") {
        return m_origins.find(key) != m_origins.end();
    } else if (type == "config") {
        return m_configs.find(key) != m_configs.end();
    }
    
    return false;
}

std::unordered_map<std::string, std::vector<std::string>> NamespaceNode::getAllSymbols() const {
    std::unordered_map<std::string, std::vector<std::string>> symbols;
    
    // 收集模板
    for (const auto& [key, templateNode] : m_templates) {
        symbols["templates"].push_back(templateNode->getName());
    }
    
    // 收集自定义
    for (const auto& [key, customNode] : m_customs) {
        symbols["customs"].push_back(customNode->getName());
    }
    
    // 收集原始嵌入
    for (const auto& [key, originNode] : m_origins) {
        symbols["origins"].push_back(originNode->getName());
    }
    
    // 收集配置
    for (const auto& [key, configNode] : m_configs) {
        symbols["configs"].push_back(configNode->getName());
    }
    
    return symbols;
}

std::string NamespaceNode::generateSymbolKey(const std::string& type, const std::string& name) const {
    return type + ":" + name;
}

std::shared_ptr<BaseNode> NamespaceNode::findSymbolRecursive(const std::string& key) const {
    // 查找模板
    auto templateIt = m_templates.find(key);
    if (templateIt != m_templates.end()) {
        return templateIt->second;
    }
    
    // 查找自定义
    auto customIt = m_customs.find(key);
    if (customIt != m_customs.end()) {
        return customIt->second;
    }
    
    // 查找原始嵌入
    auto originIt = m_origins.find(key);
    if (originIt != m_origins.end()) {
        return originIt->second;
    }
    
    // 查找配置
    auto configIt = m_configs.find(key);
    if (configIt != m_configs.end()) {
        return configIt->second;
    }
    
    // 在父命名空间中查找
    if (m_parentNamespace) {
        return m_parentNamespace->findSymbolRecursive(key);
    }
    
    return nullptr;
}

std::string NamespaceNode::toString() const {
    std::stringstream ss;
    
    if (isRootNamespace()) {
        ss << "// Root Namespace\n";
    } else {
        ss << "[Namespace] " << m_namespaceName;
        if (m_isFileDefaultNamespace) {
            ss << " // File Default";
        }
        ss << " {\n";
    }
    
    // 输出子命名空间
    for (auto& child : m_childNamespaces) {
        std::string childStr = child->toString();
        std::istringstream childStream(childStr);
        std::string line;
        while (std::getline(childStream, line)) {
            if (!isRootNamespace()) {
                ss << "    ";
            }
            ss << line << "\n";
        }
    }
    
    // 输出符号（简化）
    if (!m_templates.empty() || !m_customs.empty() || !m_origins.empty() || !m_configs.empty()) {
        if (!isRootNamespace()) {
            ss << "    // Symbols: ";
        } else {
            ss << "// Symbols: ";
        }
        ss << m_templates.size() << " templates, ";
        ss << m_customs.size() << " customs, ";
        ss << m_origins.size() << " origins, ";
        ss << m_configs.size() << " configs\n";
    }
    
    if (!isRootNamespace()) {
        ss << "}";
    }
    
    return ss.str();
}

NodePtr NamespaceNode::clone() const {
    auto cloned = std::make_shared<NamespaceNode>(m_namespaceName);
    cloned->m_isFileDefaultNamespace = m_isFileDefaultNamespace;
    
    // 注意：不克隆父子关系，这些需要在更高层级管理
    
    return cloned;
}

bool NamespaceNode::validate(ErrorReporter* errorReporter) const {
    (void)errorReporter; // 暂时不使用
    return true;
}

// ========== NamespaceManager 实现 ==========

NamespaceManager::NamespaceManager() : m_defaultNamespaceDisabled(false) {
    m_rootNamespace = std::make_shared<NamespaceNode>("");
}

std::shared_ptr<NamespaceNode> NamespaceManager::getRootNamespace() const {
    return m_rootNamespace;
}

std::shared_ptr<NamespaceNode> NamespaceManager::createOrGetNamespace(const std::string& namespacePath) {
    if (namespacePath.empty()) {
        return m_rootNamespace;
    }
    
    std::vector<std::string> pathComponents = parseNamespacePath(namespacePath);
    return createNamespaceRecursive(m_rootNamespace, pathComponents, 0);
}

void NamespaceManager::setCurrentFileNamespace(const std::string& filename, std::shared_ptr<NamespaceNode> namespaceNode) {
    m_fileNamespaces[filename] = namespaceNode;
}

std::shared_ptr<NamespaceNode> NamespaceManager::getCurrentFileNamespace(const std::string& filename) const {
    auto it = m_fileNamespaces.find(filename);
    if (it != m_fileNamespaces.end()) {
        return it->second;
    }
    
    // 如果没有设置文件命名空间且未禁用默认命名空间，创建文件默认命名空间
    if (!m_defaultNamespaceDisabled) {
        // 从文件名提取命名空间名称
        size_t lastSlash = filename.find_last_of("/\\");
        size_t lastDot = filename.find_last_of('.');
        
        std::string namespaceName;
        if (lastSlash != std::string::npos && lastDot != std::string::npos && lastDot > lastSlash) {
            namespaceName = filename.substr(lastSlash + 1, lastDot - lastSlash - 1);
        } else if (lastDot != std::string::npos) {
            namespaceName = filename.substr(0, lastDot);
        } else {
            namespaceName = filename;
        }
        
        auto fileNamespace = std::make_shared<NamespaceNode>(namespaceName);
        fileNamespace->setAsFileDefaultNamespace(true);
        m_rootNamespace->addChildNamespace(fileNamespace);
        
        // 缓存文件命名空间
        const_cast<NamespaceManager*>(this)->m_fileNamespaces[filename] = fileNamespace;
        
        return fileNamespace;
    }
    
    return m_rootNamespace;
}

std::shared_ptr<BaseNode> NamespaceManager::resolveQualifiedName(const std::string& qualifiedName, const std::string& type) const {
    return m_rootNamespace->resolveQualifiedName(qualifiedName, type);
}

void NamespaceManager::addSymbolToCurrentNamespace(const std::string& filename, std::shared_ptr<BaseNode> symbol) {
    auto currentNamespace = getCurrentFileNamespace(filename);
    
    if (auto templateNode = std::dynamic_pointer_cast<TemplateNode>(symbol)) {
        currentNamespace->addTemplate(templateNode);
    } else if (auto customNode = std::dynamic_pointer_cast<CustomNode>(symbol)) {
        currentNamespace->addCustom(customNode);
    } else if (auto originNode = std::dynamic_pointer_cast<OriginNode>(symbol)) {
        currentNamespace->addOrigin(originNode);
    } else if (auto configNode = std::dynamic_pointer_cast<ConfigNode>(symbol)) {
        currentNamespace->addConfig(configNode);
    }
}

std::shared_ptr<BaseNode> NamespaceManager::findSymbol(const std::string& filename, const std::string& type, const std::string& name) const {
    auto currentNamespace = getCurrentFileNamespace(filename);
    
    if (type == "template") {
        return currentNamespace->findTemplate("", name);
    } else if (type == "custom") {
        return currentNamespace->findCustom("", name);
    } else if (type == "origin") {
        return currentNamespace->findOrigin("", name);
    } else if (type == "config") {
        return currentNamespace->findConfig(name);
    }
    
    return nullptr;
}

bool NamespaceManager::isDefaultNamespaceDisabled() const {
    return m_defaultNamespaceDisabled;
}

void NamespaceManager::setDefaultNamespaceDisabled(bool disabled) {
    m_defaultNamespaceDisabled = disabled;
}

std::vector<std::shared_ptr<NamespaceNode>> NamespaceManager::getAllNamespaces() const {
    std::vector<std::shared_ptr<NamespaceNode>> result;
    collectNamespacesRecursive(m_rootNamespace, result);
    return result;
}

void NamespaceManager::cleanupEmptyNamespaces() {
    // 递归清理空命名空间
    // 这里简化实现，实际应该递归检查并移除空的命名空间
}

std::vector<std::string> NamespaceManager::parseNamespacePath(const std::string& namespacePath) const {
    std::vector<std::string> components;
    std::istringstream stream(namespacePath);
    std::string component;
    
    while (std::getline(stream, component, '.')) {
        if (!component.empty()) {
            components.push_back(component);
        }
    }
    
    return components;
}

std::shared_ptr<NamespaceNode> NamespaceManager::createNamespaceRecursive(std::shared_ptr<NamespaceNode> parent, 
                                                                         const std::vector<std::string>& pathComponents, 
                                                                         size_t index) {
    if (index >= pathComponents.size()) {
        return parent;
    }
    
    const std::string& currentComponent = pathComponents[index];
    
    // 查找是否已存在该命名空间
    auto existingChild = parent->getChildNamespace(currentComponent);
    if (existingChild) {
        // 递归创建下一级
        return createNamespaceRecursive(existingChild, pathComponents, index + 1);
    }
    
    // 创建新的命名空间
    auto newNamespace = std::make_shared<NamespaceNode>(currentComponent);
    parent->addChildNamespace(newNamespace);
    
    // 递归创建下一级
    return createNamespaceRecursive(newNamespace, pathComponents, index + 1);
}

void NamespaceManager::collectNamespacesRecursive(std::shared_ptr<NamespaceNode> current, 
                                                 std::vector<std::shared_ptr<NamespaceNode>>& result) const {
    if (current) {
        result.push_back(current);
        for (auto& child : current->getChildNamespaces()) {
            collectNamespacesRecursive(child, result);
        }
    }
}

} // namespace CHTL