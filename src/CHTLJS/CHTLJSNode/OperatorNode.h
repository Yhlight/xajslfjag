#ifndef CHTLJS_OPERATOR_NODE_H
#define CHTLJS_OPERATOR_NODE_H

#include "BaseNode.h"

namespace CHTLJS {

// 箭头访问节点（->）
class ArrowAccessNode : public ASTNode {
public:
    ArrowAccessNode(std::shared_ptr<ASTNode> object, std::shared_ptr<ASTNode> property,
                    const TokenLocation& location)
        : ASTNode(NodeType::ARROW_ACCESS, location),
          object_(object), property_(property) {}
    
    std::shared_ptr<ASTNode> getObject() const { return object_; }
    std::shared_ptr<ASTNode> getProperty() const { return property_; }
    
    std::vector<std::shared_ptr<ASTNode>> getChildren() const override {
        return {object_, property_};
    }
    
    void accept(Visitor* visitor) override;
    std::string toString() const override;
    
private:
    std::shared_ptr<ASTNode> object_;
    std::shared_ptr<ASTNode> property_;
};

// 事件绑定节点（&->）
class EventBindingNode : public ASTNode {
public:
    EventBindingNode(std::shared_ptr<ASTNode> selector, const std::string& event,
                     std::shared_ptr<ASTNode> handler, const TokenLocation& location)
        : ASTNode(NodeType::EVENT_BINDING, location),
          selector_(selector), event_(event), handler_(handler) {}
    
    std::shared_ptr<ASTNode> getSelector() const { return selector_; }
    const std::string& getEvent() const { return event_; }
    std::shared_ptr<ASTNode> getHandler() const { return handler_; }
    
    std::vector<std::shared_ptr<ASTNode>> getChildren() const override {
        return {selector_, handler_};
    }
    
    void accept(Visitor* visitor) override;
    std::string toString() const override;
    
private:
    std::shared_ptr<ASTNode> selector_;
    std::string event_;
    std::shared_ptr<ASTNode> handler_;
};

// 二元表达式节点
class BinaryExpressionNode : public ASTNode {
public:
    enum class Operator {
        ADD,            // +
        SUBTRACT,       // -
        MULTIPLY,       // *
        DIVIDE,         // /
        MODULO,         // %
        EQUAL,          // ==
        NOT_EQUAL,      // !=
        LESS_THAN,      // <
        GREATER_THAN,   // >
        LESS_EQUAL,     // <=
        GREATER_EQUAL,  // >=
        AND,            // &&
        OR,             // ||
        DOT             // .
    };
    
    BinaryExpressionNode(Operator op, std::shared_ptr<ASTNode> left,
                        std::shared_ptr<ASTNode> right, const TokenLocation& location)
        : ASTNode(NodeType::BINARY_EXPRESSION, location),
          operator_(op), left_(left), right_(right) {}
    
    Operator getOperator() const { return operator_; }
    std::shared_ptr<ASTNode> getLeft() const { return left_; }
    std::shared_ptr<ASTNode> getRight() const { return right_; }
    
    std::vector<std::shared_ptr<ASTNode>> getChildren() const override {
        return {left_, right_};
    }
    
    void accept(Visitor* visitor) override;
    std::string toString() const override;
    
private:
    Operator operator_;
    std::shared_ptr<ASTNode> left_;
    std::shared_ptr<ASTNode> right_;
};

// 一元表达式节点
class UnaryExpressionNode : public ASTNode {
public:
    enum class Operator {
        NOT,    // !
        MINUS,  // -
        PLUS    // +
    };
    
    UnaryExpressionNode(Operator op, std::shared_ptr<ASTNode> operand,
                       const TokenLocation& location)
        : ASTNode(NodeType::UNARY_EXPRESSION, location),
          operator_(op), operand_(operand) {}
    
    Operator getOperator() const { return operator_; }
    std::shared_ptr<ASTNode> getOperand() const { return operand_; }
    
    std::vector<std::shared_ptr<ASTNode>> getChildren() const override {
        return {operand_};
    }
    
    void accept(Visitor* visitor) override;
    std::string toString() const override;
    
private:
    Operator operator_;
    std::shared_ptr<ASTNode> operand_;
};

// 扩展访问者接口
class OperatorVisitor : public Visitor {
public:
    virtual void visitArrowAccessNode(ArrowAccessNode* node) = 0;
    virtual void visitEventBindingNode(EventBindingNode* node) = 0;
    virtual void visitBinaryExpressionNode(BinaryExpressionNode* node) = 0;
    virtual void visitUnaryExpressionNode(UnaryExpressionNode* node) = 0;
};

} // namespace CHTLJS

#endif // CHTLJS_OPERATOR_NODE_H