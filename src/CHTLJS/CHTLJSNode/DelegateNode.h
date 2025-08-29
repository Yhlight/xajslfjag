#ifndef CHTLJS_DELEGATE_NODE_H
#define CHTLJS_DELEGATE_NODE_H

#include "BaseNode.h"
#include <unordered_map>
#include <variant>

namespace CHTLJS {

// delegate块节点（delegate {}）
class DelegateNode : public ASTNode {
public:
    DelegateNode(const TokenLocation& location)
        : ASTNode(NodeType::DELEGATE_BLOCK, location) {}
    
    // 设置目标选择器（单个或数组）
    void setTarget(std::shared_ptr<ASTNode> target) {
        target_ = target;
    }
    
    std::shared_ptr<ASTNode> getTarget() const { return target_; }
    
    // 添加事件处理器
    void addEventHandler(const std::string& event, std::shared_ptr<ASTNode> handler) {
        eventHandlers_[event] = handler;
    }
    
    const std::unordered_map<std::string, std::shared_ptr<ASTNode>>& getEventHandlers() const {
        return eventHandlers_;
    }
    
    std::vector<std::shared_ptr<ASTNode>> getChildren() const override {
        std::vector<std::shared_ptr<ASTNode>> children;
        if (target_) {
            children.push_back(target_);
        }
        for (const auto& [_, handler] : eventHandlers_) {
            children.push_back(handler);
        }
        return children;
    }
    
    void accept(Visitor* visitor) override;
    std::string toString() const override;
    
private:
    std::shared_ptr<ASTNode> target_;  // 目标选择器或选择器数组
    std::unordered_map<std::string, std::shared_ptr<ASTNode>> eventHandlers_;
};

// 扩展访问者接口
class DelegateVisitor : public Visitor {
public:
    virtual void visitDelegateNode(DelegateNode* node) = 0;
};

} // namespace CHTLJS

#endif // CHTLJS_DELEGATE_NODE_H