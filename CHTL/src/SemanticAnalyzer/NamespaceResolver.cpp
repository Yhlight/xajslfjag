#include "CHTL/SemanticAnalyzer/NamespaceResolver.h"
#include <filesystem>

namespace fs = std::filesystem;

namespace CHTL {

NamespaceResolver::NamespaceResolver(const Config& config) 
    : m_Config(config) {
}

GlobalMap NamespaceResolver::Resolve(std::shared_ptr<ProgramNode> ast, const std::string& defaultNamespace) {
    GlobalMap rootMap;
    
    // If no explicit namespace and default namespace is not disabled
    if (!m_Config.DisableDefaultNamespace && !defaultNamespace.empty()) {
        // Create a namespace with the file name
        GlobalMap fileNamespace;
        Visit(ast, fileNamespace);
        rootMap.AddNamespace(defaultNamespace, fileNamespace);
    } else {
        // Process normally
        Visit(ast, rootMap);
    }
    
    return rootMap;
}

void NamespaceResolver::Visit(AstNodePtr node, GlobalMap& currentScope) {
    // Handle different node types
    if (auto namespaceNode = std::dynamic_pointer_cast<NamespaceNode>(node)) {
        ProcessNamespaceNode(namespaceNode, currentScope);
    } 
    else if (auto templateNode = std::dynamic_pointer_cast<TemplateDefinitionNode>(node)) {
        currentScope.AddTemplate(templateNode->Name, templateNode);
    }
    else if (auto customNode = std::dynamic_pointer_cast<CustomDefinitionNode>(node)) {
        currentScope.AddCustom(customNode->Name, customNode);
    }
    else if (auto originNode = std::dynamic_pointer_cast<OriginNode>(node)) {
        if (originNode->Name.has_value()) {
            currentScope.AddOrigin(originNode->Name.value(), originNode);
        }
    }
    else if (auto programNode = std::dynamic_pointer_cast<ProgramNode>(node)) {
        // Visit all children
        for (const auto& child : programNode->Children) {
            Visit(child, currentScope);
        }
    }
    else if (auto elementNode = std::dynamic_pointer_cast<ElementNode>(node)) {
        // Visit children of elements
        for (const auto& child : elementNode->Children) {
            Visit(child, currentScope);
        }
    }
    // Add more node types as needed
}

void NamespaceResolver::ProcessNamespaceNode(std::shared_ptr<NamespaceNode> node, GlobalMap& parentScope) {
    // Create a new scope for this namespace
    GlobalMap namespaceScope;
    
    // Push namespace name to stack
    m_NamespaceStack.push(node->Name);
    
    // Process all children in the namespace scope
    for (const auto& child : node->Body) {
        Visit(child, namespaceScope);
    }
    
    // Pop namespace from stack
    m_NamespaceStack.pop();
    
    // Add this namespace to parent scope
    parentScope.AddNamespace(node->Name, namespaceScope);
}

std::string NamespaceResolver::GetCurrentNamespacePath() const {
    std::string path;
    std::stack<std::string> temp = m_NamespaceStack;
    std::vector<std::string> parts;
    
    while (!temp.empty()) {
        parts.push_back(temp.top());
        temp.pop();
    }
    
    for (auto it = parts.rbegin(); it != parts.rend(); ++it) {
        if (!path.empty()) path += ".";
        path += *it;
    }
    
    return path;
}

}