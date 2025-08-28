#include "CHTLBaseNode.h"
#include <sstream>
#include <algorithm>
#include <map>

namespace CHTL {

CHTLBaseNode::CHTLBaseNode(CHTLNodeType type) : nodeType(type) {
}

std::string CHTLBaseNode::getNodeTypeName() const {
    static const std::map<CHTLNodeType, std::string> typeNames = {
        {CHTLNodeType::BASE, "Base"},
        {CHTLNodeType::ELEMENT, "Element"},
        {CHTLNodeType::TEXT, "Text"},
        {CHTLNodeType::COMMENT, "Comment"},
        {CHTLNodeType::TEMPLATE, "Template"},
        {CHTLNodeType::CUSTOM, "Custom"},
        {CHTLNodeType::STYLE, "Style"},
        {CHTLNodeType::SCRIPT, "Script"},
        {CHTLNodeType::ORIGIN, "Origin"},
        {CHTLNodeType::IMPORT, "Import"},
        {CHTLNodeType::CONFIG, "Config"},
        {CHTLNodeType::NAMESPACE, "Namespace"},
        {CHTLNodeType::OPERATOR, "Operator"}
    };
    
    auto it = typeNames.find(nodeType);
    return it != typeNames.end() ? it->second : "Unknown";
}

void CHTLBaseNode::addChild(std::shared_ptr<CHTLBaseNode> child) {
    if (child) {
        children.push_back(child);
        child->setParent(shared_from_this());
    }
}

void CHTLBaseNode::removeChild(std::shared_ptr<CHTLBaseNode> child) {
    auto it = std::find(children.begin(), children.end(), child);
    if (it != children.end()) {
        (*it)->setParent(nullptr);
        children.erase(it);
    }
}

void CHTLBaseNode::removeChildAt(size_t index) {
    if (index < children.size()) {
        children[index]->setParent(nullptr);
        children.erase(children.begin() + index);
    }
}

void CHTLBaseNode::insertChild(size_t index, std::shared_ptr<CHTLBaseNode> child) {
    if (child && index <= children.size()) {
        children.insert(children.begin() + index, child);
        child->setParent(shared_from_this());
    }
}

std::shared_ptr<CHTLBaseNode> CHTLBaseNode::getChild(size_t index) const {
    return index < children.size() ? children[index] : nullptr;
}

void CHTLBaseNode::setAttribute(const std::string& name, const std::string& value) {
    attributes[name] = value;
}

void CHTLBaseNode::removeAttribute(const std::string& name) {
    attributes.erase(name);
}

std::string CHTLBaseNode::getAttribute(const std::string& name, const std::string& defaultValue) const {
    auto it = attributes.find(name);
    return it != attributes.end() ? it->second : defaultValue;
}

bool CHTLBaseNode::hasAttribute(const std::string& name) const {
    return attributes.find(name) != attributes.end();
}

void CHTLBaseNode::setSourcePosition(size_t line, size_t column) {
    sourceLine = line;
    sourceColumn = column;
}

std::string CHTLBaseNode::toString() const {
    std::ostringstream oss;
    oss << getNodeTypeName() << "Node";
    
    if (!attributes.empty()) {
        oss << " [";
        bool first = true;
        for (const auto& attr : attributes) {
            if (!first) oss << ", ";
            oss << attr.first << "=\"" << attr.second << "\"";
            first = false;
        }
        oss << "]";
    }
    
    if (sourceLine > 0) {
        oss << " @" << sourceLine << ":" << sourceColumn;
    }
    
    return oss.str();
}

std::string CHTLBaseNode::toTreeString(int indent) const {
    std::string result = getIndentString(indent) + toString();
    
    for (const auto& child : children) {
        result += "\n" + child->toTreeString(indent + 1);
    }
    
    return result;
}

std::vector<std::shared_ptr<CHTLBaseNode>> CHTLBaseNode::findNodesByType(CHTLNodeType type) const {
    std::vector<std::shared_ptr<CHTLBaseNode>> result;
    findNodesRecursive(type, result);
    return result;
}

std::shared_ptr<CHTLBaseNode> CHTLBaseNode::findFirstNodeByType(CHTLNodeType type) const {
    if (nodeType == type) {
        return std::const_pointer_cast<CHTLBaseNode>(shared_from_this());
    }
    
    for (const auto& child : children) {
        auto found = child->findFirstNodeByType(type);
        if (found) {
            return found;
        }
    }
    
    return nullptr;
}

void CHTLBaseNode::findNodesRecursive(CHTLNodeType type, std::vector<std::shared_ptr<CHTLBaseNode>>& result) const {
    if (nodeType == type) {
        result.push_back(std::const_pointer_cast<CHTLBaseNode>(shared_from_this()));
    }
    
    for (const auto& child : children) {
        child->findNodesRecursive(type, result);
    }
}

std::string CHTLBaseNode::getIndentString(int indent) const {
    return std::string(indent * 2, ' ');
}

} // namespace CHTL