#include "CHTLNode.h"
#include <algorithm>
#include <sstream>
#include <functional>

namespace CHTL {

void CHTLNode::addChild(std::shared_ptr<CHTLNode> child) {
    if (child) {
        child->parent = this;
        children.push_back(child);
    }
}

void CHTLNode::removeChild(std::shared_ptr<CHTLNode> child) {
    auto it = std::find(children.begin(), children.end(), child);
    if (it != children.end()) {
        (*it)->parent = nullptr;
        children.erase(it);
    }
}

std::shared_ptr<CHTLNode> CHTLNode::findChild(const std::string& name) const {
    for (const auto& child : children) {
        if (child->name == name) {
            return child;
        }
    }
    return nullptr;
}

std::vector<std::shared_ptr<CHTLNode>> CHTLNode::findChildrenByType(NodeType type) const {
    std::vector<std::shared_ptr<CHTLNode>> result;
    for (const auto& child : children) {
        if (child->type == type) {
            result.push_back(child);
        }
    }
    return result;
}

void CHTLNode::setAttribute(const std::string& key, const std::string& value) {
    attributes[key] = value;
}

std::string CHTLNode::getAttribute(const std::string& key) const {
    auto it = attributes.find(key);
    return (it != attributes.end()) ? it->second : "";
}

bool CHTLNode::hasAttribute(const std::string& key) const {
    return attributes.find(key) != attributes.end();
}

void CHTLNode::removeAttribute(const std::string& key) {
    attributes.erase(key);
}

void CHTLNode::traverse(std::function<void(CHTLNode*)> visitor) {
    visitor(this);
    for (auto& child : children) {
        child->traverse(visitor);
    }
}

std::shared_ptr<CHTLNode> CHTLNode::findAncestor(NodeType type) const {
    CHTLNode* current = parent;
    while (current) {
        if (current->type == type) {
            // Need to find the shared_ptr for this raw pointer
            // This is a simplified approach
            break;
        }
        current = current->parent;
    }
    return nullptr; // Simplified - would need proper shared_ptr management
}

bool CHTLNode::isDefinition() const {
    return type == NodeType::TEMPLATE_DEFINITION ||
           type == NodeType::CUSTOM_DEFINITION ||
           type == NodeType::ORIGIN_DEFINITION ||
           type == NodeType::NAMESPACE_DEFINITION ||
           type == NodeType::CONFIGURATION_DEFINITION;
}

bool CHTLNode::isUsage() const {
    return type == NodeType::TEMPLATE_USAGE ||
           type == NodeType::CUSTOM_USAGE ||
           type == NodeType::VARIABLE_USAGE;
}

bool CHTLNode::isBlock() const {
    return type == NodeType::STYLE_BLOCK ||
           type == NodeType::SCRIPT_BLOCK ||
           type == NodeType::HTML_ELEMENT;
}

bool CHTLNode::isLeaf() const {
    return children.empty();
}

std::string CHTLNode::toString() const {
    std::stringstream ss;
    ss << "Node(type=" << static_cast<int>(type) 
       << ", name=" << name 
       << ", value=" << value 
       << ", children=" << children.size() << ")";
    return ss.str();
}

std::string CHTLNode::toXML() const {
    std::stringstream ss;
    ss << "<node type=\"" << static_cast<int>(type) << "\"";
    if (!name.empty()) ss << " name=\"" << name << "\"";
    if (!value.empty()) ss << " value=\"" << value << "\"";
    ss << ">";
    
    for (const auto& child : children) {
        ss << child->toXML();
    }
    
    ss << "</node>";
    return ss.str();
}

bool CHTLNode::isElementNode() const {
    return type == NodeType::HTML_ELEMENT;
}

bool CHTLNode::isTextNode() const {
    return type == NodeType::TEXT_NODE;
}

bool CHTLNode::isStyleNode() const {
    return type == NodeType::STYLE_BLOCK;
}

bool CHTLNode::isScriptNode() const {
    return type == NodeType::SCRIPT_BLOCK;
}

bool CHTLNode::isTemplateNode() const {
    return type == NodeType::TEMPLATE_DEFINITION;
}

bool CHTLNode::isCustomNode() const {
    return type == NodeType::CUSTOM_DEFINITION;
}

// ElementNode implementation
void ElementNode::addAttribute(const std::string& name, const std::string& value) {
    setAttribute(name, value);
}

std::string ElementNode::getAttributeValue(const std::string& name) const {
    return getAttribute(name);
}

// StyleNode implementation
void StyleNode::addProperty(const std::string& property, const std::string& value) {
    properties[property] = value;
}

void StyleNode::addSelector(const std::string& selector) {
    selectors.push_back(selector);
}

// NodeFactory implementation
std::shared_ptr<CHTLNode> NodeFactory::createDocument(const std::string& filename) {
    return std::make_shared<DocumentNode>(filename);
}

std::shared_ptr<CHTLNode> NodeFactory::createElement(const std::string& tag_name) {
    return std::make_shared<ElementNode>(tag_name);
}

std::shared_ptr<CHTLNode> NodeFactory::createText(const std::string& text) {
    return std::make_shared<TextNode>(text);
}

std::shared_ptr<CHTLNode> NodeFactory::createTemplate(const std::string& type, const std::string& name) {
    return std::make_shared<TemplateNode>(type, name);
}

std::shared_ptr<CHTLNode> NodeFactory::createCustom(const std::string& type, const std::string& name) {
    return std::make_shared<CustomNode>(type, name);
}

std::shared_ptr<CHTLNode> NodeFactory::createStyle(bool is_local) {
    return std::make_shared<StyleNode>(is_local);
}

std::shared_ptr<CHTLNode> NodeFactory::createScript(bool is_local) {
    return std::make_shared<ScriptNode>(is_local);
}

std::shared_ptr<CHTLNode> NodeFactory::createImport(const std::string& type, const std::string& path) {
    return std::make_shared<ImportNode>(type, path);
}

std::shared_ptr<CHTLNode> NodeFactory::createComment(const std::string& content, bool is_generator) {
    NodeType type = is_generator ? NodeType::GENERATOR_COMMENT : NodeType::COMMENT;
    return std::make_shared<CHTLNode>(type, content);
}

} // namespace CHTL