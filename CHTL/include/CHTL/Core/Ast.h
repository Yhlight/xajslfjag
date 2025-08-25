#pragma once

#include <string>
#include <vector>
#include <memory>
#include <variant>
#include <optional>

namespace CHTL {

    // Forward declarations of all AST node types
    struct AstNode;
    struct ProgramNode;
    struct ElementNode;
    struct TextNode;
    struct AttributeNode;
    struct StyleNode;
    struct StyleRuleNode;
    struct InlineStyleRuleNode;
    struct SelectorBlockNode;
    struct ScriptNode;
    struct CommentNode;
    struct TemplateDefNode;
    struct CustomDefNode;
    struct ImportNode;
    struct NamespaceNode;
    struct ConfigNode;

    // Using a variant to hold any possible child node type
    using AstNodePtr = std::shared_ptr<AstNode>;

    // Base class for all AST nodes
    struct AstNode {
        virtual ~AstNode() = default;
        virtual void Accept(Test::ASTVisitor* visitor) = 0;
    };

    // In every derived class, e.g., ElementNode:
    // void Accept(Test::ASTVisitor* visitor) override { visitor->Visit(*this); }

    // Represents a comment that should be generated (-- comment)
    struct CommentNode : public AstNode {
        std::string Content;
    };

    // Represents an attribute (e.g., id: "main")
    struct AttributeNode : public AstNode {
        std::string Key;
        std::variant<std::string, std::shared_ptr<VarUsageNode>> Value;
    };

    // Represents a text block (e.g., text { "Hello" })
    struct TextNode : public AstNode {
        std::string Content;
    };

    // Base class for any rule inside a style block
    struct StyleRuleNode : public AstNode {};

    // Represents a single CSS property (e.g., color: red;)
    struct InlineStyleRuleNode : public StyleRuleNode {
        std::string Key;
        std::optional<std::variant<std::string, std::shared_ptr<VarUsageNode>>> Value;
    };

    // Represents a selector block (e.g., .class { ... })
    struct SelectorBlockNode : public StyleRuleNode {
        std::string Selector;
        std::vector<std::shared_ptr<InlineStyleRuleNode>> Rules;
    };

    // Represents a style { ... } block
    struct StyleNode : public AstNode {
        std::vector<std::shared_ptr<StyleRuleNode>> Rules;
    };

    enum class ConstraintType {
        TypeName,      // e.g., [Template], [Custom]
        SpecificName   // e.g., span, Box
    };

    struct Constraint {
        ConstraintType Type;
        std::string Identifier;
    };

    // Represents an HTML element
    struct ElementNode : public AstNode {
        std::string TagName;
        std::optional<int> SpecializationIndex;
        std::vector<std::shared_ptr<AttributeNode>> Attributes;
        std::vector<AstNodePtr> Children;
        std::vector<Constraint> Constraints;
    };

    // Represents the root of a CHTL document
    struct ProgramNode : public AstNode {
        std::vector<AstNodePtr> Children;
        bool m_EmitHtml5Doctype = false;
    };

    enum class TemplateType {
        Style,
        Element,
        Var
    };

    struct TemplateDefinitionNode : public AstNode {
        TemplateType TemplateSubtype;
        std::string Name;
        std::vector<AstNodePtr> Body;
    };

    enum class BlockType { Template, Custom, Origin, Namespace, Configuration };

    struct TemplateUsageNode : public AstNode {
        std::string Name;
        std::optional<BlockType> Prefix;
        std::vector<AstNodePtr> SpecializationBody;
        std::optional<std::string> Namespace;
    };

    struct VarUsageNode : public AstNode {
        std::string TemplateName;
        std::string VariableName;
        std::optional<std::string> OverrideValue;
    };

    struct InheritNode : public StyleRuleNode {
        std::string TemplateName;
    };

    struct CustomDefinitionNode : public AstNode {
        TemplateType CustomSubtype;
        std::string Name;
        std::vector<AstNodePtr> Body;
    };

    struct DeleteNode : public StyleRuleNode {
        std::vector<std::string> Properties;
    };

    struct ElementDeleteNode : public AstNode {
        std::string TagName;
        std::optional<int> Index;
    };

    enum class InsertMode {
        After,
        Before,
        Replace,
        AtTop,
        AtBottom
    };

    struct InsertNode : public AstNode {
        InsertMode Mode;
        // The selector is a simplified element node, only tag and optional index are used.
        std::shared_p<ElementNode> TargetSelector;
        std::vector<AstNodePtr> Body;
    };

    struct OriginNode : public AstNode {
        std::string Type;
        std::optional<std::string> Name;
        std::string Content;
    };

    struct OriginUsageNode : public AstNode {
        std::string Type;
        std::string Name;
        std::optional<std::string> Namespace;
    };

    struct ImportNode : public AstNode {
        ImportType Type;
        std::optional<ImportCategory> Category;
        std::optional<std::string> SpecificType;
        std::optional<std::string> ImportedItemName;
        std::string Path;
        std::optional<std::string> Alias;
        // ... other members
    };

    struct NamespaceNode : public AstNode {
        std::string Name;
        std::vector<AstNodePtr> Body;
    };

    struct ConfigNode : public AstNode {
        std::optional<std::string> Name;
        // Using a simple map for now. A more robust implementation would have
        // specific structs for each configuration section ([Name], [OriginType], etc.)
        std::map<std::string, std::string> Settings;
    };

} // namespace CHTL
