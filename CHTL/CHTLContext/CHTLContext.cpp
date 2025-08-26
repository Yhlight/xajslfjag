#include "CHTLContext.h"
#include <iostream>
#include <filesystem>
#include <algorithm>
#include <sstream>

namespace CHTL {

CHTLContext::CHTLContext(std::shared_ptr<CHTLState> state, std::shared_ptr<CHTLManager> manager)
    : state(state), manager(manager), configLoaded(false),
      disableStyleAutoAddClass(false), disableStyleAutoAddId(false),
      disableScriptAutoAddClass(true), disableScriptAutoAddId(true),
      disableDefaultNamespace(false) {
    loadConfiguration();
}

// 文件管理
void CHTLContext::setCurrentFile(const std::string& filePath) {
    currentFilePath = filePath;
    
    // 提取文件名
    namespace fs = std::filesystem;
    fs::path path(filePath);
    currentFileName = path.stem().string();
    
    // 通过manager设置当前文件
    if (manager) {
        manager->setCurrentFile(filePath);
        
        // 获取或创建默认命名空间
        auto namespaceManager = manager->getNamespaceManager();
        if (namespaceManager && !disableDefaultNamespace) {
            std::string defaultNs = namespaceManager->getFileDefaultNamespace(filePath);
            if (defaultNs.empty()) {
                defaultNs = currentFileName;
                namespaceManager->setFileDefaultNamespace(filePath, defaultNs);
            }
            currentNamespace = defaultNs;
        }
    }
}

// 命名空间管理
void CHTLContext::enterNamespace(const std::string& name) {
    if (manager) {
        manager->enterNamespace(name);
        currentNamespace = manager->getCurrentNamespace();
    }
}

void CHTLContext::exitNamespace() {
    if (manager) {
        manager->exitNamespace();
        currentNamespace = manager->getCurrentNamespace();
    }
}

std::string CHTLContext::getCurrentNamespace() const {
    if (manager) {
        return manager->getCurrentNamespace();
    }
    return currentNamespace;
}

std::string CHTLContext::getFullQualifiedName(const std::string& name) const {
    if (manager && manager->getNamespaceManager()) {
        return manager->getNamespaceManager()->getFullyQualifiedName(name);
    }
    
    if (currentNamespace.empty()) {
        return name;
    }
    return currentNamespace + "." + name;
}

// 元素管理
void CHTLContext::pushElement(const std::string& elementName) {
    ElementContext newElement;
    newElement.elementName = elementName;
    newElement.hasLocalStyle = false;
    newElement.hasLocalScript = false;
    elementStack.push(newElement);
}

void CHTLContext::popElement() {
    if (!elementStack.empty()) {
        // 在弹出元素前应用自动选择器
        applyAutoSelectors();
        elementStack.pop();
    }
}

ElementContext& CHTLContext::getCurrentElement() {
    if (elementStack.empty()) {
        throw std::runtime_error("No current element");
    }
    return elementStack.top();
}

const ElementContext& CHTLContext::getCurrentElement() const {
    if (elementStack.empty()) {
        throw std::runtime_error("No current element");
    }
    return elementStack.top();
}

bool CHTLContext::hasCurrentElement() const {
    return !elementStack.empty();
}

// 属性管理
void CHTLContext::addAttribute(const std::string& name, const std::string& value) {
    if (!elementStack.empty()) {
        elementStack.top().attributes[name] = value;
        
        // 特殊处理class和id属性
        if (name == "class") {
            addClass(value);
        } else if (name == "id") {
            setId(value);
        }
    }
}

void CHTLContext::addClass(const std::string& className) {
    if (!elementStack.empty()) {
        // 支持多个类名（空格分隔）
        std::istringstream iss(className);
        std::string cls;
        while (iss >> cls) {
            elementStack.top().elementClasses.push_back(cls);
        }
    }
}

void CHTLContext::setId(const std::string& id) {
    if (!elementStack.empty()) {
        elementStack.top().elementId = id;
    }
}

// 选择器管理
void CHTLContext::addStyleSelector(const SelectorInfo& selector) {
    if (!elementStack.empty()) {
        elementStack.top().styleSelectors.push_back(selector);
        elementStack.top().hasLocalStyle = true;
    }
}

void CHTLContext::addScriptSelector(const SelectorInfo& selector) {
    if (!elementStack.empty()) {
        elementStack.top().scriptSelectors.push_back(selector);
        elementStack.top().hasLocalScript = true;
    }
}

std::vector<SelectorInfo> CHTLContext::getStyleSelectors() const {
    if (!elementStack.empty()) {
        return elementStack.top().styleSelectors;
    }
    return {};
}

std::vector<SelectorInfo> CHTLContext::getScriptSelectors() const {
    if (!elementStack.empty()) {
        return elementStack.top().scriptSelectors;
    }
    return {};
}

// 自动选择器
void CHTLContext::applyAutoSelectors() {
    if (elementStack.empty()) return;
    
    auto& element = elementStack.top();
    
    // 处理样式选择器自动添加
    if (element.hasLocalStyle) {
        // 自动添加类选择器
        if (!disableStyleAutoAddClass && element.elementClasses.empty()) {
            std::string firstClass = getFirstClassSelector();
            if (!firstClass.empty()) {
                element.elementClasses.push_back(firstClass);
                // 标记为自动添加
                for (auto& sel : element.styleSelectors) {
                    if (sel.type == SelectorType::CLASS_SELECTOR && sel.value == firstClass) {
                        sel.isAutoAdded = true;
                        break;
                    }
                }
            }
        }
        
        // 自动添加ID选择器
        if (!disableStyleAutoAddId && element.elementId.empty()) {
            std::string firstId = getFirstIdSelector();
            if (!firstId.empty()) {
                element.elementId = firstId;
                // 标记为自动添加
                for (auto& sel : element.styleSelectors) {
                    if (sel.type == SelectorType::ID_SELECTOR && sel.value == firstId) {
                        sel.isAutoAdded = true;
                        break;
                    }
                }
            }
        }
    }
    
    // 处理脚本选择器自动添加
    if (element.hasLocalScript) {
        // 自动添加类选择器（仅当样式没有触发且脚本启用）
        if (!disableScriptAutoAddClass && element.elementClasses.empty() && !element.hasLocalStyle) {
            for (const auto& sel : element.scriptSelectors) {
                if (sel.type == SelectorType::CLASS_SELECTOR) {
                    element.elementClasses.push_back(sel.value);
                    break;
                }
            }
        }
        
        // 自动添加ID选择器（仅当样式没有触发且脚本启用）
        if (!disableScriptAutoAddId && element.elementId.empty() && !element.hasLocalStyle) {
            for (const auto& sel : element.scriptSelectors) {
                if (sel.type == SelectorType::ID_SELECTOR) {
                    element.elementId = sel.value;
                    break;
                }
            }
        }
    }
}

bool CHTLContext::shouldAutoAddClass() const {
    if (state->isInStyleContext()) {
        return !disableStyleAutoAddClass;
    } else if (state->isInScriptContext()) {
        return !disableScriptAutoAddClass;
    }
    return false;
}

bool CHTLContext::shouldAutoAddId() const {
    if (state->isInStyleContext()) {
        return !disableStyleAutoAddId;
    } else if (state->isInScriptContext()) {
        return !disableScriptAutoAddId;
    }
    return false;
}

std::string CHTLContext::getFirstClassSelector() const {
    if (!elementStack.empty()) {
        for (const auto& sel : elementStack.top().styleSelectors) {
            if (sel.type == SelectorType::CLASS_SELECTOR) {
                return sel.value;
            }
        }
    }
    return "";
}

std::string CHTLContext::getFirstIdSelector() const {
    if (!elementStack.empty()) {
        for (const auto& sel : elementStack.top().styleSelectors) {
            if (sel.type == SelectorType::ID_SELECTOR) {
                return sel.value;
            }
        }
    }
    return "";
}

// 引用解析
std::string CHTLContext::resolveAmpersandReference() const {
    if (!hasCurrentElement()) {
        return "";
    }
    
    const auto& element = getCurrentElement();
    
    // 在样式上下文中，&优先引用class
    if (state->isInStyleContext()) {
        if (!element.elementClasses.empty()) {
            return "." + element.elementClasses[0];
        } else if (!element.elementId.empty()) {
            return "#" + element.elementId;
        }
    }
    // 在脚本上下文中，&优先引用id
    else if (state->isInScriptContext()) {
        if (!element.elementId.empty()) {
            return "#" + element.elementId;
        } else if (!element.elementClasses.empty()) {
            return "." + element.elementClasses[0];
        }
    }
    
    return "";
}

bool CHTLContext::hasClassAttribute() const {
    return hasCurrentElement() && !getCurrentElement().elementClasses.empty();
}

bool CHTLContext::hasIdAttribute() const {
    return hasCurrentElement() && !getCurrentElement().elementId.empty();
}

// 变量管理
void CHTLContext::addVariableReference(const VariableReference& varRef) {
    // TODO: 实现变量引用管理
}

bool CHTLContext::resolveVariable(const std::string& name, std::string& value) const {
    // 首先在局部符号中查找
    auto localSymbol = findLocalSymbol(name);
    if (localSymbol && localSymbol->type == SymbolType::VAR_TEMPLATE) {
        // TODO: 从符号中获取值
        return true;
    }
    
    // 然后通过manager在全局符号表中查找
    if (manager) {
        auto globalSymbol = manager->findSymbol(name, currentNamespace);
        if (globalSymbol && globalSymbol->type == SymbolType::VAR_TEMPLATE) {
            // TODO: 从符号中获取值
            return true;
        }
    }
    
    return false;
}

// 继承管理
void CHTLContext::addInheritance(const InheritanceInfo& info) {
    inheritanceChain.push_back(info);
}

void CHTLContext::removeInheritance(const std::string& baseName) {
    inheritanceChain.erase(
        std::remove_if(inheritanceChain.begin(), inheritanceChain.end(),
            [&baseName](const InheritanceInfo& info) {
                return info.baseName == baseName;
            }),
        inheritanceChain.end()
    );
}

std::vector<InheritanceInfo> CHTLContext::getInheritanceChain() const {
    return inheritanceChain;
}

bool CHTLContext::hasInheritance(const std::string& baseName) const {
    return std::any_of(inheritanceChain.begin(), inheritanceChain.end(),
        [&baseName](const InheritanceInfo& info) {
            return info.baseName == baseName;
        });
}

// 符号管理
void CHTLContext::addLocalSymbol(std::shared_ptr<SymbolInfo> symbol) {
    if (symbol) {
        localSymbols[symbol->name] = symbol;
        
        // 同时注册到manager
        if (manager) {
            manager->registerSymbol(symbol);
        }
    }
}

std::shared_ptr<SymbolInfo> CHTLContext::findLocalSymbol(const std::string& name) const {
    auto it = localSymbols.find(name);
    if (it != localSymbols.end()) {
        return it->second;
    }
    return nullptr;
}

void CHTLContext::clearLocalSymbols() {
    localSymbols.clear();
}

// 配置访问
void CHTLContext::loadConfiguration() {
    if (!manager || !manager->getConfigManager()) {
        return;
    }
    
    auto configManager = manager->getConfigManager();
    
    if (configManager->hasActiveConfiguration()) {
        configLoaded = true;
        
        // 解析选择器自动化配置
        disableStyleAutoAddClass = configManager->isStyleAutoAddClassDisabled();
        disableStyleAutoAddId = configManager->isStyleAutoAddIdDisabled();
        disableScriptAutoAddClass = configManager->isScriptAutoAddClassDisabled();
        disableScriptAutoAddId = configManager->isScriptAutoAddIdDisabled();
        disableDefaultNamespace = configManager->isDefaultNamespaceDisabled();
    }
}

std::string CHTLContext::getConfigOption(const std::string& key) const {
    if (manager && manager->getConfigManager()) {
        return manager->getConfigManager()->getStringOption(key);
    }
    return "";
}

std::vector<std::string> CHTLContext::getNameGroupOption(const std::string& key) const {
    if (manager && manager->getConfigManager()) {
        return manager->getConfigManager()->getNameGroupOption(key);
    }
    return {};
}

// 约束检查（委托给state）
bool CHTLContext::isAllowedInCurrentContext(const std::string& element) const {
    return state->isAllowedInCurrentContext(element);
}

bool CHTLContext::canUseTemplateVariable() const {
    return state->canUseTemplateVariable();
}

bool CHTLContext::canUseCustomVariable() const {
    return state->canUseCustomVariable();
}

bool CHTLContext::canUseStyleGroup() const {
    return state->canUseStyleGroup();
}

bool CHTLContext::canUseFrom() const {
    return state->canUseFrom();
}

// 状态查询（委托给state）
bool CHTLContext::isInGlobalStyle() const {
    return state->isInStyleContext() && state->isInGlobalScope();
}

bool CHTLContext::isInLocalStyle() const {
    return state->isInStyleContext() && state->isInLocalScope();
}

bool CHTLContext::isInGlobalScript() const {
    return state->isInScriptContext() && state->isInGlobalScope();
}

bool CHTLContext::isInLocalScript() const {
    return state->isInScriptContext() && state->isInLocalScope();
}

// 调试
void CHTLContext::dumpContext() const {
    std::cout << "=== CHTL Context ===" << std::endl;
    std::cout << "Current File: " << currentFilePath << std::endl;
    std::cout << "Current Namespace: " << getCurrentNamespace() << std::endl;
    std::cout << "Config Loaded: " << (configLoaded ? "Yes" : "No") << std::endl;
    std::cout << "Element Stack Size: " << elementStack.size() << std::endl;
    std::cout << "Local Symbols: " << localSymbols.size() << std::endl;
    std::cout << "Inheritance Chain: " << inheritanceChain.size() << std::endl;
}

void CHTLContext::dumpElementStack() const {
    std::cout << "=== Element Stack ===" << std::endl;
    auto temp = elementStack;
    int level = 0;
    while (!temp.empty()) {
        const auto& element = temp.top();
        std::cout << "Level " << level++ << ": " << element.elementName;
        if (!element.elementId.empty()) {
            std::cout << "#" << element.elementId;
        }
        if (!element.elementClasses.empty()) {
            for (const auto& cls : element.elementClasses) {
                std::cout << "." << cls;
            }
        }
        std::cout << std::endl;
        temp.pop();
    }
}

void CHTLContext::dumpInheritanceChain() const {
    std::cout << "=== Inheritance Chain ===" << std::endl;
    for (const auto& info : inheritanceChain) {
        std::cout << info.baseType << " " << info.baseName;
        if (!info.deletedProperties.empty()) {
            std::cout << " (deleted: ";
            for (const auto& prop : info.deletedProperties) {
                std::cout << prop << " ";
            }
            std::cout << ")";
        }
        std::cout << std::endl;
    }
}

} // namespace CHTL