#ifndef CHTLJS_JAVASCRIPT_NODE_H
#define CHTLJS_JAVASCRIPT_NODE_H

#include "BaseNode.h"
#include <optional>

namespace CHTLJS {

// 函数声明节点
class FunctionDeclarationNode : public ASTNode {
public:
    FunctionDeclarationNode(const std::string& name, const TokenLocation& location)
        : ASTNode(NodeType::FUNCTION_DECLARATION, location), name_(name) {}
    
    const std::string& getName() const { return name_; }
    
    // 添加参数
    void addParameter(const std::string& param) {
        parameters_.push_back(param);
    }
    
    const std::vector<std::string>& getParameters() const {
        return parameters_;
    }
    
    // 设置函数体
    void setBody(std::shared_ptr<ASTNode> body) {
        body_ = body;
    }
    
    std::shared_ptr<ASTNode> getBody() const { return body_; }
    
    std::vector<std::shared_ptr<ASTNode>> getChildren() const override {
        if (body_) {
            return {body_};
        }
        return {};
    }
    
    void accept(Visitor* visitor) override;
    std::string toString() const override;
    
private:
    std::string name_;
    std::vector<std::string> parameters_;
    std::shared_ptr<ASTNode> body_;
};

// 变量声明节点
class VariableDeclarationNode : public ASTNode {
public:
    enum class DeclarationType {
        CONST,
        LET,
        VAR
    };
    
    VariableDeclarationNode(DeclarationType type, const std::string& name,
                           const TokenLocation& location)
        : ASTNode(NodeType::VARIABLE_DECLARATION, location),
          declarationType_(type), name_(name) {}
    
    DeclarationType getDeclarationType() const { return declarationType_; }
    const std::string& getName() const { return name_; }
    
    // 设置初始值
    void setInitializer(std::shared_ptr<ASTNode> initializer) {
        initializer_ = initializer;
    }
    
    std::shared_ptr<ASTNode> getInitializer() const { return initializer_; }
    
    std::vector<std::shared_ptr<ASTNode>> getChildren() const override {
        if (initializer_) {
            return {initializer_};
        }
        return {};
    }
    
    void accept(Visitor* visitor) override;
    std::string toString() const override;
    
private:
    DeclarationType declarationType_;
    std::string name_;
    std::shared_ptr<ASTNode> initializer_;
};

// 对象字面量节点
class ObjectLiteralNode : public ASTNode {
public:
    ObjectLiteralNode(const TokenLocation& location)
        : ASTNode(NodeType::OBJECT_LITERAL, location) {}
    
    // 添加属性
    void addProperty(const std::string& key, std::shared_ptr<ASTNode> value) {
        properties_.push_back({key, value});
    }
    
    const std::vector<std::pair<std::string, std::shared_ptr<ASTNode>>>& getProperties() const {
        return properties_;
    }
    
    std::vector<std::shared_ptr<ASTNode>> getChildren() const override {
        std::vector<std::shared_ptr<ASTNode>> children;
        for (const auto& [_, value] : properties_) {
            children.push_back(value);
        }
        return children;
    }
    
    void accept(Visitor* visitor) override;
    std::string toString() const override;
    
private:
    std::vector<std::pair<std::string, std::shared_ptr<ASTNode>>> properties_;
};

// 数组字面量节点
class ArrayLiteralNode : public ASTNode {
public:
    ArrayLiteralNode(const TokenLocation& location)
        : ASTNode(NodeType::ARRAY_LITERAL, location) {}
    
    // 添加元素
    void addElement(std::shared_ptr<ASTNode> element) {
        elements_.push_back(element);
    }
    
    const std::vector<std::shared_ptr<ASTNode>>& getElements() const {
        return elements_;
    }
    
    std::vector<std::shared_ptr<ASTNode>> getChildren() const override {
        return elements_;
    }
    
    void accept(Visitor* visitor) override;
    std::string toString() const override;
    
private:
    std::vector<std::shared_ptr<ASTNode>> elements_;
};

// 函数调用节点
class CallExpressionNode : public ASTNode {
public:
    CallExpressionNode(std::shared_ptr<ASTNode> callee, const TokenLocation& location)
        : ASTNode(NodeType::CALL_EXPRESSION, location), callee_(callee) {}
    
    std::shared_ptr<ASTNode> getCallee() const { return callee_; }
    
    // 添加参数
    void addArgument(std::shared_ptr<ASTNode> arg) {
        arguments_.push_back(arg);
    }
    
    const std::vector<std::shared_ptr<ASTNode>>& getArguments() const {
        return arguments_;
    }
    
    std::vector<std::shared_ptr<ASTNode>> getChildren() const override {
        std::vector<std::shared_ptr<ASTNode>> children = {callee_};
        children.insert(children.end(), arguments_.begin(), arguments_.end());
        return children;
    }
    
    void accept(Visitor* visitor) override;
    std::string toString() const override;
    
private:
    std::shared_ptr<ASTNode> callee_;
    std::vector<std::shared_ptr<ASTNode>> arguments_;
};

// 扩展访问者接口
class JavaScriptVisitor : public Visitor {
public:
    virtual void visitFunctionDeclarationNode(FunctionDeclarationNode* node) = 0;
    virtual void visitVariableDeclarationNode(VariableDeclarationNode* node) = 0;
    virtual void visitObjectLiteralNode(ObjectLiteralNode* node) = 0;
    virtual void visitArrayLiteralNode(ArrayLiteralNode* node) = 0;
    virtual void visitCallExpressionNode(CallExpressionNode* node) = 0;
};

} // namespace CHTLJS

#endif // CHTLJS_JAVASCRIPT_NODE_H