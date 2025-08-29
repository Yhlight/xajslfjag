#include "BaseNode.h"
#include "TemplateNode.h"
#include "CustomNode.h"
#include "OriginNode.h"
#include "ConstraintNode.h"
#include "SpecializationNode.h"
#include "../CHTLLexer/GlobalMap.h"
#include "../../Util/StringUtils.h"
#include <iostream>
#include <queue>
#include <algorithm>

namespace CHTL {

// BaseNode实现
void BaseNode::addChild(std::unique_ptr<BaseNode> child) {
    if (child) {
        setParentForChild(child.get());
        children.push_back(std::move(child));
        onChildAdded(children.back().get());
    }
}

void BaseNode::insertChild(size_t index, std::unique_ptr<BaseNode> child) {
    if (child && index <= children.size()) {
        setParentForChild(child.get());
        children.insert(children.begin() + index, std::move(child));
        onChildAdded(children[index].get());
    }
}

void BaseNode::removeChild(size_t index) {
    if (index < children.size()) {
        BaseNode* child = children[index].get();
        removeParentForChild(child);
        onChildRemoved(child);
        children.erase(children.begin() + index);
    }
}

void BaseNode::removeChild(BaseNode* child) {
    auto it = std::find_if(children.begin(), children.end(),
                          [child](const std::unique_ptr<BaseNode>& ptr) {
                              return ptr.get() == child;
                          });
    if (it != children.end()) {
        removeParentForChild(child);
        onChildRemoved(child);
        children.erase(it);
    }
}

BaseNode* BaseNode::getChild(size_t index) const {
    return (index < children.size()) ? children[index].get() : nullptr;
}

void BaseNode::setAttribute(const String& name, const String& value) {
    String oldValue = getAttribute(name);
    attributes[name] = value;
    onAttributeChanged(name, oldValue, value);
}

String BaseNode::getAttribute(const String& name, const String& defaultValue) const {
    auto it = attributes.find(name);
    return (it != attributes.end()) ? it->second : defaultValue;
}

bool BaseNode::hasAttribute(const String& name) const {
    return attributes.find(name) != attributes.end();
}

void BaseNode::removeAttribute(const String& name) {
    auto it = attributes.find(name);
    if (it != attributes.end()) {
        String oldValue = it->second;
        attributes.erase(it);
        onAttributeChanged(name, oldValue, "");
    }
}

StringVector BaseNode::getAttributeNames() const {
    StringVector names;
    for (const auto& [name, value] : attributes) {
        names.push_back(name);
    }
    return names;
}

BaseNode* BaseNode::findChild(NodeType type) const {
    for (const auto& child : children) {
        if (child->type == type) {
            return child.get();
        }
    }
    return nullptr;
}

std::vector<BaseNode*> BaseNode::findChildren(NodeType type) const {
    std::vector<BaseNode*> result;
    for (const auto& child : children) {
        if (child->type == type) {
            result.push_back(child.get());
        }
    }
    return result;
}

BaseNode* BaseNode::findDescendant(NodeType type) const {
    for (const auto& child : children) {
        if (child->type == type) {
            return child.get();
        }
        BaseNode* found = child->findDescendant(type);
        if (found) {
            return found;
        }
    }
    return nullptr;
}

std::vector<BaseNode*> BaseNode::findDescendants(NodeType type) const {
    std::vector<BaseNode*> result;
    for (const auto& child : children) {
        if (child->type == type) {
            result.push_back(child.get());
        }
        auto childResults = child->findDescendants(type);
        result.insert(result.end(), childResults.begin(), childResults.end());
    }
    return result;
}

BaseNode* BaseNode::findByValue(const String& value) const {
    if (this->value == value) {
        return const_cast<BaseNode*>(this);
    }
    for (const auto& child : children) {
        BaseNode* found = child->findByValue(value);
        if (found) {
            return found;
        }
    }
    return nullptr;
}

BaseNode* BaseNode::findByAttribute(const String& name, const String& value) const {
    if (getAttribute(name) == value) {
        return const_cast<BaseNode*>(this);
    }
    for (const auto& child : children) {
        BaseNode* found = child->findByAttribute(name, value);
        if (found) {
            return found;
        }
    }
    return nullptr;
}

BaseNode* BaseNode::getRoot() {
    BaseNode* current = this;
    while (current->parent) {
        current = current->parent;
    }
    return current;
}

const BaseNode* BaseNode::getRoot() const {
    const BaseNode* current = this;
    while (current->parent) {
        current = current->parent;
    }
    return current;
}

size_t BaseNode::getDepth() const {
    size_t depth = 0;
    const BaseNode* current = this;
    while (current->parent) {
        depth++;
        current = current->parent;
    }
    return depth;
}

StringVector BaseNode::getPath() const {
    StringVector path;
    const BaseNode* current = this;
    while (current) {
        path.insert(path.begin(), current->value);
        current = current->parent;
    }
    return path;
}

bool BaseNode::isElementNode() const {
    return type == NodeType::ELEMENT;
}

bool BaseNode::isTextNode() const {
    return type == NodeType::TEXT;
}

bool BaseNode::isStyleNode() const {
    return type == NodeType::STYLE;
}

bool BaseNode::isScriptNode() const {
    return type == NodeType::SCRIPT;
}

bool BaseNode::isTemplateNode() const {
    return type == NodeType::TEMPLATE || type == NodeType::TEMPLATE_STYLE ||
           type == NodeType::TEMPLATE_ELEMENT || type == NodeType::TEMPLATE_VAR;
}

bool BaseNode::isCustomNode() const {
    return type == NodeType::CUSTOM || type == NodeType::CUSTOM_STYLE ||
           type == NodeType::CUSTOM_ELEMENT || type == NodeType::CUSTOM_VAR;
}

bool BaseNode::isOriginNode() const {
    return type == NodeType::ORIGIN || type == NodeType::ORIGIN_HTML ||
           type == NodeType::ORIGIN_STYLE || type == NodeType::ORIGIN_JAVASCRIPT ||
           type == NodeType::ORIGIN_CUSTOM;
}

bool BaseNode::isImportNode() const {
    return type == NodeType::IMPORT || type == NodeType::IMPORT_HTML ||
           type == NodeType::IMPORT_STYLE || type == NodeType::IMPORT_JAVASCRIPT ||
           type == NodeType::IMPORT_CHTL || type == NodeType::IMPORT_CMOD ||
           type == NodeType::IMPORT_CJMOD || type == NodeType::IMPORT_CONFIG ||
           type == NodeType::IMPORT_ORIGIN;
}

bool BaseNode::isConfigNode() const {
    return type == NodeType::CONFIGURATION || type == NodeType::CONFIG_NAME ||
           type == NodeType::CONFIG_ORIGIN_TYPE;
}

bool BaseNode::isNamespaceNode() const {
    return type == NodeType::NAMESPACE;
}

bool BaseNode::isOperatorNode() const {
    return type == NodeType::INHERIT || type == NodeType::DELETE ||
           type == NodeType::INSERT || type == NodeType::USE;
}

bool BaseNode::isCHTLJSNode() const {
    return type == NodeType::CHTLJS_SELECTOR || type == NodeType::CHTLJS_FUNCTION ||
           type == NodeType::CHTLJS_LISTEN || type == NodeType::CHTLJS_ANIMATE ||
           type == NodeType::CHTLJS_DELEGATE || type == NodeType::CHTLJS_VIR_OBJECT;
}

String BaseNode::toString() const {
    String nodeTypeName;
    switch (type) {
        case NodeType::DOCUMENT: nodeTypeName = "DOCUMENT"; break;
        case NodeType::ELEMENT: nodeTypeName = "ELEMENT"; break;
        case NodeType::TEXT: nodeTypeName = "TEXT"; break;
        case NodeType::STYLE: nodeTypeName = "STYLE"; break;
        case NodeType::SCRIPT: nodeTypeName = "SCRIPT"; break;
        case NodeType::TEMPLATE: nodeTypeName = "TEMPLATE"; break;
        case NodeType::CUSTOM: nodeTypeName = "CUSTOM"; break;
        case NodeType::ORIGIN: nodeTypeName = "ORIGIN"; break;
        case NodeType::IMPORT: nodeTypeName = "IMPORT"; break;
        case NodeType::CONFIGURATION: nodeTypeName = "CONFIGURATION"; break;
        case NodeType::NAMESPACE: nodeTypeName = "NAMESPACE"; break;
        default: nodeTypeName = "UNKNOWN"; break;
    }
    
    return nodeTypeName + "{" + value + ", children=" + std::to_string(children.size()) + 
           ", attrs=" + std::to_string(attributes.size()) + "}";
}

String BaseNode::toXML() const {
    String xml = "<node type=\"" + std::to_string(static_cast<int>(type)) + "\" value=\"" + value + "\"";
    
    for (const auto& [name, attrValue] : attributes) {
        xml += " " + name + "=\"" + attrValue + "\"";
    }
    
    if (children.empty()) {
        xml += "/>";
    } else {
        xml += ">";
        for (const auto& child : children) {
            xml += child->toXML();
        }
        xml += "</node>";
    }
    
    return xml;
}

String BaseNode::toJSON() const {
    String json = "{\"type\":" + std::to_string(static_cast<int>(type)) + 
                  ",\"value\":\"" + value + "\"";
    
    if (!attributes.empty()) {
        json += ",\"attributes\":{";
        bool first = true;
        for (const auto& [name, attrValue] : attributes) {
            if (!first) json += ",";
            json += "\"" + name + "\":\"" + attrValue + "\"";
            first = false;
        }
        json += "}";
    }
    
    if (!children.empty()) {
        json += ",\"children\":[";
        for (size_t i = 0; i < children.size(); ++i) {
            if (i > 0) json += ",";
            json += children[i]->toJSON();
        }
        json += "]";
    }
    
    json += "}";
    return json;
}

void BaseNode::dumpTree(size_t indent) const {
    String indentStr(indent * 2, ' ');
    std::cout << indentStr << toString() << std::endl;
    
    for (const auto& child : children) {
        child->dumpTree(indent + 1);
    }
}

String BaseNode::getNodeInfo() const {
    return "Node{type=" + std::to_string(static_cast<int>(type)) + 
           ", value=\"" + value + "\", pos=" + std::to_string(position.line) + 
           ":" + std::to_string(position.column) + ", depth=" + std::to_string(getDepth()) + 
           ", children=" + std::to_string(children.size()) + "}";
}

std::unique_ptr<BaseNode> BaseNode::clone() const {
    auto cloned = std::make_unique<BaseNode>(type, value, position);
    cloned->attributes = attributes;
    
    for (const auto& child : children) {
        cloned->addChild(child->clone());
    }
    
    return cloned;
}

bool BaseNode::validate() const {
    // 基础验证
    if (type == NodeType::UNKNOWN) {
        return false;
    }
    
    // 验证子节点
    for (const auto& child : children) {
        if (!child->validate()) {
            return false;
        }
    }
    
    return true;
}

StringVector BaseNode::getValidationErrors() const {
    StringVector errors;
    
    if (type == NodeType::UNKNOWN) {
        errors.push_back("Unknown node type");
    }
    
    // 收集子节点的验证错误
    for (const auto& child : children) {
        auto childErrors = child->getValidationErrors();
        errors.insert(errors.end(), childErrors.begin(), childErrors.end());
    }
    
    return errors;
}

void BaseNode::setParentForChild(BaseNode* child) {
    if (child) {
        child->parent = this;
    }
}

void BaseNode::removeParentForChild(BaseNode* child) {
    if (child) {
        child->parent = nullptr;
    }
}

// ElementNode实现
bool ElementNode::isBlockElement() const {
    return HTMLElementMap::isBlockElement(tagName);
}

bool ElementNode::isInlineElement() const {
    return HTMLElementMap::isInlineElement(tagName);
}

bool ElementNode::isSingleTag() const {
    return HTMLElementMap::isSingleTag(tagName);
}

BaseNode* ElementNode::getStyleBlock() const {
    return findChild(NodeType::STYLE);
}

BaseNode* ElementNode::getScriptBlock() const {
    return findChild(NodeType::SCRIPT);
}

std::vector<BaseNode*> ElementNode::getTextNodes() const {
    return findChildren(NodeType::TEXT);
}

String ElementNode::toHTML() const {
    String html = getOpenTag();
    
    if (!isSingleTag()) {
        for (const auto& child : children) {
            if (child->isTextNode()) {
                html += static_cast<TextNode*>(child.get())->toHTML();
            } else if (child->isElementNode()) {
                html += static_cast<ElementNode*>(child.get())->toHTML();
            }
        }
        html += getCloseTag();
    }
    
    return html;
}

String ElementNode::getOpenTag() const {
    String tag = "<" + tagName;
    
    for (const auto& [name, value] : attributes) {
        if (name != "internal") { // 跳过内部属性
            tag += " " + name + "=\"" + value + "\"";
        }
    }
    
    if (isSingleTag()) {
        tag += "/>";
    } else {
        tag += ">";
    }
    
    return tag;
}

String ElementNode::getCloseTag() const {
    return isSingleTag() ? "" : "</" + tagName + ">";
}

String ElementNode::toString() const {
    return "ElementNode{" + tagName + ", children=" + std::to_string(children.size()) + 
           ", attrs=" + std::to_string(attributes.size()) + 
           ", block=" + (isBlockElement() ? "true" : "false") + "}";
}

std::unique_ptr<BaseNode> ElementNode::clone() const {
    auto cloned = std::make_unique<ElementNode>(tagName, position);
    cloned->attributes = attributes;
    cloned->isSelfClosing = isSelfClosing;
    
    for (const auto& child : children) {
        cloned->addChild(child->clone());
    }
    
    return cloned;
}

bool ElementNode::validate() const {
    // 验证标签名
    if (tagName.empty()) {
        return false;
    }
    
    // 验证HTML元素名
    if (!HTMLElementMap::isHTMLElement(tagName)) {
        // 可能是自定义元素，暂时允许
    }
    
    // 验证单标签不应该有内容
    if (isSingleTag() && !children.empty()) {
        return false;
    }
    
    return BaseNode::validate();
}

void ElementNode::onChildAdded(BaseNode* child) {
    // 自动设置某些属性
    if (child->isStyleNode() && !hasAttribute("class") && !hasAttribute("id")) {
        // 如果有样式块但没有class或id，可能需要自动生成
    }
}

// TextNode实现
void TextNode::trim() {
    textContent = Util::StringUtils::trim(textContent);
    value = textContent;
}

void TextNode::normalize() {
    // 规范化空白字符：多个空白字符合并为一个空格
    std::regex whitespaceRegex("\\s+");
    textContent = std::regex_replace(textContent, whitespaceRegex, " ");
    trim();
}

String TextNode::toString() const {
    String preview = textContent.length() > 30 ? 
                    textContent.substr(0, 30) + "..." : textContent;
    return "TextNode{\"" + preview + "\", length=" + std::to_string(textContent.length()) + "}";
}

std::unique_ptr<BaseNode> TextNode::clone() const {
    auto cloned = std::make_unique<TextNode>(textContent, position);
    cloned->attributes = attributes;
    return cloned;
}

bool TextNode::validate() const {
    // 文本节点不应该有子节点
    if (!children.empty()) {
        return false;
    }
    
    return BaseNode::validate();
}

// NodeFactory实现
std::unique_ptr<BaseNode> NodeFactory::createDocumentNode() {
    return std::make_unique<BaseNode>(NodeType::DOCUMENT, "document");
}

std::unique_ptr<ElementNode> NodeFactory::createElementNode(const String& tagName) {
    return std::make_unique<ElementNode>(tagName);
}

std::unique_ptr<TextNode> NodeFactory::createTextNode(const String& text) {
    return std::make_unique<TextNode>(text);
}

std::unique_ptr<BaseNode> NodeFactory::createStyleNode() {
    return std::make_unique<BaseNode>(NodeType::STYLE, "style");
}

std::unique_ptr<BaseNode> NodeFactory::createScriptNode() {
    return std::make_unique<BaseNode>(NodeType::SCRIPT, "script");
}

std::unique_ptr<BaseNode> NodeFactory::createTemplateNode(const String& type, const String& name) {
    if (type == "Style") {
        return std::unique_ptr<BaseNode>(new StyleTemplateNode(name));
    } else if (type == "Element") {
        return std::unique_ptr<BaseNode>(new ElementTemplateNode(name));
    } else if (type == "Var") {
        return std::unique_ptr<BaseNode>(new VarTemplateNode(name));
    } else {
        auto node = std::make_unique<BaseNode>(NodeType::TEMPLATE, name);
        node->setAttribute("template_type", type);
        return node;
    }
}

std::unique_ptr<BaseNode> NodeFactory::createCustomNode(const String& type, const String& name) {
    if (type == "Style") {
        return std::unique_ptr<BaseNode>(new CustomStyleNode(name));
    } else if (type == "Element") {
        return std::unique_ptr<BaseNode>(new CustomElementNode(name));
    } else if (type == "Var") {
        return std::unique_ptr<BaseNode>(new CustomVarNode(name));
    } else {
        auto node = std::make_unique<BaseNode>(NodeType::CUSTOM, name);
        node->setAttribute("custom_type", type);
        return node;
    }
}

std::unique_ptr<BaseNode> NodeFactory::createOriginNode(const String& type, const String& name) {
    // 注意：这里name参数是内容，不是名称
    if (type == "Html") {
        return std::unique_ptr<BaseNode>(new HtmlOriginNode(name));
    } else if (type == "Style") {
        return std::unique_ptr<BaseNode>(new StyleOriginNode(name));
    } else if (type == "JavaScript") {
        return std::unique_ptr<BaseNode>(new JavaScriptOriginNode(name));
    } else {
        return std::unique_ptr<BaseNode>(new CustomOriginNode(type, name));
    }
}

std::unique_ptr<BaseNode> NodeFactory::createImportNode(const String& type, const String& source) {
    NodeType nodeType = NodeType::IMPORT;
    if (type == "Html") nodeType = NodeType::IMPORT_HTML;
    else if (type == "Style") nodeType = NodeType::IMPORT_STYLE;
    else if (type == "JavaScript") nodeType = NodeType::IMPORT_JAVASCRIPT;
    else if (type == "Chtl") nodeType = NodeType::IMPORT_CHTL;
    else if (type == "CJmod") nodeType = NodeType::IMPORT_CJMOD;
    
    auto node = std::make_unique<BaseNode>(nodeType, source);
    node->setAttribute("import_type", type);
    node->setAttribute("source", source);
    return node;
}

std::unique_ptr<BaseNode> NodeFactory::createConfigNode() {
    return std::make_unique<BaseNode>(NodeType::CONFIGURATION, "config");
}

std::unique_ptr<BaseNode> NodeFactory::createNamespaceNode(const String& name) {
    auto node = std::make_unique<BaseNode>(NodeType::NAMESPACE, name);
    node->setAttribute("namespace_name", name);
    return node;
}

std::unique_ptr<BaseNode> NodeFactory::createCHTLJSNode(NodeType type, const String& content) {
    auto node = std::make_unique<BaseNode>(type, content);
    node->setAttribute("chtljs_content", content);
    return node;
}

std::unique_ptr<ConstraintNode> NodeFactory::createConstraintNode(const String& constraintType, const String& targets, const String& scope) {
    ConstraintType type = ConstraintType::PRECISE; // 默认精确约束
    
    if (constraintType == "precise") {
        type = ConstraintType::PRECISE;
    } else if (constraintType == "type") {
        type = ConstraintType::TYPE;
    } else if (constraintType == "global") {
        type = ConstraintType::GLOBAL;
    }
    
    auto constraint = std::make_unique<ConstraintNode>(type);
    
    // 解析约束目标
    auto targetList = ConstraintNode::parseConstraintTargets(targets);
    constraint->targets = targetList;
    
    if (!scope.empty()) {
        constraint->scope = scope;
    }
    
    return constraint;
}

std::unique_ptr<DeleteNode> NodeFactory::createDeleteNode(const String& operationType, const String& targets) {
    auto targetList = DeleteNode::parseDeleteTargets(targets);
    auto deleteType = DeleteNode::determineOperationType(targets);
    
    auto deleteNode = std::make_unique<DeleteNode>();
    deleteNode->addTargets(targetList);
    deleteNode->operationType = deleteType;
    
    return deleteNode;
}

std::unique_ptr<InsertNode> NodeFactory::createInsertNode(const String& position, const String& target) {
    auto insertPos = InsertNode::parseInsertPosition(position);
    auto insertNode = std::make_unique<InsertNode>(insertPos);
    
    if (!target.empty()) {
        insertNode->setTarget(target);
    }
    
    return insertNode;
}

std::unique_ptr<IndexAccessNode> NodeFactory::createIndexAccessNode(const String& elementName, size_t index) {
    return IndexAccessNode::createIndexAccess(elementName, index);
}

std::unique_ptr<NoValueStyleNode> NodeFactory::createNoValueStyleNode(const String& properties) {
    auto propertyList = NoValueStyleNode::parseNoValueProperties(properties);
    return NoValueStyleNode::createNoValueStyle(propertyList);
}

} // namespace CHTL