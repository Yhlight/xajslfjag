#pragma once

#include "../Common/ASTNode.h"
#include <vector>
#include <memory>

namespace CHTL {
namespace AST {

// 前向声明
class Visitor;

// ============================================================================
// 基础节点
// ============================================================================

// 根节点
class RootNode : public ASTNodeImpl {
public:
    RootNode() : ASTNodeImpl(NodeType::ROOT, "root") {}
    
    void accept(Visitor& visitor) override;
    std::shared_ptr<ASTNode> clone() const override;
    bool validate() const override;
};

// 文本节点
class TextNode : public ASTNodeImpl {
public:
    explicit TextNode(const std::string& text) 
        : ASTNodeImpl(NodeType::TEXT_NODE, text) {}
    
    void accept(Visitor& visitor) override;
    std::shared_ptr<ASTNode> clone() const override;
};

// 注释节点
class CommentNode : public ASTNodeImpl {
public:
    enum CommentType { SINGLE, MULTI, GENERATOR };
    CommentType comment_type;
    
    CommentNode(const std::string& content, CommentType type)
        : ASTNodeImpl(NodeType::COMMENT_NODE, content), comment_type(type) {}
    
    void accept(Visitor& visitor) override;
    std::shared_ptr<ASTNode> clone() const override;
};

// ============================================================================
// HTML元素
// ============================================================================

// HTML元素节点
class HTMLElementNode : public ASTNodeImpl {
public:
    std::string tag_name;
    bool is_self_closing;
    
    HTMLElementNode(const std::string& tag) 
        : ASTNodeImpl(NodeType::HTML_ELEMENT, tag), tag_name(tag), is_self_closing(false) {}
    
    void accept(Visitor& visitor) override;
    std::shared_ptr<ASTNode> clone() const override;
    bool validate() const override;
    
    // 辅助方法
    void addAttribute(const std::string& name, const std::string& value);
    std::string getAttributeValue(const std::string& name) const;
    bool hasAttribute(const std::string& name) const;
};

// 属性节点
class AttributeNode : public ASTNodeImpl {
public:
    std::string name;
    std::string value;
    
    AttributeNode(const std::string& attr_name, const std::string& attr_value)
        : ASTNodeImpl(NodeType::ATTRIBUTE), name(attr_name), value(attr_value) {}
    
    void accept(Visitor& visitor) override;
    std::shared_ptr<ASTNode> clone() const override;
};

// ============================================================================
// 样式系统
// ============================================================================

// 样式块节点（局部和全局）
class StyleBlockNode : public ASTNodeImpl {
public:
    enum StyleBlockType { LOCAL, GLOBAL };
    StyleBlockType block_type;
    
    explicit StyleBlockNode(StyleBlockType type) 
        : ASTNodeImpl(NodeType::STYLE_BLOCK), block_type(type) {}
    
    void accept(Visitor& visitor) override;
    std::shared_ptr<ASTNode> clone() const override;
    bool validate() const override;
};

// 样式属性节点
class StylePropertyNode : public ASTNodeImpl {
public:
    std::string property_name;
    std::string property_value;
    
    StylePropertyNode(const std::string& name, const std::string& value)
        : ASTNodeImpl(NodeType::STYLE_PROPERTY), property_name(name), property_value(value) {}
    
    void accept(Visitor& visitor) override;
    std::shared_ptr<ASTNode> clone() const override;
};

// 样式选择器节点
class StyleSelectorNode : public ASTNodeImpl {
public:
    enum SelectorType { CLASS, ID, TAG, PSEUDO_CLASS, PSEUDO_ELEMENT, REFERENCE };
    SelectorType selector_type;
    std::string selector_value;
    bool is_auto_generated; // 是否是自动生成的
    
    StyleSelectorNode(SelectorType type, const std::string& value)
        : ASTNodeImpl(NodeType::STYLE_SELECTOR), selector_type(type), 
          selector_value(value), is_auto_generated(false) {}
    
    void accept(Visitor& visitor) override;
    std::shared_ptr<ASTNode> clone() const override;
    
    // 辅助方法
    std::string getFullSelector() const;
    bool isReference() const { return selector_type == REFERENCE; }
};

// ============================================================================
// 脚本系统
// ============================================================================

// 脚本块节点
class ScriptBlockNode : public ASTNodeImpl {
public:
    enum ScriptBlockType { LOCAL, GLOBAL };
    ScriptBlockType block_type;
    std::string script_content;
    
