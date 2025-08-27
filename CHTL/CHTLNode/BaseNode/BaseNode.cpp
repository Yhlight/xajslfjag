#include "BaseNode.h"
#include <algorithm>
#include <queue>

namespace CHTL {

BaseNode::BaseNode(NodeType type, const Position& pos)
    : type_(type), position_(pos) {}

void BaseNode::addChild(std::shared_ptr<BaseNode> child) {
    if (child) {
        child->setParent(shared_from_this());
        children_.push_back(child);
        onChildAdded(child);
    }
}

void BaseNode::removeChild(std::shared_ptr<BaseNode> child) {
    auto it = std::find(children_.begin(), children_.end(), child);
    if (it != children_.end()) {
        (*it)->setParent(nullptr);
        children_.erase(it);
        onChildRemoved(child);
    }
}

void BaseNode::insertChild(size_t index, std::shared_ptr<BaseNode> child) {
    if (child && index <= children_.size()) {
        child->setParent(shared_from_this());
        children_.insert(children_.begin() + index, child);
        onChildAdded(child);
    }
}

std::shared_ptr<BaseNode> BaseNode::getChild(size_t index) const {
    return (index < children_.size()) ? children_[index] : nullptr;
}

void BaseNode::setAttribute(const std::string& name, const std::string& value) {
    attributes_[name] = value;
    onAttributeChanged(name, value);
}

std::string BaseNode::getAttribute(const std::string& name) const {
    auto it = attributes_.find(name);
    return (it != attributes_.end()) ? it->second : "";
}

bool BaseNode::hasAttribute(const std::string& name) const {
    return attributes_.find(name) != attributes_.end();
}

void BaseNode::removeAttribute(const std::string& name) {
    attributes_.erase(name);
}

std::shared_ptr<BaseNode> BaseNode::findChild(NodeType type) const {
    for (const auto& child : children_) {
        if (child && child->getType() == type) {
            return child;
        }
    }
    return nullptr;
}

std::vector<std::shared_ptr<BaseNode>> BaseNode::findChildren(NodeType type) const {
    std::vector<std::shared_ptr<BaseNode>> result;
    for (const auto& child : children_) {
        if (child && child->getType() == type) {
            result.push_back(child);
        }
    }
    return result;
}

std::shared_ptr<BaseNode> BaseNode::findChildByName(const std::string& name) const {
    for (const auto& child : children_) {
        if (child && child->getName() == name) {
            return child;
        }
    }
    return nullptr;
}

void BaseNode::traverse(std::function<void(std::shared_ptr<BaseNode>)> visitor) {
    traverseDepthFirst(visitor);
}

void BaseNode::traverseDepthFirst(std::function<void(std::shared_ptr<BaseNode>)> visitor) {
    visitor(shared_from_this());
    for (const auto& child : children_) {
        if (child) {
            child->traverseDepthFirst(visitor);
        }
    }
}

void BaseNode::traverseBreadthFirst(std::function<void(std::shared_ptr<BaseNode>)> visitor) {
    std::queue<std::shared_ptr<BaseNode>> queue;
    queue.push(shared_from_this());
    
    while (!queue.empty()) {
        auto node = queue.front();
        queue.pop();
        
        visitor(node);
        
        for (const auto& child : node->getChildren()) {
            if (child) {
                queue.push(child);
            }
        }
    }
}

void BaseNode::setMetadata(const std::string& key, const std::string& value) {
    metadata_[key] = value;
}

std::string BaseNode::getMetadata(const std::string& key) const {
    auto it = metadata_.find(key);
    return (it != metadata_.end()) ? it->second : "";
}

bool BaseNode::hasMetadata(const std::string& key) const {
    return metadata_.find(key) != metadata_.end();
}

std::string nodeTypeToString(NodeType type) {
    switch (type) {
        case NodeType::BASE_NODE: return "BASE_NODE";
        case NodeType::ELEMENT_NODE: return "ELEMENT_NODE";
        case NodeType::TEXT_NODE: return "TEXT_NODE";
        case NodeType::COMMENT_NODE: return "COMMENT_NODE";
        case NodeType::TEMPLATE_NODE: return "TEMPLATE_NODE";
        case NodeType::CUSTOM_NODE: return "CUSTOM_NODE";
        case NodeType::STYLE_NODE: return "STYLE_NODE";
        case NodeType::SCRIPT_NODE: return "SCRIPT_NODE";
        case NodeType::ORIGIN_NODE: return "ORIGIN_NODE";
        case NodeType::IMPORT_NODE: return "IMPORT_NODE";
        case NodeType::CONFIG_NODE: return "CONFIG_NODE";
        case NodeType::NAMESPACE_NODE: return "NAMESPACE_NODE";
        case NodeType::OPERATOR_NODE: return "OPERATOR_NODE";
        case NodeType::DELETE_NODE: return "DELETE_NODE";
        case NodeType::INSERT_NODE: return "INSERT_NODE";
        case NodeType::INHERIT_NODE: return "INHERIT_NODE";
        case NodeType::USE_NODE: return "USE_NODE";
        case NodeType::EXCEPT_NODE: return "EXCEPT_NODE";
        case NodeType::FROM_NODE: return "FROM_NODE";
        default: return "UNKNOWN";
    }
}

} // namespace CHTL