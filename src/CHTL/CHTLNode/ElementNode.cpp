#include "ElementNode.h"
#include <algorithm>
#include <sstream>

namespace CHTL {

ElementNode::ElementNode(const std::string& tagName)
    : BaseNode(CHTLNodeType::ELEMENT_NODE, tagName)
    , m_tagName(tagName)
    , m_exported(false) {
}

const std::string& ElementNode::getTagName() const {
    return m_tagName;
}

void ElementNode::setTagName(const std::string& tagName) {
    m_tagName = tagName;
    setName(tagName);
}

bool ElementNode::isSelfClosing() const {
    const auto& selfClosingTags = getSelfClosingTags();
    return selfClosingTags.find(m_tagName) != selfClosingTags.end();
}

bool ElementNode::isBlockElement() const {
    const auto& blockElements = getBlockElements();
    return blockElements.find(m_tagName) != blockElements.end();
}

bool ElementNode::isInlineElement() const {
    const auto& inlineElements = getInlineElements();
    return inlineElements.find(m_tagName) != inlineElements.end();
}

void ElementNode::setId(const std::string& id) {
    setAttribute("id", id);
}

std::string ElementNode::getId() const {
    return getAttribute("id");
}

void ElementNode::setClass(const std::string& className) {
    setAttribute("class", className);
}

std::string ElementNode::getClass() const {
    return getAttribute("class");
}

void ElementNode::addClass(const std::string& className) {
    if (className.empty()) return;
    
    std::string currentClasses = getClass();
    std::vector<std::string> classes = splitClasses(currentClasses);
    
    // 检查是否已存在
    if (std::find(classes.begin(), classes.end(), className) == classes.end()) {
        classes.push_back(className);
        setClass(joinClasses(classes));
    }
}

bool ElementNode::removeClass(const std::string& className) {
    if (className.empty()) return false;
    
    std::string currentClasses = getClass();
    std::vector<std::string> classes = splitClasses(currentClasses);
    
    auto it = std::find(classes.begin(), classes.end(), className);
    if (it != classes.end()) {
        classes.erase(it);
        setClass(joinClasses(classes));
        return true;
    }
    
    return false;
}

bool ElementNode::hasClass(const std::string& className) const {
    if (className.empty()) return false;
    
    std::string currentClasses = getClass();
    std::vector<std::string> classes = splitClasses(currentClasses);
    
    return std::find(classes.begin(), classes.end(), className) != classes.end();
}

std::vector<std::string> ElementNode::getClasses() const {
    return splitClasses(getClass());
}

BaseNode* ElementNode::getStyleNode() const {
    return findChildByNameAndType("style", CHTLNodeType::STYLE_NODE);
}

void ElementNode::setStyleNode(NodePtr styleNode) {
    if (!styleNode || styleNode->getNodeType() != CHTLNodeType::STYLE_NODE) {
        return;
    }
    
    // 移除现有的样式节点
    BaseNode* existingStyle = getStyleNode();
    if (existingStyle) {
        removeChild(existingStyle);
    }
    
    addChild(styleNode);
}

BaseNode* ElementNode::getScriptNode() const {
    return findChildByNameAndType("script", CHTLNodeType::SCRIPT_NODE);
}

void ElementNode::setScriptNode(NodePtr scriptNode) {
    if (!scriptNode || scriptNode->getNodeType() != CHTLNodeType::SCRIPT_NODE) {
        return;
    }
    
    // 移除现有的脚本节点
    BaseNode* existingScript = getScriptNode();
    if (existingScript) {
        removeChild(existingScript);
    }
    
    addChild(scriptNode);
}

bool ElementNode::hasInlineStyle() const {
    return hasAttribute("style");
}

bool ElementNode::hasLocalScript() const {
    return getScriptNode() != nullptr;
}

void ElementNode::setExported(bool exported) {
    m_exported = exported;
}

bool ElementNode::isExported() const {
    return m_exported;
}

void ElementNode::setNamespace(const std::string& namespace_) {
    m_namespace = namespace_;
}

const std::string& ElementNode::getNamespace() const {
    return m_namespace;
}

std::shared_ptr<BaseNode> ElementNode::clone() const {
    auto cloned = std::make_shared<ElementNode>(m_tagName);
    cloned->m_value = m_value;
    cloned->m_position = m_position;
    cloned->m_access = m_access;
    cloned->m_attributes = m_attributes;
    cloned->m_namespace = m_namespace;
    cloned->m_exported = m_exported;
    return cloned;
}

std::shared_ptr<BaseNode> ElementNode::deepClone() const {
    auto cloned = std::static_pointer_cast<ElementNode>(clone());
    
    for (const auto& child : m_children) {
        auto clonedChild = child->deepClone();
        cloned->addChild(clonedChild);
    }
    
    return cloned;
}

std::string ElementNode::toString() const {
    std::stringstream ss;
    ss << "Element(" << m_tagName << ")";
    
    if (!getId().empty()) {
        ss << "#" << getId();
    }
    
    if (!getClass().empty()) {
        ss << "." << getClass();
    }
    
    return ss.str();
}

std::string ElementNode::toHtml() const {
    std::stringstream ss;
    
    // 开始标签
    ss << "<" << m_tagName;
    
    // 属性
    for (const auto& attr : m_attributes) {
        ss << " " << attr.first << "=\"" << attr.second << "\"";
    }
    
    if (isSelfClosing() && m_children.empty()) {
        ss << " />";
        return ss.str();
    }
    
    ss << ">";
    
    // 子节点内容
    for (const auto& child : m_children) {
        if (child->getNodeType() == CHTLNodeType::TEXT_NODE) {
            ss << child->getValue();
        } else if (auto elementChild = std::dynamic_pointer_cast<ElementNode>(child)) {
            ss << elementChild->toHtml();
        } else {
            ss << child->toString();
        }
    }
    
    // 结束标签
    ss << "</" << m_tagName << ">";
    
    return ss.str();
}

bool ElementNode::validate(ErrorReporter* errorReporter) const {
    if (!BaseNode::validate(errorReporter)) {
        return false;
    }
    
    return internalValidate(errorReporter);
}

bool ElementNode::internalValidate(ErrorReporter* errorReporter) const {
    // 验证标签名
    if (m_tagName.empty()) {
        if (errorReporter) {
            ErrorPosition pos("", m_position.line, m_position.column);
            errorReporter->error(ErrorType::SEMANTIC_ERROR, "Element has empty tag name", pos);
        }
        return false;
    }
    
    // 验证标签名格式
    if (!std::all_of(m_tagName.begin(), m_tagName.end(), 
                    [](char c) { return std::isalnum(c) || c == '-' || c == '_'; })) {
        if (errorReporter) {
            ErrorPosition pos("", m_position.line, m_position.column);
            errorReporter->error(ErrorType::SEMANTIC_ERROR, 
                               "Invalid characters in tag name: " + m_tagName, pos);
        }
        return false;
    }
    
    // 验证自闭合标签不应有子节点
    if (isSelfClosing() && !m_children.empty()) {
        if (errorReporter) {
            ErrorPosition pos("", m_position.line, m_position.column);
            errorReporter->warning("Self-closing tag " + m_tagName + " has children", pos);
        }
    }
    
    return true;
}

std::vector<std::string> ElementNode::splitClasses(const std::string& classes) const {
    std::vector<std::string> result;
    std::stringstream ss(classes);
    std::string item;
    
    while (std::getline(ss, item, ' ')) {
        if (!item.empty()) {
            result.push_back(item);
        }
    }
    
    return result;
}

std::string ElementNode::joinClasses(const std::vector<std::string>& classes) const {
    std::stringstream ss;
    bool first = true;
    
    for (const auto& className : classes) {
        if (!className.empty()) {
            if (!first) ss << " ";
            ss << className;
            first = false;
        }
    }
    
    return ss.str();
}

const std::unordered_set<std::string>& ElementNode::getSelfClosingTags() {
    static const std::unordered_set<std::string> selfClosingTags = {
        "area", "base", "br", "col", "embed", "hr", "img", "input",
        "link", "meta", "param", "source", "track", "wbr"
    };
    return selfClosingTags;
}

const std::unordered_set<std::string>& ElementNode::getBlockElements() {
    static const std::unordered_set<std::string> blockElements = {
        "div", "p", "h1", "h2", "h3", "h4", "h5", "h6", "header", "footer", "main",
        "section", "article", "aside", "nav", "blockquote", "pre", "ol", "ul", "li",
        "dl", "dt", "dd", "figure", "figcaption", "table", "form", "fieldset", "address"
    };
    return blockElements;
}

const std::unordered_set<std::string>& ElementNode::getInlineElements() {
    static const std::unordered_set<std::string> inlineElements = {
        "a", "abbr", "b", "bdi", "bdo", "br", "button", "cite", "code", "data", "dfn",
        "em", "i", "input", "kbd", "label", "mark", "q", "s", "samp", "small", "span",
        "strong", "sub", "sup", "time", "u", "var", "wbr", "img"
    };
    return inlineElements;
}

} // namespace CHTL