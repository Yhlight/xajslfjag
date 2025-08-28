#include "CHTLJSContext.h"
#include <algorithm>
#include <sstream>
#include <regex>
#include <iostream>

namespace CHTLJS {

CHTLJSContext::CHTLJSContext()
    : totalSelectorsProcessed_(0), totalEventListeners_(0), totalAnimations_(0),
      totalVirtualObjects_(0), totalModuleImports_(0) {
}

CHTLJSContext::~CHTLJSContext() = default;

std::shared_ptr<EnhancedSelector> CHTLJSContext::parseEnhancedSelector(const std::string& selectorText) {
    auto selector = std::make_shared<EnhancedSelector>();
    selector->selector = selectorText;
    
    // 移除{{}}包装
    std::string cleanSelector = selectorText;
    if (cleanSelector.length() >= 4 && 
        cleanSelector.substr(0, 2) == "{{" && 
        cleanSelector.substr(cleanSelector.length() - 2) == "}}") {
        cleanSelector = cleanSelector.substr(2, cleanSelector.length() - 4);
    }
    
    // 检查是否为索引选择器
    if (isIndexedSelector(cleanSelector)) {
        selector->isIndexed = true;
        selector->index = extractIndex(cleanSelector);
        // 移除索引部分
        size_t bracketPos = cleanSelector.find('[');
        if (bracketPos != std::string::npos) {
            cleanSelector = cleanSelector.substr(0, bracketPos);
        }
    }
    
    // 检查是否包含后代选择器（优先级最高）
    size_t spacePos = cleanSelector.find(' ');
    if (spacePos != std::string::npos) {
        selector->parentSelector = cleanSelector.substr(0, spacePos);
        std::string childSelector = cleanSelector.substr(spacePos + 1);
        
        if (childSelector[0] == '.') {
            selector->elementType = "class";
            selector->elementValue = childSelector.substr(1);
        } else if (childSelector[0] == '#') {
            selector->elementType = "id";
            selector->elementValue = childSelector.substr(1);
        } else {
            selector->elementType = "tag";
            selector->elementValue = childSelector;
        }
    } else {
        // 解析选择器类型（单个选择器）
        if (cleanSelector[0] == '.') {
            selector->elementType = "class";
            selector->elementValue = cleanSelector.substr(1);
        } else if (cleanSelector[0] == '#') {
            selector->elementType = "id";
            selector->elementValue = cleanSelector.substr(1);
        } else {
            selector->elementType = "tag";
            selector->elementValue = cleanSelector;
        }
    }
    
    totalSelectorsProcessed_++;
    return selector;
}

std::string CHTLJSContext::createDOMReference(const std::shared_ptr<EnhancedSelector>& selector) {
    if (!selector) {
        return "null";
    }
    
    std::stringstream ss;
    
    if (selector->elementType == "class") {
        ss << "document.querySelector('." << selector->elementValue << "')";
    } else if (selector->elementType == "id") {
        ss << "document.getElementById('" << selector->elementValue << "')";
    } else if (selector->elementType == "tag") {
        if (selector->isIndexed) {
            ss << "document.querySelectorAll('" << selector->elementValue << "')[" << selector->index << "]";
        } else {
            ss << "document.querySelectorAll('" << selector->elementValue << "')";
        }
    }
    
    // 如果有父选择器，需要先查找父元素
    if (!selector->parentSelector.empty()) {
        std::string parentRef;
        if (selector->parentSelector[0] == '.') {
            parentRef = "document.querySelector('" + selector->parentSelector + "')";
        } else if (selector->parentSelector[0] == '#') {
            parentRef = "document.getElementById('" + selector->parentSelector.substr(1) + "')";
        } else {
            parentRef = "document.querySelector('" + selector->parentSelector + "')";
        }
        
        if (selector->elementType == "class") {
            return parentRef + ".querySelector('." + selector->elementValue + "')";
        } else if (selector->elementType == "id") {
            return parentRef + ".querySelector('#" + selector->elementValue + "')";
        } else {
            if (selector->isIndexed) {
                return parentRef + ".querySelectorAll('" + selector->elementValue + "')[" + std::to_string(selector->index) + "]";
            } else {
                return parentRef + ".querySelectorAll('" + selector->elementValue + "')";
            }
        }
    }
    
    return ss.str();
}

std::string CHTLJSContext::parseEventBindingOperator(const std::string& selector, const std::string& eventType, const std::string& callback) {
    std::stringstream ss;
    
    // 解析选择器
    auto enhancedSelector = parseEnhancedSelector(selector);
    std::string domRef = createDOMReference(enhancedSelector);
    
    // 生成事件绑定代码
    ss << domRef << ".addEventListener('" << eventType << "', " << callback << ");";
    
    totalEventListeners_++;
    return ss.str();
}

std::string CHTLJSContext::parseEnhancedListener(const std::string& selector, const std::vector<EventListener>& listeners) {
    std::stringstream ss;
    
    // 解析选择器
    auto enhancedSelector = parseEnhancedSelector(selector);
    std::string domRef = createDOMReference(enhancedSelector);
    
    // 生成监听器代码
    ss << "// Enhanced listener for " << selector << "\n";
    ss << "const element = " << domRef << ";\n";
    
    for (const auto& listener : listeners) {
        ss << "element.addEventListener('" << listener.eventType << "', ";
        
        if (listener.isFunction) {
            ss << listener.callback;
        } else if (listener.isReference) {
            ss << listener.callback;
        } else {
            ss << "function() { " << listener.callback << " }";
        }
        
        ss << ");\n";
    }
    
    totalEventListeners_++;
    return ss.str();
}

std::string CHTLJSContext::parseEventDelegation(const EventDelegation& delegation) {
    std::stringstream ss;
    
    // 生成事件委托代码
    ss << "// Event delegation for " << delegation.parentSelector << "\n";
    ss << "const parentElement = document.querySelector('" << delegation.parentSelector << "');\n";
    ss << "if (parentElement) {\n";
    
    // 为每个事件类型生成处理器
    for (const auto& eventHandler : delegation.eventHandlers) {
        ss << "  parentElement.addEventListener('" << eventHandler.first << "', function(event) {\n";
        ss << "    const target = event.target;\n";
        
        // 检查目标是否匹配
        for (const auto& targetSelector : delegation.targetSelectors) {
            ss << "    if (target.matches('" << targetSelector << "')) {\n";
            ss << "      " << eventHandler.second << "\n";
            ss << "    }\n";
        }
        
        ss << "  });\n";
    }
    
    ss << "}\n";
    
    return ss.str();
}

std::string CHTLJSContext::parseAnimation(const AnimationConfig& config) {
    std::stringstream ss;
    
    // 生成动画代码
    ss << "// Animation for " << config.target << "\n";
    ss << "const animation = {\n";
    ss << "  target: " << config.target << ",\n";
    ss << "  duration: " << config.duration << ",\n";
    ss << "  easing: '" << config.easing << "',\n";
    
    // 起始状态
    if (!config.beginState.empty()) {
        ss << "  begin: {\n";
        for (const auto& property : config.beginState) {
            ss << "    " << property.first << ": '" << property.second << "',\n";
        }
        ss << "  },\n";
    }
    
    // 关键帧
    if (!config.keyframes.empty()) {
        ss << "  keyframes: [\n";
        for (const auto& keyframe : config.keyframes) {
            ss << "    {\n";
            ss << "      offset: " << keyframe.at << ",\n";
            for (const auto& property : keyframe.cssProperties) {
                ss << "      " << property.first << ": '" << property.second << "',\n";
            }
            ss << "    },\n";
        }
        ss << "  ],\n";
    }
    
    // 结束状态
    if (!config.endState.empty()) {
        ss << "  end: {\n";
        for (const auto& property : config.endState) {
            ss << "    " << property.first << ": '" << property.second << "',\n";
        }
        ss << "  },\n";
    }
    
    // 其他配置
    ss << "  loop: " << config.loop << ",\n";
    if (!config.direction.empty()) {
        ss << "  direction: '" << config.direction << "',\n";
    }
    ss << "  delay: " << config.delay << ",\n";
    if (!config.callback.empty()) {
        ss << "  callback: " << config.callback << ",\n";
    }
    
    ss << "};\n";
    ss << "const anim = requestAnimationFrame(() => {\n";
    ss << "  // Animation implementation\n";
    ss << "});\n";
    
    totalAnimations_++;
    return ss.str();
}

std::shared_ptr<VirtualObject> CHTLJSContext::createVirtualObject(const std::string& name, const std::string& sourceCode) {
    auto virtualObject = std::make_shared<VirtualObject>();
    virtualObject->name = name;
    virtualObject->sourceCode = sourceCode;
    
    // 解析源代码，提取属性和方法
    // 这里简化实现，实际应该解析CHTL JS语法
    std::regex propertyPattern(R"((\w+):\s*([^,\n]+))");
    std::sregex_iterator iter(sourceCode.begin(), sourceCode.end(), propertyPattern);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        std::string key = (*iter)[1];
        std::string value = (*iter)[2];
        
        // 判断是否为函数
        if (value.find("()") != std::string::npos || value.find("=>") != std::string::npos) {
            virtualObject->methods[key] = value;
        } else {
            virtualObject->properties[key] = CHTLJSValue(CHTLJSValueType::STRING, value);
        }
    }
    
