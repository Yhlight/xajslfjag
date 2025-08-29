#ifndef CHTL_NAMESPACE_NODE_H
#define CHTL_NAMESPACE_NODE_H

#include "BaseNode.h"

namespace CHTL {

// 命名空间节点
class NamespaceNode : public ASTNode {
public:
    NamespaceNode(const std::string& name, const TokenLocation& location)
        : ASTNode(NodeType::NAMESPACE, location), name_(name) {}
    
    const std::string& getName() const { return name_; }
    
    // 添加内容
    void addContent(std::shared_ptr<ASTNode> node) {
        content_.push_back(node);
    }
    
    const std::vector<std::shared_ptr<ASTNode>>& getContent() const {
        return content_;
    }
    
    // 添加except约束
    void addExcept(const std::string& constraint) {
        exceptConstraints_.push_back(constraint);
    }
    
    const std::vector<std::string>& getExceptConstraints() const {
        return exceptConstraints_;
    }
    
    std::vector<std::shared_ptr<ASTNode>> getChildren() const override {
        return content_;
    }
    
    void accept(Visitor* visitor) override;
    std::string toString() const override;
    
private:
    std::string name_;
    std::vector<std::shared_ptr<ASTNode>> content_;
    std::vector<std::string> exceptConstraints_;
};

// from语句节点（如 @Element Box from space）
class FromNode : public ASTNode {
public:
    FromNode(const std::string& item, const std::string& namespacePath,
             const TokenLocation& location)
        : ASTNode(NodeType::IDENTIFIER, location), 
          item_(item), namespacePath_(namespacePath) {}
    
    const std::string& getItem() const { return item_; }
    const std::string& getNamespacePath() const { return namespacePath_; }
    
    void accept(Visitor* visitor) override;
    std::string toString() const override;
    
private:
    std::string item_;
    std::string namespacePath_;
};

// 扩展访问者接口
class NamespaceVisitor : public Visitor {
public:
    virtual void visitNamespaceNode(NamespaceNode* node) = 0;
    virtual void visitFromNode(FromNode* node) = 0;
};

} // namespace CHTL

#endif // CHTL_NAMESPACE_NODE_H