#pragma once
#include "BaseNode.h"
#include <unordered_map>
#include <memory>

namespace CHTL {

/**
 * 模板类型枚举
 */
enum class TemplateType {
    STYLE_TEMPLATE,     // 样式组模板 - [Template] @Style
    ELEMENT_TEMPLATE,   // 元素模板 - [Template] @Element
    VAR_TEMPLATE        // 变量组模板 - [Template] @Var
};

/**
 * 自定义类型
 */
enum class CustomType {
    CUSTOM_STYLE,       // 自定义样式组 - [Custom] @Style
    CUSTOM_ELEMENT,     // 自定义元素 - [Custom] @Element
    CUSTOM_VAR          // 自定义变量组 - [Custom] @Var
};

/**
 * 继承类型
 */
enum class InheritanceType {
    COMPOSITION,        // 组合式继承 (@Style TemplateName)
    EXPLICIT           // 显式继承 (inherit @Style TemplateName)
};

/**
 * 特例化操作类型
 */
enum class SpecializationType {
    DELETE_PROPERTY,    // 删除属性
    DELETE_INHERITANCE, // 删除继承
    INSERT_ELEMENT,     // 插入元素
    REPLACE_ELEMENT,    // 替换元素
    INDEX_ACCESS        // 索引访问
};

// InsertPosition 定义在 OperatorNode.h 中

/**
 * 模板节点基类
 * 表示CHTL中的[Template]定义
 */
class TemplateNode : public BaseNode {
public:
    /**
     * 构造函数
     */
    TemplateNode(TemplateType templateType, const std::string& templateName);
    
    /**
     * 析构函数
     */
    virtual ~TemplateNode() = default;
    
    // ========== 模板基础属性 ==========
    
    /**
     * 获取模板类型
     */
    TemplateType getTemplateType() const;
    
    /**
     * 获取模板名称
     */
    const std::string& getTemplateName() const;
    
    /**
     * 设置模板名称
     */
    void setTemplateName(const std::string& name);
    
    /**
     * 获取完整限定名称
     */
    std::string getFullQualifiedName() const;
    
    // ========== 继承管理 ==========
    
    /**
     * 添加继承的模板
     */
    void addInheritedTemplate(const std::string& templateName);
    
    /**
     * 移除继承的模板
     */
    bool removeInheritedTemplate(const std::string& templateName);
    
    /**
     * 获取所有继承的模板
     */
    const std::vector<std::string>& getInheritedTemplates() const;
    
    /**
     * 检查是否继承了指定模板
     */
    bool inheritsFrom(const std::string& templateName) const;
    
    // ========== 参数管理 ==========
    
    /**
     * 设置模板参数
     */
    void setParameter(const std::string& name, const std::string& value);
    
    /**
     * 获取模板参数
     */
    std::string getParameter(const std::string& name) const;
    
    /**
     * 检查是否有参数
     */
    bool hasParameter(const std::string& name) const;
    
    /**
     * 获取所有参数
     */
    const std::unordered_map<std::string, std::string>& getParameters() const;
    
    /**
     * 移除参数
     */
    bool removeParameter(const std::string& name);
    
    // ========== 实例化 ==========
    
    /**
     * 实例化模板
     */
    virtual NodePtr instantiate(const std::unordered_map<std::string, std::string>& args = {}) const;
    
    /**
     * 检查是否可以实例化
     */
    virtual bool canInstantiate() const;
    
    /**
     * 获取必需的参数
     */
    virtual std::vector<std::string> getRequiredParameters() const;
    
    // ========== 重写基类方法 ==========
    
    /**
     * 克隆节点
     */
    std::shared_ptr<BaseNode> clone() const override;
    
    /**
     * 深度克隆节点
     */
    std::shared_ptr<BaseNode> deepClone() const override;
    
    /**
     * 转换为字符串
     */
    std::string toString() const override;
    
    /**
     * 验证节点
     */
    bool validate(ErrorReporter* errorReporter = nullptr) const override;

protected:
    /**
     * 内部验证方法
     */
    bool internalValidate(ErrorReporter* errorReporter) const override;
    
    /**
     * 获取模板类型字符串
     */
    std::string getTemplateTypeString() const;

private:
    TemplateType m_templateType;                        // 模板类型
    std::string m_templateName;                         // 模板名称
    std::vector<std::string> m_inheritedTemplates;     // 继承的模板
    std::unordered_map<std::string, std::string> m_parameters; // 模板参数
};

/**
 * 样式组模板节点
 */
class StyleTemplateNode : public TemplateNode {
public:
    /**
     * 构造函数
     */
    StyleTemplateNode(const std::string& templateName);
    
    /**
     * 添加CSS属性
     */
    void addCssProperty(const std::string& property, const std::string& value);
    
    /**
     * 获取CSS属性
     */
    std::string getCssProperty(const std::string& property) const;
    
    /**
     * 检查是否有CSS属性
     */
    bool hasCssProperty(const std::string& property) const;
    
    /**
     * 获取所有CSS属性
     */
    const std::unordered_map<std::string, std::string>& getCssProperties() const;
    
    /**
     * 移除CSS属性
     */
    bool removeCssProperty(const std::string& property);
    
    /**
     * 实例化模板
     */
    NodePtr instantiate(const std::unordered_map<std::string, std::string>& args = {}) const override;
    
    /**
     * 克隆节点
     */
    std::shared_ptr<BaseNode> clone() const override;

private:
    std::unordered_map<std::string, std::string> m_cssProperties; // CSS属性
};

/**
 * 元素模板节点
 */
class ElementTemplateNode : public TemplateNode {
public:
    /**
     * 构造函数
     */
    ElementTemplateNode(const std::string& templateName);
    
    /**
     * 实例化模板
     */
    NodePtr instantiate(const std::unordered_map<std::string, std::string>& args = {}) const override;
    
    /**
     * 克隆节点
     */
    std::shared_ptr<BaseNode> clone() const override;
};

/**
 * 变量组模板节点
 */
class VarTemplateNode : public TemplateNode {
public:
    /**
     * 构造函数
     */
    VarTemplateNode(const std::string& templateName);
    
    /**
     * 设置变量
     */
    void setVariable(const std::string& name, const std::string& value);
    
    /**
     * 获取变量
     */
    std::string getVariable(const std::string& name) const;
    
    /**
     * 检查是否有变量
     */
    bool hasVariable(const std::string& name) const;
    
    /**
     * 获取所有变量
     */
    const std::unordered_map<std::string, std::string>& getVariables() const;
    
    /**
     * 移除变量
     */
    bool removeVariable(const std::string& name);
    
    /**
     * 实例化模板
     */
    NodePtr instantiate(const std::unordered_map<std::string, std::string>& args = {}) const override;
    
    /**
     * 克隆节点
     */
    std::shared_ptr<BaseNode> clone() const override;

private:
    std::unordered_map<std::string, std::string> m_variables; // 变量定义
};

} // namespace CHTL