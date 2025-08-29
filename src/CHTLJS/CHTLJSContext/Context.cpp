#include "Context.h"
#include <iostream>
#include <algorithm>
#include <sstream>

namespace CHTLJS {

// ScopeInfo实现
ScopeInfo::ScopeInfo(ContextType t, const String& n) : type(t), name(n) {
}

// Context实现
Context::Context() : debugMode_(false) {
    // 创建全局作用域
    pushScope(ContextType::GLOBAL, "global");
}

Context::~Context() = default;

void Context::pushScope(ContextType type, const String& name) {
    auto scope = std::make_unique<ScopeInfo>(type, name);
    scopeStack_.push(std::move(scope));
}

void Context::popScope() {
    if (scopeStack_.size() > 1) { // 保留全局作用域
        scopeStack_.pop();
    }
}

ScopeInfo* Context::getCurrentScope() {
    return scopeStack_.empty() ? nullptr : scopeStack_.top().get();
}

const ScopeInfo* Context::getCurrentScope() const {
    return scopeStack_.empty() ? nullptr : scopeStack_.top().get();
}

size_t Context::getScopeDepth() const {
    return scopeStack_.size();
}

void Context::declareVariable(const String& name, const String& type, const String& value) {
    validateVariableName(name);
    
    ScopeInfo* currentScope = getCurrentScope();
    if (currentScope) {
        currentScope->variables[name] = value;
        
        if (debugMode_) {
            addDebugInfo("Declared variable: " + name + " = " + value + " (type: " + type + ")");
        }
    }
}

bool Context::isVariableDeclared(const String& name) const {
    // 从当前作用域向上查找
    std::stack<std::unique_ptr<ScopeInfo>> tempStack = scopeStack_;
    
    while (!tempStack.empty()) {
        const auto& scope = tempStack.top();
        if (scope->variables.find(name) != scope->variables.end()) {
            return true;
        }
        tempStack.pop();
    }
    
    // 检查全局变量
    return globalVariables_.find(name) != globalVariables_.end();
}

String Context::getVariableType(const String& name) const {
    // 这里需要类型信息的存储，暂时返回空字符串
    return "";
}

String Context::getVariableValue(const String& name) const {
    return lookupVariable(name);
}

void Context::setVariableValue(const String& name, const String& value) {
    // 在最近声明的作用域中更新变量
    std::stack<std::unique_ptr<ScopeInfo>> tempStack = scopeStack_;
    
    while (!tempStack.empty()) {
        auto& scope = tempStack.top();
        if (scope->variables.find(name) != scope->variables.end()) {
            scope->variables[name] = value;
            return;
        }
        tempStack.pop();
    }
    
    // 如果没找到，在全局作用域中设置
    globalVariables_[name] = value;
}

void Context::declareFunction(const String& name, const StringVector& parameters, const String& returnType) {
    validateFunctionName(name);
    
    ScopeInfo* currentScope = getCurrentScope();
    if (currentScope) {
        // 构建函数签名
        String signature = name + "(";
        for (size_t i = 0; i < parameters.size(); ++i) {
            if (i > 0) signature += ", ";
            signature += parameters[i];
        }
        signature += ")";
        
        if (!returnType.empty()) {
            signature += " -> " + returnType;
        }
        
        currentScope->functions[name] = signature;
        
        if (debugMode_) {
            addDebugInfo("Declared function: " + signature);
        }
    }
}

bool Context::isFunctionDeclared(const String& name) const {
    return !lookupFunction(name).empty();
}

StringVector Context::getFunctionParameters(const String& name) const {
    // 这里需要参数信息的存储，暂时返回空向量
    return StringVector();
}

String Context::getFunctionReturnType(const String& name) const {
    // 这里需要返回类型信息的存储，暂时返回空字符串
    return "";
}

void Context::addModuleImport(const String& moduleName, const String& alias) {
    String importName = alias.empty() ? moduleName : alias;
    
    if (std::find(moduleImports_.begin(), moduleImports_.end(), importName) == moduleImports_.end()) {
        moduleImports_.push_back(importName);
        
        ScopeInfo* currentScope = getCurrentScope();
        if (currentScope) {
            currentScope->moduleImports.push_back(importName);
        }
        
        if (debugMode_) {
            addDebugInfo("Added module import: " + moduleName + (alias.empty() ? "" : " as " + alias));
        }
    }
}

bool Context::isModuleImported(const String& moduleName) const {
    return std::find(moduleImports_.begin(), moduleImports_.end(), moduleName) != moduleImports_.end();
}

StringVector Context::getImportedModules() const {
    return moduleImports_;
}

String Context::getModuleAlias(const String& moduleName) const {
    // 这里需要别名映射的存储，暂时返回模块名本身
    return moduleName;
}

void Context::enterCHTLJSBlock(const String& selector) {
    validateSelector(selector);
    selectorStack_.push(selector);
    
    pushScope(ContextType::BLOCK, "chtljs_block");
    
    if (debugMode_) {
        addDebugInfo("Entered CHTL JS block with selector: " + selector);
    }
}

void Context::exitCHTLJSBlock() {
    if (!selectorStack_.empty()) {
        String selector = selectorStack_.top();
        selectorStack_.pop();
        
        popScope();
        
        if (debugMode_) {
            addDebugInfo("Exited CHTL JS block with selector: " + selector);
        }
    }
}

bool Context::isInCHTLJSBlock() const {
    return !selectorStack_.empty();
}

String Context::getCurrentSelector() const {
    return selectorStack_.empty() ? "" : selectorStack_.top();
}

void Context::addSelector(const String& selector) {
    if (std::find(usedSelectors_.begin(), usedSelectors_.end(), selector) == usedSelectors_.end()) {
        usedSelectors_.push_back(selector);
    }
}

bool Context::isSelectorUsed(const String& selector) const {
    return std::find(usedSelectors_.begin(), usedSelectors_.end(), selector) != usedSelectors_.end();
}

StringVector Context::getUsedSelectors() const {
    return usedSelectors_;
}

void Context::validateSelector(const String& selector) {
    if (selector.empty()) {
        addError("Empty selector");
        return;
    }
    
    if (!isValidSelector(selector)) {
        addError("Invalid selector: " + selector);
    }
}

void Context::addEventListener(const String& event, const String& handler) {
    if (!isValidEventName(event)) {
        addError("Invalid event name: " + event);
        return;
    }
    
    eventHandlers_[event] = handler;
    
    if (debugMode_) {
        addDebugInfo("Added event listener: " + event + " -> " + handler);
    }
}

bool Context::isEventHandled(const String& event) const {
    return eventHandlers_.find(event) != eventHandlers_.end();
}

StringVector Context::getHandledEvents() const {
    StringVector events;
    for (const auto& [event, handler] : eventHandlers_) {
        events.push_back(event);
    }
    return events;
}

String Context::getEventHandler(const String& event) const {
    auto it = eventHandlers_.find(event);
    return it != eventHandlers_.end() ? it->second : "";
}

void Context::addAnimation(const String& name, const StringUnorderedMap& properties) {
    animations_[name] = properties;
    
    if (debugMode_) {
        addDebugInfo("Added animation: " + name);
    }
}

bool Context::isAnimationDefined(const String& name) const {
    return animations_.find(name) != animations_.end();
}

StringUnorderedMap Context::getAnimationProperties(const String& name) const {
    auto it = animations_.find(name);
    return it != animations_.end() ? it->second : StringUnorderedMap();
}

void Context::declareVirtualObject(const String& name, const StringUnorderedMap& properties) {
    virtualObjects_[name] = properties;
    
    if (debugMode_) {
        addDebugInfo("Declared virtual object: " + name);
    }
}

bool Context::isVirtualObjectDeclared(const String& name) const {
    return virtualObjects_.find(name) != virtualObjects_.end();
}

StringUnorderedMap Context::getVirtualObjectProperties(const String& name) const {
    auto it = virtualObjects_.find(name);
    return it != virtualObjects_.end() ? it->second : StringUnorderedMap();
}

void Context::registerCJMODExtension(const String& keyword, const String& handler) {
    cjmodExtensions_[keyword] = handler;
    
    if (debugMode_) {
        addDebugInfo("Registered CJMOD extension: " + keyword + " -> " + handler);
    }
}

bool Context::isCJMODKeywordRegistered(const String& keyword) const {
    return cjmodExtensions_.find(keyword) != cjmodExtensions_.end();
}

String Context::getCJMODHandler(const String& keyword) const {
    auto it = cjmodExtensions_.find(keyword);
    return it != cjmodExtensions_.end() ? it->second : "";
}

StringVector Context::getRegisteredCJMODKeywords() const {
    StringVector keywords;
    for (const auto& [keyword, handler] : cjmodExtensions_) {
        keywords.push_back(keyword);
    }
    return keywords;
}

void Context::addError(const String& message, const Position& pos) {
    String errorMsg = message;
    if (pos.line > 0) {
        errorMsg += " at line " + std::to_string(pos.line) + ", column " + std::to_string(pos.column);
    }
    
    errors_.push_back(errorMsg);
}

void Context::addWarning(const String& message, const Position& pos) {
    String warningMsg = message;
    if (pos.line > 0) {
        warningMsg += " at line " + std::to_string(pos.line) + ", column " + std::to_string(pos.column);
    }
    
    warnings_.push_back(warningMsg);
}

bool Context::hasErrors() const {
    return !errors_.empty();
}

bool Context::hasWarnings() const {
    return !warnings_.empty();
}

StringVector Context::getErrors() const {
    return errors_;
}

StringVector Context::getWarnings() const {
    return warnings_;
}

void Context::clearErrors() {
    errors_.clear();
}

void Context::clearWarnings() {
    warnings_.clear();
}

void Context::addGeneratedCode(const String& code) {
    generatedCode_ += code;
}

String Context::getGeneratedCode() const {
    return generatedCode_;
}

void Context::clearGeneratedCode() {
    generatedCode_.clear();
}

void Context::addDependency(const String& dependency) {
    if (std::find(dependencies_.begin(), dependencies_.end(), dependency) == dependencies_.end()) {
        dependencies_.push_back(dependency);
    }
}

StringVector Context::getDependencies() const {
    return dependencies_;
}

bool Context::hasDependency(const String& dependency) const {
    return std::find(dependencies_.begin(), dependencies_.end(), dependency) != dependencies_.end();
}

void Context::resolveDependencies() {
    // 依赖解析逻辑
    if (debugMode_) {
        addDebugInfo("Resolved " + std::to_string(dependencies_.size()) + " dependencies");
    }
}

void Context::addOptimizationHint(const String& hint) {
    optimizationHints_.push_back(hint);
}

StringVector Context::getOptimizationHints() const {
    return optimizationHints_;
}

void Context::setDebugMode(bool enabled) {
    debugMode_ = enabled;
}

bool Context::isDebugMode() const {
    return debugMode_;
}

void Context::addDebugInfo(const String& info) {
    if (debugMode_) {
        debugInfo_.push_back(info);
    }
}

StringVector Context::getDebugInfo() const {
    return debugInfo_;
}

void Context::save() {
    // 保存当前状态的逻辑
    // 这里需要深拷贝所有状态信息
}

void Context::restore() {
    // 恢复保存状态的逻辑
}

void Context::reset() {
    // 清空所有状态，重新初始化
    while (scopeStack_.size() > 1) {
        scopeStack_.pop();
    }
    
    globalVariables_.clear();
    globalFunctions_.clear();
    moduleImports_.clear();
    usedSelectors_.clear();
    eventHandlers_.clear();
    animations_.clear();
    virtualObjects_.clear();
    cjmodExtensions_.clear();
    errors_.clear();
    warnings_.clear();
    generatedCode_.clear();
    dependencies_.clear();
    optimizationHints_.clear();
    debugInfo_.clear();
    
    // 清空选择器栈
    while (!selectorStack_.empty()) {
        selectorStack_.pop();
    }
}

Context::Statistics Context::getStatistics() const {
    Statistics stats;
    
    // 统计变量
    stats.totalVariables = globalVariables_.size();
    std::stack<std::unique_ptr<ScopeInfo>> tempStack = scopeStack_;
    while (!tempStack.empty()) {
        stats.totalVariables += tempStack.top()->variables.size();
        tempStack.pop();
    }
    
    // 统计函数
    stats.totalFunctions = globalFunctions_.size();
    tempStack = scopeStack_;
    while (!tempStack.empty()) {
        stats.totalFunctions += tempStack.top()->functions.size();
        tempStack.pop();
    }
    
    stats.totalModules = moduleImports_.size();
    stats.totalSelectors = usedSelectors_.size();
    stats.totalEvents = eventHandlers_.size();
    stats.totalAnimations = animations_.size();
    stats.totalVirtualObjects = virtualObjects_.size();
    stats.totalCJMODExtensions = cjmodExtensions_.size();
    stats.totalErrors = errors_.size();
    stats.totalWarnings = warnings_.size();
    
    return stats;
}

String Context::getContextReport() const {
    std::ostringstream oss;
    
    Statistics stats = getStatistics();
    
    oss << "CHTL JS Context Report\n";
    oss << "======================\n";
    oss << "Scope Depth: " << getScopeDepth() << "\n";
    oss << "Variables: " << stats.totalVariables << "\n";
    oss << "Functions: " << stats.totalFunctions << "\n";
    oss << "Modules: " << stats.totalModules << "\n";
    oss << "Selectors: " << stats.totalSelectors << "\n";
    oss << "Events: " << stats.totalEvents << "\n";
    oss << "Animations: " << stats.totalAnimations << "\n";
    oss << "Virtual Objects: " << stats.totalVirtualObjects << "\n";
    oss << "CJMOD Extensions: " << stats.totalCJMODExtensions << "\n";
    oss << "Errors: " << stats.totalErrors << "\n";
    oss << "Warnings: " << stats.totalWarnings << "\n";
    
    if (hasErrors()) {
        oss << "\nErrors:\n";
        for (const auto& error : errors_) {
            oss << "  - " << error << "\n";
        }
    }
    
    if (hasWarnings()) {
        oss << "\nWarnings:\n";
        for (const auto& warning : warnings_) {
            oss << "  - " << warning << "\n";
        }
    }
    
    return oss.str();
}

// 辅助方法实现
String Context::lookupVariable(const String& name) const {
    // 从当前作用域向上查找
    std::stack<std::unique_ptr<ScopeInfo>> tempStack = scopeStack_;
    
    while (!tempStack.empty()) {
        const auto& scope = tempStack.top();
        auto it = scope->variables.find(name);
        if (it != scope->variables.end()) {
            return it->second;
        }
        tempStack.pop();
    }
    
    // 检查全局变量
    auto it = globalVariables_.find(name);
    return it != globalVariables_.end() ? it->second : "";
}

String Context::lookupFunction(const String& name) const {
    // 从当前作用域向上查找
    std::stack<std::unique_ptr<ScopeInfo>> tempStack = scopeStack_;
    
    while (!tempStack.empty()) {
        const auto& scope = tempStack.top();
        auto it = scope->functions.find(name);
        if (it != scope->functions.end()) {
            return it->second;
        }
        tempStack.pop();
    }
    
    // 检查全局函数
    auto it = globalFunctions_.find(name);
    return it != globalFunctions_.end() ? it->second : "";
}

bool Context::isValidSelector(const String& selector) const {
    if (selector.empty()) {
        return false;
    }
    
    // 基本的选择器验证
    return selector[0] == '.' || selector[0] == '#' || 
           selector.find("{{") != String::npos ||
           std::isalpha(selector[0]);
}

bool Context::isValidEventName(const String& event) const {
    if (event.empty()) {
        return false;
    }
    
    // 基本的事件名验证
    StringVector validEvents = {
        "click", "mousedown", "mouseup", "mouseover", "mouseout",
        "keydown", "keyup", "keypress", "load", "unload",
        "focus", "blur", "change", "submit", "resize",
        "scroll", "hover", "touchstart", "touchend", "touchmove"
    };
    
    return std::find(validEvents.begin(), validEvents.end(), event) != validEvents.end();
}

void Context::validateVariableName(const String& name) const {
    if (name.empty()) {
        const_cast<Context*>(this)->addError("Empty variable name");
        return;
    }
    
    if (!std::isalpha(name[0]) && name[0] != '_') {
        const_cast<Context*>(this)->addError("Invalid variable name: " + name);
    }
}

void Context::validateFunctionName(const String& name) const {
    if (name.empty()) {
        const_cast<Context*>(this)->addError("Empty function name");
        return;
    }
    
    if (!std::isalpha(name[0]) && name[0] != '_') {
        const_cast<Context*>(this)->addError("Invalid function name: " + name);
    }
}

// Statistics实现
Context::Statistics::Statistics()
    : totalVariables(0), totalFunctions(0), totalModules(0),
      totalSelectors(0), totalEvents(0), totalAnimations(0),
      totalVirtualObjects(0), totalCJMODExtensions(0),
      totalErrors(0), totalWarnings(0) {
}

// ContextManager实现
size_t ContextManager::nextContextId_ = 1;

ContextManager& ContextManager::getInstance() {
    static ContextManager instance;
    return instance;
}

String ContextManager::createContext(const String& name) {
    String contextId = name.empty() ? generateContextId() : name;
    
    auto context = std::make_unique<Context>();
    contexts_[contextId] = std::move(context);
    
    if (currentContextId_.empty()) {
        currentContextId_ = contextId;
    }
    
    return contextId;
}

void ContextManager::destroyContext(const String& contextId) {
    contexts_.erase(contextId);
    
    if (currentContextId_ == contextId) {
        currentContextId_ = contexts_.empty() ? "" : contexts_.begin()->first;
    }
}

Context* ContextManager::getContext(const String& contextId) {
    auto it = contexts_.find(contextId);
    return it != contexts_.end() ? it->second.get() : nullptr;
}

Context* ContextManager::getCurrentContext() {
    return getContext(currentContextId_);
}

void ContextManager::setCurrentContext(const String& contextId) {
    if (hasContext(contextId)) {
        currentContextId_ = contextId;
    }
}

void ContextManager::pushContext(const String& contextId) {
    if (hasContext(contextId)) {
        contextStack_.push(currentContextId_);
        currentContextId_ = contextId;
    }
}

void ContextManager::popContext() {
    if (!contextStack_.empty()) {
        currentContextId_ = contextStack_.top();
        contextStack_.pop();
    }
}

StringVector ContextManager::getAllContextIds() const {
    StringVector ids;
    for (const auto& [id, context] : contexts_) {
        ids.push_back(id);
    }
    return ids;
}

size_t ContextManager::getContextCount() const {
    return contexts_.size();
}

bool ContextManager::hasContext(const String& contextId) const {
    return contexts_.find(contextId) != contexts_.end();
}

String ContextManager::generateContextId() {
    return "context_" + std::to_string(nextContextId_++);
}

// ContextGuard实现
ContextGuard::ContextGuard(ContextType type, const String& name)
    : context_(nullptr), shouldPopScope_(true), shouldPopContext_(false) {
    
    context_ = ContextManager::getInstance().getCurrentContext();
    if (context_) {
        context_->pushScope(type, name);
    }
}

ContextGuard::ContextGuard(const String& contextId)
    : context_(nullptr), shouldPopScope_(false), shouldPopContext_(true), contextId_(contextId) {
    
    ContextManager& manager = ContextManager::getInstance();
    manager.pushContext(contextId);
    context_ = manager.getCurrentContext();
}

ContextGuard::~ContextGuard() {
    if (context_) {
        if (shouldPopScope_) {
            context_->popScope();
        }
        
        if (shouldPopContext_) {
            ContextManager::getInstance().popContext();
        }
    }
}

Context* ContextGuard::getContext() {
    return context_;
}

} // namespace CHTLJS