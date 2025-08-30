#pragma once
#include "BaseNode.h"
#include "TemplateNode.h"
#include "OperatorNode.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

namespace CHTL {

// 使用已定义的枚举类型
using CustomType = ::CHTL::CustomType;
using InheritanceType = ::CHTL::InheritanceType;
using SpecializationType = ::CHTL::SpecializationType;
// InsertPosition 从 OperatorNode.h 中获取

/**
 * 继承信息
 */
struct InheritanceInfo {
    std::string templateName;           // 模板名称
    std::string templateType;           // 模板类型(@Style, @Element, @Var)
    InheritanceType inheritanceType;    // 继承类型
    std::unordered_map<std::string, std::string> parameters; // 继承时的参数
    
    InheritanceInfo() : inheritanceType(InheritanceType::COMPOSITION) {}
    InheritanceInfo(const std::string& name, const std::string& type, InheritanceType inhType)
        : templateName(name), templateType(type), inheritanceType(inhType) {}
};

/**
 * 特例化操作信息
 */
struct SpecializationInfo {
    SpecializationType type;            // 操作类型
    std::string target;                 // 目标（属性名、元素选择器等）
    std::string value;                  // 操作值
    int index;                         // 索引（用于索引访问）
    InsertPosition position;           // 插入位置（用于插入操作）
    std::vector<NodePtr> content;      // 插入/替换的内容
    
    SpecializationInfo() : index(-1), position(InsertPosition::AFTER) {}
};

/**
 * 变量引用信息
 */
struct VariableReference {
    std::string varGroupName;           // 变量组名称
    std::string varName;                // 变量名称
    std::string overrideValue;          // 覆盖值（用于特例化）
    
    VariableReference() = default;
    VariableReference(const std::string& groupName, const std::string& name)
        : varGroupName(groupName), varName(name) {}
};

/**
 * 高级样式组模板
 * 支持继承、特例化和变量
 */
class AdvancedStyleTemplateNode : public StyleTemplateNode {
public:
    /**
     * 构造函数
     */
    AdvancedStyleTemplateNode(const std::string& name);
    
    /**
     * 析构函数
     */
    virtual ~AdvancedStyleTemplateNode() = default;
    
    // ========== 继承管理 ==========
    
    /**
     * 添加继承
     */
    void addInheritance(const InheritanceInfo& inheritance);
    void addInheritance(const std::string& templateName, InheritanceType type = InheritanceType::COMPOSITION);
    
    /**
     * 移除继承
     */
    void removeInheritance(const std::string& templateName);
    
    /**
     * 获取继承列表
     */
    const std::vector<InheritanceInfo>& getInheritances() const;
    
    // ========== 无值属性支持（自定义样式组）==========
    
    /**
     * 添加无值属性
     */
    void addPropertyWithoutValue(const std::string& propertyName);
    
    /**
     * 检查是否为无值属性
     */
    bool isPropertyWithoutValue(const std::string& propertyName) const;
    
    /**
     * 获取所有无值属性
     */
    const std::vector<std::string>& getPropertiesWithoutValues() const;
    
    // ========== 特例化操作 ==========
    
    /**
     * 添加特例化操作
     */
    void addSpecialization(const SpecializationInfo& specialization);
    
    /**
     * 删除属性
     */
    void deleteProperty(const std::string& propertyName);
    
    /**
     * 删除继承
     */
    void deleteInheritance(const std::string& templateName);
    
    /**
     * 获取所有特例化操作
     */
    const std::vector<SpecializationInfo>& getSpecializations() const;
    
    // ========== 实例化 ==========
    
    /**
     * 实例化模板（支持参数和特例化）
     */
    NodePtr instantiate(const std::unordered_map<std::string, std::string>& args) const override;
    
    /**
     * 带特例化的实例化
     */
    NodePtr instantiateWithSpecializations(const std::unordered_map<std::string, std::string>& args,
                                          const std::vector<SpecializationInfo>& specializations) const;
    
