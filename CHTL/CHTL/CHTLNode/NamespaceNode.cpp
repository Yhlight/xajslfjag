#include "NamespaceNode.h"
#include <sstream>
#include <algorithm>

namespace CHTL {

void NamespaceNode::addToPath(const std::string& ns) {
    nestedPath.push_back(ns);
}

std::string NamespaceNode::getFullPath() const {
    if (nestedPath.empty()) {
        return name;
    }
    
    std::stringstream ss;
    for (size_t i = 0; i < nestedPath.size(); ++i) {
        if (i > 0) ss << ".";
        ss << nestedPath[i];
    }
    ss << "." << name;
    
    return ss.str();
}

void NamespaceNode::addExcept(const std::string& item) {
    exceptList.push_back(item);
}

bool NamespaceNode::isExcepted(const std::string& item) const {
    return std::find(exceptList.begin(), exceptList.end(), item) != exceptList.end();
}

void NamespaceNode::accept(NodeVisitor* visitor) {
    if (auto* namespaceVisitor = dynamic_cast<NamespaceNodeVisitor*>(visitor)) {
        namespaceVisitor->visitNamespaceNode(this);
    }
}

std::shared_ptr<BaseNode> NamespaceNode::clone() const {
    auto cloned = std::make_shared<NamespaceNode>(name, line, column);
    
    // 复制属性
    cloned->nestedPath = nestedPath;
    cloned->exceptList = exceptList;
    
    // 递归克隆子节点
    for (const auto& child : children) {
        cloned->addChild(child->clone());
    }
    
    return cloned;
}

std::string NamespaceNode::toString() const {
    std::stringstream ss;
    
    ss << "[Namespace] " << name;
    
    // 如果有约束，输出约束
    if (!exceptList.empty()) {
        ss << " {\n";
        for (const auto& except : exceptList) {
            ss << "    except " << except << ";\n";
        }
        
        // 输出子节点
        for (const auto& child : children) {
            ss << "    " << child->toString() << "\n";
        }
        
        ss << "}";
    } else if (!children.empty()) {
        // 只有子节点
        ss << " {\n";
        for (const auto& child : children) {
            ss << "    " << child->toString() << "\n";
        }
        ss << "}";
    }
    
    return ss.str();
}

} // namespace CHTL