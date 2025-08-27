#include "ElementNode.h"
#include <sstream>
#include <algorithm>

namespace CHTL {

// 静态成员定义
const std::unordered_set<std::string> ElementNode::SELF_CLOSING_TAGS = {
    "area", "base", "br", "col", "embed", "hr", "img", "input",
    "link", "meta", "param", "source", "track", "wbr"
};

const std::unordered_set<std::string> ElementNode::BLOCK_ELEMENTS = {
    "div", "p", "h1", "h2", "h3", "h4", "h5", "h6", "header", "footer",
    "main", "section", "article", "aside", "nav", "ul", "ol", "li",
    "dl", "dt", "dd", "table", "tr", "td", "th", "thead", "tbody",
    "tfoot", "form", "fieldset", "legend", "blockquote", "pre",
    "address", "figure", "figcaption"
};

const std::unordered_set<std::string> ElementNode::INLINE_ELEMENTS = {
    "span", "a", "em", "strong", "small", "s", "cite", "q", "dfn",
    "abbr", "data", "time", "code", "var", "samp", "kbd", "sub",
    "sup", "i", "b", "u", "mark", "ruby", "rt", "rp", "bdi", "bdo"
};

const std::unordered_set<std::string> ElementNode::VOID_ELEMENTS = {
    "area", "base", "br", "col", "embed", "hr", "img", "input",
    "link", "meta", "param", "source", "track", "wbr"
};

ElementNode::ElementNode(const std::string& tagName, const Position& pos)
    : BaseNode(NodeType::ELEMENT_NODE, pos), tagName_(tagName) {
    setName(tagName);
}

void ElementNode::addClass(const std::string& className) {
    if (!className.empty()) {
        classes_.insert(className);
        updateClassAttribute();
    }
}

void ElementNode::removeClass(const std::string& className) {
    classes_.erase(className);
    updateClassAttribute();
}

bool ElementNode::hasClass(const std::string& className) const {
    return classes_.find(className) != classes_.end();
}

std::unordered_set<std::string> ElementNode::getClasses() const {
    return classes_;
}

void ElementNode::clearClasses() {
    classes_.clear();
    updateClassAttribute();
}

void ElementNode::setId(const std::string& id) {
    if (id.empty()) {
        removeAttribute("id");
    } else {
        setAttribute("id", id);
    }
}

std::string ElementNode::getId() const {
    return getAttribute("id");
}

bool ElementNode::hasId() const {
    return hasAttribute("id");
}

void ElementNode::addStyleBlock(std::shared_ptr<BaseNode> styleNode) {
    if (styleNode && styleNode->getType() == NodeType::STYLE_NODE) {
        addChild(styleNode);
        styleBlocks_.push_back(styleNode);
    }
}

std::vector<std::shared_ptr<BaseNode>> ElementNode::getStyleBlocks() const {
    return styleBlocks_;
}

bool ElementNode::hasStyleBlocks() const {
    return !styleBlocks_.empty();
}

void ElementNode::addScriptBlock(std::shared_ptr<BaseNode> scriptNode) {
    if (scriptNode && scriptNode->getType() == NodeType::SCRIPT_NODE) {
        addChild(scriptNode);
        scriptBlocks_.push_back(scriptNode);
    }
}

std::vector<std::shared_ptr<BaseNode>> ElementNode::getScriptBlocks() const {
    return scriptBlocks_;
}

bool ElementNode::hasScriptBlocks() const {
    return !scriptBlocks_.empty();
}

void ElementNode::addTextNode(std::shared_ptr<BaseNode> textNode) {
    if (textNode && textNode->getType() == NodeType::TEXT_NODE) {
        addChild(textNode);
        textNodes_.push_back(textNode);
    }
}

std::vector<std::shared_ptr<BaseNode>> ElementNode::getTextNodes() const {
    return textNodes_;
}

std::string ElementNode::getAllText() const {
    std::string result;
    for (const auto& textNode : textNodes_) {
        if (textNode) {
            result += textNode->getContent();
        }
    }
    return result;
}

void ElementNode::setInlineStyle(const std::string& property, const std::string& value) {
    if (value.empty()) {
        inlineStyles_.erase(property);
    } else {
        inlineStyles_[property] = value;
    }
    updateStyleAttribute();
}

std::string ElementNode::getInlineStyle(const std::string& property) const {
    auto it = inlineStyles_.find(property);
    return (it != inlineStyles_.end()) ? it->second : "";
}

std::unordered_map<std::string, std::string> ElementNode::getAllInlineStyles() const {
    return inlineStyles_;
}

void ElementNode::clearInlineStyles() {
    inlineStyles_.clear();
    updateStyleAttribute();
}

bool ElementNode::isSelfClosing() const {
    return SELF_CLOSING_TAGS.find(tagName_) != SELF_CLOSING_TAGS.end();
}

bool ElementNode::isBlockElement() const {
    return BLOCK_ELEMENTS.find(tagName_) != BLOCK_ELEMENTS.end();
}

bool ElementNode::isInlineElement() const {
    return INLINE_ELEMENTS.find(tagName_) != INLINE_ELEMENTS.end();
}

bool ElementNode::isVoidElement() const {
    return VOID_ELEMENTS.find(tagName_) != VOID_ELEMENTS.end();
}

bool ElementNode::validate() const {
    // 检查标签名是否有效
    if (tagName_.empty()) {
        return false;
    }
    
    // 检查自闭合标签是否有子元素
    if (isSelfClosing() && !children_.empty()) {
        return false;
    }
    
    // 验证属性
    for (const auto& attr : attributes_) {
        if (attr.first.empty()) {
            return false;
        }
    }
    
    return true;
}

std::vector<std::string> ElementNode::getValidationErrors() const {
    std::vector<std::string> errors;
    
    if (tagName_.empty()) {
        errors.push_back("Element tag name is empty");
    }
    
    if (isSelfClosing() && !children_.empty()) {
        errors.push_back("Self-closing tag '" + tagName_ + "' cannot have children");
    }
    
    for (const auto& attr : attributes_) {
        if (attr.first.empty()) {
            errors.push_back("Attribute name is empty");
        }
    }
    
    return errors;
}

std::string ElementNode::toString() const {
    std::ostringstream oss;
    oss << "<" << tagName_;
    
    for (const auto& attr : attributes_) {
        oss << " " << attr.first << "=\"" << attr.second << "\"";
    }
    
    if (isSelfClosing()) {
        oss << " />";
    } else {
        oss << ">";
        for (const auto& child : children_) {
            if (child) {
                oss << child->toString();
            }
        }
        oss << "</" << tagName_ << ">";
    }
    
    return oss.str();
}

std::shared_ptr<BaseNode> ElementNode::clone() const {
    auto cloned = std::make_shared<ElementNode>(tagName_, position_);
    
    // 复制属性
    for (const auto& attr : attributes_) {
        cloned->setAttribute(attr.first, attr.second);
    }
    
    // 复制类名
    for (const auto& className : classes_) {
        cloned->addClass(className);
    }
    
    // 复制内联样式
    for (const auto& style : inlineStyles_) {
        cloned->setInlineStyle(style.first, style.second);
    }
    
    // 复制标志
    cloned->autoClass_ = autoClass_;
    cloned->autoId_ = autoId_;
    
    // 复制子节点
    for (const auto& child : children_) {
        if (child) {
            cloned->addChild(child->clone());
        }
    }
    
    return cloned;
}

void ElementNode::accept(CHTLGenerator* /*generator*/) {
    // 在实现CHTLGenerator时会完成这个方法
    // generator->visitElementNode(this);
}

void ElementNode::onChildAdded(std::shared_ptr<BaseNode> /*child*/) {
    rebuildChildCaches();
}

void ElementNode::onChildRemoved(std::shared_ptr<BaseNode> /*child*/) {
    rebuildChildCaches();
}

void ElementNode::onAttributeChanged(const std::string& name, const std::string& value) {
    if (name == "class") {
        // 解析class属性更新classes_
        classes_.clear();
        std::istringstream iss(value);
        std::string className;
        while (iss >> className) {
            classes_.insert(className);
        }
    }
}

void ElementNode::updateClassAttribute() {
    if (classes_.empty()) {
        removeAttribute("class");
    } else {
        std::ostringstream oss;
        bool first = true;
        for (const auto& className : classes_) {
            if (!first) oss << " ";
            oss << className;
            first = false;
        }
        setAttribute("class", oss.str());
    }
}

void ElementNode::updateStyleAttribute() {
    if (inlineStyles_.empty()) {
        removeAttribute("style");
    } else {
        std::ostringstream oss;
        bool first = true;
        for (const auto& style : inlineStyles_) {
            if (!first) oss << "; ";
            oss << style.first << ": " << style.second;
            first = false;
        }
        setAttribute("style", oss.str());
    }
}

void ElementNode::rebuildChildCaches() {
    styleBlocks_.clear();
    scriptBlocks_.clear();
    textNodes_.clear();
    
    for (const auto& child : children_) {
        if (!child) continue;
        
        switch (child->getType()) {
            case NodeType::STYLE_NODE:
                styleBlocks_.push_back(child);
                break;
            case NodeType::SCRIPT_NODE:
                scriptBlocks_.push_back(child);
                break;
            case NodeType::TEXT_NODE:
                textNodes_.push_back(child);
                break;
            default:
                break;
        }
    }
}

} // namespace CHTL