    // BaseNode 接口实现
    std::string toString() const override;
    NodePtr clone() const override;

private:
    std::vector<InheritanceInfo> m_inheritances;        // 继承信息
    std::vector<std::string> m_propertiesWithoutValues; // 无值属性列表
    std::vector<SpecializationInfo> m_specializations;  // 特例化操作
};

/**
 * 高级元素模板
 * 支持索引访问、插入、删除等操作
 */
class AdvancedElementTemplateNode : public ElementTemplateNode {
public:
    /**
     * 构造函数
     */
    AdvancedElementTemplateNode(const std::string& name);
    
    /**
     * 析构函数
     */
    virtual ~AdvancedElementTemplateNode() = default;
    
    // ========== 继承管理 ==========
    
    void addInheritance(const InheritanceInfo& inheritance);
    void addInheritance(const std::string& templateName, InheritanceType type = InheritanceType::COMPOSITION);
    void removeInheritance(const std::string& templateName);
    const std::vector<InheritanceInfo>& getInheritances() const;
    
    // ========== 索引访问 ==========
    
    /**
     * 通过索引获取子元素
     */
    NodePtr getChildByIndex(int index) const;
    
    /**
     * 通过标签名和索引获取元素
     */
    NodePtr getElementByTagAndIndex(const std::string& tagName, int index) const;
    
    /**
     * 获取指定标签的所有元素
     */
    std::vector<NodePtr> getElementsByTag(const std::string& tagName) const;
    
    // ========== 特例化操作 ==========
    
    void addSpecialization(const SpecializationInfo& specialization);
    
    /**
     * 插入元素
     */
    void insertElement(InsertPosition position, const std::string& selector, 
                      const std::vector<NodePtr>& elements);
    void insertElement(InsertPosition position, int index, 
                      const std::vector<NodePtr>& elements);
    
    /**
     * 删除元素
     */
    void deleteElement(const std::string& selector);
    void deleteElement(int index);
    void deleteInheritance(const std::string& templateName);
    
    /**
     * 替换元素
     */
    void replaceElement(const std::string& selector, const std::vector<NodePtr>& elements);
    void replaceElement(int index, const std::vector<NodePtr>& elements);
    
    const std::vector<SpecializationInfo>& getSpecializations() const;
    
    // ========== 实例化 ==========
    
    NodePtr instantiate(const std::unordered_map<std::string, std::string>& args) const override;
    NodePtr instantiateWithSpecializations(const std::unordered_map<std::string, std::string>& args,
                                          const std::vector<SpecializationInfo>& specializations) const;
    
    // BaseNode 接口实现
    std::string toString() const override;
    NodePtr clone() const override;

private:
    std::vector<InheritanceInfo> m_inheritances;
    std::vector<SpecializationInfo> m_specializations;
    
    /**
     * 应用特例化操作
     */
    void applySpecializations(NodePtr instance, const std::vector<SpecializationInfo>& specializations) const;
    
    /**
     * 解析选择器
     */
    std::vector<NodePtr> resolveSelector(const std::string& selector, NodePtr root) const;
};

/**
 * 高级变量组模板
 * 支持变量覆盖和特例化
 */
class AdvancedVarTemplateNode : public VarTemplateNode {
public:
    /**
     * 构造函数
     */
    AdvancedVarTemplateNode(const std::string& name);
    
    /**
     * 析构函数
     */
    virtual ~AdvancedVarTemplateNode() = default;
    
    // ========== 继承管理 ==========
    
    void addInheritance(const InheritanceInfo& inheritance);
    void addInheritance(const std::string& templateName, InheritanceType type = InheritanceType::COMPOSITION);
    void removeInheritance(const std::string& templateName);
    const std::vector<InheritanceInfo>& getInheritances() const;
    
    // ========== 变量管理 ==========
    
    /**
     * 设置变量值
     */
    void setVariable(const std::string& name, const std::string& value);
    
    /**
     * 获取变量值
     */
    std::string getVariable(const std::string& name) const;
    
