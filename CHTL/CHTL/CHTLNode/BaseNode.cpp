#include "BaseNode.h"
#include <algorithm>
#include <sstream>
#include <unordered_set>

namespace CHTL {

// BaseNode 实现
void BaseNode::addChild(std::shared_ptr<BaseNode> child) {
    if (child) {
        children.push_back(child);
        child->setParent(shared_from_this());
    }
}

void BaseNode::removeChild(std::shared_ptr<BaseNode> child) {
    auto it = std::find(children.begin(), children.end(), child);
    if (it != children.end()) {
        (*it)->setParent(std::weak_ptr<BaseNode>());
        children.erase(it);
    }
}

void BaseNode::insertChild(size_t index, std::shared_ptr<BaseNode> child) {
    if (child && index <= children.size()) {
        children.insert(children.begin() + index, child);
        child->setParent(shared_from_this());
    }
}

// ElementNode 实现
void ElementNode::setAttribute(const std::string& name, const std::string& value) {
    attributes[name] = value;
}

std::string ElementNode::getAttribute(const std::string& name) const {
    auto it = attributes.find(name);
    return (it != attributes.end()) ? it->second : "";
}

bool ElementNode::hasAttribute(const std::string& name) const {
    return attributes.find(name) != attributes.end();
}

void ElementNode::removeAttribute(const std::string& name) {
    attributes.erase(name);
}

void ElementNode::addStyleNode(std::shared_ptr<BaseNode> style) {
    if (style) {
        styleNodes.push_back(style);
    }
}

void ElementNode::addScriptNode(std::shared_ptr<BaseNode> script) {
    if (script) {
        scriptNodes.push_back(script);
    }
}

bool ElementNode::isSelfClosing() const {
    // HTML5 自闭合标签
    static const std::unordered_set<std::string> selfClosingTags = {
        "area", "base", "br", "col", "embed", "hr", "img", "input",
        "link", "meta", "param", "source", "track", "wbr"
    };
    
    return selfClosingTags.find(tagName) != selfClosingTags.end();
}

void ElementNode::accept(NodeVisitor* visitor) {
    visitor->visitElementNode(this);
}

std::shared_ptr<BaseNode> ElementNode::clone() const {
    auto cloned = std::make_shared<ElementNode>(tagName, line, column);
    
    // 复制属性
    cloned->attributes = attributes;
    
    // 递归克隆子节点
    for (const auto& child : children) {
        cloned->addChild(child->clone());
    }
    
    // 克隆样式节点
    for (const auto& style : styleNodes) {
        cloned->addStyleNode(style->clone());
    }
    
    // 克隆脚本节点
    for (const auto& script : scriptNodes) {
        cloned->addScriptNode(script->clone());
    }
    
    return cloned;
}

std::string ElementNode::toString() const {
    std::stringstream ss;
    ss << "<" << tagName;
    
    // 添加属性
    for (const auto& [name, value] : attributes) {
        ss << " " << name << "=\"" << value << "\"";
    }
    
    if (isSelfClosing() && children.empty()) {
        ss << " />";
    } else {
        ss << ">";
        
        // 添加子节点内容
        for (const auto& child : children) {
            ss << child->toString();
        }
        
        ss << "</" << tagName << ">";
    }
    
    return ss.str();
}

// TextNode 实现
void TextNode::accept(NodeVisitor* visitor) {
    visitor->visitTextNode(this);
}

std::shared_ptr<BaseNode> TextNode::clone() const {
    return std::make_shared<TextNode>(content, isLiteral, line, column);
}

std::string TextNode::toString() const {
    return content;
}

} // namespace CHTL