#ifndef CHTL_TEMPLATE_NODE_H
#define CHTL_TEMPLATE_NODE_H

#include "BaseNode.h"
#include <unordered_set>

namespace CHTL {

// 模板类型
enum class TemplateType {
    STYLE,      // @Style
    ELEMENT,    // @Element
    VAR         // @Var
};

// 模板节点
class TemplateNode : public ASTNode {
public:
    TemplateNode(TemplateType templateType, const std::string& name, 
                 const TokenLocation& location)
        : ASTNode(NodeType::TEMPLATE, location), 
          templateType_(templateType), name_(name) {}
    
    TemplateType getTemplateType() const { return templateType_; }
    const std::string& getName() const { return name_; }
    
    // 内容管理
    void setContent(std::shared_ptr<ASTNode> content) {
        content_ = content;
    }
    
    std::shared_ptr<ASTNode> getContent() const { return content_; }
    
    // 继承管理
    void addInheritedTemplate(const std::string& templateName) {
        inheritedTemplates_.insert(templateName);
    }
    
    const std::unordered_set<std::string>& getInheritedTemplates() const {
        return inheritedTemplates_;
    }
    
    std::vector<std::shared_ptr<ASTNode>> getChildren() const override {
        if (content_) {
            return {content_};
        }
        return {};
    }
    
    void accept(Visitor* visitor) override;
    std::string toString() const override;
    
private:
    TemplateType templateType_;
    std::string name_;
    std::shared_ptr<ASTNode> content_;
    std::unordered_set<std::string> inheritedTemplates_;
};

// 模板使用节点（如 @Style DefaultText;）
class TemplateUseNode : public ASTNode {
public:
    TemplateUseNode(TemplateType templateType, const std::string& name,
                    const TokenLocation& location)
        : ASTNode(NodeType::FUNCTION_CALL, location),
          templateType_(templateType), name_(name) {}
    
    TemplateType getTemplateType() const { return templateType_; }
    const std::string& getName() const { return name_; }
    
    // 特例化参数（用于自定义）
    void addSpecialization(const std::string& key, const std::string& value) {
        specializations_[key] = value;
    }
    
    const std::unordered_map<std::string, std::string>& getSpecializations() const {
        return specializations_;
    }
    
    void accept(Visitor* visitor) override;
    std::string toString() const override;
    
private:
    TemplateType templateType_;
    std::string name_;
    std::unordered_map<std::string, std::string> specializations_;
};

// 扩展访问者接口
class TemplateVisitor : public Visitor {
public:
    virtual void visitTemplateNode(TemplateNode* node) = 0;
    virtual void visitTemplateUseNode(TemplateUseNode* node) = 0;
};

} // namespace CHTL

#endif // CHTL_TEMPLATE_NODE_H