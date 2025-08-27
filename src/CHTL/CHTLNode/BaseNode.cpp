#include "BaseNode.h"
#include "NodeVisitor.h"
#include <algorithm>
#include <sstream>
#include <regex>

namespace CHTL {

// BaseNode实现

BaseNode::BaseNode(NodeType type, const NodePosition& pos)
    : nodeType(type), position(pos) {
}

void BaseNode::AddChild(std::shared_ptr<BaseNode> child) {
    if (child) {
        child->SetParent(shared_from_this());
        children.push_back(child);
    }
}

void BaseNode::InsertChild(size_t index, std::shared_ptr<BaseNode> child) {
    if (child && index <= children.size()) {
        child->SetParent(shared_from_this());
        children.insert(children.begin() + index, child);
    }
}

void BaseNode::RemoveChild(std::shared_ptr<BaseNode> child) {
    auto it = std::find(children.begin(), children.end(), child);
    if (it != children.end()) {
        (*it)->SetParent(nullptr);
        children.erase(it);
    }
}

void BaseNode::RemoveChild(size_t index) {
    if (index < children.size()) {
        children[index]->SetParent(nullptr);
        children.erase(children.begin() + index);
    }
}

std::shared_ptr<BaseNode> BaseNode::GetChild(size_t index) const {
    return (index < children.size()) ? children[index] : nullptr;
}

std::vector<std::shared_ptr<BaseNode>> BaseNode::FindChildrenByType(NodeType type) const {
    std::vector<std::shared_ptr<BaseNode>> result;
    for (const auto& child : children) {
        if (child->GetNodeType() == type) {
            result.push_back(child);
        }
    }
    return result;
}

std::shared_ptr<BaseNode> BaseNode::FindFirstChildByType(NodeType type) const {
    for (const auto& child : children) {
        if (child->GetNodeType() == type) {
            return child;
        }
    }
    return nullptr;
}

void BaseNode::SetMetadata(const std::string& key, const std::string& value) {
    metadata[key] = value;
}

std::string BaseNode::GetMetadata(const std::string& key) const {
    auto it = metadata.find(key);
    return it != metadata.end() ? it->second : "";
}

bool BaseNode::HasMetadata(const std::string& key) const {
    return metadata.find(key) != metadata.end();
}

bool BaseNode::Validate() const {
    // 基础验证：检查子节点
    for (const auto& child : children) {
        if (!child || !child->Validate()) {
            return false;
        }
    }
    return true;
}

std::string BaseNode::GetDescription() const {
    return NodeTypeToString(nodeType) + " at (" + 
           std::to_string(position.line) + ":" + 
           std::to_string(position.column) + ")";
}

std::string BaseNode::ToString() const {
    std::ostringstream oss;
    oss << GetDescription();
    if (!children.empty()) {
        oss << " [" << children.size() << " children]";
    }
    return oss.str();
}

// ElementNode实现

ElementNode::ElementNode(const std::string& tag, const NodePosition& pos)
    : BaseNode(NodeType::ELEMENT, pos), tagName(tag), isSelfClosing(false) {
}

void ElementNode::SetAttribute(const std::string& name, const std::string& value) {
    attributes[name] = value;
}

std::string ElementNode::GetAttribute(const std::string& name) const {
    auto it = attributes.find(name);
    return it != attributes.end() ? it->second : "";
}

bool ElementNode::HasAttribute(const std::string& name) const {
    return attributes.find(name) != attributes.end();
}

void ElementNode::RemoveAttribute(const std::string& name) {
    attributes.erase(name);
}

void ElementNode::AddCssClass(const std::string& className) {
    std::string currentClass = GetAttribute("class");
    if (currentClass.empty()) {
        SetAttribute("class", className);
    } else {
        // 检查类是否已存在
        if (!HasCssClass(className)) {
            SetAttribute("class", currentClass + " " + className);
        }
    }
}

void ElementNode::RemoveCssClass(const std::string& className) {
    std::string currentClass = GetAttribute("class");
    if (!currentClass.empty()) {
        // 使用正则表达式移除指定类名
        std::regex classRegex("\\b" + className + "\\b");
        std::string newClass = std::regex_replace(currentClass, classRegex, "");
        
        // 清理多余的空格
        std::regex spaceRegex("\\s+");
        newClass = std::regex_replace(newClass, spaceRegex, " ");
        
        // 去除首尾空格
        newClass.erase(0, newClass.find_first_not_of(" "));
        newClass.erase(newClass.find_last_not_of(" ") + 1);
        
        if (newClass.empty()) {
            RemoveAttribute("class");
        } else {
            SetAttribute("class", newClass);
        }
    }
}

bool ElementNode::HasCssClass(const std::string& className) const {
    std::string currentClass = GetAttribute("class");
    if (currentClass.empty()) {
        return false;
    }
    
    std::regex classRegex("\\b" + className + "\\b");
    return std::regex_search(currentClass, classRegex);
}

void ElementNode::SetId(const std::string& id) {
    SetAttribute("id", id);
}

std::string ElementNode::GetId() const {
    return GetAttribute("id");
}

std::string ElementNode::GetTextContent() const {
    std::ostringstream oss;
    for (const auto& child : children) {
        if (child->GetNodeType() == NodeType::TEXT) {
            auto textNode = std::static_pointer_cast<TextNode>(child);
            oss << textNode->GetContent();
        } else if (child->GetNodeType() == NodeType::ELEMENT) {
            auto elementNode = std::static_pointer_cast<ElementNode>(child);
            oss << elementNode->GetTextContent();
        }
    }
    return oss.str();
}

std::shared_ptr<BaseNode> ElementNode::Clone() const {
    auto cloned = std::make_shared<ElementNode>(tagName, position);
    cloned->attributes = attributes;
    cloned->isSelfClosing = isSelfClosing;
    cloned->metadata = metadata;
    
    // 克隆子节点
    for (const auto& child : children) {
        cloned->AddChild(child->Clone());
    }
    
    return cloned;
}

bool ElementNode::Validate() const {
    // 验证标签名
    if (tagName.empty()) {
        return false;
    }
    
    // 验证标签名只包含有效字符
    if (!std::regex_match(tagName, std::regex("[a-zA-Z][a-zA-Z0-9-]*"))) {
        return false;
    }
    
    return BaseNode::Validate();
}

std::string ElementNode::GetDescription() const {
    std::ostringstream oss;
    oss << "Element<" << tagName << ">";
    if (!attributes.empty()) {
        oss << " [" << attributes.size() << " attrs]";
    }
    oss << " at (" << position.line << ":" << position.column << ")";
    return oss.str();
}

void ElementNode::Accept(NodeVisitor& visitor) {
    visitor.VisitElement(*this);
}

// TextNode实现

TextNode::TextNode(const std::string& text, const NodePosition& pos)
    : BaseNode(NodeType::TEXT, pos), content(text), isUnquoted(false) {
}

std::string TextNode::GetProcessedContent() const {
    if (isUnquoted) {
        return content; // 无修饰字面量直接返回
    }
    
    // 处理转义字符
    std::string processed = content;
    std::regex escapeRegex("\\\\([nrtfb\"'\\\\])");
    
    // 使用简单的字符串替换代替lambda
    std::unordered_map<std::string, std::string> escapes = {
        {"\\n", "\n"}, {"\\r", "\r"}, {"\\t", "\t"}, {"\\f", "\f"}, 
        {"\\b", "\b"}, {"\\\"", "\""}, {"\\'", "'"}, {"\\\\", "\\"}
    };
    
    for (const auto& escape : escapes) {
        size_t pos = 0;
        while ((pos = processed.find(escape.first, pos)) != std::string::npos) {
            processed.replace(pos, escape.first.length(), escape.second);
            pos += escape.second.length();
        }
    }
    
    return processed;
}

bool TextNode::IsWhitespace() const {
    return std::all_of(content.begin(), content.end(), [](char c) {
        return std::isspace(c);
    });
}

std::shared_ptr<BaseNode> TextNode::Clone() const {
    auto cloned = std::make_shared<TextNode>(content, position);
    cloned->isUnquoted = isUnquoted;
    cloned->metadata = metadata;
    return cloned;
}

bool TextNode::Validate() const {
    // 文本节点不应该有子节点
    return children.empty();
}

std::string TextNode::GetDescription() const {
    std::ostringstream oss;
    oss << "Text";
    if (isUnquoted) {
        oss << "(unquoted)";
    }
    oss << "[\"" << content.substr(0, 20);
    if (content.length() > 20) {
        oss << "...";
    }
    oss << "\"] at (" << position.line << ":" << position.column << ")";
    return oss.str();
}

void TextNode::Accept(NodeVisitor& visitor) {
    visitor.VisitText(*this);
}

// CommentNode实现

CommentNode::CommentNode(const std::string& text, CommentType type, const NodePosition& pos)
    : BaseNode(NodeType::COMMENT, pos), content(text), commentType(type) {
}

std::string CommentNode::GetCommentTypeName() const {
    switch (commentType) {
        case CommentType::LINE: return "LINE";
        case CommentType::BLOCK: return "BLOCK";
        case CommentType::GENERATOR: return "GENERATOR";
        default: return "UNKNOWN";
    }
}

std::shared_ptr<BaseNode> CommentNode::Clone() const {
    auto cloned = std::make_shared<CommentNode>(content, commentType, position);
    cloned->metadata = metadata;
    return cloned;
}

bool CommentNode::Validate() const {
    // 注释节点不应该有子节点
    return children.empty();
}

std::string CommentNode::GetDescription() const {
    std::ostringstream oss;
    oss << "Comment(" << GetCommentTypeName() << ")";
    oss << "[\"" << content.substr(0, 20);
    if (content.length() > 20) {
        oss << "...";
    }
    oss << "\"] at (" << position.line << ":" << position.column << ")";
    return oss.str();
}

void CommentNode::Accept(NodeVisitor& visitor) {
    visitor.VisitComment(*this);
}

// 辅助函数

std::string NodeTypeToString(NodeType type) {
    switch (type) {
        case NodeType::ELEMENT: return "ELEMENT";
        case NodeType::TEXT: return "TEXT";
        case NodeType::COMMENT: return "COMMENT";
        case NodeType::TEMPLATE: return "TEMPLATE";
        case NodeType::CUSTOM: return "CUSTOM";
        case NodeType::NAMESPACE: return "NAMESPACE";
        case NodeType::CONFIGURATION: return "CONFIGURATION";
        case NodeType::IMPORT: return "IMPORT";
        case NodeType::ORIGIN: return "ORIGIN";
        case NodeType::STYLE_BLOCK: return "STYLE_BLOCK";
        case NodeType::SCRIPT_BLOCK: return "SCRIPT_BLOCK";
        case NodeType::USE_STATEMENT: return "USE_STATEMENT";
        case NodeType::EXCEPT_CLAUSE: return "EXCEPT_CLAUSE";
        case NodeType::OPERATOR: return "OPERATOR";
        case NodeType::ATTRIBUTE: return "ATTRIBUTE";
        case NodeType::ATTRIBUTE_VALUE: return "ATTRIBUTE_VALUE";
        case NodeType::STYLE_PROPERTY: return "STYLE_PROPERTY";
        case NodeType::VARIABLE_REFERENCE: return "VARIABLE_REFERENCE";
        case NodeType::UNKNOWN: return "UNKNOWN";
        default: return "INVALID";
    }
}

// NodeFactory实现

std::shared_ptr<ElementNode> NodeFactory::CreateElement(const std::string& tagName, const NodePosition& pos) {
    return std::make_shared<ElementNode>(tagName, pos);
}

std::shared_ptr<TextNode> NodeFactory::CreateText(const std::string& content, const NodePosition& pos) {
    return std::make_shared<TextNode>(content, pos);
}

std::shared_ptr<CommentNode> NodeFactory::CreateComment(const std::string& content, CommentNode::CommentType type, const NodePosition& pos) {
    return std::make_shared<CommentNode>(content, type, pos);
}

} // namespace CHTL