#include "SelectorAutomation.h"
#include <algorithm>
#include <sstream>
#include <regex>

namespace CHTL {

SelectorAutomation::SelectorAutomation(std::shared_ptr<ConfigurationManager> configManager)
    : configManager_(configManager), totalSelectorsProcessed_(0), autoAttributesGenerated_(0),
      styleBlockAutomations_(0), scriptBlockAutomations_(0) {
    loadAutomationRuleFromConfig();
}

SelectorAutomation::~SelectorAutomation() = default;

std::shared_ptr<Selector> SelectorAutomation::parseSelector(const std::string& selectorText, int lineNumber, int columnNumber) {
    auto selector = std::make_shared<Selector>();
    selector->fullSelector = selectorText;
    selector->lineNumber = lineNumber;
    selector->columnNumber = columnNumber;
    selector->type = getSelectorType(selectorText);
    selector->value = extractSelectorValue(selectorText);
    
    totalSelectorsProcessed_++;
    return selector;
}

bool SelectorAutomation::processElementAutomation(std::shared_ptr<SelectorContext>& context) {
    if (!context) {
        return false;
    }
    
    // 处理样式块自动化
    if (context->hasStyleBlock) {
        processStyleBlockAutomation(context);
    }
    
    // 处理脚本块自动化
    if (context->hasScriptBlock) {
        processScriptBlockAutomation(context);
    }
    
    return true;
}

bool SelectorAutomation::processStyleBlockAutomation(std::shared_ptr<SelectorContext>& context) {
    if (!context || !context->hasStyleBlock) {
        return false;
    }
    
    // 查找类选择器
    for (const auto& selector : context->selectors) {
        if (selector->type == SelectorType::CLASS && !automationRule_.disableStyleAutoAddClass) {
            if (context->className.empty()) {
                context->className = selector->value;
                autoAttributesGenerated_++;
            }
            break; // 只添加第一个类选择器
        }
    }
    
    // 查找ID选择器
    for (const auto& selector : context->selectors) {
        if (selector->type == SelectorType::ID && !automationRule_.disableStyleAutoAddId) {
            if (context->idName.empty()) {
                context->idName = selector->value;
                autoAttributesGenerated_++;
            }
            break; // 只添加第一个ID选择器
        }
    }
    
    styleBlockAutomations_++;
    return true;
}

bool SelectorAutomation::processScriptBlockAutomation(std::shared_ptr<SelectorContext>& context) {
    if (!context || !context->hasScriptBlock) {
        return false;
    }
    
    // 如果样式块没有触发类自动化添加，且脚本块中有类选择器
    if (context->className.empty() && !automationRule_.disableScriptAutoAddClass) {
        for (const auto& selector : context->selectors) {
            if (selector->type == SelectorType::CLASS) {
                context->className = selector->value;
                autoAttributesGenerated_++;
                break;
            }
        }
    }
    
    // 如果样式块没有触发ID自动化添加，且脚本块中有ID选择器
    if (context->idName.empty() && !automationRule_.disableScriptAutoAddId) {
        for (const auto& selector : context->selectors) {
            if (selector->type == SelectorType::ID) {
                context->idName = selector->value;
                autoAttributesGenerated_++;
                break;
            }
        }
    }
    
    scriptBlockAutomations_++;
    return true;
}

std::string SelectorAutomation::resolveReferenceSelector(const std::string& selectorText, const std::shared_ptr<SelectorContext>& context) {
    if (!context) {
        return selectorText;
    }
    
    std::string result = selectorText;
    
    // 处理 & 引用选择器
    if (result.find("&") != std::string::npos) {
        // 对于样式块，& 优先选择类名
        if (context->hasStyleBlock && !context->className.empty()) {
            std::regex regex("&");
            result = std::regex_replace(result, regex, "." + context->className);
        }
        // 对于脚本块，& 优先选择ID
        else if (context->hasScriptBlock && !context->idName.empty()) {
            std::regex regex("&");
            result = std::regex_replace(result, regex, "#" + context->idName);
        }
        // 如果都没有，使用类名（如果有的话）
        else if (!context->className.empty()) {
            std::regex regex("&");
            result = std::regex_replace(result, regex, "." + context->className);
        }
        // 最后的选择：使用ID
        else if (!context->idName.empty()) {
            std::regex regex("&");
            result = std::regex_replace(result, regex, "#" + context->idName);
        }
    }
    
    return result;
}

const AutomationRule& SelectorAutomation::getAutomationRule() const {
    return automationRule_;
}

void SelectorAutomation::setAutomationRule(const AutomationRule& rule) {
    automationRule_ = rule;
}

bool SelectorAutomation::loadAutomationRuleFromConfig(const std::string& configGroupName) {
    if (!configManager_) {
        return false;
    }
    
    auto config = configManager_->getConfigurationGroup(configGroupName);
    if (!config) {
        return false;
    }
    
    // 加载自动化规则
    auto disableStyleAutoAddClass = configManager_->getConfig(configGroupName, "DISABLE_STYLE_AUTO_ADD_CLASS");
    if (disableStyleAutoAddClass.type == ConfigValueType::BOOLEAN) {
        automationRule_.disableStyleAutoAddClass = disableStyleAutoAddClass.boolValue;
    }
    
    auto disableStyleAutoAddId = configManager_->getConfig(configGroupName, "DISABLE_STYLE_AUTO_ADD_ID");
    if (disableStyleAutoAddId.type == ConfigValueType::BOOLEAN) {
        automationRule_.disableStyleAutoAddId = disableStyleAutoAddId.boolValue;
    }
    
    auto disableScriptAutoAddClass = configManager_->getConfig(configGroupName, "DISABLE_SCRIPT_AUTO_ADD_CLASS");
    if (disableScriptAutoAddClass.type == ConfigValueType::BOOLEAN) {
        automationRule_.disableScriptAutoAddClass = disableScriptAutoAddClass.boolValue;
    }
    
    auto disableScriptAutoAddId = configManager_->getConfig(configGroupName, "DISABLE_SCRIPT_AUTO_ADD_ID");
    if (disableScriptAutoAddId.type == ConfigValueType::BOOLEAN) {
        automationRule_.disableScriptAutoAddId = disableScriptAutoAddId.boolValue;
    }
    
    return true;
}

bool SelectorAutomation::validateSelector(const std::string& selectorText) const {
    if (selectorText.empty()) {
        return false;
    }
    
    // 基本语法验证
    std::regex validSelector("^[.#]?[a-zA-Z_][a-zA-Z0-9_-]*$|^[a-zA-Z_][a-zA-Z0-9_-]*$|^:[a-zA-Z_][a-zA-Z0-9_-]*$|^::[a-zA-Z_][a-zA-Z0-9_-]*$|^\\*$");
    
    return std::regex_match(selectorText, validSelector);
}

SelectorType SelectorAutomation::getSelectorType(const std::string& selectorText) const {
    if (isClassSelector(selectorText)) {
        return SelectorType::CLASS;
    } else if (isIdSelector(selectorText)) {
        return SelectorType::ID;
    } else if (isPseudoClassSelector(selectorText)) {
        return SelectorType::PSEUDO_CLASS;
    } else if (isPseudoElementSelector(selectorText)) {
        return SelectorType::PSEUDO_ELEMENT;
    } else if (isTagSelector(selectorText)) {
        return SelectorType::TAG;
    } else if (isCombinatorSelector(selectorText)) {
        return SelectorType::COMBINATOR;
    } else if (isUniversalSelector(selectorText)) {
        return SelectorType::UNIVERSAL;
    }
    
    return SelectorType::TAG; // 默认为标签选择器
}

std::string SelectorAutomation::extractSelectorValue(const std::string& selectorText) const {
    if (selectorText.empty()) {
        return "";
    }
    
    // 移除前缀符号
    if (selectorText[0] == '.') {
        return selectorText.substr(1);
    } else if (selectorText[0] == '#') {
        return selectorText.substr(1);
    } else if (selectorText[0] == ':') {
        if (selectorText.length() > 1 && selectorText[1] == ':') {
            return selectorText.substr(2); // 伪元素
        } else {
            return selectorText.substr(1); // 伪类
        }
    }
    
    return selectorText;
}

bool SelectorAutomation::needsAutoAttribute(const std::shared_ptr<SelectorContext>& context, SelectorType selectorType) const {
    if (!context) {
        return false;
    }
    
    switch (selectorType) {
        case SelectorType::CLASS:
            return context->className.empty();
        case SelectorType::ID:
            return context->idName.empty();
        default:
            return false;
    }
}

std::string SelectorAutomation::generateAutoAttribute(const std::shared_ptr<SelectorContext>& context, SelectorType selectorType) const {
    if (!context) {
        return "";
    }
    
    switch (selectorType) {
        case SelectorType::CLASS:
            if (context->className.empty()) {
                // 查找第一个类选择器
                for (const auto& selector : context->selectors) {
                    if (selector->type == SelectorType::CLASS) {
                        return "class: " + selector->value + ";";
                    }
                }
            }
            break;
        case SelectorType::ID:
            if (context->idName.empty()) {
                // 查找第一个ID选择器
                for (const auto& selector : context->selectors) {
                    if (selector->type == SelectorType::ID) {
                        return "id: " + selector->value + ";";
                    }
                }
            }
            break;
        default:
            break;
    }
    
    return "";
}

int SelectorAutomation::getSelectorPriority(const std::shared_ptr<Selector>& selector) const {
    if (!selector) {
        return 0;
    }
    
    // 选择器优先级规则（数值越大优先级越高）
    switch (selector->type) {
        case SelectorType::ID:
            return 100;
        case SelectorType::CLASS:
        case SelectorType::PSEUDO_CLASS:
            return 10;
        case SelectorType::PSEUDO_ELEMENT:
            return 1;
        case SelectorType::TAG:
            return 1;
        case SelectorType::COMBINATOR:
            return 5;
        case SelectorType::UNIVERSAL:
            return 0;
        default:
            return 0;
    }
}

void SelectorAutomation::sortSelectorsByPriority(std::vector<std::shared_ptr<Selector>>& selectors) const {
    std::sort(selectors.begin(), selectors.end(), 
        [this](const std::shared_ptr<Selector>& a, const std::shared_ptr<Selector>& b) {
            return getSelectorPriority(a) > getSelectorPriority(b);
        });
}

std::string SelectorAutomation::getStatistics() const {
    std::stringstream ss;
    ss << "选择器自动化统计信息:\n";
    ss << "总选择器处理数量: " << totalSelectorsProcessed_ << "\n";
    ss << "自动属性生成数量: " << autoAttributesGenerated_ << "\n";
    ss << "样式块自动化数量: " << styleBlockAutomations_ << "\n";
    ss << "脚本块自动化数量: " << scriptBlockAutomations_ << "\n";
    ss << "\n自动化规则:\n";
    ss << "  禁止样式块自动添加类选择器: " << (automationRule_.disableStyleAutoAddClass ? "是" : "否") << "\n";
    ss << "  禁止样式块自动添加ID选择器: " << (automationRule_.disableStyleAutoAddId ? "是" : "否") << "\n";
    ss << "  禁止脚本块自动添加类选择器: " << (automationRule_.disableScriptAutoAddClass ? "是" : "否") << "\n";
    ss << "  禁止脚本块自动添加ID选择器: " << (automationRule_.disableScriptAutoAddId ? "是" : "否") << "\n";
    
    return ss.str();
}

void SelectorAutomation::clearStatistics() {
    totalSelectorsProcessed_ = 0;
    autoAttributesGenerated_ = 0;
    styleBlockAutomations_ = 0;
    scriptBlockAutomations_ = 0;
}

std::vector<std::string> SelectorAutomation::parseCSSSelector(const std::string& selectorText) const {
    std::vector<std::string> selectors;
    std::stringstream ss(selectorText);
    std::string selector;
    
    while (std::getline(ss, selector, ',')) {
        // 去除空白字符
        selector.erase(0, selector.find_first_not_of(" \t"));
        selector.erase(selector.find_last_not_of(" \t") + 1);
        
        if (!selector.empty()) {
            selectors.push_back(selector);
        }
    }
    
    return selectors;
}

bool SelectorAutomation::isClassSelector(const std::string& selectorText) const {
    return !selectorText.empty() && selectorText[0] == '.';
}

bool SelectorAutomation::isIdSelector(const std::string& selectorText) const {
    return !selectorText.empty() && selectorText[0] == '#';
}

bool SelectorAutomation::isPseudoClassSelector(const std::string& selectorText) const {
    return selectorText.length() > 1 && selectorText[0] == ':' && selectorText[1] != ':';
}

bool SelectorAutomation::isPseudoElementSelector(const std::string& selectorText) const {
    return selectorText.length() > 2 && selectorText[0] == ':' && selectorText[1] == ':';
}

bool SelectorAutomation::isTagSelector(const std::string& selectorText) const {
    if (selectorText.empty()) {
        return false;
    }
    
    // 标签选择器：以字母开头，不包含特殊字符
    return std::isalpha(selectorText[0]) && 
           selectorText.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_-") == std::string::npos;
}

bool SelectorAutomation::isCombinatorSelector(const std::string& selectorText) const {
    // 组合选择器：包含空格、>、+、~等组合符，或者包含多个选择器（如div.box）
    if (selectorText.find(' ') != std::string::npos ||
        selectorText.find('>') != std::string::npos ||
        selectorText.find('+') != std::string::npos ||
        selectorText.find('~') != std::string::npos) {
        return true;
    }
    
    // 检查是否包含多个选择器（如div.box, .box#header等）
    // 这种情况：标签选择器 + 类选择器，或者类选择器 + ID选择器等
    bool hasTag = false, hasClass = false, hasId = false, hasPseudo = false;
    
    // 检查是否以字母开头（标签选择器）
    if (!selectorText.empty() && std::isalpha(selectorText[0])) {
        hasTag = true;
    }
    
    // 检查是否包含类选择器
    if (selectorText.find('.') != std::string::npos) {
        hasClass = true;
    }
    
    // 检查是否包含ID选择器
    if (selectorText.find('#') != std::string::npos) {
        hasId = true;
    }
    
    // 检查是否包含伪类或伪元素选择器
    if (selectorText.find(':') != std::string::npos) {
        hasPseudo = true;
    }
    
    // 如果包含多种类型的选择器，则为组合选择器
    int typeCount = (hasTag ? 1 : 0) + (hasClass ? 1 : 0) + (hasId ? 1 : 0) + (hasPseudo ? 1 : 0);
    return typeCount > 1;
}

bool SelectorAutomation::isUniversalSelector(const std::string& selectorText) const {
    return selectorText == "*";
}

void SelectorAutomation::updateStatistics(const std::string& type) {
    if (type == "selector") {
        totalSelectorsProcessed_++;
    } else if (type == "attribute") {
        autoAttributesGenerated_++;
    } else if (type == "style") {
        styleBlockAutomations_++;
    } else if (type == "script") {
        scriptBlockAutomations_++;
    }
}

} // namespace CHTL