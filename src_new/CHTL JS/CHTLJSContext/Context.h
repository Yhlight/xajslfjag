#pragma once

#include <string>
#include <unordered_map>
#include <memory>

namespace CHTL {

class CHTLJSContext {
public:
    CHTLJSContext();
    ~CHTLJSContext();
    
    // 虚拟对象管理
    void registerVirtualObject(const std::string& name, const std::string& definition);
    bool hasVirtualObject(const std::string& name) const;
    std::string getVirtualObjectDefinition(const std::string& name) const;
    
    // 函数注册
    void registerFunction(const std::string& name, const std::string& signature);
    bool hasFunction(const std::string& name) const;
    
private:
    std::unordered_map<std::string, std::string> m_virtualObjects;
    std::unordered_map<std::string, std::string> m_functions;
};

}