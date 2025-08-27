#ifndef CHTLJS_EXPRESSION_NODES_H
#define CHTLJS_EXPRESSION_NODES_H

#include "BaseNode.h"
#include <unordered_map>

namespace CHTLJS {

// 字面量节点
class LiteralNode : public BaseNode {
private:
    std::string value;
    std::string literalType; // "string", "number", "boolean", "null"
    
public:
    LiteralNode(const std::string& val, const std::string& type, int line = 0, int col = 0)
        : BaseNode(NodeType::LITERAL, line, col), value(val), literalType(type) {}
    
    const std::string& getValue() const { return value; }
    const std::string& getLiteralType() const { return literalType; }
    
    void accept(NodeVisitor* visitor) override;
    std::shared_ptr<BaseNode> clone() const override;
    std::string toJavaScript() const override;
};

// 标识符节点
class IdentifierNode : public BaseNode {
private:
    std::string name;
    
public:
    IdentifierNode(const std::string& n, int line = 0, int col = 0)
        : BaseNode(NodeType::IDENTIFIER, line, col), name(n) {}
    
    const std::string& getName() const { return name; }
    
    void accept(NodeVisitor* visitor) override;
    std::shared_ptr<BaseNode> clone() const override;
    std::string toJavaScript() const override;
};

// CHTL选择器节点
class SelectorNode : public BaseNode {
private:
    std::string selector;     // 选择器内容（不包括{{}}）
    std::string selectorType; // "element", "class", "id", "complex"
    int index;                // 索引访问，-1表示不使用
    
public:
    SelectorNode(const std::string& sel, const std::string& type, int idx = -1, int line = 0, int col = 0)
        : BaseNode(NodeType::SELECTOR_EXPRESSION, line, col), 
          selector(sel), selectorType(type), index(idx) {}
    
    const std::string& getSelector() const { return selector; }
    const std::string& getSelectorType() const { return selectorType; }
    int getIndex() const { return index; }
    
    void accept(NodeVisitor* visitor) override;
    std::shared_ptr<BaseNode> clone() const override;
    std::string toJavaScript() const override;
};

// 二元表达式节点
class BinaryExpressionNode : public BaseNode {
private:
    std::string op;
    std::shared_ptr<BaseNode> left;
    std::shared_ptr<BaseNode> right;
    
public:
    BinaryExpressionNode(const std::string& oper, std::shared_ptr<BaseNode> l, 
                        std::shared_ptr<BaseNode> r, int line = 0, int col = 0)
        : BaseNode(NodeType::BINARY_EXPRESSION, line, col), op(oper), left(l), right(r) {}
    
    const std::string& getOperator() const { return op; }
    std::shared_ptr<BaseNode> getLeft() const { return left; }
    std::shared_ptr<BaseNode> getRight() const { return right; }
    
    void accept(NodeVisitor* visitor) override;
    std::shared_ptr<BaseNode> clone() const override;
    std::string toJavaScript() const override;
};

// 成员访问节点（包括 . 和 -> 访问）
class MemberExpressionNode : public BaseNode {
private:
    std::shared_ptr<BaseNode> object;
    std::shared_ptr<BaseNode> property;
    bool isArrowAccess;  // true表示使用->，false表示使用.
    bool isComputed;     // true表示使用[]访问
    
public:
    MemberExpressionNode(std::shared_ptr<BaseNode> obj, std::shared_ptr<BaseNode> prop,
                        bool arrow = false, bool computed = false, int line = 0, int col = 0)
        : BaseNode(NodeType::MEMBER_EXPRESSION, line, col), 
          object(obj), property(prop), isArrowAccess(arrow), isComputed(computed) {}
    
    std::shared_ptr<BaseNode> getObject() const { return object; }
    std::shared_ptr<BaseNode> getProperty() const { return property; }
    bool getIsArrowAccess() const { return isArrowAccess; }
    bool getIsComputed() const { return isComputed; }
    
    void accept(NodeVisitor* visitor) override;
    std::shared_ptr<BaseNode> clone() const override;
    std::string toJavaScript() const override;
};

// 函数调用节点
class CallExpressionNode : public BaseNode {
private:
    std::shared_ptr<BaseNode> callee;
    std::vector<std::shared_ptr<BaseNode>> arguments;
    
public:
    CallExpressionNode(std::shared_ptr<BaseNode> func, int line = 0, int col = 0)
        : BaseNode(NodeType::CALL_EXPRESSION, line, col), callee(func) {}
    
    std::shared_ptr<BaseNode> getCallee() const { return callee; }
    void addArgument(std::shared_ptr<BaseNode> arg) { arguments.push_back(arg); }
    const std::vector<std::shared_ptr<BaseNode>>& getArguments() const { return arguments; }
    
