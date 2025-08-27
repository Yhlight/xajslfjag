#pragma once

#include <string>
#include <vector>
#include <memory>
#include <stack>
#include <unordered_map>

namespace CHTL {

// 前向声明
class BaseNode;
class ConfigNode;

// CHTL上下文 - 管理解析和生成过程中的状态
class CHTLContext {
public:
    CHTLContext();
    ~CHTLContext();
    
    // 作用域管理
    void EnterScope(const std::string& scopeType);
    void ExitScope();
    std::string GetCurrentScope() const;
    bool IsInScope(const std::string& scopeType) const;
    
    // 元素栈管理
    void PushElement(const std::string& elementName);
    void PopElement();
    std::string GetCurrentElement() const;
    std::vector<std::string> GetElementPath() const;
    
    // 配置管理
    void SetActiveConfig(std::shared_ptr<ConfigNode> config);
    std::shared_ptr<ConfigNode> GetActiveConfig() const;
    
    // 局部样式/脚本管理
    void SetInLocalStyle(bool inLocal) { inLocalStyle = inLocal; }
    void SetInLocalScript(bool inLocal) { inLocalScript = inLocal; }
    bool IsInLocalStyle() const { return inLocalStyle; }
    bool IsInLocalScript() const { return inLocalScript; }
    
    // 自动化选择器状态
    void SetAutoClassAdded(bool added) { autoClassAdded = added; }
    void SetAutoIdAdded(bool added) { autoIdAdded = added; }
    bool IsAutoClassAdded() const { return autoClassAdded; }
    bool IsAutoIdAdded() const { return autoIdAdded; }
    
    // 临时变量存储
    void SetVariable(const std::string& name, const std::string& value);
    std::string GetVariable(const std::string& name) const;
    bool HasVariable(const std::string& name) const;
    void ClearVariables();
    
    // 错误上下文
    void SetCurrentFile(const std::string& filename) { currentFile = filename; }
    std::string GetCurrentFile() const { return currentFile; }
    
    // 调试信息
    std::string GetContextInfo() const;
    
private:
    // 作用域栈
    std::stack<std::string> scopeStack;
    
    // 元素栈
    std::stack<std::string> elementStack;
    
    // 活动配置
    std::shared_ptr<ConfigNode> activeConfig;
    
    // 状态标志
    bool inLocalStyle;
    bool inLocalScript;
    bool autoClassAdded;
    bool autoIdAdded;
    
    // 临时变量
    std::unordered_map<std::string, std::string> variables;
    
    // 当前文件
    std::string currentFile;
};

} // namespace CHTL