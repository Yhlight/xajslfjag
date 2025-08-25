#pragma once

#include <string>
#include <map>
#include <memory>
#include "Ast.h"

namespace CHTL {

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

        void Merge(const GlobalMap& other) {
            m_Templates.insert(other.m_Templates.begin(), other.m_Templates.end());
            m_Customs.insert(other.m_Customs.begin(), other.m_Customs.end());
            m_Origins.insert(other.m_Origins.begin(), other.m_Origins.end());
            for (const auto& pair : other.m_Namespaces) {
                m_Namespaces[pair.first].Merge(pair.second);
            }
        }

    private:
        std::map<std::string, std::shared_ptr<TemplateDefinitionNode>> m_Templates;
        std::map<std::string, std::shared_ptr<CustomDefinitionNode>> m_Customs;
        std::map<std::string, std::shared_ptr<OriginNode>> m_Origins;
        std::map<std::string, GlobalMap> m_Namespaces;
    };

}