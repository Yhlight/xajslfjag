#include "CHTLJSContext.h"
#include <algorithm>
#include <sstream>

namespace CHTLJS {

// Scope 实现
bool Scope::addSymbol(const Symbol& symbol) {
    if (hasSymbol(symbol.name)) {
        return false;
    }
    symbols[symbol.name] = symbol;
    return true;
}

Symbol* Scope::findSymbol(const std::string& name) {
    auto it = symbols.find(name);
    return it != symbols.end() ? &it->second : nullptr;
}

Symbol* Scope::findSymbolRecursive(const std::string& name) {
    Symbol* symbol = findSymbol(name);
    if (symbol) {
        return symbol;
    }
    
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
ScopeGuard::ScopeGuard(CHTLJSContext* ctx, ScopeType type, const std::string& name)
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

// CHTLJSContext 实现
CHTLJSContext::CHTLJSContext() {
    state = std::make_unique<CHTLJSState>();
    globalScope = std::make_shared<Scope>(ScopeType::GLOBAL, "global");
    currentScope = globalScope;
    scopeStack.push(globalScope);
}

void CHTLJSContext::enterScope(ScopeType type, const std::string& name) {
    auto newScope = std::make_shared<Scope>(type, name, currentScope);
    currentScope->addChild(newScope);
    currentScope = newScope;
    scopeStack.push(newScope);
}

void CHTLJSContext::exitScope() {
    if (scopeStack.size() > 1) {
        scopeStack.pop();
        currentScope = scopeStack.top();
    }
}

ScopeGuard CHTLJSContext::createScope(ScopeType type, const std::string& name) {
    return ScopeGuard(this, type, name);
}

bool CHTLJSContext::registerSymbol(const std::string& name, SymbolType type, 
                                  std::shared_ptr<BaseNode> node, bool isConst) {
    Symbol symbol(name, type, node);
    symbol.scope = currentScope->getType();
    symbol.isConst = isConst;
    
    // 如果是虚对象，标记
    if (type == SymbolType::VIRTUAL_OBJECT) {
        symbol.isVirtual = true;
    }
    
    return currentScope->addSymbol(symbol);
}

Symbol* CHTLJSContext::lookupSymbol(const std::string& name) {
    return currentScope->findSymbolRecursive(name);
}

bool CHTLJSContext::registerVirtualObject(const std::string& name, 
                                          const std::vector<std::string>& methods) {
    // 注册虚对象到符号表
    if (!registerSymbol(name, SymbolType::VIRTUAL_OBJECT, nullptr)) {
        return false;
    }
    
    // 记录虚对象的方法
    virtualObjectMethods[name] = methods;
    return true;
}

bool CHTLJSContext::isVirtualObject(const std::string& name) const {
    return virtualObjectMethods.find(name) != virtualObjectMethods.end();
}

std::string CHTLJSContext::registerSelector(const std::string& selector) {
    // 检查是否已经缓存
    auto it = selectorCache.find(selector);
    if (it != selectorCache.end()) {
        return it->second;
    }
    
    // 生成唯一的变量名
    static int selectorCounter = 0;
    std::string varName = "_sel" + std::to_string(++selectorCounter);
    
    // 缓存选择器
    selectorCache[selector] = varName;
    
    // 注册到符号表
    registerSymbol(varName, SymbolType::SELECTOR, nullptr, true);
    
    return varName;
}

bool CHTLJSContext::hasSelector(const std::string& selector) const {
    return selectorCache.find(selector) != selectorCache.end();
}

std::string CHTLJSContext::getSelectorVariable(const std::string& selector) const {
    auto it = selectorCache.find(selector);
    return it != selectorCache.end() ? it->second : "";
}

void CHTLJSContext::registerCHTLJSFunction(const std::string& name) {
    registeredCHTLJSFunctions.insert(name);
    registerSymbol(name, SymbolType::CHTLJS_FUNCTION, nullptr);
}

bool CHTLJSContext::isCHTLJSFunction(const std::string& name) const {
    return registeredCHTLJSFunctions.find(name) != registeredCHTLJSFunctions.end();
}

void CHTLJSContext::bindVirtualObject(const std::string& functionName) {
    // 将函数标记为虚对象绑定
    (void)functionName;  // 暂时未使用
    state->markAsVirtualContext();
}

void CHTLJSContext::registerAnimation(const std::string& name, std::shared_ptr<BaseNode> animNode) {
    animations[name] = animNode;
    registerSymbol(name, SymbolType::ANIMATION, animNode);
}

std::shared_ptr<BaseNode> CHTLJSContext::getAnimation(const std::string& name) const {
    auto it = animations.find(name);
    return it != animations.end() ? it->second : nullptr;
}

void CHTLJSContext::addDelegation(const DelegationInfo& info) {
    delegations.push_back(info);
}

bool CHTLJSContext::validateContext() const {
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

bool CHTLJSContext::validateArrowUsage(const std::string& object) const {
    // 检查是否可以使用箭头操作符
    if (!state->canUseArrowOperator()) {
        return false;
    }
    
    // 检查对象是否存在
    Symbol* symbol = const_cast<CHTLJSContext*>(this)->lookupSymbol(object);
    if (!symbol) {
        return false;
    }
    
    // 检查是否是合法的对象类型
    return symbol->type == SymbolType::SELECTOR || 
           symbol->type == SymbolType::VIRTUAL_OBJECT ||
           symbol->type == SymbolType::VARIABLE;
}

bool CHTLJSContext::validateSelectorUsage(const std::string& selector) const {
    // 验证选择器语法
    if (selector.empty()) {
        return false;
    }
    
    // 基本的CSS选择器验证
    // TODO: 实现更完整的CSS选择器验证
    
    return true;
}

bool CHTLJSContext::validateCHTLJSFunctionCall(const std::string& function) const {
    // 检查是否可以使用CHTL JS特性
    if (!state->canUseCHTLJSFeature()) {
        return false;
    }
    
    // 检查函数是否已注册
    return isCHTLJSFunction(function);
}

std::string CHTLJSContext::generateSelectorCode(const std::string& selector) const {
    // 生成选择器代码
    std::string varName = getSelectorVariable(selector);
    if (varName.empty()) {
        // 如果没有缓存，生成新的
        varName = const_cast<CHTLJSContext*>(this)->registerSelector(selector);
    }
    
    return "const " + varName + " = document.querySelectorAll('" + selector + "');";
}

std::string CHTLJSContext::generateArrowAccessCode(const std::string& object, 
                                                   const std::string& property) const {
    // 生成箭头访问代码
    Symbol* symbol = const_cast<CHTLJSContext*>(this)->lookupSymbol(object);
    if (!symbol) {
        return "";  // 错误情况
    }
    
    if (symbol->type == SymbolType::SELECTOR) {
        // 对于选择器，生成forEach调用
        return object + ".forEach(el => el." + property + ")";
    } else {
        // 对于普通对象，生成直接访问
        return object + "." + property;
    }
}

std::string CHTLJSContext::generateEventBindingCode(const std::string& selector, 
                                                   const std::string& event, 
                                                   const std::string& handler) const {
    // 生成事件绑定代码
    std::string varName = getSelectorVariable(selector);
    if (varName.empty()) {
        varName = const_cast<CHTLJSContext*>(this)->registerSelector(selector);
    }
    
    std::stringstream ss;
    ss << varName << ".forEach(el => {\n";
    ss << "  el.addEventListener('" << event << "', " << handler << ");\n";
    ss << "});";
    
    return ss.str();
}

void CHTLJSContext::addError(const std::string& error) {
    errors.push_back(error);
}

void CHTLJSContext::addWarning(const std::string& warning) {
    warnings.push_back(warning);
}

void CHTLJSContext::reset() {
    // 重置状态机
    state->reset();
    
    // 清空作用域栈
    while (!scopeStack.empty()) {
        scopeStack.pop();
    }
    
    // 重新初始化
    globalScope = std::make_shared<Scope>(ScopeType::GLOBAL, "global");
    currentScope = globalScope;
    scopeStack.push(globalScope);
    
    // 清空其他状态
    selectorCache.clear();
    virtualObjectMethods.clear();
    registeredCHTLJSFunctions.clear();
    animations.clear();
    delegations.clear();
    errors.clear();
    warnings.clear();
}

} // namespace CHTLJS