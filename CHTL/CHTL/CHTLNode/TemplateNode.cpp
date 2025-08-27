#include "TemplateNode.hpp"
#include "../CHTLState/CHTLState.hpp"
#include <sstream>
#include <regex>
#include <algorithm>

namespace CHTL {

TemplateNode::TemplateNode(TemplateType type, const std::string& name)
    : BaseNode(NodeType::TEMPLATE, name), templateType(type), templateName(name),
      isAbstract(false) {
}

TemplateType TemplateNode::getTemplateType() const {
    return templateType;
}

void TemplateNode::setTemplateType(TemplateType type) {
    templateType = type;
}

const std::string& TemplateNode::getTemplateName() const {
    return templateName;
}

void TemplateNode::setTemplateName(const std::string& name) {
    templateName = name;
    setName(name);
}

const std::string& TemplateNode::getNamespacePath() const {
    return namespacePath;
}

void TemplateNode::setNamespacePath(const std::string& path) {
    namespacePath = path;
}

void TemplateNode::addInheritance(const std::string& parentTemplate) {
    auto it = std::find(inheritanceChain.begin(), inheritanceChain.end(), parentTemplate);
    if (it == inheritanceChain.end()) {
        inheritanceChain.push_back(parentTemplate);
    }
}

void TemplateNode::removeInheritance(const std::string& parentTemplate) {
    auto it = std::find(inheritanceChain.begin(), inheritanceChain.end(), parentTemplate);
    if (it != inheritanceChain.end()) {
        inheritanceChain.erase(it);
    }
}

const std::vector<std::string>& TemplateNode::getInheritanceChain() const {
    return inheritanceChain;
}

bool TemplateNode::hasInheritance() const {
    return !inheritanceChain.empty();
}

bool TemplateNode::inheritsFrom(const std::string& templateName) const {
    return std::find(inheritanceChain.begin(), inheritanceChain.end(), templateName) 
           != inheritanceChain.end();
}

void TemplateNode::setVariable(const std::string& name, const std::string& value) {
    variables[name] = value;
}

std::string TemplateNode::getVariable(const std::string& name) const {
    auto it = variables.find(name);
    return (it != variables.end()) ? it->second : "";
}

bool TemplateNode::hasVariable(const std::string& name) const {
    return variables.find(name) != variables.end();
}

void TemplateNode::removeVariable(const std::string& name) {
    variables.erase(name);
}

const std::unordered_map<std::string, std::string>& TemplateNode::getVariables() const {
    return variables;
}

bool TemplateNode::getIsAbstract() const {
    return isAbstract;
}

void TemplateNode::setIsAbstract(bool abstract) {
    isAbstract = abstract;
}

void TemplateNode::addRequiredParam(const std::string& param) {
    requiredParams.insert(param);
}

void TemplateNode::removeRequiredParam(const std::string& param) {
    requiredParams.erase(param);
}

bool TemplateNode::isParamRequired(const std::string& param) const {
    return requiredParams.find(param) != requiredParams.end();
}

const std::unordered_set<std::string>& TemplateNode::getRequiredParams() const {
    return requiredParams;
}

std::shared_ptr<BaseNode> TemplateNode::instantiate(const std::unordered_map<std::string, std::string>& params) const {
    if (!canInstantiate(params)) {
        return nullptr;
    }
    
    switch (templateType) {
        case TemplateType::STYLE:
            return TemplateInstantiator::instantiateStyleTemplate(*this, params);
        case TemplateType::ELEMENT:
            return TemplateInstantiator::instantiateElementTemplate(*this, params);
        case TemplateType::VAR:
            return TemplateInstantiator::instantiateVarTemplate(*this, params);
        default:
            return nullptr;
    }
}

bool TemplateNode::canInstantiate(const std::unordered_map<std::string, std::string>& params) const {
    if (isAbstract) {
        return false;
    }
    
    // 检查所有必需参数是否都已提供
    for (const auto& required : requiredParams) {
        if (params.find(required) == params.end()) {
            return false;
        }
    }
    
    return true;
}

std::string TemplateNode::resolveContent(const std::unordered_map<std::string, std::string>& params) const {
    std::stringstream ss;
    
    // 遍历所有子节点，生成内容
    for (const auto& child : getChildren()) {
        switch (templateType) {
            case TemplateType::STYLE:
                ss << child->toCSS();
                break;
            case TemplateType::ELEMENT:
                ss << child->toHTML();
                break;
            case TemplateType::VAR:
                // 变量模板特殊处理
                ss << child->toString();
                break;
        }
    }
    
    return processVariableSubstitution(ss.str(), params);
}

std::string TemplateNode::toString() const {
    std::stringstream ss;
    ss << "TemplateNode{type=" << getTypeString() << ", name=" << templateName;
    if (!namespacePath.empty()) {
        ss << ", namespace=" << namespacePath;
    }
    if (isAbstract) {
        ss << ", abstract=true";
    }
    if (!inheritanceChain.empty()) {
        ss << ", inherits=[";
        for (size_t i = 0; i < inheritanceChain.size(); ++i) {
            if (i > 0) ss << ", ";
            ss << inheritanceChain[i];
        }
        ss << "]";
    }
    if (!variables.empty()) {
        ss << ", vars=" << variables.size();
    }
    ss << "}";
    return ss.str();
}

std::string TemplateNode::toHTML() const {
    if (templateType == TemplateType::ELEMENT) {
        return resolveContent();
    }
    return "";
}

std::string TemplateNode::toCSS() const {
    if (templateType == TemplateType::STYLE) {
        return resolveContent();
    }
    return "";
}

std::string TemplateNode::toJavaScript() const {
    if (templateType == TemplateType::VAR) {
        // 生成JavaScript变量定义
        std::stringstream ss;
        for (const auto& var : variables) {
            ss << "const " << var.first << " = " << var.second << ";\n";
        }
        return ss.str();
    }
    return "";
}

void TemplateNode::validate(CHTLState& state) {
    BaseNode::validate(state);
    
    // 模板名称验证
    if (templateName.empty()) {
        setErrorMessage("模板名称不能为空");
        return;
    }
    
    // 继承链循环检测
    std::unordered_set<std::string> visited;
    for (const auto& parent : inheritanceChain) {
        if (visited.find(parent) != visited.end()) {
            setErrorMessage("检测到循环继承: " + parent);
            return;
        }
        visited.insert(parent);
        
        if (parent == templateName) {
            setErrorMessage("模板不能继承自己");
            return;
        }
    }
    
    // 变量组模板特殊验证
    if (templateType == TemplateType::VAR) {
        if (variables.empty() && !isAbstract) {
            setMetadata("warning", "变量组模板没有定义任何变量");
        }
    }
}

std::shared_ptr<BaseNode> TemplateNode::clone() const {
    auto cloned = std::make_shared<TemplateNode>(templateType, templateName);
    
    // 复制基类数据
    cloned->setPosition(getLine(), getColumn(), getPosition());
    cloned->setValid(getIsValid());
    cloned->setErrorMessage(getErrorMessage());
    
    // 复制TemplateNode特有数据
    cloned->namespacePath = namespacePath;
    cloned->inheritanceChain = inheritanceChain;
    cloned->variables = variables;
    cloned->isAbstract = isAbstract;
    cloned->requiredParams = requiredParams;
    
    // 克隆子节点
    for (const auto& child : getChildren()) {
        cloned->addChild(child->clone());
    }
    
    return cloned;
}

void TemplateNode::mergeWith(const TemplateNode& other) {
    if (templateType != other.templateType) {
        return; // 不同类型的模板不能合并
    }
    
    // 合并变量
    for (const auto& var : other.variables) {
        variables[var.first] = var.second;
    }
    
    // 合并继承链
    for (const auto& parent : other.inheritanceChain) {
        addInheritance(parent);
    }
    
    // 合并必需参数
    for (const auto& param : other.requiredParams) {
        requiredParams.insert(param);
    }
    
    // 合并子节点
    for (const auto& child : other.getChildren()) {
        addChild(child->clone());
    }
}

bool TemplateNode::isCompatibleWith(const TemplateNode& other) const {
    return templateType == other.templateType;
}

std::string TemplateNode::getTypeString() const {
    switch (templateType) {
        case TemplateType::STYLE: return "Style";
        case TemplateType::ELEMENT: return "Element";
        case TemplateType::VAR: return "Var";
        default: return "Unknown";
    }
}

std::string TemplateNode::processVariableSubstitution(const std::string& content, 
                                                     const std::unordered_map<std::string, std::string>& params) const {
    return TemplateInstantiator::substituteVariables(content, params);
}

// TemplateInstantiator 实现
std::shared_ptr<BaseNode> TemplateInstantiator::instantiateStyleTemplate(
    const TemplateNode& templateNode,
    const std::unordered_map<std::string, std::string>& params) {
    
    // 创建样式节点
    auto styleNode = std::make_shared<BaseNode>(NodeType::STYLE, templateNode.getTemplateName());
    
    // 克隆并替换所有子节点
    for (const auto& child : templateNode.getChildren()) {
        auto clonedChild = cloneAndSubstitute(child, params);
        styleNode->addChild(clonedChild);
    }
    
    return styleNode;
}

std::shared_ptr<BaseNode> TemplateInstantiator::instantiateElementTemplate(
    const TemplateNode& templateNode,
    const std::unordered_map<std::string, std::string>& params) {
    
    // 创建元素容器节点
    auto containerNode = std::make_shared<BaseNode>(NodeType::ELEMENT, templateNode.getTemplateName());
    
    // 克隆并替换所有子节点
    for (const auto& child : templateNode.getChildren()) {
        auto clonedChild = cloneAndSubstitute(child, params);
        containerNode->addChild(clonedChild);
    }
    
    return containerNode;
}

std::shared_ptr<BaseNode> TemplateInstantiator::instantiateVarTemplate(
    const TemplateNode& templateNode,
    const std::unordered_map<std::string, std::string>& params) {
    
    // 创建变量节点
    auto varNode = std::make_shared<BaseNode>(NodeType::VALUE, templateNode.getTemplateName());
    
    // 合并模板变量和参数
    auto mergedParams = params;
    for (const auto& var : templateNode.getVariables()) {
        if (mergedParams.find(var.first) == mergedParams.end()) {
            mergedParams[var.first] = var.second;
        }
    }
    
    // 设置变量属性
    for (const auto& var : mergedParams) {
        varNode->setAttribute(var.first, var.second);
    }
    
    return varNode;
}

std::shared_ptr<BaseNode> TemplateInstantiator::cloneAndSubstitute(
    const std::shared_ptr<BaseNode>& node,
    const std::unordered_map<std::string, std::string>& params) {
    
    auto cloned = node->clone();
    
    // 替换属性中的变量
    for (const auto& attr : cloned->getAttributes()) {
        std::string substitutedValue = substituteVariables(attr.second.value, params);
        cloned->setAttribute(attr.first, substitutedValue, attr.second.isLiteral);
    }
    
    // 递归处理子节点
    const auto& children = cloned->getChildren();
    for (size_t i = 0; i < children.size(); ++i) {
        auto substitutedChild = cloneAndSubstitute(children[i], params);
        // 替换子节点
        cloned->removeChild(children[i]);
        cloned->insertChild(i, substitutedChild);
    }
    
    return cloned;
}

std::string TemplateInstantiator::substituteVariables(
    const std::string& content,
    const std::unordered_map<std::string, std::string>& params) {
    
    std::string result = content;
    
    // 使用正则表达式查找变量引用 ${variableName} 或 ThemeColor(variableName)
    std::regex varPattern(R"(\$\{(\w+)\}|(\w+)\((\w+)\))");
    std::smatch match;
    
    while (std::regex_search(result, match, varPattern)) {
        std::string varName;
        std::string replacement;
        
        if (match[1].matched) {
            // ${variableName} 格式
            varName = match[1].str();
        } else if (match[2].matched && match[3].matched) {
            // ThemeColor(variableName) 格式
            varName = match[3].str();
        }
        
        auto it = params.find(varName);
        if (it != params.end()) {
            replacement = it->second;
        } else {
            replacement = match[0].str(); // 保持原样
        }
        
        result.replace(match[0].first, match[0].second, replacement);
    }
    
    return result;
}

}