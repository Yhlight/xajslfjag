#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <variant>

namespace CHTL {

// 前向声明
class BaseNode;
class ElementNode;
class TextNode;
class AttributeNode;
class StyleBlockNode;
class ScriptBlockNode;
class TemplateNode;
class CustomNode;
class OriginNode;
class ImportNode;
class NamespaceNode;
class ConfigurationNode;
class UseStatementNode;
class ConstraintNode;

// 节点类型枚举
enum class NodeType {
    BASE,
    ELEMENT,
    TEXT,
    ATTRIBUTE,
    STYLE_BLOCK,
    SCRIPT_BLOCK,
    TEMPLATE,
    CUSTOM,
    ORIGIN,
    IMPORT,
    NAMESPACE,
    CONFIGURATION,
    USE_STATEMENT,
    CONSTRAINT,
    ROOT
};

// 基础节点类
class BaseNode {
public:
    BaseNode(NodeType type, size_t line = 0, size_t column = 0);
    virtual ~BaseNode() = default;
    
    NodeType getType() const { return type_; }
    size_t getLine() const { return line_; }
    size_t getColumn() const { return column_; }
    
    void setLine(size_t line) { line_ = line; }
    void setColumn(size_t column) { column_ = column; }
    
    // 子节点管理
    void addChild(std::shared_ptr<BaseNode> child);
    void removeChild(std::shared_ptr<BaseNode> child);
    const std::vector<std::shared_ptr<BaseNode>>& getChildren() const { return children_; }
    
    // 父节点管理
    void setParent(std::shared_ptr<BaseNode> parent);
    std::shared_ptr<BaseNode> getParent() const { return parent_.lock(); }
    
    // 属性管理
    void setAttribute(const std::string& key, const std::string& value);
    std::string getAttribute(const std::string& key) const;
    bool hasAttribute(const std::string& key) const;
    const std::unordered_map<std::string, std::string>& getAllAttributes() const { return attributes_; }
    
    // 虚拟函数
    virtual std::string toString() const;
    virtual std::string toHTML() const = 0;
    
protected:
    NodeType type_;
    size_t line_;
    size_t column_;
    std::vector<std::shared_ptr<BaseNode>> children_;
    std::weak_ptr<BaseNode> parent_;
    std::unordered_map<std::string, std::string> attributes_;
};

// 根节点
class RootNode : public BaseNode {
public:
    RootNode();
    std::string toHTML() const override;
    std::string toString() const override;
};

// 元素节点
class ElementNode : public BaseNode {
public:
    ElementNode(const std::string& tagName, size_t line = 0, size_t column = 0);
    
    std::string getTagName() const { return tagName_; }
    void setTagName(const std::string& tagName) { tagName_ = tagName; }
    
    // 属性节点管理
    void addAttributeNode(std::shared_ptr<AttributeNode> attrNode);
    std::vector<std::shared_ptr<AttributeNode>> getAttributeNodes() const;
    
    // 样式和脚本块管理
    void setStyleBlock(std::shared_ptr<StyleBlockNode> styleBlock);
    void setScriptBlock(std::shared_ptr<ScriptBlockNode> scriptBlock);
    std::shared_ptr<StyleBlockNode> getStyleBlock() const { return styleBlock_; }
    std::shared_ptr<ScriptBlockNode> getScriptBlock() const { return scriptBlock_; }
    
    std::string toHTML() const override;
    std::string toString() const override;
    
private:
    std::string tagName_;
    std::shared_ptr<StyleBlockNode> styleBlock_;
    std::shared_ptr<ScriptBlockNode> scriptBlock_;
};

// 文本节点
class TextNode : public BaseNode {
public:
    TextNode(const std::string& content, size_t line = 0, size_t column = 0);
    
    std::string getContent() const { return content_; }
    void setContent(const std::string& content) { content_ = content; }
    
    std::string toHTML() const override;
    std::string toString() const override;
    
private:
    std::string content_;
};

// 属性节点
class AttributeNode : public BaseNode {
public:
    AttributeNode(const std::string& name, const std::string& value, size_t line = 0, size_t column = 0);
    
