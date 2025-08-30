#include "BaseNode.h"
#include <algorithm>
#include <sstream>

namespace CHTL {

BaseNode::BaseNode(NodeType type) : m_type(type) {}

BaseNode::~BaseNode() = default;

void BaseNode::setAttribute(const std::string& name, const std::string& value) {
    m_attributes[name] = value;
}

std::string BaseNode::getAttribute(const std::string& name) const {
    auto it = m_attributes.find(name);
    if (it != m_attributes.end()) {
        return it->second;
    }
    return "";
}

bool BaseNode::hasAttribute(const std::string& name) const {
    return m_attributes.find(name) != m_attributes.end();
}

void BaseNode::addChild(std::shared_ptr<BaseNode> child) {
    if (child) {
        child->setParent(shared_from_this());
        m_children.push_back(child);
    }
}

void BaseNode::removeChild(std::shared_ptr<BaseNode> child) {
    auto it = std::find(m_children.begin(), m_children.end(), child);
    if (it != m_children.end()) {
        (*it)->setParent(nullptr);
        m_children.erase(it);
    }
}

void BaseNode::insertChild(size_t index, std::shared_ptr<BaseNode> child) {
    if (child && index <= m_children.size()) {
        child->setParent(shared_from_this());
        m_children.insert(m_children.begin() + index, child);
    }
}

std::shared_ptr<BaseNode> BaseNode::getChild(size_t index) const {
    if (index < m_children.size()) {
        return m_children[index];
    }
    return nullptr;
}

std::shared_ptr<BaseNode> BaseNode::findChild(NodeType type) const {
    for (const auto& child : m_children) {
        if (child->getType() == type) {
            return child;
        }
    }
    return nullptr;
}

std::shared_ptr<BaseNode> BaseNode::findChild(const std::string& value) const {
    for (const auto& child : m_children) {
        if (child->getValue() == value) {
            return child;
        }
    }
    return nullptr;
}

std::vector<std::shared_ptr<BaseNode>> BaseNode::findChildren(NodeType type) const {
    std::vector<std::shared_ptr<BaseNode>> result;
    
    for (const auto& child : m_children) {
        if (child->getType() == type) {
            result.push_back(child);
        }
    }
    
    return result;
}

std::string BaseNode::toString() const {
    std::ostringstream ss;
    ss << "Node(type=" << static_cast<int>(m_type) << ", value=" << m_value << ")";
    return ss.str();
}

}