    virtualObjects_[name] = virtualObject;
    totalVirtualObjects_++;
    return virtualObject;
}

std::string CHTLJSContext::parseVirtualObjectAccess(const std::string& objectName, const std::string& propertyName) {
    auto it = virtualObjects_.find(objectName);
    if (it == virtualObjects_.end()) {
        return "// Virtual object '" + objectName + "' not found";
    }
    
    const auto& virtualObject = it->second;
    
    // 检查是否为方法
    auto methodIt = virtualObject->methods.find(propertyName);
    if (methodIt != virtualObject->methods.end()) {
        return "// Method call: " + objectName + "." + propertyName + "()";
    }
    
    // 检查是否为属性
    auto propertyIt = virtualObject->properties.find(propertyName);
    if (propertyIt != virtualObject->properties.end()) {
        return "// Property access: " + objectName + "." + propertyName;
    }
    
    return "// Property '" + propertyName + "' not found in virtual object '" + objectName + "'";
}

std::string CHTLJSContext::parseModuleImports(const std::vector<ModuleImport>& imports) {
    std::stringstream ss;
    
    ss << "// Module imports\n";
    for (const auto& import : imports) {
        ss << "import '" << import.path << "'";
        if (!import.alias.empty()) {
            ss << " as " << import.alias;
        }
        ss << ";\n";
    }
    
    totalModuleImports_++;
    return ss.str();
}