    ScriptBlockNode(ScriptBlockType type, const std::string& content = "")
        : ASTNodeImpl(NodeType::SCRIPT_BLOCK), block_type(type), script_content(content) {}
    
    void accept(Visitor& visitor) override;
    std::shared_ptr<ASTNode> clone() const override;
};

// ============================================================================
// 模板系统
// ============================================================================

// 模板样式组节点
class TemplateStyleNode : public ASTNodeImpl {
public:
    std::string template_name;
    std::vector<std::string> inheritance; // 继承的模板
    
    explicit TemplateStyleNode(const std::string& name)
        : ASTNodeImpl(NodeType::TEMPLATE_STYLE, name), template_name(name) {}
    
    void accept(Visitor& visitor) override;
    std::shared_ptr<ASTNode> clone() const override;
    bool validate() const override;
    
    void addInheritance(const std::string& parent_template) {
        inheritance.push_back(parent_template);
    }
};

// 模板元素节点
class TemplateElementNode : public ASTNodeImpl {
public:
    std::string template_name;
    std::vector<std::string> inheritance;
    
    explicit TemplateElementNode(const std::string& name)
        : ASTNodeImpl(NodeType::TEMPLATE_ELEMENT, name), template_name(name) {}
    
    void accept(Visitor& visitor) override;
    std::shared_ptr<ASTNode> clone() const override;
    bool validate() const override;
    
    void addInheritance(const std::string& parent_template) {
        inheritance.push_back(parent_template);
    }
};

// 模板变量组节点
class TemplateVarNode : public ASTNodeImpl {
public:
    std::string template_name;
    std::unordered_map<std::string, std::string> variables;
    
    explicit TemplateVarNode(const std::string& name)
        : ASTNodeImpl(NodeType::TEMPLATE_VAR, name), template_name(name) {}
    
    void accept(Visitor& visitor) override;
    std::shared_ptr<ASTNode> clone() const override;
    
    void addVariable(const std::string& name, const std::string& value) {
        variables[name] = value;
    }
    
    std::string getVariable(const std::string& name) const {
        auto it = variables.find(name);
        return it != variables.end() ? it->second : "";
    }
};

// ============================================================================
// 自定义系统
// ============================================================================

// 自定义样式组节点
class CustomStyleNode : public ASTNodeImpl {
public:
    std::string custom_name;
    std::vector<std::string> inheritance;
    std::vector<std::string> deleted_properties; // delete操作的属性
    std::vector<std::string> deleted_inheritance; // delete操作的继承
    
    explicit CustomStyleNode(const std::string& name)
        : ASTNodeImpl(NodeType::CUSTOM_STYLE, name), custom_name(name) {}
    
    void accept(Visitor& visitor) override;
    std::shared_ptr<ASTNode> clone() const override;
    bool validate() const override;
    
    void addInheritance(const std::string& parent) {
        inheritance.push_back(parent);
    }
    
    void addDeletedProperty(const std::string& property) {
        deleted_properties.push_back(property);
    }
    
    void addDeletedInheritance(const std::string& parent) {
        deleted_inheritance.push_back(parent);
    }
};

// 自定义元素节点
class CustomElementNode : public ASTNodeImpl {
public:
    std::string custom_name;
    std::vector<std::string> inheritance;
    
    explicit CustomElementNode(const std::string& name)
        : ASTNodeImpl(NodeType::CUSTOM_ELEMENT, name), custom_name(name) {}
    
    void accept(Visitor& visitor) override;
    std::shared_ptr<ASTNode> clone() const override;
    bool validate() const override;
    
    void addInheritance(const std::string& parent) {
        inheritance.push_back(parent);
    }
};

// 自定义变量组节点
class CustomVarNode : public ASTNodeImpl {
public:
    std::string custom_name;
    std::unordered_map<std::string, std::string> variables;
    std::unordered_map<std::string, std::string> specializations; // 特例化
    
    explicit CustomVarNode(const std::string& name)
        : ASTNodeImpl(NodeType::CUSTOM_VAR, name), custom_name(name) {}
    
