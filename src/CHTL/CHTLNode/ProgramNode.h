#ifndef CHTL_PROGRAM_NODE_H
#define CHTL_PROGRAM_NODE_H

#include "BaseNode.h"

namespace CHTL {

// 程序根节点
class ProgramNode : public ASTNode {
public:
    ProgramNode(const std::string& filename, const TokenLocation& location)
        : ASTNode(NodeType::PROGRAM, location), filename_(filename) {}
    
    const std::string& getFilename() const { return filename_; }
    
    // 添加顶层节点
    void addTopLevelNode(std::shared_ptr<ASTNode> node) {
        topLevelNodes_.push_back(node);
    }
    
    const std::vector<std::shared_ptr<ASTNode>>& getTopLevelNodes() const {
        return topLevelNodes_;
    }
    
    std::vector<std::shared_ptr<ASTNode>> getChildren() const override {
        return topLevelNodes_;
    }
    
    // 获取use语句（如果有）
    std::shared_ptr<ASTNode> getUseStatement() const {
        for (const auto& node : topLevelNodes_) {
            if (node->getType() == NodeType::USE_OP) {
                return node;
            }
        }
        return nullptr;
    }
    
    void accept(Visitor* visitor) override;
    std::string toString() const override;
    
private:
    std::string filename_;
    std::vector<std::shared_ptr<ASTNode>> topLevelNodes_;
};

// 完整的访问者接口
class CompleteVisitor : public Visitor,
                        public CommentVisitor,
                        public TemplateVisitor,
                        public CustomVisitor,
                        public StyleVisitor,
                        public ScriptVisitor,
                        public OriginVisitor,
                        public ImportVisitor,
                        public ConfigVisitor,
                        public NamespaceVisitor,
                        public OperatorVisitor {
public:
    virtual void visitProgramNode(ProgramNode* node) = 0;
};

} // namespace CHTL

#endif // CHTL_PROGRAM_NODE_H