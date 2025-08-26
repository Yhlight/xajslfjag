#ifndef CHTL_JS_NODES_H
#define CHTL_JS_NODES_H

#include "ASTNode.h"
#include "BasicNodes.h"

namespace CHTL {

// 增强选择器节点 {{}}
class EnhancedSelectorNode : public ASTNode {
public:
    enum SelectorType {
        TAG_OR_AUTO,    // {{box}} - 先判断tag，然后查找类名/id
        CLASS,          // {{.box}}
        ID,             // {{#box}}
        TAG,            // {{button}}
        DESCENDANT,     // {{.box button}}
        INDEXED         // {{button[0]}}
    };
    
private:
    SelectorType selectorType;
    std::string selector;
    int index;  // 用于索引访问
    bool hasIndex;
    
public:
    EnhancedSelectorNode(const std::string& selector, const Token& token);
    
    SelectorType getSelectorType() const { return selectorType; }
    std::string getSelector() const { return selector; }
    
    void setIndex(int idx) { index = idx; hasIndex = true; }
    int getIndex() const { return index; }
    bool getHasIndex() const { return hasIndex; }
    
    void accept(ASTVisitor* visitor) override;
    std::string toString() const override;
    std::shared_ptr<ASTNode> clone() const override;
    
private:
    void parseSelector();
};

// 监听器表达式节点
class ListenExpressionNode : public ASTNode {
private:
    std::shared_ptr<EnhancedSelectorNode> target;
    std::unordered_map<std::string, std::string> eventHandlers;  // 事件名 -> 处理器代码
    
public:
    ListenExpressionNode(const Token& token);
    
    void setTarget(std::shared_ptr<EnhancedSelectorNode> t) { target = t; }
    std::shared_ptr<EnhancedSelectorNode> getTarget() const { return target; }
    
    void addEventHandler(const std::string& event, const std::string& handler);
    std::unordered_map<std::string, std::string> getEventHandlers() const { return eventHandlers; }
    
    void accept(ASTVisitor* visitor) override;
    std::string toString() const override;
    std::shared_ptr<ASTNode> clone() const override;
};

// 事件委托节点
class DelegateExpressionNode : public ASTNode {
private:
    std::shared_ptr<EnhancedSelectorNode> parentSelector;
    std::vector<std::shared_ptr<EnhancedSelectorNode>> targetSelectors;
    std::unordered_map<std::string, std::string> eventHandlers;
    
public:
    DelegateExpressionNode(const Token& token);
    
    void setParentSelector(std::shared_ptr<EnhancedSelectorNode> parent) { parentSelector = parent; }
    std::shared_ptr<EnhancedSelectorNode> getParentSelector() const { return parentSelector; }
    
    void addTargetSelector(std::shared_ptr<EnhancedSelectorNode> target);
    std::vector<std::shared_ptr<EnhancedSelectorNode>> getTargetSelectors() const { return targetSelectors; }
    
    void addEventHandler(const std::string& event, const std::string& handler);
    std::unordered_map<std::string, std::string> getEventHandlers() const { return eventHandlers; }
    
    void accept(ASTVisitor* visitor) override;
    std::string toString() const override;
    std::shared_ptr<ASTNode> clone() const override;
};

// 动画表达式节点
class AnimateExpressionNode : public ASTNode {
public:
    struct KeyFrame {
        double at;  // 时间点 (0.0 - 1.0)
        std::unordered_map<std::string, std::string> properties;
    };
    
private:
    std::vector<std::shared_ptr<EnhancedSelectorNode>> targets;
    int duration;  // 毫秒
    std::string easing;
    std::unordered_map<std::string, std::string> beginState;
    std::vector<KeyFrame> keyFrames;
    std::unordered_map<std::string, std::string> endState;
    int loop;  // -1 表示无限循环
    std::string direction;
    int delay;  // 毫秒
    std::string callback;
    
public:
    AnimateExpressionNode(const Token& token);
    
    void addTarget(std::shared_ptr<EnhancedSelectorNode> target) { targets.push_back(target); }
    std::vector<std::shared_ptr<EnhancedSelectorNode>> getTargets() const { return targets; }
    
