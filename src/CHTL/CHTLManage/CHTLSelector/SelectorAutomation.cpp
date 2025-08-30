#include "SelectorAutomation.h"
#include "../../../Util/StringUtils.h"
#include "../../CHTLNode/BaseNode.h"
#include <regex>
#include <algorithm>

namespace CHTL {

// 配置加载
void SelectorAutomationConfig::loadFromConfiguration(const ConfigurationManager& configManager) {
    // 简化实现：直接使用默认配置
    // 如果需要从配置管理器读取，需要实现相应的接口
    
    // 使用文档中的默认值
    disableStyleAutoAddClass = false;
    disableStyleAutoAddId = false;
    disableScriptAutoAddClass = true;
    disableScriptAutoAddId = true;
}

// SelectorAutomationManager 实现
SelectorAutomationManager::SelectorAutomationManager(const SelectorAutomationConfig& config)
    : config(config) {}

StringVector SelectorAutomationManager::analyzeElementSelectors(const BaseNode* elementNode) {
    StringVector selectors;
    
    if (!elementNode) return selectors;
    
    // 遍历元素的所有子节点，查找样式和脚本块
    for (const auto& child : elementNode->getChildren()) {
        if (child->getType() == NodeType::STYLE) {
            auto styleSelectors = extractStyleSelectors(child.get());
            selectors.insert(selectors.end(), styleSelectors.begin(), styleSelectors.end());
        }
        else if (child->getType() == NodeType::SCRIPT) {
            auto scriptSelectors = extractScriptSelectorReferences(child.get());
            selectors.insert(selectors.end(), scriptSelectors.begin(), scriptSelectors.end());
        }
    }
    
    return selectors;
}

void SelectorAutomationManager::autoAddSelectorsToElement(BaseNode* elementNode) {
    if (!elementNode) return;
    
    auto selectors = analyzeElementSelectors(elementNode);
    
    // 自动添加第一个类选择器
    if (!config.disableStyleAutoAddClass) {
        String firstClass = getFirstClassSelector(selectors);
        if (!firstClass.empty() && !elementHasAttribute(elementNode, "class")) {
            addAttributeToElement(elementNode, "class", cleanSelectorName(firstClass));
        }
    }
    
    // 自动添加第一个ID选择器
    if (!config.disableStyleAutoAddId) {
        String firstId = getFirstIdSelector(selectors);
        if (!firstId.empty() && !elementHasAttribute(elementNode, "id")) {
            addAttributeToElement(elementNode, "id", cleanSelectorName(firstId));
        }
    }
}

void SelectorAutomationManager::processLocalStyleSelectors(BaseNode* elementNode, BaseNode* styleNode) {
    // 根据文档要求：当局部style内部存在多组类选择器时，若class属性缺失，则自动添加第一个类选择器
    if (!elementNode || !styleNode) return;
    
    auto styleSelectors = extractStyleSelectors(styleNode);
    
    // 处理类选择器自动化
    if (!config.disableStyleAutoAddClass) {
        String firstClass = getFirstClassSelector(styleSelectors);
        if (!firstClass.empty() && !elementHasAttribute(elementNode, "class")) {
            addAttributeToElement(elementNode, "class", cleanSelectorName(firstClass));
        }
    }
    
    // 处理ID选择器自动化
    if (!config.disableStyleAutoAddId) {
        String firstId = getFirstIdSelector(styleSelectors);
        if (!firstId.empty() && !elementHasAttribute(elementNode, "id")) {
            addAttributeToElement(elementNode, "id", cleanSelectorName(firstId));
        }
    }
}

void SelectorAutomationManager::processLocalScriptSelectors(BaseNode* elementNode, BaseNode* scriptNode) {
    // 根据文档要求：当局部script内部存在多组类选择器时，若class属性，局部style没有触发class自动化添加缺失，第一个{{.box}}会被自动添加
    if (!elementNode || !scriptNode) return;
    
    auto scriptSelectors = extractScriptSelectorReferences(scriptNode);
    
    // 只有在脚本自动化开启时才处理
    if (!config.disableScriptAutoAddClass) {
        String firstClass = getFirstClassSelector(scriptSelectors);
        if (!firstClass.empty() && !elementHasAttribute(elementNode, "class")) {
            // 检查是否已经通过样式块自动化添加了类名
            bool hasStyleClassAutomation = false;
            for (const auto& child : elementNode->getChildren()) {
                if (child->getType() == NodeType::STYLE) {
                    auto styleSelectors = extractStyleSelectors(child.get());
                    if (!getFirstClassSelector(styleSelectors).empty()) {
                        hasStyleClassAutomation = true;
                        break;
                    }
                }
            }
            
            // 如果样式块没有触发自动化，脚本块来处理
            if (!hasStyleClassAutomation) {
                addAttributeToElement(elementNode, "class", cleanSelectorName(firstClass));
            }
        }
    }
    
    // ID选择器的类似处理
    if (!config.disableScriptAutoAddId) {
        String firstId = getFirstIdSelector(scriptSelectors);
        if (!firstId.empty() && !elementHasAttribute(elementNode, "id")) {
            bool hasStyleIdAutomation = false;
            for (const auto& child : elementNode->getChildren()) {
                if (child->getType() == NodeType::STYLE) {
                    auto styleSelectors = extractStyleSelectors(child.get());
                    if (!getFirstIdSelector(styleSelectors).empty()) {
                        hasStyleIdAutomation = true;
                        break;
                    }
                }
            }
            
            if (!hasStyleIdAutomation) {
                addAttributeToElement(elementNode, "id", cleanSelectorName(firstId));
            }
        }
    }
}

String SelectorAutomationManager::resolveAmpersandReference(const BaseNode* contextNode, const String& referenceContext) const {
    // 根据文档：对于局部style来说，& 引用选择器优先选择class
    // 对于局部script来说，& 引用选择器优先选择id
    
    if (!contextNode) return "";
    
    // 查找父级元素节点
    const BaseNode* elementNode = contextNode;
    while (elementNode && elementNode->getType() != NodeType::ELEMENT) {
        elementNode = elementNode->getParent();
    }
    
    if (!elementNode) return "";
    
    if (referenceContext == "style") {
        // 样式块中的&引用，优先选择class
        if (elementHasAttribute(elementNode, "class")) {
            return "." + elementNode->getAttribute("class");
        } else if (elementHasAttribute(elementNode, "id")) {
            return "#" + elementNode->getAttribute("id");
        }
    } else if (referenceContext == "script") {
        // 脚本块中的&引用，优先选择id
        if (elementHasAttribute(elementNode, "id")) {
            return "#" + elementNode->getAttribute("id");
        } else if (elementHasAttribute(elementNode, "class")) {
            return "." + elementNode->getAttribute("class");
        }
    }
    
    return "";
}

String SelectorAutomationManager::resolveSelectorReference(const String& selectorRef, const BaseNode* contextNode) const {
    // 解析{{.box}}, {{#box}}, {{box}}等选择器引用
    
    // 提取选择器内容（去掉{{}}）
    String selector = selectorRef;
    if (Util::StringUtils::starts_with(selector, "{{") && Util::StringUtils::ends_with(selector, "}}")) {
        selector = selector.substr(2, selector.length() - 4);
    }
    
    // 返回DOM选择器字符串
    return "document.querySelector('" + selector + "')";
}

bool SelectorAutomationManager::validateSelector(const String& selector) {
    if (selector.empty()) return false;
    
    // 基本的选择器验证
    // 类选择器
    if (Util::StringUtils::starts_with(selector, ".")) {
        return selector.length() > 1 && std::isalpha(selector[1]);
    }
    
    // ID选择器
    if (Util::StringUtils::starts_with(selector, "#")) {
        return selector.length() > 1 && std::isalpha(selector[1]);
    }
    
    // 标签选择器
    if (std::isalpha(selector[0])) {
        return true;
    }
    
    return false;
}

String SelectorAutomationManager::getFirstClassSelector(const StringVector& selectors) {
    for (const auto& selector : selectors) {
        if (Util::StringUtils::starts_with(selector, ".")) {
            return selector;
        }
    }
    return "";
}

String SelectorAutomationManager::getFirstIdSelector(const StringVector& selectors) {
    for (const auto& selector : selectors) {
        if (Util::StringUtils::starts_with(selector, "#")) {
            return selector;
        }
    }
    return "";
}

SelectorInfo SelectorAutomationManager::parseSelector(const String& selectorString, const Position& position) {
    String cleanSelector = Util::StringUtils::trim(selectorString);
    
    if (Util::StringUtils::starts_with(cleanSelector, ".")) {
        return SelectorInfo(SelectorType::CLASS, cleanSelector.substr(1), cleanSelector, position);
    } else if (Util::StringUtils::starts_with(cleanSelector, "#")) {
        return SelectorInfo(SelectorType::ID, cleanSelector.substr(1), cleanSelector, position);
    } else if (cleanSelector.find(":") != String::npos) {
        return SelectorInfo(SelectorType::PSEUDO, cleanSelector, cleanSelector, position);
    } else if (cleanSelector.find("[") != String::npos) {
        return SelectorInfo(SelectorType::ATTRIBUTE, cleanSelector, cleanSelector, position);
    } else if (cleanSelector.find(" ") != String::npos) {
        return SelectorInfo(SelectorType::COMPLEX, cleanSelector, cleanSelector, position);
    } else {
        return SelectorInfo(SelectorType::TAG, cleanSelector, cleanSelector, position);
    }
}

StringVector SelectorAutomationManager::extractStyleSelectors(const BaseNode* styleNode) {
    StringVector selectors;
    
    if (!styleNode) return selectors;
    
    // 解析样式块内容，提取选择器
    String content = styleNode->getValue();
    
    // 使用正则表达式匹配CSS选择器
    std::regex selectorRegex(R"(([.#]?[a-zA-Z][a-zA-Z0-9_-]*)\s*\{)");
    std::sregex_iterator begin(content.begin(), content.end(), selectorRegex);
    std::sregex_iterator end;
    
    for (auto it = begin; it != end; ++it) {
        String selector = (*it)[1].str();
        if (validateSelector(selector)) {
            selectors.push_back(selector);
        }
    }
    
    return selectors;
}

StringVector SelectorAutomationManager::extractScriptSelectorReferences(const BaseNode* scriptNode) {
    StringVector selectors;
    
    if (!scriptNode) return selectors;
    
    String content = scriptNode->getValue();
    
    // 匹配{{selector}}模式
    std::regex selectorRefRegex(R"(\{\{([^}]+)\}\})");
    std::sregex_iterator begin(content.begin(), content.end(), selectorRefRegex);
    std::sregex_iterator end;
    
    for (auto it = begin; it != end; ++it) {
        String selectorRef = (*it)[1].str();
        
        // 只处理.box和#box形式的引用（根据文档要求）
        if (Util::StringUtils::starts_with(selectorRef, ".") || Util::StringUtils::starts_with(selectorRef, "#")) {
            selectors.push_back(selectorRef);
        }
    }
    
    return selectors;
}

bool SelectorAutomationManager::elementHasAttribute(const BaseNode* element, const String& attributeName) const {
    if (!element) return false;
    
    // 这里需要实际检查元素的属性
    // 简化实现，假设BaseNode有getAttribute方法
    try {
        String value = element->getAttribute(attributeName);
        return !value.empty();
    } catch (...) {
        return false;
    }
}

void SelectorAutomationManager::addAttributeToElement(BaseNode* element, const String& attributeName, const String& attributeValue) {
    if (!element) return;
    
    // 这里需要实际为元素添加属性
    // 简化实现，假设BaseNode有setAttribute方法
    try {
        element->setAttribute(attributeName, attributeValue);
    } catch (...) {
        // 忽略错误
    }
}

String SelectorAutomationManager::cleanSelectorName(const String& selector) {
    String clean = selector;
    if (Util::StringUtils::starts_with(clean, ".") || Util::StringUtils::starts_with(clean, "#")) {
        clean = clean.substr(1);
    }
    return clean;
}

// ReferenceRuleManager 实现
void ReferenceRuleManager::processReferences(BaseNode* documentNode, const SelectorAutomationManager& selectorManager) {
    if (!documentNode) return;
    
    processNodeReferences(documentNode, selectorManager);
}

String ReferenceRuleManager::resolveVariableReference(const String& variableRef, const BaseNode* contextNode) {
    // 解析 ThemeColor(tableColor) 形式的变量引用
    
    std::regex varRefRegex(R"(([a-zA-Z][a-zA-Z0-9_]*)\(([^)]+)\))");
    std::smatch match;
    
    if (std::regex_match(variableRef, match, varRefRegex)) {
        String varGroupName = match[1].str();
        String varName = match[2].str();
        
        String varValue = findVariableDefinition(varName, contextNode);
        return varValue.empty() ? variableRef : varValue;
    }
    
    return variableRef;
}

String ReferenceRuleManager::resolveNamespaceReference(const String& namespaceRef, const BaseNode* contextNode) {
    // 解析 from space.room2 形式的命名空间引用
    
    if (Util::StringUtils::starts_with(namespaceRef, "from ")) {
        String namespacePath = namespaceRef.substr(5);
        
        BaseNode* namespaceNode = findNamespaceDefinition(namespacePath, contextNode);
        if (namespaceNode) {
            return namespaceNode->getValue();
        }
    }
    
    return namespaceRef;
}

String ReferenceRuleManager::findVariableDefinition(const String& variableName, const BaseNode* contextNode) {
    // 向上遍历AST查找变量定义
    const BaseNode* current = contextNode;
    
    while (current) {
        // 查找变量模板或自定义变量
        for (const auto& child : current->getChildren()) {
            if (child->getType() == NodeType::TEMPLATE || child->getType() == NodeType::CUSTOM) {
                // 检查是否为变量组
                if (child->getAttribute("type") == "Var") {
                    // 在变量组中查找指定变量
                    for (const auto& varChild : child->getChildren()) {
                        if (varChild->getAttribute("name") == variableName) {
                            return varChild->getValue();
                        }
                    }
                }
            }
        }
        current = current->getParent();
    }
    
    return "";
}

BaseNode* ReferenceRuleManager::findNamespaceDefinition(const String& namespaceName, const BaseNode* contextNode) {
    // 查找命名空间定义（简化实现）
    const BaseNode* current = contextNode;
    
    while (current) {
        for (const auto& child : current->getChildren()) {
            if (child->getType() == NodeType::NAMESPACE) {
                if (child->getAttribute("name") == namespaceName) {
                    return child.get();
                }
            }
        }
        current = current->getParent();
    }
    
    return nullptr;
}

void ReferenceRuleManager::processNodeReferences(BaseNode* node, const SelectorAutomationManager& selectorManager) {
    if (!node) return;
    
    // 处理当前节点的引用
    String nodeValue = node->getValue();
    
    // 简化实现：暂时不直接修改节点内容
    // 在实际应用中，应该在生成阶段处理这些引用
    
    // 处理&引用（记录但不直接修改）
    if (nodeValue.find("&") != String::npos) {
        String context = (node->getType() == NodeType::STYLE) ? "style" : "script";
        String resolved = selectorManager.resolveAmpersandReference(node, context);
        // 在这里可以记录引用信息，但不直接修改节点
    }
    
    // 处理{{selector}}引用（记录但不直接修改）
    std::regex selectorRefRegex(R"(\{\{([^}]+)\}\})");
    std::sregex_iterator begin(nodeValue.begin(), nodeValue.end(), selectorRefRegex);
    std::sregex_iterator end;
    
    for (auto it = begin; it != end; ++it) {
        String selectorRef = (*it).str();
        String resolved = selectorManager.resolveSelectorReference(selectorRef, node);
        // 在这里可以记录引用信息，但不直接修改节点
    }
    
    // 递归处理子节点
    for (auto& child : node->getChildren()) {
        processNodeReferences(child.get(), selectorManager);
    }
}

// 全局实例
static SelectorAutomationManager globalSelectorAutomation;
static ReferenceRuleManager globalReferenceRuleManager;

SelectorAutomationManager& getGlobalSelectorAutomation() {
    return globalSelectorAutomation;
}

ReferenceRuleManager& getGlobalReferenceRuleManager() {
    return globalReferenceRuleManager;
}

} // namespace CHTL