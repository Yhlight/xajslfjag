#include "CHTLNodes.h"
#include <algorithm>
#include <sstream>
#include <functional>

namespace CHTL {

// ====== CHTLNode 基础实现 ======

CHTLNode::CHTLNode(CHTLNodeType nodeType, const std::string& nodeName, const std::string& nodeValue)
    : type(nodeType), value(nodeValue), name(nodeName), line(0), column(0), position(0) {
}

void CHTLNode::addChild(std::shared_ptr<CHTLNode> child) {
    if (child) {
        child->parent = shared_from_this();
        children.push_back(child);
    }
}

void CHTLNode::removeChild(std::shared_ptr<CHTLNode> child) {
    auto it = std::find(children.begin(), children.end(), child);
    if (it != children.end()) {
        (*it)->parent.reset();
        children.erase(it);
    }
}

std::shared_ptr<CHTLNode> CHTLNode::findChild(const std::string& name, CHTLNodeType type) {
    for (auto& child : children) {
        if (child->name == name && child->type == type) {
            return child;
        }
    }
    return nullptr;
}

std::vector<std::shared_ptr<CHTLNode>> CHTLNode::findChildren(CHTLNodeType type) {
    std::vector<std::shared_ptr<CHTLNode>> result;
    for (auto& child : children) {
        if (child->type == type) {
            result.push_back(child);
        }
    }
    return result;
}

void CHTLNode::setAttribute(const std::string& key, const std::string& value) {
    attributes[key] = value;
}

std::string CHTLNode::getAttribute(const std::string& key) const {
    auto it = attributes.find(key);
    return it != attributes.end() ? it->second : "";
}

bool CHTLNode::hasAttribute(const std::string& key) const {
    return attributes.find(key) != attributes.end();
}

void CHTLNode::removeAttribute(const std::string& key) {
    attributes.erase(key);
}

void CHTLNode::setPosition(size_t l, size_t c, size_t p) {
    line = l;
    column = c;
    position = p;
}

void CHTLNode::traverse(std::function<void(std::shared_ptr<CHTLNode>)> visitor) {
    visitor(shared_from_this());
    for (auto& child : children) {
        child->traverse(visitor);
    }
}

bool CHTLNode::isDefinition() const {
    return type == CHTLNodeType::TEMPLATE_DEFINITION ||
           type == CHTLNodeType::CUSTOM_DEFINITION ||
           type == CHTLNodeType::ORIGIN_DEFINITION ||
           type == CHTLNodeType::CONFIGURATION_DEFINITION ||
           type == CHTLNodeType::VARIABLE_DEFINITION;
}

bool CHTLNode::isUsage() const {
    return type == CHTLNodeType::TEMPLATE_USAGE ||
           type == CHTLNodeType::CUSTOM_USAGE ||
           type == CHTLNodeType::ORIGIN_USAGE ||
           type == CHTLNodeType::VARIABLE_USAGE;
}

bool CHTLNode::isBlock() const {
    return type == CHTLNodeType::STYLE_BLOCK ||
           type == CHTLNodeType::SCRIPT_BLOCK ||
           type == CHTLNodeType::CONFIGURATION_BLOCK ||
           type == CHTLNodeType::HTML_ELEMENT;
}

bool CHTLNode::isLeaf() const {
    return type == CHTLNodeType::TEXT_NODE ||
           type == CHTLNodeType::STRING_LITERAL ||
           type == CHTLNodeType::NUMBER_LITERAL ||
           type == CHTLNodeType::IDENTIFIER ||
           isComment();
}

bool CHTLNode::isComment() const {
    return type == CHTLNodeType::LINE_COMMENT ||
           type == CHTLNodeType::BLOCK_COMMENT ||
           type == CHTLNodeType::GENERATOR_COMMENT;
}

std::string CHTLNode::getTypeName() const {
    switch (type) {
        case CHTLNodeType::DOCUMENT: return "DOCUMENT";
        case CHTLNodeType::HTML_ELEMENT: return "HTML_ELEMENT";
        case CHTLNodeType::TEXT_NODE: return "TEXT_NODE";
        case CHTLNodeType::ATTRIBUTE: return "ATTRIBUTE";
        case CHTLNodeType::USE_DECLARATION: return "USE_DECLARATION";
        case CHTLNodeType::NAMESPACE_DECLARATION: return "NAMESPACE_DECLARATION";
        case CHTLNodeType::TEMPLATE_DEFINITION: return "TEMPLATE_DEFINITION";
        case CHTLNodeType::TEMPLATE_USAGE: return "TEMPLATE_USAGE";
        case CHTLNodeType::CUSTOM_DEFINITION: return "CUSTOM_DEFINITION";
        case CHTLNodeType::CUSTOM_USAGE: return "CUSTOM_USAGE";
        case CHTLNodeType::ORIGIN_DEFINITION: return "ORIGIN_DEFINITION";
        case CHTLNodeType::ORIGIN_USAGE: return "ORIGIN_USAGE";
        case CHTLNodeType::CONFIGURATION_DEFINITION: return "CONFIGURATION_DEFINITION";
        case CHTLNodeType::CONFIGURATION_BLOCK: return "CONFIGURATION_BLOCK";
        case CHTLNodeType::IMPORT_STATEMENT: return "IMPORT_STATEMENT";
        case CHTLNodeType::STYLE_BLOCK: return "STYLE_BLOCK";
        case CHTLNodeType::STYLE_RULE: return "STYLE_RULE";
        case CHTLNodeType::STYLE_PROPERTY: return "STYLE_PROPERTY";
        case CHTLNodeType::SCRIPT_BLOCK: return "SCRIPT_BLOCK";
        case CHTLNodeType::INHERIT_STATEMENT: return "INHERIT_STATEMENT";
        case CHTLNodeType::DELETE_STATEMENT: return "DELETE_STATEMENT";
        case CHTLNodeType::INSERT_STATEMENT: return "INSERT_STATEMENT";
        case CHTLNodeType::CONSTRAINT_CLAUSE: return "CONSTRAINT_CLAUSE";
        case CHTLNodeType::VARIABLE_DEFINITION: return "VARIABLE_DEFINITION";
        case CHTLNodeType::VARIABLE_USAGE: return "VARIABLE_USAGE";
        case CHTLNodeType::LINE_COMMENT: return "LINE_COMMENT";
        case CHTLNodeType::BLOCK_COMMENT: return "BLOCK_COMMENT";
        case CHTLNodeType::GENERATOR_COMMENT: return "GENERATOR_COMMENT";
        case CHTLNodeType::STRING_LITERAL: return "STRING_LITERAL";
        case CHTLNodeType::NUMBER_LITERAL: return "NUMBER_LITERAL";
        case CHTLNodeType::UNQUOTED_LITERAL: return "UNQUOTED_LITERAL";
        case CHTLNodeType::IDENTIFIER: return "IDENTIFIER";
        case CHTLNodeType::CLASS_SELECTOR: return "CLASS_SELECTOR";
        case CHTLNodeType::ID_SELECTOR: return "ID_SELECTOR";
        case CHTLNodeType::REFERENCE_SELECTOR: return "REFERENCE_SELECTOR";
        case CHTLNodeType::INFO_BLOCK: return "INFO_BLOCK";
        case CHTLNodeType::EXPORT_BLOCK: return "EXPORT_BLOCK";
        default: return "UNKNOWN";
    }
}

std::string CHTLNode::toString(int indent) const {
    std::stringstream ss;
    std::string indentStr(indent * 2, ' ');
    
    ss << indentStr << getTypeName();
    if (!name.empty()) {
        ss << " (" << name << ")";
    }
    if (!value.empty()) {
        ss << " = \"" << value << "\"";
    }
    
    // 显示属性
    if (!attributes.empty()) {
        ss << " [";
        bool first = true;
        for (const auto& attr : attributes) {
            if (!first) ss << ", ";
            ss << attr.first << "=" << attr.second;
            first = false;
        }
        ss << "]";
    }
    
    ss << "\n";
    
    // 递归显示子节点
    for (const auto& child : children) {
        ss << child->toString(indent + 1);
    }
    
    return ss.str();
}

// ====== CHTLDocumentNode 实现 ======

CHTLDocumentNode::CHTLDocumentNode() : CHTLNode(CHTLNodeType::DOCUMENT, "document") {
}

std::vector<std::shared_ptr<CHTLNode>> CHTLDocumentNode::getUseDeclarations() {
    return findChildren(CHTLNodeType::USE_DECLARATION);
}

std::vector<std::shared_ptr<CHTLNode>> CHTLDocumentNode::getNamespaceDeclarations() {
    return findChildren(CHTLNodeType::NAMESPACE_DECLARATION);
}

std::vector<std::shared_ptr<CHTLNode>> CHTLDocumentNode::getTemplateDefinitions() {
    return findChildren(CHTLNodeType::TEMPLATE_DEFINITION);
}

std::vector<std::shared_ptr<CHTLNode>> CHTLDocumentNode::getCustomDefinitions() {
    return findChildren(CHTLNodeType::CUSTOM_DEFINITION);
}

std::vector<std::shared_ptr<CHTLNode>> CHTLDocumentNode::getImportStatements() {
    return findChildren(CHTLNodeType::IMPORT_STATEMENT);
}

std::vector<std::shared_ptr<CHTLNode>> CHTLDocumentNode::getConfigurationDefinitions() {
    return findChildren(CHTLNodeType::CONFIGURATION_DEFINITION);
}

// ====== CHTLElementNode 实现 ======

CHTLElementNode::CHTLElementNode(const std::string& tagName) 
    : CHTLNode(CHTLNodeType::HTML_ELEMENT, tagName) {
}

std::shared_ptr<CHTLNode> CHTLElementNode::getStyleBlock() {
    return findChild("style", CHTLNodeType::STYLE_BLOCK);
}

std::shared_ptr<CHTLNode> CHTLElementNode::getScriptBlock() {
    return findChild("script", CHTLNodeType::SCRIPT_BLOCK);
}

std::vector<std::shared_ptr<CHTLNode>> CHTLElementNode::getTextNodes() {
    return findChildren(CHTLNodeType::TEXT_NODE);
}

std::vector<std::shared_ptr<CHTLNode>> CHTLElementNode::getChildElements() {
    return findChildren(CHTLNodeType::HTML_ELEMENT);
}

void CHTLElementNode::setId(const std::string& id) {
    setAttribute("id", id);
}

void CHTLElementNode::setClass(const std::string& className) {
    setAttribute("class", className);
}

void CHTLElementNode::addClass(const std::string& className) {
    std::string currentClass = getAttribute("class");
    if (currentClass.empty()) {
        setClass(className);
    } else {
        setClass(currentClass + " " + className);
    }
}

void CHTLElementNode::removeClass(const std::string& className) {
    std::string currentClass = getAttribute("class");
    size_t pos = currentClass.find(className);
    if (pos != std::string::npos) {
        currentClass.erase(pos, className.length());
        // 清理多余空格
        while (currentClass.find("  ") != std::string::npos) {
            currentClass.replace(currentClass.find("  "), 2, " ");
        }
        setClass(currentClass);
    }
}

std::string CHTLElementNode::getId() const {
    return getAttribute("id");
}

std::string CHTLElementNode::getClass() const {
    return getAttribute("class");
}

std::vector<std::string> CHTLElementNode::getClasses() const {
    std::vector<std::string> classes;
    std::string classStr = getAttribute("class");
    
    std::stringstream ss(classStr);
    std::string className;
    while (std::getline(ss, className, ' ')) {
        if (!className.empty()) {
            classes.push_back(className);
        }
    }
    
    return classes;
}

// ====== CHTLTemplateNode 实现 ======

CHTLTemplateNode::CHTLTemplateNode(const std::string& name, const std::string& type)
    : CHTLNode(CHTLNodeType::TEMPLATE_DEFINITION, name), templateType(type) {
    setAttribute("template_type", type);
}

void CHTLTemplateNode::addInheritance(const std::string& parentName, const std::string& parentType) {
    setAttribute("inherit_" + std::to_string(getInheritances().size()), parentType + ":" + parentName);
}

std::vector<std::pair<std::string, std::string>> CHTLTemplateNode::getInheritances() {
    std::vector<std::pair<std::string, std::string>> inheritances;
    
    for (const auto& attr : attributes) {
        if (attr.first.find("inherit_") == 0) {
            size_t colonPos = attr.second.find(':');
            if (colonPos != std::string::npos) {
                std::string type = attr.second.substr(0, colonPos);
                std::string name = attr.second.substr(colonPos + 1);
                inheritances.push_back({name, type});
            }
        }
    }
    
    return inheritances;
}

// ====== CHTLCustomNode 实现 ======

CHTLCustomNode::CHTLCustomNode(const std::string& name, const std::string& type)
    : CHTLNode(CHTLNodeType::CUSTOM_DEFINITION, name), customType(type) {
    setAttribute("custom_type", type);
}

std::vector<std::shared_ptr<CHTLNode>> CHTLCustomNode::getDeleteStatements() {
    return findChildren(CHTLNodeType::DELETE_STATEMENT);
}

std::vector<std::shared_ptr<CHTLNode>> CHTLCustomNode::getInsertStatements() {
    return findChildren(CHTLNodeType::INSERT_STATEMENT);
}

std::vector<std::shared_ptr<CHTLNode>> CHTLCustomNode::getInheritStatements() {
    return findChildren(CHTLNodeType::INHERIT_STATEMENT);
}

// ====== CHTLStyleNode 实现 ======

CHTLStyleNode::CHTLStyleNode(bool local) : CHTLNode(CHTLNodeType::STYLE_BLOCK, "style"), isLocal(local) {
    setAttribute("is_local", local ? "true" : "false");
}

std::vector<std::shared_ptr<CHTLNode>> CHTLStyleNode::getStyleRules() {
    return findChildren(CHTLNodeType::STYLE_RULE);
}

std::vector<std::shared_ptr<CHTLNode>> CHTLStyleNode::getStyleProperties() {
    return findChildren(CHTLNodeType::STYLE_PROPERTY);
}

void CHTLStyleNode::addStyleRule(const std::string& selector, const std::vector<std::pair<std::string, std::string>>& properties) {
    auto rule = std::make_shared<CHTLNode>(CHTLNodeType::STYLE_RULE, selector);
    
    for (const auto& prop : properties) {
        auto property = std::make_shared<CHTLNode>(CHTLNodeType::STYLE_PROPERTY, prop.first, prop.second);
        rule->addChild(property);
    }
    
    addChild(rule);
}

void CHTLStyleNode::addStyleProperty(const std::string& property, const std::string& value) {
    auto prop = std::make_shared<CHTLNode>(CHTLNodeType::STYLE_PROPERTY, property, value);
    addChild(prop);
}

// ====== CHTLScriptNode 实现 ======

CHTLScriptNode::CHTLScriptNode(bool local) 
    : CHTLNode(CHTLNodeType::SCRIPT_BLOCK, "script"), isLocal(local) {
    setAttribute("is_local", local ? "true" : "false");
}

void CHTLScriptNode::setScriptContent(const std::string& content) {
    scriptContent = content;
    value = content;
}

std::string CHTLScriptNode::getScriptContent() const {
    return scriptContent;
}

// ====== CHTLImportNode 实现 ======

CHTLImportNode::CHTLImportNode(const std::string& type, const std::string& path)
    : CHTLNode(CHTLNodeType::IMPORT_STATEMENT), importType(type), importPath(path) {
    setAttribute("import_type", type);
    setAttribute("import_path", path);
}

void CHTLImportNode::setAlias(const std::string& aliasName) {
    alias = aliasName;
    setAttribute("alias", aliasName);
}

void CHTLImportNode::addExceptItem(const std::string& item) {
    exceptItems.push_back(item);
    
    std::string exceptStr;
    for (size_t i = 0; i < exceptItems.size(); ++i) {
        if (i > 0) exceptStr += ",";
        exceptStr += exceptItems[i];
    }
    setAttribute("except_items", exceptStr);
}

void CHTLImportNode::setFromPath(const std::string& path) {
    importPath = path;
    setAttribute("import_path", path);
}

// ====== CHTLNodeFactory 实现 ======

std::shared_ptr<CHTLNode> CHTLNodeFactory::createNode(CHTLNodeType type, const std::string& name, const std::string& value) {
    return std::make_shared<CHTLNode>(type, name, value);
}

std::shared_ptr<CHTLDocumentNode> CHTLNodeFactory::createDocument() {
    return std::make_shared<CHTLDocumentNode>();
}

std::shared_ptr<CHTLElementNode> CHTLNodeFactory::createElement(const std::string& tagName) {
    return std::make_shared<CHTLElementNode>(tagName);
}

std::shared_ptr<CHTLNode> CHTLNodeFactory::createTextNode(const std::string& text) {
    return std::make_shared<CHTLNode>(CHTLNodeType::TEXT_NODE, "text", text);
}

std::shared_ptr<CHTLTemplateNode> CHTLNodeFactory::createTemplate(const std::string& name, const std::string& type) {
    return std::make_shared<CHTLTemplateNode>(name, type);
}

std::shared_ptr<CHTLCustomNode> CHTLNodeFactory::createCustom(const std::string& name, const std::string& type) {
    return std::make_shared<CHTLCustomNode>(name, type);
}

std::shared_ptr<CHTLStyleNode> CHTLNodeFactory::createStyle(bool local) {
    return std::make_shared<CHTLStyleNode>(local);
}

std::shared_ptr<CHTLScriptNode> CHTLNodeFactory::createScript(bool local) {
    return std::make_shared<CHTLScriptNode>(local);
}

std::shared_ptr<CHTLImportNode> CHTLNodeFactory::createImport(const std::string& type, const std::string& path) {
    return std::make_shared<CHTLImportNode>(type, path);
}

std::shared_ptr<CHTLNode> CHTLNodeFactory::createUseDeclaration(const std::string& useType) {
    auto node = std::make_shared<CHTLNode>(CHTLNodeType::USE_DECLARATION, "use", useType);
    node->setAttribute("use_type", useType);
    return node;
}

std::shared_ptr<CHTLNode> CHTLNodeFactory::createNamespaceDeclaration(const std::string& namespaceName) {
    return std::make_shared<CHTLNode>(CHTLNodeType::NAMESPACE_DECLARATION, namespaceName);
}

std::shared_ptr<CHTLNode> CHTLNodeFactory::createConfigurationDefinition() {
    return std::make_shared<CHTLNode>(CHTLNodeType::CONFIGURATION_DEFINITION, "configuration");
}

std::shared_ptr<CHTLNode> CHTLNodeFactory::createLineComment(const std::string& comment) {
    return std::make_shared<CHTLNode>(CHTLNodeType::LINE_COMMENT, "line_comment", comment);
}

std::shared_ptr<CHTLNode> CHTLNodeFactory::createBlockComment(const std::string& comment) {
    return std::make_shared<CHTLNode>(CHTLNodeType::BLOCK_COMMENT, "block_comment", comment);
}

std::shared_ptr<CHTLNode> CHTLNodeFactory::createGeneratorComment(const std::string& comment) {
    return std::make_shared<CHTLNode>(CHTLNodeType::GENERATOR_COMMENT, "generator_comment", comment);
}

} // namespace CHTL