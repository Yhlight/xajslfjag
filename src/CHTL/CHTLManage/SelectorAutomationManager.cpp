#include "SelectorAutomationManager.h"
#include <regex>
#include <sstream>
#include <algorithm>
#include <cctype>

namespace CHTL {

// ========== AutomationRule 实现 ==========

void AutomationRule::loadFromConfig(const ConfigurationNode* configNode) {
    if (!configNode) {
        return;
    }
    
    // 加载样式自动化配置
    const auto* disableStyleClass = configNode->getConfigValue("DISABLE_STYLE_AUTO_ADD_CLASS");
    if (disableStyleClass) {
        enableStyleAutoClass = !disableStyleClass->getBool();
    }
    
    const auto* disableStyleId = configNode->getConfigValue("DISABLE_STYLE_AUTO_ADD_ID");
    if (disableStyleId) {
        enableStyleAutoId = !disableStyleId->getBool();
    }
    
    // 加载脚本自动化配置
    const auto* disableScriptClass = configNode->getConfigValue("DISABLE_SCRIPT_AUTO_ADD_CLASS");
    if (disableScriptClass) {
        enableScriptAutoClass = !disableScriptClass->getBool();
    }
    
    const auto* disableScriptId = configNode->getConfigValue("DISABLE_SCRIPT_AUTO_ADD_ID");
    if (disableScriptId) {
        enableScriptAutoId = !disableScriptId->getBool();
    }
}

// ========== SelectorInfo 实现 ==========

std::string SelectorInfo::toCSSSelector() const {
    switch (type) {
        case SelectorType::CLASS_SELECTOR:
            return "." + name;
        case SelectorType::ID_SELECTOR:
            return "#" + name;
        case SelectorType::REFERENCE_SELECTOR:
            return "&"; // 这里需要在上下文中解析
        default:
            return name;
    }
}

std::string SelectorInfo::toCHTLJSSelector() const {
    switch (type) {
        case SelectorType::CLASS_SELECTOR:
            return "{{." + name + "}}";
        case SelectorType::ID_SELECTOR:
            return "{{#" + name + "}}";
        case SelectorType::REFERENCE_SELECTOR:
            return "{{&}}";
        default:
            return "{{" + name + "}}";
    }
}

// ========== AutomationContext 实现 ==========

AutomationContext::AutomationContext(std::shared_ptr<ElementNode> element) : m_element(element) {
}

std::shared_ptr<ElementNode> AutomationContext::getElement() const {
    return m_element;
}

void AutomationContext::addSelector(const SelectorInfo& selector) {
    // 避免重复添加相同的选择器
    for (const auto& existing : m_selectors) {
        if (existing.type == selector.type && existing.name == selector.name) {
            return;
        }
    }
    m_selectors.push_back(selector);
}

const std::vector<SelectorInfo>& AutomationContext::getSelectors() const {
    return m_selectors;
}

std::vector<SelectorInfo> AutomationContext::getClassSelectors() const {
    std::vector<SelectorInfo> classSelectors;
    for (const auto& selector : m_selectors) {
        if (selector.type == SelectorType::CLASS_SELECTOR) {
            classSelectors.push_back(selector);
        }
    }
    return classSelectors;
}

std::vector<SelectorInfo> AutomationContext::getIdSelectors() const {
    std::vector<SelectorInfo> idSelectors;
    for (const auto& selector : m_selectors) {
        if (selector.type == SelectorType::ID_SELECTOR) {
            idSelectors.push_back(selector);
        }
    }
    return idSelectors;
}

SelectorInfo AutomationContext::getFirstClassSelector() const {
    for (const auto& selector : m_selectors) {
        if (selector.type == SelectorType::CLASS_SELECTOR) {
            return selector;
        }
    }
    return SelectorInfo(); // 返回默认的选择器信息
}

SelectorInfo AutomationContext::getFirstIdSelector() const {
    for (const auto& selector : m_selectors) {
        if (selector.type == SelectorType::ID_SELECTOR) {
            return selector;
        }
    }
    return SelectorInfo(); // 返回默认的选择器信息
}

bool AutomationContext::hasClassSelector() const {
    return !getClassSelectors().empty();
}

bool AutomationContext::hasIdSelector() const {
    return !getIdSelectors().empty();
}

void AutomationContext::clearSelectors() {
    m_selectors.clear();
}

// ========== SelectorAutomationManager 实现 ==========

SelectorAutomationManager::SelectorAutomationManager() {
    // 设置默认的自动化规则
    m_automationRules.enableStyleAutoClass = true;
    m_automationRules.enableStyleAutoId = true;
    m_automationRules.enableScriptAutoClass = false;
    m_automationRules.enableScriptAutoId = false;
}

void SelectorAutomationManager::setAutomationRules(const AutomationRule& rules) {
    m_automationRules = rules;
}

const AutomationRule& SelectorAutomationManager::getAutomationRules() const {
    return m_automationRules;
}

void SelectorAutomationManager::processStyleAutomation(std::shared_ptr<ElementNode> element) {
    if (!element) {
        return;
    }
    
    // 获取或创建自动化上下文
    auto* context = getAutomationContext(element);
    if (!context) {
        context = createAutomationContext(element);
    }
    
    // 解析元素中的样式块
    for (auto child : element->getChildren()) {
        if (auto styleNode = std::dynamic_pointer_cast<StyleNode>(child)) {
            auto selectors = parseStyleSelectors(styleNode);
            
            // 添加选择器到上下文
            for (const auto& selector : selectors) {
                context->addSelector(selector);
            }
            
            // 应用自动化规则
            if (m_automationRules.enableStyleAutoClass) {
                applyClassAutomation(element, selectors);
            }
            
            if (m_automationRules.enableStyleAutoId) {
                applyIdAutomation(element, selectors);
            }
        }
    }
}

void SelectorAutomationManager::processScriptAutomation(std::shared_ptr<ElementNode> element) {
    if (!element) {
        return;
    }
    
    // 获取或创建自动化上下文
    auto* context = getAutomationContext(element);
    if (!context) {
        context = createAutomationContext(element);
    }
    
    // 解析元素中的脚本块
    for (auto child : element->getChildren()) {
        if (auto scriptNode = std::dynamic_pointer_cast<ScriptNode>(child)) {
            auto selectors = parseScriptSelectors(scriptNode);
            
            // 添加选择器到上下文
            for (const auto& selector : selectors) {
                context->addSelector(selector);
            }
            
            // 应用自动化规则
            if (m_automationRules.enableScriptAutoClass) {
                applyClassAutomation(element, selectors);
            }
            
            if (m_automationRules.enableScriptAutoId) {
                applyIdAutomation(element, selectors);
            }
        }
    }
}

std::vector<SelectorInfo> SelectorAutomationManager::parseStyleSelectors(std::shared_ptr<StyleNode> styleNode) {
    std::vector<SelectorInfo> selectors;
    
    if (!styleNode) {
        return selectors;
    }
    
    // 获取样式内容
    std::string styleContent = styleNode->toString(); // 简化：实际应该获取CSS内容
    
    // 解析CSS选择器
    auto cssSelectors = parseCSSSelectors(styleContent);
    selectors.insert(selectors.end(), cssSelectors.begin(), cssSelectors.end());
    
    return selectors;
}

std::vector<SelectorInfo> SelectorAutomationManager::parseScriptSelectors(std::shared_ptr<ScriptNode> scriptNode) {
    std::vector<SelectorInfo> selectors;
    
    if (!scriptNode) {
        return selectors;
    }
    
    // 获取脚本内容
    std::string scriptContent = scriptNode->getContent();
    
    // 解析CHTL JS选择器
    auto jsSelectors = parseCHTLJSSelectors(scriptContent);
    selectors.insert(selectors.end(), jsSelectors.begin(), jsSelectors.end());
    
    return selectors;
}

void SelectorAutomationManager::applyClassAutomation(std::shared_ptr<ElementNode> element, const std::vector<SelectorInfo>& selectors) {
    if (!element) {
        return;
    }
    
    // 查找第一个类选择器
    for (const auto& selector : selectors) {
        if (selector.type == SelectorType::CLASS_SELECTOR) {
            // 检查元素是否已经有class属性
            if (!element->hasAttribute("class")) {
                element->setAttribute("class", selector.name);
                m_usedClassNames.insert(selector.name);
                break; // 只自动添加第一个类选择器
            }
        }
    }
}

void SelectorAutomationManager::applyIdAutomation(std::shared_ptr<ElementNode> element, const std::vector<SelectorInfo>& selectors) {
    if (!element) {
        return;
    }
    
    // 查找第一个ID选择器
    for (const auto& selector : selectors) {
        if (selector.type == SelectorType::ID_SELECTOR) {
            // 检查元素是否已经有id属性
            if (!element->hasAttribute("id")) {
                element->setAttribute("id", selector.name);
                m_usedIdNames.insert(selector.name);
                break; // 只自动添加第一个ID选择器
            }
        }
    }
}

std::string SelectorAutomationManager::resolveReferenceSelector(std::shared_ptr<ElementNode> element, const std::string& context) {
    if (!element) {
        return "";
    }
    
    auto* automationContext = getAutomationContext(element);
    if (!automationContext) {
        return "";
    }
    
    // 根据上下文选择优先级
    if (context == "style") {
        // 样式上下文优先选择类名
        if (automationContext->hasClassSelector()) {
            return automationContext->getFirstClassSelector().toCSSSelector();
        } else if (automationContext->hasIdSelector()) {
            return automationContext->getFirstIdSelector().toCSSSelector();
        }
    } else if (context == "script") {
        // 脚本上下文优先选择ID
        if (automationContext->hasIdSelector()) {
            return automationContext->getFirstIdSelector().toCSSSelector();
        } else if (automationContext->hasClassSelector()) {
            return automationContext->getFirstClassSelector().toCSSSelector();
        }
    }
    
    return "";
}

AutomationContext* SelectorAutomationManager::getAutomationContext(std::shared_ptr<ElementNode> element) {
    if (!element) {
        return nullptr;
    }
    
    auto it = m_automationContexts.find(element.get());
    if (it != m_automationContexts.end()) {
        return it->second.get();
    }
    
    return nullptr;
}

AutomationContext* SelectorAutomationManager::createAutomationContext(std::shared_ptr<ElementNode> element) {
    if (!element) {
        return nullptr;
    }
    
    auto context = std::make_unique<AutomationContext>(element);
    auto* contextPtr = context.get();
    m_automationContexts[element.get()] = std::move(context);
    
    return contextPtr;
}

void SelectorAutomationManager::clearAutomationContext(std::shared_ptr<ElementNode> element) {
    if (!element) {
        return;
    }
    
    m_automationContexts.erase(element.get());
}

void SelectorAutomationManager::clearAllAutomationContexts() {
    m_automationContexts.clear();
}

std::string SelectorAutomationManager::generateAutomationReport() const {
    std::stringstream ss;
    
    ss << "Selector Automation Report:\n";
    ss << "===========================\n\n";
    
    ss << "Automation Rules:\n";
    ss << "  Style Auto Class: " << (m_automationRules.enableStyleAutoClass ? "Enabled" : "Disabled") << "\n";
    ss << "  Style Auto ID: " << (m_automationRules.enableStyleAutoId ? "Enabled" : "Disabled") << "\n";
    ss << "  Script Auto Class: " << (m_automationRules.enableScriptAutoClass ? "Enabled" : "Disabled") << "\n";
    ss << "  Script Auto ID: " << (m_automationRules.enableScriptAutoId ? "Enabled" : "Disabled") << "\n\n";
    
    ss << "Used Class Names (" << m_usedClassNames.size() << "):\n";
    for (const auto& className : m_usedClassNames) {
        ss << "  ." << className << "\n";
    }
    ss << "\n";
    
    ss << "Used ID Names (" << m_usedIdNames.size() << "):\n";
    for (const auto& idName : m_usedIdNames) {
        ss << "  #" << idName << "\n";
    }
    ss << "\n";
    
    ss << "Automation Contexts (" << m_automationContexts.size() << "):\n";
    for (const auto& [element, context] : m_automationContexts) {
        ss << "  Element: " << element->getName() << " (" << context->getSelectors().size() << " selectors)\n";
    }
    
    return ss.str();
}

bool SelectorAutomationManager::validateSelector(const SelectorInfo& selector) const {
    return isValidSelectorName(selector.name);
}

std::string SelectorAutomationManager::normalizeSelector(const std::string& selectorName) const {
    std::string normalized = selectorName;
    
    // 移除前导和尾随空格
    normalized.erase(0, normalized.find_first_not_of(' '));
    normalized.erase(normalized.find_last_not_of(' ') + 1);
    
    // 替换无效字符
    for (char& c : normalized) {
        if (!std::isalnum(c) && c != '_' && c != '-') {
            c = '_';
        }
    }
    
    return normalized;
}

std::vector<SelectorInfo> SelectorAutomationManager::parseCSSSelectors(const std::string& cssContent) {
    std::vector<SelectorInfo> selectors;
    
    // 提取类选择器
    auto classNames = extractClassSelectors(cssContent);
    for (const auto& className : classNames) {
        selectors.emplace_back(SelectorType::CLASS_SELECTOR, className, true);
    }
    
    // 提取ID选择器
    auto idNames = extractIdSelectors(cssContent);
    for (const auto& idName : idNames) {
        selectors.emplace_back(SelectorType::ID_SELECTOR, idName, true);
    }
    
    // 提取引用选择器
    auto references = extractReferenceSelectors(cssContent);
    for (const auto& reference : references) {
        selectors.emplace_back(SelectorType::REFERENCE_SELECTOR, reference, true);
    }
    
    return selectors;
}

std::vector<SelectorInfo> SelectorAutomationManager::parseCHTLJSSelectors(const std::string& jsContent) {
    std::vector<SelectorInfo> selectors;
    
    // 使用正则表达式匹配 {{.className}} 和 {{#idName}} 模式
    std::regex classRegex(R"(\{\{\.([a-zA-Z_][a-zA-Z0-9_-]*)\}\})");
    std::regex idRegex(R"(\{\{#([a-zA-Z_][a-zA-Z0-9_-]*)\}\})");
    std::regex refRegex(R"(\{\{&\}\})");
    
    std::sregex_iterator iter;
    std::sregex_iterator end;
    
    // 查找类选择器
    iter = std::sregex_iterator(jsContent.begin(), jsContent.end(), classRegex);
    for (; iter != end; ++iter) {
        std::string className = (*iter)[1].str();
        selectors.emplace_back(SelectorType::CLASS_SELECTOR, className, true);
    }
    
    // 查找ID选择器
    iter = std::sregex_iterator(jsContent.begin(), jsContent.end(), idRegex);
    for (; iter != end; ++iter) {
        std::string idName = (*iter)[1].str();
        selectors.emplace_back(SelectorType::ID_SELECTOR, idName, true);
    }
    
    // 查找引用选择器
    iter = std::sregex_iterator(jsContent.begin(), jsContent.end(), refRegex);
    for (; iter != end; ++iter) {
        selectors.emplace_back(SelectorType::REFERENCE_SELECTOR, "&", true);
    }
    
    return selectors;
}

std::vector<std::string> SelectorAutomationManager::extractClassSelectors(const std::string& content) {
    std::vector<std::string> classNames;
    std::regex classRegex(R"(\.([a-zA-Z_][a-zA-Z0-9_-]*))");
    
    std::sregex_iterator iter(content.begin(), content.end(), classRegex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        std::string className = (*iter)[1].str();
        // 避免重复添加
        if (std::find(classNames.begin(), classNames.end(), className) == classNames.end()) {
            classNames.push_back(className);
        }
    }
    
    return classNames;
}

std::vector<std::string> SelectorAutomationManager::extractIdSelectors(const std::string& content) {
    std::vector<std::string> idNames;
    std::regex idRegex(R"(#([a-zA-Z_][a-zA-Z0-9_-]*))");
    
    std::sregex_iterator iter(content.begin(), content.end(), idRegex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        std::string idName = (*iter)[1].str();
        // 避免重复添加
        if (std::find(idNames.begin(), idNames.end(), idName) == idNames.end()) {
            idNames.push_back(idName);
        }
    }
    
    return idNames;
}

std::vector<std::string> SelectorAutomationManager::extractReferenceSelectors(const std::string& content) {
    std::vector<std::string> references;
    std::regex refRegex(R"(&(?=\s*[:;{]))"); // & 后面跟着 : ; { 等
    
    std::sregex_iterator iter(content.begin(), content.end(), refRegex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        references.push_back("&");
    }
    
    return references;
}

std::string SelectorAutomationManager::generateUniqueClassName(const std::string& baseName) {
    std::string uniqueName = normalizeSelector(baseName);
    int counter = 1;
    
    while (m_usedClassNames.find(uniqueName) != m_usedClassNames.end()) {
        uniqueName = normalizeSelector(baseName) + "_" + std::to_string(counter);
        counter++;
    }
    
    return uniqueName;
}

std::string SelectorAutomationManager::generateUniqueIdName(const std::string& baseName) {
    std::string uniqueName = normalizeSelector(baseName);
    int counter = 1;
    
    while (m_usedIdNames.find(uniqueName) != m_usedIdNames.end()) {
        uniqueName = normalizeSelector(baseName) + "_" + std::to_string(counter);
        counter++;
    }
    
    return uniqueName;
}

bool SelectorAutomationManager::isValidSelectorName(const std::string& name) const {
    if (name.empty()) {
        return false;
    }
    
    // 检查第一个字符
    if (!std::isalpha(name[0]) && name[0] != '_') {
        return false;
    }
    
    // 检查其余字符
    for (char c : name) {
        if (!std::isalnum(c) && c != '_' && c != '-') {
            return false;
        }
    }
    
    return true;
}

void SelectorAutomationManager::applySelectorToElement(std::shared_ptr<ElementNode> element, const SelectorInfo& selector) {
    if (!element || !validateSelector(selector)) {
        return;
    }
    
    switch (selector.type) {
        case SelectorType::CLASS_SELECTOR:
            if (!element->hasAttribute("class")) {
                element->setAttribute("class", selector.name);
                m_usedClassNames.insert(selector.name);
            }
            break;
        case SelectorType::ID_SELECTOR:
            if (!element->hasAttribute("id")) {
                element->setAttribute("id", selector.name);
                m_usedIdNames.insert(selector.name);
            }
            break;
        case SelectorType::REFERENCE_SELECTOR:
            // 引用选择器不直接应用到元素
            break;
    }
}

// ========== ReferenceRuleProcessor 实现 ==========

ReferenceRuleProcessor::ReferenceRuleProcessor(SelectorAutomationManager* manager) 
    : m_automationManager(manager) {
    // 设置默认优先级
    m_priorities["style"] = SelectorType::CLASS_SELECTOR;  // 样式优先类名
    m_priorities["script"] = SelectorType::ID_SELECTOR;    // 脚本优先ID
}

std::string ReferenceRuleProcessor::processStyleReferences(std::shared_ptr<ElementNode> element, const std::string& styleContent) {
    if (!element || !m_automationManager) {
        return styleContent;
    }
    
    std::string resolvedSelector = resolveReference(element, "style");
    if (!resolvedSelector.empty()) {
        return replaceReferences(styleContent, resolvedSelector);
    }
    
    return styleContent;
}

std::string ReferenceRuleProcessor::processScriptReferences(std::shared_ptr<ElementNode> element, const std::string& scriptContent) {
    if (!element || !m_automationManager) {
        return scriptContent;
    }
    
    std::string resolvedSelector = resolveReference(element, "script");
    if (!resolvedSelector.empty()) {
        return replaceReferences(scriptContent, resolvedSelector);
    }
    
    return scriptContent;
}

std::string ReferenceRuleProcessor::resolveReference(std::shared_ptr<ElementNode> element, const std::string& context) {
    return m_automationManager->resolveReferenceSelector(element, context);
}

std::string ReferenceRuleProcessor::replaceReferences(const std::string& content, const std::string& replacement) {
    std::string result = content;
    
    // 替换 & 引用
    std::regex refRegex(R"(&(?=\s*[:;{]))");
    result = std::regex_replace(result, refRegex, replacement);
    
    // 替换 {{&}} 引用
    std::regex jsRefRegex(R"(\{\{&\}\})");
    std::string jsReplacement = "document.querySelector('" + replacement + "')";
    result = std::regex_replace(result, jsRefRegex, jsReplacement);
    
    return result;
}

void ReferenceRuleProcessor::setReferencePriority(const std::string& context, SelectorType priority) {
    m_priorities[context] = priority;
}

SelectorType ReferenceRuleProcessor::getReferencePriority(const std::string& context) const {
    auto it = m_priorities.find(context);
    if (it != m_priorities.end()) {
        return it->second;
    }
    return SelectorType::CLASS_SELECTOR; // 默认优先类选择器
}

std::vector<size_t> ReferenceRuleProcessor::findReferencePositions(const std::string& content) {
    std::vector<size_t> positions;
    
    size_t pos = content.find('&');
    while (pos != std::string::npos) {
        if (isValidReferenceContext(content, pos)) {
            positions.push_back(pos);
        }
        pos = content.find('&', pos + 1);
    }
    
    return positions;
}

bool ReferenceRuleProcessor::isValidReferenceContext(const std::string& content, size_t position) const {
    if (position >= content.length()) {
        return false;
    }
    
    // 检查 & 后面是否跟着有效的CSS语法
    if (position + 1 < content.length()) {
        char nextChar = content[position + 1];
        return (nextChar == ':' || nextChar == ';' || nextChar == '{' || std::isspace(nextChar));
    }
    
    return false;
}

} // namespace CHTL