#include "SelectorAutomation.h"
#include <regex>
#include <sstream>
#include <algorithm>
#include <cctype>

namespace CHTL {

// AutomationConfig实现
AutomationConfig AutomationConfig::fromConfigBlock(const std::unordered_map<std::string, std::string>& config) {
    AutomationConfig result;
    
    auto getValue = [&](const std::string& key) -> bool {
        auto it = config.find(key);
        if (it != config.end()) {
            std::string value = it->second;
            std::transform(value.begin(), value.end(), value.begin(), ::tolower);
            return value == "true" || value == "1";
        }
        return false;
    };
    
    result.disableStyleAutoAddClass = getValue("DISABLE_STYLE_AUTO_ADD_CLASS");
    result.disableStyleAutoAddId = getValue("DISABLE_STYLE_AUTO_ADD_ID");
    result.disableScriptAutoAddClass = getValue("DISABLE_SCRIPT_AUTO_ADD_CLASS");
    result.disableScriptAutoAddId = getValue("DISABLE_SCRIPT_AUTO_ADD_ID");
    result.disableDefaultNamespace = getValue("DISABLE_DEFAULT_NAMESPACE");
    
    return result;
}

// SelectorAutomation实现
SelectorAutomation::SelectorAutomation() {}

std::vector<SelectorInfo> SelectorAutomation::parseSelectors(const std::string& css) {
    std::vector<SelectorInfo> selectors;
    
    // 简化的CSS选择器解析
    // 实际实现需要更复杂的CSS解析器
    std::regex selectorRegex(R"(([.#]?[\w-]+(?:\[[^\]]+\])?(?::[^{]+)?))");
    
    size_t pos = 0;
    size_t line = 1;
    size_t column = 1;
    
    auto begin = std::sregex_iterator(css.begin(), css.end(), selectorRegex);
    auto end = std::sregex_iterator();
    
    for (auto it = begin; it != end; ++it) {
        std::smatch match = *it;
        std::string selector = match[0];
        
        SelectorInfo info;
        info.raw = selector;
        info.type = identifySelectorType(selector);
        info.value = extractSelectorValue(selector, info.type);
        info.line = line;
        info.column = column;
        
        selectors.push_back(info);
        
        // 更新位置
        for (size_t i = pos; i < match.position(); ++i) {
            if (css[i] == '\n') {
                line++;
                column = 1;
            } else {
                column++;
            }
        }
        pos = match.position() + match.length();
    }
    
    return selectors;
}

std::vector<SelectorInfo> SelectorAutomation::extractFromStyleBlock(const std::string& styleContent) {
    return parseSelectors(styleContent);
}

std::vector<SelectorInfo> SelectorAutomation::extractFromScriptBlock(const std::string& scriptContent) {
    std::vector<SelectorInfo> selectors;
    
    // 匹配 {{.class}}, {{#id}}, {{tag}} 等CHTL JS选择器
    std::regex chtljsRegex(R"(\{\{([.#]?[\w-]+(?:\[\d+\])?)\}\})");
    
    size_t line = 1;
    size_t column = 1;
    size_t pos = 0;
    
    auto begin = std::sregex_iterator(scriptContent.begin(), scriptContent.end(), chtljsRegex);
    auto end = std::sregex_iterator();
    
    for (auto it = begin; it != end; ++it) {
        std::smatch match = *it;
        std::string selector = match[1];
        
        SelectorInfo info;
        info.raw = "{{" + selector + "}}";
        info.type = identifySelectorType(selector);
        info.value = extractSelectorValue(selector, info.type);
        info.line = line;
        info.column = column;
        
        selectors.push_back(info);
        
        // 更新位置
        for (size_t i = pos; i < match.position(); ++i) {
            if (scriptContent[i] == '\n') {
                line++;
                column = 1;
            } else {
                column++;
            }
        }
        pos = match.position() + match.length();
    }
    
    return selectors;
}

std::optional<std::string> SelectorAutomation::getFirstClassSelector(const std::vector<SelectorInfo>& selectors) {
    for (const auto& selector : selectors) {
        if (selector.type == SelectorType::Class) {
            return selector.value;
        }
    }
    return std::nullopt;
}

std::optional<std::string> SelectorAutomation::getFirstIdSelector(const std::vector<SelectorInfo>& selectors) {
    for (const auto& selector : selectors) {
        if (selector.type == SelectorType::Id) {
            return selector.value;
        }
    }
    return std::nullopt;
}

bool SelectorAutomation::shouldAutoAddClass(bool hasClassAttribute, bool hasStyleClassSelector) const {
    return !config_.disableStyleAutoAddClass && !hasClassAttribute && hasStyleClassSelector;
}

bool SelectorAutomation::shouldAutoAddId(bool hasIdAttribute, bool hasStyleIdSelector) const {
    return !config_.disableStyleAutoAddId && !hasIdAttribute && hasStyleIdSelector;
}

bool SelectorAutomation::shouldScriptAutoAddClass(bool hasClassAttribute, bool hasStyleAutoAdded) const {
    return !config_.disableScriptAutoAddClass && !hasClassAttribute && !hasStyleAutoAdded;
}

bool SelectorAutomation::shouldScriptAutoAddId(bool hasIdAttribute, bool hasStyleAutoAdded) const {
    return !config_.disableScriptAutoAddId && !hasIdAttribute && !hasStyleAutoAdded;
}

std::string SelectorAutomation::resolveReferenceSelector(const std::string& selector,
                                                        const std::string& contextClass,
                                                        const std::string& contextId,
                                                        bool isInStyle) {
    SelectorReferenceResolver resolver;
    resolver.setContext(contextClass, contextId);
    
    // 对于局部style，& 优先选择class
    // 对于局部script，& 优先选择id
    std::string result = selector;
    
    if (result.find('&') != std::string::npos) {
        if (isInStyle && !contextClass.empty()) {
            // 替换 & 为 .class
            size_t pos = 0;
            while ((pos = result.find('&', pos)) != std::string::npos) {
                result.replace(pos, 1, "." + contextClass);
                pos += contextClass.length() + 1;
            }
        } else if (!isInStyle && !contextId.empty()) {
            // 替换 & 为 #id
            size_t pos = 0;
            while ((pos = result.find('&', pos)) != std::string::npos) {
                result.replace(pos, 1, "#" + contextId);
                pos += contextId.length() + 1;
            }
        }
    }
    
    return result;
}

std::string SelectorAutomation::processSelector(const std::string& selector,
                                              const std::string& autoClass,
                                              const std::string& autoId) {
    std::string processed = selector;
    
    // 处理引用选择器
    if (selector.find('&') != std::string::npos) {
        processed = resolveReferenceSelector(selector, autoClass, autoId);
    }
    
    return processed;
}

SelectorType SelectorAutomation::identifySelectorType(const std::string& selector) {
    if (selector.empty()) return SelectorType::Unknown;
    
    char first = selector[0];
    switch (first) {
        case '.':
            return SelectorType::Class;
        case '#':
            return SelectorType::Id;
        case '[':
            return SelectorType::Attribute;
        case ':':
            return SelectorType::Pseudo;
        case '*':
            return SelectorType::Universal;
        case '&':
            return SelectorType::Reference;
        default:
            // 检查是否是标签选择器
            if (std::isalpha(first)) {
                return SelectorType::Tag;
            }
            return SelectorType::Unknown;
    }
}

std::string SelectorAutomation::extractSelectorValue(const std::string& selector, SelectorType type) {
    switch (type) {
        case SelectorType::Class:
        case SelectorType::Id:
            return selector.substr(1); // 去掉 . 或 #
        case SelectorType::Tag:
            {
                // 提取标签名（可能包含[index]）
                size_t pos = selector.find('[');
                if (pos != std::string::npos) {
                    return selector.substr(0, pos);
                }
                return selector;
            }
        default:
            return selector;
    }
}

bool SelectorAutomation::isValidSelector(const std::string& selector) {
    if (selector.empty()) return false;
    
    // 简单的验证规则
    std::regex validRegex(R"(^[.#*]?[\w-]+(?:\[[^\]]+\])?(?::[^{]+)?$)");
    return std::regex_match(selector, validRegex);
}

std::string SelectorAutomation::normalizeSelector(const std::string& selector) {
    std::string normalized = selector;
    
    // 去除前后空白
    normalized.erase(0, normalized.find_first_not_of(" \t\n\r"));
    normalized.erase(normalized.find_last_not_of(" \t\n\r") + 1);
    
    return normalized;
}

// SelectorReferenceResolver实现
SelectorReferenceResolver::SelectorReferenceResolver() {}

void SelectorReferenceResolver::setContext(const std::string& elementClass, const std::string& elementId) {
    contextClass_ = elementClass;
    contextId_ = elementId;
}

std::string SelectorReferenceResolver::resolve(const std::string& selector) {
    if (!hasReference(selector)) {
        return selector;
    }
    
    return resolveCompound(selector);
}

std::string SelectorReferenceResolver::resolveCompound(const std::string& selector) {
    std::string result = selector;
    
    // 替换所有的 & 引用
    size_t pos = 0;
    while ((pos = result.find('&', pos)) != std::string::npos) {
        std::string replacement = resolveSingleReference("&", true);
        result.replace(pos, 1, replacement);
        pos += replacement.length();
    }
    
    return result;
}

bool SelectorReferenceResolver::hasReference(const std::string& selector) const {
    return selector.find('&') != std::string::npos;
}

SelectorType SelectorReferenceResolver::getReferencePreference(bool isInStyle) const {
    // 对于局部style，& 优先选择class
    // 对于局部script，& 优先选择id
    return isInStyle ? SelectorType::Class : SelectorType::Id;
}

std::string SelectorReferenceResolver::resolveSingleReference(const std::string& ref, bool isInStyle) {
    if (ref != "&") {
        return ref;
    }
    
    SelectorType preference = getReferencePreference(isInStyle);
    
    if (preference == SelectorType::Class && !contextClass_.empty()) {
        return "." + contextClass_;
    } else if (preference == SelectorType::Id && !contextId_.empty()) {
        return "#" + contextId_;
    }
    
    // 如果没有合适的上下文，返回原始引用
    return ref;
}

std::vector<std::string> SelectorReferenceResolver::splitSelector(const std::string& selector) {
    std::vector<std::string> parts;
    std::stringstream ss(selector);
    std::string part;
    
    // 简单的空格分割
    while (ss >> part) {
        parts.push_back(part);
    }
    
    return parts;
}

// CHTLJSSelectorProcessor实现
CHTLJSSelectorProcessor::CHTLJSSelectorProcessor() {}

std::string CHTLJSSelectorProcessor::processSelector(const std::string& selector) {
    if (!isCHTLJSSelector(selector)) {
        return selector;
    }
    
    // 提取选择器内容（去掉 {{ 和 }}）
    std::string content = selector.substr(2, selector.length() - 4);
    
    return toJavaScript(content);
}

std::vector<std::string> CHTLJSSelectorProcessor::extractSelectors(const std::string& code) {
    std::vector<std::string> selectors;
    
    std::regex selectorRegex(R"(\{\{([.#]?[\w-]+(?:\[\d+\])?)\}\})");
    auto begin = std::sregex_iterator(code.begin(), code.end(), selectorRegex);
    auto end = std::sregex_iterator();
    
    for (auto it = begin; it != end; ++it) {
        selectors.push_back((*it)[0]);
    }
    
    return selectors;
}

bool CHTLJSSelectorProcessor::isCHTLJSSelector(const std::string& str) const {
    return str.length() > 4 && str.substr(0, 2) == "{{" && str.substr(str.length() - 2) == "}}";
}

std::string CHTLJSSelectorProcessor::toJavaScript(const std::string& selector) {
    ParsedSelector parsed = parseSelector(selector);
    return generateQuerySelector(parsed);
}

SelectorType CHTLJSSelectorProcessor::getType(const std::string& selector) const {
    if (selector.empty()) return SelectorType::Unknown;
    
    if (selector[0] == '.') return SelectorType::Class;
    if (selector[0] == '#') return SelectorType::Id;
    if (std::isalpha(selector[0])) return SelectorType::Tag;
    
    return SelectorType::Unknown;
}

std::string CHTLJSSelectorProcessor::getValue(const std::string& selector) const {
    ParsedSelector parsed = parseSelector(selector);
    return parsed.value;
}

std::optional<int> CHTLJSSelectorProcessor::getIndex(const std::string& selector) const {
    ParsedSelector parsed = parseSelector(selector);
    return parsed.index;
}

CHTLJSSelectorProcessor::ParsedSelector CHTLJSSelectorProcessor::parseSelector(const std::string& selector) {
    ParsedSelector result;
    
    // 检查是否有索引
    size_t bracketPos = selector.find('[');
    std::string mainPart = selector;
    
    if (bracketPos != std::string::npos) {
        mainPart = selector.substr(0, bracketPos);
        
        // 提取索引
        size_t endBracket = selector.find(']', bracketPos);
        if (endBracket != std::string::npos) {
            std::string indexStr = selector.substr(bracketPos + 1, endBracket - bracketPos - 1);
            try {
                result.index = std::stoi(indexStr);
            } catch (...) {
                // 无效的索引
            }
        }
    }
    
    // 识别类型
    result.type = getType(mainPart);
    
    // 提取值
    switch (result.type) {
        case SelectorType::Class:
        case SelectorType::Id:
            result.value = mainPart.substr(1);
            break;
        case SelectorType::Tag:
            result.value = mainPart;
            break;
        default:
            result.value = mainPart;
    }
    
    return result;
}

std::string CHTLJSSelectorProcessor::generateQuerySelector(const ParsedSelector& parsed) {
    std::stringstream js;
    
    // 构建选择器字符串
    std::string selectorStr;
    switch (parsed.type) {
        case SelectorType::Class:
            selectorStr = "." + parsed.value;
            break;
        case SelectorType::Id:
            selectorStr = "#" + parsed.value;
            break;
        case SelectorType::Tag:
            selectorStr = parsed.value;
            break;
        default:
            selectorStr = parsed.value;
    }
    
    // 生成JavaScript代码
    if (parsed.index.has_value()) {
        // 如果有索引，使用querySelectorAll并访问特定索引
        js << "document.querySelectorAll('" << selectorStr << "')[" << parsed.index.value() << "]";
    } else {
        // 没有索引，使用querySelector
        js << "document.querySelector('" << selectorStr << "')";
    }
    
    return js.str();
}

// ReferenceRuleManager实现
void ReferenceRuleManager::registerRule(const std::string& name, const std::string& pattern) {
    rules_[name] = pattern;
}

std::optional<std::string> ReferenceRuleManager::getRule(const std::string& name) const {
    auto it = rules_.find(name);
    if (it != rules_.end()) {
        return it->second;
    }
    return std::nullopt;
}

std::string ReferenceRuleManager::applyRule(const std::string& name, const std::string& context) {
    auto ruleOpt = getRule(name);
    if (!ruleOpt) {
        return context;
    }
    
    // 应用规则（简单的字符串替换）
    std::string result = *ruleOpt;
    
    // 替换占位符
    size_t pos = 0;
    while ((pos = result.find("$context", pos)) != std::string::npos) {
        result.replace(pos, 8, context);
        pos += context.length();
    }
    
    return result;
}

bool ReferenceRuleManager::hasRule(const std::string& name) const {
    return rules_.find(name) != rules_.end();
}

void ReferenceRuleManager::removeRule(const std::string& name) {
    rules_.erase(name);
}

void ReferenceRuleManager::clearRules() {
    rules_.clear();
}

} // namespace CHTL