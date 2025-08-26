#ifndef CHTL_CONTEXT_H
#define CHTL_CONTEXT_H

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace CHTL {

class CHTLContext {
private:
    // Current parsing context (not symbol storage)
    std::string current_namespace;
    std::string current_element;
    std::string current_template;
    std::string current_custom;
    
    // Configuration for current compilation
    std::unordered_map<std::string, std::string> configuration;
    
    // Local constraints (parsing rules)
    std::unordered_set<std::string> local_constraints;
    
public:
    CHTLContext();
    
    // Current parsing context management
    void setCurrentNamespace(const std::string& ns);
    void setCurrentElement(const std::string& element);
    void setCurrentTemplate(const std::string& template_name);
    void setCurrentCustom(const std::string& custom_name);
    
    std::string getCurrentNamespace() const { return current_namespace; }
    std::string getCurrentElement() const { return current_element; }
    std::string getCurrentTemplate() const { return current_template; }
    std::string getCurrentCustom() const { return current_custom; }
    
    // Configuration management (compilation settings)
    void setConfig(const std::string& key, const std::string& value);
    std::string getConfig(const std::string& key) const;
    bool hasConfig(const std::string& key) const;
    
    // Local constraint management (parsing rules)
    void addConstraint(const std::string& constraint);
    void removeConstraint(const std::string& constraint);
    bool hasConstraint(const std::string& constraint) const;
    bool isAllowed(const std::string& operation) const;
    
    void clear();
    void clearLocal();
};

} // namespace CHTL

#endif // CHTL_CONTEXT_H