#pragma once

#include "BaseNode.h"
#include <unordered_map>

namespace CHTL {

// 模板类型
enum class TemplateType {
    STYLE,      // @Style 样式组模板
    ELEMENT,    // @Element 元素模板
    VAR         // @Var 变量组模板
};

// 模板节点
class TemplateNode : public BaseNode {
public:
    TemplateNode(TemplateType type, const std::string& name);
    
    std::string GetName() const override { return templateName; }
    TemplateType GetTemplateType() const { return templateType; }
    
    std::string Generate() const override;
    NodePtr Clone() const override;
    void Accept(NodeVisitor* visitor) override;
    
    // 模板内容管理
    void SetContent(NodePtr content) { contentNode = content; }
    NodePtr GetContent() const { return contentNode; }
    
    // 样式组模板特有方法
    void AddStyleProperty(const std::string& name, const std::string& value);
    std::unordered_map<std::string, std::string> GetStyleProperties() const { return styleProperties; }
    
    // 变量组模板特有方法
    void AddVariable(const std::string& name, const std::string& value);
    std::string GetVariable(const std::string& name) const;
    std::unordered_map<std::string, std::string> GetVariables() const { return variables; }
    
    // 继承处理
    void AddInheritedTemplate(const std::string& templateName);
    std::vector<std::string> GetInheritedTemplates() const { return inheritedTemplates; }
    
    // 实例化模板
    NodePtr Instantiate(const std::unordered_map<std::string, std::string>& params = {}) const;
    
private:
    TemplateType templateType;
    std::string templateName;
    NodePtr contentNode;  // 模板内容（对于元素模板）
    
    // 样式组模板的属性
    std::unordered_map<std::string, std::string> styleProperties;
    
    // 变量组模板的变量
    std::unordered_map<std::string, std::string> variables;
    
    // 继承的模板
    std::vector<std::string> inheritedTemplates;
};

} // namespace CHTL