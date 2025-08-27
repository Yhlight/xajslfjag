#include "CustomNode.h"
#include <sstream>

namespace CHTL {

void CustomNode::setProperty(const std::string& key, const std::string& value) {
    properties[key] = value;
    // 如果设置了值，从无值属性中移除
    noValueProperties.erase(key);
}

std::string CustomNode::getProperty(const std::string& key) const {
    auto it = properties.find(key);
    return (it != properties.end()) ? it->second : "";
}

bool CustomNode::hasProperty(const std::string& key) const {
    return properties.find(key) != properties.end() || 
           noValueProperties.find(key) != noValueProperties.end();
}

void CustomNode::removeProperty(const std::string& key) {
    properties.erase(key);
    noValueProperties.erase(key);
}

void CustomNode::addNoValueProperty(const std::string& prop) {
    noValueProperties.insert(prop);
    // 如果在有值属性中存在，移除它
    properties.erase(prop);
}

bool CustomNode::hasNoValueProperty(const std::string& prop) const {
    return noValueProperties.find(prop) != noValueProperties.end();
}

void CustomNode::addInheritance(const std::string& name) {
    inheritFrom.push_back(name);
}

void CustomNode::addDeletedProperty(const std::string& prop) {
    deletedProperties.push_back(prop);
}

void CustomNode::addDeletedInheritance(const std::string& name) {
    deletedInheritances.push_back(name);
}

void CustomNode::accept(NodeVisitor* visitor) {
    if (auto* customVisitor = dynamic_cast<CustomNodeVisitor*>(visitor)) {
        customVisitor->visitCustomNode(this);
    }
}

std::shared_ptr<BaseNode> CustomNode::clone() const {
    auto cloned = std::make_shared<CustomNode>(name, customType, line, column);
    
    // 复制所有属性
    cloned->properties = properties;
    cloned->noValueProperties = noValueProperties;
    cloned->inheritFrom = inheritFrom;
    cloned->deletedProperties = deletedProperties;
    cloned->deletedInheritances = deletedInheritances;
    
    // 递归克隆子节点
    for (const auto& child : children) {
        cloned->addChild(child->clone());
    }
    
    return cloned;
}

std::string CustomNode::toString() const {
    std::stringstream ss;
    ss << "[Custom] ";
    
    switch (customType) {
        case CustomType::STYLE:
            ss << "@Style ";
            break;
        case CustomType::ELEMENT:
            ss << "@Element ";
            break;
        case CustomType::VAR:
            ss << "@Var ";
            break;
    }
    
    ss << name << " {\n";
    
    // 输出无值属性
    for (const auto& prop : noValueProperties) {
        ss << "    " << prop << ",\n";
    }
    
    // 输出有值属性
    for (const auto& [key, value] : properties) {
        ss << "    " << key << ": " << value << ";\n";
    }
    
    // 输出继承
    for (const auto& inherit : inheritFrom) {
        ss << "    inherit " << inherit << ";\n";
    }
    
    // 输出删除操作
    for (const auto& del : deletedProperties) {
        ss << "    delete " << del << ";\n";
    }
    
    // 输出子节点
    for (const auto& child : children) {
        ss << "    " << child->toString() << "\n";
    }
    
    ss << "}";
    
    return ss.str();
}

} // namespace CHTL