    std::string getName() const { return name_; }
    std::string getValue() const { return value_; }
    void setName(const std::string& name) { name_ = name; }
    void setValue(const std::string& value) { value_ = value; }
    
    std::string toHTML() const override;
    std::string toString() const override;
    
private:
    std::string name_;
    std::string value_;
};

// 样式块节点
class StyleBlockNode : public BaseNode {
public:
    StyleBlockNode(size_t line = 0, size_t column = 0);
    
    // 选择器管理
    void addSelector(const std::string& selector);
    const std::vector<std::string>& getSelectors() const { return selectors_; }
    
    // CSS规则管理
    void addCSSRule(const std::string& property, const std::string& value);
    const std::unordered_map<std::string, std::string>& getCSSRules() const { return cssRules_; }
    
    // 自动添加类/ID
    void setAutoAddClass(bool enabled) { autoAddClass_ = enabled; }
    void setAutoAddId(bool enabled) { autoAddId_ = enabled; }
    bool shouldAutoAddClass() const { return autoAddClass_; }
    bool shouldAutoAddId() const { return autoAddId_; }
    
    std::string toHTML() const override;
    std::string toString() const override;
    
private:
    std::vector<std::string> selectors_;
    std::unordered_map<std::string, std::string> cssRules_;
    bool autoAddClass_;
    bool autoAddId_;
};

// 脚本块节点
class ScriptBlockNode : public BaseNode {
public:
    ScriptBlockNode(size_t line = 0, size_t column = 0);
    
    // JavaScript代码管理
    void setJavaScriptCode(const std::string& code);
    std::string getJavaScriptCode() const { return jsCode_; }
    
    // 选择器管理
    void addSelector(const std::string& selector);
    const std::vector<std::string>& getSelectors() const { return selectors_; }
    
    // 自动添加类/ID
    void setAutoAddClass(bool enabled) { autoAddClass_ = enabled; }
    void setAutoAddId(bool enabled) { autoAddId_ = enabled; }
    bool shouldAutoAddClass() const { return autoAddClass_; }
    bool shouldAutoAddId() const { return autoAddId_; }
    
    std::string toHTML() const override;
    std::string toString() const override;
    
private:
    std::string jsCode_;
    std::vector<std::string> selectors_;
    bool autoAddClass_;
    bool autoAddId_;
};

// 模板节点
class TemplateNode : public BaseNode {
public:
    TemplateNode(const std::string& type, const std::string& name, size_t line = 0, size_t column = 0);
    
    std::string getTemplateType() const { return templateType_; }
    std::string getTemplateName() const { return templateName_; }
    
    std::string toHTML() const override;
    std::string toString() const override;
    
private:
    std::string templateType_;  // @Style, @Element, @Var
    std::string templateName_;
};

// 自定义节点
class CustomNode : public BaseNode {
public:
    CustomNode(const std::string& type, const std::string& name, size_t line = 0, size_t column = 0);
    
    std::string getCustomType() const { return customType_; }
    std::string getCustomName() const { return customName_; }
    
    // 特例化操作
    void addDeleteOperation(const std::string& operation);
    void addInsertOperation(const std::string& operation);
    const std::vector<std::string>& getDeleteOperations() const { return deleteOperations_; }
    const std::vector<std::string>& getInsertOperations() const { return insertOperations_; }
    
    std::string toHTML() const override;
    std::string toString() const override;
    
private:
    std::string customType_;  // @Style, @Element, @Var
    std::string customName_;
    std::vector<std::string> deleteOperations_;
    std::vector<std::string> insertOperations_;
};

// 原始嵌入节点
class OriginNode : public BaseNode {
public:
    OriginNode(const std::string& type, const std::string& name = "", size_t line = 0, size_t column = 0);
    
    std::string getOriginType() const { return originType_; }
    std::string getOriginName() const { return originName_; }
    void setContent(const std::string& content) { content_ = content; }
    std::string getContent() const { return content_; }
    
    std::string toHTML() const override;
    std::string toString() const override;
    
private:
    std::string originType_;  // @Html, @Style, @JavaScript, 自定义类型
    std::string originName_;  // 可选的名称
    std::string content_;     // 原始内容
};

// 导入节点
class ImportNode : public BaseNode {
public:
    ImportNode(const std::string& type, const std::string& path, const std::string& alias = "", 
               size_t line = 0, size_t column = 0);
    
