#include "TemplateNode.h"
#include "../CHTLLexer/GlobalMap.h"
#include <algorithm>
#include <sstream>
#include <iostream>

namespace CHTL {

// TemplateNode基类实现
void TemplateNode::setTemplateProperty(const String& key, const String& value) {
    templateProperties[key] = value;
}

String TemplateNode::getTemplateProperty(const String& key, const String& defaultValue) const {
    auto it = templateProperties.find(key);
    return (it != templateProperties.end()) ? it->second : defaultValue;
}

bool TemplateNode::hasTemplateProperty(const String& key) const {
    return templateProperties.find(key) != templateProperties.end();
}

void TemplateNode::removeTemplateProperty(const String& key) {
    templateProperties.erase(key);
}

void TemplateNode::addInheritance(const String& parentTemplate) {
    if (std::find(inheritanceChain.begin(), inheritanceChain.end(), parentTemplate) == inheritanceChain.end()) {
        inheritanceChain.push_back(parentTemplate);
    }
}

void TemplateNode::removeInheritance(const String& parentTemplate) {
    auto it = std::find(inheritanceChain.begin(), inheritanceChain.end(), parentTemplate);
    if (it != inheritanceChain.end()) {
        inheritanceChain.erase(it);
    }
}

bool TemplateNode::inheritsFrom(const String& parentTemplate) const {
    return std::find(inheritanceChain.begin(), inheritanceChain.end(), parentTemplate) != inheritanceChain.end();
}

String TemplateNode::toString() const {
    std::ostringstream oss;
    oss << "[Template] @" << templateType << " " << templateName;
    if (!inheritanceChain.empty()) {
        oss << " inherits from: ";
        for (size_t i = 0; i < inheritanceChain.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << inheritanceChain[i];
        }
    }
    return oss.str();
}

std::unique_ptr<BaseNode> TemplateNode::clone() const {
    // 子类需要重写此方法
    return nullptr;
}

bool TemplateNode::validate() const {
    if (templateName.empty()) {
        return false;
    }
    if (templateType != "Style" && templateType != "Element" && templateType != "Var") {
        return false;
    }
    return validateTemplateContent();
}

// StyleTemplateNode实现
void StyleTemplateNode::setStyleProperty(const String& property, const String& value) {
    styleProperties[property] = value;
}

String StyleTemplateNode::getStyleProperty(const String& property, const String& defaultValue) const {
    auto it = styleProperties.find(property);
    return (it != styleProperties.end()) ? it->second : defaultValue;
}

bool StyleTemplateNode::hasStyleProperty(const String& property) const {
    return styleProperties.find(property) != styleProperties.end();
}

void StyleTemplateNode::removeStyleProperty(const String& property) {
    styleProperties.erase(property);
}

StringVector StyleTemplateNode::getStylePropertyNames() const {
    StringVector names;
    for (const auto& pair : styleProperties) {
        names.push_back(pair.first);
    }
    return names;
}

void StyleTemplateNode::addPseudoClass(const String& pseudoClass) {
    if (std::find(pseudoClasses.begin(), pseudoClasses.end(), pseudoClass) == pseudoClasses.end()) {
        pseudoClasses.push_back(pseudoClass);
    }
}

void StyleTemplateNode::addPseudoElement(const String& pseudoElement) {
    if (std::find(pseudoElements.begin(), pseudoElements.end(), pseudoElement) == pseudoElements.end()) {
        pseudoElements.push_back(pseudoElement);
    }
}

bool StyleTemplateNode::hasPseudoClass(const String& pseudoClass) const {
    return std::find(pseudoClasses.begin(), pseudoClasses.end(), pseudoClass) != pseudoClasses.end();
}

bool StyleTemplateNode::hasPseudoElement(const String& pseudoElement) const {
    return std::find(pseudoElements.begin(), pseudoElements.end(), pseudoElement) != pseudoElements.end();
}

String StyleTemplateNode::generateCSS(const String& selector) const {
    std::ostringstream css;
    
    String targetSelector = selector.empty() ? "." + templateName : selector;
    
    // 生成基础样式
    if (!styleProperties.empty()) {
        css << targetSelector << " {\n";
        for (const auto& prop : styleProperties) {
            css << "  " << prop.first << ": " << prop.second << ";\n";
        }
        css << "}\n";
    }
    
    // 生成伪类样式
    for (const auto& pseudoClass : pseudoClasses) {
        css << targetSelector << ":" << pseudoClass << " {\n";
        // 伪类样式需要从子节点中获取
        css << "}\n";
    }
    
    // 生成伪元素样式
    for (const auto& pseudoElement : pseudoElements) {
        css << targetSelector << "::" << pseudoElement << " {\n";
        // 伪元素样式需要从子节点中获取
        css << "}\n";
    }
    
    return css.str();
}

String StyleTemplateNode::generateInlineStyle() const {
    std::ostringstream style;
    bool first = true;
    
    for (const auto& prop : styleProperties) {
        if (!first) style << " ";
        style << prop.first << ": " << prop.second << ";";
        first = false;
    }
    
    return style.str();
}

String StyleTemplateNode::toString() const {
    std::ostringstream oss;
    oss << TemplateNode::toString();
    oss << " (" << styleProperties.size() << " properties";
    if (!pseudoClasses.empty() || !pseudoElements.empty()) {
        oss << ", " << pseudoClasses.size() << " pseudo-classes, " 
            << pseudoElements.size() << " pseudo-elements";
    }
    oss << ")";
    return oss.str();
}

std::unique_ptr<BaseNode> StyleTemplateNode::clone() const {
    auto cloned = std::make_unique<StyleTemplateNode>(templateName, position);
    cloned->templateProperties = templateProperties;
    cloned->inheritanceChain = inheritanceChain;
    cloned->isExported = isExported;
    cloned->styleProperties = styleProperties;
    cloned->pseudoClasses = pseudoClasses;
    cloned->pseudoElements = pseudoElements;
    
    // 克隆子节点
    for (const auto& child : children) {
        cloned->addChild(child->clone());
    }
    
    return std::unique_ptr<BaseNode>(cloned.release());
}

bool StyleTemplateNode::validateTemplateContent() const {
    // 验证CSS属性是否有效
    for (const auto& prop : styleProperties) {
        if (!CSSPropertyMap::isCSSProperty(prop.first)) {
            std::cerr << "Warning: Unknown CSS property: " << prop.first << std::endl;
        }
    }
    return true;
}

// ElementTemplateNode实现
void ElementTemplateNode::addElement(const String& elementType) {
    elementSequence.push_back(elementType);
}

void ElementTemplateNode::insertElement(size_t index, const String& elementType) {
    if (index <= elementSequence.size()) {
        elementSequence.insert(elementSequence.begin() + index, elementType);
    }
}

void ElementTemplateNode::removeElement(size_t index) {
    if (index < elementSequence.size()) {
        elementSequence.erase(elementSequence.begin() + index);
    }
}

void ElementTemplateNode::removeElement(const String& elementType) {
    auto it = std::find(elementSequence.begin(), elementSequence.end(), elementType);
    if (it != elementSequence.end()) {
        elementSequence.erase(it);
    }
}

String ElementTemplateNode::getElement(size_t index) const {
    return (index < elementSequence.size()) ? elementSequence[index] : "";
}

void ElementTemplateNode::setElementProperty(const String& element, const String& property, const String& value) {
    String key = element + "." + property;
    elementProperties[key] = value;
}

String ElementTemplateNode::getElementProperty(const String& element, const String& property, const String& defaultValue) const {
    String key = element + "." + property;
    auto it = elementProperties.find(key);
    return (it != elementProperties.end()) ? it->second : defaultValue;
}

bool ElementTemplateNode::hasElementProperty(const String& element, const String& property) const {
    String key = element + "." + property;
    return elementProperties.find(key) != elementProperties.end();
}

String ElementTemplateNode::generateHTML() const {
    std::ostringstream html;
    
    for (const auto& element : elementSequence) {
        html << "<" << element;
        
        // 添加元素属性
        for (const auto& prop : elementProperties) {
            if (prop.first.substr(0, element.length() + 1) == element + ".") {
                String attrName = prop.first.substr(element.length() + 1);
                html << " " << attrName << "=\"" << prop.second << "\"";
            }
        }
        
        html << ">";
        
        if (!HTMLElementMap::isSingleTag(element)) {
            html << "</" << element << ">";
        }
        html << "\n";
    }
    
    return html.str();
}

StringVector ElementTemplateNode::generateElementList() const {
    return elementSequence;
}

String ElementTemplateNode::toString() const {
    std::ostringstream oss;
    oss << TemplateNode::toString();
    oss << " (" << elementSequence.size() << " elements)";
    return oss.str();
}

std::unique_ptr<BaseNode> ElementTemplateNode::clone() const {
    auto cloned = std::make_unique<ElementTemplateNode>(templateName, position);
    cloned->templateProperties = templateProperties;
    cloned->inheritanceChain = inheritanceChain;
    cloned->isExported = isExported;
    cloned->elementSequence = elementSequence;
    cloned->elementProperties = elementProperties;
    
    // 克隆子节点
    for (const auto& child : children) {
        cloned->addChild(child->clone());
    }
    
    return std::unique_ptr<BaseNode>(cloned.release());
}

bool ElementTemplateNode::validateTemplateContent() const {
    // 验证元素是否有效
    for (const auto& element : elementSequence) {
        if (!HTMLElementMap::isHTMLElement(element)) {
            std::cerr << "Warning: Unknown HTML element: " << element << std::endl;
        }
    }
    return true;
}

// VarTemplateNode实现
void VarTemplateNode::setVariable(const String& name, const String& value, const String& type) {
    variables[name] = value;
    variableTypes[name] = type;
}

String VarTemplateNode::getVariable(const String& name, const String& defaultValue) const {
    auto it = variables.find(name);
    return (it != variables.end()) ? it->second : defaultValue;
}

bool VarTemplateNode::hasVariable(const String& name) const {
    return variables.find(name) != variables.end();
}

void VarTemplateNode::removeVariable(const String& name) {
    variables.erase(name);
    variableTypes.erase(name);
    variableComments.erase(name);
}

StringVector VarTemplateNode::getVariableNames() const {
    StringVector names;
    for (const auto& pair : variables) {
        names.push_back(pair.first);
    }
    return names;
}

void VarTemplateNode::setVariableType(const String& name, const String& type) {
    variableTypes[name] = type;
}

String VarTemplateNode::getVariableType(const String& name) const {
    auto it = variableTypes.find(name);
    return (it != variableTypes.end()) ? it->second : "string";
}

void VarTemplateNode::setVariableComment(const String& name, const String& comment) {
    variableComments[name] = comment;
}

String VarTemplateNode::getVariableComment(const String& name) const {
    auto it = variableComments.find(name);
    return (it != variableComments.end()) ? it->second : "";
}

String VarTemplateNode::resolveVariable(const String& name, const StringUnorderedMap& context) const {
    // 首先检查上下文
    auto contextIt = context.find(name);
    if (contextIt != context.end()) {
        return contextIt->second;
    }
    
    // 然后检查模板变量
    return getVariable(name);
}

StringUnorderedMap VarTemplateNode::resolveAllVariables(const StringUnorderedMap& context) const {
    StringUnorderedMap resolved = variables;  // 复制基础变量
    
    // 用上下文覆盖
    for (const auto& pair : context) {
        resolved[pair.first] = pair.second;
    }
    
    return resolved;
}

String VarTemplateNode::generateVariableDeclarations() const {
    std::ostringstream declarations;
    
    for (const auto& var : variables) {
        declarations << "var " << var.first << " = ";
        
        String type = getVariableType(var.first);
        if (type == "string") {
            declarations << "\"" << var.second << "\"";
        } else if (type == "number") {
            declarations << var.second;
        } else if (type == "boolean") {
            declarations << var.second;
        } else {
            declarations << "\"" << var.second << "\"";  // 默认为字符串
        }
        
        String comment = getVariableComment(var.first);
        if (!comment.empty()) {
            declarations << "; // " << comment;
        }
        
        declarations << "\n";
    }
    
    return declarations.str();
}

String VarTemplateNode::generateJSONExport() const {
    std::ostringstream json;
    json << "{\n";
    
    bool first = true;
    for (const auto& var : variables) {
        if (!first) json << ",\n";
        json << "  \"" << var.first << "\": \"" << var.second << "\"";
        first = false;
    }
    
    json << "\n}";
    return json.str();
}

String VarTemplateNode::toString() const {
    std::ostringstream oss;
    oss << TemplateNode::toString();
    oss << " (" << variables.size() << " variables)";
    return oss.str();
}

std::unique_ptr<BaseNode> VarTemplateNode::clone() const {
    auto cloned = std::make_unique<VarTemplateNode>(templateName, position);
    cloned->templateProperties = templateProperties;
    cloned->inheritanceChain = inheritanceChain;
    cloned->isExported = isExported;
    cloned->variables = variables;
    cloned->variableTypes = variableTypes;
    cloned->variableComments = variableComments;
    
    // 克隆子节点
    for (const auto& child : children) {
        cloned->addChild(child->clone());
    }
    
    return std::unique_ptr<BaseNode>(cloned.release());
}

bool VarTemplateNode::validateTemplateContent() const {
    // 验证变量类型是否有效
    StringVector validTypes = {"string", "number", "boolean", "color", "size", "url"};
    
    for (const auto& varType : variableTypes) {
        if (std::find(validTypes.begin(), validTypes.end(), varType.second) == validTypes.end()) {
            std::cerr << "Warning: Unknown variable type: " << varType.second << std::endl;
        }
    }
    
    return true;
}

// 模板工厂类
class TemplateNodeFactory {
public:
    static std::unique_ptr<StyleTemplateNode> createStyleTemplate(const String& name) {
        return std::make_unique<StyleTemplateNode>(name);
    }
    
    static std::unique_ptr<ElementTemplateNode> createElementTemplate(const String& name) {
        return std::make_unique<ElementTemplateNode>(name);
    }
    
    static std::unique_ptr<VarTemplateNode> createVarTemplate(const String& name) {
        return std::make_unique<VarTemplateNode>(name);
    }
};

} // namespace CHTL