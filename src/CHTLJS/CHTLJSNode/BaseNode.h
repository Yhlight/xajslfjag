#ifndef CHTLJS_BASE_NODE_H
#define CHTLJS_BASE_NODE_H

#include <string>
#include <memory>
#include <vector>
#include <optional>
#include "../CHTLJSLexer/Token.h"

namespace CHTLJS {

// AST节点类型
enum class NodeType {
    // 基础节点
    PROGRAM,
    STATEMENT,
    EXPRESSION,
    IDENTIFIER,
    LITERAL,
    
    // CHTL JS特有节点
    MODULE_BLOCK,
    ENHANCED_SELECTOR,
    LISTEN_BLOCK,
    DELEGATE_BLOCK,
    ANIMATE_BLOCK,
    INEVERAWAY_BLOCK,
    VIRTUAL_OBJECT,
    
    // 动画相关节点
    ANIMATE_BEGIN,
    ANIMATE_WHEN,
    ANIMATE_END,
    
    // JavaScript节点
    FUNCTION_DECLARATION,
    VARIABLE_DECLARATION,
    OBJECT_LITERAL,
    ARRAY_LITERAL,
    PROPERTY,
    
    // 操作符节点
    ARROW_ACCESS,
    EVENT_BINDING,
    BINARY_EXPRESSION,
    UNARY_EXPRESSION,
    CALL_EXPRESSION,
    MEMBER_EXPRESSION,
    
    // 控制流节点
    IF_STATEMENT,
    FOR_STATEMENT,
    WHILE_STATEMENT,
    RETURN_STATEMENT,
    
    // 其他
    COMMENT
};

// 前向声明
class Visitor;

// AST基础节点
class ASTNode {
public:
    ASTNode(NodeType type, const TokenLocation& location)
        : type_(type), location_(location) {}
    
    virtual ~ASTNode() = default;
    
    // 获取节点类型
    NodeType getType() const { return type_; }
    
    // 获取位置信息
    const TokenLocation& getLocation() const { return location_; }
    
    // 访问者模式
    virtual void accept(Visitor* visitor) = 0;
    
    // 获取子节点
    virtual std::vector<std::shared_ptr<ASTNode>> getChildren() const {
        return {};
    }
    
    // 调试输出
    virtual std::string toString() const = 0;
    
protected:
    NodeType type_;
    TokenLocation location_;
};

// 标识符节点
class IdentifierNode : public ASTNode {
public:
    IdentifierNode(const std::string& name, const TokenLocation& location)
        : ASTNode(NodeType::IDENTIFIER, location), name_(name) {}
    
    const std::string& getName() const { return name_; }
    
    void accept(Visitor* visitor) override;
    std::string toString() const override;
    
private:
    std::string name_;
};

// 字面量节点
class LiteralNode : public ASTNode {
public:
    enum class LiteralType {
        STRING,
        NUMBER,
        BOOLEAN,
        NULL_VALUE,
        UNQUOTED
    };
    
    LiteralNode(LiteralType literalType, const TokenValue& value,
                const TokenLocation& location)
        : ASTNode(NodeType::LITERAL, location), 
          literalType_(literalType), value_(value) {}
    
    LiteralType getLiteralType() const { return literalType_; }
    const TokenValue& getValue() const { return value_; }
    
    void accept(Visitor* visitor) override;
    std::string toString() const override;
    
private:
    LiteralType literalType_;
    TokenValue value_;
};

// 访问者接口
class Visitor {
public:
    virtual ~Visitor() = default;
    
    virtual void visitIdentifierNode(IdentifierNode* node) = 0;
    virtual void visitLiteralNode(LiteralNode* node) = 0;
};

} // namespace CHTLJS

#endif // CHTLJS_BASE_NODE_H