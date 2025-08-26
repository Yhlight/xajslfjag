#ifndef CHTL_ELEMENT_NODES_H
#define CHTL_ELEMENT_NODES_H

#include "Node.h"
#include "BasicNodes.h"

namespace CHTL {

// 属性节点
class AttributeNode : public Node {
private:
    std::string name;
    std::shared_ptr<LiteralNode> value;
    bool useColon;  // 使用 : 还是 = (CE对等式)
    
public:
    AttributeNode(const std::string& name, const Token& token);
    
    void setName(const std::string& n) { name = n; }
    std::string getName() const { return name; }
    
    void setValue(std::shared_ptr<LiteralNode> v) { value = v; }
    std::shared_ptr<LiteralNode> getValue() const { return value; }
    
    void setUseColon(bool colon) { useColon = colon; }
    bool getUseColon() const { return useColon; }
    
    void accept(Visitor* visitor) override;
    std::string toString() const override;
    std::shared_ptr<Node> clone() const override;
};

// 元素节点
class ElementNode : public Node {
private:
    std::string tagName;
    std::vector<std::shared_ptr<AttributeNode>> attributes;
    std::shared_ptr<BlockNode> localStyleBlock;
    std::shared_ptr<BlockNode> localScriptBlock;
    bool isSelfClosing;
    
public:
    ElementNode(const std::string& tagName, const Token& token);
    
    void setTagName(const std::string& name) { tagName = name; }
    std::string getTagName() const { return tagName; }
    
    void addAttribute(std::shared_ptr<AttributeNode> attr);
    std::vector<std::shared_ptr<AttributeNode>> getAttributes() const { return attributes; }
    std::shared_ptr<AttributeNode> getAttribute(const std::string& name) const;
    bool hasAttribute(const std::string& name) const;
    
    void setLocalStyleBlock(std::shared_ptr<BlockNode> block) { localStyleBlock = block; }
    std::shared_ptr<BlockNode> getLocalStyleBlock() const { return localStyleBlock; }
    bool hasLocalStyle() const { return localStyleBlock != nullptr; }
    
    void setLocalScriptBlock(std::shared_ptr<BlockNode> block) { localScriptBlock = block; }
    std::shared_ptr<BlockNode> getLocalScriptBlock() const { return localScriptBlock; }
    bool hasLocalScript() const { return localScriptBlock != nullptr; }
    
    void setIsSelfClosing(bool selfClosing) { isSelfClosing = selfClosing; }
    bool getIsSelfClosing() const { return isSelfClosing; }
    
    // 获取class和id属性的便捷方法
    std::string getClassAttribute() const;
    std::string getIdAttribute() const;
    std::vector<std::string> getClassList() const;
    
    void accept(Visitor* visitor) override;
    std::string toString() const override;
    std::shared_ptr<Node> clone() const override;
};

// 样式块节点
class StyleBlockNode : public Node {
private:
    bool isGlobal;  // 是否为全局样式块
    std::string content;  // 原始CSS内容（用于内联样式）
    
public:
    StyleBlockNode(bool global, const Token& token);
    
    void setIsGlobal(bool global) { isGlobal = global; }
    bool getIsGlobal() const { return isGlobal; }
    
    void setContent(const std::string& css) { content = css; }
    std::string getContent() const { return content; }
    
    void accept(Visitor* visitor) override;
    std::string toString() const override;
    std::shared_ptr<Node> clone() const override;
};

// CSS规则节点
class CSSRuleNode : public Node {
private:
    std::string selector;
    std::unordered_map<std::string, std::string> properties;
    bool isInlineStyle;  // 是否为内联样式
    
public:
    CSSRuleNode(const std::string& selector, const Token& token);
    
    void setSelector(const std::string& sel) { selector = sel; }
    std::string getSelector() const { return selector; }
    
    void addProperty(const std::string& name, const std::string& value);
    std::unordered_map<std::string, std::string> getProperties() const { return properties; }
    std::string getProperty(const std::string& name) const;
    bool hasProperty(const std::string& name) const;
    
    void setIsInlineStyle(bool inline_) { isInlineStyle = inline_; }
    bool getIsInlineStyle() const { return isInlineStyle; }
    
    void accept(Visitor* visitor) override;
    std::string toString() const override;
    std::shared_ptr<Node> clone() const override;
};

// 脚本块节点
class ScriptBlockNode : public Node {
private:
    bool isGlobal;  // 是否为全局脚本块
    std::string content;  // 原始JS内容
    
public:
    ScriptBlockNode(bool global, const Token& token);
    
    void setIsGlobal(bool global) { isGlobal = global; }
    bool getIsGlobal() const { return isGlobal; }
    
    void setContent(const std::string& js) { content = js; }
    std::string getContent() const { return content; }
    
    void accept(Visitor* visitor) override;
    std::string toString() const override;
    std::shared_ptr<Node> clone() const override;
};

// 属性节点（用于style和script块内的属性）
class PropertyNode : public Node {
private:
    std::string name;
    std::string value;
    bool hasValue;  // 对于自定义样式组，可能没有值
    
public:
    PropertyNode(const std::string& name, const Token& token);
    
    void setName(const std::string& n) { name = n; }
    std::string getName() const { return name; }
    
    void setValue(const std::string& v) { value = v; hasValue = true; }
    std::string getValue() const { return value; }
    bool getHasValue() const { return hasValue; }
    
    void accept(Visitor* visitor) override;
    std::string toString() const override;
    std::shared_ptr<Node> clone() const override;
};

} // namespace CHTL

#endif // CHTL_ELEMENT_NODES_H