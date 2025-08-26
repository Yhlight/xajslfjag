#ifndef CHTL_TEMPLATE_NODES_H
#define CHTL_TEMPLATE_NODES_H

#include "ASTNode.h"
#include "BasicNodes.h"

namespace CHTL {

// 模板类型
enum class TemplateType {
    STYLE,      // @Style
    ELEMENT,    // @Element
    VAR         // @Var
};

// 模板声明节点
class TemplateDeclarationNode : public ASTNode {
private:
    TemplateType templateType;
    std::string name;
    std::shared_ptr<BlockNode> body;
    std::vector<std::string> inheritFrom;  // 继承的模板
    
public:
    TemplateDeclarationNode(TemplateType type, const std::string& name, const Token& token);
    
    TemplateType getTemplateType() const { return templateType; }
    std::string getName() const { return name; }
    
    void setBody(std::shared_ptr<BlockNode> b) { body = b; }
    std::shared_ptr<BlockNode> getBody() const { return body; }
    
    void addInheritance(const std::string& parent) { inheritFrom.push_back(parent); }
    std::vector<std::string> getInheritances() const { return inheritFrom; }
    
    void accept(ASTVisitor* visitor) override;
    std::string toString() const override;
    std::shared_ptr<ASTNode> clone() const override;
};

// 自定义声明节点
class CustomDeclarationNode : public ASTNode {
private:
    TemplateType customType;
    std::string name;
    std::shared_ptr<BlockNode> body;
    std::vector<std::string> inheritFrom;
    
public:
    CustomDeclarationNode(TemplateType type, const std::string& name, const Token& token);
    
    TemplateType getCustomType() const { return customType; }
    std::string getName() const { return name; }
    
    void setBody(std::shared_ptr<BlockNode> b) { body = b; }
    std::shared_ptr<BlockNode> getBody() const { return body; }
    
    void addInheritance(const std::string& parent) { inheritFrom.push_back(parent); }
    std::vector<std::string> getInheritances() const { return inheritFrom; }
    
    void accept(ASTVisitor* visitor) override;
    std::string toString() const override;
    std::shared_ptr<ASTNode> clone() const override;
};

// 模板使用节点
class TemplateUsageNode : public ASTNode {
private:
    TemplateType templateType;
    std::string name;
    std::shared_ptr<BlockNode> specializationBlock;  // 特例化块
    bool hasFullPrefix;  // 是否使用全缀名
    
public:
    TemplateUsageNode(TemplateType type, const std::string& name, const Token& token);
    
    TemplateType getTemplateType() const { return templateType; }
    std::string getName() const { return name; }
    
    void setSpecializationBlock(std::shared_ptr<BlockNode> block) { specializationBlock = block; }
    std::shared_ptr<BlockNode> getSpecializationBlock() const { return specializationBlock; }
    bool hasSpecialization() const { return specializationBlock != nullptr; }
    
    void setHasFullPrefix(bool full) { hasFullPrefix = full; }
    bool getHasFullPrefix() const { return hasFullPrefix; }
    
    void accept(ASTVisitor* visitor) override;
    std::string toString() const override;
    std::shared_ptr<ASTNode> clone() const override;
};

// 自定义使用节点
class CustomUsageNode : public ASTNode {
private:
    TemplateType customType;
    std::string name;
    std::shared_ptr<BlockNode> specializationBlock;
    bool hasFullPrefix;
    
public:
    CustomUsageNode(TemplateType type, const std::string& name, const Token& token);
    
    TemplateType getCustomType() const { return customType; }
    std::string getName() const { return name; }
    
    void setSpecializationBlock(std::shared_ptr<BlockNode> block) { specializationBlock = block; }
    std::shared_ptr<BlockNode> getSpecializationBlock() const { return specializationBlock; }
    bool hasSpecialization() const { return specializationBlock != nullptr; }
    
    void setHasFullPrefix(bool full) { hasFullPrefix = full; }
    bool getHasFullPrefix() const { return hasFullPrefix; }
    
