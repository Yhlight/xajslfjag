#pragma once

#include "TemplateNode.h"

namespace CHTL {

// 自定义基类
class CustomNode : public BaseNode {
public:
    struct InsertionPoint {
        String position;  // after, before, replace, at top, at bottom
        String target;
        String content;
        size_t index;     // 用于索引访问
    };
    
    String customType;  // Style, Element, Var
    String customName;
    StringUnorderedMap customProperties;
    StringVector specializationChain;
    StringVector deletionTargets;
    std::vector<InsertionPoint> insertionPoints;
    bool isExported;
    
    CustomNode(NodeType type, const String& cType, const String& cName, const Position& pos = Position())
        : BaseNode(type, cName, pos), customType(cType), customName(cName), isExported(false) {}
    
    // 自定义基础操作
    void setCustomProperty(const String& key, const String& value);
    String getCustomProperty(const String& key, const String& defaultValue = "") const;
    bool hasCustomProperty(const String& key) const;
    void removeCustomProperty(const String& key);
    
    // 特例化管理
    void addSpecialization(const String& baseCustom);
    void removeSpecialization(const String& baseCustom);
    bool specializesFrom(const String& baseCustom) const;
    StringVector getSpecializationChain() const { return specializationChain; }
    
    // 删除操作
    void markForDeletion(const String& target);
    void unmarkForDeletion(const String& target);
    bool isMarkedForDeletion(const String& target) const;
    StringVector getDeletionTargets() const { return deletionTargets; }
    void clearDeletions();
    
    // 插入操作
    void addInsertion(const InsertionPoint& point);
    void removeInsertion(size_t index);
    std::vector<InsertionPoint> getInsertions() const { return insertionPoints; }
    void clearInsertions();
    
    // 导出管理
    void setExported(bool exported) { isExported = exported; }
    bool getExported() const { return isExported; }
    
    // 重写基类方法
    String toString() const override;
    std::unique_ptr<BaseNode> clone() const override;
    bool validate() const override;
    
protected:
    virtual bool validateCustomContent() const = 0;
    virtual void applySpecializations() = 0;
};

// 自定义样式组
class CustomStyleNode : public CustomNode {
public:
    StringUnorderedMap baseProperties;  // 基础属性
    StringUnorderedMap optionalProperties;  // 可选属性（无值样式组）
    StringVector requiredProperties;  // 必需属性
    StringUnorderedMap defaultValues;  // 默认值
    StringVector deletedProperties;  // 删除的属性
    StringVector deletedInheritances;  // 删除的继承
    
    explicit CustomStyleNode(const String& name, const Position& pos = Position())
        : CustomNode(NodeType::CUSTOM_STYLE, "Style", name, pos) {}
    
    // 基础属性管理
    void setBaseProperty(const String& property, const String& value);
    String getBaseProperty(const String& property, const String& defaultValue = "") const;
    bool hasBaseProperty(const String& property) const;
    void removeBaseProperty(const String& property);
    
    // 可选属性管理（无值样式组）
    void addOptionalProperty(const String& property);
    void removeOptionalProperty(const String& property);
    bool isOptionalProperty(const String& property) const;
    StringVector getOptionalProperties() const { 
        StringVector result;
        for (const auto& prop : optionalProperties) {
            result.push_back(prop.first);
        }
        return result;
    }
    
    // 必需属性管理
    void addRequiredProperty(const String& property);
    void removeRequiredProperty(const String& property);
    bool isRequiredProperty(const String& property) const;
    StringVector getRequiredProperties() const { return requiredProperties; }
    
    // 默认值管理
    void setDefaultValue(const String& property, const String& value);
    String getDefaultValue(const String& property) const;
    bool hasDefaultValue(const String& property) const;
    
    // 删除操作
    void deleteProperty(const String& property);
    void deleteInheritance(const String& inheritance);
    bool isPropertyDeleted(const String& property) const;
    bool isInheritanceDeleted(const String& inheritance) const;
    
    // 特例化处理
    void specializeProperty(const String& property, const String& value);
    StringUnorderedMap getSpecializedProperties(const StringUnorderedMap& parameters) const;
    
    // CSS生成
    String generateCSS(const String& selector, const StringUnorderedMap& parameters = {}) const;
    String generateInlineStyle(const StringUnorderedMap& parameters = {}) const;
    
    // 验证
    bool validateParameters(const StringUnorderedMap& parameters) const;
    StringVector getMissingRequiredProperties(const StringUnorderedMap& parameters) const;
    
    // 重写方法
    String toString() const override;
    std::unique_ptr<BaseNode> clone() const override;
    
protected:
    bool validateCustomContent() const override;
    void applySpecializations() override;
};

// 自定义元素
class CustomElementNode : public CustomNode {
public:
    StringVector elementSequence;
    StringUnorderedMap elementIndexMap;  // 元素索引映射
    StringUnorderedMap elementProperties;
    StringVector deletedElements;
    StringVector deletedInheritances;
    
    explicit CustomElementNode(const String& name, const Position& pos = Position())
        : CustomNode(NodeType::CUSTOM_ELEMENT, "Element", name, pos) {}
    
    // 元素序列管理
    void addElement(const String& elementType);
    void insertElementAt(size_t index, const String& elementType);
    void removeElementAt(size_t index);
    void removeElement(const String& elementType);
    String getElementAt(size_t index) const;
    size_t getElementCount() const { return elementSequence.size(); }
    
    // 索引访问支持
    void setElementIndex(const String& elementType, size_t index);
    size_t getElementIndex(const String& elementType) const;
    bool hasElementIndex(const String& elementType) const;
    String getElementByIndex(size_t index) const;
    