    /**
     * 获取变量值（支持覆盖）
     */
    std::string getVariable(const std::string& name, const std::string& overrideValue) const;
    
    /**
     * 检查是否有变量
     */
    bool hasVariable(const std::string& name) const;
    
    /**
     * 获取所有变量
     */
    const std::unordered_map<std::string, std::string>& getVariables() const;
    
    // ========== 特例化操作 ==========
    
    void addSpecialization(const SpecializationInfo& specialization);
    
    /**
     * 变量特例化（覆盖变量值）
     */
    void specializeVariable(const std::string& varName, const std::string& newValue);
    
    const std::vector<SpecializationInfo>& getSpecializations() const;
    
    // ========== 实例化 ==========
    
    NodePtr instantiate(const std::unordered_map<std::string, std::string>& args) const override;
    NodePtr instantiateWithSpecializations(const std::unordered_map<std::string, std::string>& args,
                                          const std::vector<SpecializationInfo>& specializations) const;
    
    // BaseNode 接口实现
    std::string toString() const override;
    NodePtr clone() const override;

private:
    std::vector<InheritanceInfo> m_inheritances;
    std::vector<SpecializationInfo> m_specializations;
    std::unordered_map<std::string, std::string> m_variables;
};

/**
 * 自定义节点基类
 * 表示CHTL中的[Custom]定义
 */
class CustomNode : public BaseNode {
public:
    /**
     * 构造函数
     */
    CustomNode(CustomType type, const std::string& name);
    
    /**
     * 析构函数
     */
    virtual ~CustomNode() = default;
    
    /**
     * 获取自定义类型
     */
    CustomType getCustomType() const;
    
    /**
     * 获取名称
     */
    const std::string& getName() const;
    
    /**
     * 设置名称
     */
    void setName(const std::string& name);
    
    // BaseNode 接口实现
    CHTLNodeType getNodeType() const { return CHTLNodeType::CUSTOM_STYLE_NODE; }
    std::string toString() const override;
    NodePtr clone() const override;
    bool validate(ErrorReporter* errorReporter = nullptr) const override;

protected:
    CustomType m_customType;
    std::string m_name;
};

/**
 * 模板使用节点
 * 表示模板的使用（实例化）
 */
class TemplateUsageNode : public BaseNode {
public:
    /**
     * 构造函数
     */
    TemplateUsageNode(const std::string& templateType, const std::string& templateName);
    
    /**
     * 析构函数
     */
    virtual ~TemplateUsageNode() = default;
    
    /**
     * 获取模板类型
     */
    const std::string& getTemplateType() const;
    
    /**
     * 获取模板名称
     */
    const std::string& getTemplateName() const;
    
    /**
     * 设置参数
     */
    void setParameter(const std::string& name, const std::string& value);
    
    /**
     * 获取参数
     */
    std::string getParameter(const std::string& name) const;
    
    /**
     * 获取所有参数
     */
    const std::unordered_map<std::string, std::string>& getParameters() const;
    
    /**
     * 添加特例化操作
     */
    void addSpecialization(const SpecializationInfo& specialization);
    
    /**
     * 获取特例化操作
     */
    const std::vector<SpecializationInfo>& getSpecializations() const;
    
    /**
     * 设置是否为全缀名
     */
    void setFullyQualified(bool fullyQualified);
    
    /**
     * 检查是否为全缀名
     */
    bool isFullyQualified() const;
    
    // BaseNode 接口实现
    CHTLNodeType getNodeType() const { return CHTLNodeType::TEMPLATE_USAGE_NODE; }
    std::string toString() const override;
    NodePtr clone() const override;
    bool validate(ErrorReporter* errorReporter = nullptr) const override;

private:
    std::string m_templateType;                         // 模板类型(@Style, @Element, @Var)
    std::string m_templateName;                         // 模板名称
    std::unordered_map<std::string, std::string> m_parameters; // 参数
    std::vector<SpecializationInfo> m_specializations;  // 特例化操作
    bool m_fullyQualified;                              // 是否为全缀名
};

} // namespace CHTL