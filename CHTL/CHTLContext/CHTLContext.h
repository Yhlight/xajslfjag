#ifndef CHTL_CONTEXT_H
#define CHTL_CONTEXT_H

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace CHTL {

class CHTLContext {
private:
    // Symbol tables
    std::unordered_map<std::string, std::string> templates;
    std::unordered_map<std::string, std::string> customs;
    std::unordered_map<std::string, std::string> variables;
    std::unordered_map<std::string, std::string> origins;
    
    // Current context
    std::string current_namespace;
    std::string current_element;
    std::string current_template;
    std::string current_custom;
    
    // Imports and modules
    std::unordered_set<std::string> imported_modules;
    std::unordered_map<std::string, std::string> module_aliases;
    
    // Configuration
    std::unordered_map<std::string, std::string> configuration;
    
    // Constraints
    std::unordered_set<std::string> global_constraints;
    std::unordered_set<std::string> local_constraints;
    
public:
    CHTLContext();
    
    // Symbol management
    void addTemplate(const std::string& name, const std::string& content);
    void addCustom(const std::string& name, const std::string& content);
    void addVariable(const std::string& name, const std::string& value);
    void addOrigin(const std::string& name, const std::string& content);
    
    bool hasTemplate(const std::string& name) const;
    bool hasCustom(const std::string& name) const;
    bool hasVariable(const std::string& name) const;
    bool hasOrigin(const std::string& name) const;
    
    std::string getTemplate(const std::string& name) const;
    std::string getCustom(const std::string& name) const;
    std::string getVariable(const std::string& name) const;
    std::string getOrigin(const std::string& name) const;
    
    // Context management
    void setCurrentNamespace(const std::string& ns);
    void setCurrentElement(const std::string& element);
    void setCurrentTemplate(const std::string& template_name);
    void setCurrentCustom(const std::string& custom_name);
    
    std::string getCurrentNamespace() const { return current_namespace; }
    std::string getCurrentElement() const { return current_element; }
    std::string getCurrentTemplate() const { return current_template; }
    std::string getCurrentCustom() const { return current_custom; }
    
    // Import management
    void addImport(const std::string& module, const std::string& alias = "");
    bool isImported(const std::string& module) const;
    std::string resolveAlias(const std::string& alias) const;
    
    // Configuration management
    void setConfig(const std::string& key, const std::string& value);
    std::string getConfig(const std::string& key) const;
    bool hasConfig(const std::string& key) const;
    
    // Constraint management
    void addConstraint(const std::string& constraint, bool is_global = false);
    void removeConstraint(const std::string& constraint, bool is_global = false);
    bool hasConstraint(const std::string& constraint) const;
    bool isAllowed(const std::string& operation) const;
    
    // Utility
    std::vector<std::string> getAvailableTemplates() const;
    std::vector<std::string> getAvailableCustoms() const;
    std::vector<std::string> getAvailableVariables() const;
    
    void clear();
    void clearLocal(); // Keep global symbols, clear local context
};

} // namespace CHTL

#endif // CHTL_CONTEXT_H