    // 元素属性管理
    void setElementProperty(const String& element, const String& property, const String& value);
    void setElementPropertyByIndex(size_t index, const String& property, const String& value);
    String getElementProperty(const String& element, const String& property) const;
    String getElementPropertyByIndex(size_t index, const String& property) const;
    
    // 删除操作
    void deleteElement(const String& element);
    void deleteElementByIndex(size_t index);
    void deleteInheritance(const String& inheritance);
    bool isElementDeleted(const String& element) const;
    bool isInheritanceDeleted(const String& inheritance) const;
    
    // 插入操作特化
    void insertAfter(const String& target, const String& newElement);
    void insertBefore(const String& target, const String& newElement);
    void replaceElement(const String& target, const String& newElement);
    void insertAtTop(const String& newElement);
    void insertAtBottom(const String& newElement);
    void insertAfterIndex(size_t index, const String& newElement);
    void insertBeforeIndex(size_t index, const String& newElement);
    void replaceElementAtIndex(size_t index, const String& newElement);
    
    // 特例化处理
    void addStyleToElement(const String& element, const String& styleContent);
    void addStyleToElementByIndex(size_t index, const String& styleContent);
    void addScriptToElement(const String& element, const String& scriptContent);
    
    // HTML生成
    String generateHTML(const StringUnorderedMap& parameters = {}) const;
    StringVector generateElementList() const;
    
    // 重写方法
    String toString() const override;
    std::unique_ptr<BaseNode> clone() const override;
    
protected:
    bool validateCustomContent() const override;
    void applySpecializations() override;
    
private:
    void updateElementIndices();
    String processInsertions(const String& html) const;
};

// 自定义变量组
class CustomVarNode : public CustomNode {
public:
    StringUnorderedMap baseVariables;
    StringUnorderedMap variableTypes;
    StringUnorderedMap variableConstraints;  // 变量约束
    StringVector deletedVariables;
    
    explicit CustomVarNode(const String& name, const Position& pos = Position())
        : CustomNode(NodeType::CUSTOM_VAR, "Var", name, pos) {}
    
    // 变量管理
    void setVariable(const String& name, const String& value, const String& type = "string");
    String getVariable(const String& name, const String& defaultValue = "") const;
    bool hasVariable(const String& name) const;
    void removeVariable(const String& name);
    StringVector getVariableNames() const;
    
    // 变量类型管理
    void setVariableType(const String& name, const String& type);
    String getVariableType(const String& name) const;
    
    // 变量约束
    void setVariableConstraint(const String& name, const String& constraint);
    String getVariableConstraint(const String& name) const;
    bool validateVariableValue(const String& name, const String& value) const;
    
    // 删除操作
    void deleteVariable(const String& name);
    bool isVariableDeleted(const String& name) const;
    
    // 特例化处理
    void specializeVariable(const String& name, const String& value);
    StringUnorderedMap getSpecializedVariables(const StringUnorderedMap& parameters) const;
    
    // 变量解析
    String resolveVariable(const String& name, const StringUnorderedMap& context = {}) const;
    StringUnorderedMap resolveAllVariables(const StringUnorderedMap& context = {}) const;
    
    // 生成
    String generateVariableDeclarations(const StringUnorderedMap& parameters = {}) const;
    String generateJSONExport(const StringUnorderedMap& parameters = {}) const;
    
    // 重写方法
    String toString() const override;
    std::unique_ptr<BaseNode> clone() const override;
    
protected:
    bool validateCustomContent() const override;
    void applySpecializations() override;
};

// 自定义实例化器
class CustomInstantiator {
public:
    static std::unique_ptr<BaseNode> instantiateCustomStyle(
        const CustomStyleNode& customNode, 
        const StringUnorderedMap& parameters = {});
    
    static std::unique_ptr<BaseNode> instantiateCustomElement(
        const CustomElementNode& customNode, 
        const StringUnorderedMap& parameters = {});
    
    static StringUnorderedMap instantiateCustomVar(
        const CustomVarNode& customNode, 
        const StringUnorderedMap& parameters = {});
    
    // 自定义合并
    static std::unique_ptr<CustomStyleNode> mergeCustomStyles(
        const CustomStyleNode& base, 
        const CustomStyleNode& derived);
    
    static std::unique_ptr<CustomElementNode> mergeCustomElements(
        const CustomElementNode& base, 
        const CustomElementNode& derived);
    
    static std::unique_ptr<CustomVarNode> mergeCustomVars(
        const CustomVarNode& base, 
        const CustomVarNode& derived);
    
    // 参数替换工具
    static String substituteParameters(const String& text, const StringUnorderedMap& parameters);
    
private:
    static void applyDeletions(StringUnorderedMap& properties, const StringVector& deletions);
    static void applyInsertions(StringVector& sequence, const std::vector<CustomNode::InsertionPoint>& insertions);
};

// 自定义解析器
class CustomResolver {
public:
    explicit CustomResolver(const Context& context);
    
    // 自定义解析
    std::unique_ptr<CustomNode> resolveCustom(const String& name, const String& type) const;
    StringVector getAvailableCustoms(const String& type = "") const;
    
    // 特例化解析
    std::vector<std::unique_ptr<CustomNode>> resolveSpecializationChain(const String& name, const String& type) const;
    bool hasCircularSpecialization(const String& name, const String& type) const;
    
    // 自定义验证
    bool validateCustom(const CustomNode& customNode) const;
    StringVector getCustomErrors(const CustomNode& customNode) const;
    
private:
    const Context& context;
    
    std::unique_ptr<CustomNode> findCustom(const String& name, const String& type) const;
    bool checkSpecializationLoop(const String& name, const String& type, std::set<String>& visited) const;
};

} // namespace CHTL