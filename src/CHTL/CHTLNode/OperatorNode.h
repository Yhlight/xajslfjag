#ifndef CHTL_OPERATOR_NODE_H
#define CHTL_OPERATOR_NODE_H

#include "BaseNode.h"
#include <variant>

namespace CHTL {

// 操作符类型
enum class OperatorType {
    DELETE,     // delete
    INSERT,     // insert
    INHERIT,    // inherit
    EXCEPT,     // except
    USE         // use
};

// delete操作节点
class DeleteNode : public ASTNode {
public:
    DeleteNode(const TokenLocation& location)
        : ASTNode(NodeType::DELETE_OP, location) {}
    
    // 添加要删除的项
    void addDeleteItem(const std::string& item) {
        deleteItems_.push_back(item);
    }
    
    const std::vector<std::string>& getDeleteItems() const {
        return deleteItems_;
    }
    
    void accept(Visitor* visitor) override;
    std::string toString() const override;
    
private:
    std::vector<std::string> deleteItems_;
};

// insert操作节点
class InsertNode : public ASTNode {
public:
    enum class Position {
        AFTER,      // after
        BEFORE,     // before
        REPLACE,    // replace
        AT_TOP,     // at top
        AT_BOTTOM   // at bottom
    };
    
    InsertNode(Position position, const std::string& selector,
               const TokenLocation& location)
        : ASTNode(NodeType::INSERT_OP, location), 
          position_(position), selector_(selector) {}
    
    Position getPosition() const { return position_; }
    const std::string& getSelector() const { return selector_; }
    
    // 设置要插入的内容
    void setContent(std::shared_ptr<ASTNode> content) {
        content_ = content;
    }
    
    std::shared_ptr<ASTNode> getContent() const { return content_; }
    
    std::vector<std::shared_ptr<ASTNode>> getChildren() const override {
        if (content_) {
            return {content_};
        }
        return {};
    }
    
    void accept(Visitor* visitor) override;
    std::string toString() const override;
    
private:
    Position position_;
    std::string selector_;
    std::shared_ptr<ASTNode> content_;
};

// inherit操作节点
class InheritNode : public ASTNode {
public:
    InheritNode(const std::string& target, const TokenLocation& location)
        : ASTNode(NodeType::INHERIT_OP, location), target_(target) {}
    
    const std::string& getTarget() const { return target_; }
    
    void accept(Visitor* visitor) override;
    std::string toString() const override;
    
private:
    std::string target_;
};

// except约束节点
class ExceptNode : public ASTNode {
public:
    ExceptNode(const TokenLocation& location)
        : ASTNode(NodeType::EXCEPT_OP, location) {}
    
    // 添加约束项
    void addConstraint(const std::string& constraint) {
        constraints_.push_back(constraint);
    }
    
    const std::vector<std::string>& getConstraints() const {
        return constraints_;
    }
    
    void accept(Visitor* visitor) override;
    std::string toString() const override;
    
private:
    std::vector<std::string> constraints_;
};

// use语句节点
class UseNode : public ASTNode {
public:
    UseNode(const std::string& target, const TokenLocation& location)
        : ASTNode(NodeType::USE_OP, location), target_(target) {}
    
    const std::string& getTarget() const { return target_; }
    
    void accept(Visitor* visitor) override;
    std::string toString() const override;
    
private:
    std::string target_;
};

// 扩展访问者接口
class OperatorVisitor : public Visitor {
public:
    virtual void visitDeleteNode(DeleteNode* node) = 0;
    virtual void visitInsertNode(InsertNode* node) = 0;
    virtual void visitInheritNode(InheritNode* node) = 0;
    virtual void visitExceptNode(ExceptNode* node) = 0;
    virtual void visitUseNode(UseNode* node) = 0;
};

} // namespace CHTL

#endif // CHTL_OPERATOR_NODE_H