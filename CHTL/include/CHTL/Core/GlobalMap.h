#pragma once

#include <string>
#include <map>
#include <memory>
#include <vector>
#include <stdexcept>
#include "Ast.h"

namespace CHTL {

    class NamespaceConflictException : public std::runtime_error {
    public:
        NamespaceConflictException(const std::string& name, const std::string& type)
            : std::runtime_error("Namespace conflict: " + type + " '" + name + "' already exists") {}
    };

    class GlobalMap {
    public:
        void AddTemplate(const std::string& name, std::shared_ptr<TemplateDefinitionNode> node) {
            m_Templates[name] = node;
        }

        std::shared_ptr<TemplateDefinitionNode> GetTemplate(const std::string& name) const {
            auto it = m_Templates.find(name);
            return (it != m_Templates.end()) ? it->second : nullptr;
        }

        void AddCustom(const std::string& name, std::shared_ptr<CustomDefinitionNode> node) {
            m_Customs[name] = node;
        }

        std::shared_ptr<CustomDefinitionNode> GetCustom(const std::string& name) const {
            auto it = m_Customs.find(name);
            return (it != m_Customs.end()) ? it->second : nullptr;
        }

        void AddOrigin(const std::string& name, std::shared_ptr<OriginNode> node) {
            m_Origins[name] = node;
        }

        std::shared_ptr<OriginNode> GetOrigin(const std::string& name) const {
            auto it = m_Origins.find(name);
            return (it != m_Origins.end()) ? it->second : nullptr;
        }

        const std::map<std::string, std::shared_ptr<TemplateDefinitionNode>>& GetAllTemplates() const {
            return m_Templates;
        }

        const std::map<std::string, std::shared_ptr<CustomDefinitionNode>>& GetAllCustoms() const {
            return m_Customs;
        }

        void Merge(const GlobalMap& other, bool checkConflicts = true) {
            // Merge templates with conflict detection
            for (const auto& [name, node] : other.m_Templates) {
                if (checkConflicts && m_Templates.find(name) != m_Templates.end()) {
                    throw NamespaceConflictException(name, "template");
                }
                m_Templates[name] = node;
            }
            
            // Merge customs with conflict detection
            for (const auto& [name, node] : other.m_Customs) {
                if (checkConflicts && m_Customs.find(name) != m_Customs.end()) {
                    throw NamespaceConflictException(name, "custom");
                }
                m_Customs[name] = node;
            }
            
            // Merge origins with conflict detection
            for (const auto& [name, node] : other.m_Origins) {
                if (checkConflicts && m_Origins.find(name) != m_Origins.end()) {
                    throw NamespaceConflictException(name, "origin");
                }
                m_Origins[name] = node;
            }
            
            // Merge nested namespaces
            for (const auto& [nsName, nsMap] : other.m_Namespaces) {
                if (m_Namespaces.find(nsName) == m_Namespaces.end()) {
                    m_Namespaces[nsName] = nsMap;
                } else {
                    // Merge same-named namespaces
                    m_Namespaces[nsName].Merge(nsMap, checkConflicts);
                }
            }
        }
        
        // Add namespace support
        void AddNamespace(const std::string& name, const GlobalMap& namespaceMap) {
            if (m_Namespaces.find(name) == m_Namespaces.end()) {
                m_Namespaces[name] = namespaceMap;
            } else {
                // Merge with existing namespace of same name
                m_Namespaces[name].Merge(namespaceMap);
            }
        }
        
        GlobalMap* GetNamespace(const std::string& name) {
            auto it = m_Namespaces.find(name);
            return (it != m_Namespaces.end()) ? &it->second : nullptr;
        }
        
        // Resolve item from namespace path (e.g., "space.room.item")
        template<typename T>
        std::shared_ptr<T> ResolveFromNamespace(const std::string& path, 
            std::shared_ptr<T> (GlobalMap::*getter)(const std::string&) const) {
            size_t dotPos = path.find('.');
            if (dotPos == std::string::npos) {
                // No namespace, look in current scope
                return (this->*getter)(path);
            }
            
            std::string nsName = path.substr(0, dotPos);
            std::string remaining = path.substr(dotPos + 1);
            
            auto ns = GetNamespace(nsName);
            if (ns) {
                return ns->ResolveFromNamespace(remaining, getter);
            }
            
            return nullptr;
        }

    private:
        std::map<std::string, std::shared_ptr<TemplateDefinitionNode>> m_Templates;
        std::map<std::string, std::shared_ptr<CustomDefinitionNode>> m_Customs;
        std::map<std::string, std::shared_ptr<OriginNode>> m_Origins;
        std::map<std::string, GlobalMap> m_Namespaces;
    };

}