    std::string getImportType() const { return importType_; }
    std::string getPath() const { return path_; }
    std::string getAlias() const { return alias_; }
    
    // 通配符和子模块支持
    void setWildcard(bool wildcard) { isWildcard_ = wildcard; }
    void addSubModule(const std::string& subModule);
    bool isWildcard() const { return isWildcard_; }
    const std::vector<std::string>& getSubModules() const { return subModules_; }
    
    std::string toHTML() const override;
    std::string toString() const override;
    
private:
    std::string importType_;  // @Html, @Style, @JavaScript, @Chtl, @CJmod, @Config
    std::string path_;
    std::string alias_;
    bool isWildcard_;
    std::vector<std::string> subModules_;
};

// 命名空间节点
class NamespaceNode : public BaseNode {
public:
    NamespaceNode(const std::string& name, size_t line = 0, size_t column = 0);
    
    std::string getNamespaceName() const { return namespaceName_; }
    
    // 嵌套命名空间支持
    void addNestedNamespace(std::shared_ptr<NamespaceNode> nested);
    const std::vector<std::shared_ptr<NamespaceNode>>& getNestedNamespaces() const { return nestedNamespaces_; }
    
    std::string toHTML() const override;
    std::string toString() const override;
    
private:
    std::string namespaceName_;
    std::vector<std::shared_ptr<NamespaceNode>> nestedNamespaces_;
};

// 配置节点
class ConfigurationNode : public BaseNode {
public:
    ConfigurationNode(const std::string& name = "", size_t line = 0, size_t column = 0);
    
    std::string getConfigName() const { return configName_; }
    
    // 配置项管理
    void addConfigItem(const std::string& key, const std::string& value);
    void addNameGroup(const std::string& key, const std::vector<std::string>& values);
    void addOriginType(const std::string& key, const std::string& value);
    
    const std::unordered_map<std::string, std::string>& getConfigItems() const { return configItems_; }
    const std::unordered_map<std::string, std::vector<std::string>>& getNameGroups() const { return nameGroups_; }
    const std::unordered_map<std::string, std::string>& getOriginTypes() const { return originTypes_; }
    
    std::string toHTML() const override;
    std::string toString() const override;
    
private:
    std::string configName_;
    std::unordered_map<std::string, std::string> configItems_;
    std::unordered_map<std::string, std::vector<std::string>> nameGroups_;
    std::unordered_map<std::string, std::string> originTypes_;
};

// use语句节点
class UseStatementNode : public BaseNode {
public:
    UseStatementNode(const std::string& type, const std::string& value = "", size_t line = 0, size_t column = 0);
    
    std::string getUseType() const { return useType_; }  // html5, @Config
    std::string getUseValue() const { return useValue_; }  // Basic, Std等
    
    std::string toHTML() const override;
    std::string toString() const override;
    
private:
    std::string useType_;
    std::string useValue_;
};

// 约束节点
class ConstraintNode : public BaseNode {
public:
    ConstraintNode(size_t line = 0, size_t column = 0);
    
    // 约束类型管理
    void addElementConstraint(const std::string& element);
    void addTypeConstraint(const std::string& type);
    void addGlobalConstraint(const std::string& constraint);
    
    const std::vector<std::string>& getElementConstraints() const { return elementConstraints_; }
    const std::vector<std::string>& getTypeConstraints() const { return typeConstraints_; }
    const std::vector<std::string>& getGlobalConstraints() const { return globalConstraints_; }
    
    std::string toHTML() const override;
    std::string toString() const override;
    
private:
    std::vector<std::string> elementConstraints_;  // HTML元素约束
    std::vector<std::string> typeConstraints_;     // 类型约束
    std::vector<std::string> globalConstraints_;   // 全局约束
};

// 节点工厂函数
std::shared_ptr<BaseNode> createNode(NodeType type, const std::string& name = "", 
                                    size_t line = 0, size_t column = 0);

} // namespace CHTL