    void accept(Visitor& visitor) override;
    std::shared_ptr<ASTNode> clone() const override;
    
    void addVariable(const std::string& name, const std::string& value) {
        variables[name] = value;
    }
    
    void addSpecialization(const std::string& name, const std::string& value) {
        specializations[name] = value;
    }
};

// ============================================================================
// 原始嵌入系统
// ============================================================================

// 原始嵌入块节点
class OriginBlockNode : public ASTNodeImpl {
public:
    enum OriginType { HTML, STYLE, JAVASCRIPT, CUSTOM };
    OriginType origin_type;
    std::string type_name; // 自定义类型名称
    std::string origin_name; // 带名原始嵌入的名称
    std::string content;
    
    OriginBlockNode(OriginType type, const std::string& content = "")
        : ASTNodeImpl(NodeType::ORIGIN_BLOCK), origin_type(type), content(content) {}
    
    OriginBlockNode(const std::string& custom_type, const std::string& content = "")
        : ASTNodeImpl(NodeType::ORIGIN_BLOCK), origin_type(CUSTOM), 
          type_name(custom_type), content(content) {}
    
    void accept(Visitor& visitor) override;
    std::shared_ptr<ASTNode> clone() const override;
    
    bool isCustomType() const { return origin_type == CUSTOM; }
    bool isNamed() const { return !origin_name.empty(); }
    
    void setName(const std::string& name) { origin_name = name; }
};

// ============================================================================
// 配置系统
// ============================================================================

// 配置节点
class ConfigurationNode : public ASTNodeImpl {
public:
    std::string config_name; // 命名配置组的名称（可选）
    std::unordered_map<std::string, std::string> settings;
    
    ConfigurationNode(const std::string& name = "")
        : ASTNodeImpl(NodeType::CONFIGURATION), config_name(name) {}
    
    void accept(Visitor& visitor) override;
    std::shared_ptr<ASTNode> clone() const override;
    
    void setSetting(const std::string& key, const std::string& value) {
        settings[key] = value;
    }
    
    std::string getSetting(const std::string& key, const std::string& default_value = "") const {
        auto it = settings.find(key);
        return it != settings.end() ? it->second : default_value;
    }
    
    bool isNamed() const { return !config_name.empty(); }
};

// 配置项节点
class ConfigItemNode : public ASTNodeImpl {
public:
    std::string key;
    std::string value;
    
    ConfigItemNode(const std::string& k, const std::string& v)
        : ASTNodeImpl(NodeType::CONFIG_ITEM), key(k), value(v) {}
    
    void accept(Visitor& visitor) override;
    std::shared_ptr<ASTNode> clone() const override;
};

// ============================================================================
// 命名空间系统
// ============================================================================

// 命名空间节点
class NamespaceNode : public ASTNodeImpl {
public:
    std::string namespace_name;
    std::string parent_namespace;
    
    explicit NamespaceNode(const std::string& name)
        : ASTNodeImpl(NodeType::NAMESPACE, name), namespace_name(name) {}
    
    void accept(Visitor& visitor) override;
    std::shared_ptr<ASTNode> clone() const override;
    bool validate() const override;
    
    std::string getFullPath() const {
        return parent_namespace.empty() ? namespace_name : parent_namespace + "." + namespace_name;
    }
    
    void setParentNamespace(const std::string& parent) {
        parent_namespace = parent;
    }
};

// ============================================================================
// 导入系统
// ============================================================================

// 导入节点
class ImportNode : public ASTNodeImpl {
public:
    enum ImportType { HTML_FILE, STYLE_FILE, JAVASCRIPT_FILE, CHTL_FILE, CJMOD_FILE, 
                      TEMPLATE_ITEM, CUSTOM_ITEM, ORIGIN_ITEM, CONFIG_ITEM };
    ImportType import_type;
    std::string import_target;  // 导入的目标（文件路径或项目名）
    std::string import_path;    // from后的路径
    std::string alias_name;     // as后的别名
    
    ImportNode(ImportType type, const std::string& target, const std::string& path)
        : ASTNodeImpl(NodeType::IMPORT), import_type(type), 
          import_target(target), import_path(path) {}
    
    void accept(Visitor& visitor) override;
    std::shared_ptr<ASTNode> clone() const override;
    bool validate() const override;
    
