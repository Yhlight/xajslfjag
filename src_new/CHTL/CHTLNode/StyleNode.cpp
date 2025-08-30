#include "StyleNode.h"
#include <sstream>
#include <regex>

namespace CHTL {

StyleNode::StyleNode(StyleNodeType styleType)
    : BaseNode(CHTLNodeType::STYLE_NODE, "style")
    , m_styleType(styleType) {
}

StyleNodeType StyleNode::getStyleType() const {
    return m_styleType;
}

void StyleNode::setStyleType(StyleNodeType type) {
    m_styleType = type;
}

void StyleNode::addCssProperty(const std::string& property, const std::string& value) {
    m_cssProperties[property] = value;
}

std::string StyleNode::getCssProperty(const std::string& property) const {
    auto it = m_cssProperties.find(property);
    return (it != m_cssProperties.end()) ? it->second : "";
}

bool StyleNode::hasCssProperty(const std::string& property) const {
    return m_cssProperties.find(property) != m_cssProperties.end();
}

bool StyleNode::removeCssProperty(const std::string& property) {
    auto it = m_cssProperties.find(property);
    if (it != m_cssProperties.end()) {
        m_cssProperties.erase(it);
        return true;
    }
    return false;
}

const std::unordered_map<std::string, std::string>& StyleNode::getCssProperties() const {
    return m_cssProperties;
}

void StyleNode::clearCssProperties() {
    m_cssProperties.clear();
}

void StyleNode::setSelector(const std::string& selector) {
    m_selector = selector;
}

const std::string& StyleNode::getSelector() const {
    return m_selector;
}

bool StyleNode::hasSelector() const {
    return !m_selector.empty();
}

bool StyleNode::isClassSelector() const {
    return !m_selector.empty() && m_selector[0] == '.';
}

bool StyleNode::isIdSelector() const {
    return !m_selector.empty() && m_selector[0] == '#';
}

bool StyleNode::isPseudoClassSelector() const {
    return m_selector.find(':') != std::string::npos && 
           m_selector.find("::") == std::string::npos;
}

bool StyleNode::isPseudoElementSelector() const {
    return m_selector.find("::") != std::string::npos;
}

std::string StyleNode::getAutoClassName() const {
    if (isClassSelector()) {
        return extractSelectorName(m_selector);
    }
    return "";
}

std::string StyleNode::getAutoId() const {
    if (isIdSelector()) {
        return extractSelectorName(m_selector);
    }
    return "";
}

bool StyleNode::needsAutoClass() const {
    return isClassSelector() && m_styleType == StyleNodeType::LOCAL_STYLE;
}

bool StyleNode::needsAutoId() const {
    return isIdSelector() && m_styleType == StyleNodeType::LOCAL_STYLE;
}

std::shared_ptr<BaseNode> StyleNode::clone() const {
    auto cloned = std::make_shared<StyleNode>(m_styleType);
    cloned->m_position = m_position;
    cloned->m_access = m_access;
    cloned->m_attributes = m_attributes;
    cloned->m_selector = m_selector;
    cloned->m_cssProperties = m_cssProperties;
    return cloned;
}

std::string StyleNode::toString() const {
    std::stringstream ss;
    ss << "Style";
    
    if (hasSelector()) {
        ss << "(" << m_selector << ")";
    }
    
    if (!m_cssProperties.empty()) {
        ss << " {";
        bool first = true;
        for (const auto& prop : m_cssProperties) {
            if (!first) ss << ", ";
            ss << prop.first << ": " << prop.second;
            first = false;
        }
        ss << "}";
    }
    
    return ss.str();
}

std::string StyleNode::toCss() const {
    std::stringstream ss;
    
    if (hasSelector()) {
        ss << m_selector << " {\n";
    } else {
        ss << "{\n";
    }
    
    for (const auto& prop : m_cssProperties) {
        ss << "  " << prop.first << ": " << prop.second << ";\n";
    }
    
    ss << "}";
    
    return ss.str();
}

bool StyleNode::validate(ErrorReporter* errorReporter) const {
    if (!BaseNode::validate(errorReporter)) {
        return false;
    }
    
    return internalValidate(errorReporter);
}

bool StyleNode::internalValidate(ErrorReporter* errorReporter) const {
    // 验证CSS属性名
    for (const auto& prop : m_cssProperties) {
        if (prop.first.empty()) {
            if (errorReporter) {
                ErrorPosition pos("", m_position.line, m_position.column);
                errorReporter->error(ErrorType::SEMANTIC_ERROR, "Empty CSS property name", pos);
            }
            return false;
        }
    }
    
    // 验证选择器格式
    if (hasSelector()) {
        if (m_selector.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-_.:#&") 
            != std::string::npos) {
            if (errorReporter) {
                ErrorPosition pos("", m_position.line, m_position.column);
                errorReporter->warning("Invalid characters in selector: " + m_selector, pos);
            }
        }
    }
    
    return true;
}

std::string StyleNode::extractSelectorName(const std::string& selector) const {
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

} // namespace CHTL