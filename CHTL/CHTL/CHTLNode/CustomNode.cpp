#include "CustomNode.h"
#include <algorithm>

namespace CHTL {

CustomNode::CustomNode(CustomType type, const std::string& name)
    : BaseNode(NodeType::CUSTOM), customType(type), customName(name) {
}

std::string CustomNode::Generate() const {
    // 自定义定义本身不生成输出
    return "";
}

NodePtr CustomNode::Clone() const {
    auto clone = std::make_shared<CustomNode>(customType, customName);
    clone->SetPosition(line, column);
    
    if (contentNode) {
        clone->SetContent(contentNode->Clone());
    }
    
    clone->styleProperties = styleProperties;
    clone->variables = variables;
    clone->inheritedCustoms = inheritedCustoms;
    clone->deletedProperties = deletedProperties;
    clone->deletedInheritances = deletedInheritances;
    
    return clone;
}

void CustomNode::Accept(NodeVisitor* visitor) {
    visitor->VisitCustomNode(this);
}

void CustomNode::AddStyleProperty(const std::string& name, const std::string& value) {
    styleProperties[name] = value;
}

bool CustomNode::HasStyleProperty(const std::string& name) const {
    return styleProperties.find(name) != styleProperties.end();
}

std::string CustomNode::GetStyleProperty(const std::string& name) const {
    auto it = styleProperties.find(name);
    if (it != styleProperties.end()) {
        return it->second;
    }
    return "";
}

void CustomNode::DeleteProperty(const std::string& name) {
    styleProperties.erase(name);
    deletedProperties.insert(name);
}

void CustomNode::DeleteInheritance(const std::string& type, const std::string& name) {
    std::string key = type + ":" + name;
    deletedInheritances.insert(key);
    
    // 从继承列表中移除
    inheritedCustoms.erase(
        std::remove_if(inheritedCustoms.begin(), inheritedCustoms.end(),
            [&](const std::pair<std::string, std::string>& pair) {
                return pair.first == type && pair.second == name;
            }),
        inheritedCustoms.end()
    );
}

void CustomNode::AddInheritedCustom(const std::string& type, const std::string& name) {
    inheritedCustoms.push_back({type, name});
}

void CustomNode::AddVariable(const std::string& name, const std::string& value) {
    variables[name] = value;
}

std::string CustomNode::GetVariable(const std::string& name) const {
    auto it = variables.find(name);
    if (it != variables.end()) {
        return it->second;
    }
    return "";
}

bool CustomNode::HasVariable(const std::string& name) const {
    return variables.find(name) != variables.end();
}

NodePtr CustomNode::Instantiate(const std::unordered_map<std::string, std::string>& params,
                               const std::set<std::string>& deletions) const {
    // TODO: 实现特例化实例化逻辑
    // 1. 应用继承的自定义
    // 2. 应用自身的属性
    // 3. 应用参数覆盖
    // 4. 处理删除操作
    
    return Clone();
}

bool CustomNode::IsValuelessStyleGroup() const {
    if (customType != CustomType::STYLE) {
        return false;
    }
    
    // 检查是否有无值属性
    for (const auto& [name, value] : styleProperties) {
        if (value.empty()) {
            return true;
        }
    }
    
    return false;
}

} // namespace CHTL