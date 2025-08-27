#include "Context.h"
#include <sstream>
#include <algorithm>
#include <stdexcept>

namespace CHTL {

Context::Context() 
    : stateManager(std::make_unique<StateManager>())
    , globalMap(std::make_unique<GlobalMap>())
    , debugMode(false)
    , strictMode(false) {
    InitializeDefaults();
}

Context::~Context() = default;

Context::Context(Context&& other) noexcept
    : stateManager(std::move(other.stateManager))
    , globalMap(std::move(other.globalMap))
    , currentFilePath(std::move(other.currentFilePath))
    , namespaceStack(std::move(other.namespaceStack))
    , templateDefinitions(std::move(other.templateDefinitions))
    , customDefinitions(std::move(other.customDefinitions))
    , namespaceDefinitions(std::move(other.namespaceDefinitions))
    , configurationNode(std::move(other.configurationNode))
    , importedFiles(std::move(other.importedFiles))
    , errorMessages(std::move(other.errorMessages))
    , warningMessages(std::move(other.warningMessages))
    , debugMode(other.debugMode)
    , strictMode(other.strictMode) {
}

Context& Context::operator=(Context&& other) noexcept {
    if (this != &other) {
        stateManager = std::move(other.stateManager);
        globalMap = std::move(other.globalMap);
        currentFilePath = std::move(other.currentFilePath);
        namespaceStack = std::move(other.namespaceStack);
        templateDefinitions = std::move(other.templateDefinitions);
        customDefinitions = std::move(other.customDefinitions);
        namespaceDefinitions = std::move(other.namespaceDefinitions);
        configurationNode = std::move(other.configurationNode);
        importedFiles = std::move(other.importedFiles);
        errorMessages = std::move(other.errorMessages);
        warningMessages = std::move(other.warningMessages);
        debugMode = other.debugMode;
        strictMode = other.strictMode;
    }
    return *this;
}

StateManager& Context::GetStateManager() {
    return *stateManager;
}

const StateManager& Context::GetStateManager() const {
    return *stateManager;
}

GlobalMap& Context::GetGlobalMap() {
    return *globalMap;
}

const GlobalMap& Context::GetGlobalMap() const {
    return *globalMap;
}

void Context::SetCurrentFilePath(const std::string& filePath) {
    currentFilePath = filePath;
}

const std::string& Context::GetCurrentFilePath() const {
    return currentFilePath;
}

void Context::EnterNamespace(const std::string& namespaceName) {
    namespaceStack.push(namespaceName);
    stateManager->SetCurrentNamespace(GetFullNamespacePath());
}

void Context::ExitNamespace() {
    if (!namespaceStack.empty()) {
        namespaceStack.pop();
        stateManager->SetCurrentNamespace(GetFullNamespacePath());
    }
}

std::string Context::GetCurrentNamespace() const {
    return namespaceStack.empty() ? "" : namespaceStack.top();
}

std::string Context::GetFullNamespacePath() const {
    if (namespaceStack.empty()) {
        return "";
    }
    
    std::ostringstream oss;
    std::stack<std::string> temp = namespaceStack;
    std::vector<std::string> namespaces;
    
    while (!temp.empty()) {
        namespaces.push_back(temp.top());
        temp.pop();
    }
    
    std::reverse(namespaces.begin(), namespaces.end());
    
    for (size_t i = 0; i < namespaces.size(); ++i) {
        if (i > 0) {
            oss << ".";
        }
        oss << namespaces[i];
    }
    
    return oss.str();
}

void Context::RegisterTemplate(const std::string& name, std::shared_ptr<TemplateNode> templateNode) {
    std::string fullName = GetFullNamespacePath();
    if (!fullName.empty()) {
        fullName += ".";
    }
    fullName += name;
    
    templateDefinitions[fullName] = templateNode;
}

std::shared_ptr<TemplateNode> Context::GetTemplate(const std::string& name) const {
    // 首先尝试在当前命名空间中查找
    std::string fullName = GetFullNamespacePath();
    if (!fullName.empty()) {
        fullName += ".";
    }
    fullName += name;
    
    auto it = templateDefinitions.find(fullName);
    if (it != templateDefinitions.end()) {
        return it->second;
    }
    
    // 如果在当前命名空间中找不到，尝试在全局命名空间中查找
    it = templateDefinitions.find(name);
    if (it != templateDefinitions.end()) {
        return it->second;
    }
    
    return nullptr;
}

bool Context::HasTemplate(const std::string& name) const {
    return GetTemplate(name) != nullptr;
}

void Context::RegisterCustom(const std::string& name, std::shared_ptr<CustomNode> customNode) {
    std::string fullName = GetFullNamespacePath();
    if (!fullName.empty()) {
        fullName += ".";
    }
    fullName += name;
    
    customDefinitions[fullName] = customNode;
}

std::shared_ptr<CustomNode> Context::GetCustom(const std::string& name) const {
    // 首先尝试在当前命名空间中查找
    std::string fullName = GetFullNamespacePath();
    if (!fullName.empty()) {
        fullName += ".";
    }
    fullName += name;
    
    auto it = customDefinitions.find(fullName);
    if (it != customDefinitions.end()) {
        return it->second;
    }
    
    // 如果在当前命名空间中找不到，尝试在全局命名空间中查找
    it = customDefinitions.find(name);
    if (it != customDefinitions.end()) {
        return it->second;
    }
    
    return nullptr;
}

bool Context::HasCustom(const std::string& name) const {
    return GetCustom(name) != nullptr;
}

void Context::RegisterNamespace(const std::string& name, std::shared_ptr<NamespaceNode> namespaceNode) {
    std::string fullName = GetFullNamespacePath();
    if (!fullName.empty()) {
        fullName += ".";
    }
    fullName += name;
    
    namespaceDefinitions[fullName] = namespaceNode;
}

std::shared_ptr<NamespaceNode> Context::GetNamespace(const std::string& name) const {
    auto it = namespaceDefinitions.find(name);
    if (it != namespaceDefinitions.end()) {
        return it->second;
    }
    return nullptr;
}

bool Context::HasNamespace(const std::string& name) const {
    return GetNamespace(name) != nullptr;
}

void Context::SetConfiguration(std::shared_ptr<ConfigurationNode> configNode) {
    configurationNode = configNode;
}

std::shared_ptr<ConfigurationNode> Context::GetConfiguration() const {
    return configurationNode;
}

void Context::AddImportedFile(const std::string& filePath) {
    importedFiles.push_back(filePath);
}

bool Context::IsFileImported(const std::string& filePath) const {
    return std::find(importedFiles.begin(), importedFiles.end(), filePath) != importedFiles.end();
}

const std::vector<std::string>& Context::GetImportedFiles() const {
    return importedFiles;
}

void Context::AddError(const std::string& message) {
    errorMessages.push_back(message);
}

void Context::AddError(const std::string& message, size_t line, size_t column) {
    errorMessages.push_back(FormatMessage(message, line, column));
}

void Context::AddWarning(const std::string& message) {
    warningMessages.push_back(message);
}

void Context::AddWarning(const std::string& message, size_t line, size_t column) {
    warningMessages.push_back(FormatMessage(message, line, column));
}

const std::vector<std::string>& Context::GetErrors() const {
    return errorMessages;
}

const std::vector<std::string>& Context::GetWarnings() const {
    return warningMessages;
}

bool Context::HasErrors() const {
    return !errorMessages.empty();
}

bool Context::HasWarnings() const {
    return !warningMessages.empty();
}

void Context::ClearMessages() {
    errorMessages.clear();
    warningMessages.clear();
}

void Context::SetDebugMode(bool enabled) {
    debugMode = enabled;
}

bool Context::IsDebugMode() const {
    return debugMode;
}

void Context::SetStrictMode(bool enabled) {
    strictMode = enabled;
}

bool Context::IsStrictMode() const {
    return strictMode;
}

void Context::Reset() {
    stateManager->Reset();
    globalMap->Reset();
    currentFilePath.clear();
    
    while (!namespaceStack.empty()) {
        namespaceStack.pop();
    }
    
    templateDefinitions.clear();
    customDefinitions.clear();
    namespaceDefinitions.clear();
    configurationNode.reset();
    importedFiles.clear();
    errorMessages.clear();
    warningMessages.clear();
    
    InitializeDefaults();
}

std::unique_ptr<Context> Context::Clone() const {
    auto newContext = std::make_unique<Context>();
    
    // 复制基本设置
    newContext->currentFilePath = currentFilePath;
    newContext->debugMode = debugMode;
    newContext->strictMode = strictMode;
    
    // 复制命名空间栈
    newContext->namespaceStack = namespaceStack;
    
    // 复制定义映射（这些通常是只读的）
    newContext->templateDefinitions = templateDefinitions;
    newContext->customDefinitions = customDefinitions;
    newContext->namespaceDefinitions = namespaceDefinitions;
    newContext->configurationNode = configurationNode;
    
    // 复制导入文件列表
    newContext->importedFiles = importedFiles;
    
    // 不复制错误和警告消息（新的上下文应该有自己的错误追踪）
    
    return newContext;
}

void Context::Merge(const Context& other) {
    // 合并定义映射
    for (const auto& pair : other.templateDefinitions) {
        templateDefinitions[pair.first] = pair.second;
    }
    
    for (const auto& pair : other.customDefinitions) {
        customDefinitions[pair.first] = pair.second;
    }
    
    for (const auto& pair : other.namespaceDefinitions) {
        namespaceDefinitions[pair.first] = pair.second;
    }
    
    // 合并导入文件列表
    for (const std::string& file : other.importedFiles) {
        if (!IsFileImported(file)) {
            importedFiles.push_back(file);
        }
    }
    
    // 合并错误和警告消息
    errorMessages.insert(errorMessages.end(), other.errorMessages.begin(), other.errorMessages.end());
    warningMessages.insert(warningMessages.end(), other.warningMessages.begin(), other.warningMessages.end());
}

std::string Context::GetStatistics() const {
    std::ostringstream oss;
    oss << "CHTL上下文统计信息:\n";
    oss << "  当前文件: " << currentFilePath << "\n";
    oss << "  当前命名空间: " << GetFullNamespacePath() << "\n";
    oss << "  模板定义数量: " << templateDefinitions.size() << "\n";
    oss << "  自定义定义数量: " << customDefinitions.size() << "\n";
    oss << "  命名空间数量: " << namespaceDefinitions.size() << "\n";
    oss << "  导入文件数量: " << importedFiles.size() << "\n";
    oss << "  错误数量: " << errorMessages.size() << "\n";
    oss << "  警告数量: " << warningMessages.size() << "\n";
    oss << "  调试模式: " << (debugMode ? "开启" : "关闭") << "\n";
    oss << "  严格模式: " << (strictMode ? "开启" : "关闭") << "\n";
    oss << "  状态栈深度: " << stateManager->GetStackDepth() << "\n";
    return oss.str();
}

bool Context::Validate() const {
    // 验证状态管理器状态
    if (!stateManager->IsBalanced()) {
        return false;
    }
    
    // 验证命名空间一致性
    if (namespaceStack.empty() && !GetFullNamespacePath().empty()) {
        return false;
    }
    
    // 验证其他一致性检查
    // ...
    
    return true;
}

void Context::InitializeDefaults() {
    // 设置默认配置
    debugMode = false;
    strictMode = false;
}

std::string Context::FormatMessage(const std::string& message, size_t line, size_t column) const {
    std::ostringstream oss;
    oss << "[" << currentFilePath << ":" << line << ":" << column << "] " << message;
    return oss.str();
}

// NamespaceGuard实现

NamespaceGuard::NamespaceGuard(Context& ctx, const std::string& namespaceName)
    : context(ctx), shouldExit(true) {
    context.EnterNamespace(namespaceName);
}

NamespaceGuard::~NamespaceGuard() {
    if (shouldExit) {
        try {
            context.ExitNamespace();
        } catch (...) {
            // 析构函数中不抛出异常
        }
    }
}

NamespaceGuard::NamespaceGuard(NamespaceGuard&& other) noexcept
    : context(other.context), shouldExit(other.shouldExit) {
    other.shouldExit = false;
}

NamespaceGuard& NamespaceGuard::operator=(NamespaceGuard&& other) noexcept {
    if (this != &other) {
        if (shouldExit) {
            try {
                context.ExitNamespace();
            } catch (...) {
                // 忽略异常
            }
        }
        
        // 由于Context是引用，不能重新赋值，所以这里应该检查是否是同一个context
        // 实际上移动赋值在这个场景下是有问题的，因为context是引用
        // 我们简单地更新状态
        shouldExit = other.shouldExit;
        other.shouldExit = false;
    }
    return *this;
}

void NamespaceGuard::Release() {
    if (shouldExit) {
        context.ExitNamespace();
        shouldExit = false;
    }
}

} // namespace CHTL