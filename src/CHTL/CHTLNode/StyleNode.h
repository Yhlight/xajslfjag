#ifndef CHTL_STYLE_NODE_H
#define CHTL_STYLE_NODE_H

#include "BaseNode.h"

namespace CHTL {

// 样式块类型
enum class StyleBlockType {
    GLOBAL,     // 全局样式块
    LOCAL       // 局部样式块
};

// 样式块节点
class StyleNode : public ASTNode {
public:
    StyleNode(StyleBlockType blockType, const TokenLocation& location)
        : ASTNode(NodeType::STYLE_BLOCK, location), blockType_(blockType) {}
    
    StyleBlockType getBlockType() const { return blockType_; }
    
    // 添加样式规则
    void addRule(std::shared_ptr<ASTNode> rule) {
        rules_.push_back(rule);
    }
    
    const std::vector<std::shared_ptr<ASTNode>>& getRules() const {
        return rules_;
    }
    
    std::vector<std::shared_ptr<ASTNode>> getChildren() const override {
        return rules_;
    }
    
    void accept(Visitor* visitor) override;
    std::string toString() const override;
    
private:
    StyleBlockType blockType_;
    std::vector<std::shared_ptr<ASTNode>> rules_;
};

// CSS选择器节点
class SelectorNode : public ASTNode {
public:
    enum class SelectorType {
        CLASS,          // .className
        ID,             // #id
        TAG,            // tagName
        PSEUDO_CLASS,   // :hover
        PSEUDO_ELEMENT, // ::before
        REFERENCE,      // &
        COMPOUND        // 复合选择器
    };
    
    SelectorNode(const std::string& selector, SelectorType type,
                 const TokenLocation& location)
        : ASTNode(NodeType::SELECTOR, location), 
          selector_(selector), selectorType_(type) {}
    
    const std::string& getSelector() const { return selector_; }
    SelectorType getSelectorType() const { return selectorType_; }
    
    // 设置选择器内容（CSS属性）
    void setContent(std::shared_ptr<ASTNode> content) {
        content_ = content;
    }
    
    std::shared_ptr<ASTNode> getContent() const { return content_; }
    
    std::vector<std::shared_ptr<ASTNode>> getChildren() const override {
        if (content_) {
            return {content_};
        }
        return {};
    }
    
    void accept(Visitor* visitor) override;
    std::string toString() const override;
    
private:
    std::string selector_;
    SelectorType selectorType_;
    std::shared_ptr<ASTNode> content_;
};

// CSS属性节点
class PropertyNode : public ASTNode {
public:
    PropertyNode(const std::string& name, const std::string& value,
                 const TokenLocation& location)
        : ASTNode(NodeType::PROPERTY, location), name_(name), value_(value) {}
    
    const std::string& getName() const { return name_; }
    const std::string& getValue() const { return value_; }
    
    // 是否使用了变量组
    void setVariableGroup(const std::string& groupName) {
        variableGroup_ = groupName;
    }
    
    const std::optional<std::string>& getVariableGroup() const {
        return variableGroup_;
    }
    
    void accept(Visitor* visitor) override;
    std::string toString() const override;
    
private:
    std::string name_;
    std::string value_;
    std::optional<std::string> variableGroup_;
};

// 扩展访问者接口
class StyleVisitor : public Visitor {
public:
    virtual void visitStyleNode(StyleNode* node) = 0;
    virtual void visitSelectorNode(SelectorNode* node) = 0;
    virtual void visitPropertyNode(PropertyNode* node) = 0;
};

} // namespace CHTL

#endif // CHTL_STYLE_NODE_H