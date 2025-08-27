#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

namespace CHTLJS {

// 节点类型
enum class NodeType {
    // 基础节点
    PROGRAM,           // 程序根节点
    STATEMENT,         // 语句
    EXPRESSION,        // 表达式
    
    // CHTL JS特有节点
    ENHANCED_SELECTOR, // {{}} 增强选择器
    ARROW_ACCESS,      // -> 访问
    VIR_DECLARATION,   // vir 声明
    CHTLJS_FUNCTION,   // CHTL JS函数（listen, delegate, animate）
    EVENT_BINDING,     // &-> 事件绑定
    
    // JS标准节点
    IDENTIFIER,        // 标识符
    LITERAL,           // 字面量
    BINARY_OP,         // 二元操作
    UNARY_OP,          // 一元操作
    ASSIGNMENT,        // 赋值
    FUNCTION_CALL,     // 函数调用
    MEMBER_ACCESS,     // 成员访问
    ARRAY_ACCESS,      // 数组访问
    OBJECT_LITERAL,    // 对象字面量
    ARRAY_LITERAL,     // 数组字面量
    FUNCTION_EXPR,     // 函数表达式
    ARROW_FUNCTION,    // 箭头函数
    
    // 控制流
    IF_STATEMENT,      // if语句
    FOR_STATEMENT,     // for循环
    WHILE_STATEMENT,   // while循环
    RETURN_STATEMENT,  // return语句
    
    // 声明
    VAR_DECLARATION,   // 变量声明
    FUNCTION_DECL,     // 函数声明
};

// 前向声明
class CHTLJSNode;
using NodePtr = std::shared_ptr<CHTLJSNode>;

// 基础节点类
class CHTLJSNode {
public:
    CHTLJSNode(NodeType type) : nodeType(type), line(0), column(0) {}
    virtual ~CHTLJSNode() = default;
    
    NodeType GetType() const { return nodeType; }
    
    // 生成JavaScript代码
    virtual std::string GenerateJS() const = 0;
    
    // 克隆节点
    virtual NodePtr Clone() const = 0;
    
    // 子节点管理
    virtual void AddChild(NodePtr child) { children.push_back(child); }
    virtual NodePtr GetChild(size_t index) const;
    virtual size_t GetChildCount() const { return children.size(); }
    virtual std::vector<NodePtr>& GetChildren() { return children; }
    
    // 位置信息
    void SetPosition(int l, int c) { line = l; column = c; }
    int GetLine() const { return line; }
    int GetColumn() const { return column; }
    
protected:
    NodeType nodeType;
    std::vector<NodePtr> children;
    int line;
    int column;
};

// 增强选择器节点
class EnhancedSelectorNode : public CHTLJSNode {
public:
    EnhancedSelectorNode(const std::string& selector);
    
    std::string GetSelector() const { return selector; }
    void SetIndex(int idx) { index = idx; hasIndex = true; }
    
    std::string GenerateJS() const override;
    NodePtr Clone() const override;
    
    // 判断选择器类型
    bool IsClassSelector() const;
    bool IsIdSelector() const;
    bool IsTagSelector() const;
    bool IsComplexSelector() const;
    
private:
    std::string selector;
    int index;
    bool hasIndex;
    
    std::string GenerateSelectorJS() const;
};

// 箭头访问节点
class ArrowAccessNode : public CHTLJSNode {
public:
    ArrowAccessNode(NodePtr object, const std::string& property);
    
    NodePtr GetObject() const { return object; }
    std::string GetProperty() const { return property; }
    
    std::string GenerateJS() const override;
    NodePtr Clone() const override;
    
private:
    NodePtr object;
    std::string property;
};

// 虚对象声明节点
class VirDeclarationNode : public CHTLJSNode {
public:
    VirDeclarationNode(const std::string& name, NodePtr value);
    
    std::string GetName() const { return name; }
    NodePtr GetValue() const { return value; }
    
    std::string GenerateJS() const override;
    NodePtr Clone() const override;
    
private:
    std::string name;
    NodePtr value;
};

// CHTL JS函数节点
class CHTLJSFunctionNode : public CHTLJSNode {
public:
    CHTLJSFunctionNode(const std::string& name);
    
    std::string GetFunctionName() const { return functionName; }
    
    void AddProperty(const std::string& key, NodePtr value);
    NodePtr GetProperty(const std::string& key) const;
    bool HasProperty(const std::string& key) const;
    
    std::string GenerateJS() const override;
    NodePtr Clone() const override;
    
private:
    std::string functionName;
    std::unordered_map<std::string, NodePtr> properties;
    
    std::string GenerateListenJS() const;
    std::string GenerateDelegateJS() const;
    std::string GenerateAnimateJS() const;
};

// 事件绑定节点
class EventBindingNode : public CHTLJSNode {
public:
    EventBindingNode(NodePtr target, const std::string& event, NodePtr handler);
    
    NodePtr GetTarget() const { return target; }
    std::string GetEvent() const { return event; }
    NodePtr GetHandler() const { return handler; }
    
    std::string GenerateJS() const override;
    NodePtr Clone() const override;
    
private:
    NodePtr target;
    std::string event;
    NodePtr handler;
};

// 字面量节点
class LiteralNode : public CHTLJSNode {
public:
    enum class LiteralType {
        STRING,
        NUMBER,
        BOOLEAN,
        NULL_TYPE,
        UNDEFINED
    };
    
    LiteralNode(const std::string& value, LiteralType type);
    
    std::string GetValue() const { return value; }
    LiteralType GetLiteralType() const { return literalType; }
    
    std::string GenerateJS() const override;
    NodePtr Clone() const override;
    
private:
    std::string value;
    LiteralType literalType;
};

// 标识符节点
class IdentifierNode : public CHTLJSNode {
public:
    IdentifierNode(const std::string& name);
    
    std::string GetName() const { return name; }
    
    std::string GenerateJS() const override;
    NodePtr Clone() const override;
    
private:
    std::string name;
};

// 二元操作节点
class BinaryOpNode : public CHTLJSNode {
public:
    BinaryOpNode(const std::string& op, NodePtr left, NodePtr right);
    
    std::string GetOperator() const { return op; }
    NodePtr GetLeft() const { return left; }
    NodePtr GetRight() const { return right; }
    
    std::string GenerateJS() const override;
    NodePtr Clone() const override;
    
private:
    std::string op;
    NodePtr left;
    NodePtr right;
};

// 函数调用节点
class FunctionCallNode : public CHTLJSNode {
public:
    FunctionCallNode(NodePtr function);
    
    NodePtr GetFunction() const { return function; }
    void AddArgument(NodePtr arg) { arguments.push_back(arg); }
    std::vector<NodePtr> GetArguments() const { return arguments; }
    
    std::string GenerateJS() const override;
    NodePtr Clone() const override;
    
private:
    NodePtr function;
    std::vector<NodePtr> arguments;
};

// 对象字面量节点
class ObjectLiteralNode : public CHTLJSNode {
public:
    ObjectLiteralNode();
    
    void AddProperty(const std::string& key, NodePtr value);
    std::unordered_map<std::string, NodePtr> GetProperties() const { return properties; }
    
    std::string GenerateJS() const override;
    NodePtr Clone() const override;
    
private:
    std::unordered_map<std::string, NodePtr> properties;
    std::vector<std::string> propertyOrder; // 保持属性顺序
};

} // namespace CHTLJS