    void accept(NodeVisitor* visitor) override;
    std::shared_ptr<BaseNode> clone() const override;
    std::string toJavaScript() const override;
};

// CHTL JS Listen表达式节点
class ListenExpressionNode : public BaseNode {
private:
    std::unordered_map<std::string, std::shared_ptr<BaseNode>> events;
    
public:
    ListenExpressionNode(int line = 0, int col = 0)
        : BaseNode(NodeType::LISTEN_EXPRESSION, line, col) {}
    
    void addEvent(const std::string& event, std::shared_ptr<BaseNode> handler) {
        events[event] = handler;
    }
    const std::unordered_map<std::string, std::shared_ptr<BaseNode>>& getEvents() const { return events; }
    
    void accept(NodeVisitor* visitor) override;
    std::shared_ptr<BaseNode> clone() const override;
    std::string toJavaScript() const override;
};

// CHTL JS Animate表达式节点
class AnimateExpressionNode : public BaseNode {
private:
    std::shared_ptr<BaseNode> target;
    int duration;
    std::string easing;
    std::unordered_map<std::string, std::string> beginState;
    std::unordered_map<std::string, std::string> endState;
    std::vector<std::pair<double, std::unordered_map<std::string, std::string>>> whenStates;
    int loop;
    std::string direction;
    int delay;
    std::shared_ptr<BaseNode> callback;
    
public:
    AnimateExpressionNode(int line = 0, int col = 0)
        : BaseNode(NodeType::ANIMATE_EXPRESSION, line, col), 
          duration(0), loop(1), delay(0) {}
    
    // Setter方法
    void setTarget(std::shared_ptr<BaseNode> t) { target = t; }
    void setDuration(int d) { duration = d; }
    void setEasing(const std::string& e) { easing = e; }
    void setLoop(int l) { loop = l; }
    void setDirection(const std::string& d) { direction = d; }
    void setDelay(int d) { delay = d; }
    void setCallback(std::shared_ptr<BaseNode> c) { callback = c; }
    
    // 状态管理
    void addBeginProperty(const std::string& prop, const std::string& value) {
        beginState[prop] = value;
    }
    void addEndProperty(const std::string& prop, const std::string& value) {
        endState[prop] = value;
    }
    void addWhenState(double at, const std::unordered_map<std::string, std::string>& state) {
        whenStates.push_back({at, state});
    }
    
    // Getter方法
    std::shared_ptr<BaseNode> getTarget() const { return target; }
    int getDuration() const { return duration; }
    const std::string& getEasing() const { return easing; }
    
    void accept(NodeVisitor* visitor) override;
    std::shared_ptr<BaseNode> clone() const override;
    std::string toJavaScript() const override;
};

// 绑定表达式节点 (&->)
class BindExpressionNode : public BaseNode {
private:
    std::shared_ptr<BaseNode> target;
    std::string eventName;
    std::shared_ptr<BaseNode> handler;
    
public:
    BindExpressionNode(std::shared_ptr<BaseNode> t, const std::string& event, 
                      std::shared_ptr<BaseNode> h, int line = 0, int col = 0)
        : BaseNode(NodeType::BIND_EXPRESSION, line, col), 
          target(t), eventName(event), handler(h) {}
    
    std::shared_ptr<BaseNode> getTarget() const { return target; }
    const std::string& getEventName() const { return eventName; }
    std::shared_ptr<BaseNode> getHandler() const { return handler; }
    
    void accept(NodeVisitor* visitor) override;
    std::shared_ptr<BaseNode> clone() const override;
    std::string toJavaScript() const override;
};

// 扩展访问者接口
class ExpressionNodeVisitor : public NodeVisitor {
public:
    virtual void visitLiteralNode(LiteralNode* node) = 0;
    virtual void visitIdentifierNode(IdentifierNode* node) = 0;
    virtual void visitSelectorNode(SelectorNode* node) = 0;
    virtual void visitBinaryExpressionNode(BinaryExpressionNode* node) = 0;
    virtual void visitMemberExpressionNode(MemberExpressionNode* node) = 0;
    virtual void visitCallExpressionNode(CallExpressionNode* node) = 0;
    virtual void visitListenExpressionNode(ListenExpressionNode* node) = 0;
    virtual void visitAnimateExpressionNode(AnimateExpressionNode* node) = 0;
    virtual void visitBindExpressionNode(BindExpressionNode* node) = 0;
};

} // namespace CHTLJS

#endif // CHTLJS_EXPRESSION_NODES_H