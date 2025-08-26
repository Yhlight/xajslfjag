#ifndef CHTL_CONTEXT_H
#define CHTL_CONTEXT_H

#include <string>
#include <unordered_map>
#include <vector>
#include <stack>

namespace CHTL {

enum class ContextType {
    GLOBAL, NAMESPACE, ELEMENT, STYLE, SCRIPT, TEMPLATE, CUSTOM
};

struct ContextInfo {
    ContextType type;
    std::string name;
    std::unordered_map<std::string, std::string> properties;
    
    ContextInfo(ContextType t = ContextType::GLOBAL, const std::string& n = "")
        : type(t), name(n) {}
};

class Context {
private:
    std::stack<ContextInfo> context_stack;
    std::unordered_map<std::string, std::string> global_symbols;
    
public:
    Context();
    
    void pushContext(ContextType type, const std::string& name = "");
    void popContext();
    
    const ContextInfo& getCurrentContext() const;
    bool isInContext(ContextType type) const;
    
    void setProperty(const std::string& key, const std::string& value);
    std::string getProperty(const std::string& key) const;
    
    void addSymbol(const std::string& name, const std::string& type);
    bool hasSymbol(const std::string& name) const;
    
    void clear();
};

} // namespace CHTL

#endif