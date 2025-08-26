#ifndef CHTL_TEMPLATE_NODES_H
#define CHTL_TEMPLATE_NODES_H

#include "ASTNode.h"
#include <unordered_map>

namespace chtl {
namespace ast {

// 引用节点（@Style、@Element、@Var等的使用）
class ReferenceNode : public ASTNode {
public:
    enum class ReferenceType {
        STYLE,          // @Style
        ELEMENT,        // @Element
        VAR,            // @Var
        HTML,           // @Html
        JAVASCRIPT,     // @JavaScript
        CHTL,           // @Chtl
        CJMOD,          // @CJmod
        CONFIG,         // @Config
        UNKNOWN
    };
    
    ReferenceNode(ReferenceType type, const std::string& name,
                  const TokenLocation& location = TokenLocation())
        : ASTNode(ASTNodeType::REFERENCE, location), refType(type), name(name) {}
    
    void accept(ASTVisitor& visitor) override;
    std::shared_ptr<ASTNode> clone() const override;
    
    // 基本属性
    ReferenceType getReferenceType() const { return refType; }
    const std::string& getName() const { return name; }
    
    // 特例化参数（用于自定义）
    void addSpecialization(const std::string& key, const std::string& value);
    std::unordered_map<std::string, std::string> getSpecializations() const { return specializations; }
    bool hasSpecializations() const { return !specializations.empty(); }
    
    // from子句
    void setFromClause(std::shared_ptr<class FromClauseNode> from);
    std::shared_ptr<class FromClauseNode> getFromClause() const;
    
    // 全缀名支持
    bool isFullyQualified() const { return fullyQualified; }
    void setFullyQualified(bool fq) { fullyQualified = fq; }
    
private:
    ReferenceType refType;
    std::string name;
    std::unordered_map<std::string, std::string> specializations;
    bool fullyQualified = false;
};

// from子句节点
class FromClauseNode : public ASTNode {
public:
    FromClauseNode(const std::string& source,
                   const TokenLocation& location = TokenLocation())
        : ASTNode(ASTNodeType::FROM_CLAUSE, location), source(source) {}
    
    void accept(ASTVisitor& visitor) override;
    std::shared_ptr<ASTNode> clone() const override;
    
    const std::string& getSource() const { return source; }
    
private:
    std::string source;  // 命名空间或路径
};

// 元素模板/自定义节点
class ElementGroupNode : public ASTNode {
public:
    enum class GroupType {
        TEMPLATE,    // [Template] @Element
        CUSTOM       // [Custom] @Element
    };
    
    ElementGroupNode(GroupType type, const std::string& name,
                     const TokenLocation& location = TokenLocation())
        : ASTNode(type == GroupType::TEMPLATE ? ASTNodeType::TEMPLATE : ASTNodeType::CUSTOM, location),
          groupType(type), name(name) {}
    
    void accept(ASTVisitor& visitor) override;
    std::shared_ptr<ASTNode> clone() const override;
    
    // 基本属性
    GroupType getGroupType() const { return groupType; }
    const std::string& getName() const { return name; }
    
    // 元素管理（模板内容）
    void addElement(std::shared_ptr<ElementNode> element);
    std::vector<std::shared_ptr<ElementNode>> getElements() const;
    
    // 继承管理
    void addInheritance(std::shared_ptr<ReferenceNode> ref);
    std::vector<std::shared_ptr<ReferenceNode>> getInheritances() const;
    
private:
    GroupType groupType;
    std::string name;
};

// 变量组节点
class VarGroupNode : public ASTNode {
public:
    enum class GroupType {
        TEMPLATE,    // [Template] @Var
        CUSTOM       // [Custom] @Var
    };
    
    VarGroupNode(GroupType type, const std::string& name,
                 const TokenLocation& location = TokenLocation())
        : ASTNode(type == GroupType::TEMPLATE ? ASTNodeType::TEMPLATE : ASTNodeType::CUSTOM, location),
          groupType(type), name(name) {}
    
    void accept(ASTVisitor& visitor) override;
    std::shared_ptr<ASTNode> clone() const override;
    
    // 基本属性
    GroupType getGroupType() const { return groupType; }
    const std::string& getName() const { return name; }
    
    // 变量管理
    void addVariable(const std::string& name, const std::string& value);
    std::unordered_map<std::string, std::string> getVariables() const { return variables; }
    
    // 继承管理
    void addInheritance(std::shared_ptr<ReferenceNode> ref);
    std::vector<std::shared_ptr<ReferenceNode>> getInheritances() const;
    
private:
    GroupType groupType;
    std::string name;
    std::unordered_map<std::string, std::string> variables;
};

// 变量使用节点（在值中使用变量）
class VarUsageNode : public ASTNode {
public:
    VarUsageNode(const std::string& groupName, const std::string& varName,
                 const TokenLocation& location = TokenLocation())
        : ASTNode(ASTNodeType::EXPRESSION, location), groupName(groupName), varName(varName) {}
    
