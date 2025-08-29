#include "Context.h"
#include "../../Util/StringUtils.h"
#include <iostream>
#include <algorithm>

namespace CHTL {

Context::Context() : classCounter(0), idCounter(0) {
    // 创建全局作用域
    pushScope("global", ContextType::GLOBAL);
    initializeDefaultConfig();
}

void Context::pushScope(const String& name, ContextType type) {
    size_t depth = scopes.size();
    scopes.emplace_back(name, type, depth);
}

void Context::popScope() {
    if (scopes.size() > 1) {  // 保留全局作用域
        scopes.pop_back();
    }
}

Scope* Context::getCurrentScope() {
    return scopes.empty() ? nullptr : &scopes.back();
}

const Scope* Context::getCurrentScope() const {
    return scopes.empty() ? nullptr : &scopes.back();
}

void Context::defineVariable(const String& name, const Variable& var) {
    if (Scope* scope = getCurrentScope()) {
        scope->variables[name] = var.value;
    }
}

Variable* Context::lookupVariable(const String& name) {
    return findVariableInScopes(name);
}

const Variable* Context::lookupVariable(const String& name) const {
    return findVariableInScopes(name);
}

bool Context::hasVariable(const String& name) const {
    return lookupVariable(name) != nullptr;
}

void Context::removeVariable(const String& name) {
    if (Scope* scope = getCurrentScope()) {
        scope->variables.erase(name);
    }
}

void Context::defineTemplate(const String& name, const String& type, const String& content) {
    if (Scope* scope = getCurrentScope()) {
        String key = makeKey(name, type);
        scope->templates[key] = content;
    }
}

String Context::lookupTemplate(const String& name, const String& type) const {
    return findTemplateInScopes(name, type);
}

bool Context::hasTemplate(const String& name, const String& type) const {
    return !lookupTemplate(name, type).empty();
}

void Context::removeTemplate(const String& name, const String& type) {
    if (Scope* scope = getCurrentScope()) {
        String key = makeKey(name, type);
        scope->templates.erase(key);
    }
}

StringVector Context::getTemplateNames(const String& type) const {
    StringVector names;
    for (const auto& scope : scopes) {
        for (const auto& [key, content] : scope.templates) {
            if (type.empty() || key.find(type) != String::npos) {
                // 提取名称部分（去掉类型后缀）
                size_t pos = key.find(':');
                String name = (pos != String::npos) ? key.substr(0, pos) : key;
                if (std::find(names.begin(), names.end(), name) == names.end()) {
                    names.push_back(name);
                }
            }
        }
    }
    return names;
}

void Context::defineCustom(const String& name, const String& type, const String& content) {
    if (Scope* scope = getCurrentScope()) {
        String key = makeKey(name, type);
        scope->customs[key] = content;
    }
}

String Context::lookupCustom(const String& name, const String& type) const {
    return findCustomInScopes(name, type);
}

bool Context::hasCustom(const String& name, const String& type) const {
    return !lookupCustom(name, type).empty();
}

void Context::removeCustom(const String& name, const String& type) {
    if (Scope* scope = getCurrentScope()) {
        String key = makeKey(name, type);
        scope->customs.erase(key);
    }
}

StringVector Context::getCustomNames(const String& type) const {
    StringVector names;
    for (const auto& scope : scopes) {
        for (const auto& [key, content] : scope.customs) {
            if (type.empty() || key.find(type) != String::npos) {
                size_t pos = key.find(':');
                String name = (pos != String::npos) ? key.substr(0, pos) : key;
                if (std::find(names.begin(), names.end(), name) == names.end()) {
                    names.push_back(name);
                }
            }
        }
    }
    return names;
}

void Context::defineOrigin(const String& name, const String& type, const String& content) {
    if (Scope* scope = getCurrentScope()) {
        String key = makeKey(name, type);
        scope->origins[key] = content;
    }
}

String Context::lookupOrigin(const String& name, const String& type) const {
    return findOriginInScopes(name, type);
}

bool Context::hasOrigin(const String& name, const String& type) const {
    return !lookupOrigin(name, type).empty();
}

void Context::removeOrigin(const String& name, const String& type) {
    if (Scope* scope = getCurrentScope()) {
        String key = makeKey(name, type);
        scope->origins.erase(key);
    }
}

void Context::addImport(const String& importPath) {
    if (Scope* scope = getCurrentScope()) {
        if (std::find(scope->imports.begin(), scope->imports.end(), importPath) == scope->imports.end()) {
            scope->imports.push_back(importPath);
        }
    }
}

bool Context::hasImport(const String& importPath) const {
    for (const auto& scope : scopes) {
        if (std::find(scope.imports.begin(), scope.imports.end(), importPath) != scope.imports.end()) {
            return true;
        }
    }
    return false;
}

StringVector Context::getImports() const {
    StringVector allImports;
    for (const auto& scope : scopes) {
        allImports.insert(allImports.end(), scope.imports.begin(), scope.imports.end());
    }
    return allImports;
}

void Context::clearImports() {
    if (Scope* scope = getCurrentScope()) {
        scope->imports.clear();
    }
}

void Context::enterNamespace(const String& name) {
    namespaceStack.push_back(name);
}

void Context::exitNamespace() {
    if (!namespaceStack.empty()) {
        namespaceStack.pop_back();
    }
}

String Context::getCurrentNamespace() const {
    if (namespaceStack.empty()) {
        return "";
    }
    return Util::StringUtils::join(namespaceStack, ".");
}

String Context::getFullyQualifiedName(const String& name) const {
    String ns = getCurrentNamespace();
    return ns.empty() ? name : ns + "." + name;
}

void Context::setConfigValue(const String& key, const String& value) {
    config[key] = value;
}

String Context::getConfigValue(const String& key, const String& defaultValue) const {
    auto it = config.find(key);
    return (it != config.end()) ? it->second : defaultValue;
}

bool Context::hasConfigValue(const String& key) const {
    return config.find(key) != config.end();
}

bool Context::isStyleAutoAddClassDisabled() const {
    return getConfigValue("DISABLE_STYLE_AUTO_ADD_CLASS", "false") == "true";
}

bool Context::isStyleAutoAddIdDisabled() const {
    return getConfigValue("DISABLE_STYLE_AUTO_ADD_ID", "false") == "true";
}

bool Context::isScriptAutoAddClassDisabled() const {
    return getConfigValue("DISABLE_SCRIPT_AUTO_ADD_CLASS", "true") == "true";
}

bool Context::isScriptAutoAddIdDisabled() const {
    return getConfigValue("DISABLE_SCRIPT_AUTO_ADD_ID", "true") == "true";
}

bool Context::isDefaultNamespaceDisabled() const {
    return getConfigValue("DISABLE_DEFAULT_NAMESPACE", "false") == "true";
}

ContextType Context::getCurrentContextType() const {
    const Scope* scope = getCurrentScope();
    return scope ? scope->type : ContextType::GLOBAL;
}

bool Context::isInTemplateContext() const {
    for (const auto& scope : scopes) {
        if (scope.type == ContextType::TEMPLATE) {
            return true;
        }
    }
    return false;
}

bool Context::isInCustomContext() const {
    for (const auto& scope : scopes) {
        if (scope.type == ContextType::CUSTOM) {
            return true;
        }
    }
    return false;
}

bool Context::isInStyleContext() const {
    return getCurrentContextType() == ContextType::STYLE;
}

bool Context::isInScriptContext() const {
    return getCurrentContextType() == ContextType::SCRIPT;
}

bool Context::isInNamespaceContext() const {
    return !namespaceStack.empty();
}

void Context::setInheritanceTarget(const String& target, const String& type) {
    String key = makeKey("inherit", type);
    inheritanceTargets[key] = target;
}

String Context::getInheritanceTarget(const String& type) const {
    String key = makeKey("inherit", type);
    auto it = inheritanceTargets.find(key);
    return (it != inheritanceTargets.end()) ? it->second : "";
}

bool Context::hasInheritanceTarget(const String& type) const {
    return !getInheritanceTarget(type).empty();
}

void Context::clearInheritanceTarget(const String& type) {
    String key = makeKey("inherit", type);
    inheritanceTargets.erase(key);
}

void Context::markForDeletion(const String& target, const String& type) {
    String key = makeKey(target, type);
    deletionMarks[key] = "true";
}

bool Context::isMarkedForDeletion(const String& target, const String& type) const {
    String key = makeKey(target, type);
    return deletionMarks.find(key) != deletionMarks.end();
}

StringVector Context::getDeletionTargets(const String& type) const {
    StringVector targets;
    for (const auto& [key, value] : deletionMarks) {
        if (type.empty() || key.find(type) != String::npos) {
            size_t pos = key.find(':');
            String target = (pos != String::npos) ? key.substr(0, pos) : key;
            targets.push_back(target);
        }
    }
    return targets;
}

void Context::clearDeletionMarks() {
    deletionMarks.clear();
}

void Context::addInsertionPoint(const InsertionPoint& point) {
    insertionPoints.push_back(point);
}

std::vector<Context::InsertionPoint> Context::getInsertionPoints(const String& type) const {
    std::vector<InsertionPoint> filtered;
    for (const auto& point : insertionPoints) {
        if (type.empty() || point.type == type) {
            filtered.push_back(point);
        }
    }
    return filtered;
}

void Context::clearInsertionPoints() {
    insertionPoints.clear();
}

String Context::generateClassName(const String& prefix) {
    String className = prefix + std::to_string(++classCounter);
    addGeneratedClass(className);
    return className;
}

String Context::generateIdName(const String& prefix) {
    String idName = prefix + std::to_string(++idCounter);
    addGeneratedId(idName);
    return idName;
}

void Context::addGeneratedClass(const String& className) {
    if (std::find(generatedClasses.begin(), generatedClasses.end(), className) == generatedClasses.end()) {
        generatedClasses.push_back(className);
    }
}

void Context::addGeneratedId(const String& idName) {
    if (std::find(generatedIds.begin(), generatedIds.end(), idName) == generatedIds.end()) {
        generatedIds.push_back(idName);
    }
}

void Context::reportError(const String& message) {
    errors.push_back(message);
}

void Context::reportWarning(const String& message) {
    warnings.push_back(message);
}

void Context::clearDiagnostics() {
    errors.clear();
    warnings.clear();
}

void Context::dumpContext() const {
    std::cout << "=== Context Dump ===" << std::endl;
    std::cout << "Scope Depth: " << getScopeDepth() << std::endl;
    std::cout << "Current Namespace: " << getCurrentNamespace() << std::endl;
    std::cout << "Context Type: " << static_cast<int>(getCurrentContextType()) << std::endl;
    
    for (size_t i = 0; i < scopes.size(); ++i) {
        const auto& scope = scopes[i];
        std::cout << "Scope[" << i << "]: " << scope.name 
                  << " (type=" << static_cast<int>(scope.type) 
                  << ", depth=" << scope.depth << ")" << std::endl;
        
        if (!scope.variables.empty()) {
            std::cout << "  Variables: ";
            for (const auto& [name, value] : scope.variables) {
                std::cout << name << "=" << value << " ";
            }
            std::cout << std::endl;
        }
        
        if (!scope.templates.empty()) {
            std::cout << "  Templates: " << scope.templates.size() << std::endl;
        }
        
        if (!scope.customs.empty()) {
            std::cout << "  Customs: " << scope.customs.size() << std::endl;
        }
        
        if (!scope.origins.empty()) {
            std::cout << "  Origins: " << scope.origins.size() << std::endl;
        }
        
        if (!scope.imports.empty()) {
            std::cout << "  Imports: " << Util::StringUtils::join(scope.imports, ", ") << std::endl;
        }
    }
    
    if (!config.empty()) {
        std::cout << "Config: " << config.size() << " items" << std::endl;
    }
    
    if (hasErrors()) {
        std::cout << "Errors: " << errors.size() << std::endl;
    }
    
    if (hasWarnings()) {
        std::cout << "Warnings: " << warnings.size() << std::endl;
    }
    
    std::cout << "===================" << std::endl;
}

String Context::getContextInfo() const {
    return "Context{scopes=" + std::to_string(getScopeDepth()) + 
           ", namespace=" + getCurrentNamespace() + 
           ", type=" + std::to_string(static_cast<int>(getCurrentContextType())) + 
           ", errors=" + std::to_string(errors.size()) + 
           ", warnings=" + std::to_string(warnings.size()) + "}";
}

Context* Context::clone() const {
    Context* cloned = new Context();
    cloned->scopes = this->scopes;
    cloned->namespaceStack = this->namespaceStack;
    cloned->config = this->config;
    cloned->inheritanceTargets = this->inheritanceTargets;
    cloned->deletionMarks = this->deletionMarks;
    cloned->insertionPoints = this->insertionPoints;
    cloned->generatedClasses = this->generatedClasses;
    cloned->generatedIds = this->generatedIds;
    cloned->errors = this->errors;
    cloned->warnings = this->warnings;
    cloned->classCounter = this->classCounter;
    cloned->idCounter = this->idCounter;
    return cloned;
}

void Context::merge(const Context& other) {
    // 合并配置
    for (const auto& [key, value] : other.config) {
        config[key] = value;
    }
    
    // 合并错误和警告
    errors.insert(errors.end(), other.errors.begin(), other.errors.end());
    warnings.insert(warnings.end(), other.warnings.begin(), other.warnings.end());
    
    // 合并生成的类和ID
    for (const String& cls : other.generatedClasses) {
        addGeneratedClass(cls);
    }
    for (const String& id : other.generatedIds) {
        addGeneratedId(id);
    }
}

// 私有方法实现
Variable* Context::findVariableInScopes(const String& name) {
    // 从当前作用域向上查找
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
        auto varIt = it->variables.find(name);
        if (varIt != it->variables.end()) {
            static Variable var(name, varIt->second);
            return &var;
        }
    }
    return nullptr;
}

