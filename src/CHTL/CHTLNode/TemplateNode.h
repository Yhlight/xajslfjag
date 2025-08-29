#pragma once

#include "BaseNode.h"

namespace CHTL {

// 模板基类
class TemplateNode : public BaseNode {
public:
    String templateType;  // Style, Element, Var
    String templateName;
    StringUnorderedMap templateProperties;
    StringVector inheritanceChain;
    bool isExported;
    
    TemplateNode(NodeType type, const String& tType, const String& tName, const Position& pos = Position())
        : BaseNode(type, tName, pos), templateType(tType), templateName(tName), isExported(false) {}
    
    // 模板基础操作
    void setTemplateProperty(const String& key, const String& value);
    String getTemplateProperty(const String& key, const String& defaultValue = "") const;
    bool hasTemplateProperty(const String& key) const;
    void removeTemplateProperty(const String& key);
    
    // 继承管理
    void addInheritance(const String& parentTemplate);
    void removeInheritance(const String& parentTemplate);
    bool inheritsFrom(const String& parentTemplate) const;
    StringVector getInheritanceChain() const { return inheritanceChain; }
    
    // 导出管理
    void setExported(bool exported) { isExported = exported; }
    bool getExported() const { return isExported; }
    
    // 重写基类方法
    String toString() const override;
    std::unique_ptr<BaseNode> clone() const override;
    bool validate() const override;
    
protected:
    virtual bool validateTemplateContent() const = 0;
};

// 样式组模板
class StyleTemplateNode : public TemplateNode {
public:
    StringUnorderedMap styleProperties;
    StringVector pseudoClasses;
    StringVector pseudoElements;
    
    explicit StyleTemplateNode(const String& name, const Position& pos = Position())
        : TemplateNode(NodeType::TEMPLATE_STYLE, "Style", name, pos) {}
    
    // CSS属性管理
    void setStyleProperty(const String& property, const String& value);
    String getStyleProperty(const String& property, const String& defaultValue = "") const;
    bool hasStyleProperty(const String& property) const;
    void removeStyleProperty(const String& property);
    StringVector getStylePropertyNames() const;
    
    // 伪类和伪元素
    void addPseudoClass(const String& pseudoClass);
    void addPseudoElement(const String& pseudoElement);
    bool hasPseudoClass(const String& pseudoClass) const;
    bool hasPseudoElement(const String& pseudoElement) const;
    
    // CSS生成
    String generateCSS(const String& selector = "") const;
    String generateInlineStyle() const;
    
    // 重写方法
    String toString() const override;
    std::unique_ptr<BaseNode> clone() const override;
    
protected:
    bool validateTemplateContent() const override;
};

// 元素模板
class ElementTemplateNode : public TemplateNode {
public:
    StringVector elementSequence;  // 元素序列
    StringUnorderedMap elementProperties;  // 元素属性映射
    
    explicit ElementTemplateNode(const String& name, const Position& pos = Position())
        : TemplateNode(NodeType::TEMPLATE_ELEMENT, "Element", name, pos) {}
    
    // 元素序列管理
    void addElement(const String& elementType);
    void insertElement(size_t index, const String& elementType);
    void removeElement(size_t index);
    void removeElement(const String& elementType);
    size_t getElementCount() const { return elementSequence.size(); }
    String getElement(size_t index) const;
    
    // 元素属性管理
    void setElementProperty(const String& element, const String& property, const String& value);
    String getElementProperty(const String& element, const String& property, const String& defaultValue = "") const;
    bool hasElementProperty(const String& element, const String& property) const;
    
    // HTML生成
    String generateHTML() const;
    StringVector generateElementList() const;
    
    // 重写方法
    String toString() const override;
    std::unique_ptr<BaseNode> clone() const override;
    
protected:
    bool validateTemplateContent() const override;
};

// 变量组模板
class VarTemplateNode : public TemplateNode {
public:
    StringUnorderedMap variables;
    StringUnorderedMap variableTypes;  // 变量类型信息
    StringUnorderedMap variableComments;  // 变量注释
    
    explicit VarTemplateNode(const String& name, const Position& pos = Position())
        : TemplateNode(NodeType::TEMPLATE_VAR, "Var", name, pos) {}
    
    // 变量管理
    void setVariable(const String& name, const String& value, const String& type = "string");
    String getVariable(const String& name, const String& defaultValue = "") const;
    bool hasVariable(const String& name) const;
    void removeVariable(const String& name);
    StringVector getVariableNames() const;
    
    // 变量类型管理
    void setVariableType(const String& name, const String& type);
    String getVariableType(const String& name) const;
    
    // 变量注释
    void setVariableComment(const String& name, const String& comment);
    String getVariableComment(const String& name) const;
    
    // 变量解析
    String resolveVariable(const String& name, const StringUnorderedMap& context = {}) const;
    StringUnorderedMap resolveAllVariables(const StringUnorderedMap& context = {}) const;
    
    // 生成
    String generateVariableDeclarations() const;
    String generateJSONExport() const;
    
    // 重写方法
    String toString() const override;
    std::unique_ptr<BaseNode> clone() const override;
    
protected:
    bool validateTemplateContent() const override;
};

// 模板实例化器
class TemplateInstantiator {
public:
    static std::unique_ptr<BaseNode> instantiateStyleTemplate(
        const StyleTemplateNode& templateNode, 
        const StringUnorderedMap& parameters = {});
    
    static std::unique_ptr<BaseNode> instantiateElementTemplate(
        const ElementTemplateNode& templateNode, 
        const StringUnorderedMap& parameters = {});
    
    static StringUnorderedMap instantiateVarTemplate(
        const VarTemplateNode& templateNode, 
        const StringUnorderedMap& parameters = {});
    
    // 模板合并
    static std::unique_ptr<StyleTemplateNode> mergeStyleTemplates(
        const StyleTemplateNode& base, 
        const StyleTemplateNode& derived);
    
    static std::unique_ptr<ElementTemplateNode> mergeElementTemplates(
        const ElementTemplateNode& base, 
        const ElementTemplateNode& derived);
    
    static std::unique_ptr<VarTemplateNode> mergeVarTemplates(
        const VarTemplateNode& base, 
        const VarTemplateNode& derived);
    
private:
    static String substituteParameters(const String& text, const StringUnorderedMap& parameters);
    static StringUnorderedMap mergeVariableMaps(const StringUnorderedMap& base, const StringUnorderedMap& derived);
};

// 模板解析器
class TemplateResolver {
public:
    explicit TemplateResolver(const Context& context);
    
    // 模板解析
    std::unique_ptr<TemplateNode> resolveTemplate(const String& name, const String& type) const;
    StringVector getAvailableTemplates(const String& type = "") const;
    
    // 继承解析
    std::vector<std::unique_ptr<TemplateNode>> resolveInheritanceChain(const String& name, const String& type) const;
    bool hasCircularInheritance(const String& name, const String& type) const;
    
    // 模板验证
    bool validateTemplate(const TemplateNode& templateNode) const;
    StringVector getTemplateErrors(const TemplateNode& templateNode) const;
    
private:
    const Context& context;
    
    std::unique_ptr<TemplateNode> findTemplate(const String& name, const String& type) const;
    bool checkInheritanceLoop(const String& name, const String& type, std::set<String>& visited) const;
};

} // namespace CHTL