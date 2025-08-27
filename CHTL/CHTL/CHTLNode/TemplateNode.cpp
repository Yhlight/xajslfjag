#include "TemplateNode.h"
#include <sstream>

namespace CHTL {

TemplateNode::TemplateNode(TemplateType type, const std::string& name)
    : BaseNode(NodeType::TEMPLATE), templateType(type), templateName(name) {
}

std::string TemplateNode::Generate() const {
    // 模板定义本身不生成任何输出
    // 只有在被使用时才会生成内容
    return "";
}

NodePtr TemplateNode::Clone() const {
    auto clone = std::make_shared<TemplateNode>(templateType, templateName);
    clone->SetPosition(line, column);
    
    // 克隆内容
    if (contentNode) {
        clone->SetContent(contentNode->Clone());
    }
    
    // 复制属性
    clone->styleProperties = styleProperties;
    clone->variables = variables;
    clone->inheritedTemplates = inheritedTemplates;
    
    return clone;
}

void TemplateNode::Accept(NodeVisitor* visitor) {
    visitor->VisitTemplateNode(this);
}

void TemplateNode::AddStyleProperty(const std::string& name, const std::string& value) {
    styleProperties[name] = value;
}

void TemplateNode::AddVariable(const std::string& name, const std::string& value) {
    variables[name] = value;
}

std::string TemplateNode::GetVariable(const std::string& name) const {
    auto it = variables.find(name);
    if (it != variables.end()) {
        return it->second;
    }
    return "";
}

void TemplateNode::AddInheritedTemplate(const std::string& templateName) {
    inheritedTemplates.push_back(templateName);
}

NodePtr TemplateNode::Instantiate(const std::unordered_map<std::string, std::string>& params) const {
    switch (templateType) {
        case TemplateType::STYLE: {
            // 对于样式组模板，返回一个包含所有样式属性的节点
            auto styleNode = std::make_shared<BaseNode>(NodeType::STYLE);
            
            // 先应用继承的模板的属性
            // TODO: 从GlobalMap获取继承的模板并应用
            
            // 应用自身的属性
            for (const auto& [name, value] : styleProperties) {
                // TODO: 创建样式属性节点
            }
            
            return styleNode;
        }
        
        case TemplateType::ELEMENT: {
            // 对于元素模板，克隆内容节点
            if (contentNode) {
                return contentNode->Clone();
            }
            return nullptr;
        }
        
        case TemplateType::VAR: {
            // 变量组模板不直接实例化
            // 其值在使用时通过查找获取
            return nullptr;
        }
        
        default:
            return nullptr;
    }
}

} // namespace CHTL