const Variable* Context::findVariableInScopes(const String& name) const {
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
        auto varIt = it->variables.find(name);
        if (varIt != it->variables.end()) {
            static Variable var(name, varIt->second);
            return &var;
        }
    }
    return nullptr;
}

String Context::findTemplateInScopes(const String& name, const String& type) const {
    String key = makeKey(name, type);
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
        auto templateIt = it->templates.find(key);
        if (templateIt != it->templates.end()) {
            return templateIt->second;
        }
    }
    return "";
}

String Context::findCustomInScopes(const String& name, const String& type) const {
    String key = makeKey(name, type);
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
        auto customIt = it->customs.find(key);
        if (customIt != it->customs.end()) {
            return customIt->second;
        }
    }
    return "";
}

String Context::findOriginInScopes(const String& name, const String& type) const {
    String key = makeKey(name, type);
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
        auto originIt = it->origins.find(key);
        if (originIt != it->origins.end()) {
            return originIt->second;
        }
    }
    return "";
}

String Context::makeKey(const String& name, const String& type) const {
    return type.empty() ? name : name + ":" + type;
}

void Context::initializeDefaultConfig() {
    // 初始化默认配置值
    setConfigValue("INDEX_INITIAL_COUNT", "0");
    setConfigValue("DISABLE_NAME_GROUP", "true");
    setConfigValue("DISABLE_CUSTOM_ORIGIN_TYPE", "false");
    setConfigValue("DEBUG_MODE", "false");
    setConfigValue("DISABLE_STYLE_AUTO_ADD_CLASS", "false");
    setConfigValue("DISABLE_STYLE_AUTO_ADD_ID", "false");
    setConfigValue("DISABLE_SCRIPT_AUTO_ADD_CLASS", "true");
    setConfigValue("DISABLE_SCRIPT_AUTO_ADD_ID", "true");
    setConfigValue("DISABLE_DEFAULT_NAMESPACE", "false");
}

