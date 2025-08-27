#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <stack>
#include "../CHTLState/State.h"
#include "../CHTLLexer/GlobalMap.h"

namespace CHTL {

// 前向声明
class CHTLNode;
class TemplateNode;
class CustomNode;
class NamespaceNode;
class ConfigurationNode;

/// CHTL解析上下文类，管理解析过程中的所有上下文信息
class Context {
private:
    /// 状态管理器
    std::unique_ptr<StateManager> stateManager;
    
    /// 全局映射表
    std::unique_ptr<GlobalMap> globalMap;
    
    /// 当前文件路径
    std::string currentFilePath;
    
    /// 当前命名空间栈
    std::stack<std::string> namespaceStack;
    
    /// 模板定义映射
    std::unordered_map<std::string, std::shared_ptr<TemplateNode>> templateDefinitions;
    
    /// 自定义定义映射
    std::unordered_map<std::string, std::shared_ptr<CustomNode>> customDefinitions;
    
    /// 命名空间映射
    std::unordered_map<std::string, std::shared_ptr<NamespaceNode>> namespaceDefinitions;
    
    /// 配置节点
    std::shared_ptr<ConfigurationNode> configurationNode;
    
    /// 导入的文件列表（用于循环依赖检测）
    std::vector<std::string> importedFiles;
    
    /// 错误消息列表
    std::vector<std::string> errorMessages;
    
    /// 警告消息列表
    std::vector<std::string> warningMessages;
    
    /// 调试信息开关
    bool debugMode;
    
    /// 严格模式开关
    bool strictMode;

public:
    Context();
    ~Context();
    
    // 禁止拷贝构造和赋值
    Context(const Context&) = delete;
    Context& operator=(const Context&) = delete;
    
    // 允许移动
    Context(Context&&) noexcept;
    Context& operator=(Context&&) noexcept;
    
    /// 获取状态管理器
    StateManager& GetStateManager();
    const StateManager& GetStateManager() const;
    
    /// 获取全局映射表
    GlobalMap& GetGlobalMap();
    const GlobalMap& GetGlobalMap() const;
    
    /// 设置当前文件路径
    void SetCurrentFilePath(const std::string& filePath);
    
    /// 获取当前文件路径
    const std::string& GetCurrentFilePath() const;
    
    /// 进入命名空间
    void EnterNamespace(const std::string& namespaceName);
    
    /// 退出命名空间
    void ExitNamespace();
    
    /// 获取当前命名空间
    std::string GetCurrentNamespace() const;
    
    /// 获取完整的命名空间路径
    std::string GetFullNamespacePath() const;
    
    /// 注册模板定义
    void RegisterTemplate(const std::string& name, std::shared_ptr<TemplateNode> templateNode);
    
    /// 获取模板定义
    std::shared_ptr<TemplateNode> GetTemplate(const std::string& name) const;
    
    /// 检查模板是否存在
    bool HasTemplate(const std::string& name) const;
    
    /// 注册自定义定义
    void RegisterCustom(const std::string& name, std::shared_ptr<CustomNode> customNode);
    
    /// 获取自定义定义
    std::shared_ptr<CustomNode> GetCustom(const std::string& name) const;
    
    /// 检查自定义是否存在
    bool HasCustom(const std::string& name) const;
    
    /// 注册命名空间
    void RegisterNamespace(const std::string& name, std::shared_ptr<NamespaceNode> namespaceNode);
    
    /// 获取命名空间
    std::shared_ptr<NamespaceNode> GetNamespace(const std::string& name) const;
    
    /// 检查命名空间是否存在
    bool HasNamespace(const std::string& name) const;
    
    /// 设置配置节点
    void SetConfiguration(std::shared_ptr<ConfigurationNode> configNode);
    
    /// 获取配置节点
    std::shared_ptr<ConfigurationNode> GetConfiguration() const;
    
    /// 添加导入文件
    void AddImportedFile(const std::string& filePath);
    
    /// 检查文件是否已导入（循环依赖检测）
    bool IsFileImported(const std::string& filePath) const;
    
    /// 获取导入文件列表
    const std::vector<std::string>& GetImportedFiles() const;
    
    /// 添加错误消息
    void AddError(const std::string& message);
    
    /// 添加错误消息（带位置信息）
    void AddError(const std::string& message, size_t line, size_t column);
    
    /// 添加警告消息
    void AddWarning(const std::string& message);
    
    /// 添加警告消息（带位置信息）
    void AddWarning(const std::string& message, size_t line, size_t column);
    
    /// 获取错误消息列表
    const std::vector<std::string>& GetErrors() const;
    
    /// 获取警告消息列表
    const std::vector<std::string>& GetWarnings() const;
    
    /// 检查是否有错误
    bool HasErrors() const;
    
    /// 检查是否有警告
    bool HasWarnings() const;
    
    /// 清除所有错误和警告
    void ClearMessages();
    
    /// 设置调试模式
    void SetDebugMode(bool enabled);
    
    /// 获取调试模式状态
    bool IsDebugMode() const;
    
    /// 设置严格模式
    void SetStrictMode(bool enabled);
    
    /// 获取严格模式状态
    bool IsStrictMode() const;
    
    /// 重置上下文
    void Reset();
    
    /// 克隆上下文（用于并行处理）
    std::unique_ptr<Context> Clone() const;
    
    /// 合并另一个上下文的信息
    void Merge(const Context& other);
    
    /// 获取上下文统计信息
    std::string GetStatistics() const;
    
    /// 验证上下文完整性
    bool Validate() const;

private:
    /// 初始化默认设置
    void InitializeDefaults();
    
    /// 格式化错误消息
    std::string FormatMessage(const std::string& message, size_t line, size_t column) const;
};

/// RAII上下文保护器，自动管理命名空间进入和退出
class NamespaceGuard {
private:
    Context& context;
    bool shouldExit;
    
public:
    NamespaceGuard(Context& ctx, const std::string& namespaceName);
    ~NamespaceGuard();
    
    // 禁止拷贝
    NamespaceGuard(const NamespaceGuard&) = delete;
    NamespaceGuard& operator=(const NamespaceGuard&) = delete;
    
    // 允许移动
    NamespaceGuard(NamespaceGuard&& other) noexcept;
    NamespaceGuard& operator=(NamespaceGuard&& other) noexcept;
    
    /// 提前退出命名空间
    void Release();
};

} // namespace CHTL