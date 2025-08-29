#ifndef CHTL_BASE_NODE_H
#define CHTL_BASE_NODE_H

#include <string>
#include <memory>
#include <vector>
#include <optional>
#include "../CHTLLexer/Token.h"

namespace CHTL {

// AST节点类型
enum class NodeType {
    // 基础节点
    PROGRAM,
    ELEMENT,
    TEXT,
    COMMENT,
    ATTRIBUTE,
    
    // 模板和自定义
    TEMPLATE,
    CUSTOM,
    
    // 原始嵌入
    ORIGIN,
    
    // 导入和配置
    IMPORT,
    CONFIGURATION,
    NAMESPACE,
    
    // 样式和脚本
    STYLE_BLOCK,
    SCRIPT_BLOCK,
    
    // 操作符节点
    DELETE_OP,
    INSERT_OP,
    INHERIT_OP,
    EXCEPT_OP,
    USE_OP,
    
    // 特殊节点
    INFO,
    EXPORT,
    
    // 表达式节点
    IDENTIFIER,
    LITERAL,
    SELECTOR,
    PROPERTY,
    FUNCTION_CALL
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

// 元素节点（HTML元素）
class ElementNode : public ASTNode {
public:
    ElementNode(const std::string& tagName, const TokenLocation& location)
        : ASTNode(NodeType::ELEMENT, location), tagName_(tagName) {}
    
    // 获取标签名
    const std::string& getTagName() const { return tagName_; }
    
    // 属性管理
    void addAttribute(const std::string& name, const std::string& value) {
        attributes_[name] = value;
    }
    
    const std::unordered_map<std::string, std::string>& getAttributes() const {
        return attributes_;
    }
    
    std::optional<std::string> getAttribute(const std::string& name) const {
        auto it = attributes_.find(name);
        if (it != attributes_.end()) {
            return it->second;
        }
        return std::nullopt;
    }
    
    // 子节点管理
    void addChild(std::shared_ptr<ASTNode> child) {
        children_.push_back(child);
    }
    
    const std::vector<std::shared_ptr<ASTNode>>& getChildNodes() const {
        return children_;
    }
    
    std::vector<std::shared_ptr<ASTNode>> getChildren() const override {
        return children_;
    }
    
    // 特殊属性
    void setIndex(size_t index) { index_ = index; }
    std::optional<size_t> getIndex() const { return index_; }
    
    void accept(Visitor* visitor) override;
    std::string toString() const override;
    
private:
    std::string tagName_;
    std::unordered_map<std::string, std::string> attributes_;
    std::vector<std::shared_ptr<ASTNode>> children_;
    std::optional<size_t> index_;  // 用于索引访问 div[0]
};

// 文本节点
class TextNode : public ASTNode {
public:
    TextNode(const std::string& content, const TokenLocation& location)
        : ASTNode(NodeType::TEXT, location), content_(content) {}
    
    const std::string& getContent() const { return content_; }
    void setContent(const std::string& content) { content_ = content; }
    
    void accept(Visitor* visitor) override;
    std::string toString() const override;
    
private:
    std::string content_;
};

// 属性节点
class AttributeNode : public ASTNode {
public:
    AttributeNode(const std::string& name, const std::string& value, 
                  const TokenLocation& location)
        : ASTNode(NodeType::ATTRIBUTE, location), name_(name), value_(value) {}
    
    const std::string& getName() const { return name_; }
    const std::string& getValue() const { return value_; }
    
    void accept(Visitor* visitor) override;
    std::string toString() const override;
    
private:
    std::string name_;
    std::string value_;
};

// 访问者接口
class Visitor {
public:
    virtual ~Visitor() = default;
    
    virtual void visitElementNode(ElementNode* node) = 0;
    virtual void visitTextNode(TextNode* node) = 0;
    virtual void visitAttributeNode(AttributeNode* node) = 0;
};

} // namespace CHTL

#endif // CHTL_BASE_NODE_H