#include "BaseNode.h"
#include <algorithm>
#include <sstream>
#include <queue>

namespace CHTL {

// ========== BaseNode 实现 ==========

BaseNode::BaseNode(CHTLNodeType type, const std::string& name)
    : m_nodeType(type)
    , m_name(name)
    , m_access(NodeAccess::PUBLIC)
    , m_parent(nullptr) {
}

CHTLNodeType BaseNode::getNodeType() const {
    return m_nodeType;
}

const std::string& BaseNode::getName() const {
    return m_name;
}

void BaseNode::setName(const std::string& name) {
    m_name = name;
}

const std::string& BaseNode::getValue() const {
    return m_value;
}

void BaseNode::setValue(const std::string& value) {
    m_value = value;
}

const Position& BaseNode::getPosition() const {
    return m_position;
}

void BaseNode::setPosition(const Position& position) {
    m_position = position;
}

NodeAccess BaseNode::getAccess() const {
    return m_access;
}

void BaseNode::setAccess(NodeAccess access) {
    m_access = access;
}

BaseNode* BaseNode::getParent() const {
    return m_parent;
}

void BaseNode::setParent(BaseNode* parent) {
    m_parent = parent;
}

size_t BaseNode::getChildCount() const {
    return m_children.size();
}

BaseNode* BaseNode::getChild(size_t index) const {
    if (index >= m_children.size()) {
        return nullptr;
    }
    return m_children[index].get();
}

const std::vector<std::shared_ptr<BaseNode>>& BaseNode::getChildren() const {
    return m_children;
}

void BaseNode::addChild(std::shared_ptr<BaseNode> child) {
    if (child) {
        child->setParent(this);
        m_children.push_back(child);
    }
}

void BaseNode::insertChild(size_t index, std::shared_ptr<BaseNode> child) {
    if (child && index <= m_children.size()) {
        child->setParent(this);
        m_children.insert(m_children.begin() + index, child);
    }
}

bool BaseNode::removeChild(BaseNode* child) {
    if (!child) return false;
    
    auto it = std::find_if(m_children.begin(), m_children.end(),
                          [child](const std::shared_ptr<BaseNode>& node) {
                              return node.get() == child;
                          });
    
    if (it != m_children.end()) {
        (*it)->setParent(nullptr);
        m_children.erase(it);
        return true;
    }
    
    return false;
}

bool BaseNode::removeChild(size_t index) {
    if (index >= m_children.size()) {
        return false;
    }
    
    m_children[index]->setParent(nullptr);
    m_children.erase(m_children.begin() + index);
    return true;
}

void BaseNode::clearChildren() {
    for (auto& child : m_children) {
        child->setParent(nullptr);
    }
    m_children.clear();
}

void BaseNode::setAttribute(const std::string& key, const std::string& value) {
    m_attributes[key] = value;
}

std::string BaseNode::getAttribute(const std::string& key) const {
    auto it = m_attributes.find(key);
    return (it != m_attributes.end()) ? it->second : "";
}

bool BaseNode::hasAttribute(const std::string& key) const {
    return m_attributes.find(key) != m_attributes.end();
}

bool BaseNode::removeAttribute(const std::string& key) {
    auto it = m_attributes.find(key);
    if (it != m_attributes.end()) {
        m_attributes.erase(it);
        return true;
    }
    return false;
}

const std::unordered_map<std::string, std::string>& BaseNode::getAttributes() const {
    return m_attributes;
}

BaseNode* BaseNode::findChildByName(const std::string& name) const {
    for (const auto& child : m_children) {
        if (child->getName() == name) {
            return child.get();
        }
    }
    return nullptr;
}

std::vector<BaseNode*> BaseNode::findChildrenByType(CHTLNodeType type) const {
    std::vector<BaseNode*> result;
    for (const auto& child : m_children) {
        if (child->getNodeType() == type) {
            result.push_back(child.get());
        }
    }
    return result;
}

BaseNode* BaseNode::findChildByNameAndType(const std::string& name, CHTLNodeType type) const {
    for (const auto& child : m_children) {
        if (child->getName() == name && child->getNodeType() == type) {
            return child.get();
        }
    }
    return nullptr;
}

BaseNode* BaseNode::findNodeDFS(const std::string& name) const {
    if (m_name == name) {
        return const_cast<BaseNode*>(this);
    }
    
    for (const auto& child : m_children) {
        BaseNode* found = child->findNodeDFS(name);
        if (found) {
            return found;
        }
    }
    
    return nullptr;
}

BaseNode* BaseNode::findNodeBFS(const std::string& name) const {
    std::queue<const BaseNode*> queue;
    queue.push(this);
    
    while (!queue.empty()) {
        const BaseNode* current = queue.front();
        queue.pop();
        
        if (current->getName() == name) {
            return const_cast<BaseNode*>(current);
        }
        
        for (const auto& child : current->getChildren()) {
            queue.push(child.get());
        }
    }
    
    return nullptr;
}

std::shared_ptr<BaseNode> BaseNode::clone() const {
    auto cloned = std::make_shared<BaseNode>(m_nodeType, m_name);
    cloned->m_value = m_value;
    cloned->m_position = m_position;
    cloned->m_access = m_access;
    cloned->m_attributes = m_attributes;
    return cloned;
}

std::shared_ptr<BaseNode> BaseNode::deepClone() const {
    auto cloned = clone();
    
    for (const auto& child : m_children) {
        auto clonedChild = child->deepClone();
        cloned->addChild(clonedChild);
    }
    
    return cloned;
}

void BaseNode::accept(class NodeVisitor& visitor) {
    // 基础实现，子类可以重写
}

std::string BaseNode::toString() const {
    std::stringstream ss;
    ss << getNodeTypeString() << "(" << m_name << ")";
    if (!m_value.empty()) {
        ss << ": " << m_value;
    }
    return ss.str();
}

std::string BaseNode::toXml() const {
    std::stringstream ss;
    ss << "<" << getNodeTypeString();
    
    if (!m_name.empty()) {
        ss << " name=\"" << m_name << "\"";
    }
    
    for (const auto& attr : m_attributes) {
        ss << " " << attr.first << "=\"" << attr.second << "\"";
    }
    
    if (m_children.empty() && m_value.empty()) {
        ss << "/>";
    } else {
        ss << ">";
        
        if (!m_value.empty()) {
            ss << m_value;
        }
        
        for (const auto& child : m_children) {
            ss << child->toXml();
        }
        
        ss << "</" << getNodeTypeString() << ">";
    }
    
    return ss.str();
}

std::string BaseNode::toJson() const {
    std::stringstream ss;
    ss << "{";
    ss << "\"type\":\"" << getNodeTypeString() << "\"";
    
    if (!m_name.empty()) {
        ss << ",\"name\":\"" << m_name << "\"";
    }
    
    if (!m_value.empty()) {
        ss << ",\"value\":\"" << m_value << "\"";
    }
    
    if (!m_attributes.empty()) {
        ss << ",\"attributes\":{";
        bool first = true;
        for (const auto& attr : m_attributes) {
            if (!first) ss << ",";
            ss << "\"" << attr.first << "\":\"" << attr.second << "\"";
            first = false;
        }
        ss << "}";
    }
    
    if (!m_children.empty()) {
        ss << ",\"children\":[";
        bool first = true;
        for (const auto& child : m_children) {
            if (!first) ss << ",";
            ss << child->toJson();
            first = false;
        }
        ss << "]";
    }
    
    ss << "}";
    return ss.str();
}

std::string BaseNode::getDebugInfo() const {
    std::stringstream ss;
    ss << "Node: " << getNodeTypeString() << "\n";
    ss << "Name: " << m_name << "\n";
    ss << "Value: " << m_value << "\n";
    ss << "Position: " << m_position.line << ":" << m_position.column << "\n";
    ss << "Children: " << m_children.size() << "\n";
    ss << "Attributes: " << m_attributes.size() << "\n";
    return ss.str();
}

bool BaseNode::validate(ErrorReporter* errorReporter) const {
    return internalValidate(errorReporter);
}

bool BaseNode::checkIntegrity() const {
    // 检查父子关系的一致性
    for (const auto& child : m_children) {
        if (child->getParent() != this) {
            return false;
        }
        
        if (!child->checkIntegrity()) {
            return false;
        }
    }
    
    return true;
}

bool BaseNode::internalValidate(ErrorReporter* errorReporter) const {
    // 基础验证逻辑
    if (m_name.empty() && m_nodeType != CHTLNodeType::ROOT_NODE) {
        if (errorReporter) {
            errorReporter->warning("Node has empty name", ErrorPosition("", m_position.line, m_position.column));
        }
        return false;
    }
    
    return true;
}

std::string BaseNode::getNodeTypeString() const {
    switch (m_nodeType) {
        case CHTLNodeType::ROOT_NODE: return "ROOT";
        case CHTLNodeType::ELEMENT_NODE: return "ELEMENT";
        case CHTLNodeType::TEXT_NODE: return "TEXT";
        case CHTLNodeType::COMMENT_NODE: return "COMMENT";
        case CHTLNodeType::ATTRIBUTE_NODE: return "ATTRIBUTE";
        case CHTLNodeType::STYLE_NODE: return "STYLE";
        case CHTLNodeType::INLINE_STYLE_NODE: return "INLINE_STYLE";
        case CHTLNodeType::SELECTOR_STYLE_NODE: return "SELECTOR_STYLE";
        case CHTLNodeType::SCRIPT_NODE: return "SCRIPT";
        case CHTLNodeType::TEMPLATE_STYLE_NODE: return "TEMPLATE_STYLE";
        case CHTLNodeType::TEMPLATE_ELEMENT_NODE: return "TEMPLATE_ELEMENT";
        case CHTLNodeType::TEMPLATE_VAR_NODE: return "TEMPLATE_VAR";
        case CHTLNodeType::CUSTOM_STYLE_NODE: return "CUSTOM_STYLE";
        case CHTLNodeType::CUSTOM_ELEMENT_NODE: return "CUSTOM_ELEMENT";
        case CHTLNodeType::CUSTOM_VAR_NODE: return "CUSTOM_VAR";
        case CHTLNodeType::ORIGIN_HTML_NODE: return "ORIGIN_HTML";
        case CHTLNodeType::ORIGIN_STYLE_NODE: return "ORIGIN_STYLE";
        case CHTLNodeType::ORIGIN_JAVASCRIPT_NODE: return "ORIGIN_JAVASCRIPT";
        case CHTLNodeType::ORIGIN_CUSTOM_NODE: return "ORIGIN_CUSTOM";
        case CHTLNodeType::IMPORT_HTML_NODE: return "IMPORT_HTML";
        case CHTLNodeType::IMPORT_STYLE_NODE: return "IMPORT_STYLE";
        case CHTLNodeType::IMPORT_JAVASCRIPT_NODE: return "IMPORT_JAVASCRIPT";
        case CHTLNodeType::IMPORT_CHTL_NODE: return "IMPORT_CHTL";
        case CHTLNodeType::IMPORT_CJMOD_NODE: return "IMPORT_CJMOD";
        case CHTLNodeType::IMPORT_CONFIG_NODE: return "IMPORT_CONFIG";
        case CHTLNodeType::IMPORT_TEMPLATE_NODE: return "IMPORT_TEMPLATE";
        case CHTLNodeType::IMPORT_CUSTOM_NODE: return "IMPORT_CUSTOM";
        case CHTLNodeType::IMPORT_ORIGIN_NODE: return "IMPORT_ORIGIN";
        case CHTLNodeType::CONFIG_NODE: return "CONFIG";
        case CHTLNodeType::NAMESPACE_NODE: return "NAMESPACE";
        case CHTLNodeType::USE_NODE: return "USE";
        case CHTLNodeType::INHERIT_NODE: return "INHERIT";
        case CHTLNodeType::DELETE_NODE: return "DELETE";
        case CHTLNodeType::INSERT_NODE: return "INSERT";
        case CHTLNodeType::EXCEPT_NODE: return "EXCEPT";
        case CHTLNodeType::VARIABLE_NODE: return "VARIABLE";
        case CHTLNodeType::SELECTOR_NODE: return "SELECTOR";
        case CHTLNodeType::INDEX_ACCESS_NODE: return "INDEX_ACCESS";
        case CHTLNodeType::UNKNOWN_NODE: return "UNKNOWN";
        default: return "UNDEFINED";
    }
}

// ========== NodeFactory 实现 ==========

NodePtr NodeFactory::createNode(CHTLNodeType type, const std::string& name) {
    return std::make_shared<BaseNode>(type, name);
}

NodePtr NodeFactory::createFromToken(const Token& token) {
    CHTLNodeType nodeType = CHTLNodeType::UNKNOWN_NODE;
    
    switch (token.type) {
        case TokenType::HTML_ELEMENT:
            nodeType = CHTLNodeType::ELEMENT_NODE;
            break;
        case TokenType::TEXT:
            nodeType = CHTLNodeType::TEXT_NODE;
            break;
        case TokenType::STYLE:
            nodeType = CHTLNodeType::STYLE_NODE;
            break;
        case TokenType::TEMPLATE:
            nodeType = CHTLNodeType::TEMPLATE_STYLE_NODE;
            break;
        case TokenType::CUSTOM:
            nodeType = CHTLNodeType::CUSTOM_STYLE_NODE;
            break;
        case TokenType::ORIGIN:
            nodeType = CHTLNodeType::ORIGIN_HTML_NODE;
            break;
        case TokenType::IMPORT:
            nodeType = CHTLNodeType::IMPORT_HTML_NODE;
            break;
        case TokenType::NAMESPACE:
            nodeType = CHTLNodeType::NAMESPACE_NODE;
            break;
        case TokenType::CONFIGURATION:
            nodeType = CHTLNodeType::CONFIG_NODE;
            break;
        default:
            nodeType = CHTLNodeType::UNKNOWN_NODE;
            break;
    }
    
    auto node = std::make_shared<BaseNode>(nodeType, token.value);
    node->setPosition(token.position);
    return node;
}

NodePtr NodeFactory::createTextNode(const std::string& text) {
    auto node = std::make_shared<BaseNode>(CHTLNodeType::TEXT_NODE, "text");
    node->setValue(text);
    return node;
}

NodePtr NodeFactory::createElementNode(const std::string& tagName) {
    return std::make_shared<BaseNode>(CHTLNodeType::ELEMENT_NODE, tagName);
}

NodePtr NodeFactory::createAttributeNode(const std::string& name, const std::string& value) {
    auto node = std::make_shared<BaseNode>(CHTLNodeType::ATTRIBUTE_NODE, name);
    node->setValue(value);
    return node;
}

} // namespace CHTL