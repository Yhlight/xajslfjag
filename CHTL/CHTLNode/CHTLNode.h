#ifndef CHTL_NODE_H
#define CHTL_NODE_H

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <functional>

namespace CHTL {

enum class NodeType {
    // Document structure
    DOCUMENT,
    USE_DECLARATION,
    
    // Definitions
    TEMPLATE_DEFINITION,
    CUSTOM_DEFINITION,
    ORIGIN_DEFINITION,
    NAMESPACE_DEFINITION,
    CONFIGURATION_DEFINITION,
    
    // Elements
    HTML_ELEMENT,
    TEXT_NODE,
    
    // Styles and Scripts
    STYLE_BLOCK,
    SCRIPT_BLOCK,
    STYLE_RULE,
    
    // Usage
    TEMPLATE_USAGE,
    CUSTOM_USAGE,
    VARIABLE_USAGE,
    
    // Import
    IMPORT_STATEMENT,
    
    // Attributes
    ATTRIBUTE,
    ATTRIBUTE_LIST,
    
    // Comments
    COMMENT,
    GENERATOR_COMMENT,
    
    // Literals
    STRING_LITERAL,
    NUMBER_LITERAL,
    IDENTIFIER
};

class CHTLNode {
public:
    NodeType type;
    std::string value;
    std::string name;
    std::unordered_map<std::string, std::string> attributes;
    std::vector<std::shared_ptr<CHTLNode>> children;
    
    CHTLNode* parent;
    size_t line;
    size_t column;
    
    CHTLNode(NodeType t = NodeType::DOCUMENT, const std::string& v = "", const std::string& n = "")
        : type(t), value(v), name(n), parent(nullptr), line(0), column(0) {}
    
    virtual ~CHTLNode() = default;
    
    // Tree operations
    void addChild(std::shared_ptr<CHTLNode> child);
    void removeChild(std::shared_ptr<CHTLNode> child);
    std::shared_ptr<CHTLNode> findChild(const std::string& name) const;
    std::vector<std::shared_ptr<CHTLNode>> findChildrenByType(NodeType type) const;
    
    // Attribute operations
    void setAttribute(const std::string& key, const std::string& value);
    std::string getAttribute(const std::string& key) const;
    bool hasAttribute(const std::string& key) const;
    void removeAttribute(const std::string& key);
    
    // Tree traversal
    void traverse(std::function<void(CHTLNode*)> visitor);
    std::shared_ptr<CHTLNode> findAncestor(NodeType type) const;
    
    // Utility
    bool isDefinition() const;
    bool isUsage() const;
    bool isBlock() const;
    bool isLeaf() const;
    
    // Serialization
    std::string toString() const;
    std::string toXML() const;
    
    // Type checking
    bool isElementNode() const;
    bool isTextNode() const;
    bool isStyleNode() const;
    bool isScriptNode() const;
    bool isTemplateNode() const;
    bool isCustomNode() const;
};

// Specialized node types
class DocumentNode : public CHTLNode {
public:
    std::string filename;
    std::string use_declaration;
    
    DocumentNode(const std::string& file = "") 
        : CHTLNode(NodeType::DOCUMENT), filename(file) {}
};

class ElementNode : public CHTLNode {
public:
    std::string tag_name;
    bool is_self_closing;
    
    ElementNode(const std::string& tag = "") 
        : CHTLNode(NodeType::HTML_ELEMENT, "", tag), tag_name(tag), is_self_closing(false) {}
    
    void addAttribute(const std::string& name, const std::string& value);
    std::string getAttributeValue(const std::string& name) const;
};

class TextNode : public CHTLNode {
public:
    std::string text_content;
    bool preserve_whitespace;
    
    TextNode(const std::string& text = "") 
        : CHTLNode(NodeType::TEXT_NODE, text), text_content(text), preserve_whitespace(false) {}
};

class TemplateNode : public CHTLNode {
public:
    std::string template_type; // Style, Element, Var
    std::string template_name;
    
    TemplateNode(const std::string& type = "", const std::string& name = "") 
        : CHTLNode(NodeType::TEMPLATE_DEFINITION, "", name), template_type(type), template_name(name) {}
};

class CustomNode : public CHTLNode {
public:
    std::string custom_type; // Style, Element, Var
    std::string custom_name;
    
    CustomNode(const std::string& type = "", const std::string& name = "") 
        : CHTLNode(NodeType::CUSTOM_DEFINITION, "", name), custom_type(type), custom_name(name) {}
};

class StyleNode : public CHTLNode {
public:
    bool is_local; // true for local style blocks
    std::vector<std::string> selectors;
    std::unordered_map<std::string, std::string> properties;
    
    StyleNode(bool local = false) 
        : CHTLNode(NodeType::STYLE_BLOCK), is_local(local) {}
    
    void addProperty(const std::string& property, const std::string& value);
    void addSelector(const std::string& selector);
};

class ScriptNode : public CHTLNode {
public:
    bool is_local; // true for local script blocks
    std::string script_content;
    
    ScriptNode(bool local = false) 
        : CHTLNode(NodeType::SCRIPT_BLOCK), is_local(local) {}
};

class ImportNode : public CHTLNode {
public:
    std::string import_type; // Html, Style, JavaScript, Chtl, etc.
    std::string import_path;
    std::string alias;
    
    ImportNode(const std::string& type = "", const std::string& path = "") 
        : CHTLNode(NodeType::IMPORT_STATEMENT), import_type(type), import_path(path) {}
};

// Factory for creating nodes
class NodeFactory {
public:
    static std::shared_ptr<CHTLNode> createDocument(const std::string& filename = "");
    static std::shared_ptr<CHTLNode> createElement(const std::string& tag_name);
    static std::shared_ptr<CHTLNode> createText(const std::string& text);
    static std::shared_ptr<CHTLNode> createTemplate(const std::string& type, const std::string& name);
    static std::shared_ptr<CHTLNode> createCustom(const std::string& type, const std::string& name);
    static std::shared_ptr<CHTLNode> createStyle(bool is_local = false);
    static std::shared_ptr<CHTLNode> createScript(bool is_local = false);
    static std::shared_ptr<CHTLNode> createImport(const std::string& type, const std::string& path);
    static std::shared_ptr<CHTLNode> createComment(const std::string& content, bool is_generator = false);
};

} // namespace CHTL

#endif // CHTL_NODE_H