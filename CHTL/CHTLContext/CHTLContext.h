#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include "../CHTLState/CHTLState.h"
#include "../CHTLLexer/CHTLGlobalMap.h"

namespace CHTL {

// CHTL上下文管理器
class CHTLContext {
public:
    CHTLContext();
    ~CHTLContext() = default;
    
    // 状态管理
    void setState(CHTLCompilerState state);
    CHTLCompilerState getCurrentState() const;
    bool canTransitionTo(CHTLCompilerState newState) const;
    bool transitionTo(CHTLCompilerState newState, const std::string& trigger = "");
    
    // 上下文管理
    void pushContext();
    void popContext();
    ParseContext& getCurrentContext();
    const ParseContext& getCurrentContext() const;
    
    // 元素栈管理
    void pushElement(const std::string& element);
    void popElement();
    std::string getCurrentElement() const;
    std::vector<std::string> getElementStack() const;
    
    // 命名空间管理
    void setCurrentNamespace(const std::string& namespace_);
    std::string getCurrentNamespace() const;
    bool createNamespace(const std::string& name, const std::string& sourceFile = "");
    bool createNestedNamespace(const std::string& parent, const std::string& child, const std::string& sourceFile = "");
    
    // 模板管理
    void setCurrentTemplate(const std::string& template_);
    std::string getCurrentTemplate() const;
    
    // 自定义管理
    void setCurrentCustom(const std::string& custom);
    std::string getCurrentCustom() const;
    
    // 原始嵌入管理
    void setCurrentOrigin(const std::string& origin);
    std::string getCurrentOrigin() const;
    
    // 块状态管理
    void enterStyleBlock();
    void exitStyleBlock();
    void enterScriptBlock();
    void exitScriptBlock();
    void enterTemplateBlock();
    void exitTemplateBlock();
    void enterCustomBlock();
    void exitCustomBlock();
    void enterOriginBlock();
    void exitOriginBlock();
    void enterConstraintBlock();
    void exitConstraintBlock();
    
    // 状态查询
    bool isInStyleBlock() const;
    bool isInScriptBlock() const;
    bool isInTemplateBlock() const;
    bool isInCustomBlock() const;
    bool isInOriginBlock() const;
    bool isInConstraintBlock() const;
    
    // 全局映射表管理
    void addSymbol(const GlobalSymbol& symbol);
    std::shared_ptr<GlobalSymbol> findSymbol(const std::string& name, 
                                            GlobalSymbolType type = GlobalSymbolType::TEMPLATE_STYLE,
                                            const std::string& namespace_ = "");
    bool hasSymbol(const std::string& name, 
                  GlobalSymbolType type = GlobalSymbolType::TEMPLATE_STYLE,
                  const std::string& namespace_ = "");
    
    // 命名空间项管理
    bool addNamespaceItem(const std::string& namespaceName, std::shared_ptr<NamespaceItem> item);
    std::shared_ptr<NamespaceItem> getNamespaceItem(const std::string& namespaceName, const std::string& itemName);
    std::vector<std::shared_ptr<NamespaceItem>> getNamespaceItems(const std::string& namespaceName);
    
    // 配置组管理
    bool addConfigurationGroup(const std::string& name, const ConfigurationGroup& config);
    std::shared_ptr<ConfigurationGroup> getConfigurationGroup(const std::string& name);
    bool updateConfigurationItem(const std::string& groupName, const std::string& key, const std::string& value);
    
    // 导入管理
    void addImport(const ImportInfo& import);
    std::vector<ImportInfo> getImports() const;
    bool checkCircularDependency(const std::string& filePath);
    bool checkDuplicateImport(const std::string& type, const std::string& path);
    
    // 路径解析
    std::vector<std::string> resolveWildcardPath(const std::string& path);
    std::vector<std::string> resolveSubModulePath(const std::string& path);
    
    // 错误处理
    void setError(const std::string& error, size_t line = 0, size_t column = 0);
    bool hasError() const;
    const std::vector<std::string>& getErrors() const;
    void clearErrors();
    
    // 状态历史
    void addStateTransition(const StateTransition& transition);
    const std::vector<StateTransition>& getStateHistory() const;
    
    // 重置上下文
    void reset();
    
    // 调试信息
    std::string getContextInfo() const;
    std::string getStateInfo() const;
    
    // 获取内部组件
    std::shared_ptr<CHTLState> getState() const { return state_; }
    std::shared_ptr<CHTLGlobalMap> getGlobalMap() const { return globalMap_; }
    
private:
    std::shared_ptr<CHTLState> state_;
    std::shared_ptr<CHTLGlobalMap> globalMap_;
    
    // 上下文验证
    bool validateContext() const;
    void updateContext();
    
    // 状态转换日志
    void logStateTransition(CHTLCompilerState from, CHTLCompilerState to, const std::string& trigger);
};

} // namespace CHTL