// Guard实现
ContextGuard::ContextGuard(Context& ctx, const String& scopeName, ContextType type)
    : context(ctx), shouldPop(true) {
    context.pushScope(scopeName, type);
}

ContextGuard::~ContextGuard() {
    if (shouldPop) {
        context.popScope();
    }
}

void ContextGuard::release() {
    shouldPop = false;
}

NamespaceGuard::NamespaceGuard(Context& ctx, const String& namespaceName)
    : context(ctx), shouldExit(true) {
    context.enterNamespace(namespaceName);
}

NamespaceGuard::~NamespaceGuard() {
    if (shouldExit) {
        context.exitNamespace();
    }
}

void NamespaceGuard::release() {
    shouldExit = false;
}

// Factory实现
std::unique_ptr<Context> ContextFactory::createGlobalContext() {
    return std::make_unique<Context>();
}

std::unique_ptr<Context> ContextFactory::createTemplateContext(const String& templateName) {
    auto context = createGlobalContext();
    context->pushScope(templateName, ContextType::TEMPLATE);
    return context;
}

std::unique_ptr<Context> ContextFactory::createCustomContext(const String& customName) {
    auto context = createGlobalContext();
    context->pushScope(customName, ContextType::CUSTOM);
    return context;
}

std::unique_ptr<Context> ContextFactory::createStyleContext() {
    auto context = createGlobalContext();
    context->pushScope("style", ContextType::STYLE);
    return context;
}

std::unique_ptr<Context> ContextFactory::createScriptContext() {
    auto context = createGlobalContext();
    context->pushScope("script", ContextType::SCRIPT);
    return context;
}

std::unique_ptr<Context> ContextFactory::createImportContext() {
    auto context = createGlobalContext();
    context->pushScope("import", ContextType::IMPORT);
    return context;
}

std::unique_ptr<Context> ContextFactory::createNamespaceContext(const String& namespaceName) {
    auto context = createGlobalContext();
    context->pushScope(namespaceName, ContextType::NAMESPACE);
    context->enterNamespace(namespaceName);
    return context;
}

} // namespace CHTL