    void setDuration(int dur) { duration = dur; }
    int getDuration() const { return duration; }
    
    void setEasing(const std::string& e) { easing = e; }
    std::string getEasing() const { return easing; }
    
    void setBeginState(const std::unordered_map<std::string, std::string>& state) { beginState = state; }
    std::unordered_map<std::string, std::string> getBeginState() const { return beginState; }
    
    void addKeyFrame(const KeyFrame& frame) { keyFrames.push_back(frame); }
    std::vector<KeyFrame> getKeyFrames() const { return keyFrames; }
    
    void setEndState(const std::unordered_map<std::string, std::string>& state) { endState = state; }
    std::unordered_map<std::string, std::string> getEndState() const { return endState; }
    
    void setLoop(int l) { loop = l; }
    int getLoop() const { return loop; }
    
    void setDirection(const std::string& dir) { direction = dir; }
    std::string getDirection() const { return direction; }
    
    void setDelay(int d) { delay = d; }
    int getDelay() const { return delay; }
    
    void setCallback(const std::string& cb) { callback = cb; }
    std::string getCallback() const { return callback; }
    
    void accept(ASTVisitor* visitor) override;
    std::string toString() const override;
    std::shared_ptr<ASTNode> clone() const override;
};

// 虚对象声明节点
class VirtualObjectNode : public ASTNode {
private:
    std::string name;
    std::string functionName;  // 右侧的CHTL JS函数名
    std::unordered_map<std::string, std::string> functionContent;  // 函数内容
    
public:
    VirtualObjectNode(const std::string& name, const Token& token);
    
    std::string getName() const { return name; }
    
    void setFunctionName(const std::string& func) { functionName = func; }
    std::string getFunctionName() const { return functionName; }
    
    void addFunctionContent(const std::string& key, const std::string& content);
    std::unordered_map<std::string, std::string> getFunctionContent() const { return functionContent; }
    
    void accept(ASTVisitor* visitor) override;
    std::string toString() const override;
    std::shared_ptr<ASTNode> clone() const override;
};

// 虚对象访问节点
class VirtualObjectAccessNode : public ASTNode {
private:
    std::string objectName;
    std::string memberName;
    bool useArrow;  // 使用 -> 还是 .
    
public:
    VirtualObjectAccessNode(const std::string& object, const std::string& member, const Token& token);
    
    std::string getObjectName() const { return objectName; }
    std::string getMemberName() const { return memberName; }
    
    void setUseArrow(bool arrow) { useArrow = arrow; }
    bool getUseArrow() const { return useArrow; }
    
    void accept(ASTVisitor* visitor) override;
    std::string toString() const override;
    std::shared_ptr<ASTNode> clone() const override;
};

// JS语句节点（通用）
class JSStatementNode : public ASTNode {
private:
    std::string statement;
    
public:
    JSStatementNode(const std::string& stmt, const Token& token);
    
    std::string getStatement() const { return statement; }
    
    void accept(ASTVisitor* visitor) override;
    std::string toString() const override;
    std::shared_ptr<ASTNode> clone() const override;
};

// 函数调用节点
class FunctionCallNode : public ASTNode {
private:
    std::string functionName;
    std::vector<std::string> arguments;
    
public:
    FunctionCallNode(const std::string& name, const Token& token);
    
    std::string getFunctionName() const { return functionName; }
    
    void addArgument(const std::string& arg) { arguments.push_back(arg); }
    std::vector<std::string> getArguments() const { return arguments; }
    
    void accept(ASTVisitor* visitor) override;
    std::string toString() const override;
    std::shared_ptr<ASTNode> clone() const override;
};

// 选择器表达式节点（用于 & 引用）
class SelectorExpressionNode : public ASTNode {
private:
    std::string selector;
    bool isReference;  // 是否为 & 引用
    
public:
    SelectorExpressionNode(const std::string& sel, bool ref, const Token& token);
    
    std::string getSelector() const { return selector; }
    bool getIsReference() const { return isReference; }
    
    void accept(ASTVisitor* visitor) override;
    std::string toString() const override;
    std::shared_ptr<ASTNode> clone() const override;
};

} // namespace CHTL

#endif // CHTL_JS_NODES_H