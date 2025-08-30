#include "CustomNode.h"
#include "../CHTLLexer/GlobalMap.h"
#include <algorithm>
#include <sstream>
#include <iostream>
#include <set>

namespace CHTL {

// CustomNode基类实现
void CustomNode::setCustomProperty(const String& key, const String& value) {
    customProperties[key] = value;
}

String CustomNode::getCustomProperty(const String& key, const String& defaultValue) const {
    auto it = customProperties.find(key);
    return (it != customProperties.end()) ? it->second : defaultValue;
}

bool CustomNode::hasCustomProperty(const String& key) const {
    return customProperties.find(key) != customProperties.end();
}

void CustomNode::removeCustomProperty(const String& key) {
    customProperties.erase(key);
}

void CustomNode::addSpecialization(const String& baseCustom) {
    if (std::find(specializationChain.begin(), specializationChain.end(), baseCustom) == specializationChain.end()) {
        specializationChain.push_back(baseCustom);
    }
}

void CustomNode::removeSpecialization(const String& baseCustom) {
    auto it = std::find(specializationChain.begin(), specializationChain.end(), baseCustom);
    if (it != specializationChain.end()) {
        specializationChain.erase(it);
    }
}

bool CustomNode::specializesFrom(const String& baseCustom) const {
    return std::find(specializationChain.begin(), specializationChain.end(), baseCustom) != specializationChain.end();
}

void CustomNode::markForDeletion(const String& target) {
    if (std::find(deletionTargets.begin(), deletionTargets.end(), target) == deletionTargets.end()) {
        deletionTargets.push_back(target);
    }
}

void CustomNode::unmarkForDeletion(const String& target) {
    auto it = std::find(deletionTargets.begin(), deletionTargets.end(), target);
    if (it != deletionTargets.end()) {
        deletionTargets.erase(it);
    }
}

bool CustomNode::isMarkedForDeletion(const String& target) const {
    return std::find(deletionTargets.begin(), deletionTargets.end(), target) != deletionTargets.end();
}

void CustomNode::clearDeletions() {
    deletionTargets.clear();
}

void CustomNode::addInsertion(const InsertionPoint& point) {
    insertionPoints.push_back(point);
}

void CustomNode::removeInsertion(size_t index) {
    if (index < insertionPoints.size()) {
        insertionPoints.erase(insertionPoints.begin() + index);
    }
}

void CustomNode::clearInsertions() {
    insertionPoints.clear();
}

String CustomNode::toString() const {
    std::ostringstream oss;
    oss << "[Custom] @" << customType << " " << customName;
    if (!specializationChain.empty()) {
        oss << " specializes from: ";
        for (size_t i = 0; i < specializationChain.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << specializationChain[i];
        }
    }
    return oss.str();
}

std::unique_ptr<BaseNode> CustomNode::clone() const {
    // 子类需要重写此方法
    return nullptr;
}

bool CustomNode::validate() const {
    if (customName.empty()) {
        return false;
    }
    if (customType != "Style" && customType != "Element" && customType != "Var") {
        return false;
    }
    return validateCustomContent();
}

// CustomStyleNode实现
void CustomStyleNode::setBaseProperty(const String& property, const String& value) {
    baseProperties[property] = value;
}

String CustomStyleNode::getBaseProperty(const String& property, const String& defaultValue) const {
    auto it = baseProperties.find(property);
    return (it != baseProperties.end()) ? it->second : defaultValue;
}

bool CustomStyleNode::hasBaseProperty(const String& property) const {
    return baseProperties.find(property) != baseProperties.end();
}

void CustomStyleNode::removeBaseProperty(const String& property) {
    baseProperties.erase(property);
}

void CustomStyleNode::addOptionalProperty(const String& property) {
    optionalProperties[property] = "";  // 空值表示可选
}

void CustomStyleNode::removeOptionalProperty(const String& property) {
    optionalProperties.erase(property);
}

bool CustomStyleNode::isOptionalProperty(const String& property) const {
    return optionalProperties.find(property) != optionalProperties.end();
}

void CustomStyleNode::addRequiredProperty(const String& property) {
    if (std::find(requiredProperties.begin(), requiredProperties.end(), property) == requiredProperties.end()) {
        requiredProperties.push_back(property);
    }
}

void CustomStyleNode::removeRequiredProperty(const String& property) {
    auto it = std::find(requiredProperties.begin(), requiredProperties.end(), property);
    if (it != requiredProperties.end()) {
        requiredProperties.erase(it);
    }
}

bool CustomStyleNode::isRequiredProperty(const String& property) const {
    return std::find(requiredProperties.begin(), requiredProperties.end(), property) != requiredProperties.end();
}

void CustomStyleNode::setDefaultValue(const String& property, const String& value) {
    defaultValues[property] = value;
}

String CustomStyleNode::getDefaultValue(const String& property) const {
    auto it = defaultValues.find(property);
    return (it != defaultValues.end()) ? it->second : "";
}

bool CustomStyleNode::hasDefaultValue(const String& property) const {
    return defaultValues.find(property) != defaultValues.end();
}

void CustomStyleNode::deleteProperty(const String& property) {
    if (std::find(deletedProperties.begin(), deletedProperties.end(), property) == deletedProperties.end()) {
        deletedProperties.push_back(property);
    }
}

void CustomStyleNode::deleteInheritance(const String& inheritance) {
    if (std::find(deletedInheritances.begin(), deletedInheritances.end(), inheritance) == deletedInheritances.end()) {
        deletedInheritances.push_back(inheritance);
    }
}

bool CustomStyleNode::isPropertyDeleted(const String& property) const {
    return std::find(deletedProperties.begin(), deletedProperties.end(), property) != deletedProperties.end();
}

bool CustomStyleNode::isInheritanceDeleted(const String& inheritance) const {
    return std::find(deletedInheritances.begin(), deletedInheritances.end(), inheritance) != deletedInheritances.end();
}

void CustomStyleNode::specializeProperty(const String& property, const String& value) {
    // 特例化属性将替换基础属性值
    baseProperties[property] = value;
}

StringUnorderedMap CustomStyleNode::getSpecializedProperties(const StringUnorderedMap& parameters) const {
    StringUnorderedMap specialized = baseProperties;
    
    // 应用参数替换
    for (auto& prop : specialized) {
        prop.second = CustomInstantiator::substituteParameters(prop.second, parameters);
    }
    
    // 添加默认值（如果未提供）
    for (const auto& defaultVal : defaultValues) {
        if (specialized.find(defaultVal.first) == specialized.end()) {
            specialized[defaultVal.first] = CustomInstantiator::substituteParameters(defaultVal.second, parameters);
        }
    }
    
    // 移除删除的属性
    for (const auto& deletedProp : deletedProperties) {
        specialized.erase(deletedProp);
    }
    
    return specialized;
}

String CustomStyleNode::generateCSS(const String& selector, const StringUnorderedMap& parameters) const {
    std::ostringstream css;
    auto props = getSpecializedProperties(parameters);
    
    if (!props.empty()) {
        css << selector << " {\n";
        for (const auto& prop : props) {
            css << "  " << prop.first << ": " << prop.second << ";\n";
        }
        css << "}\n";
    }
    
    return css.str();
}

String CustomStyleNode::generateInlineStyle(const StringUnorderedMap& parameters) const {
    std::ostringstream style;
    auto props = getSpecializedProperties(parameters);
    bool first = true;
    
    for (const auto& prop : props) {
        if (!first) style << " ";
        style << prop.first << ": " << prop.second << ";";
        first = false;
    }
    
    return style.str();
}

bool CustomStyleNode::validateParameters(const StringUnorderedMap& parameters) const {
    return getMissingRequiredProperties(parameters).empty();
}

StringVector CustomStyleNode::getMissingRequiredProperties(const StringUnorderedMap& parameters) const {
    StringVector missing;
    
    for (const auto& required : requiredProperties) {
        if (parameters.find(required) == parameters.end() && !hasDefaultValue(required)) {
            missing.push_back(required);
        }
    }
    
    return missing;
}

String CustomStyleNode::toString() const {
    std::ostringstream oss;
    oss << CustomNode::toString();
    oss << " (" << baseProperties.size() << " base properties";
    if (!requiredProperties.empty()) {
        oss << ", " << requiredProperties.size() << " required";
    }
    if (!optionalProperties.empty()) {
        oss << ", " << optionalProperties.size() << " optional";
    }
    oss << ")";
    return oss.str();
}

std::unique_ptr<BaseNode> CustomStyleNode::clone() const {
    auto cloned = std::make_unique<CustomStyleNode>(customName, position);
    cloned->customProperties = customProperties;
    cloned->specializationChain = specializationChain;
    cloned->deletionTargets = deletionTargets;
    cloned->insertionPoints = insertionPoints;
    cloned->isExported = isExported;
    cloned->baseProperties = baseProperties;
    cloned->optionalProperties = optionalProperties;
    cloned->requiredProperties = requiredProperties;
    cloned->defaultValues = defaultValues;
    cloned->deletedProperties = deletedProperties;
    cloned->deletedInheritances = deletedInheritances;
    
    // 克隆子节点
    for (const auto& child : children) {
        cloned->addChild(child->clone());
    }
    
    return std::unique_ptr<BaseNode>(cloned.release());
}

bool CustomStyleNode::validateCustomContent() const {
    // 验证CSS属性是否有效
    for (const auto& prop : baseProperties) {
        if (!CSSPropertyMap::isCSSProperty(prop.first)) {
            std::cerr << "Warning: Unknown CSS property: " << prop.first << std::endl;
        }
    }
    
    // 验证必需属性不在删除列表中
    for (const auto& required : requiredProperties) {
        if (isPropertyDeleted(required)) {
            std::cerr << "Error: Required property '" << required << "' is marked for deletion" << std::endl;
            return false;
        }
    }
    
    return true;
}

void CustomStyleNode::applySpecializations() {
    // 应用特例化操作
    // 这里可以实现具体的特例化逻辑
}

// CustomElementNode实现
void CustomElementNode::addElement(const String& elementType) {
    elementSequence.push_back(elementType);
    updateElementIndices();
}

void CustomElementNode::insertElementAt(size_t index, const String& elementType) {
    if (index <= elementSequence.size()) {
        elementSequence.insert(elementSequence.begin() + index, elementType);
        updateElementIndices();
    }
}

void CustomElementNode::removeElementAt(size_t index) {
    if (index < elementSequence.size()) {
        elementSequence.erase(elementSequence.begin() + index);
        updateElementIndices();
    }
}

void CustomElementNode::removeElement(const String& elementType) {
    auto it = std::find(elementSequence.begin(), elementSequence.end(), elementType);
    if (it != elementSequence.end()) {
        elementSequence.erase(it);
        updateElementIndices();
    }
}

String CustomElementNode::getElementAt(size_t index) const {
    return (index < elementSequence.size()) ? elementSequence[index] : "";
}

void CustomElementNode::setElementIndex(const String& elementType, size_t index) {
    elementIndexMap[elementType] = std::to_string(index);
}

size_t CustomElementNode::getElementIndex(const String& elementType) const {
    auto it = elementIndexMap.find(elementType);
    return (it != elementIndexMap.end()) ? std::stoul(it->second) : SIZE_MAX;
}

bool CustomElementNode::hasElementIndex(const String& elementType) const {
    return elementIndexMap.find(elementType) != elementIndexMap.end();
}

String CustomElementNode::getElementByIndex(size_t index) const {
    return getElementAt(index);
}

void CustomElementNode::setElementProperty(const String& element, const String& property, const String& value) {
    String key = element + "." + property;
    elementProperties[key] = value;
}

void CustomElementNode::setElementPropertyByIndex(size_t index, const String& property, const String& value) {
    if (index < elementSequence.size()) {
        setElementProperty(elementSequence[index], property, value);
    }
}

String CustomElementNode::getElementProperty(const String& element, const String& property) const {
    String key = element + "." + property;
    auto it = elementProperties.find(key);
    return (it != elementProperties.end()) ? it->second : "";
}

String CustomElementNode::getElementPropertyByIndex(size_t index, const String& property) const {
    if (index < elementSequence.size()) {
        return getElementProperty(elementSequence[index], property);
    }
    return "";
}

void CustomElementNode::deleteElement(const String& element) {
    if (std::find(deletedElements.begin(), deletedElements.end(), element) == deletedElements.end()) {
        deletedElements.push_back(element);
    }
}

void CustomElementNode::deleteElementByIndex(size_t index) {
    if (index < elementSequence.size()) {
        deleteElement(elementSequence[index]);
    }
}

void CustomElementNode::deleteInheritance(const String& inheritance) {
    if (std::find(deletedInheritances.begin(), deletedInheritances.end(), inheritance) == deletedInheritances.end()) {
        deletedInheritances.push_back(inheritance);
    }
}

bool CustomElementNode::isElementDeleted(const String& element) const {
    return std::find(deletedElements.begin(), deletedElements.end(), element) != deletedElements.end();
}

bool CustomElementNode::isInheritanceDeleted(const String& inheritance) const {
    return std::find(deletedInheritances.begin(), deletedInheritances.end(), inheritance) != deletedInheritances.end();
}

void CustomElementNode::insertAfter(const String& target, const String& newElement) {
    InsertionPoint point;
    point.position = "after";
    point.target = target;
    point.content = newElement;
    addInsertion(point);
}

void CustomElementNode::insertBefore(const String& target, const String& newElement) {
    InsertionPoint point;
    point.position = "before";
    point.target = target;
    point.content = newElement;
    addInsertion(point);
}

void CustomElementNode::replaceElement(const String& target, const String& newElement) {
    InsertionPoint point;
    point.position = "replace";
    point.target = target;
    point.content = newElement;
    addInsertion(point);
}

void CustomElementNode::insertAtTop(const String& newElement) {
    InsertionPoint point;
    point.position = "at top";
    point.content = newElement;
    addInsertion(point);
}

void CustomElementNode::insertAtBottom(const String& newElement) {
    InsertionPoint point;
    point.position = "at bottom";
    point.content = newElement;
    addInsertion(point);
}

void CustomElementNode::insertAfterIndex(size_t index, const String& newElement) {
    InsertionPoint point;
    point.position = "after";
    point.index = index;
    point.content = newElement;
    addInsertion(point);
}

void CustomElementNode::insertBeforeIndex(size_t index, const String& newElement) {
    InsertionPoint point;
    point.position = "before";
    point.index = index;
    point.content = newElement;
    addInsertion(point);
}

void CustomElementNode::replaceElementAtIndex(size_t index, const String& newElement) {
    InsertionPoint point;
    point.position = "replace";
    point.index = index;
    point.content = newElement;
    addInsertion(point);
}

void CustomElementNode::addStyleToElement(const String& element, const String& styleContent) {
    setElementProperty(element, "_style", styleContent);
}

void CustomElementNode::addStyleToElementByIndex(size_t index, const String& styleContent) {
    if (index < elementSequence.size()) {
        addStyleToElement(elementSequence[index], styleContent);
    }
}

void CustomElementNode::addScriptToElement(const String& element, const String& scriptContent) {
    setElementProperty(element, "_script", scriptContent);
}

String CustomElementNode::generateHTML(const StringUnorderedMap& parameters) const {
    std::ostringstream html;
    StringVector sequence = elementSequence;
    
    // 应用插入操作
    // 这里简化处理，实际需要更复杂的逻辑
    for (const auto& element : sequence) {
        if (!isElementDeleted(element)) {
            html << "<" << element;
            
            // 添加元素属性
            for (const auto& prop : elementProperties) {
                if (prop.first.substr(0, element.length() + 1) == element + ".") {
                    String attrName = prop.first.substr(element.length() + 1);
                    if (attrName != "_style" && attrName != "_script") {
                        String attrValue = CustomInstantiator::substituteParameters(prop.second, parameters);
                        html << " " << attrName << "=\"" << attrValue << "\"";
                    }
                }
            }
            
            html << ">";
            
            // 添加样式或脚本内容
            String styleContent = getElementProperty(element, "_style");
            String scriptContent = getElementProperty(element, "_script");
            
            if (!styleContent.empty()) {
                html << "\n  <style>" << CustomInstantiator::substituteParameters(styleContent, parameters) << "</style>";
            }
            
            if (!scriptContent.empty()) {
                html << "\n  <script>" << CustomInstantiator::substituteParameters(scriptContent, parameters) << "</script>";
            }
            
            if (!HTMLElementMap::isSingleTag(element)) {
                html << "</" << element << ">";
            }
            html << "\n";
        }
    }
    
    return html.str();
}

StringVector CustomElementNode::generateElementList() const {
    StringVector result;
    for (const auto& element : elementSequence) {
        if (!isElementDeleted(element)) {
            result.push_back(element);
        }
    }
    return result;
}

String CustomElementNode::toString() const {
    std::ostringstream oss;
    oss << CustomNode::toString();
    oss << " (" << elementSequence.size() << " elements";
    if (!deletedElements.empty()) {
        oss << ", " << deletedElements.size() << " deleted";
    }
    oss << ")";
    return oss.str();
}

std::unique_ptr<BaseNode> CustomElementNode::clone() const {
    auto cloned = std::make_unique<CustomElementNode>(customName, position);
    cloned->customProperties = customProperties;
    cloned->specializationChain = specializationChain;
    cloned->deletionTargets = deletionTargets;
    cloned->insertionPoints = insertionPoints;
    cloned->isExported = isExported;
    cloned->elementSequence = elementSequence;
    cloned->elementIndexMap = elementIndexMap;
    cloned->elementProperties = elementProperties;
    cloned->deletedElements = deletedElements;
    cloned->deletedInheritances = deletedInheritances;
    
    // 克隆子节点
    for (const auto& child : children) {
        cloned->addChild(child->clone());
    }
    
    return std::unique_ptr<BaseNode>(cloned.release());
}

bool CustomElementNode::validateCustomContent() const {
    // 验证元素是否有效
    for (const auto& element : elementSequence) {
        if (!HTMLElementMap::isHTMLElement(element)) {
            std::cerr << "Warning: Unknown HTML element: " << element << std::endl;
        }
    }
    return true;
}

void CustomElementNode::applySpecializations() {
    // 应用特例化操作
}

void CustomElementNode::updateElementIndices() {
    elementIndexMap.clear();
    for (size_t i = 0; i < elementSequence.size(); ++i) {
        elementIndexMap[elementSequence[i]] = std::to_string(i);
    }
}

String CustomElementNode::processInsertions(const String& html) const {
    // 简化的插入处理
    return html;
}

// CustomVarNode实现
void CustomVarNode::setVariable(const String& name, const String& value, const String& type) {
    baseVariables[name] = value;
    variableTypes[name] = type;
}

String CustomVarNode::getVariable(const String& name, const String& defaultValue) const {
    auto it = baseVariables.find(name);
    return (it != baseVariables.end()) ? it->second : defaultValue;
}

bool CustomVarNode::hasVariable(const String& name) const {
    return baseVariables.find(name) != baseVariables.end();
}

void CustomVarNode::removeVariable(const String& name) {
    baseVariables.erase(name);
    variableTypes.erase(name);
    variableConstraints.erase(name);
}

StringVector CustomVarNode::getVariableNames() const {
    StringVector names;
    for (const auto& pair : baseVariables) {
        names.push_back(pair.first);
    }
    return names;
}

void CustomVarNode::setVariableType(const String& name, const String& type) {
    variableTypes[name] = type;
}

String CustomVarNode::getVariableType(const String& name) const {
    auto it = variableTypes.find(name);
    return (it != variableTypes.end()) ? it->second : "string";
}

void CustomVarNode::setVariableConstraint(const String& name, const String& constraint) {
    variableConstraints[name] = constraint;
}

String CustomVarNode::getVariableConstraint(const String& name) const {
    auto it = variableConstraints.find(name);
    return (it != variableConstraints.end()) ? it->second : "";
}

bool CustomVarNode::validateVariableValue(const String& name, const String& value) const {
    // 简化的验证逻辑
    String constraint = getVariableConstraint(name);
    return constraint.empty() || value.find(constraint) != String::npos;
}

void CustomVarNode::deleteVariable(const String& name) {
    if (std::find(deletedVariables.begin(), deletedVariables.end(), name) == deletedVariables.end()) {
        deletedVariables.push_back(name);
    }
}

bool CustomVarNode::isVariableDeleted(const String& name) const {
    return std::find(deletedVariables.begin(), deletedVariables.end(), name) != deletedVariables.end();
}

void CustomVarNode::specializeVariable(const String& name, const String& value) {
    baseVariables[name] = value;
}

StringUnorderedMap CustomVarNode::getSpecializedVariables(const StringUnorderedMap& parameters) const {
    StringUnorderedMap specialized = baseVariables;
    
    // 应用参数替换
    for (auto& var : specialized) {
        var.second = CustomInstantiator::substituteParameters(var.second, parameters);
    }
    
    // 移除删除的变量
    for (const auto& deletedVar : deletedVariables) {
        specialized.erase(deletedVar);
    }
    
    return specialized;
}

String CustomVarNode::resolveVariable(const String& name, const StringUnorderedMap& context) const {
    // 首先检查上下文
    auto contextIt = context.find(name);
    if (contextIt != context.end()) {
        return contextIt->second;
    }
    
    // 然后检查自定义变量（如果未删除）
    if (!isVariableDeleted(name)) {
        return getVariable(name);
    }
    
    return "";
}

StringUnorderedMap CustomVarNode::resolveAllVariables(const StringUnorderedMap& context) const {
    StringUnorderedMap resolved = getSpecializedVariables(context);
    
    // 用上下文覆盖
    for (const auto& pair : context) {
        resolved[pair.first] = pair.second;
    }
    
    return resolved;
}

String CustomVarNode::generateVariableDeclarations(const StringUnorderedMap& parameters) const {
    std::ostringstream declarations;
    auto vars = getSpecializedVariables(parameters);
    
    for (const auto& var : vars) {
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
        
        declarations << ";\n";
    }
    
    return declarations.str();
}

String CustomVarNode::generateJSONExport(const StringUnorderedMap& parameters) const {
    std::ostringstream json;
    auto vars = getSpecializedVariables(parameters);
    
    json << "{\n";
    bool first = true;
    for (const auto& var : vars) {
        if (!first) json << ",\n";
        json << "  \"" << var.first << "\": \"" << var.second << "\"";
        first = false;
    }
    json << "\n}";
    
    return json.str();
}

String CustomVarNode::toString() const {
    std::ostringstream oss;
    oss << CustomNode::toString();
    oss << " (" << baseVariables.size() << " variables";
    if (!deletedVariables.empty()) {
        oss << ", " << deletedVariables.size() << " deleted";
    }
    oss << ")";
    return oss.str();
}

std::unique_ptr<BaseNode> CustomVarNode::clone() const {
    auto cloned = std::make_unique<CustomVarNode>(customName, position);
    cloned->customProperties = customProperties;
    cloned->specializationChain = specializationChain;
    cloned->deletionTargets = deletionTargets;
    cloned->insertionPoints = insertionPoints;
    cloned->isExported = isExported;
    cloned->baseVariables = baseVariables;
    cloned->variableTypes = variableTypes;
    cloned->variableConstraints = variableConstraints;
    cloned->deletedVariables = deletedVariables;
    
    // 克隆子节点
    for (const auto& child : children) {
        cloned->addChild(child->clone());
    }
    
    return std::unique_ptr<BaseNode>(cloned.release());
}

bool CustomVarNode::validateCustomContent() const {
    // 验证变量类型是否有效
    StringVector validTypes = {"string", "number", "boolean", "color", "size", "url"};
    
    for (const auto& varType : variableTypes) {
        if (std::find(validTypes.begin(), validTypes.end(), varType.second) == validTypes.end()) {
            std::cerr << "Warning: Unknown variable type: " << varType.second << std::endl;
        }
    }
    
    return true;
}

void CustomVarNode::applySpecializations() {
    // 应用特例化操作
}

// CustomInstantiator实现
String CustomInstantiator::substituteParameters(const String& text, const StringUnorderedMap& parameters) {
    String result = text;
    
    // 简单的参数替换逻辑
    for (const auto& param : parameters) {
        String placeholder = "{" + param.first + "}";
        size_t pos = 0;
        while ((pos = result.find(placeholder, pos)) != String::npos) {
            result.replace(pos, placeholder.length(), param.second);
            pos += param.second.length();
        }
    }
    
    return result;
}

void CustomInstantiator::applyDeletions(StringUnorderedMap& properties, const StringVector& deletions) {
    for (const auto& deletion : deletions) {
        properties.erase(deletion);
    }
}

void CustomInstantiator::applyInsertions(StringVector& sequence, const std::vector<CustomNode::InsertionPoint>& insertions) {
    // 简化的插入逻辑
    for (const auto& insertion : insertions) {
        if (insertion.position == "at top") {
            sequence.insert(sequence.begin(), insertion.content);
        } else if (insertion.position == "at bottom") {
            sequence.push_back(insertion.content);
        }
        // 其他插入操作需要更复杂的实现
    }
}

} // namespace CHTL