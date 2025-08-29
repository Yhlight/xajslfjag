#include "ScriptNode.h"
#include <regex>
#include <sstream>
#include <algorithm>

namespace CHTL {

// ========== CHTLJSSelector 实现 ==========

std::string CHTLJSSelector::toString() const {
    switch (type) {
        case SelectorType::TAG:
            return "{{" + selector + "}}";
        case SelectorType::CLASS:
            return "{{." + selector + "}}";
        case SelectorType::ID:
            return "{{#" + selector + "}}";
        case SelectorType::COMPLEX:
            return "{{" + selector + "}}";
        case SelectorType::INDEXED:
            return "{{" + selector + "[" + std::to_string(index) + "]}}";
        case SelectorType::REFERENCE:
            return "{{&}}";
        default:
            return "{{" + selector + "}}";
    }
}

// ========== ScriptNode 实现 ==========

ScriptNode::ScriptNode(ScriptType type) 
    : BaseNode(CHTLNodeType::SCRIPT_NODE, "script"), 
      m_scriptType(type), 
      m_containsCHTLJS(false) {
}

ScriptType ScriptNode::getScriptType() const {
    return m_scriptType;
}

void ScriptNode::setScriptType(ScriptType type) {
    m_scriptType = type;
}

const std::string& ScriptNode::getContent() const {
    return m_content;
}

void ScriptNode::setContent(const std::string& content) {
    m_content = content;
    parseCHTLJSSelectors();
}

void ScriptNode::addLine(const std::string& line) {
    if (!m_content.empty()) {
        m_content += "\n";
    }
    m_content += line;
    parseCHTLJSSelectors();
}

const std::vector<CHTLJSSelector>& ScriptNode::getReferencedSelectors() const {
    return m_referencedSelectors;
}

void ScriptNode::addReferencedSelector(const CHTLJSSelector& selector) {
    m_referencedSelectors.push_back(selector);
}

bool ScriptNode::containsCHTLJS() const {
    return m_containsCHTLJS;
}

void ScriptNode::setContainsCHTLJS(bool contains) {
    m_containsCHTLJS = contains;
}

const std::vector<std::string>& ScriptNode::getAutoAddedClasses() const {
    return m_autoAddedClasses;
}

const std::vector<std::string>& ScriptNode::getAutoAddedIds() const {
    return m_autoAddedIds;
}

void ScriptNode::addAutoClass(const std::string& className) {
    if (std::find(m_autoAddedClasses.begin(), m_autoAddedClasses.end(), className) == m_autoAddedClasses.end()) {
        m_autoAddedClasses.push_back(className);
    }
}

void ScriptNode::addAutoId(const std::string& idName) {
    if (std::find(m_autoAddedIds.begin(), m_autoAddedIds.end(), idName) == m_autoAddedIds.end()) {
        m_autoAddedIds.push_back(idName);
    }
}

void ScriptNode::processSelectorAutomation() {
    for (const auto& selector : m_referencedSelectors) {
        if (selector.type == SelectorType::CLASS) {
            addAutoClass(selector.selector);
        } else if (selector.type == SelectorType::ID) {
            addAutoId(selector.selector);
        }
    }
}

std::string ScriptNode::convertToStandardJS() const {
    std::string result = m_content;
    
    // 转换{{selector}}语法为标准DOM选择器
    std::regex selectorRegex(R"(\{\{([^}]+)\}\})");
    std::sregex_iterator iter(result.begin(), result.end(), selectorRegex);
    std::sregex_iterator end;
    
    std::vector<std::pair<std::string, std::string>> replacements;
    
    for (; iter != end; ++iter) {
        std::string match = iter->str();
        std::string selectorText = (*iter)[1].str();
        
        CHTLJSSelector selector = parseSingleSelector(selectorText);
        std::string jsSelector = convertSelectorToJS(selector);
        
        replacements.push_back({match, jsSelector});
    }
    
    // 执行替换
    for (const auto& replacement : replacements) {
        size_t pos = result.find(replacement.first);
        while (pos != std::string::npos) {
            result.replace(pos, replacement.first.length(), replacement.second);
            pos = result.find(replacement.first, pos + replacement.second.length());
        }
    }
    
    // 转换&->语法为标准的addEventListener（简化处理）
    std::regex eventBindingRegex(R"(\{\{([^}]+)\}\}\s*&->\s*(\w+)\s*\{([^}]*)\})");
    // 由于lambda函数复杂，这里简化处理
    // 实际应该使用std::regex_replace的回调形式
    
    // 转换->操作符为标准的点操作符（简化处理）
    // 将{{selector}}->method替换为document.querySelector('selector').method
    std::regex arrowOperatorRegex(R"(\{\{([^}]+)\}\}\s*->\s*(\w+))");
    // 简化处理，不使用lambda
    
    return result;
}

void ScriptNode::parseCHTLJSSelectors() {
    m_referencedSelectors.clear();
    m_containsCHTLJS = false;
    
    // 使用正则表达式查找{{selector}}模式
    std::regex selectorRegex(R"(\{\{([^}]+)\}\})");
    std::sregex_iterator iter(m_content.begin(), m_content.end(), selectorRegex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        m_containsCHTLJS = true;
        std::string selectorText = (*iter)[1].str();
        CHTLJSSelector selector = parseSingleSelector(selectorText);
        m_referencedSelectors.push_back(selector);
    }
    
    // 检查其他CHTL JS语法
    if (m_content.find("->") != std::string::npos ||
        m_content.find("&->") != std::string::npos ||
        m_content.find("listen") != std::string::npos ||
        m_content.find("animate") != std::string::npos ||
        m_content.find("delegate") != std::string::npos) {
        m_containsCHTLJS = true;
    }
}

CHTLJSSelector ScriptNode::parseSingleSelector(const std::string& selectorText) const {
    std::string trimmed = selectorText;
    
    // 移除首尾空格
    trimmed.erase(0, trimmed.find_first_not_of(' '));
    trimmed.erase(trimmed.find_last_not_of(' ') + 1);
    
    // 检查引用选择器
    if (trimmed == "&") {
        return CHTLJSSelector(SelectorType::REFERENCE, "&");
    }
    
    // 检查索引选择器 tag[index]
    std::regex indexRegex(R"((\w+)\[(\d+)\])");
    std::smatch indexMatch;
    if (std::regex_match(trimmed, indexMatch, indexRegex)) {
        std::string tag = indexMatch[1].str();
        int index = std::stoi(indexMatch[2].str());
        return CHTLJSSelector(SelectorType::INDEXED, tag, index);
    }
    
    // 检查类选择器
    if (trimmed.front() == '.') {
        return CHTLJSSelector(SelectorType::CLASS, trimmed.substr(1));
    }
    
    // 检查ID选择器
    if (trimmed.front() == '#') {
        return CHTLJSSelector(SelectorType::ID, trimmed.substr(1));
    }
    
    // 检查复合选择器（包含空格）
    if (trimmed.find(' ') != std::string::npos) {
        return CHTLJSSelector(SelectorType::COMPLEX, trimmed);
    }
    
    // 默认为标签选择器
    return CHTLJSSelector(SelectorType::TAG, trimmed);
}

std::string ScriptNode::convertSelectorToJS(const CHTLJSSelector& selector) const {
    switch (selector.type) {
        case SelectorType::TAG:
            return "document.querySelector('" + selector.selector + "')";
        case SelectorType::CLASS:
            return "document.querySelector('." + selector.selector + "')";
        case SelectorType::ID:
            return "document.querySelector('#" + selector.selector + "')";
        case SelectorType::COMPLEX:
            return "document.querySelector('" + selector.selector + "')";
        case SelectorType::INDEXED:
            return "document.querySelectorAll('" + selector.selector + "')[" + std::to_string(selector.index) + "]";
        case SelectorType::REFERENCE:
            // &引用需要根据上下文确定，这里返回一个占位符
            return "/* & reference needs context resolution */";
        default:
            return "document.querySelector('" + selector.selector + "')";
    }
}

std::string ScriptNode::toString() const {
    std::stringstream ss;
    ss << "script {\n";
    
    if (m_containsCHTLJS) {
        ss << "    // CHTL JS Extended Syntax\n";
    }
    
    std::istringstream contentStream(m_content);
    std::string line;
    while (std::getline(contentStream, line)) {
        ss << "    " << line << "\n";
    }
    
    ss << "}";
    return ss.str();
}

NodePtr ScriptNode::clone() const {
    auto cloned = std::make_shared<ScriptNode>(m_scriptType);
    cloned->m_content = m_content;
    cloned->m_referencedSelectors = m_referencedSelectors;
    cloned->m_containsCHTLJS = m_containsCHTLJS;
    cloned->m_autoAddedClasses = m_autoAddedClasses;
    cloned->m_autoAddedIds = m_autoAddedIds;
    return cloned;
}

bool ScriptNode::validate(ErrorReporter* errorReporter) const {
    (void)errorReporter; // 暂时不使用
    return true;
}

// ========== CHTLJSListenerNode 实现 ==========

CHTLJSListenerNode::CHTLJSListenerNode() : ScriptNode(ScriptType::CHTL_JS) {
}

void CHTLJSListenerNode::addEventListener(const std::string& event, const std::string& handler) {
    m_eventListeners[event] = handler;
}

const std::unordered_map<std::string, std::string>& CHTLJSListenerNode::getEventListeners() const {
    return m_eventListeners;
}

std::string CHTLJSListenerNode::convertToStandardEventBinding() const {
    std::stringstream ss;
    
    for (const auto& [event, handler] : m_eventListeners) {
        ss << "addEventListener('" << event << "', " << handler << ");\n";
    }
    
    return ss.str();
}

// ========== CHTLJSAnimationNode 实现 ==========

CHTLJSAnimationNode::CHTLJSAnimationNode() 
    : ScriptNode(ScriptType::CHTL_JS), m_duration(1000), m_easing("ease"), m_loop(1), m_delay(0) {
}

void CHTLJSAnimationNode::setTarget(const CHTLJSSelector& target) {
    m_target = target;
}

void CHTLJSAnimationNode::setDuration(int duration) {
    m_duration = duration;
}

void CHTLJSAnimationNode::setEasing(const std::string& easing) {
    m_easing = easing;
}

void CHTLJSAnimationNode::setLoop(int loop) {
    m_loop = loop;
}

void CHTLJSAnimationNode::setDelay(int delay) {
    m_delay = delay;
}

void CHTLJSAnimationNode::addKeyframe(double at, const std::unordered_map<std::string, std::string>& styles) {
    m_keyframes.push_back({at, styles});
}

std::string CHTLJSAnimationNode::convertToStandardAnimation() const {
    std::stringstream ss;
    
    ss << "{\n";
    ss << "  const target = " << convertSelectorToJS(m_target) << ";\n";
    ss << "  const keyframes = [\n";
    
    for (const auto& [at, styles] : m_keyframes) {
        ss << "    {\n";
        ss << "      offset: " << at << ",\n";
        for (const auto& [prop, value] : styles) {
            ss << "      " << prop << ": '" << value << "',\n";
        }
        ss << "    },\n";
    }
    
    ss << "  ];\n";
    ss << "  const options = {\n";
    ss << "    duration: " << m_duration << ",\n";
    ss << "    easing: '" << m_easing << "',\n";
    ss << "    iterations: " << m_loop << ",\n";
    ss << "    delay: " << m_delay << "\n";
    ss << "  };\n";
    ss << "  target.animate(keyframes, options);\n";
    ss << "}";
    
    return ss.str();
}

// ========== CHTLJSDelegateNode 实现 ==========

CHTLJSDelegateNode::CHTLJSDelegateNode() : ScriptNode(ScriptType::CHTL_JS) {
}

void CHTLJSDelegateNode::setDelegateTarget(const std::vector<CHTLJSSelector>& targets) {
    m_delegateTargets = targets;
}

void CHTLJSDelegateNode::addDelegateEvent(const std::string& event, const std::string& handler) {
    m_delegateEvents[event] = handler;
}

std::string CHTLJSDelegateNode::convertToStandardDelegate() const {
    std::stringstream ss;
    
    for (const auto& [event, handler] : m_delegateEvents) {
        ss << "document.addEventListener('" << event << "', function(e) {\n";
        
        for (const auto& target : m_delegateTargets) {
            std::string targetSelector;
            switch (target.type) {
                case SelectorType::TAG:
                    targetSelector = target.selector;
                    break;
                case SelectorType::CLASS:
                    targetSelector = "." + target.selector;
                    break;
                case SelectorType::ID:
                    targetSelector = "#" + target.selector;
                    break;
                default:
                    targetSelector = target.selector;
                    break;
            }
            
            ss << "  if (e.target.matches('" << targetSelector << "')) {\n";
            ss << "    (" << handler << ")(e);\n";
            ss << "  }\n";
        }
        
        ss << "}, true);\n";
    }
    
    return ss.str();
}

} // namespace CHTL