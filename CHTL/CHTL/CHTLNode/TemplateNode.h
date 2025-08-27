#ifndef CHTL_TEMPLATE_NODE_H
#define CHTL_TEMPLATE_NODE_H

#include "BaseNode.h"
#include <unordered_map>

namespace CHTL {

// 模板类型枚举
enum class TemplateType {
    STYLE,      // @Style 样式组模板
    ELEMENT,    // @Element 元素模板
    VAR         // @Var 变量组模板
};

// 模板节点类
class TemplateNode : public BaseNode {
private:
    std::string name;
    TemplateType templateType;
    std::unordered_map<std::string, std::string> properties;  // 用于样式组和变量组
    std::vector<std::string> inheritFrom;  // 继承的模板列表
    
public:
    TemplateNode(const std::string& n, TemplateType type, int line = 0, int col = 0)
        : BaseNode(NodeType::TEMPLATE, line, col), name(n), templateType(type) {}
    
    // 名称访问
    const std::string& getName() const { return name; }
    void setName(const std::string& n) { name = n; }
    
    // 模板类型访问
    TemplateType getTemplateType() const { return templateType; }
    void setTemplateType(TemplateType type) { templateType = type; }
    
    // 属性管理（用于样式组和变量组）
    void setProperty(const std::string& key, const std::string& value);
    std::string getProperty(const std::string& key) const;
    bool hasProperty(const std::string& key) const;
    void removeProperty(const std::string& key);
    const std::unordered_map<std::string, std::string>& getProperties() const { return properties; }
    
    // 继承管理
    void addInheritance(const std::string& templateName);
    const std::vector<std::string>& getInheritances() const { return inheritFrom; }
    bool hasInheritance() const { return !inheritFrom.empty(); }
    
    // 重写虚函数
    void accept(NodeVisitor* visitor) override;
    std::shared_ptr<BaseNode> clone() const override;
    std::string toString() const override;
};

// 扩展访问者接口
class TemplateNodeVisitor : public NodeVisitor {
public:
    virtual void visitTemplateNode(TemplateNode* node) = 0;
};

} // namespace CHTL

#endif // CHTL_TEMPLATE_NODE_H