bool CHTLJSContext::validateSelector(const std::string& selectorText) const {
    if (selectorText.empty()) {
        return false;
    }
    
    // 检查是否被{{}}包围
    if (selectorText.length() < 4) {
        return false;
    }
    
    if (selectorText.substr(0, 2) != "{{" || 
        selectorText.substr(selectorText.length() - 2) != "}}") {
        return false;
    }
    
    // 提取内部选择器
    std::string innerSelector = selectorText.substr(2, selectorText.length() - 4);
    
    // 基本语法验证
    std::regex validSelector("^[.#]?[a-zA-Z_][a-zA-Z0-9_-]*$|^[a-zA-Z_][a-zA-Z0-9_-]*$|^[.#]?[a-zA-Z_][a-zA-Z0-9_-]*\\s+[.#]?[a-zA-Z_][a-zA-Z0-9_-]*$|^[.#]?[a-zA-Z_][a-zA-Z0-9_-]*\\[\\d+\\]$");
    
    return std::regex_match(innerSelector, validSelector);
}

std::string CHTLJSContext::getSelectorType(const std::string& selectorText) const {
    if (!validateSelector(selectorText)) {
        return "invalid";
    }
    
    std::string innerSelector = selectorText.substr(2, selectorText.length() - 4);
    
    // 检查是否包含后代选择器（优先级最高）
    if (innerSelector.find(' ') != std::string::npos) {
        return "descendant";
    } else if (isIndexedSelector(innerSelector)) {
        return "indexed";
    } else if (innerSelector[0] == '.') {
        return "class";
    } else if (innerSelector[0] == '#') {
        return "id";
    } else {
        return "tag";
    }
}

