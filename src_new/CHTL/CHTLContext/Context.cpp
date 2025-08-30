#include "Context.h"
#include <regex>

namespace CHTL {

// VarGroup实现
void VarGroup::SetVariable(const std::string& name, const std::string& value) {
    m_variables[name] = value;
}

std::string VarGroup::GetVariable(const std::string& name) const {
    auto it = m_variables.find(name);
    if (it != m_variables.end()) {
        return it->second;
    }
    return "";
}

bool VarGroup::HasVariable(const std::string& name) const {
    return m_variables.find(name) != m_variables.end();
}

const std::unordered_map<std::string, std::string>& VarGroup::GetAllVariables() const {
    return m_variables;
}

void VarGroup::Merge(const VarGroup& other) {
    for (const auto& [name, value] : other.m_variables) {
        if (m_variables.find(name) == m_variables.end()) {
            m_variables[name] = value;
        }
    }
}

void VarGroup::Clear() {
    m_variables.clear();
}

std::string VarGroup::ReplaceVariables(const std::string& content, const VarGroup& varGroup) {
    std::string result = content;
    
    // 替换VarGroup(varName)格式的变量引用
    std::regex varRegex(R"((\w+)\((\w+)\))");
    auto iter = std::sregex_iterator(result.begin(), result.end(), varRegex);
    auto end = std::sregex_iterator();
    
    std::vector<std::pair<std::string, std::string>> replacements;
    
    for (; iter != end; ++iter) {
        std::string varGroupName = (*iter)[1].str();
        std::string varName = (*iter)[2].str();
        std::string fullRef = (*iter).str();
        
        // 查找变量值
        std::string value = varGroup.GetVariable(varName);
        if (!value.empty()) {
            replacements.push_back({fullRef, value});
        }
    }
    
    // 执行替换
    for (const auto& replacement : replacements) {
        size_t pos = 0;
        while ((pos = result.find(replacement.first, pos)) != std::string::npos) {
            result.replace(pos, replacement.first.length(), replacement.second);
            pos += replacement.second.length();
        }
    }
    
    return result;
}

bool VarGroup::ContainsVariableReferences(const std::string& content) {
    std::regex varRegex(R"(\w+\(\w+\))");
    return std::regex_search(content, varRegex);
}

// CHTLContext实现
CHTLContext::CHTLContext() {
    m_varGroup = std::make_shared<VarGroup>();
}

CHTLContext::~CHTLContext() = default;

std::shared_ptr<VarGroup> CHTLContext::CreateVarGroup() {
    auto newVarGroup = std::make_shared<VarGroup>();
    m_varGroup = newVarGroup;
    return newVarGroup;
}

}