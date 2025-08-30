#pragma once

#include <string>
#include <unordered_map>
#include <memory>

namespace CHTL {

class VarGroup {
public:
    VarGroup() = default;
    
    void SetVariable(const std::string& name, const std::string& value);
    std::string GetVariable(const std::string& name) const;
    bool HasVariable(const std::string& name) const;
    const std::unordered_map<std::string, std::string>& GetAllVariables() const;
    void Merge(const VarGroup& other);
    void Clear();
    
    static std::string ReplaceVariables(const std::string& content, const VarGroup& varGroup);
    static bool ContainsVariableReferences(const std::string& content);
    
private:
    std::unordered_map<std::string, std::string> m_variables;
};

class CHTLContext {
public:
    CHTLContext();
    ~CHTLContext();
    
    // 变量组管理
    std::shared_ptr<VarGroup> GetVarGroup() const { return m_varGroup; }
    void SetVarGroup(std::shared_ptr<VarGroup> varGroup) { m_varGroup = varGroup; }
    std::shared_ptr<VarGroup> CreateVarGroup();
    
    // 处理后的CSS
    std::string GetProcessedCSS() const { return m_processedCSS; }
    void SetProcessedCSS(const std::string& css) { m_processedCSS = css; }
    
private:
    std::shared_ptr<VarGroup> m_varGroup;
    std::string m_processedCSS;
};

}