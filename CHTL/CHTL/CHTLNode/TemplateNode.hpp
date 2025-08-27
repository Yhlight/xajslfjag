#pragma once
#include "BaseNode.hpp"
#include <unordered_set>

namespace CHTL {

/**
 * 模板类型枚举
 */
enum class TemplateType {
    STYLE,                // 样式组模板
    ELEMENT,              // 元素模板
    VAR                   // 变量组模板
};

/**
 * CHTL模板节点类
 * 实现模板系统，支持样式组、元素和变量组模板
 */
class TemplateNode : public BaseNode {
private:
    TemplateType templateType;
    std::string templateName;
    std::string namespacePath;
    std::vector<std::string> inheritanceChain;  // 继承链
    std::unordered_map<std::string, std::string> variables;  // 变量映射
    bool isAbstract;                            // 是否是抽象模板
    std::unordered_set<std::string> requiredParams; // 必需参数

public:
    TemplateNode(TemplateType type, const std::string& name = "");
    
    // 模板特定方法
    TemplateType getTemplateType() const;
    void setTemplateType(TemplateType type);
    
    const std::string& getTemplateName() const;
    void setTemplateName(const std::string& name);
    
    const std::string& getNamespacePath() const;
    void setNamespacePath(const std::string& path);
    
    // 继承管理
    void addInheritance(const std::string& parentTemplate);
    void removeInheritance(const std::string& parentTemplate);
    const std::vector<std::string>& getInheritanceChain() const;
    bool hasInheritance() const;
    bool inheritsFrom(const std::string& templateName) const;
    
    // 变量管理（用于变量组模板）
    void setVariable(const std::string& name, const std::string& value);
    std::string getVariable(const std::string& name) const;
    bool hasVariable(const std::string& name) const;
    void removeVariable(const std::string& name);
    const std::unordered_map<std::string, std::string>& getVariables() const;
    
    // 抽象模板
    bool getIsAbstract() const;
    void setIsAbstract(bool abstract);
    
    // 必需参数管理
    void addRequiredParam(const std::string& param);
    void removeRequiredParam(const std::string& param);
    bool isParamRequired(const std::string& param) const;
    const std::unordered_set<std::string>& getRequiredParams() const;
    
    // 模板实例化
    std::shared_ptr<BaseNode> instantiate(const std::unordered_map<std::string, std::string>& params = {}) const;
    bool canInstantiate(const std::unordered_map<std::string, std::string>& params) const;
    
    // 模板解析
    std::string resolveContent(const std::unordered_map<std::string, std::string>& params = {}) const;
    
    // 重写基类方法
    std::string toString() const override;
    std::string toHTML() const override;
    std::string toCSS() const override;
    std::string toJavaScript() const override;
    void validate(CHTLState& state) override;
    std::shared_ptr<BaseNode> clone() const override;
    
    // 模板特殊方法
    void mergeWith(const TemplateNode& other);
    bool isCompatibleWith(const TemplateNode& other) const;
    
private:
    std::string getTypeString() const;
    std::string processVariableSubstitution(const std::string& content, 
                                           const std::unordered_map<std::string, std::string>& params) const;
};

/**
 * 模板实例化器
 * 负责处理模板的实例化过程
 */
class TemplateInstantiator {
public:
    static std::shared_ptr<BaseNode> instantiateStyleTemplate(
        const TemplateNode& templateNode,
        const std::unordered_map<std::string, std::string>& params = {}
    );
    
    static std::shared_ptr<BaseNode> instantiateElementTemplate(
        const TemplateNode& templateNode,
        const std::unordered_map<std::string, std::string>& params = {}
    );
    
    static std::shared_ptr<BaseNode> instantiateVarTemplate(
        const TemplateNode& templateNode,
        const std::unordered_map<std::string, std::string>& params = {}
    );
    
public:
    static std::shared_ptr<BaseNode> cloneAndSubstitute(
        const std::shared_ptr<BaseNode>& node,
        const std::unordered_map<std::string, std::string>& params
    );
    
    static std::string substituteVariables(
        const std::string& content,
        const std::unordered_map<std::string, std::string>& params
    );
};

}