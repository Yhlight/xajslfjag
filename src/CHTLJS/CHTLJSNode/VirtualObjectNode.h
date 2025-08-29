#ifndef CHTLJS_VIRTUAL_OBJECT_NODE_H
#define CHTLJS_VIRTUAL_OBJECT_NODE_H

#include "BaseNode.h"

namespace CHTLJS {

// 虚对象节点（vir name = ...）
class VirtualObjectNode : public ASTNode {
public:
    VirtualObjectNode(const std::string& name, const TokenLocation& location)
        : ASTNode(NodeType::VIRTUAL_OBJECT, location), name_(name) {}
    
    const std::string& getName() const { return name_; }
    
    // 设置关联的CHTL JS函数
    void setAssociatedFunction(std::shared_ptr<ASTNode> function) {
        associatedFunction_ = function;
    }
    
    std::shared_ptr<ASTNode> getAssociatedFunction() const {
        return associatedFunction_;
    }
    
    std::vector<std::shared_ptr<ASTNode>> getChildren() const override {
        if (associatedFunction_) {
            return {associatedFunction_};
        }
        return {};
    }
    
    void accept(Visitor* visitor) override;
    std::string toString() const override;
    
private:
    std::string name_;
    std::shared_ptr<ASTNode> associatedFunction_;
};

// iNeverAway节点
class INeverAwayNode : public ASTNode {
public:
    INeverAwayNode(const TokenLocation& location)
        : ASTNode(NodeType::INEVERAWAY_BLOCK, location) {}
    
    // 添加键定义
    void addKeyDefinition(const std::string& key, const std::string& type,
                         std::shared_ptr<ASTNode> value) {
        KeyDefinition def{key, type, value};
        keyDefinitions_.push_back(def);
    }
    
    struct KeyDefinition {
        std::string key;
        std::string type;  // Void<A>, Void<B>, etc.
        std::shared_ptr<ASTNode> value;
    };
    
    const std::vector<KeyDefinition>& getKeyDefinitions() const {
        return keyDefinitions_;
    }
    
    std::vector<std::shared_ptr<ASTNode>> getChildren() const override {
        std::vector<std::shared_ptr<ASTNode>> children;
        for (const auto& def : keyDefinitions_) {
            if (def.value) {
                children.push_back(def.value);
            }
        }
        return children;
    }
    
    void accept(Visitor* visitor) override;
    std::string toString() const override;
    
private:
    std::vector<KeyDefinition> keyDefinitions_;
};

// 扩展访问者接口
class VirtualObjectVisitor : public Visitor {
public:
    virtual void visitVirtualObjectNode(VirtualObjectNode* node) = 0;
    virtual void visitINeverAwayNode(INeverAwayNode* node) = 0;
};

} // namespace CHTLJS

#endif // CHTLJS_VIRTUAL_OBJECT_NODE_H