    void accept(ASTVisitor& visitor) override;
    std::shared_ptr<ASTNode> clone() const override;
    
    const std::string& getGroupName() const { return groupName; }
    const std::string& getVarName() const { return varName; }
    
    // 特例化值
    void setSpecializedValue(const std::string& value) { specializedValue = value; }
    const std::string& getSpecializedValue() const { return specializedValue; }
    bool hasSpecializedValue() const { return !specializedValue.empty(); }
    
private:
    std::string groupName;
    std::string varName;
    std::string specializedValue;
};

// 操作节点基类
class OperationNode : public ASTNode {
public:
    enum class OperationType {
        INHERIT,    // inherit操作
        DELETE,     // delete操作
        INSERT      // insert操作
    };
    
    OperationNode(OperationType type, const TokenLocation& location = TokenLocation())
        : ASTNode(getNodeType(type), location), opType(type) {}
    
    OperationType getOperationType() const { return opType; }
    
protected:
    static ASTNodeType getNodeType(OperationType type) {
        switch (type) {
            case OperationType::INHERIT: return ASTNodeType::INHERIT;
            case OperationType::DELETE: return ASTNodeType::DELETE;
            case OperationType::INSERT: return ASTNodeType::INSERT;
            default: return ASTNodeType::UNKNOWN;
        }
    }
    
private:
    OperationType opType;
};

// 继承节点
class InheritNode : public OperationNode {
public:
    InheritNode(std::shared_ptr<ReferenceNode> ref,
                const TokenLocation& location = TokenLocation())
        : OperationNode(OperationType::INHERIT, location), reference(ref) {}
    
    void accept(ASTVisitor& visitor) override;
    std::shared_ptr<ASTNode> clone() const override;
    
    std::shared_ptr<ReferenceNode> getReference() const { return reference; }
    
private:
    std::shared_ptr<ReferenceNode> reference;
};

// 删除节点
class DeleteNode : public OperationNode {
public:
    enum class DeleteTarget {
        PROPERTY,       // 删除属性
        ELEMENT,        // 删除元素
        INHERITANCE     // 删除继承
    };
    
    DeleteNode(DeleteTarget target, const std::string& targetName,
               const TokenLocation& location = TokenLocation())
        : OperationNode(OperationType::DELETE, location), 
          deleteTarget(target), targetName(targetName) {}
    
    void accept(ASTVisitor& visitor) override;
    std::shared_ptr<ASTNode> clone() const override;
    
    DeleteTarget getDeleteTarget() const { return deleteTarget; }
    const std::string& getTargetName() const { return targetName; }
    
    // 对于元素删除，可能需要选择器
    void setSelector(const std::string& sel) { selector = sel; }
    const std::string& getSelector() const { return selector; }
    
private:
    DeleteTarget deleteTarget;
    std::string targetName;
    std::string selector;  // 用于元素删除
};

// 插入节点
class InsertNode : public OperationNode {
public:
    enum class InsertPosition {
        AFTER,      // after
        BEFORE,     // before
        REPLACE,    // replace
        AT_TOP,     // at top
        AT_BOTTOM   // at bottom
    };
    
    InsertNode(InsertPosition pos, const std::string& target,
               const TokenLocation& location = TokenLocation())
        : OperationNode(OperationType::INSERT, location),
          position(pos), targetSelector(target) {}
    
    void accept(ASTVisitor& visitor) override;
    std::shared_ptr<ASTNode> clone() const override;
    
    InsertPosition getPosition() const { return position; }
    const std::string& getTargetSelector() const { return targetSelector; }
    
    // 要插入的内容
    void addContent(std::shared_ptr<ASTNode> node);
    std::vector<std::shared_ptr<ASTNode>> getContent() const;
    
private:
    InsertPosition position;
    std::string targetSelector;
};

// 扩展访问者接口
class ASTVisitor {
public:
    virtual void visit(ReferenceNode& node) = 0;
    virtual void visit(FromClauseNode& node) = 0;
    virtual void visit(ElementGroupNode& node) = 0;
    virtual void visit(VarGroupNode& node) = 0;
    virtual void visit(VarUsageNode& node) = 0;
    virtual void visit(InheritNode& node) = 0;
    virtual void visit(DeleteNode& node) = 0;
    virtual void visit(InsertNode& node) = 0;
};

} // namespace ast
} // namespace chtl

#endif // CHTL_TEMPLATE_NODES_H