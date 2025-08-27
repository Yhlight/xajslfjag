#include "BaseNode.h"
#include <algorithm>
#include <sstream>

namespace CHTL {

// 定义void元素（自闭合标签）
static const std::vector<std::string> voidElements = {
    "area", "base", "br", "col", "embed", "hr", "img", "input",
    "link", "meta", "param", "source", "track", "wbr"
};

// BaseNode实现
void BaseNode::RemoveChild(NodePtr child) {
    children.erase(
        std::remove(children.begin(), children.end(), child),
        children.end()
    );
}

NodePtr BaseNode::GetChild(size_t index) const {
    if (index < children.size()) {
        return children[index];
    }
    return nullptr;
}

std::string BaseNode::ToString() const {
    std::stringstream ss;
    ss << "Node[type=" << static_cast<int>(nodeType) 
       << ", name=" << GetName()
       << ", children=" << children.size()
       << ", pos=" << line << ":" << column << "]";
    return ss.str();
}

// ElementNode实现
ElementNode::ElementNode(const std::string& tag) 
    : BaseNode(NodeType::ELEMENT), tagName(tag) {
}

std::string ElementNode::Generate() const {
    std::stringstream ss;
    
    // 开始标签
    ss << "<" << tagName;
    
    // 添加属性
    for (const auto& [name, value] : attributes) {
        ss << " " << name << "=\"" << value << "\"";
    }
    
    // 自闭合标签
    if (IsVoidElement() && children.empty() && !styleNode && !scriptNode) {
        ss << " />";
        return ss.str();
    }
    
    ss << ">";
    
    // 添加局部样式
    if (styleNode) {
        ss << styleNode->Generate();
    }
    
    // 添加子节点
    for (const auto& child : children) {
        ss << child->Generate();
    }
    
    // 添加局部脚本
    if (scriptNode) {
        ss << scriptNode->Generate();
    }
    
    // 结束标签
    ss << "</" << tagName << ">";
    
    return ss.str();
}

NodePtr ElementNode::Clone() const {
    auto clone = std::make_shared<ElementNode>(tagName);
    clone->attributes = attributes;
    clone->SetPosition(line, column);
    
    // 克隆子节点
    for (const auto& child : children) {
        clone->AddChild(child->Clone());
    }
    
    // 克隆样式和脚本节点
    if (styleNode) {
        clone->SetStyleNode(styleNode->Clone());
    }
    if (scriptNode) {
        clone->SetScriptNode(scriptNode->Clone());
    }
    
    return clone;
}

void ElementNode::Accept(NodeVisitor* visitor) {
    visitor->VisitElementNode(this);
}

void ElementNode::SetAttribute(const std::string& name, const std::string& value) {
    attributes[name] = value;
}

std::string ElementNode::GetAttribute(const std::string& name) const {
    auto it = attributes.find(name);
    if (it != attributes.end()) {
        return it->second;
    }
    return "";
}

bool ElementNode::HasAttribute(const std::string& name) const {
    return attributes.find(name) != attributes.end();
}

void ElementNode::RemoveAttribute(const std::string& name) {
    attributes.erase(name);
}

bool ElementNode::IsSelfClosing() const {
    // 检查是否是自定义的自闭合标签
    return HasAttribute("self-closing") && GetAttribute("self-closing") == "true";
}

bool ElementNode::IsVoidElement() const {
    return std::find(voidElements.begin(), voidElements.end(), tagName) != voidElements.end();
}

// TextNode实现
TextNode::TextNode(const std::string& content) 
    : BaseNode(NodeType::TEXT), content(content) {
}

std::string TextNode::Generate() const {
    // HTML实体转义
    std::string result = content;
    size_t pos = 0;
    
    // 转义 <
    while ((pos = result.find("<", pos)) != std::string::npos) {
        result.replace(pos, 1, "&lt;");
        pos += 4;
    }
    
    // 转义 >
    pos = 0;
    while ((pos = result.find(">", pos)) != std::string::npos) {
        result.replace(pos, 1, "&gt;");
        pos += 4;
    }
    
    // 转义 &
    pos = 0;
    while ((pos = result.find("&", pos)) != std::string::npos) {
        // 跳过已经是实体的情况
        if (result.substr(pos, 4) != "&lt;" && result.substr(pos, 4) != "&gt;") {
            result.replace(pos, 1, "&amp;");
            pos += 5;
        } else {
            pos++;
        }
    }
    
    return result;
}

NodePtr TextNode::Clone() const {
    auto clone = std::make_shared<TextNode>(content);
    clone->SetPosition(line, column);
    return clone;
}

void TextNode::Accept(NodeVisitor* visitor) {
    visitor->VisitTextNode(this);
}

} // namespace CHTL