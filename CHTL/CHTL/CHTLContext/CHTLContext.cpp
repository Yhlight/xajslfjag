#include "CHTLContext.h"
#include <algorithm>
#include <sstream>

namespace CHTL {

// Scope 实现
bool Scope::addSymbol(const Symbol& symbol) {
    if (hasSymbol(symbol.name)) {
        return false;  // 符号已存在
    }
    symbols[symbol.name] = symbol;
    return true;
}

Symbol* Scope::findSymbol(const std::string& name) {
    auto it = symbols.find(name);
    return it != symbols.end() ? &it->second : nullptr;
}

Symbol* Scope::findSymbolRecursive(const std::string& name) {
    // 先在当前作用域查找
    Symbol* symbol = findSymbol(name);
    if (symbol) {
        return symbol;
    }
    
    // 递归向上查找
    if (parent) {
        return parent->findSymbolRecursive(name);
    }
    
    return nullptr;
}

bool Scope::hasSymbol(const std::string& name) const {
    return symbols.find(name) != symbols.end();
}

void Scope::addChild(std::shared_ptr<Scope> child) {
    children.push_back(child);
}

std::shared_ptr<Scope> Scope::findChild(const std::string& name) {
    for (auto& child : children) {
        if (child->getName() == name) {
            return child;
        }
    }
    return nullptr;
}

// ScopeGuard 实现
ScopeGuard::ScopeGuard(CHTLContext* ctx, ScopeType type, const std::string& name)
    : context(ctx) {
    if (context) {
        context->enterScope(type, name);
    }
}

ScopeGuard::~ScopeGuard() {
    if (context) {
        context->exitScope();
    }
}

ScopeGuard::ScopeGuard(ScopeGuard&& other) noexcept
    : context(other.context) {
    other.context = nullptr;
}

ScopeGuard& ScopeGuard::operator=(ScopeGuard&& other) noexcept {
    if (this != &other) {
        if (context) {
            context->exitScope();
        }
        context = other.context;
        other.context = nullptr;
    }
    return *this;
}

// CHTLContext 实现
CHTLContext::CHTLContext() {
    state = std::make_unique<CHTLState>();
    globalScope = std::make_shared<Scope>(ScopeType::GLOBAL, "global");
    currentScope = globalScope;
    scopeStack.push(globalScope);
}

void CHTLContext::enterScope(ScopeType type, const std::string& name) {
    auto newScope = std::make_shared<Scope>(type, name, currentScope);
    currentScope->addChild(newScope);
    currentScope = newScope;
    scopeStack.push(newScope);
}

void CHTLContext::exitScope() {
    if (scopeStack.size() > 1) {
        scopeStack.pop();
        currentScope = scopeStack.top();
    }
}

ScopeGuard CHTLContext::createScope(ScopeType type, const std::string& name) {
    return ScopeGuard(this, type, name);
}

bool CHTLContext::registerSymbol(const std::string& name, SymbolType type, std::shared_ptr<BaseNode> node) {
    Symbol symbol(name, type, node);
    symbol.scope = currentScope->getType();
    
    // 如果在命名空间中，记录命名空间路径
    if (!currentNamespace.empty()) {
        symbol.namespacePath = currentNamespace;
    }
    
    return currentScope->addSymbol(symbol);
}

Symbol* CHTLContext::lookupSymbol(const std::string& name) {
    return currentScope->findSymbolRecursive(name);
}

Symbol* CHTLContext::lookupSymbolInNamespace(const std::string& name, const std::string& ns) {
    // 查找命名空间
    auto nsNode = getNamespace(ns);
    if (!nsNode) {
        return nullptr;
    }
    
    // 在命名空间作用域中查找符号
    // TODO: 实现命名空间符号查找
    // 暂时返回普通查找结果
    return lookupSymbol(name);
}

void CHTLContext::setActiveConfig(std::shared_ptr<ConfigNode> config) {
    activeConfig = config;
}

void CHTLContext::registerNamedConfig(const std::string& name, std::shared_ptr<ConfigNode> config) {
    namedConfigs[name] = config;
}

std::shared_ptr<ConfigNode> CHTLContext::getNamedConfig(const std::string& name) {
    auto it = namedConfigs.find(name);
    return it != namedConfigs.end() ? it->second : nullptr;
}

void CHTLContext::enterNamespace(const std::string& name) {
    if (currentNamespace.empty()) {
        currentNamespace = name;
    } else {
        currentNamespace += "." + name;
    }
}

void CHTLContext::exitNamespace() {
    auto pos = currentNamespace.rfind('.');
    if (pos != std::string::npos) {
        currentNamespace = currentNamespace.substr(0, pos);
    } else {
        currentNamespace.clear();
    }
}

void CHTLContext::registerNamespace(std::shared_ptr<NamespaceNode> ns) {
    namespaces[ns->getFullPath()] = ns;
}

std::shared_ptr<NamespaceNode> CHTLContext::getNamespace(const std::string& path) {
    auto it = namespaces.find(path);
    return it != namespaces.end() ? it->second : nullptr;
}

bool CHTLContext::registerImport(const std::string& path, const std::string& resolvedPath) {
    if (isImported(path)) {
        addWarning("File '" + path + "' is already imported");
        return false;
    }
    importedFiles[path] = resolvedPath;
    return true;
}

bool CHTLContext::isImported(const std::string& path) const {
    return importedFiles.find(path) != importedFiles.end();
}

std::string CHTLContext::getResolvedImport(const std::string& path) const {
    auto it = importedFiles.find(path);
    return it != importedFiles.end() ? it->second : "";
}

void CHTLContext::addImportedModule(const std::string& moduleName) {
    importedModules.insert(moduleName);
}

bool CHTLContext::isModuleImported(const std::string& moduleName) const {
    return importedModules.find(moduleName) != importedModules.end();
}

void CHTLContext::addConstraint(const std::vector<std::string>& exceptList, std::shared_ptr<BaseNode> scope) {
    constraints.push_back({exceptList, scope});
}

bool CHTLContext::isAllowed(const std::string& item) const {
    // 检查当前作用域的所有约束
    for (const auto& constraint : constraints) {
        if (std::find(constraint.exceptList.begin(), constraint.exceptList.end(), item) 
            != constraint.exceptList.end()) {
            return false;
        }
    }
    return true;
}

bool CHTLContext::isAllowedInScope(const std::string& item, std::shared_ptr<BaseNode> scope) const {
    for (const auto& constraint : constraints) {
        if (constraint.scope == scope) {
            if (std::find(constraint.exceptList.begin(), constraint.exceptList.end(), item) 
                != constraint.exceptList.end()) {
                return false;
            }
        }
    }
    return true;
}

bool CHTLContext::validateContext() const {
    // 验证状态机
    if (!state->validateCurrentState()) {
        return false;
    }
    
    // 验证作用域栈
    if (scopeStack.empty()) {
        return false;
    }
    
    return true;
}

bool CHTLContext::validateSymbolUsage(const std::string& symbol, StateType currentState) const {
    // 根据当前状态验证符号使用是否合法
    // 这里需要根据语法文档实现具体的验证规则
    
    // 例如：在全局样式块中的限制
    if (currentState == StateType::IN_STYLE && currentScope->getType() == ScopeType::GLOBAL) {
        // 允许的符号类型
        std::vector<std::string> allowedInGlobalStyle = {
            "@Style", "@Var", "inherit", "delete", "from"
        };
        
        // 检查是否在允许列表中
        bool allowed = std::find(allowedInGlobalStyle.begin(), 
                               allowedInGlobalStyle.end(), symbol) != allowedInGlobalStyle.end();
        
        if (!allowed && !isAllowed(symbol)) {
            return false;
        }
    }
    
    return true;
}

bool CHTLContext::validateStyleBlockContent(const std::vector<std::string>& usedFeatures, bool isLocal) const {
    if (isLocal) {
        // 局部样式块允许的功能
        std::vector<std::string> allowedLocal = {
            "@Style", "@Var", "inherit", "delete", "from", 
            ".", "#", "&", "::", ":"  // 选择器相关
        };
        
        for (const auto& feature : usedFeatures) {
            if (std::find(allowedLocal.begin(), allowedLocal.end(), feature) == allowedLocal.end()) {
                return false;
            }
        }
    } else {
        // 全局样式块的限制更严格
        std::vector<std::string> allowedGlobal = {
            "@Style", "@Var", "inherit", "delete", "from"
        };
        
        for (const auto& feature : usedFeatures) {
            if (std::find(allowedGlobal.begin(), allowedGlobal.end(), feature) == allowedGlobal.end()) {
                return false;
            }
        }
    }
    
    return true;
}

bool CHTLContext::validateScriptBlockContent(const std::vector<std::string>& usedFeatures, bool isLocal) const {
    if (isLocal) {
        // 局部脚本块允许CHTL特性
        std::vector<std::string> allowedLocal = {
            "@Var", "from", "--", "[Origin]"
        };
        
        for (const auto& feature : usedFeatures) {
            if (feature.find("@") == 0 || feature.find("[") == 0) {
                if (std::find(allowedLocal.begin(), allowedLocal.end(), feature) == allowedLocal.end()) {
                    return false;
                }
            }
        }
    } else {
        // 全局脚本块禁止大部分CHTL语法
        for (const auto& feature : usedFeatures) {
            if (feature != "--" && feature != "[Origin]") {
                if (feature.find("@") == 0 || feature.find("[") == 0) {
                    return false;
                }
            }
        }
    }
    
    return true;
}

void CHTLContext::addError(const std::string& error) {
    errors.push_back(error);
}

void CHTLContext::addWarning(const std::string& warning) {
    warnings.push_back(warning);
}

void CHTLContext::reset() {
    // 重置状态机
    state->reset();
    
    // 清空作用域栈，只保留全局作用域
    while (!scopeStack.empty()) {
        scopeStack.pop();
    }
    
    // 重新初始化
    globalScope = std::make_shared<Scope>(ScopeType::GLOBAL, "global");
    currentScope = globalScope;
    scopeStack.push(globalScope);
    
    // 清空其他状态
    activeConfig = nullptr;
    namedConfigs.clear();
    namespaces.clear();
    currentNamespace.clear();
    importedFiles.clear();
    importedModules.clear();
    constraints.clear();
    errors.clear();
    warnings.clear();
}

} // namespace CHTL