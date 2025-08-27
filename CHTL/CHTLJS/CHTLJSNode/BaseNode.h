#ifndef CHTLJS_BASE_NODE_H
#define CHTLJS_BASE_NODE_H

#include <string>
#include <vector>
#include <memory>

namespace CHTLJS {

// CHTL JS 节点类型枚举
enum class NodeType {
    // 表达式
    LITERAL,              // 字面量
    IDENTIFIER,           // 标识符
    BINARY_EXPRESSION,    // 二元表达式
    UNARY_EXPRESSION,     // 一元表达式
    ASSIGNMENT,           // 赋值表达式
    CALL_EXPRESSION,      // 函数调用
    MEMBER_EXPRESSION,    // 成员访问
    ARRAY_EXPRESSION,     // 数组表达式
    OBJECT_EXPRESSION,    // 对象表达式
    
    // 语句
    BLOCK_STATEMENT,      // 块语句
    EXPRESSION_STATEMENT, // 表达式语句
    IF_STATEMENT,         // if语句
    FOR_STATEMENT,        // for循环
    WHILE_STATEMENT,      // while循环
    DO_WHILE_STATEMENT,   // do-while循环
    RETURN_STATEMENT,     // return语句
    BREAK_STATEMENT,      // break语句
    CONTINUE_STATEMENT,   // continue语句
    
    // 声明
    VARIABLE_DECLARATION, // 变量声明
    FUNCTION_DECLARATION, // 函数声明
    
    // CHTL JS 特殊节点
    VIR_DECLARATION,      // vir声明
    SELECTOR_EXPRESSION,  // {{}}选择器
    LISTEN_EXPRESSION,    // listen表达式
    ANIMATE_EXPRESSION,   // animate表达式
    DELEGATE_EXPRESSION,  // delegate表达式
    ARROW_ACCESS,         // ->访问
    BIND_EXPRESSION,      // &->绑定
    
    // 程序根节点
    PROGRAM              // 程序节点
};

// 前向声明
class NodeVisitor;

// 基础节点类
class BaseNode {
protected:
    NodeType type;
    int line;
    int column;
    
public:
    BaseNode(NodeType t, int l = 0, int c = 0) 
        : type(t), line(l), column(c) {}
    
    virtual ~BaseNode() = default;
    
    // 基本属性访问
    NodeType getType() const { return type; }
    int getLine() const { return line; }
    int getColumn() const { return column; }
    
    // 访问者模式
    virtual void accept(NodeVisitor* visitor) = 0;
    
    // 克隆节点
    virtual std::shared_ptr<BaseNode> clone() const = 0;
    
    // 生成JavaScript代码
    virtual std::string toJavaScript() const = 0;
};

// 程序节点
class ProgramNode : public BaseNode {
private:
    std::vector<std::shared_ptr<BaseNode>> statements;
    
public:
    ProgramNode(int line = 0, int col = 0)
        : BaseNode(NodeType::PROGRAM, line, col) {}
    
    // 语句管理
    void addStatement(std::shared_ptr<BaseNode> stmt);
    const std::vector<std::shared_ptr<BaseNode>>& getStatements() const { return statements; }
    
    // 重写虚函数
    void accept(NodeVisitor* visitor) override;
    std::shared_ptr<BaseNode> clone() const override;
    std::string toJavaScript() const override;
};

// 访问者接口
class NodeVisitor {
public:
    virtual ~NodeVisitor() = default;
    
    virtual void visitProgramNode(ProgramNode* node) = 0;
    // 其他节点类型的访问方法将在各自的文件中定义
};

} // namespace CHTLJS

#endif // CHTLJS_BASE_NODE_H