#ifndef CHTLJS_LISTEN_NODE_H
#define CHTLJS_LISTEN_NODE_H

#include "BaseNode.h"
#include <unordered_map>

namespace CHTLJS {

// listen块节点（listen {}）
class ListenNode : public ASTNode {
public:
    ListenNode(const TokenLocation& location)
        : ASTNode(NodeType::LISTEN_BLOCK, location) {}
    
    // 添加事件处理器
    void addEventHandler(const std::string& event, std::shared_ptr<ASTNode> handler) {
        eventHandlers_[event] = handler;
    }
    
    const std::unordered_map<std::string, std::shared_ptr<ASTNode>>& getEventHandlers() const {
        return eventHandlers_;
    }
    
    std::vector<std::shared_ptr<ASTNode>> getChildren() const override {
        std::vector<std::shared_ptr<ASTNode>> children;
        for (const auto& [_, handler] : eventHandlers_) {
            children.push_back(handler);
        }
        return children;
    }
    
    void accept(Visitor* visitor) override;
    std::string toString() const override;
    
private:
    std::unordered_map<std::string, std::shared_ptr<ASTNode>> eventHandlers_;
};

// 扩展访问者接口
class ListenVisitor : public Visitor {
public:
    virtual void visitListenNode(ListenNode* node) = 0;
};

} // namespace CHTLJS

#endif // CHTLJS_LISTEN_NODE_H