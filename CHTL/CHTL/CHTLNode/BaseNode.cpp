#include "BaseNode.hpp"
#include "../CHTLState/CHTLState.hpp"
#include <iostream>
#include <sstream>
#include <algorithm>

namespace CHTL {

// BaseNode 实现
BaseNode::BaseNode(NodeType type, const std::string& name)
    : nodeType(type), nodeName(name), line(0), column(0), position(0), isValid(true) {
}

NodeType BaseNode::getType() const {
    return nodeType;
}

const std::string& BaseNode::getName() const {
    return nodeName;
}

void BaseNode::setName(const std::string& name) {
    nodeName = name;
}

void BaseNode::addChild(std::shared_ptr<BaseNode> child) {
    if (child) {
        child->setParent(shared_from_this());
        children.push_back(child);
    }
}

void BaseNode::removeChild(std::shared_ptr<BaseNode> child) {
    auto it = std::find(children.begin(), children.end(), child);
    if (it != children.end()) {
        (*it)->setParent(std::shared_ptr<BaseNode>());
        children.erase(it);
    }
}

void BaseNode::insertChild(size_t index, std::shared_ptr<BaseNode> child) {
    if (child && index <= children.size()) {
        child->setParent(shared_from_this());
        children.insert(children.begin() + index, child);
    }
}

const std::vector<std::shared_ptr<BaseNode>>& BaseNode::getChildren() const {
    return children;
}

std::shared_ptr<BaseNode> BaseNode::getChild(size_t index) const {
    if (index < children.size()) {
        return children[index];
    }
    return nullptr;
}

size_t BaseNode::getChildCount() const {
    return children.size();
}

std::shared_ptr<BaseNode> BaseNode::getParent() const {
    return parent.lock();
}

void BaseNode::setParent(std::shared_ptr<BaseNode> parentNode) {
    parent = parentNode;
}

std::shared_ptr<BaseNode> BaseNode::findChild(const std::string& name) const {
    for (const auto& child : children) {
        if (child->getName() == name) {
            return child;
        }
    }
    return nullptr;
}

std::shared_ptr<BaseNode> BaseNode::findChildByType(NodeType type) const {
    for (const auto& child : children) {
        if (child->getType() == type) {
            return child;
        }
    }
    return nullptr;
}

std::vector<std::shared_ptr<BaseNode>> BaseNode::findChildrenByType(NodeType type) const {
    std::vector<std::shared_ptr<BaseNode>> result;
    for (const auto& child : children) {
        if (child->getType() == type) {
            result.push_back(child);
        }
    }
    return result;
}

std::shared_ptr<BaseNode> BaseNode::findDescendant(const std::string& name) const {
    for (const auto& child : children) {
        if (child->getName() == name) {
            return child;
        }
        auto descendant = child->findDescendant(name);
        if (descendant) {
            return descendant;
        }
    }
    return nullptr;
}

void BaseNode::setAttribute(const std::string& name, const std::string& value, bool isLiteral) {
    attributes[name] = NodeAttribute(name, value, isLiteral, false);
}

void BaseNode::setAttribute(const NodeAttribute& attr) {
    attributes[attr.name] = attr;
}

NodeAttribute BaseNode::getAttribute(const std::string& name) const {
    auto it = attributes.find(name);
    return (it != attributes.end()) ? it->second : NodeAttribute();
}

bool BaseNode::hasAttribute(const std::string& name) const {
    return attributes.find(name) != attributes.end();
}

void BaseNode::removeAttribute(const std::string& name) {
    attributes.erase(name);
}

const std::unordered_map<std::string, NodeAttribute>& BaseNode::getAttributes() const {
    return attributes;
}

bool BaseNode::hasMetadata(const std::string& key) const {
    return metadata.find(key) != metadata.end();
}

void BaseNode::removeMetadata(const std::string& key) {
    metadata.erase(key);
}

void BaseNode::setPosition(size_t line, size_t column, size_t position) {
    this->line = line;
    this->column = column;
    this->position = position;
}

size_t BaseNode::getLine() const {
    return line;
}

size_t BaseNode::getColumn() const {
    return column;
}

size_t BaseNode::getPosition() const {
    return position;
}

bool BaseNode::getIsValid() const {
    return isValid;
}

void BaseNode::setValid(bool valid) {
    isValid = valid;
}

const std::string& BaseNode::getErrorMessage() const {
    return errorMessage;
}

void BaseNode::setErrorMessage(const std::string& error) {
    errorMessage = error;
    isValid = false;
}

std::string BaseNode::toString() const {
    std::stringstream ss;
    ss << "Node{type=" << static_cast<int>(nodeType) << ", name=" << nodeName << "}";
    return ss.str();
}

std::string BaseNode::toHTML() const {
    // 基类默认实现，子类应该重写
    return "";
}

std::string BaseNode::toCSS() const {
    // 基类默认实现，子类应该重写
    return "";
}

std::string BaseNode::toJavaScript() const {
    // 基类默认实现，子类应该重写
    return "";
}

void BaseNode::validate(CHTLState& state) {
    // 基类默认验证，子类可以重写
    isValid = true;
    errorMessage.clear();
}

std::shared_ptr<BaseNode> BaseNode::clone() const {
    auto cloned = createNode(nodeType, nodeName);
    cloned->attributes = attributes;
    cloned->metadata = metadata;
    cloned->line = line;
    cloned->column = column;
    cloned->position = position;
    cloned->isValid = isValid;
    cloned->errorMessage = errorMessage;
    
    // 克隆子节点
    for (const auto& child : children) {
        cloned->addChild(child->clone());
    }
    
    return cloned;
}

void BaseNode::accept(class NodeVisitor& visitor) {
    // 访问者模式的默认实现
}

void BaseNode::dumpTree(int indent) const {
    std::cout << std::string(indent * 2, ' ') << toString() << std::endl;
    for (const auto& child : children) {
        child->dumpTree(indent + 1);
    }
}

std::string BaseNode::getTreeString(int indent) const {
    std::stringstream ss;
    ss << std::string(indent * 2, ' ') << toString() << "\n";
    for (const auto& child : children) {
        ss << child->getTreeString(indent + 1);
    }
    return ss.str();
}

std::shared_ptr<BaseNode> BaseNode::createNode(NodeType type, const std::string& name) {
    switch (type) {
        case NodeType::ELEMENT:
            return std::make_shared<ElementNode>(name);
        case NodeType::TEXT:
            return std::make_shared<TextNode>(name);
        default:
            return std::make_shared<BaseNode>(type, name);
    }
}

// ElementNode 实现
ElementNode::ElementNode(const std::string& tag)
    : BaseNode(NodeType::ELEMENT, tag), tagName(tag), isSelfClosing(false),
      hasStyleBlock(false), hasScriptBlock(false) {
}

const std::string& ElementNode::getTagName() const {
    return tagName;
}

void ElementNode::setTagName(const std::string& tag) {
    tagName = tag;
    setName(tag);
}

bool ElementNode::getIsSelfClosing() const {
    return isSelfClosing;
}

void ElementNode::setSelfClosing(bool selfClosing) {
    isSelfClosing = selfClosing;
}

bool ElementNode::getHasStyleBlock() const {
    return hasStyleBlock;
}

void ElementNode::setHasStyleBlock(bool hasStyle) {
    hasStyleBlock = hasStyle;
}

bool ElementNode::getHasScriptBlock() const {
    return hasScriptBlock;
}

void ElementNode::setHasScriptBlock(bool hasScript) {
    hasScriptBlock = hasScript;
}

void ElementNode::addClass(const std::string& className) {
    if (std::find(cssClasses.begin(), cssClasses.end(), className) == cssClasses.end()) {
        cssClasses.push_back(className);
        updateClassAttribute();
    }
}

void ElementNode::removeClass(const std::string& className) {
    auto it = std::find(cssClasses.begin(), cssClasses.end(), className);
    if (it != cssClasses.end()) {
        cssClasses.erase(it);
        updateClassAttribute();
    }
}

bool ElementNode::hasClass(const std::string& className) const {
    return std::find(cssClasses.begin(), cssClasses.end(), className) != cssClasses.end();
}

const std::vector<std::string>& ElementNode::getClasses() const {
    return cssClasses;
}

void ElementNode::updateClassAttribute() {
    if (cssClasses.empty()) {
        removeAttribute("class");
    } else {
        std::stringstream ss;
        for (size_t i = 0; i < cssClasses.size(); ++i) {
            if (i > 0) ss << " ";
            ss << cssClasses[i];
        }
        setAttribute("class", ss.str());
    }
}

void ElementNode::setId(const std::string& id) {
    idAttribute = id;
    setAttribute("id", id);
}

const std::string& ElementNode::getId() const {
    return idAttribute;
}

std::string ElementNode::toString() const {
    std::stringstream ss;
    ss << "ElementNode{tag=" << tagName;
    if (!cssClasses.empty()) {
        ss << ", classes=[";
        for (size_t i = 0; i < cssClasses.size(); ++i) {
            if (i > 0) ss << ", ";
            ss << cssClasses[i];
        }
        ss << "]";
    }
    if (!idAttribute.empty()) {
        ss << ", id=" << idAttribute;
    }
    ss << "}";
    return ss.str();
}

std::string ElementNode::toHTML() const {
    std::stringstream ss;
    ss << "<" << tagName;
    
    // 添加属性
    for (const auto& attr : getAttributes()) {
        ss << " " << attr.first << "=\"" << attr.second.value << "\"";
    }
    
    if (isSelfClosing) {
        ss << " />";
    } else {
        ss << ">";
        
        // 添加子节点内容
        for (const auto& child : getChildren()) {
            ss << child->toHTML();
        }
        
        ss << "</" << tagName << ">";
    }
    
    return ss.str();
}

void ElementNode::validate(CHTLState& state) {
    BaseNode::validate(state);
    
    // 验证HTML元素标签名
    if (tagName.empty()) {
        setErrorMessage("元素标签名不能为空");
        return;
    }
    
    // 验证自闭合标签
    const std::vector<std::string> selfClosingTags = {
        "area", "base", "br", "col", "embed", "hr", "img", "input",
        "link", "meta", "param", "source", "track", "wbr"
    };
    
    bool shouldBeSelfClosing = std::find(selfClosingTags.begin(), selfClosingTags.end(), tagName) 
                              != selfClosingTags.end();
    
    if (shouldBeSelfClosing && !isSelfClosing) {
        // 警告：标签应该是自闭合的
        setMetadata("warning", "标签 " + tagName + " 应该是自闭合的");
    }
}

std::shared_ptr<BaseNode> ElementNode::clone() const {
    auto cloned = std::make_shared<ElementNode>(tagName);
    
    // 复制基类数据
    cloned->attributes = getAttributes();
    cloned->setPosition(getLine(), getColumn(), getPosition());
    cloned->setValid(getIsValid());
    cloned->setErrorMessage(getErrorMessage());
    
    // 复制ElementNode特有数据
    cloned->isSelfClosing = isSelfClosing;
    cloned->hasStyleBlock = hasStyleBlock;
    cloned->hasScriptBlock = hasScriptBlock;
    cloned->cssClasses = cssClasses;
    cloned->idAttribute = idAttribute;
    
    // 克隆子节点
    for (const auto& child : getChildren()) {
        cloned->addChild(child->clone());
    }
    
    return cloned;
}

// TextNode 实现
TextNode::TextNode(const std::string& text)
    : BaseNode(NodeType::TEXT, "text"), content(text), isLiteral(false), needsEscaping(true) {
}

const std::string& TextNode::getContent() const {
    return content;
}

void TextNode::setContent(const std::string& text) {
    content = text;
}

bool TextNode::getIsLiteral() const {
    return isLiteral;
}

void TextNode::setIsLiteral(bool literal) {
    isLiteral = literal;
}

bool TextNode::getNeedsEscaping() const {
    return needsEscaping;
}

void TextNode::setNeedsEscaping(bool escaping) {
    needsEscaping = escaping;
}

std::string TextNode::getProcessedContent() const {
    if (!needsEscaping || isLiteral) {
        return content;
    }
    
    // HTML转义
    std::string result = content;
    std::string from[] = {"&", "<", ">", "\"", "'"};
    std::string to[] = {"&amp;", "&lt;", "&gt;", "&quot;", "&#39;"};
    
    for (size_t i = 0; i < 5; ++i) {
        size_t pos = 0;
        while ((pos = result.find(from[i], pos)) != std::string::npos) {
            result.replace(pos, from[i].length(), to[i]);
            pos += to[i].length();
        }
    }
    
    return result;
}

void TextNode::appendContent(const std::string& text) {
    content += text;
}

std::string TextNode::toString() const {
    return "TextNode{content=\"" + content + "\", literal=" + 
           (isLiteral ? "true" : "false") + "}";
}

std::string TextNode::toHTML() const {
    return getProcessedContent();
}

void TextNode::validate(CHTLState& state) {
    BaseNode::validate(state);
    // 文本节点通常不需要特殊验证
}

std::shared_ptr<BaseNode> TextNode::clone() const {
    auto cloned = std::make_shared<TextNode>(content);
    
    // 复制基类数据
    cloned->setPosition(getLine(), getColumn(), getPosition());
    cloned->setValid(getIsValid());
    cloned->setErrorMessage(getErrorMessage());
    
    // 复制TextNode特有数据
    cloned->isLiteral = isLiteral;
    cloned->needsEscaping = needsEscaping;
    
    return cloned;
}

}