#include "BaseNode.h"
#include "NodeVisitor.h"
#include <algorithm>
#include <sstream>
#include <iomanip>

namespace CHTL {

// BaseNode 实现
BaseNode::BaseNode(NodeType type, const std::string& name)
    : type_(type), name_(name), parent_(nullptr) {
}

void BaseNode::addChild(NodePtr child) {
    if (child) {
        child->setParent(shared_from_this());
        children_.push_back(child);
    }
}

void BaseNode::insertChild(size_t index, NodePtr child) {
    if (child && index <= children_.size()) {
        child->setParent(shared_from_this());
        children_.insert(children_.begin() + index, child);
    }
}

bool BaseNode::removeChild(NodePtr child) {
    auto it = std::find(children_.begin(), children_.end(), child);
    if (it != children_.end()) {
        (*it)->setParent(nullptr);
        children_.erase(it);
        return true;
    }
    return false;
}

NodePtr BaseNode::removeChild(size_t index) {
    if (index < children_.size()) {
        NodePtr child = children_[index];
        child->setParent(nullptr);
        children_.erase(children_.begin() + index);
        return child;
    }
    return nullptr;
}

void BaseNode::clearChildren() {
    for (auto& child : children_) {
        child->setParent(nullptr);
    }
    children_.clear();
}

NodePtr BaseNode::getChild(size_t index) const {
    return (index < children_.size()) ? children_[index] : nullptr;
}

NodeList BaseNode::findChildrenByName(const std::string& name) const {
    NodeList result;
    for (const auto& child : children_) {
        if (child->getName() == name) {
            result.push_back(child);
        }
    }
    return result;
}

NodeList BaseNode::findChildrenByType(NodeType type) const {
    NodeList result;
    for (const auto& child : children_) {
        if (child->getType() == type) {
            result.push_back(child);
        }
    }
    return result;
}

void BaseNode::setAttribute(const std::string& key, const std::string& value) {
    attributes_[key] = value;
}

std::string BaseNode::getAttribute(const std::string& key, const std::string& default_value) const {
    auto it = attributes_.find(key);
    return (it != attributes_.end()) ? it->second : default_value;
}

bool BaseNode::hasAttribute(const std::string& key) const {
    return attributes_.find(key) != attributes_.end();
}

bool BaseNode::removeAttribute(const std::string& key) {
    return attributes_.erase(key) > 0;
}

void BaseNode::clearAttributes() {
    attributes_.clear();
}

int BaseNode::getDepth() const {
    int depth = 0;
    NodePtr current = parent_;
    while (current) {
        depth++;
        current = current->getParent();
    }
    return depth;
}

NodePtr BaseNode::getRoot() {
    NodePtr current = shared_from_this();
    while (current->getParent()) {
        current = current->getParent();
    }
    return current;
}

NodeList BaseNode::getAncestors() const {
    NodeList ancestors;
    NodePtr current = parent_;
    while (current) {
        ancestors.push_back(current);
        current = current->getParent();
    }
    return ancestors;
}

NodeList BaseNode::getDescendants() const {
    NodeList descendants;
    for (const auto& child : children_) {
        descendants.push_back(child);
        NodeList child_descendants = child->getDescendants();
        descendants.insert(descendants.end(), child_descendants.begin(), child_descendants.end());
    }
    return descendants;
}

NodePtr BaseNode::clone(bool deep) const {
    auto cloned = std::make_shared<BaseNode>(type_, name_);
    cloned->attributes_ = attributes_;
    cloned->start_pos_ = start_pos_;
    cloned->end_pos_ = end_pos_;
    cloned->raw_content_ = raw_content_;
    
    if (deep) {
        for (const auto& child : children_) {
            cloned->addChild(child->clone(true));
        }
    }
    
    return cloned;
}

void BaseNode::accept(NodeVisitor& visitor) {
    visitor.visitBase(*this);
}

std::string BaseNode::toString(int indent) const {
    std::ostringstream oss;
    oss << getIndent(indent) << "Node{type=" << static_cast<int>(type_) 
        << ", name=\"" << name_ << "\"";
    
    if (!attributes_.empty()) {
        oss << ", attributes={";
        bool first = true;
        for (const auto& attr : attributes_) {
            if (!first) oss << ", ";
            oss << attr.first << "=\"" << attr.second << "\"";
            first = false;
        }
        oss << "}";
    }
    
    oss << "}";
    
    if (!children_.empty()) {
        oss << " {" << std::endl;
        for (const auto& child : children_) {
            oss << child->toString(indent + 1) << std::endl;
        }
        oss << getIndent(indent) << "}";
    }
    
    return oss.str();
}

std::string BaseNode::toXml(int indent) const {
    std::ostringstream oss;
    oss << getIndent(indent) << "<node type=\"" << static_cast<int>(type_) << "\"";
    
    if (!name_.empty()) {
        oss << " name=\"" << escapeString(name_) << "\"";
    }
    
    for (const auto& attr : attributes_) {
        oss << " " << attr.first << "=\"" << escapeString(attr.second) << "\"";
    }
    
    if (children_.empty()) {
        oss << "/>";
    } else {
        oss << ">" << std::endl;
        for (const auto& child : children_) {
            oss << child->toXml(indent + 1) << std::endl;
        }
        oss << getIndent(indent) << "</node>";
    }
    
    return oss.str();
}

std::string BaseNode::toJson(int indent) const {
    std::ostringstream oss;
    oss << getIndent(indent) << "{" << std::endl;
    oss << getIndent(indent + 1) << "\"type\": " << static_cast<int>(type_) << "," << std::endl;
    oss << getIndent(indent + 1) << "\"name\": \"" << escapeString(name_) << "\"";
    
    if (!attributes_.empty()) {
        oss << "," << std::endl << getIndent(indent + 1) << "\"attributes\": {" << std::endl;
        bool first = true;
        for (const auto& attr : attributes_) {
            if (!first) oss << "," << std::endl;
            oss << getIndent(indent + 2) << "\"" << attr.first << "\": \"" << escapeString(attr.second) << "\"";
            first = false;
        }
        oss << std::endl << getIndent(indent + 1) << "}";
    }
    
    if (!children_.empty()) {
        oss << "," << std::endl << getIndent(indent + 1) << "\"children\": [" << std::endl;
        for (size_t i = 0; i < children_.size(); ++i) {
            if (i > 0) oss << "," << std::endl;
            oss << children_[i]->toJson(indent + 2);
        }
        oss << std::endl << getIndent(indent + 1) << "]";
    }
    
    oss << std::endl << getIndent(indent) << "}";
    return oss.str();
}

std::string BaseNode::validate() const {
    // 基本验证
    if (name_.empty() && type_ != NodeType::TEXT) {
        return "Node name cannot be empty for non-text nodes";
    }
    
    // 验证子节点
    for (const auto& child : children_) {
        std::string child_error = child->validate();
        if (!child_error.empty()) {
            return "Child node validation failed: " + child_error;
        }
    }
    
    return ""; // 验证通过
}

std::string BaseNode::getIndent(int level) const {
    return std::string(level * 2, ' ');
}

std::string BaseNode::escapeString(const std::string& str) const {
    std::string result;
    for (char c : str) {
        switch (c) {
            case '<': result += "&lt;"; break;
            case '>': result += "&gt;"; break;
            case '&': result += "&amp;"; break;
            case '"': result += "&quot;"; break;
            case '\'': result += "&#39;"; break;
            default: result += c; break;
        }
    }
    return result;
}

// ElementNode 实现
ElementNode::ElementNode(const std::string& tag, bool is_html_element)
    : BaseNode(NodeType::ELEMENT, tag), tag_(tag), is_self_closing_(false), is_html_element_(is_html_element) {
}

NodePtr ElementNode::clone(bool deep) const {
    auto cloned = std::make_shared<ElementNode>(tag_, is_html_element_);
    cloned->attributes_ = attributes_;
    cloned->start_pos_ = start_pos_;
    cloned->end_pos_ = end_pos_;
    cloned->raw_content_ = raw_content_;
    cloned->is_self_closing_ = is_self_closing_;
    
    if (deep) {
        for (const auto& child : children_) {
            cloned->addChild(child->clone(true));
        }
    }
    
    return cloned;
}

void ElementNode::accept(NodeVisitor& visitor) {
    visitor.visitElement(*this);
}

std::string ElementNode::toString(int indent) const {
    std::ostringstream oss;
    oss << getIndent(indent) << "ElementNode{tag=\"" << tag_ << "\"";
    
    if (is_html_element_) {
        oss << ", html=true";
    }
    
    if (is_self_closing_) {
        oss << ", self-closing=true";
    }
    
    if (!attributes_.empty()) {
        oss << ", attributes={";
        bool first = true;
        for (const auto& attr : attributes_) {
            if (!first) oss << ", ";
            oss << attr.first << "=\"" << attr.second << "\"";
            first = false;
        }
        oss << "}";
    }
    
    oss << "}";
    
    if (!children_.empty()) {
        oss << " {" << std::endl;
        for (const auto& child : children_) {
            oss << child->toString(indent + 1) << std::endl;
        }
        oss << getIndent(indent) << "}";
    }
    
    return oss.str();
}

std::string ElementNode::toHtml(int indent) const {
    std::ostringstream oss;
    oss << getIndent(indent) << "<" << tag_;
    
    // 添加属性
    for (const auto& attr : attributes_) {
        oss << " " << attr.first << "=\"" << escapeString(attr.second) << "\"";
    }
    
    if (is_self_closing_ || children_.empty()) {
        oss << "/>";
    } else {
        oss << ">";
        
        // 如果只有一个文本子节点，不换行
        if (children_.size() == 1 && children_[0]->getType() == NodeType::TEXT) {
            auto text_node = std::static_pointer_cast<TextNode>(children_[0]);
            oss << escapeString(text_node->getText());
        } else {
            oss << std::endl;
            for (const auto& child : children_) {
                if (child->getType() == NodeType::ELEMENT) {
                    auto element_child = std::static_pointer_cast<ElementNode>(child);
                    oss << element_child->toHtml(indent + 1) << std::endl;
                } else if (child->getType() == NodeType::TEXT) {
                    auto text_child = std::static_pointer_cast<TextNode>(child);
                    oss << getIndent(indent + 1) << escapeString(text_child->getText()) << std::endl;
                }
            }
            oss << getIndent(indent);
        }
        
        oss << "</" << tag_ << ">";
    }
    
    return oss.str();
}

std::string ElementNode::validate() const {
    // 检查标签名
    if (tag_.empty()) {
        return "Element tag name cannot be empty";
    }
    
    // 检查HTML元素的有效性
    if (is_html_element_) {
        // 这里可以添加HTML标准验证
        // 例如检查是否为有效的HTML标签名
    }
    
    // 自闭合标签不应该有子节点
    if (is_self_closing_ && !children_.empty()) {
        return "Self-closing element should not have children";
    }
    
    return BaseNode::validate();
}

// TextNode 实现
TextNode::TextNode(const std::string& text, bool is_literal)
    : BaseNode(NodeType::TEXT, "text"), text_(text), is_literal_(is_literal), preserve_whitespace_(false) {
}

void TextNode::appendText(const std::string& text) {
    text_ += text;
}

NodePtr TextNode::clone(bool deep) const {
    auto cloned = std::make_shared<TextNode>(text_, is_literal_);
    cloned->attributes_ = attributes_;
    cloned->start_pos_ = start_pos_;
    cloned->end_pos_ = end_pos_;
    cloned->raw_content_ = raw_content_;
    cloned->preserve_whitespace_ = preserve_whitespace_;
    
    if (deep) {
        for (const auto& child : children_) {
            cloned->addChild(child->clone(true));
        }
    }
    
    return cloned;
}

void TextNode::accept(NodeVisitor& visitor) {
    visitor.visitText(*this);
}

std::string TextNode::toString(int indent) const {
    std::ostringstream oss;
    oss << getIndent(indent) << "TextNode{text=\"" << escapeString(text_) << "\"";
    
    if (is_literal_) {
        oss << ", literal=true";
    }
    
    if (preserve_whitespace_) {
        oss << ", preserve-whitespace=true";
    }
    
    oss << "}";
    
    return oss.str();
}

std::string TextNode::validate() const {
    // 文本节点一般不需要特殊验证
    // 但可以检查是否包含非法字符等
    
    return BaseNode::validate();
}

} // namespace CHTL