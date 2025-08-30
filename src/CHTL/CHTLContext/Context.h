#ifndef CHTL_CONTEXT_H
#define CHTL_CONTEXT_H

#include <string>
#include <memory>
#include <unordered_map>
#include <stack>
#include <vector>
#include "../CHTLState/State.h"
#include "../CHTLLexer/GlobalMap.h"

namespace CHTL {

// 前向声明
class ConfigurationInfo;

// 编译上下文
class CompileContext {
public:
    CompileContext(const std::string& sourceFile);
    ~CompileContext() = default;
    
    // 文件信息
    const std::string& getSourceFile() const { return sourceFile_; }
    void setSourceFile(const std::string& file) { sourceFile_ = file; }
    
    // 当前位置信息
    size_t getCurrentLine() const { return currentLine_; }
    size_t getCurrentColumn() const { return currentColumn_; }
    void setPosition(size_t line, size_t col) {
        currentLine_ = line;
        currentColumn_ = col;
    }
    
    // 命名空间管理
    const std::string& getCurrentNamespace() const { return currentNamespace_; }
    void enterNamespace(const std::string& ns);
    void exitNamespace();
    std::string getFullQualifiedName(const std::string& name) const;
    
    // 配置管理
    void setConfiguration(std::shared_ptr<ConfigurationInfo> config);
    std::shared_ptr<ConfigurationInfo> getConfiguration() const { return configuration_; }
    
    // 获取配置项
    std::string getConfigValue(const std::string& key, const std::string& defaultValue = "") const;
    bool isConfigEnabled(const std::string& key, bool defaultValue = false) const;
    
    // 局部作用域管理
    void enterScope(const std::string& scopeName);
    void exitScope();
    const std::string& getCurrentScope() const;
    
    // 符号表管理（局部）
    void defineLocalSymbol(const std::string& name, const std::string& type);
    bool hasLocalSymbol(const std::string& name) const;
    std::string getLocalSymbolType(const std::string& name) const;
    
    // 错误和警告管理
    void addError(const std::string& message, size_t line = 0, size_t col = 0);
    void addWarning(const std::string& message, size_t line = 0, size_t col = 0);
    const std::vector<std::string>& getErrors() const { return errors_; }
    const std::vector<std::string>& getWarnings() const { return warnings_; }
    bool hasErrors() const { return !errors_.empty(); }
    
    // 导入管理
    void addImportedFile(const std::string& file);
    bool isFileImported(const std::string& file) const;
    const std::vector<std::string>& getImportedFiles() const { return importedFiles_; }
    
    // 选择器自动化上下文
    struct SelectorContext {
        std::vector<std::string> classSelectors;
        std::vector<std::string> idSelectors;
        bool hasAutoAddedClass = false;
        bool hasAutoAddedId = false;
        std::string currentElementName;
    };
    
    SelectorContext& getCurrentSelectorContext() { return selectorContext_; }
    const SelectorContext& getCurrentSelectorContext() const { return selectorContext_; }
    
    // 样式块上下文
    bool isInLocalStyleBlock() const { return inLocalStyleBlock_; }
    void setInLocalStyleBlock(bool value) { inLocalStyleBlock_ = value; }
    
    // 脚本块上下文
    bool isInLocalScriptBlock() const { return inLocalScriptBlock_; }
    void setInLocalScriptBlock(bool value) { inLocalScriptBlock_ = value; }
    
    // 获取状态管理器
    StateManager& getStateManager() { return stateManager_; }
    const StateManager& getStateManager() const { return stateManager_; }

private:
    std::string sourceFile_;
    size_t currentLine_;
    size_t currentColumn_;
    
    // 命名空间栈
    std::stack<std::string> namespaceStack_;
    std::string currentNamespace_;
    
    // 配置信息
    std::shared_ptr<ConfigurationInfo> configuration_;
    
    // 作用域栈
    std::stack<std::string> scopeStack_;
    
    // 局部符号表（每个作用域一个）
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> localSymbols_;
    
    // 错误和警告
    std::vector<std::string> errors_;
    std::vector<std::string> warnings_;
    
    // 导入的文件
    std::vector<std::string> importedFiles_;
    std::unordered_set<std::string> importedFileSet_;
    
    // 选择器自动化上下文
    SelectorContext selectorContext_;
    
    // 样式和脚本块标志
    bool inLocalStyleBlock_;
    bool inLocalScriptBlock_;
    
    // 状态管理器
    StateManager stateManager_;
};

// 上下文管理器（单例）
class ContextManager {
public:
    static ContextManager& getInstance() {
        static ContextManager instance;
        return instance;
    }
    
    // 创建新的编译上下文
    std::shared_ptr<CompileContext> createContext(const std::string& sourceFile);
    
    // 获取当前活动上下文
    std::shared_ptr<CompileContext> getCurrentContext() const;
    
    // 设置当前活动上下文
    void setCurrentContext(std::shared_ptr<CompileContext> context);
    
    // 获取指定文件的上下文
    std::shared_ptr<CompileContext> getContext(const std::string& file) const;
    
    // 移除上下文
    void removeContext(const std::string& file);
    
    // 清空所有上下文
    void clearAll();

private:
    ContextManager() = default;
    ContextManager(const ContextManager&) = delete;
    ContextManager& operator=(const ContextManager&) = delete;
    
    // 文件到上下文的映射
    std::unordered_map<std::string, std::shared_ptr<CompileContext>> contexts_;
    
    // 当前活动上下文
    std::shared_ptr<CompileContext> currentContext_;
};

// RAII上下文切换守卫
class ContextGuard {
public:
    explicit ContextGuard(std::shared_ptr<CompileContext> newContext)
        : manager_(ContextManager::getInstance()),
          previousContext_(manager_.getCurrentContext()) {
        manager_.setCurrentContext(newContext);
    }
    
    ~ContextGuard() {
        manager_.setCurrentContext(previousContext_);
    }
    
    // 禁止拷贝
    ContextGuard(const ContextGuard&) = delete;
    ContextGuard& operator=(const ContextGuard&) = delete;

private:
    ContextManager& manager_;
    std::shared_ptr<CompileContext> previousContext_;
};

} // namespace CHTL

#endif // CHTL_CONTEXT_H