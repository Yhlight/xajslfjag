#include "Context.h"
#include <sstream>
#include <algorithm>

namespace CHTL {

// ========== SymbolInfo 实现 ==========

void SymbolInfo::updateFullName() {
    if (namespace_.empty()) {
        fullName = name;
    } else {
        fullName = namespace_ + "::" + name;
    }
}

bool SymbolInfo::isTemplateSymbol() const {
    return type == SymbolType::TEMPLATE_STYLE ||
           type == SymbolType::TEMPLATE_ELEMENT ||
           type == SymbolType::TEMPLATE_VAR;
}

bool SymbolInfo::isCustomSymbol() const {
    return type == SymbolType::CUSTOM_STYLE ||
           type == SymbolType::CUSTOM_ELEMENT ||
           type == SymbolType::CUSTOM_VAR;
}

std::string SymbolInfo::toString() const {
    std::stringstream ss;
    ss << "SymbolInfo{type:" << static_cast<int>(type)
       << ", name:'" << name << "'"
       << ", fullName:'" << fullName << "'"
       << ", namespace:'" << namespace_ << "'"
       << ", isExported:" << isExported << "}";
    return ss.str();
}

// ========== ScopeInfo 实现 ==========

bool ScopeInfo::addSymbol(const SymbolInfo& symbol) {
    // 检查符号是否已存在
    if (symbols.find(symbol.name) != symbols.end()) {
        return false; // 符号已存在
    }
    
    symbols[symbol.name] = symbol;
    return true;
}

SymbolInfo* ScopeInfo::findSymbol(const std::string& name) {
    auto it = symbols.find(name);
    return (it != symbols.end()) ? &it->second : nullptr;
}

const SymbolInfo* ScopeInfo::findSymbol(const std::string& name) const {
    auto it = symbols.find(name);
    return (it != symbols.end()) ? &it->second : nullptr;
}

bool ScopeInfo::hasSymbol(const std::string& name) const {
    return symbols.find(name) != symbols.end();
}

void ScopeInfo::addConstraint(const std::string& constraint) {
    constraints.insert(constraint);
}

bool ScopeInfo::isConstrained(const std::string& item) const {
    return constraints.find(item) != constraints.end();
}

std::vector<SymbolInfo> ScopeInfo::getAllSymbols() const {
    std::vector<SymbolInfo> result;
    for (const auto& pair : symbols) {
        result.push_back(pair.second);
    }
    return result;
}

// ========== SelectorContext 实现 ==========

void SelectorContext::setClass(const std::string& className, bool isExplicit) {
    currentClass = className;
    hasExplicitClass = isExplicit;
}

void SelectorContext::setId(const std::string& idName, bool isExplicit) {
    currentId = idName;
    hasExplicitId = isExplicit;
}

void SelectorContext::addAutoClass(const std::string& className) {
    if (std::find(autoClasses.begin(), autoClasses.end(), className) == autoClasses.end()) {
        autoClasses.push_back(className);
    }
}

void SelectorContext::addAutoId(const std::string& idName) {
    if (std::find(autoIds.begin(), autoIds.end(), idName) == autoIds.end()) {
        autoIds.push_back(idName);
    }
}

std::string SelectorContext::getEffectiveClass() const {
    if (hasExplicitClass) {
        return currentClass;
    }
    
    // 如果没有显式类名，使用第一个自动类名
    if (!autoClasses.empty()) {
        return autoClasses[0];
    }
    
    return "";
}

std::string SelectorContext::getEffectiveId() const {
    if (hasExplicitId) {
        return currentId;
    }
    
    // 如果没有显式ID，使用第一个自动ID
    if (!autoIds.empty()) {
        return autoIds[0];
    }
    
    return "";
}

void SelectorContext::reset() {
    currentClass.clear();
    currentId.clear();
    autoClasses.clear();
    autoIds.clear();
    hasExplicitClass = false;
    hasExplicitId = false;
}

// ========== RAIIScopeManager 实现 ==========

RAIIScopeManager::RAIIScopeManager(CHTLContext* context, 
                                   ContextScopeType scopeType,
                                   const std::string& name,
                                   const std::string& namespace_)
    : m_context(context), m_valid(false) {
    
    if (m_context) {
        m_valid = m_context->enterScope(scopeType, name, namespace_);
    }
}

RAIIScopeManager::~RAIIScopeManager() {
    if (m_valid && m_context) {
        m_context->exitScope();
    }
}

const ScopeInfo& RAIIScopeManager::getCurrentScope() const {
    static ScopeInfo invalidScope;
    if (m_context) {
        return m_context->getCurrentScope();
    }
    return invalidScope;
}

bool RAIIScopeManager::isValid() const {
    return m_valid;
}

// ========== CHTLContext 实现 ==========

CHTLContext::CHTLContext() {
    // 初始化全局作用域
    m_scopeStack.push(ScopeInfo(ContextScopeType::GLOBAL_SCOPE, "global", "", 0));
    m_namespaceStack.push(""); // 空命名空间
}

bool CHTLContext::enterScope(ContextScopeType scopeType, 
                             const std::string& name,
                             const std::string& namespace_) {
    size_t newDepth = m_scopeStack.size();
    std::string currentNamespace = namespace_.empty() ? getCurrentNamespace() : namespace_;
    
    ScopeInfo newScope(scopeType, name, currentNamespace, newDepth);
    m_scopeStack.push(newScope);
    
    return true;
}

bool CHTLContext::exitScope() {
    if (m_scopeStack.size() <= 1) {
        reportError("Cannot exit global scope");
        return false;
    }
    
    m_scopeStack.pop();
    return true;
}

const ScopeInfo& CHTLContext::getCurrentScope() const {
    static ScopeInfo invalidScope;
    if (m_scopeStack.empty()) {
        return invalidScope;
    }
    return m_scopeStack.top();
}

size_t CHTLContext::getScopeDepth() const {
    return m_scopeStack.size();
}

std::unique_ptr<RAIIScopeManager> CHTLContext::createScopeManager(ContextScopeType scopeType,
                                                                   const std::string& name,
                                                                   const std::string& namespace_) {
    return std::make_unique<RAIIScopeManager>(this, scopeType, name, namespace_);
}

// ========== 符号管理实现 ==========

bool CHTLContext::addSymbol(const SymbolInfo& symbol) {
    if (m_scopeStack.empty()) {
        reportError("No active scope to add symbol");
        return false;
    }
    
    return const_cast<ScopeInfo&>(m_scopeStack.top()).addSymbol(symbol);
}

SymbolInfo* CHTLContext::findSymbol(const std::string& name) {
    // 从当前作用域向上查找
    std::stack<ScopeInfo> tempStack = m_scopeStack;
    while (!tempStack.empty()) {
        ScopeInfo& scope = const_cast<ScopeInfo&>(tempStack.top());
        SymbolInfo* symbol = scope.findSymbol(name);
        if (symbol) {
            return symbol;
        }
        tempStack.pop();
    }
    
    return nullptr;
}

const SymbolInfo* CHTLContext::findSymbol(const std::string& name) const {
    // 从当前作用域向上查找
    std::stack<ScopeInfo> tempStack = m_scopeStack;
    while (!tempStack.empty()) {
        const ScopeInfo& scope = tempStack.top();
        const SymbolInfo* symbol = scope.findSymbol(name);
        if (symbol) {
            return symbol;
        }
        tempStack.pop();
    }
    
    return nullptr;
}

SymbolInfo* CHTLContext::findSymbolInNamespace(const std::string& name, const std::string& namespace_) {
    std::stack<ScopeInfo> tempStack = m_scopeStack;
    while (!tempStack.empty()) {
        ScopeInfo& scope = const_cast<ScopeInfo&>(tempStack.top());
        if (scope.namespace_ == namespace_) {
            SymbolInfo* symbol = scope.findSymbol(name);
            if (symbol) {
                return symbol;
            }
        }
        tempStack.pop();
    }
    
    return nullptr;
}

bool CHTLContext::hasSymbol(const std::string& name) const {
    return findSymbol(name) != nullptr;
}

std::vector<SymbolInfo> CHTLContext::getCurrentScopeSymbols() const {
    if (m_scopeStack.empty()) {
        return {};
    }
    
    return m_scopeStack.top().getAllSymbols();
}

// ========== 命名空间管理实现 ==========

void CHTLContext::setCurrentNamespace(const std::string& namespace_) {
    if (!m_namespaceStack.empty()) {
        m_namespaceStack.pop();
    }
    m_namespaceStack.push(namespace_);
}

std::string CHTLContext::getCurrentNamespace() const {
    if (m_namespaceStack.empty()) {
        return "";
    }
    return m_namespaceStack.top();
}

void CHTLContext::enterNamespace(const std::string& namespace_) {
    std::string currentNs = getCurrentNamespace();
    std::string newNs = currentNs.empty() ? namespace_ : currentNs + "::" + namespace_;
    m_namespaceStack.push(newNs);
}

void CHTLContext::exitNamespace() {
    if (m_namespaceStack.size() > 1) {
        m_namespaceStack.pop();
    }
}

// ========== 选择器上下文管理实现 ==========

SelectorContext& CHTLContext::getSelectorContext() {
    return m_selectorContext;
}

const SelectorContext& CHTLContext::getSelectorContext() const {
    return m_selectorContext;
}

void CHTLContext::resetSelectorContext() {
    m_selectorContext.reset();
}

// ========== 约束管理实现 ==========

void CHTLContext::addConstraint(const std::string& constraint) {
    if (!m_scopeStack.empty()) {
        const_cast<ScopeInfo&>(m_scopeStack.top()).addConstraint(constraint);
    }
}

bool CHTLContext::isConstrained(const std::string& item) const {
    // 检查所有作用域的约束
    std::stack<ScopeInfo> tempStack = m_scopeStack;
    while (!tempStack.empty()) {
        const ScopeInfo& scope = tempStack.top();
        if (scope.isConstrained(item)) {
            return true;
        }
        tempStack.pop();
    }
    
    return false;
}

void CHTLContext::clearConstraints() {
    if (!m_scopeStack.empty()) {
        const_cast<ScopeInfo&>(m_scopeStack.top()).constraints.clear();
    }
}

// ========== 继承关系管理实现 ==========

void CHTLContext::addInheritance(const std::string& child, const std::string& parent) {
    m_inheritanceMap[child].push_back(parent);
}

bool CHTLContext::isInherited(const std::string& child, const std::string& parent) const {
    auto it = m_inheritanceMap.find(child);
    if (it != m_inheritanceMap.end()) {
        const auto& parents = it->second;
        return std::find(parents.begin(), parents.end(), parent) != parents.end();
    }
    return false;
}

std::vector<std::string> CHTLContext::getInheritanceChain(const std::string& symbol) const {
    std::vector<std::string> chain;
    std::unordered_set<std::string> visited; // 防止循环继承
    
    std::function<void(const std::string&)> buildChain = [&](const std::string& current) {
        if (visited.find(current) != visited.end()) {
            return; // 检测到循环，停止
        }
        
        visited.insert(current);
        chain.push_back(current);
        
        auto it = m_inheritanceMap.find(current);
        if (it != m_inheritanceMap.end()) {
            for (const auto& parent : it->second) {
                buildChain(parent);
            }
        }
    };
    
    buildChain(symbol);
    return chain;
}

// ========== 状态管理实现 ==========

void CHTLContext::reset() {
    // 清空作用域栈
    while (!m_scopeStack.empty()) {
        m_scopeStack.pop();
    }
    
    // 清空命名空间栈
    while (!m_namespaceStack.empty()) {
        m_namespaceStack.pop();
    }
    
    // 重新初始化
    m_scopeStack.push(ScopeInfo(ContextScopeType::GLOBAL_SCOPE, "global", "", 0));
    m_namespaceStack.push("");
    
    // 重置其他状态
    m_selectorContext.reset();
    m_inheritanceMap.clear();
}

void CHTLContext::setErrorCallback(std::function<void(const std::string&)> callback) {
    m_errorCallback = callback;
}

// ========== 私有方法实现 ==========

void CHTLContext::reportError(const std::string& message) {
    if (m_errorCallback) {
        m_errorCallback(message);
    }
}

} // namespace CHTL