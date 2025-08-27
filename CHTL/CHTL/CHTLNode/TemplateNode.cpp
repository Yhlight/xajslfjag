#include "TemplateNode.h"
#include <sstream>

namespace CHTL {

void TemplateNode::setProperty(const std::string& key, const std::string& value) {
    properties[key] = value;
}

std::string TemplateNode::getProperty(const std::string& key) const {
    auto it = properties.find(key);
    return (it != properties.end()) ? it->second : "";
}

bool TemplateNode::hasProperty(const std::string& key) const {
    return properties.find(key) != properties.end();
}

void TemplateNode::removeProperty(const std::string& key) {
    properties.erase(key);
}

void TemplateNode::addInheritance(const std::string& templateName) {
    inheritFrom.push_back(templateName);
}

void TemplateNode::accept(NodeVisitor* visitor) {
    if (auto* templateVisitor = dynamic_cast<TemplateNodeVisitor*>(visitor)) {
        templateVisitor->visitTemplateNode(this);
    }
}

std::shared_ptr<BaseNode> TemplateNode::clone() const {
    auto cloned = std::make_shared<TemplateNode>(name, templateType, line, column);
    
    // 复制属性
    cloned->properties = properties;
    cloned->inheritFrom = inheritFrom;
    
    // 递归克隆子节点
    for (const auto& child : children) {
        cloned->addChild(child->clone());
    }
    
    return cloned;
}

std::string TemplateNode::toString() const {
    std::stringstream ss;
    ss << "[Template] ";
    
    switch (templateType) {
        case TemplateType::STYLE:
            ss << "@Style ";
            break;
        case TemplateType::ELEMENT:
            ss << "@Element ";
            break;
        case TemplateType::VAR:
            ss << "@Var ";
            break;
    }
    
    ss << name << " {\n";
    
    // 输出属性
    for (const auto& [key, value] : properties) {
        ss << "    " << key << ": " << value << ";\n";
    }
    
    // 输出子节点
    for (const auto& child : children) {
        ss << "    " << child->toString() << "\n";
    }
    
    ss << "}";
    
    return ss.str();
}

} // namespace CHTL