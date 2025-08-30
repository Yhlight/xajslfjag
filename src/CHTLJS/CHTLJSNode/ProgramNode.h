#ifndef CHTLJS_PROGRAM_NODE_H
#define CHTLJS_PROGRAM_NODE_H

#include "BaseNode.h"

namespace CHTLJS {

// 程序根节点
class ProgramNode : public ASTNode {
public:
    ProgramNode(const std::string& filename, const TokenLocation& location)
        : ASTNode(NodeType::PROGRAM, location), filename_(filename) {}
    
    const std::string& getFilename() const { return filename_; }
    
    // 添加语句
    void addStatement(std::shared_ptr<ASTNode> statement) {
        statements_.push_back(statement);
    }
    
    const std::vector<std::shared_ptr<ASTNode>>& getStatements() const {
        return statements_;
    }
    
    std::vector<std::shared_ptr<ASTNode>> getChildren() const override {
        return statements_;
    }
    
    void accept(Visitor* visitor) override;
    std::string toString() const override;
    
private:
    std::string filename_;
    std::vector<std::shared_ptr<ASTNode>> statements_;
};

// 语句节点
class StatementNode : public ASTNode {
public:
    StatementNode(std::shared_ptr<ASTNode> expression, const TokenLocation& location)
        : ASTNode(NodeType::STATEMENT, location), expression_(expression) {}
    
    std::shared_ptr<ASTNode> getExpression() const { return expression_; }
    
    std::vector<std::shared_ptr<ASTNode>> getChildren() const override {
        if (expression_) {
            return {expression_};
        }
        return {};
    }
    
    void accept(Visitor* visitor) override;
    std::string toString() const override;
    
private:
    std::shared_ptr<ASTNode> expression_;
};

// 完整的访问者接口 - 简化版本
class CompleteVisitor : public Visitor {
public:
    virtual void visitProgramNode(ProgramNode* node) = 0;
    virtual void visitStatementNode(StatementNode* node) = 0;
    // 其他visit方法由具体类定义
};

} // namespace CHTLJS

#endif // CHTLJS_PROGRAM_NODE_H