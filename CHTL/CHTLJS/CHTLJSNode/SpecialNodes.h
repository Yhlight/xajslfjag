#ifndef CHTLJS_SPECIAL_NODES_H
#define CHTLJS_SPECIAL_NODES_H

#include "BaseNode.h"
#include <unordered_map>
#include <string>

namespace CHTLJS {

// 虚对象声明节点 (vir)
class VirDeclarationNode : public BaseNode {
private:
    std::string name;
    std::shared_ptr<BaseNode> initializer;  // 可以是iNeverAway或其他表达式
    
public:
    VirDeclarationNode(const std::string& n, std::shared_ptr<BaseNode> init, 
                      int line = 0, int col = 0)
        : BaseNode(NodeType::VIR_DECLARATION, line, col), name(n), initializer(init) {}
    
    const std::string& getName() const { return name; }
    std::shared_ptr<BaseNode> getInitializer() const { return initializer; }
    
    void accept(NodeVisitor* visitor) override;
    std::shared_ptr<BaseNode> clone() const override;
    std::string toJavaScript() const override;
};

// iNeverAway表达式节点
class INeverAwayNode : public BaseNode {
public:
    struct FunctionDef {
        std::string name;
        std::string state;  // 状态标记，如<A>, <B>等
        std::vector<std::string> paramTypes;  // 参数类型（用于文档）
        std::shared_ptr<BaseNode> body;
        
        FunctionDef(const std::string& n, const std::string& s = "")
            : name(n), state(s) {}
    };
    
private:
    std::vector<FunctionDef> functions;
    std::string mangledPrefix;  // 编译器生成的唯一前缀
    
public:
    INeverAwayNode(int line = 0, int col = 0)
        : BaseNode(NodeType::INEVERAWAY_EXPRESSION, line, col) {
        // 生成唯一前缀
        static int counter = 0;
        mangledPrefix = "_ina_" + std::to_string(counter++);
    }
    
    void addFunction(const FunctionDef& func) { functions.push_back(func); }
    const std::vector<FunctionDef>& getFunctions() const { return functions; }
    const std::string& getMangledPrefix() const { return mangledPrefix; }
    
    // 生成函数的唯一名称
    std::string getMangledName(const std::string& funcName, const std::string& state) const {
        if (state.empty()) {
            return mangledPrefix + "_" + funcName;
        }
        // 移除<>并转换为合法的JS标识符
        std::string cleanState = state;
        if (!cleanState.empty() && cleanState[0] == '<') {
            cleanState = cleanState.substr(1, cleanState.length() - 2);
        }
        return mangledPrefix + "_" + funcName + "_" + cleanState;
    }
    
    void accept(NodeVisitor* visitor) override;
    std::shared_ptr<BaseNode> clone() const override;
    std::string toJavaScript() const override;
};

// 委托表达式节点 (delegate)
class DelegateExpressionNode : public BaseNode {
private:
    std::shared_ptr<BaseNode> parent;  // 父元素选择器
    std::vector<std::shared_ptr<BaseNode>> targets;  // 目标选择器列表
    std::unordered_map<std::string, std::shared_ptr<BaseNode>> handlers;  // 事件处理器
    
public:
    DelegateExpressionNode(std::shared_ptr<BaseNode> p, int line = 0, int col = 0)
        : BaseNode(NodeType::DELEGATE_EXPRESSION, line, col), parent(p) {}
    
    std::shared_ptr<BaseNode> getParent() const { return parent; }
    void addTarget(std::shared_ptr<BaseNode> target) { targets.push_back(target); }
    const std::vector<std::shared_ptr<BaseNode>>& getTargets() const { return targets; }
    
    void addHandler(const std::string& event, std::shared_ptr<BaseNode> handler) {
        handlers[event] = handler;
    }
    const std::unordered_map<std::string, std::shared_ptr<BaseNode>>& getHandlers() const {
        return handlers;
    }
    
    void accept(NodeVisitor* visitor) override;
    std::shared_ptr<BaseNode> clone() const override;
    std::string toJavaScript() const override;
};

// 扩展访问者接口
class SpecialNodeVisitor : public NodeVisitor {
public:
    virtual void visitVirDeclarationNode(VirDeclarationNode* node) = 0;
    virtual void visitINeverAwayNode(INeverAwayNode* node) = 0;
    virtual void visitDelegateExpressionNode(DelegateExpressionNode* node) = 0;
};

} // namespace CHTLJS

#endif // CHTLJS_SPECIAL_NODES_H