    void accept(ASTVisitor* visitor) override;
    std::string toString() const override;
    std::shared_ptr<ASTNode> clone() const override;
};

// 变量引用节点
class VariableReferenceNode : public ASTNode {
private:
    std::string variableGroupName;
    std::string variableName;
    std::unordered_map<std::string, std::string> specializations;  // 特例化参数
    
public:
    VariableReferenceNode(const std::string& groupName, const std::string& varName, const Token& token);
    
    std::string getVariableGroupName() const { return variableGroupName; }
    std::string getVariableName() const { return variableName; }
    
    void addSpecialization(const std::string& key, const std::string& value);
    std::unordered_map<std::string, std::string> getSpecializations() const { return specializations; }
    
    void accept(ASTVisitor* visitor) override;
    std::string toString() const override;
    std::shared_ptr<ASTNode> clone() const override;
};

// 继承语句节点
class InheritStatementNode : public ASTNode {
private:
    TemplateType inheritType;
    std::string inheritFrom;
    bool isExplicit;  // 是否使用了 inherit 关键字
    
public:
    InheritStatementNode(TemplateType type, const std::string& from, bool explicit_, const Token& token);
    
    TemplateType getInheritType() const { return inheritType; }
    std::string getInheritFrom() const { return inheritFrom; }
    bool getIsExplicit() const { return isExplicit; }
    
    void accept(ASTVisitor* visitor) override;
    std::string toString() const override;
    std::shared_ptr<ASTNode> clone() const override;
};

// 删除语句节点
class DeleteStatementNode : public ASTNode {
private:
    std::vector<std::string> targetNames;  // 要删除的属性/元素/继承
    bool isProperty;      // 删除属性
    bool isElement;       // 删除元素
    bool isInheritance;   // 删除继承
    
public:
    DeleteStatementNode(const Token& token);
    
    void addTarget(const std::string& target) { targetNames.push_back(target); }
    std::vector<std::string> getTargets() const { return targetNames; }
    
    void setIsProperty(bool prop) { isProperty = prop; }
    void setIsElement(bool elem) { isElement = elem; }
    void setIsInheritance(bool inherit) { isInheritance = inherit; }
    
    bool getIsProperty() const { return isProperty; }
    bool getIsElement() const { return isElement; }
    bool getIsInheritance() const { return isInheritance; }
    
    void accept(ASTVisitor* visitor) override;
    std::string toString() const override;
    std::shared_ptr<ASTNode> clone() const override;
};

// 插入语句节点
class InsertStatementNode : public ASTNode {
public:
    enum InsertPosition {
        AFTER,
        BEFORE,
        REPLACE,
        AT_TOP,
        AT_BOTTOM
    };
    
private:
    InsertPosition position;
    std::string targetSelector;  // 插入位置的选择器
    std::shared_ptr<BlockNode> contentBlock;  // 要插入的内容
    
public:
    InsertStatementNode(InsertPosition pos, const Token& token);
    
    void setPosition(InsertPosition pos) { position = pos; }
    InsertPosition getPosition() const { return position; }
    
    void setTargetSelector(const std::string& selector) { targetSelector = selector; }
    std::string getTargetSelector() const { return targetSelector; }
    
    void setContentBlock(std::shared_ptr<BlockNode> block) { contentBlock = block; }
    std::shared_ptr<BlockNode> getContentBlock() const { return contentBlock; }
    
    void accept(ASTVisitor* visitor) override;
    std::string toString() const override;
    std::shared_ptr<ASTNode> clone() const override;
};

// 索引访问节点
class IndexAccessNode : public ASTNode {
private:
    std::string elementName;
    int index;
    
public:
    IndexAccessNode(const std::string& name, int idx, const Token& token);
    
    std::string getElementName() const { return elementName; }
    int getIndex() const { return index; }
    
    void accept(ASTVisitor* visitor) override;
    std::string toString() const override;
    std::shared_ptr<ASTNode> clone() const override;
};

} // namespace CHTL

#endif // CHTL_TEMPLATE_NODES_H