    void setAlias(const std::string& alias) { alias_name = alias; }
    bool hasAlias() const { return !alias_name.empty(); }
    
    std::string getImportTypeString() const;
};

// ============================================================================
// 特殊操作节点
// ============================================================================

// 继承操作节点
class InheritNode : public ASTNodeImpl {
public:
    std::string target_name;
    std::string target_type; // @Style, @Element, @Var等
    
    InheritNode(const std::string& type, const std::string& name)
        : ASTNodeImpl(NodeType::INHERIT), target_type(type), target_name(name) {}
    
    void accept(Visitor& visitor) override;
    std::shared_ptr<ASTNode> clone() const override;
};

// 删除操作节点
class DeleteNode : public ASTNodeImpl {
public:
    enum DeleteType { PROPERTY, INHERITANCE, ELEMENT };
    DeleteType delete_type;
    std::vector<std::string> targets;
    
    DeleteNode(DeleteType type) : ASTNodeImpl(NodeType::DELETE_OP), delete_type(type) {}
    
    void accept(Visitor& visitor) override;
    std::shared_ptr<ASTNode> clone() const override;
    
    void addTarget(const std::string& target) {
        targets.push_back(target);
    }
};

// 插入操作节点
class InsertNode : public ASTNodeImpl {
public:
    enum InsertPosition { AFTER, BEFORE, REPLACE, AT_TOP, AT_BOTTOM };
    InsertPosition position;
    std::string selector; // 位置选择器
    
    InsertNode(InsertPosition pos, const std::string& sel = "")
        : ASTNodeImpl(NodeType::INSERT_OP), position(pos), selector(sel) {}
    
    void accept(Visitor& visitor) override;
    std::shared_ptr<ASTNode> clone() const override;
    
    std::string getPositionString() const;
};

// ============================================================================
// 约束系统
// ============================================================================

// 约束节点
class ExceptNode : public ASTNodeImpl {
public:
    enum ExceptType { ELEMENT_CONSTRAINT, TYPE_CONSTRAINT };
    ExceptType except_type;
    std::vector<std::string> constraints;
    
    explicit ExceptNode(ExceptType type) : ASTNodeImpl(NodeType::EXCEPT), except_type(type) {}
    
    void accept(Visitor& visitor) override;
    std::shared_ptr<ASTNode> clone() const override;
    
    void addConstraint(const std::string& constraint) {
        constraints.push_back(constraint);
    }
};

// ============================================================================
// Use声明系统
// ============================================================================

// Use声明节点
class UseDeclarationNode : public ASTNodeImpl {
public:
    enum UseType { HTML5, CONFIG_GROUP };
    UseType use_type;
    std::string target_name; // 对于配置组是配置组名称
    
    UseDeclarationNode(UseType type, const std::string& target = "")
        : ASTNodeImpl(NodeType::USE_DECLARATION), use_type(type), target_name(target) {}
    
    void accept(Visitor& visitor) override;
    std::shared_ptr<ASTNode> clone() const override;
    bool validate() const override;
};

// ============================================================================
// 变量和函数调用
// ============================================================================

// 变量节点
class VariableNode : public ASTNodeImpl {
public:
    std::string variable_name;
    std::string variable_value;
    std::string variable_type; // 变量类型（如果有）
    
    VariableNode(const std::string& name, const std::string& value, const std::string& type = "")
        : ASTNodeImpl(NodeType::VARIABLE), variable_name(name), 
          variable_value(value), variable_type(type) {}
    
    void accept(Visitor& visitor) override;
    std::shared_ptr<ASTNode> clone() const override;
};

// 函数调用节点
class FunctionCallNode : public ASTNodeImpl {
public:
    std::string function_name;
    std::vector<std::string> arguments;
    std::unordered_map<std::string, std::string> named_arguments;
    
    explicit FunctionCallNode(const std::string& name)
        : ASTNodeImpl(NodeType::FUNCTION_CALL), function_name(name) {}
    
    void accept(Visitor& visitor) override;
    std::shared_ptr<ASTNode> clone() const override;
    
    void addArgument(const std::string& arg) {
        arguments.push_back(arg);
    }
    
    void addNamedArgument(const std::string& name, const std::string& value) {
        named_arguments[name] = value;
    }
};

} // namespace AST
} // namespace CHTL