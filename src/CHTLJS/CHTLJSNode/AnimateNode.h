#ifndef CHTLJS_ANIMATE_NODE_H
#define CHTLJS_ANIMATE_NODE_H

#include "BaseNode.h"
#include <unordered_map>
#include <optional>

namespace CHTLJS {

// animate块节点（animate {}）
class AnimateNode : public ASTNode {
public:
    AnimateNode(const TokenLocation& location)
        : ASTNode(NodeType::ANIMATE_BLOCK, location) {}
    
    // 设置目标
    void setTarget(std::shared_ptr<ASTNode> target) {
        target_ = target;
    }
    
    std::shared_ptr<ASTNode> getTarget() const { return target_; }
    
    // 设置动画属性
    void setProperty(const std::string& name, std::shared_ptr<ASTNode> value) {
        properties_[name] = value;
    }
    
    const std::unordered_map<std::string, std::shared_ptr<ASTNode>>& getProperties() const {
        return properties_;
    }
    
    // 设置begin块
    void setBegin(std::shared_ptr<ASTNode> begin) {
        begin_ = begin;
    }
    
    std::shared_ptr<ASTNode> getBegin() const { return begin_; }
    
    // 设置when数组
    void addWhen(std::shared_ptr<ASTNode> when) {
        whenStates_.push_back(when);
    }
    
    const std::vector<std::shared_ptr<ASTNode>>& getWhenStates() const {
        return whenStates_;
    }
    
    // 设置end块
    void setEnd(std::shared_ptr<ASTNode> end) {
        end_ = end;
    }
    
    std::shared_ptr<ASTNode> getEnd() const { return end_; }
    
    std::vector<std::shared_ptr<ASTNode>> getChildren() const override {
        std::vector<std::shared_ptr<ASTNode>> children;
        if (target_) children.push_back(target_);
        if (begin_) children.push_back(begin_);
        children.insert(children.end(), whenStates_.begin(), whenStates_.end());
        if (end_) children.push_back(end_);
        return children;
    }
    
    void accept(Visitor* visitor) override;
    std::string toString() const override;
    
private:
    std::shared_ptr<ASTNode> target_;
    std::unordered_map<std::string, std::shared_ptr<ASTNode>> properties_;
    std::shared_ptr<ASTNode> begin_;
    std::vector<std::shared_ptr<ASTNode>> whenStates_;
    std::shared_ptr<ASTNode> end_;
};

// 动画状态节点（begin/when/end块）
class AnimateStateNode : public ASTNode {
public:
    enum class StateType {
        BEGIN,
        WHEN,
        END
    };
    
    AnimateStateNode(StateType stateType, const TokenLocation& location)
        : ASTNode(stateType == StateType::BEGIN ? NodeType::ANIMATE_BEGIN :
                  stateType == StateType::WHEN ? NodeType::ANIMATE_WHEN :
                  NodeType::ANIMATE_END, location),
          stateType_(stateType) {}
    
    StateType getStateType() const { return stateType_; }
    
    // 设置at值（仅用于when）
    void setAt(double at) {
        at_ = at;
    }
    
    std::optional<double> getAt() const { return at_; }
    
    // 添加CSS属性
    void addProperty(const std::string& name, const std::string& value) {
        properties_[name] = value;
    }
    
    const std::unordered_map<std::string, std::string>& getProperties() const {
        return properties_;
    }
    
    void accept(Visitor* visitor) override;
    std::string toString() const override;
    
private:
    StateType stateType_;
    std::optional<double> at_;  // 仅用于when状态
    std::unordered_map<std::string, std::string> properties_;
};

// 扩展访问者接口
class AnimateVisitor : public Visitor {
public:
    virtual void visitAnimateNode(AnimateNode* node) = 0;
    virtual void visitAnimateStateNode(AnimateStateNode* node) = 0;
};

} // namespace CHTLJS

#endif // CHTLJS_ANIMATE_NODE_H