#include "TemplateNode.h"
#include "NodeVisitor.h"
#include <sstream>
#include <algorithm>
#include <regex>
#include <unordered_set>

namespace CHTL {

TemplateNode::TemplateNode(TemplateType type, const std::string& name)
    : BaseNode(NodeType::TEMPLATE, name), template_type_(type), 
      template_name_(name), is_abstract_(false) {
}

void TemplateNode::addInheritance(const std::string& parent_template) {
    if (std::find(inheritance_chain_.begin(), inheritance_chain_.end(), parent_template) 
        == inheritance_chain_.end()) {
        inheritance_chain_.push_back(parent_template);
    }
}

bool TemplateNode::removeInheritance(const std::string& parent_template) {
    auto it = std::find(inheritance_chain_.begin(), inheritance_chain_.end(), parent_template);
    if (it != inheritance_chain_.end()) {
        inheritance_chain_.erase(it);
        return true;
    }
    return false;
}

void TemplateNode::clearInheritance() {
    inheritance_chain_.clear();
}

bool TemplateNode::inheritsFrom(const std::string& template_name) const {
    return std::find(inheritance_chain_.begin(), inheritance_chain_.end(), template_name) 
           != inheritance_chain_.end();
}

void TemplateNode::addVariable(const VariableDefinition& variable) {
    if (isValidVariableName(variable.name)) {
        variables_[variable.name] = variable;
    }
}

const VariableDefinition* TemplateNode::getVariable(const std::string& name) const {
    auto it = variables_.find(name);
    return (it != variables_.end()) ? &it->second : nullptr;
}

bool TemplateNode::removeVariable(const std::string& name) {
    return variables_.erase(name) > 0;
}

bool TemplateNode::hasVariable(const std::string& name) const {
    return variables_.find(name) != variables_.end();
}

std::vector<std::string> TemplateNode::getVariableNames() const {
    std::vector<std::string> names;
    for (const auto& var : variables_) {
        names.push_back(var.first);
    }
    std::sort(names.begin(), names.end());
    return names;
}

bool TemplateNode::expandTo(NodePtr target_node, 
                           const std::unordered_map<std::string, std::string>& parameters) const {
    if (!target_node) {
        return false;
    }

    try {
        switch (template_type_) {
            case TemplateType::STYLE:
                // 展开样式模板：复制样式属性到目标节点
                for (const auto& attr : attributes_) {
                    std::string resolved_value = resolveVariableReferences(attr.second, parameters);
                    target_node->setAttribute(attr.first, resolved_value);
                }
                break;

            case TemplateType::ELEMENT:
                // 展开元素模板：复制子节点到目标节点
                for (const auto& child : children_) {
                    NodePtr cloned_child = child->clone(true);
                    // 解析子节点中的变量引用
                    // 这里需要递归处理子节点的属性和内容
                    target_node->addChild(cloned_child);
                }
                break;

            case TemplateType::VAR:
                // 变量组模板不需要直接展开，而是在使用时替换变量值
                break;
        }

        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool TemplateNode::merge(const TemplateNode& other) {
    if (template_type_ != other.template_type_) {
        return false; // 不同类型的模板不能合并
    }

    // 合并属性
    for (const auto& attr : other.attributes_) {
        setAttribute(attr.first, attr.second);
    }

    // 合并子节点
    for (const auto& child : other.children_) {
        addChild(child->clone(true));
    }

    // 合并变量定义
    for (const auto& var : other.variables_) {
        variables_[var.first] = var.second;
    }

    // 合并继承链
    for (const auto& parent : other.inheritance_chain_) {
        addInheritance(parent);
    }

    return true;
}

std::string TemplateNode::validateTemplate() const {
    // 检查模板名称
    if (template_name_.empty()) {
        return "Template name cannot be empty";
    }

    // 检查循环继承
    std::unordered_set<std::string> visited;
    if (hasCircularInheritance(visited)) {
        return "Circular inheritance detected in template: " + template_name_;
    }

    // 根据模板类型进行特定验证
    switch (template_type_) {
        case TemplateType::STYLE:
            // 样式模板应该有样式属性
            if (attributes_.empty()) {
                return "Style template should have style attributes";
            }
            break;

        case TemplateType::ELEMENT:
            // 元素模板应该有子节点或属性
            if (children_.empty() && attributes_.empty()) {
                return "Element template should have children or attributes";
            }
            break;

        case TemplateType::VAR:
            // 变量组模板应该有变量定义
            if (variables_.empty()) {
                return "Variable template should have variable definitions";
            }
            break;
    }

    return ""; // 验证通过
}

bool TemplateNode::hasCircularInheritance(std::unordered_set<std::string>& visited) const {
    if (visited.find(template_name_) != visited.end()) {
        return true; // 发现循环
    }

    visited.insert(template_name_);

    // 这里需要访问模板管理器来检查父模板
    // 由于我们还没有实现模板管理器，先简单返回false
    // 在实际实现中，需要递归检查所有父模板

    visited.erase(template_name_);
    return false;
}

NodePtr TemplateNode::clone(bool deep) const {
    auto cloned = std::make_shared<TemplateNode>(template_type_, template_name_);
    cloned->attributes_ = attributes_;
    cloned->start_pos_ = start_pos_;
    cloned->end_pos_ = end_pos_;
    cloned->raw_content_ = raw_content_;
    cloned->inheritance_chain_ = inheritance_chain_;
    cloned->variables_ = variables_;
    cloned->is_abstract_ = is_abstract_;

    if (deep) {
        for (const auto& child : children_) {
            cloned->addChild(child->clone(true));
        }
    }

    return cloned;
}

void TemplateNode::accept(NodeVisitor& visitor) {
    visitor.visitTemplate(*this);
}

std::string TemplateNode::toString(int indent) const {
    std::ostringstream oss;
    oss << getIndent(indent) << "TemplateNode{type=" << getTemplateTypeString()
        << ", name=\"" << template_name_ << "\"";

    if (is_abstract_) {
        oss << ", abstract=true";
    }

    if (!inheritance_chain_.empty()) {
        oss << ", inherits=[";
        for (size_t i = 0; i < inheritance_chain_.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << inheritance_chain_[i];
        }
        oss << "]";
    }

    if (!variables_.empty()) {
        oss << ", variables=" << variables_.size();
    }

    oss << "}";

    if (!children_.empty()) {
        oss << " {" << std::endl;
        for (const auto& child : children_) {
            oss << child->toString(indent + 1) << std::endl;
        }
        oss << getIndent(indent) << "}";
    }

    return oss.str();
}

std::string TemplateNode::generateTemplateCode(int indent) const {
    std::ostringstream oss;
    oss << getIndent(indent) << "[Template] @" << getTemplateTypeString() 
        << " " << template_name_;

    if (!inheritance_chain_.empty()) {
        oss << std::endl << getIndent(indent) << "{" << std::endl;
        
        // 添加继承声明
        for (const auto& parent : inheritance_chain_) {
            oss << getIndent(indent + 1) << "@" << getTemplateTypeString() 
                << " " << parent << ";" << std::endl;
        }
        
        // 添加模板内容
        if (template_type_ == TemplateType::VAR) {
            // 变量组模板
            for (const auto& var : variables_) {
                oss << getIndent(indent + 1) << var.second.name << ": "
                    << var.second.value << ";" << std::endl;
            }
        } else {
            // 样式或元素模板
            for (const auto& attr : attributes_) {
                oss << getIndent(indent + 1) << attr.first << ": " 
                    << attr.second << ";" << std::endl;
            }
            
            for (const auto& child : children_) {
                oss << child->toString(indent + 1) << std::endl;
            }
        }
        
        oss << getIndent(indent) << "}";
    } else {
        oss << std::endl << getIndent(indent) << "{" << std::endl;
        
        if (template_type_ == TemplateType::VAR) {
            for (const auto& var : variables_) {
                oss << getIndent(indent + 1) << var.second.name << ": "
                    << var.second.value << ";" << std::endl;
            }
        } else {
            for (const auto& attr : attributes_) {
                oss << getIndent(indent + 1) << attr.first << ": " 
                    << attr.second << ";" << std::endl;
            }
            
            for (const auto& child : children_) {
                oss << child->toString(indent + 1) << std::endl;
            }
        }
        
        oss << getIndent(indent) << "}";
    }

    return oss.str();
}

std::string TemplateNode::validate() const {
    std::string template_error = validateTemplate();
    if (!template_error.empty()) {
        return template_error;
    }

    return BaseNode::validate();
}

std::string TemplateNode::getTemplateTypeString() const {
    switch (template_type_) {
        case TemplateType::STYLE: return "Style";
        case TemplateType::ELEMENT: return "Element";
        case TemplateType::VAR: return "Var";
        default: return "Unknown";
    }
}

std::string TemplateNode::resolveVariableReferences(const std::string& content,
                                                   const std::unordered_map<std::string, std::string>& parameters) const {
    std::string result = content;
    
    // 首先使用传入的参数替换
    for (const auto& param : parameters) {
        std::string var_ref = template_name_ + "(" + param.first + ")";
        size_t pos = 0;
        while ((pos = result.find(var_ref, pos)) != std::string::npos) {
            result.replace(pos, var_ref.length(), param.second);
            pos += param.second.length();
        }
    }
    
    // 然后使用模板定义的变量替换
    for (const auto& var : variables_) {
        std::string var_ref = template_name_ + "(" + var.first + ")";
        size_t pos = 0;
        while ((pos = result.find(var_ref, pos)) != std::string::npos) {
            result.replace(pos, var_ref.length(), var.second.value);
            pos += var.second.value.length();
        }
    }
    
    return result;
}

bool TemplateNode::isValidVariableName(const std::string& name) const {
    if (name.empty()) {
        return false;
    }
    
    // 变量名应该以字母或下划线开始，后续可以包含字母、数字、下划线
    if (!std::isalpha(name[0]) && name[0] != '_') {
        return false;
    }
    
    for (size_t i = 1; i < name.length(); ++i) {
        if (!std::isalnum(name[i]) && name[i] != '_') {
            return false;
        }
    }
    
    return true;
}

} // namespace CHTL