std::string CHTLJSContext::generateJavaScript(const std::string& context) {
    std::stringstream ss;
    
    ss << "// Generated JavaScript from CHTL JS\n";
    ss << "// Context: " << context << "\n\n";
    
    // 生成模块导入代码
    if (!moduleImports_.empty()) {
        ss << parseModuleImports(moduleImports_);
        ss << "\n";
    }
    
    // 生成事件委托代码
    for (const auto& delegation : eventDelegations_) {
        ss << parseEventDelegation(delegation);
        ss << "\n";
    }
    
    return ss.str();
}

std::string CHTLJSContext::getStatistics() const {
    std::stringstream ss;
    ss << "CHTL JS 上下文统计信息:\n";
    ss << "总选择器处理数量: " << totalSelectorsProcessed_ << "\n";
    ss << "总事件监听器数量: " << totalEventListeners_ << "\n";
    ss << "总动画数量: " << totalAnimations_ << "\n";
    ss << "总虚对象数量: " << totalVirtualObjects_ << "\n";
    ss << "总模块导入数量: " << totalModuleImports_ << "\n";
    ss << "\n虚对象列表:\n";
    
    for (const auto& pair : virtualObjects_) {
        const auto& virtualObject = pair.second;
        ss << "  " << virtualObject->name << " (属性: " << virtualObject->properties.size() 
           << ", 方法: " << virtualObject->methods.size() << ")\n";
    }
    
    return ss.str();
}

void CHTLJSContext::clearStatistics() {
    totalSelectorsProcessed_ = 0;
    totalEventListeners_ = 0;
    totalAnimations_ = 0;
    totalVirtualObjects_ = 0;
    totalModuleImports_ = 0;
}

std::vector<std::string> CHTLJSContext::parseSelectorComponents(const std::string& selectorText) const {
    std::vector<std::string> components;
    std::stringstream ss(selectorText);
    std::string component;
    
    while (std::getline(ss, component, ' ')) {
        if (!component.empty()) {
            components.push_back(component);
        }
    }
    
    return components;
}

bool CHTLJSContext::isIndexedSelector(const std::string& selectorText) const {
    return selectorText.find('[') != std::string::npos && 
           selectorText.find(']') != std::string::npos;
}

int CHTLJSContext::extractIndex(const std::string& selectorText) const {
    size_t startPos = selectorText.find('[');
    size_t endPos = selectorText.find(']');
    
    if (startPos != std::string::npos && endPos != std::string::npos && endPos > startPos) {
        std::string indexStr = selectorText.substr(startPos + 1, endPos - startPos - 1);
        try {
            return std::stoi(indexStr);
        } catch (...) {
            return 0;
        }
    }
    
    return 0;
}

void CHTLJSContext::updateStatistics(const std::string& type) {
    if (type == "selector") {
        totalSelectorsProcessed_++;
    } else if (type == "event") {
        totalEventListeners_++;
    } else if (type == "animation") {
        totalAnimations_++;
    } else if (type == "virtual") {
        totalVirtualObjects_++;
    } else if (type == "module") {
        totalModuleImports_++;
    }
}

} // namespace CHTLJS