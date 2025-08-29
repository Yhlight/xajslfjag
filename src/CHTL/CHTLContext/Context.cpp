#include "Context.h"
#include <sstream>

namespace CHTL {

CompileContext::CompileContext(const std::string& sourceFile)
    : sourceFile_(sourceFile), 
      currentLine_(1), 
      currentColumn_(1),
      inLocalStyleBlock_(false),
      inLocalScriptBlock_(false) {
    // 设置默认命名空间为文件名
    size_t lastSlash = sourceFile.find_last_of("/\\");
    size_t lastDot = sourceFile.find_last_of('.');
    
    if (lastDot != std::string::npos && (lastSlash == std::string::npos || lastDot > lastSlash)) {
        std::string filename = (lastSlash != std::string::npos) 
            ? sourceFile.substr(lastSlash + 1, lastDot - lastSlash - 1)
            : sourceFile.substr(0, lastDot);
        currentNamespace_ = filename;
    }
}

void CompileContext::enterNamespace(const std::string& ns) {
    namespaceStack_.push(currentNamespace_);
    if (currentNamespace_.empty()) {
        currentNamespace_ = ns;
    } else {
        currentNamespace_ += "." + ns;
    }
}

void CompileContext::exitNamespace() {
    if (!namespaceStack_.empty()) {
        currentNamespace_ = namespaceStack_.top();
        namespaceStack_.pop();
    }
}

std::string CompileContext::getFullQualifiedName(const std::string& name) const {
    if (currentNamespace_.empty()) {
        return name;
    }
    return currentNamespace_ + "." + name;
}

void CompileContext::setConfiguration(std::shared_ptr<ConfigurationInfo> config) {
    configuration_ = config;
}

std::string CompileContext::getConfigValue(const std::string& key, 
                                          const std::string& defaultValue) const {
    if (configuration_) {
        auto value = configuration_->getProperty(key);
        if (value.has_value()) {
            return value.value();
        }
    }
    return defaultValue;
}

bool CompileContext::isConfigEnabled(const std::string& key, bool defaultValue) const {
    std::string value = getConfigValue(key, defaultValue ? "true" : "false");
    return value == "true" || value == "1" || value == "yes" || value == "on";
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

void CompileContext::addImportedFile(const std::string& file) {
    if (importedFileSet_.find(file) == importedFileSet_.end()) {
        importedFiles_.push_back(file);
        importedFileSet_.insert(file);
    }
}

bool CompileContext::isFileImported(const std::string& file) const {
    return importedFileSet_.find(file) != importedFileSet_.end();
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

} // namespace CHTL