#include "Context.h"
#include <sstream>

namespace CHTLJS {

CompileContext::CompileContext(const std::string& sourceFile)
    : sourceFile_(sourceFile), 
      currentLine_(1), 
      currentColumn_(1) {
}

void CompileContext::enterVirtualObject(const std::string& name) {
    currentVirtualObject_ = name;
    // 注册到全局映射表
    GlobalMap::getInstance().registerVirtualObject(name, sourceFile_);
}

void CompileContext::exitVirtualObject() {
    currentVirtualObject_.clear();
}

void CompileContext::enterModule(const std::string& modulePath) {
    currentModule_ = modulePath;
    // 注册模块
    GlobalMap::getInstance().registerModule(modulePath);
}

void CompileContext::exitModule() {
    currentModule_.clear();
    moduleDependencies_.clear();
}

void CompileContext::addModuleDependency(const std::string& dependency) {
    moduleDependencies_.push_back(dependency);
    if (!currentModule_.empty()) {
        GlobalMap::getInstance().addModuleDependency(currentModule_, dependency);
    }
}

void CompileContext::pushSelectorContext(const EnhancedSelectorContext& ctx) {
    selectorStack_.push(ctx);
}

void CompileContext::popSelectorContext() {
    if (!selectorStack_.empty()) {
        selectorStack_.pop();
    }
}

CompileContext::EnhancedSelectorContext* CompileContext::getCurrentSelectorContext() {
    if (!selectorStack_.empty()) {
        return &selectorStack_.top();
    }
    return nullptr;
}

void CompileContext::enterAnimation() {
    inAnimation_ = true;
    animationContext_ = AnimationContext();
}

void CompileContext::exitAnimation() {
    inAnimation_ = false;
}

void CompileContext::enterCHTLJSFunction(const std::string& functionName) {
    chtljsFunctionStack_.push(functionName);
    currentCHTLJSFunction_ = functionName;
    
    // 注册CHTL JS函数
    GlobalMap::getInstance().registerCHTLJSFunction(functionName, "chtljs_function");
}

void CompileContext::exitCHTLJSFunction() {
    if (!chtljsFunctionStack_.empty()) {
        chtljsFunctionStack_.pop();
        currentCHTLJSFunction_ = chtljsFunctionStack_.empty() ? "" : chtljsFunctionStack_.top();
    }
}

void CompileContext::registerEventDelegation(const std::string& parentSelector) {
    GlobalMap::getInstance().registerDelegation(parentSelector);
}

void CompileContext::addDelegationTarget(const std::string& targetSelector, 
                                        const std::string& event, 
                                        const std::string& handler) {
    // 获取当前的父选择器（应该从状态或上下文中获取）
    auto selectorCtx = getCurrentSelectorContext();
    if (selectorCtx) {
        auto delegation = GlobalMap::getInstance().getDelegation(selectorCtx->selector);
        if (delegation) {
            delegation->addTarget(targetSelector, event, handler);
        }
    }
}

void CompileContext::enterScope(const std::string& scopeName) {
    scopeStack_.push(scopeName);
    // 为新作用域创建符号表
    if (localSymbols_.find(scopeName) == localSymbols_.end()) {
        localSymbols_[scopeName] = {};
    }
}

void CompileContext::exitScope() {
    if (!scopeStack_.empty()) {
        std::string scope = scopeStack_.top();
        scopeStack_.pop();
        // 清理作用域的符号表
        localSymbols_.erase(scope);
    }
}

const std::string& CompileContext::getCurrentScope() const {
    static const std::string globalScope = "global";
    if (scopeStack_.empty()) {
        return globalScope;
    }
    return scopeStack_.top();
}

void CompileContext::defineLocalSymbol(const std::string& name, const std::string& type) {
    const std::string& scope = getCurrentScope();
    localSymbols_[scope][name] = type;
}

bool CompileContext::hasLocalSymbol(const std::string& name) const {
    const std::string& scope = getCurrentScope();
    auto scopeIt = localSymbols_.find(scope);
    if (scopeIt != localSymbols_.end()) {
        return scopeIt->second.find(name) != scopeIt->second.end();
    }
    return false;
}

std::string CompileContext::getLocalSymbolType(const std::string& name) const {
    const std::string& scope = getCurrentScope();
    auto scopeIt = localSymbols_.find(scope);
    if (scopeIt != localSymbols_.end()) {
        auto symbolIt = scopeIt->second.find(name);
        if (symbolIt != scopeIt->second.end()) {
            return symbolIt->second;
        }
    }
    return "";
}

void CompileContext::addError(const std::string& message, size_t line, size_t col) {
    std::stringstream ss;
    ss << sourceFile_ << ":";
    if (line > 0) {
        ss << line << ":" << col << ": ";
    } else {
        ss << currentLine_ << ":" << currentColumn_ << ": ";
    }
    ss << "error: " << message;
    errors_.push_back(ss.str());
}

void CompileContext::addWarning(const std::string& message, size_t line, size_t col) {
    std::stringstream ss;
    ss << sourceFile_ << ":";
    if (line > 0) {
        ss << line << ":" << col << ": ";
    } else {
        ss << currentLine_ << ":" << currentColumn_ << ": ";
    }
    ss << "warning: " << message;
    warnings_.push_back(ss.str());
}

// ContextManager 实现

std::shared_ptr<CompileContext> ContextManager::createContext(const std::string& sourceFile) {
    auto context = std::make_shared<CompileContext>(sourceFile);
    contexts_[sourceFile] = context;
    return context;
}

std::shared_ptr<CompileContext> ContextManager::getCurrentContext() const {
    return currentContext_;
}

void ContextManager::setCurrentContext(std::shared_ptr<CompileContext> context) {
    currentContext_ = context;
}

std::shared_ptr<CompileContext> ContextManager::getContext(const std::string& file) const {
    auto it = contexts_.find(file);
    if (it != contexts_.end()) {
        return it->second;
    }
    return nullptr;
}

void ContextManager::removeContext(const std::string& file) {
    contexts_.erase(file);
    if (currentContext_ && currentContext_->getSourceFile() == file) {
        currentContext_ = nullptr;
    }
}

void ContextManager::clearAll() {
    contexts_.clear();
    currentContext_ = nullptr;
}

} // namespace CHTLJS