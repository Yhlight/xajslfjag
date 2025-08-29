#include "LocalStyleManager.h"
#include <algorithm>
#include <regex>
#include <sstream>

namespace CHTL {

// ========== LocalStyleManager 实现 ==========

LocalStyleManager::LocalStyleManager()
    : m_errorReporter(&getGlobalErrorReporter())
    , m_classCounter(0)
    , m_idCounter(0)
    , m_currentElement(nullptr) {
}

void LocalStyleManager::processLocalStyleBlock(StyleNode* styleNode, ElementNode* parentElement) {
    if (!styleNode || !parentElement) {
        reportError("Invalid style node or parent element");
        return;
    }
    
    setCurrentElement(parentElement);
    updateStatistics("style_blocks_processed");
    
    // 根据样式类型进行不同的处理
    switch (styleNode->getStyleType()) {
        case StyleNodeType::LOCAL_STYLE:
            // 处理局部样式块的所有功能
            processInlineStyle(styleNode);
            processSelectorStyles(styleNode, parentElement);
            break;
            
        case StyleNodeType::INLINE_STYLE:
            // 处理内联样式
            processInlineStyle(styleNode);
            break;
            
        case StyleNodeType::SELECTOR_STYLE:
            // 处理选择器样式
            processSelectorStyles(styleNode, parentElement);
            break;
    }
}

std::string LocalStyleManager::processInlineStyle(const StyleNode* styleNode) {
    if (!styleNode) return "";
    
    std::stringstream inlineStyle;
    const auto& properties = styleNode->getCssProperties();
    
    bool first = true;
    for (const auto& [property, value] : properties) {
        if (!first) inlineStyle << " ";
        inlineStyle << property << ": " << value << ";";
        first = false;
    }
    
    updateStatistics("inline_styles_processed");
    return inlineStyle.str();
}

std::vector<StyleRule> LocalStyleManager::processSelectorStyles(const StyleNode* styleNode, ElementNode* parentElement) {
    std::vector<StyleRule> rules;
    
    if (!styleNode || !parentElement) {
        return rules;
    }
    
    // 如果有选择器，处理选择器样式
    if (styleNode->hasSelector()) {
        StyleRule rule;
        rule.originalSelector = styleNode->getSelector();
        rule.selector = processContextSelector(rule.originalSelector, parentElement);
        rule.properties = styleNode->getCssProperties();
        rule.type = parseSelector(rule.originalSelector);
        rule.targetElement = parentElement;
        rule.isGlobal = true;
        rule.priority = calculateStylePriority(rule);
        
        // 根据选择器类型进行自动化处理
        if (rule.type == LocalStyleType::CLASS_SELECTOR) {
            std::string className = extractSelectorName(rule.originalSelector);
            if (!m_automationRules.disableStyleAutoAddClass) {
                autoAddClassName(parentElement, className);
            }
        } else if (rule.type == LocalStyleType::ID_SELECTOR) {
            std::string idName = extractSelectorName(rule.originalSelector);
            if (!m_automationRules.disableStyleAutoAddId) {
                autoAddId(parentElement, idName);
            }
        }
        
        rules.push_back(rule);
        addGlobalStyleRule(rule);
        updateStatistics("selector_styles_processed");
    }
    
    return rules;
}

std::string LocalStyleManager::processContextSelector(const std::string& selector, ElementNode* element) {
    if (!element) return selector;
    
    std::string processedSelector = selector;
    
    // 处理&符号的上下文推导
    if (selector.find('&') != std::string::npos) {
        std::string contextRef;
        
        // 优先选择类名
        if (element->hasAttribute("class")) {
            std::string className = element->getAttribute("class");
            if (!className.empty()) {
                contextRef = "." + className;
            }
        }
        
        // 如果没有类名，选择ID
        if (contextRef.empty() && element->hasAttribute("id")) {
            std::string idName = element->getAttribute("id");
            if (!idName.empty()) {
                contextRef = "#" + idName;
            }
        }
        
        // 如果都没有，生成自动类名
        if (contextRef.empty()) {
            std::string autoClass = generateAutoClassName(element->getTagName());
            autoAddClassName(element, autoClass);
            contextRef = "." + autoClass;
        }
        
        // 替换&符号
        processedSelector = std::regex_replace(processedSelector, std::regex("&"), contextRef);
        updateStatistics("context_selectors_processed");
    }
    
    return processedSelector;
}

std::string LocalStyleManager::autoAddClassName(ElementNode* element, const std::string& className) {
    if (!element || className.empty()) return "";
    
    std::string finalClassName = className;
    
    // 添加前缀
    if (!m_automationRules.classPrefix.empty()) {
        finalClassName = m_automationRules.classPrefix + finalClassName;
    }
    
    // 确保类名唯一
    if (isNameUsed(finalClassName, true)) {
        finalClassName = generateUniqueName(finalClassName, m_usedClassNames);
    }
    
    // 添加到元素
    element->addClass(finalClassName);
    
    // 记录使用的类名
    m_usedClassNames.insert(finalClassName);
    m_generatedClasses.push_back(finalClassName);
    
    updateStatistics("auto_classes_added");
    return finalClassName;
}

std::string LocalStyleManager::autoAddId(ElementNode* element, const std::string& idName) {
    if (!element || idName.empty()) return "";
    
    std::string finalIdName = idName;
    
    // 添加前缀
    if (!m_automationRules.idPrefix.empty()) {
        finalIdName = m_automationRules.idPrefix + finalIdName;
    }
    
    // 确保ID唯一
    if (isNameUsed(finalIdName, false)) {
        finalIdName = generateUniqueName(finalIdName, m_usedIds);
    }
    
    // 设置到元素
    element->setId(finalIdName);
    
    // 记录使用的ID
    m_usedIds.insert(finalIdName);
    m_generatedIds.push_back(finalIdName);
    
    updateStatistics("auto_ids_added");
    return finalIdName;
}

std::string LocalStyleManager::generateAutoClassName(const std::string& base) {
    std::string className = base.empty() ? "auto" : base;
    className += "-" + std::to_string(++m_classCounter);
    
    // 确保唯一性
    while (isNameUsed(className, true)) {
        className = base + "-" + std::to_string(++m_classCounter);
    }
    
    return className;
}

std::string LocalStyleManager::generateAutoId(const std::string& base) {
    std::string idName = base.empty() ? "auto" : base;
    idName += "-" + std::to_string(++m_idCounter);
    
    // 确保唯一性
    while (isNameUsed(idName, false)) {
        idName = base + "-" + std::to_string(++m_idCounter);
    }
    
    return idName;
}

LocalStyleType LocalStyleManager::parseSelector(const std::string& selector) {
    if (selector.empty()) return LocalStyleType::INLINE_STYLE;
    
    if (selector[0] == '.') {
        if (selector.find("::") != std::string::npos) {
            return LocalStyleType::PSEUDO_ELEMENT;
        } else if (selector.find(':') != std::string::npos) {
            return LocalStyleType::PSEUDO_CLASS;
        }
        return LocalStyleType::CLASS_SELECTOR;
    }
    
    if (selector[0] == '#') {
        if (selector.find("::") != std::string::npos) {
            return LocalStyleType::PSEUDO_ELEMENT;
        } else if (selector.find(':') != std::string::npos) {
            return LocalStyleType::PSEUDO_CLASS;
        }
        return LocalStyleType::ID_SELECTOR;
    }
    
    if (selector.find('&') != std::string::npos) {
        return LocalStyleType::CONTEXT_SELECTOR;
    }
    
    if (selector.find("::") != std::string::npos) {
        return LocalStyleType::PSEUDO_ELEMENT;
    }
    
    if (selector.find(':') != std::string::npos) {
        return LocalStyleType::PSEUDO_CLASS;
    }
    
    return LocalStyleType::INLINE_STYLE;
}

std::string LocalStyleManager::extractSelectorName(const std::string& selector) {
    if (selector.empty()) return "";
    
    std::string name = selector;
    
    // 移除前导符号
    if (name[0] == '.' || name[0] == '#') {
        name = name.substr(1);
    }
    
    // 移除伪类/伪元素部分
    size_t colonPos = name.find(':');
    if (colonPos != std::string::npos) {
        name = name.substr(0, colonPos);
    }
    
    return name;
}

bool LocalStyleManager::validateSelector(const std::string& selector) {
    if (selector.empty()) return false;
    
    // 基本格式验证
    std::regex selectorPattern(R"(^[.#&]?[a-zA-Z_][\w-]*(::|:[\w-]+)*$)");
    return std::regex_match(selector, selectorPattern);
}

std::string LocalStyleManager::processPseudoClassSelector(const std::string& selector, ElementNode* element) {
    if (!element) return selector;
    
    // 处理伪类选择器，如 &:hover
    std::string processed = processContextSelector(selector, element);
    updateStatistics("pseudo_class_selectors_processed");
    return processed;
}

std::string LocalStyleManager::processPseudoElementSelector(const std::string& selector, ElementNode* element) {
    if (!element) return selector;
    
    // 处理伪元素选择器，如 &::before
    std::string processed = processContextSelector(selector, element);
    updateStatistics("pseudo_element_selectors_processed");
    return processed;
}

void LocalStyleManager::addGlobalStyleRule(const StyleRule& rule) {
    m_globalStyles.push_back(rule);
    
    // 如果选择器已存在，合并属性
    auto it = m_selectorMap.find(rule.selector);
    if (it != m_selectorMap.end()) {
        // 按书写顺序替换重复属性
        for (const auto& [property, value] : rule.properties) {
            it->second.properties[property] = value;
        }
    } else {
        m_selectorMap[rule.selector] = rule;
    }
    
    updateStatistics("global_rules_added");
}

const std::vector<StyleRule>& LocalStyleManager::getGlobalStyleRules() const {
    return m_globalStyles;
}

std::string LocalStyleManager::generateGlobalCSS() {
    std::stringstream css;
    
    // 按优先级排序
    std::vector<StyleRule> sortedRules = m_globalStyles;
    std::sort(sortedRules.begin(), sortedRules.end(), 
             [this](const StyleRule& a, const StyleRule& b) {
                 return calculateStylePriority(a) < calculateStylePriority(b);
             });
    
    for (const auto& rule : sortedRules) {
        if (rule.isGlobal && !rule.properties.empty()) {
            css << formatCSSRule(rule) << "\n";
        }
    }
    
    return css.str();
}

void LocalStyleManager::clearGlobalStyles() {
    m_globalStyles.clear();
    m_selectorMap.clear();
    updateStatistics("global_styles_cleared");
}

size_t LocalStyleManager::calculateStylePriority(const StyleRule& rule) {
    size_t priority = 0;
    
    // ID选择器优先级最高
    if (rule.type == LocalStyleType::ID_SELECTOR) {
        priority += 100;
    }
    
    // 类选择器次之
    if (rule.type == LocalStyleType::CLASS_SELECTOR) {
        priority += 10;
    }
    
    // 伪类和伪元素增加优先级
    if (rule.type == LocalStyleType::PSEUDO_CLASS || 
        rule.type == LocalStyleType::PSEUDO_ELEMENT) {
        priority += 1;
    }
    
    // 自定义优先级
    priority += rule.priority;
    
    return priority;
}

void LocalStyleManager::resolveStyleConflicts() {
    // 按书写顺序解决冲突（后写的覆盖先写的）
    for (auto& [selector, rule] : m_selectorMap) {
        // 查找相同选择器的后续规则
        for (size_t i = 0; i < m_globalStyles.size(); ++i) {
            if (m_globalStyles[i].selector == selector) {
                // 合并属性
                for (const auto& [property, value] : m_globalStyles[i].properties) {
                    rule.properties[property] = value;
                }
            }
        }
    }
    
    updateStatistics("conflicts_resolved");
}

void LocalStyleManager::mergeDuplicateSelectors() {
    std::unordered_map<std::string, StyleRule> mergedRules;
    
    for (const auto& rule : m_globalStyles) {
        auto it = mergedRules.find(rule.selector);
        if (it != mergedRules.end()) {
            // 合并属性
            for (const auto& [property, value] : rule.properties) {
                it->second.properties[property] = value;
            }
        } else {
            mergedRules[rule.selector] = rule;
        }
    }
    
    // 重建全局样式列表
    m_globalStyles.clear();
    for (const auto& [selector, rule] : mergedRules) {
        m_globalStyles.push_back(rule);
    }
    
    updateStatistics("selectors_merged");
}

void LocalStyleManager::setAutomationRules(const AutomationRules& rules) {
    m_automationRules = rules;
}

const AutomationRules& LocalStyleManager::getAutomationRules() const {
    return m_automationRules;
}

void LocalStyleManager::setErrorReporter(ErrorReporter* errorReporter) {
    m_errorReporter = errorReporter ? errorReporter : &getGlobalErrorReporter();
}

void LocalStyleManager::setCurrentElement(ElementNode* element) {
    m_currentElement = element;
}

ElementNode* LocalStyleManager::getCurrentElement() const {
    return m_currentElement;
}

void LocalStyleManager::pushElementContext(ElementNode* element) {
    m_elementStack.push_back(element);
    setCurrentElement(element);
}

ElementNode* LocalStyleManager::popElementContext() {
    if (!m_elementStack.empty()) {
        ElementNode* element = m_elementStack.back();
        m_elementStack.pop_back();
        
        // 设置新的当前元素
        m_currentElement = m_elementStack.empty() ? nullptr : m_elementStack.back();
        
        return element;
    }
    return nullptr;
}

std::unordered_map<std::string, size_t> LocalStyleManager::getStatistics() const {
    return m_statistics;
}

std::vector<std::string> LocalStyleManager::getGeneratedClassNames() const {
    return m_generatedClasses;
}

std::vector<std::string> LocalStyleManager::getGeneratedIds() const {
    return m_generatedIds;
}

void LocalStyleManager::reset() {
    m_globalStyles.clear();
    m_selectorMap.clear();
    m_classCounter = 0;
    m_idCounter = 0;
    m_usedClassNames.clear();
    m_usedIds.clear();
    m_generatedClasses.clear();
    m_generatedIds.clear();
    m_elementStack.clear();
    m_currentElement = nullptr;
    m_statistics.clear();
}

// ========== 私有辅助方法 ==========

std::string LocalStyleManager::generateUniqueName(const std::string& base, 
                                                 const std::unordered_set<std::string>& usedNames) {
    std::string uniqueName = base;
    size_t counter = 1;
    
    while (usedNames.find(uniqueName) != usedNames.end()) {
        uniqueName = base + "-" + std::to_string(counter++);
    }
    
    return uniqueName;
}

bool LocalStyleManager::isNameUsed(const std::string& name, bool isClass) const {
    if (isClass) {
        return m_usedClassNames.find(name) != m_usedClassNames.end();
    } else {
        return m_usedIds.find(name) != m_usedIds.end();
    }
}

void LocalStyleManager::updateStatistics(const std::string& key, size_t increment) {
    m_statistics[key] += increment;
}

void LocalStyleManager::reportError(const std::string& message) {
    if (m_errorReporter) {
        ErrorPosition pos("", 0, 0);
        m_errorReporter->error(ErrorType::SEMANTIC_ERROR, message, pos);
    }
}

void LocalStyleManager::reportWarning(const std::string& message) {
    if (m_errorReporter) {
        ErrorPosition pos("", 0, 0);
        m_errorReporter->warning(message, pos);
    }
}

std::string LocalStyleManager::formatCSSRule(const StyleRule& rule) {
    std::stringstream css;
    
    css << rule.selector << " {\n";
    for (const auto& [property, value] : rule.properties) {
        css << "  " << property << ": " << value << ";\n";
    }
    css << "}";
    
    return css.str();
}

// 简化实现的其他方法

// ========== LocalStyleManagerFactory 实现 ==========

std::unique_ptr<LocalStyleManager> LocalStyleManagerFactory::createStandardManager() {
    return std::make_unique<LocalStyleManager>();
}

std::unique_ptr<LocalStyleManager> LocalStyleManagerFactory::createStrictManager() {
    auto manager = std::make_unique<LocalStyleManager>();
    
    AutomationRules strictRules;
    strictRules.enableAutoClass = false;
    strictRules.enableAutoId = false;
    manager->setAutomationRules(strictRules);
    
    return manager;
}

std::unique_ptr<LocalStyleManager> LocalStyleManagerFactory::createCustomManager(const AutomationRules& rules) {
    auto manager = std::make_unique<LocalStyleManager>();
    manager->setAutomationRules(rules);
    return manager;
}

} // namespace CHTL